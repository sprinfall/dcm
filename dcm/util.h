#ifndef DCM_UTIL_H_
#define DCM_UTIL_H_

#include <string>

#include "dcm/defs.h"

namespace dcm {

// -----------------------------------------------------------------------------

std::uint16_t SwapUint16(std::uint16_t value);
std::uint32_t SwapUint32(std::uint32_t value);

void Swap16(void* src);
void Swap32(void* src);
void Swap64(void* src);

// -----------------------------------------------------------------------------

std::string VRToString(VR vr);

// Convert a string to VR enum type.
// Return false if the string is not a valid VR.
bool StringToVR(const std::string& str, VR* vr = nullptr);

// For VRs of OB, OD, OF, OL, OW, SQ, UN and UC, UR, UT, the 16 bits
// following the two character VR Field are reserved for use by later
// versions of the DICOM Standard.
// See PS 3.5 Section 7.1.2 - Data Element Structure with Explicit VR.
bool Is16BitsFollowingVrReversed(VR vr);

}  // namespace dcm

#endif  // DCM_UTIL_H_
