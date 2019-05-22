#include "dcm/data_element.h"

#include <cassert>
#include <cctype>
#include <cstring>  // std::memcpy

#include "boost/algorithm/string.hpp"

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

// -----------------------------------------------------------------------------

// Application Entity
bool CheckAE(const std::string& value) {
  // Max: 16

  if (value.empty() || value.size() > 16) {
    return false;
  }

  return true;
}

// Age String
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

// Code String
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

// Date
bool CheckDA(const std::string& value) {
  // Fix: 8
  // YYYYMMDD; 0-9.
  // Example: "20050822"

  if (value.size() != 8) {
    return false;
  }

  for (char c : value) {
    if (!IsDigit(c)) {
      return false;
    }
  }

  // TODO: Check the range.

  return true;
}

// Time
bool CheckTM(const std::string& value) {
  // Max: 16
  // A string of characters of the format HHMMSS.FRAC;
  // Example: "183200.00"
  // Allowed characters: 0-9, and period(.)

  if (value.size() > 16) {
    return false;
  }

  // TODO

  return true;
}

// Date Time
bool CheckDT(const std::string& value) {
  // Max: 26
  // Concatenated datetime string in the format: YYYYMMDDHHMMSS.FFFFFF&ZZXX
  // Example: "20050812183000.00"
  // Allowed characters: 0-9, plus(+), minus(-) and period(.)

  if (value.size() > 26) {
    return false;
  }

  // TODO

  return true;
}

// Decimal String
bool CheckDS(const std::string& value) {
  // Max: 16
  // A string of characters representing either a fixed point number or a
  // floating point number.
  // Example: "12345.67", "-5.0e3"
  // Allowed characters: 0-9, plus(+), minus(-), E, e, and period(.)

  if (value.size() > 16) {
    return false;
  }

  // TODO: Check more.

  return true;
}

// Integer String
bool CheckIS(const std::string& value) {
  // Max: 12
  // A string of characters representing an Integer in base-10 (decimal), shall
  // contain only the characters 0 - 9, with an optional leading "+"or "-".

  if (value.size() > 12) {
    return false;
  }

  std::size_t i = 0;

  if (value.size() > 1) {
    if (value[0] == '+' || value[0] == '-') {
      ++i;
    }
  }

  for (; i < value.size(); ++i) {
    if (!IsDigit(value[i])) {
      return false;
    }
  }

  return true;
}

// Long String
bool CheckLO(const std::string& value) {
  // Max: 64

  if (value.size() > 64) {
    return false;
  }

  return true;
}

// Short Text
bool CheckST(const std::string& value) {
  // Max: 1024
  // A character string that may contain one or more paragraphs.

  if (value.size() > 1024) {
    return false;
  }

  return true;
}

// Long Text
bool CheckLT(const std::string& value) {
  // Max: 10240

  if (value.size() > 10240) {
    return false;
  }

  return true;
}

// Unlimited Text
bool CheckUT(const std::string& value) {
  // Max: 4,294,967,294 (2^32 - 2)

  return true;
}

// Person Name
bool CheckPN(const std::string& value) {
  // Max: 64
  // TODO: Max length of PN is 3*64+2 = 194 characters (not bytes!)

  if (value.size() > 64) {
    return false;
  }

  // TODO

  return true;
}

// Short String
bool CheckSH(const std::string& value) {
  // Max: 16
  // Example: telephone numbers, IDs

  if (value.size() > 16) {
    return false;
  }

  return true;
}

// Unlimited Characters
// TODO
bool CheckUC(const std::string& value) {
  return true;
}

// Unique Identifier(UID)
bool CheckUI(const std::string& value) {
  // Max: 64
  // A character string containing a UID that is used to uniquely identify a
  // wide variety of items.
  // Example: "1.2.840.10008.1.1"

  if (value.size() > 64) {
    return false;
  }

  // TODO

  return true;
}

