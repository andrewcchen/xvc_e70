#ifndef JTAG_H
#define JTAG_H

#define JTAG_MAX_BITS 2000

void jtag_shift(int num_bits, uint8_t tms_bytes[], uint8_t tdi_bytes[], uint8_t tdo_bytes[]);

#endif
