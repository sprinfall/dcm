#include "dcm/data_set.h"

#include "dcm/visitor.h"

namespace dcm {

DataSet::DataSet(Tag tag, Endian endian)
    : DataElement(tag, tag.IsEmpty() ? VR::UNKNOWN : VR::SQ, endian),
      explicit_vr_(true) {

  // Undefined length makes more sense to a data set.
  length_ = kUndefinedLength;
}  

DataSet::~DataSet() {
  Clear();
}

void DataSet::Accept(Visitor& visitor) {
  visitor.VisitDataSet(this);
}

DataElement* DataSet::operator[](std::size_t index) {
  assert(index < elements_.size());
  return elements_[index];
}

const DataElement* DataSet::operator[](std::size_t index) const {
  assert(index < elements_.size());
  return elements_[index];
}

const DataElement* DataSet::At(std::size_t index) const {
  if (index < elements_.size()) {
    return elements_[index];
  }
  return nullptr;
}


void DataSet::AddElement(DataElement* element) {
  elements_.push_back(element);
}

const DataElement* DataSet::GetElement(Tag tag) const {
  for (DataElement* element : elements_) {
    if (element->tag() == tag) {
      return element;
    }
  }
  return nullptr;
}

void DataSet::Clear() {
  endian_ = kLittleEndian;
  explicit_vr_ = true;

  for (DataElement* element : elements_) {
    delete element;
  }
  elements_.clear(); 
}

std::size_t DataSet::GetLength(Tag tag) const {
  const DataElement* element = GetElement(tag);
  if (element == nullptr) {
    return kUndefinedLength;  // TODO
  }
  return element->length();
}

//bool DataSet::GetBuffer(Tag tag, Buffer* buffer, std::size_t* length) const {
//  const DataElement* element = GetElement(tag);
//  if (element != nullptr) {
//    *buffer = element->buffer();
//    *length = element->length();
//    return true;
//  }
//  return false;
//}

// type: Uint16, Int32, etc.
#define GET_VALUE(type)\
const DataElement* element = GetElement(tag);\
if (element != nullptr) {\
  return element->Get##type(value);\
}\
return false;

bool DataSet::GetString(Tag tag, std::string* value) const {
  GET_VALUE(String);
}

bool DataSet::SetString(Tag tag, const std::string& value) {
  DataElement* element = GetElement(tag);
  if (element == nullptr) {
    // TODO: Insert a new element.
    return false;
  }

  element->SetString(value);
  return true;
}

bool DataSet::GetUint16(Tag tag, std::uint16_t* value) const {
  GET_VALUE(Uint16);
}

bool DataSet::GetUint32(Tag tag, std::uint32_t* value) const {
  GET_VALUE(Uint32);
}

bool DataSet::GetInt16(Tag tag, std::int16_t* value) const {
  GET_VALUE(Int16);
}

bool DataSet::GetInt32(Tag tag, std::int32_t* value) const {
  GET_VALUE(Int32);
}

bool DataSet::GetFloat32(Tag tag, float32_t* value) const {
  GET_VALUE(Float32);
}

bool DataSet::GetFloat64(Tag tag, float64_t* value) const {
  GET_VALUE(Float64);
}

DataElement* DataSet::GetElement(Tag tag) {
  for (DataElement* element : elements_) {
    if (element->tag() == tag) {
      return element;
    }
  }
  return nullptr;
}

}  // namespace dcm
