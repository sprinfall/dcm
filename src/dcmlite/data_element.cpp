#include "dcmlite/data_element.h"

namespace dcmlite {

// NOTE:
// In order to be consistent with the initial state of buffer, length_ is
// initialized as 0 instead of kUndefinedLength.
DataElement::DataElement(const Tag& tag, VR::Type vr_type)
    : tag_(tag), vr_type_(vr_type), length_(0) {
}

DataElement::~DataElement() {
}

void DataElement::SetBuffer(boost::shared_array<char> buffer, size_t length) {
  buffer_ = buffer;
  length_ = length;
}

bool DataElement::AsString(std::string* value) const {
  if (buffer_ && length_ > 0) {
    if (buffer_[length_ - 1] == ' ') {
      // Remove the padding space.
      value->assign(buffer_.get(), length_ - 1);
    } else {
      value->assign(buffer_.get(), length_);
    }
    return true;
  }
  return false;
}

bool DataElement::AsUint16(std::uint16_t* value) const {
  if (buffer_ && length_ == 2) {
    *value = *(std::uint16_t*)buffer_.get();
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
