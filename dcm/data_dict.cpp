#include "dcm/data_dict.h"

#include <algorithm>

namespace dcm {
namespace dict {

// -----------------------------------------------------------------------------

struct VMEntry {
  std::uint32_t tag;
  VM vm;
};

#include "dcm/dict.inl"

#include "dcm/dict_vm.inl"

// -----------------------------------------------------------------------------

const Entry* GetEntry(Tag tag) {
  auto less = [](const Entry& entry, std::uint32_t tag) {
    return entry.tag < tag;
  };

  const auto end = g_dict + ARRAY_SIZE(g_dict);

  auto it = std::lower_bound(g_dict, end, tag.ToUint32(), less);
  if (it != end && it->tag == tag) {
    return it;
  }

  return nullptr;
}

const char* GetName(Tag tag) {
  if (tag == tags::kSeqDelimatation) {
    return "Sequence Delimatation";
  }
  if (tag == tags::kSeqItemDelimatation) {
    return "Sequence Item Delimatation";
  }
  if (tag == tags::kSeqItemPrefix) {
    return "Sequence Item Prefix";
  }

  const Entry* entry = GetEntry(tag);
  return (entry != nullptr ? entry->keyword : "");
}


VR GetVR(Tag tag) {
  const Entry* entry = GetEntry(tag);
  return (entry != nullptr ? entry->vr : VR::UN);
}

const VM* GetVM(Tag tag) {
  auto less = [](const VMEntry& entry, std::uint32_t tag) {
    return entry.tag < tag;
  };

  const auto end = g_dict_vm + ARRAY_SIZE(g_dict_vm);

  auto it = std::lower_bound(g_dict_vm, end, tag.ToUint32(), less);

  if (it != end && it->tag == tag) {
    return &it->vm;
  }

  return nullptr;
}

bool CheckVM(Tag tag, std::size_t value) {
  const VM* vm = GetVM(tag);

  if (vm != nullptr) {
    return vm->Check(value);
  }

  // Tag not in the VM dict, assume the VM is 1.
  return value == 1;
}

}  // namespace dict
}  // namespace dcm
