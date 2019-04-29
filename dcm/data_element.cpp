#include "dcm/data_element.h"

#include <iostream>
#include <sstream>

#include "dcm/visitor.h"

namespace dcm {

// NOTE:
// In order to be consistent with the initial state of buffer, |length_| is
// initialized as 0 instead of kUndefinedLength.
DataElement::DataElement(Tag tag, VR::Type vr_type, Endian endian)
    : tag_(tag)
    , vr_type_(vr_type)
    , endian_(endian)
    , length_(0) {
}

void DataElement::SetBuffer(Buffer buffer, std::size_t length) {
  buffer_ = buffer;
  length_ = length;
}

// TODO: Check VR.
bool DataElement::GetString(std::string* value) const {
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

bool DataElement::GetUint16(std::uint16_t* value) const {
  if (GetNumber<std::uint16_t>(value)) {
    AdjustBytes16(value);
    return true;
  }
  return false;
}

bool DataElement::GetUint32(std::uint32_t* value) const {
  if (GetNumber<std::uint32_t>(value)) {
    AdjustBytes32(value);
    return true;
  }
  return false;
}

bool DataElement::GetInt16(std::int16_t* value) const {
  if (GetNumber<std::int16_t>(value)) {
    AdjustBytes16(value);
    return true;
  }
  return false;
}

bool DataElement::GetInt32(std::int32_t* value) const {
  if (GetNumber<std::int32_t>(value)) {
    AdjustBytes32(value);
    return true;
  }
  return false;
}


bool DataElement::GetFloat32(float32_t* value) const {
  if (GetNumber<float32_t>(value, 4)) {
    AdjustBytes32(value);
    return true;
  }
  return false;
}

bool DataElement::GetFloat64(float64_t* value) const {
  if (GetNumber<float64_t>(value, 8)) {
    AdjustBytes64(value);
    return true;
  }
  return false;
}

void DataElement::Accept(Visitor& visitor) {
  visitor.VisitDataElement(this);
}

void DataElement::PrintValue(std::ostream& os) const {
  switch (vr_type_) {
    case VR::AT:  // Attribute Tag
      // Ordered pair of 16-bit (2-byte) unsigned integers that is the value
      // of a Data Element Tag.
      // TODO
      break;

    case VR::OB:
    case VR::OW:
      os << "<Binary>";
      break;

    case VR::AE:  // Application Entity
    case VR::AS:  // Age String
    case VR::CS:  // Code String
    case VR::DS:  // Decimal String
    case VR::SH:  // Short String
    case VR::LO:  // Long String
    case VR::ST:  // Short Text
    case VR::LT:  // Long Text
    case VR::IS:  // Integer String
    case VR::UI:  // UID
    case VR::UR:  // URI/URL
    case VR::DA:  // Date
    case VR::TM:  // Time
    case VR::PN:  // Person Name
    {
      std::string str;
      GetString(&str);
      os << str;
    }
    break;

    case VR::US:  // Unsigned Short
    {
      std::uint16_t value = 0;
      GetUint16(&value);
      os << value;
    }
    break;

    case VR::SS:  // Signed Short
    {
      std::int16_t value = 0;
      GetInt16(&value);
      os << value;
    }
    break;

    case VR::UL:  // Unsigned Long
    {
      std::uint32_t value = 0;
      GetUint32(&value);
      os << value;
    }
    break;

    case VR::SL:  // Signed Long
    {
      std::int32_t value = 0;
      GetInt32(&value);
      os << value;
    }
    break;

    case VR::FL:  // Floating Point Single
    {
      float32_t value = 0.0;
      GetFloat32(&value);
      os << value;
    }
    break;

    case VR::FD:  // Floating Point Double
    {
      float64_t value = 0.0;
      GetFloat64(&value);
      os << value;
    }
    break;

    default:
      break;
  }
}

void DataElement::PrintValue(std::string* str) const {
  std::stringstream ss;
  PrintValue(ss);
  *str = ss.str();
}

void DataElement::AdjustBytes16(void* value) const {
  if (endian_ != PlatformEndian()) {
    Swap16(&value);
  }
}

void DataElement::AdjustBytes32(void* value) const {
  if (endian_ != PlatformEndian()) {
    Swap32(&value);
  }
}

void DataElement::AdjustBytes64(void* value) const {
  if (endian_ != PlatformEndian()) {
    Swap64(&value);
  }
}

std::ostream& operator<<(std::ostream& os, const DataElement& element) {
  os << element.tag() << "\t" << VR::ToString(element.vr_type());

  if (element.vr_type() == VR::SQ) {
    os << "*";
  }

  os << "\t";

  if (element.length() != kUndefinedLength) {
    os << element.length();
  } else {
    os << "-1";
  }

  os << "\t";
  element.PrintValue(os);

  return os;
}

}  // namespace dcm
