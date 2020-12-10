#include "atmel_start.h"
#include "usb_start.h"
#include "hal_gpio.h"

#include <stdlib.h>
#include <string.h>

#if CONF_USBD_HS_SP
static uint8_t single_desc_bytes[] = {
    /* Device descriptors and Configuration descriptors list. */
    CDCD_ACM_HS_DESCES_LS_FS};
static uint8_t single_desc_bytes_hs[] = {
    /* Device descriptors and Configuration descriptors list. */
    CDCD_ACM_HS_DESCES_HS};
#define CDCD_BUF_SIZ CONF_USB_CDCD_ACM_DATA_BULKIN_MAXPKSZ_HS
#else
static uint8_t single_desc_bytes[] = {
    /* Device descriptors and Configuration descriptors list. */
    CDCD_ACM_DESCES_LS_FS};
#define CDCD_BUF_SIZ CONF_USB_CDCD_ACM_DATA_BULKIN_MAXPKSZ
#endif

static struct usbd_descriptors single_desc[]
    = {{single_desc_bytes, single_desc_bytes + sizeof(single_desc_bytes)}
#if CONF_USBD_HS_SP
       ,
       {single_desc_bytes_hs, single_desc_bytes_hs + sizeof(single_desc_bytes_hs)}
#endif
};

//static uint8_t usbd_cdc_buffer[CDCD_BUF_SIZ];

/** Ctrl endpoint buffer */
static uint8_t ctrl_buffer[64] __attribute__ ((aligned (4)));

static uint8_t read_buffer[512] __attribute__ ((aligned (4)));
static volatile int read_pos, read_len;
static volatile bool ready, reading, writing;

/**
 * \brief Callback invoked when bulk OUT data received
 */
static bool usb_device_cb_bulk_out(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	(void) ep;
	(void) rc;

	read_len = count;
	reading = 0;

	/* No error. */
	return false;
}

/**
 * \brief Callback invoked when bulk IN data received
 */
static bool usb_device_cb_bulk_in(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	(void) ep;
	(void) rc;
	(void) count;

	writing = 0;

	/* No error. */
	return false;
}

/**
 * \brief Callback invoked when Line State Change
 */
static bool usb_device_cb_state_c(usb_cdc_control_signal_t state)
{
	if (state.rs232.DTR) {
		/* Callbacks must be registered after endpoint allocation */
		cdcdf_acm_register_callback(CDCDF_ACM_CB_READ, (FUNC_PTR)usb_device_cb_bulk_out);
		cdcdf_acm_register_callback(CDCDF_ACM_CB_WRITE, (FUNC_PTR)usb_device_cb_bulk_in);

		ready = 1;
	}

	/* No error. */
	return false;
}

/**
 * \brief CDC ACM Init
 */
void cdc_device_acm_init(void)
{
	/* usb stack init */
	usbdc_init(ctrl_buffer);

	/* usbdc_register_funcion inside */
	cdcdf_acm_init();

	usbdc_start(single_desc);
	usbdc_attach();
}

void usb_init(void) {
	cdc_device_acm_init();
}

static void usb_read(uint8_t *dest, int len) {
	while (len > 0) {
		while (reading) __WFE();
		
		while (len > 0 && read_pos != read_len) {
			*dest++ = read_buffer[read_pos++];
			len--;
		}

		if (read_pos == read_len) {
			read_pos = 0;
			read_len = 0;
			reading = 1;
			cdcdf_acm_read(read_buffer, sizeof(read_buffer));
		}
	}
}

static void usb_write(uint8_t *src, int len) {
	while (writing) __WFE();
	writing = 0;
	cdcdf_acm_write(src, len);
}

