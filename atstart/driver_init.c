/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_init.h"
#include <hal_init.h>
#include <hpl_pmc.h>
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hpl_usart_base.h>

struct usart_sync_descriptor USART_EDBG;

void USART_EDBG_PORT_init(void)
{

	gpio_set_pin_function(PA21, MUX_PA21A_USART1_RXD1);

	gpio_set_pin_function(PB4, MUX_PB4D_USART1_TXD1);
}

void USART_EDBG_CLOCK_init(void)
{
	_pmc_enable_periph_clock(ID_USART1);
}

void USART_EDBG_init(void)
{
	USART_EDBG_CLOCK_init();
	USART_EDBG_PORT_init();
	usart_sync_init(&USART_EDBG, USART1, _usart_get_usart_sync());
}

/* The USB module requires a GCLK_USB of 48 MHz ~ 0.25% clock
 * for low speed and full speed operation. */
#if (CONF_USBHS_SRC == CONF_SRC_USB_48M)
#if (CONF_USBHS_FREQUENCY > (48000000 + 48000000 / 400)) || (CONF_USBHS_FREQUENCY < (48000000 - 48000000 / 400))
#warning USB clock should be 48MHz ~ 0.25% clock, check your configuration!
#endif
#endif

void USB_DEVICE_INSTANCE_CLOCK_init(void)
{
	_pmc_enable_periph_clock(ID_USBHS);
}

void USB_DEVICE_INSTANCE_init(void)
{
	USB_DEVICE_INSTANCE_CLOCK_init();
	usb_d_init();
}

void system_init(void)
{
	init_mcu();

	_pmc_enable_periph_clock(ID_PIOA);

	_pmc_enable_periph_clock(ID_PIOB);

	_pmc_enable_periph_clock(ID_PIOC);

	/* Disable Watchdog */
	hri_wdt_set_MR_WDDIS_bit(WDT);

	/* GPIO on PA19 */

	gpio_set_pin_level(TCK,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(TCK, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(TCK, GPIO_PIN_FUNCTION_OFF);

	/* GPIO on PB2 */

	// Set pin direction to input
	gpio_set_pin_direction(TDO, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(TDO,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_DOWN);

	gpio_set_pin_function(TDO, GPIO_PIN_FUNCTION_OFF);

	/* GPIO on PC8 */

	gpio_set_pin_level(LED,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   true);

	// Set pin direction to output
	gpio_set_pin_direction(LED, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(LED, GPIO_PIN_FUNCTION_OFF);

	/* GPIO on PC17 */

	gpio_set_pin_level(TMS,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   true);

	// Set pin direction to output
	gpio_set_pin_direction(TMS, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(TMS, GPIO_PIN_FUNCTION_OFF);

	/* GPIO on PC30 */

	gpio_set_pin_level(TDI,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   true);

	// Set pin direction to output
	gpio_set_pin_direction(TDI, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(TDI, GPIO_PIN_FUNCTION_OFF);

	USART_EDBG_init();

	USB_DEVICE_INSTANCE_init();
}
