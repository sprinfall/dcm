#ifndef DCMLITE_TAGS_READER_H_
#define DCMLITE_TAGS_READER_H_
#pragma once

#include <vector>

#include "dcmlite/tag.h"

namespace dcmlite {

class DataElement;
class TagsReadHandler;

// Read the given tags without loading the whole data set.
class TagsReader {
public:
  TagsReader();
  ~TagsReader();

  // Add a tag to read.
  TagsReader& AddTag(const Tag& tag);

  // Read tags from the given file.
  bool ReadFile(const std::string& file_path);

  DataElement* GetElement(const Tag& tag) const;

private:
  TagsReadHandler* handler_;
};

}  // namespace dcmlite

#endif  // DCMLITE_TAGS_READER_H_
