/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_examples.h"
#include "driver_init.h"
#include "utils.h"

/**
 * Example of using USART_EDBG to write "Hello World" using the IO abstraction.
 */
void USART_EDBG_example(void)
{
	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&USART_EDBG, &io);
	usart_sync_enable(&USART_EDBG);

	io_write(io, (uint8_t *)"Hello World!", 12);
}
