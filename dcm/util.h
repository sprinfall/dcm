#ifndef DCM_UTIL_H_
#define DCM_UTIL_H_

#include "dcm/defs.h"

namespace dcm {
namespace util {

// -----------------------------------------------------------------------------

std::uint16_t SwapUint16(std::uint16_t value);
std::uint32_t SwapUint32(std::uint32_t value);

void Swap16(void* value);
void Swap32(void* value);
void Swap64(void* value);

// |size| should be 2, 4 or 8.
void SwapBytes(void* value, std::size_t size);

}  // namespace util
}  // namespace dcm

#endif  // DCM_UTIL_H_
