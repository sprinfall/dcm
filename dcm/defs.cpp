#include "dcm/defs.h"

namespace dcm {

static union {
  char c[4];
  unsigned long mylong;
} endian_test = { { 'l', '?', '?', 'b' } };

#define ENDIANNESS ((char)endian_test.mylong)

Endian PlatformEndian() {
  static Endian endian = (ENDIANNESS == 'l') ? kLittleEndian : kBigEndian;
  return endian;
}

std::uint16_t SwapUint16(std::uint16_t value) {
  return (((value & 0xFF00) >> 8) | ((value & 0x00FF) << 8));
}

std::uint32_t SwapUint32(std::uint32_t value) {
  return (((value & 0x000000FF) << 24) |
          ((value & 0x0000FF00) << 8) |
          ((value & 0x00FF0000) >> 8) |
          ((value & 0xFF000000) >> 24));
}

void Swap16(void* src) {
  std::uint16_t* p = (std::uint16_t*)src;
  *p = SwapUint16(*p);
}

void Swap32(void* src) {
  std::uint32_t* p = (std::uint32_t*)src;
  *p = SwapUint32(*p);
}

void Swap64(void* src) {
  std::uint32_t* p = (std::uint32_t*)src;
  std::uint32_t c = p[0];

  p[0] = SwapUint32(p[1]);
  p[1] = SwapUint32(c);
}

}  // namespace dcm
