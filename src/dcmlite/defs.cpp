#include "defs.h"

namespace dcmlite {

static union { char c[4]; unsigned long mylong; } endian_test = { { 'l', '?', '?', 'b' } };
#define ENDIANNESS ((char)endian_test.mylong)

Endian GetPlatformEndian() {
  return (ENDIANNESS == 'l') ? kLittleEndian : kBigEndian;
}

}  // namespace dcmlite
