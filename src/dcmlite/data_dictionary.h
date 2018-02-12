#ifndef DCMLITE_DATA_DICTIONARY_H_
#define DCMLITE_DATA_DICTIONARY_H_
#pragma once

// Data dictionary singleton.

#include "dcmlite/data_entry.h"
#include "dcmlite/tag.h"

namespace dcmlite {

class DataDictionary {
public:
  static DataDictionary& Get();

  ~DataDictionary() = default;

  const DataEntry* FindEntry(const Tag& tag) const;

private:
  DataDictionary() = default;
};

}  // namespace dcmlite

#endif  // DCMLITE_DATA_DICTIONARY_H_
