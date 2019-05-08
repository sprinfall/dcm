#ifndef DCM_UTIL_H_
#define DCM_UTIL_H_

#include "dcm/defs.h"

namespace dcm {

// -----------------------------------------------------------------------------

std::uint16_t SwapUint16(std::uint16_t value);
std::uint32_t SwapUint32(std::uint32_t value);

void Swap16(void* src);
void Swap32(void* src);
void Swap64(void* src);

}  // namespace dcm

#endif  // DCM_UTIL_H_
