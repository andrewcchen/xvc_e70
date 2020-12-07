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

__attribute__ ((section (".itcm.usb_read")))
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

__attribute__ ((section (".itcm.usb_write")))
static void usb_write(uint8_t *src, int len) {
	while (writing) __WFE();
	writing = 0;
	cdcdf_acm_write(src, len);
}

#define NOP asm volatile ("nop")

__attribute__ ((section (".itcm.xvc")))
static void xvc(void) {
	static char xvcInfo[] = "xvcServer_v1.0:2000\n";

	static uint8_t cmd[16];

	static uint8_t tms_bytes[256];
	static uint8_t tdi_bytes[256];
	static uint8_t tdo_bytes[256];

	static uint8_t tms_bits[2048] __attribute__ ((section (".dtcm.tms_bits")));
	static uint8_t tdi_bits[2048] __attribute__ ((section (".dtcm.tdi_bits")));
	static uint8_t tdo_bits[2048] __attribute__ ((section (".dtcm.tdo_bits")));

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

		for (int i = 0; i < num_bits; i++) {
			tms_bits[i] = (tms_bytes[i/8] >> (i&7)) & 1;
			tdi_bits[i] = (tdi_bytes[i/8] >> (i&7)) & 1;
		}

		__disable_irq();

#define NOP4 NOP; NOP; NOP; NOP
#define NOP16 NOP4; NOP4; NOP4; NOP

		for (int i = 0; i < num_bits; i++) {

			tdo_bits[i] = !!gpio_get_pin_level(TDO);
			gpio_set_pin_level(TMS, tms_bits[i]);
			gpio_set_pin_level(TDI, tdi_bits[i]);
			gpio_set_pin_level(TCK, 1);

			NOP16;

			gpio_set_pin_level(TCK, 0);

			NOP16; NOP16; NOP16; NOP16;
		}

		__enable_irq();

		memset(tdo_bytes, 0, num_bytes);
		for (int i = 0; i < num_bits; i++) {
			tdo_bytes[i/8] |= (tdo_bits[i]&1) << (i&7);
		}

		usb_write(tdo_bytes, num_bytes);
		return;
	}

	ASSERT(false);
}

__attribute__ ((section (".itcm.main")))
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
