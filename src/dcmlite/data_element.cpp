#include "dcmlite/data_element.h"

namespace dcmlite {

DataElement::DataElement(const Tag& tag, VR::Type vr_type)
    : tag_(tag), vr_type_(vr_type), length_(kUndefinedLength) {
}

DataElement::~DataElement() {
}

void DataElement::SetBuffer(boost::shared_array<char> buffer, size_t length) {
  buffer_ = buffer;
  length_ = length;
}

bool DataElement::AsString(std::string* value) const {
  if (buffer_ && length_ > 0) {
    value->assign(buffer_.get(), length_);
    return true;
  }
  return false;
}

bool DataElement::AsUint32(std::uint32_t* value) const {
  if (buffer_ && length_ == 4) {
    *value = *(std::uint32_t*)buffer_.get();
    return true;
  }
  return false;
}

}  // namespace dcmlite
