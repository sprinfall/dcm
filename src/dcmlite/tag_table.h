#ifndef DCMLITE_TAG_TABLE_H_
#define DCMLITE_TAG_TABLE_H_
#pragma once

#include "dcmlite/defs.h"
#include "dcmlite/vr.h"

namespace dcmlite {

// Tags description and VR.
struct TagDescription {
  std::uint32_t tag;
  std::string description;
  std::string keyword;
  VR::Type vr_type;
};

extern const TagDescription g_tag_table[];

}  // namespace dcmlite

#endif  // DCMLITE_TAG_TABLE_H_