// Universal Resource Identifier
bool CheckUR(const std::string& value) {
  // Max: 64
  // A string of characters that identifies a URI or a URL as defined in
  // [RFC3986]. Leading spaces are not allowed. Trailing spaces shallbe ignored.
  // Data Elements with this VR shall not be multi-valued.
  // Example: "1.2.840.10008.1.1"

  // TODO

  return true;
}

// -----------------------------------------------------------------------------

bool CheckStringValue(VR vr, const std::string& value) {
  switch (vr.code()) {
    case VR::AE:
      return CheckAE(value);

    case VR::AS:
      return CheckAS(value);

    case VR::CS:
      return CheckCS(value);

    case VR::DA:
      return CheckDA(value);

    case VR::TM:
      return CheckTM(value);

    case VR::DT:
      return CheckDT(value);

    case VR::DS:
      return CheckDS(value);

    case VR::IS:
      return CheckIS(value);

    case VR::LO:
      return CheckLO(value);

    case VR::ST:
      return CheckST(value);

    case VR::LT:
      return CheckLT(value);

    case VR::UT:
      return CheckUT(value);

    case VR::PN:
      return CheckPN(value);

    case VR::SH:
      return CheckSH(value);

    case VR::UC:
      return CheckUC(value);

    case VR::UI:
      return CheckUI(value);

    case VR::UR:
      return CheckUR(value);

    default:
      break;
  }

  return false;
}

}  // namespace

// -----------------------------------------------------------------------------

DataElement::DataElement(Tag tag, ByteOrder byte_order)
    : tag_(tag), byte_order_(byte_order), length_(0) {
  vr_ = dict::GetVR(tag);
}

DataElement::DataElement(Tag tag, VR vr, ByteOrder byte_order)
    : tag_(tag), vr_(vr), byte_order_(byte_order), length_(0) {
}

void DataElement::Accept(Visitor& visitor) const {
  visitor.VisitDataElement(this);
}

// TODO: Add trailing space or NULL byte if necessary.
bool DataElement::SetBuffer(Buffer&& buffer) {
  if (buffer.size() % 2 != 0) {
    return false;
  }

  length_ = buffer.size();  // NOTE: Before std::move()!
  buffer_ = std::move(buffer);

  return true;
}

// -----------------------------------------------------------------------------

std::size_t DataElement::GetVM() const {
  const VR::Code code = vr_.code();

  if (code == VR::OF || code == VR::OD || code == VR::OB || code == VR::OW ||
      code == VR::OL) {
    return 1;
  }

  if (code == VR::US || code == VR::SS) {
    return length_ / 2;
  }

  if (code == VR::UL || code == VR::SL || code == VR::FL || code == VR::AT) {
    return length_ / 4;
  }

  if (code == VR::FD) {
    return length_ / 8;
  }

  if (code == VR::ST || code == VR::LT || code == VR::UT || code == VR::UR) {
    return length_ > 0 ? 1 : 0;
  }

  if (vr_.IsBackSlashVM()) {
    // AE, AS, UI, etc.
    if (buffer_.empty()) {
      return 0;
    }
    std::size_t vm = 1;
    for (std::size_t i = 0; i < buffer_.size(); ++i) {
      if (buffer_[i] == '\\') {
        ++vm;
      }
    }
    return vm;
  }

  return 1;  // SQ, UN
}

// -----------------------------------------------------------------------------
// String

bool DataElement::GetString(std::string* value) const {
  if (!vr_.IsString()) {
    return false;
  }

  if (buffer_.empty()) {
    value->clear();
    return true;
  }

  std::size_t size = buffer_.size();

  if (size > 0) {
    // Remove blank trailing space (or NULL byte for UI).
    if (vr_ == VR::UI) {
      if (buffer_.back() == '\0') {
        --size;
      }
    } else {
      if (buffer_.back() == ' ') {
        --size;
      }
    }
  }

  value->assign(&buffer_[0], size);

  return true;
}

std::string DataElement::GetString() const {
  std::string value;
  GetString(&value);
  return value;
}