__attribute__ ((noinline))
RAMFUNC static void jtag(int num, uint8_t tms[], uint8_t tdi[], uint8_t tdo[]) {
	// [0]=tms, [1]=tdi, [2]=tdo
	static uint8_t bits[2048][3];

	ASSERT(num <= 2048);

	// Store offset of gpio output register in bit array
	// 0x30 for 1 (PIO_SODR), 0x34 for 0 (PIO_CODR)
	for (int i = 0; i < num; i++) {
		bits[i][0] = ((tms[i/8] >> (i&7)) & 1) ? 0x30 : 0x34;
		bits[i][1] = ((tdi[i/8] >> (i&7)) & 1) ? 0x30 : 0x34;
	}

	register int tmp;
	register uint8_t *ptr = (uint8_t *)bits;

	asm volatile (
		"cpsid i                                 \n\t"
		".balign 8                               \n\t"
		"1:                                      \n\t"

		"str %[tck_bit], [%[tck_base], #0x34]    \n\t"

		"ldrb %[t1], [%[ptr], #0]                \n\t"
		"str %[tms_bit], [%[tms_base], %[t1]]    \n\t"

		"ldrb %[t1], [%[ptr], #1]                \n\t"
		"str %[tdi_bit], [%[tdi_base], %[t1]]    \n\t"

		"nop;nop                                 \n\t"

		"str %[tck_bit], [%[tck_base], #0x30]    \n\t"

		"nop;nop;nop;nop                         \n\t"

		"ldr %[t1], [%[tdo_base], #0x3C]         \n\t"
		"lsrs %[t1], %[tdo_pin]                  \n\t"
		"strb %[t1], [%[ptr], #2]                \n\t"

		"adds %[ptr], #3                         \n\t"
		"cmp %[ptr], %[end]                      \n\t"
		"bne 1b                                  \n\t"

		"cpsie i                                 \n\t"

		: [t1] "=&l" (tmp),
		  [ptr] "+l" (ptr)
		: [end] "h" (bits + num*3),
		  [tck_base] "l" (port_to_reg(GPIO_PORT(TCK))),
		  [tms_base] "l" (port_to_reg(GPIO_PORT(TMS))),
		  [tdi_base] "l" (port_to_reg(GPIO_PORT(TDI))),
		  [tdo_base] "l" (port_to_reg(GPIO_PORT(TDO))),
		  [tck_bit] "l" (1U << GPIO_PIN(TCK)),
		  [tms_bit] "l" (1U << GPIO_PIN(TMS)),
		  [tdi_bit] "l" (1U << GPIO_PIN(TDI)),
		  [tdo_pin] "n" (GPIO_PIN(TDO))
		: "cc", "memory");

	// we rely on at least two pins having the same gpio ports to have enough low registers

	for (int i = 0; i < num; i++) {
		tdo[i/8] |= (bits[i][2] & 1) << (i&7);
	}
}

static void xvc(void) {
	static char xvcInfo[] = "xvcServer_v1.0:2000\n";

	static uint8_t cmd[16];

	static uint8_t tms_bytes[256];
	static uint8_t tdi_bytes[256];
	static uint8_t tdo_bytes[256];

	usb_read(cmd, 2);

	if (memcmp(cmd, "ge", 2) == 0) {
		usb_read(cmd, 6);
		usb_write((uint8_t *)xvcInfo, strlen(xvcInfo));
		return;
	}

	if (memcmp(cmd, "se", 2) == 0) {
		usb_read(cmd, 9);
		//usb_write(cmd + 5, 4);
		uint32_t period = 10;
		usb_write((uint8_t *)&period, 4);
		return;
	}

	if (memcmp(cmd, "sh", 2) == 0) {
		int num_bits;

		usb_read(cmd, 4);
		usb_read((uint8_t *)&num_bits, 4);

		ASSERT(num_bits <= 2000);

		int num_bytes = (num_bits + 7) / 8;

		usb_read(tms_bytes, num_bytes);
		usb_read(tdi_bytes, num_bytes);

		jtag(num_bits, tms_bytes, tdi_bytes, tdo_bytes);

		usb_write(tdo_bytes, num_bytes);
		return;
	}

	ASSERT(false);
}

int main(void) {
	atmel_start_init();

	while (!cdcdf_acm_is_enabled()) {
		// wait cdc acm to be installed
	};

	cdcdf_acm_register_callback(CDCDF_ACM_CB_STATE_C, (FUNC_PTR)usb_device_cb_state_c);

	while (true) {
		xvc();
	}

	return 0;
}
