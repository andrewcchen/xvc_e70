#ifndef JTAG_H
#define JTAG_H

#define JTAG_MAX_BYTES 2000
#define XVC_INFO "xvcServer_v1.0:2000\n"
#define JTAG_MAX_BITS (JTAG_MAX_BYTES * 8)

void jtag_shift(int num_bits, uint8_t tms_bytes[], uint8_t tdi_bytes[], uint8_t tdo_bytes[]);

#endif
