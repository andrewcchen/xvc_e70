#include "atmel_start.h"

void panic(void) {
	__disable_irq();
	gpio_set_pin_level(LED, 0);
	__BKPT(0);
	while (1);
}

void assert_failed(const char *const file, const int line) {
	(void) file;
	(void) line;
	panic();
}
