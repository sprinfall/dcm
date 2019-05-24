#include "dcm/data_set.h"

#include <algorithm>  // for lower_bound

#include "dcm/visitor.h"

namespace dcm {

// -----------------------------------------------------------------------------

DataSet::DataSet(VR::Type vr_type, ByteOrder byte_order, Charset charset)
    : vr_type_(vr_type), byte_order_(byte_order), charset_(charset) {
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

void DataSet::ConvertByteOrder(ByteOrder byte_order) {
  if (byte_order != byte_order_) {
    for (DataElement* element : elements_) {
      // Group 0002 is always little endian, keep as it is.
      if (element->tag().group() != 2) {
        element->ConvertByteOrder(byte_order);
      }
    }
    byte_order_ = byte_order;
  }
}

// -----------------------------------------------------------------------------

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
  vr_type_ = VR::EXPLICIT;
  byte_order_ = ByteOrder::LE;

  for (DataElement* element : elements_) {
    delete element;
  }
  elements_.clear(); 
}

// -----------------------------------------------------------------------------

// Get an element by tag or return.
#define GET_OR_RETURN(ret)               \
  const DataElement* element = Get(tag); \
  if (element == nullptr) {              \
    return ret;                          \
  }

// Get an element by tag or return false.
#define GET_OR_RETURN_FALSE() GET_OR_RETURN(false)

// -----------------------------------------------------------------------------

bool DataSet::GetVL(Tag tag, std::uint32_t* vl) const {
  GET_OR_RETURN_FALSE();
  *vl = element->length();
  return true;
}

std::uint32_t DataSet::GetVL(Tag tag, std::uint32_t vl_default) const {
  GET_OR_RETURN(vl_default);
  return element->length();
}

bool DataSet::GetVM(Tag tag, std::size_t* vm) const {
  GET_OR_RETURN_FALSE();
  *vm = element->GetVM();
  return true;
}

std::size_t DataSet::GetVM(Tag tag, std::size_t vm_default) const {
  GET_OR_RETURN(vm_default);
  return element->GetVM();
}

bool DataSet::GetString(Tag tag, std::string* value) const {
  GET_OR_RETURN_FALSE();
  return element->GetString(value);
}

std::string DataSet::GetString(Tag tag) const {
  GET_OR_RETURN("");
  return element->GetString();
}

bool DataSet::SetString(Tag tag, const std::string& value) {
  return Set(tag, [&value](DataElement* element) {
    return element->SetString(value);
  });
}

bool DataSet::GetStringArray(Tag tag, std::vector<std::string>* values) const {
  GET_OR_RETURN_FALSE();
  return element->GetStringArray(values);
}

bool DataSet::SetStringArray(Tag tag, const std::vector<std::string>& values) {
  return Set(tag, [&values](DataElement* element) {
    return element->SetStringArray(values);
  });
}

bool DataSet::GetUint16(Tag tag, std::uint16_t* value) const {
  GET_OR_RETURN_FALSE();
  return element->GetUint16(value);
}

bool DataSet::SetUint16(Tag tag, std::uint16_t value) {
  return Set(tag, [&value](DataElement* element) {
    return element->SetUint16(value);
  });
}

bool DataSet::GetUint16Array(Tag tag,
                             std::vector<std::uint16_t>* values) const {
  GET_OR_RETURN_FALSE();
  return element->GetUint16Array(values);
}

bool DataSet::SetUint16Array(Tag tag,
                             const std::vector<std::uint16_t>& values) {
  return Set(tag, [&values](DataElement* element) {
    return element->SetUint16Array(values);
  });
}

bool DataSet::GetInt16(Tag tag, std::int16_t* value) const {
  GET_OR_RETURN_FALSE();
  return element->GetInt16(value);
}

bool DataSet::SetInt16(Tag tag, std::int16_t value) {
  return Set(tag, [&value](DataElement* element) {
    return element->SetInt16(value);
  });
}

bool DataSet::GetInt16Array(Tag tag, std::vector<std::int16_t>* values) const {
  GET_OR_RETURN_FALSE();
  return element->GetInt16Array(values);
}

bool DataSet::SetInt16Array(Tag tag, const std::vector<std::int16_t>& values) {
  return Set(tag, [&values](DataElement* element) {
    return element->SetInt16Array(values);
  });
}

bool DataSet::GetUint32(Tag tag, std::uint32_t* value) const {
  GET_OR_RETURN_FALSE();
  return element->GetUint32(value);
}

bool DataSet::SetUint32(Tag tag, std::uint32_t value) {
  return Set(tag, [&value](DataElement* element) {
    return element->SetUint32(value);
  });
}

bool DataSet::GetUint32Array(Tag tag,
                             std::vector<std::uint32_t>* values) const {
  GET_OR_RETURN_FALSE();
  return element->GetUint32Array(values);
}

bool DataSet::SetUint32Array(Tag tag,
                             const std::vector<std::uint32_t>& values) {
  return Set(tag, [&values](DataElement* element) {
    return element->SetUint32Array(values);
  });
}

bool DataSet::GetInt32(Tag tag, std::int32_t* value) const {
  GET_OR_RETURN_FALSE();
  return element->GetInt32(value);
}

bool DataSet::SetInt32(Tag tag, std::int32_t value) {
  return Set(tag, [&value](DataElement* element) {
    return element->SetInt32(value);
  });
}

bool DataSet::GetInt32Array(Tag tag, std::vector<std::int32_t>* values) const {
  GET_OR_RETURN_FALSE();
  return element->GetInt32Array(values);
}

bool DataSet::SetInt32Array(Tag tag, const std::vector<std::int32_t>& values) {
  return Set(tag, [&values](DataElement* element) {
    return element->SetInt32Array(values);
  });
}

bool DataSet::GetFloat32(Tag tag, float32_t* value) const {
  GET_OR_RETURN_FALSE();
  return element->GetFloat32(value);
}

bool DataSet::SetFloat32(Tag tag, float32_t value) {
  return Set(tag, [&value](DataElement* element) {
    return element->SetFloat32(value);
  });
}

bool DataSet::GetFloat32Array(Tag tag, std::vector<float32_t>* values) const {
  GET_OR_RETURN_FALSE();
  return element->GetFloat32Array(values);
}

bool DataSet::SetFloat32Array(Tag tag, const std::vector<float32_t>& values) {
  return Set(tag, [&values](DataElement* element) {
    return element->SetFloat32Array(values);
  });
}

bool DataSet::GetFloat64(Tag tag, float64_t* value) const {
  GET_OR_RETURN_FALSE();
  return element->GetFloat64(value);
}

bool DataSet::SetFloat64(Tag tag, float64_t value) {
  return Set(tag, [&value](DataElement* element) {
    return element->SetFloat64(value);
  });
}

bool DataSet::GetFloat64Array(Tag tag, std::vector<float64_t>* values) const {
  GET_OR_RETURN_FALSE();
  return element->GetFloat64Array(values);
}

bool DataSet::SetFloat64Array(Tag tag, const std::vector<float64_t>& values) {
  return Set(tag, [&values](DataElement* element) {
    return element->SetFloat64Array(values);
  });
}

// -----------------------------------------------------------------------------

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

bool DataSet::Set(Tag tag, std::function<bool(DataElement*)> setter) {
  DataElement* element = Find(tag);
  if (element != nullptr) {
    return setter(element);
  }

  element = new DataElement(tag, byte_order_);

  if (setter(element)) {
    return Insert(element);
  } else {
    delete element;
    return false;
  }
}

}  // namespace dcm
