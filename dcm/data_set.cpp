#include "dcm/data_set.h"

#include <algorithm>  // for lower_bound

#include "dcm/visitor.h"

namespace dcm {

DataSet::DataSet(Endian endian, Charset charset)
    : endian_(endian), charset_(charset), explicit_vr_(true) {
}  

DataSet::~DataSet() {
  Clear();
}

void DataSet::Accept(Visitor& visitor) const {
  visitor.VisitDataSet(this);

  // NOTE:
  // The visitor is responsible for tranversing the child elements.
  // The drawback is that we end up duplicating the tranversal code in each
  // concrete visitor. (See Design Patterns, P.339)
}

const DataElement* DataSet::operator[](std::size_t index) const {
  assert(index < elements_.size());
  return elements_[index];
}

const DataElement* DataSet::Get(Tag tag) const {
  return const_cast<DataSet*>(this)->Find(tag);
}

bool DataSet::Append(DataElement* element) {
  if (elements_.empty() || element->tag() > elements_.back()->tag()) {
    elements_.push_back(element);
    return true;
  }
  return false;
}

bool DataSet::Insert(DataElement* element) {
  auto it = LowerBound(element->tag());
  if (it != elements_.end() && (*it)->tag() == element->tag()) {
    return false;
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

bool DataSet::GetString(Tag tag, std::string* value) const {
  const DataElement* element = Get(tag);
  if (element != nullptr) {
    return element->GetString(value);
  }
  return false;
}

bool DataSet::SetString(Tag tag, const std::string& value) {
  DataElement* element = Find(tag);
  if (element != nullptr) {
    return element->SetString(value);
  }

  element = new DataElement(tag, endian_);

  if (element->SetString(value)) {
    return Insert(element);
  } else {
    delete element;
    return false;
  }
}

bool DataSet::GetInt16(Tag tag, std::int16_t* value) const {
  const DataElement* element = Get(tag);
  if (element != nullptr) {
    return element->GetInt16(value);
  }
  return false;
}

bool DataSet::GetInt32(Tag tag, std::int32_t* value) const {
  const DataElement* element = Get(tag);
  if (element != nullptr) {
    return element->GetInt32(value);
  }
  return false;
}

bool DataSet::GetUint16(Tag tag, std::uint16_t* value) const {
  const DataElement* element = Get(tag);
  if (element != nullptr) {
    return element->GetUint16(value);
  }
  return false;
}

bool DataSet::GetUint32(Tag tag, std::uint32_t* value) const {
  const DataElement* element = Get(tag);
  if (element != nullptr) {
    return element->GetUint32(value);
  }
  return false;
}

bool DataSet::GetFloat32(Tag tag, float32_t* value) const {
  const DataElement* element = Get(tag);
  if (element != nullptr) {
    return element->GetFloat32(value);
  }
  return false;
}

bool DataSet::GetFloat64(Tag tag, float64_t* value) const {
  const DataElement* element = Get(tag);
  if (element != nullptr) {
    return element->GetFloat64(value);
  }
  return false;
}

DataSet::Elements::iterator DataSet::LowerBound(Tag tag) {
  auto less = [](DataElement* lhs, Tag tag) {
    return lhs->tag() < tag;
  };

  return std::lower_bound(elements_.begin(), elements_.end(), tag, less);
}

DataElement* DataSet::Find(Tag tag) {
  auto it = LowerBound(tag);
  if (it != elements_.end() && (*it)->tag() == tag) {
    return *it;
  }
  return nullptr;
}

}  // namespace dcm
