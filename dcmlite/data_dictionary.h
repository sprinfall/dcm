#ifndef DCMLITE_DATA_DICTIONARY_H_
#define DCMLITE_DATA_DICTIONARY_H_

// Data dictionary singleton.

#include <memory>
#include <mutex>

#include "dcmlite/data_entry.h"
#include "dcmlite/tag.h"

namespace dcmlite {

class DataDictionary {
public:
  static DataDictionary& Get();

  ~DataDictionary() = default;

  const DataEntry* FindEntry(Tag tag) const;

private:
  DataDictionary() = default;

  static std::unique_ptr<DataDictionary> s_instance;
  static std::mutex s_mutex;
};

}  // namespace dcmlite

#endif  // DCMLITE_DATA_DICTIONARY_H_
