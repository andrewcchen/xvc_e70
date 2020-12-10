#include "atmel_start.h"

#include "jtag.h"

#if GPIO_PORT(TMS) != GPIO_PORT(TDI)
#error TMS and TDI must be on the same GPIO port
#endif

static struct {
	uint32_t tms_tdi;
	uint32_t tdo;
} jtag_kernel_data[JTAG_MAX_BITS] __attribute__((section(".dtcm")));

// gpio registers:
// 0x30 PIO_SODR: set high
// 0x34 PIO_CODR: set low
// 0x38 PIO_ODSR: set output
// 0x3C PIO_PSDR: read input

__attribute__ ((noinline, section(".itcm")))
void jtag_shift(int num_bits, uint8_t tms_bytes[], uint8_t tdi_bytes[], uint8_t tdo_bytes[]) {

	ASSERT(num_bits <= JTAG_MAX_BITS);

	for (int i = 0; i < num_bits; i++) {
		int tms = (tms_bytes[i/8] >> (i&7)) & 1;
		int tdi = (tdi_bytes[i/8] >> (i&7)) & 1;
		jtag_kernel_data[i].tms_tdi = (tms << GPIO_PIN(TMS)) | (tdi << GPIO_PIN(TDI));
	}

	__disable_irq();

	gpio_set_pin_level(TCK, 0);

	// Setting only TMS & TDI pins in PIO_OWSR to ensure only those pins are written
	void *port = port_to_reg(GPIO_PORT(TMS));
	uint32_t owsr = hri_pio_read_OWSR_reg(port);
	hri_pio_write_OWSR_reg(port, (1U << GPIO_PIN(TMS)) | (1U << GPIO_PIN(TDI)));

	register int tmp;
	register void *ptr = jtag_kernel_data;
	asm volatile (
		".balign 8                             \n\t"
		"1:                                    \n\t"
		"ldr %[tmp], [%[ptr], #0]              \n\t"
		"str %[tmp], [%[tms_tdi_base], #0x38]  \n\t"
		"ldr %[tmp], [%[tdo_base], #0x3C]      \n\t"
		"str %[tck_bit], [%[tck_base], #0x30]  \n\t"
		"str %[tck_bit], [%[tck_base], #0x34]  \n\t"
		"str %[tmp], [%[ptr], #4]              \n\t"
		"adds %[ptr], #8                       \n\t"
		"cmp %[ptr], %[end]                    \n\t"
		"bne 1b                                \n\t"
		: [tmp] "=&l" (tmp),
		  [ptr] "+l" (ptr)
		: [end] "r" (jtag_kernel_data + num_bits),
		  [tck_base] "l" (port_to_reg(GPIO_PORT(TCK))),
		  [tms_tdi_base] "l" (port_to_reg(GPIO_PORT(TMS))),
		  [tdo_base] "l" (port_to_reg(GPIO_PORT(TDO))),
		  [tck_bit] "l" (1U << GPIO_PIN(TCK))
		: "cc", "memory");

	hri_pio_write_OWSR_reg(port, owsr);

	__enable_irq();

	memset(tdo_bytes, 0, (num_bits + 7) / 8);

	for (int i = 0; i < num_bits; i++) {
		int tdo = jtag_kernel_data[i].tdo >> GPIO_PIN(TDO);
		tdo_bytes[i/8] |= (tdo&1) << (i&7);
	}
}
