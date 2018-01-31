#include "data_dictionary.h"

#include <memory>  // for unique_ptr

#include "dcmlite/tag_table.h"

namespace dcmlite {

DataDictionary& DataDictionary::Get() {
  static std::unique_ptr<DataDictionary> s_instance(new DataDictionary());
  return *s_instance;
}

DataDictionary::~DataDictionary() {
  for (TagMap::value_type& v : tag_map_) {
    delete v.second;
  }
  tag_map_.clear();
}

const DataEntry* DataDictionary::FindEntry(const Tag& tag) const {
  TagMap::const_iterator it = tag_map_.find(tag);
  if (it == tag_map_.end()) {
    return NULL;
  }
  return it->second;
}

DataDictionary::DataDictionary() {
  for (std::size_t i = 0; g_tag_table[i].tag != 0; ++i) {
    RegisterTag(g_tag_table[i].tag, g_tag_table[i].vr_type, g_tag_table[i].description);
  }
}

void DataDictionary::RegisterTag(std::uint32_t tag_key, VR::Type vr_type, const std::string& description) {
  DataEntry* entry = new DataEntry;
  entry->tag = Tag(tag_key);
  entry->vr_type = vr_type;
  entry->description = description;

  tag_map_[entry->tag] = entry;
}

}  // namespace dcmlite
