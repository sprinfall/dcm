#include "dcmlite/data_set.h"
#include <iostream>
#include "dcmlite/visitor.h"

namespace dcmlite {

DataSet::DataSet(const Tag& tag)
    : DataElement(tag, tag.IsEmpty() ? VR::UNKNOWN : VR::SQ)
    , endian_(kLittleEndian)
    , explicit_vr_(true) {

  // Undefined length makes more sense to a data set.
  length_ = kUndefinedLength;
}  

DataSet::~DataSet() {
  Clear();
}

void DataSet::Accept(Visitor& visitor) const {
  visitor.VisitDataSet(this);
}

void DataSet::AddElement(DataElement* element) {
  elements_.push_back(element);
}

void DataSet::Clear() {
  endian_ = kLittleEndian;
  explicit_vr_ = true;

  for (DataElement* element : elements_) {
    delete element;
  }
  elements_.clear(); 
}

bool DataSet::GetBuffer(const Tag& tag,
                        boost::shared_array<char>* buffer,
                        size_t* len) const {
  DataElement* element = GetElement(tag);
  if (element != NULL) {
    *buffer = element->buffer();
    *len = element->length();
    return true;
  }
  return false;
}

// TODO: Check VR?
bool DataSet::GetString(const Tag& tag, std::string* value) const {
  DataElement* element = GetElement(tag);
  if (element != NULL) {
    return element->AsString(value);
  }
  return false;
}

// TODO: Check VR?
bool DataSet::GetUint16(const Tag& tag, std::uint16_t* value) const {
  DataElement* element = GetElement(tag);
  if (element != NULL) {
    if (element->AsUint16(value)) {
      AdjustBytesUint16(*value);
      return true;
    }
  }
  return false;
}

// TODO: Check VR?
bool DataSet::GetUint32(const Tag& tag, std::uint32_t* value) const {
  DataElement* element = GetElement(tag);
  if (element != NULL) {
    if (element->AsUint32(value)) {
      AdjustBytesUint32(*value);
      return true;
    }
  }
  return false;
}

void DataSet::AdjustBytesUint16(std::uint16_t& value) const {
  if (endian_ != PlatformEndian()) {
    value = ReverseBytesUint16(value);
  }
}

void DataSet::AdjustBytesUint32(std::uint32_t& value) const {
  if (endian_ != PlatformEndian()) {
    value = ReverseBytesUint32(value);
  }
}

DataElement* DataSet::GetElement(const Tag& tag) const {
  for (DataElement* element : elements_) {
    if (element->tag() == tag) {
      return element;
    }
  }
  return NULL;
}

}  // namespace dcmlite
