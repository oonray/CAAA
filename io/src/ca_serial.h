#ifndef CA_SERIAL_H_
#define CA_SERIAL_H_

#include <ca_dbg.h>
#include <ca_fileio.h>

enum ca_baud {
  CA_B_300 = 300,
  CA_B_1200 = 1200,
  CA_B_DFAULT = 9600,
  CA_B_960 = 9600,
  CA_B_19200 = 19200,
  CA_B_115200 = 115200
};

#endif // CA_SERIAL_H_
