#include "dcm/data_element.h"

#include <cctype>
#include <iostream>
#include <sstream>

#include "dcm/data_dict.h"
#include "dcm/util.h"
#include "dcm/visitor.h"

namespace dcm {

// -----------------------------------------------------------------------------

namespace {

// The behavior of `std::isalpha` and other functions from <cctype> is undefined
// if the argument's value is neither representable as `unsigned char` nor equal
// to `EOF`. To use these functions safely with plain `chars` (or `signed chars`
// ), the argument should first be converted to `unsigned char`.

// See: https://en.cppreference.com/w/cpp/string/byte/isalpha

inline bool IsAlpha(char ch) {
  return std::isalpha(static_cast<unsigned char>(ch)) != 0;
}

inline bool IsUpper(char ch) {
  return std::isupper(static_cast<unsigned char>(ch)) != 0;
}

inline bool IsDigit(char ch) {
  return std::isdigit(static_cast<unsigned char>(ch)) != 0;
}

bool CheckAE(const std::string& value) {
  // Max: 16

  if (value.size() > 16) {
    return false;
  }

  return true;
}

bool CheckAS(const std::string& value) {
  // Fix: 4
  // Format: nnnD, nnnW, nnnM, nnnY.
  // E.g., "018M".

  if (value.size() != 4) {
    return false;
  }

  for (std::size_t i = 0; i < 3; ++i) {
    if (!IsDigit(value[i])) {
      return false;
    }
  }

  if (value[3] != 'D' || value[3] != 'W' || value[3] != 'M' ||
      value[3] != 'Y') {
    return false;
  }

  return true;
}

bool CheckCS(const std::string& value) {
  // Max: 16
  // Uppercase characters, 0-9, SPACE, underscore (_).
  // E.g., "CD123_4"

  if (value.size() > 16) {
    return false;
  }

  auto is_valid = [](char c) {
    return (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == ' ' ||
      c == '_';
  };

  for (char c : value) {
    if (!is_valid(c)) {
      return false;
    }
  }
  return true;
}

bool CheckLT(const std::string& value) {
  // Max: 10240

  if (value.size() > 10240) {
    return false;
  }

  return true;
}

bool CheckStringValue(VR vr, const std::string& value) {
  switch (vr) {
    case VR::AE:
      return CheckAE(value);

    case VR::AS:
      return CheckAS(value);

    case VR::CS:
      return CheckCS(value);

    case VR::LT:
      return CheckLT(value);

    default:
      break;
  }

  return false;
}

}  // namespace

// -----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const DataElement& element) {
  os << element.tag() << "\t" << VRToString(element.vr());

  if (element.vr() == VR::SQ) {
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

// -----------------------------------------------------------------------------

// NOTE:
// In order to be consistent with the initial state of buffer, |length_| is
// initialized as 0 instead of kUndefinedLength.
DataElement::DataElement(Tag tag, VR vr, Endian endian)
    : tag_(tag), vr_(vr), endian_(endian), length_(0) {
}

DataElement::DataElement(Tag tag, Endian endian)
    : tag_(tag), endian_(endian), length_(0) {
  vr_ = DataDict::GetVR(tag);
}

void DataElement::Accept(Visitor& visitor) {
  visitor.VisitDataElement(this);
}

// TODO: Check VR.
bool DataElement::GetString(std::string* value) const {
  if (buffer_.empty()) {
    *value = "";
    return true;
  }

  if (buffer_.back() == ' ') {  // Padding space
    value->assign(&buffer_[0], buffer_.size() - 1);
  } else {
    value->assign(&buffer_[0], buffer_.size());
  }
  return true;
}

bool DataElement::SetString(const std::string& value) {
  if (!CheckStringValue(vr_, value)) {
    return false;
  }

  bool padding = (value.size() % 2 == 1);

  length_ = value.size();
  if (padding) {
    length_ += 1;
  }

  buffer_.resize(length_);

  if (length_ > 0) {
    std::memcpy(&buffer_[0], value.data(), value.size());
  }

  if (padding) {
    buffer_[length_ - 1] = ' ';
  }

  return true;
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

// TODO: Format/Indentation
void DataElement::PrintValue(std::ostream& os) const {
  switch (vr_) {
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

}  // namespace dcm
