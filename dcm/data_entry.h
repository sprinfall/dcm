#ifndef DCM_DATA_ENTRY_H_
#define DCM_DATA_ENTRY_H_

// Data dictionary entry.

#include "dcm/defs.h"
#include "dcm/vr.h"

namespace dcm {

class DataEntry {
public:
  std::uint32_t tag;
  VR::Type vr_type;
  std::string keyword;
  std::string description;
};

}  // namespace dcm

#endif  // DCM_DATA_ENTRY_H_
