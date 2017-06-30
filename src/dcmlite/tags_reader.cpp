#include "dcmlite/tags_reader.h"

#include <iostream>

#include "dcmlite/data_element.h"
#include "dcmlite/data_set.h"
#include "dcmlite/dicom_reader.h"
#include "dcmlite/read_handler.h"

namespace dcmlite {

TagsReader::TagsReader()
    : handler_(new TagsReadHandler) {
}

TagsReader::~TagsReader() {
  delete handler_;
}

TagsReader& TagsReader::AddTag(const Tag& tag) {
  handler_->AddTag(tag);
  return *this;
}

bool TagsReader::ReadFile(const std::string& file_path) {
  DicomReader reader(handler_);
  return reader.ReadFile(file_path);
}

DataElement* TagsReader::GetElement(const Tag& tag) const {
  return handler_->GetElement(tag);
}

}  // namespace dcmlite
