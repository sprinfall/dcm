#include "dcm/data_set.h"

#include <algorithm>

#include "dcm/visitor.h"

namespace dcm {

// -----------------------------------------------------------------------------

namespace {

bool Less(DataElement* lhs, DataElement* rhs) {
  return lhs->tag() < rhs->tag();
};

}  // namespace

// -----------------------------------------------------------------------------

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

const DataElement* DataSet::GetElement(Tag tag) const {
  return const_cast<DataSet*>(this)->DoGetElement(tag);
}

bool DataSet::AppendElement(DataElement* element) {
  if (elements_.empty() || element->tag() > elements_.back()->tag()) {
    elements_.push_back(element);
    return true;
  }
  return false;
}

bool DataSet::InsertElement(DataElement* element) {
  auto it = std::lower_bound(elements_.begin(), elements_.end(), element, Less);
  if (it != elements_.end() && (*it)->tag() == element->tag()) {
    return false;  // The tag already exists.
  }
  elements_.insert(it, element);
  return true;
}

void DataSet::Clear() {
  endian_ = Endian::Little();
  explicit_vr_ = true;

  for (DataElement* element : elements_) {
    delete element;
  }
  elements_.clear(); 
}

std::size_t DataSet::GetValueLength(Tag tag) const {
  const DataElement* element = GetElement(tag);
  if (element == nullptr) {
    return kUndefinedLength;  // TODO
  }
  return element->length();
}

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

// TODO: trim value?
bool DataSet::SetString(Tag tag, const std::string& value) {
  DataElement* element = DoGetElement(tag);
  if (element != nullptr) {
    return element->SetString(value);
  }

  element = new DataElement(tag, endian_);

  if (!element->SetString(value)) {
    delete element;
    return false;
  }

  InsertElement(element);
  return true;
}

bool DataSet::GetInt16(Tag tag, std::int16_t* value) const {
  GET_VALUE(Int16);
}

bool DataSet::GetUint16(Tag tag, std::uint16_t* value) const {
  GET_VALUE(Uint16);
}

bool DataSet::GetInt32(Tag tag, std::int32_t* value) const {
  GET_VALUE(Int32);
}

bool DataSet::GetUint32(Tag tag, std::uint32_t* value) const {
  GET_VALUE(Uint32);
}

bool DataSet::GetFloat32(Tag tag, float32_t* value) const {
  GET_VALUE(Float32);
}

bool DataSet::GetFloat64(Tag tag, float64_t* value) const {
  GET_VALUE(Float64);
}

DataElement* DataSet::DoGetElement(Tag tag/*, bool create*/) {
  std::size_t j = 0;  // begin
  std::size_t k = elements_.size();  // end
  std::size_t i = 0;

  while (j < k) {
    i = j + (k - j) / 2;

    if (tag < elements_[i]->tag()) {
      k = i;
    } else if (tag > elements_[i]->tag()) {
      j = i + 1;
    } else {
      return elements_[i];
    }
  }

  //if (create) {
  //  DataElement* element = new DataElement(tag, endian_);
  //  elements_.insert(elements_.begin() + i, element);
  //  return element;
  //}

  return nullptr;
}

}  // namespace dcm
