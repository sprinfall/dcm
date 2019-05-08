#ifndef DCM_DATA_DICT_H_
#define DCM_DATA_DICT_H_

// Data dictionary singleton.

#include "dcm/defs.h"

namespace dcm {

class DataEntry {
public:
  std::uint32_t tag;
  VR vr;
  const char* keyword;
  const char* description;
};

class DataDict {
  DataDict() = delete;

public:
  static const DataEntry* GetEntry(Tag tag);

  // Get the VR of the tag.
  // Return VR::UNKNOWN if the tag doesn't exist in the dictionary.
  static VR GetVR(Tag tag);
};

}  // namespace dcm

#endif  // DCM_DATA_DICT_H_
