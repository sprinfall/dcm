#ifndef DCM_DATA_DICT_H_
#define DCM_DATA_DICT_H_

#include "dcm/defs.h"

namespace dcm {
namespace dict {

// -----------------------------------------------------------------------------

struct Entry {
  std::uint32_t tag;
  VR vr;
  const char* keyword;
};

// -----------------------------------------------------------------------------

const Entry* GetEntry(Tag tag);

// Get VR of the given tag.
// Return VR::UN if the tag doesn't exist in the dictionary.
VR GetVR(Tag tag);

// Get VM of the given tag.
// Return false if the tag's VM is 1.
// For saving memory, the VM dictionary doesn't store the tags whose VM is 1.
const VM* GetVM(Tag tag);

// Check if the value is a valid VM for the given tag.
bool CheckVM(Tag tag, std::size_t value);

}  // namespace dict
}  // namespace dcm

#endif  // DCM_DATA_DICT_H_
