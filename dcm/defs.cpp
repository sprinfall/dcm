#include "dcm/defs.h"

namespace dcm {

static union {
  char c[4];
  unsigned long mylong;
} endian_test = { { 'l', '?', '?', 'b' } };

#define ENDIANNESS ((char)endian_test.mylong)

const Endian kOSEndian = (ENDIANNESS == 'l') ? Endian::Little() : Endian::Big();

}  // namespace dcm
