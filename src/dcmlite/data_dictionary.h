#ifndef DCMLITE_DATA_DICTIONARY_H_
#define DCMLITE_DATA_DICTIONARY_H_
#pragma once

#include <map>

#include "dcmlite/defs.h"
#include "dcmlite/tag.h"
#include "dcmlite/vr.h"

namespace dcmlite {

class DataEntry {
public:
  Tag tag;
  VR::Type vr_type;
  std::string description;
};

class DataDictionary {
public:
  static DataDictionary& Get();

  ~DataDictionary();

  const DataEntry* FindEntry(const Tag& tag) const;

private:
  DataDictionary();

  void RegisterTag(std::uint32_t tag_key, VR::Type vr_type, const std::string& description);

private:
  typedef std::map<Tag, DataEntry*> TagMap;
  TagMap tag_map_;
};

}  // namespace dcmlite

#endif  // DCMLITE_DATA_DICTIONARY_H_