bool DataElement::GetStringArray(std::vector<std::string>* values) const {
  std::string value;
  if (!GetString(&value)) {
    return false;
  }

  if (!vr_.IsBackSlashVM()) {  // TODO: Just return false?
    // TODO: What if value is empty? Return false?
    values->push_back(std::move(value));
    return true;
  }

  boost::split(*values, value, boost::is_any_of("\\"));
  return true;
}

std::vector<std::string> DataElement::GetStringArray() const {
  std::vector<std::string> values;
  GetStringArray(&values);
  return values;
}

// TODO: Split by "\\", check each value separately.
bool DataElement::SetString(const std::string& value) {
  if (!CheckStringValue(vr_, value)) {
    return false;
  }

  DoSetString(value);

  return true;
}

bool DataElement::SetStringArray(const std::vector<std::string>& values) {
  if (!vr_.IsBackSlashVM()) {
    return false;
  }

  if (!dict::CheckVM(tag_, values.size())) {
    return false;
  }

  // Check each value separately.
  for (const std::string& value : values) {
    if (!CheckStringValue(vr_, value)) {
      return false;
    }
  }

  std::string joined_value = boost::join(values, "\\");
  DoSetString(joined_value);

  return true;
}

// -----------------------------------------------------------------------------

void DataElement::DoSetString(const std::string& value) {
  const bool odd = value.size() % 2 == 1;

  length_ = value.size();

  if (odd) {
    // +1 for blank trailing space.
    length_ += 1;
  }

  buffer_.resize(length_);

  if (length_ > 0) {
    std::memcpy(&buffer_[0], value.data(), value.size());
  }

  if (odd) {
    // Add blank trailing space (or NULL byte for UI).
    if (vr_ == VR::UI) {
      buffer_[length_ - 1] = '\0';
    } else {
      buffer_[length_ - 1] = ' ';
    }
  }
}

bool DataElement::GetNumber(VR vr, std::size_t size, void* value) const {
  if (vr_ != vr) {
    return false;
  }

  // length_ > size when VM > 1.
  assert(length_ >= size);

  std::memcpy(value, &buffer_[0], size);

  if (byte_order_ != kByteOrderOS) {
    SwapBytes(value, size);
  }

  return true;
}

bool DataElement::SetNumber(VR vr, std::size_t size, void* value) {
  if (vr_ != vr) {
    return false;
  }

  if (!dict::CheckVM(tag_, 1)) {
    return false;
  }

  length_ = size;
  buffer_.resize(size);

  std::memcpy(&buffer_[0], value, size);

  if (byte_order_ != kByteOrderOS) {
    SwapBytes(&buffer_[0], size);
  }

  return true;
}

bool DataElement::GetNumberArray(VR vr, std::size_t size, std::size_t count,
                                 void* values) const {
  if (vr != vr_) {
    return false;
  }

  assert(length_ == size * count);

  std::memcpy(values, &buffer_[0], length_);

  if (byte_order_ != kByteOrderOS) {
    char* dst = reinterpret_cast<char*>(values);
    for (std::size_t i = 0; i < count; ++i, dst += size) {
      SwapBytes(dst, size);
    }
  }

  return true;
}

bool DataElement::SetNumberArray(VR vr, std::size_t size, std::size_t count,
                                 const void* values) {
  if (vr != vr_) {
    return false;
  }

  if (!dict::CheckVM(tag_, count)) {
    return false;
  }

  buffer_.resize(size * count);

  char* dst = &buffer_[0];

  std::memcpy(dst, values, buffer_.size());

  if (byte_order_ != kByteOrderOS) {
    for (std::size_t i = 0; i < count; ++i, dst += size) {
      SwapBytes(dst, size);
    }
  }

  return true;
}

void DataElement::SwapBytes(void* value, std::size_t size) const {
  if (size == 2) {
    Swap16(value);
  } else if (size == 4) {
    Swap32(value);
  } else if (size == 8) {
    Swap64(value);
  }
}

}  // namespace dcm
