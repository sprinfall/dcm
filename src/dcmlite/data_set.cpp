#include "dcmlite/data_set.h"
#include <iostream>
#include "dcmlite/visitor.h"

namespace dcmlite {

DataSet::DataSet(const Tag& tag, Endian endian)
    : DataElement(tag, tag.IsEmpty() ? VR::UNKNOWN : VR::SQ, endian)
    , explicit_vr_(true) {

  // Undefined length makes more sense to a data set.
  length_ = kUndefinedLength;
}  

DataSet::~DataSet() {
  Clear();
}

void DataSet::Accept(Visitor& visitor) {
  visitor.VisitDataSet(this);
}

DataElement* DataSet::operator[](size_t index) {
  assert(index < elements_.size());
  return elements_[index];
}

const DataElement* DataSet::operator[](size_t index) const {
  assert(index < elements_.size());
  return elements_[index];
}

const DataElement* DataSet::At(size_t index) const {
  if (index < elements_.size()) {
    return elements_[index];
  }
  return NULL;
}


void DataSet::AddElement(DataElement* element) {
  elements_.push_back(element);
}

const DataElement* DataSet::GetElement(const Tag& tag) const {
  for (DataElement* element : elements_) {
    if (element->tag() == tag) {
      return element;
    }
  }
  return NULL;
}

void DataSet::Clear() {
  endian_ = kLittleEndian;
  explicit_vr_ = true;

  for (DataElement* element : elements_) {
    delete element;
  }
  elements_.clear(); 
}

// type: Uint16, Int32, etc.
#define GET_VALUE(type)\
const DataElement* element = GetElement(tag);\
if (element != NULL) {\
  return element->Get##type(value);\
}\
return false;

bool DataSet::GetString(const Tag& tag, std::string* value) const {
  GET_VALUE(String);
}

bool DataSet::GetUint16(const Tag& tag, std::uint16_t* value) const {
  GET_VALUE(Uint16);
}

bool DataSet::GetUint32(const Tag& tag, std::uint32_t* value) const {
  GET_VALUE(Uint32);
}

bool DataSet::GetInt16(const Tag& tag, std::int16_t* value) const {
  GET_VALUE(Int16);
}

bool DataSet::GetInt32(const Tag& tag, std::int32_t* value) const {
  GET_VALUE(Int32);
}

bool DataSet::GetFloat32(const Tag& tag, float32_t* value) const {
  GET_VALUE(Float32);
}

bool DataSet::GetFloat64(const Tag& tag, float64_t* value) const {
  GET_VALUE(Float64);
}

}  // namespace dcmlite
