#include "dcm/util.h"

namespace dcm {
namespace util {

// -----------------------------------------------------------------------------

std::uint16_t SwapUint16(std::uint16_t value) {
  return (((value & 0xFF00) >> 8) | ((value & 0x00FF) << 8));
}

std::uint32_t SwapUint32(std::uint32_t value) {
  return (((value & 0x000000FF) << 24) |
          ((value & 0x0000FF00) << 8) |
          ((value & 0x00FF0000) >> 8) |
          ((value & 0xFF000000) >> 24));
}

void Swap16(void* value) {
  std::uint16_t* p = static_cast<std::uint16_t*>(value);
  *p = SwapUint16(*p);
}

void Swap32(void* value) {
  std::uint32_t* p = static_cast<std::uint32_t*>(value);
  *p = SwapUint32(*p);
}

void Swap64(void* value) {
  std::uint32_t* p = static_cast<std::uint32_t*>(value);
  std::uint32_t c = p[0];

  p[0] = SwapUint32(p[1]);
  p[1] = SwapUint32(c);
}

void SwapBytes(void* value, std::size_t size) {
  if (size == 2) {
    Swap16(value);
  } else if (size == 4) {
    Swap32(value);
  } else if (size == 8) {
    Swap64(value);
  }
}

}  // namespace util
}  // namespace dcm
