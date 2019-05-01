#include "dcm/data_dict.h"

namespace dcm {

#include "dcm/data_dict.inl"

const DataEntry* DataDict::GetEntry(Tag tag) {
  std::uint32_t tag_value = tag.ToUint32();

  std::size_t j = 0;  // begin
  std::size_t k = DATA_DICT_SIZE;  // end
  std::size_t i = 0;

  while (j < k) {
    i = j + (k - j) / 2;

    const DataEntry& entry = g_data_dict[i];

    if (tag_value < entry.tag) {
      k = i;
    } else if (tag_value > entry.tag) {
      j = i + 1;
    } else {
      return &entry;
    }
  }

  return nullptr;
}

VR DataDict::GetVR(Tag tag) {
  const DataEntry* entry = DataDict::GetEntry(tag);
  return (entry != nullptr ? entry->vr : VR::UNKNOWN);
}

}  // namespace dcm
