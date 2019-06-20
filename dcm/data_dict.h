#ifndef DCM_DATA_DICT_H_
#define DCM_DATA_DICT_H_

#include "dcm/defs.h"

namespace dcm {
namespace dict {

// Data dictionary entry.
// E.g., { 0x00080018, VR::UI, "SOPInstanceUID" }
struct Entry {
  std::uint32_t tag;
  VR vr;
  const char* keyword;  // TODO: Rename
};

// Get the data entry of the tag.
// Return nullptr if the the dictionary has no such tag.
const Entry* GetEntry(Tag tag);

// Get the name (or description) of the tag.
const char* GetName(Tag tag);

// Get the VR of the tag.
// Return VR::UN if the the dictionary has no such tag.
VR GetVR(Tag tag);

// Get the VM of the tag.
// Return nullptr if the tag's VM is 1.
// For saving memory, the VM dictionary doesn't store the tags whose VM is 1.
const VM* GetVM(Tag tag);

// Check if the value is a valid VM for the given tag.
bool CheckVM(Tag tag, std::size_t value);

}  // namespace dict
}  // namespace dcm

#endif  // DCM_DATA_DICT_H_
