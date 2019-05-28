#include "dcm/data_set.h"

#include <algorithm>  // for lower_bound

#include "boost/core/ignore_unused.hpp"

#include "dcm/data_sequence.h"
#include "dcm/visitor.h"

namespace dcm {

namespace {

// -----------------------------------------------------------------------------

class SetVRTypeVisitor : public Visitor {
public:
  explicit SetVRTypeVisitor(VR::Type vr_type) : vr_type_(vr_type) {
  }

  void VisitDataElement(const DataElement* data_element) override {
    boost::ignore_unused(data_element);
  }

  void VisitDataSequence(const DataSequence* data_sequence) override {
    for (std::size_t i = 0; i < data_sequence->size(); ++i) {
      data_sequence->At(i).data_set->Accept(*this);
    }
  }

  void VisitDataSet(const DataSet* data_set) override {
    const_cast<DataSet*>(data_set)->set_vr_type(vr_type_);

    for (std::size_t i = 0; i < data_set->size(); ++i) {
      data_set->At(i)->Accept(*this);
    }
  }

private:
  VR::Type vr_type_;
};

// -----------------------------------------------------------------------------

class SetByteOrderVisitor : public Visitor {
public:
  explicit SetByteOrderVisitor(ByteOrder byte_order) : byte_order_(byte_order) {
  }

  void VisitDataElement(const DataElement* data_element) override {
    const_cast<DataElement*>(data_element)->SetByteOrder(byte_order_);
  }

  void VisitDataSequence(const DataSequence* data_sequence) override {
    for (std::size_t i = 0; i < data_sequence->size(); ++i) {
      data_sequence->At(i).data_set->Accept(*this);
    }
  }

  void VisitDataSet(const DataSet* data_set) override {
    const_cast<DataSet*>(data_set)->set_byte_order(byte_order_);

    for (std::size_t i = 0; i < data_set->size(); ++i) {
      data_set->At(i)->Accept(*this);
    }
  }

private:
  ByteOrder byte_order_;
};

// -----------------------------------------------------------------------------

class UpdateSeqLengthVisitor : public Visitor {
public:
  void VisitDataElement(const DataElement* data_element) override {
    boost::ignore_unused(data_element);
  }

  void VisitDataSequence(const DataSequence* data_sequence) override {
    DataSequence* sequence = const_cast<DataSequence*>(data_sequence);

    for (std::size_t i = 0; i < sequence->size(); ++i) {
      DataSequence::Item& item = sequence->At(i);

      item.data_set->Accept(*this);

      if (item.delimitation != nullptr) {
        item.prefix->set_length(kUndefinedLength);
      } else {
        std::uint32_t length = 0;
        for (std::size_t i = 0; i < item.data_set->size(); ++i) {
          length += item.data_set->At(i)->GetElementLength(vr_type_, true);
        }
        item.prefix->set_length(length);
      }
    }

    std::uint32_t length = sequence->GetElementLength(vr_type_, true);
    std::uint32_t self_length = sequence->GetElementLength(vr_type_, false);
    sequence->set_length(length - self_length);
  }

  void VisitDataSet(const DataSet* data_set) override {
    vr_type_ = data_set->vr_type();

    for (std::size_t i = 0; i < data_set->size(); ++i) {
      data_set->At(i)->Accept(*this);
    }
  }

private:
  VR::Type vr_type_ = VR::EXPLICIT;
};

}  // namespace

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

void DataSet::SetVRType(VR::Type vr_type) {
  if (vr_type != vr_type_) {
    SetVRTypeVisitor v(vr_type);
    v.VisitDataSet(this);

    // VR type change impacts the element length.
    // The value length of the sequences and the sequence items must be updated
    // if they are not delimitated by delimitation tags.
    {
      UpdateSeqLengthVisitor v;
      Accept(v);
    }
  }
}

void DataSet::SetByteOrder(ByteOrder byte_order) {
  if (byte_order != byte_order_) {
    SetByteOrderVisitor v(byte_order);
    v.VisitDataSet(this);
  }
}

#if 0
std::uint32_t DataSet::GetGroupLength(std::uint16_t group) const {
  Tag tag(group, 0);

  auto less = [](Tag tag, DataElement* element) {
    return tag < element->tag();
  };

  auto it = std::upper_bound(elements_.begin(), elements_.end(), tag, less);

  if (it == elements_.end() || (*it)->tag().group() != group) {
    // No such group.
    return 0;
  }

  std::uint32_t group_length = 0;
  for (; it != elements_.end() && (*it)->tag().group() == group; ++it) {
    group_length += (*it)->GetElementLength(vr_type_, true);
  }

  return group_length;
}
#endif  // 0

bool DataSet::UpdateGroupLength(std::uint16_t group) {
  Tag tag(group, 0);

  auto it = LowerBound(tag);
  if (it == elements_.end() || (*it)->tag() != tag) {
    // No such group or no group length element.
    return false;
  }

  // Group length element.
  auto element = *it;
  auto vr_type = group == 2 ? VR::EXPLICIT : vr_type_;

  std::uint32_t group_length = 0;
  for (++it; it != elements_.end() && (*it)->tag().group() == group; ++it) {
    group_length += (*it)->GetElementLength(vr_type, true);
  }

  element->SetUint32(group_length);
  return true;
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
  auto less = [](DataElement* element, Tag tag) {
    return element->tag() < tag;
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
