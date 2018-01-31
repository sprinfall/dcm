#ifndef DCMLITE_DATA_ELEMENT_H_
#define DCMLITE_DATA_ELEMENT_H_
#pragma once

#include <cstdint>
#include <iosfwd>

#include "boost/shared_array.hpp"

#include "dcmlite/tag.h"
#include "dcmlite/vr.h"

namespace dcmlite {

typedef boost::shared_array<char> Buffer;

class Visitor;

class DataElement {
public:
  DataElement(const Tag& tag, VR::Type vr_type, Endian endian);

  virtual ~DataElement();

  const Tag& tag() const {
    return tag_;
  }

  // NOTE: VR is not allowed to change after construction.
  VR::Type vr_type() const {
    return vr_type_;
  }

  // Get the value length.
  std::size_t length() const {
    return length_;
  }

  // NOTE: SetBuffer will also set length.
  void set_length(std::size_t length) {
    length_ = length;
  }

  // Get the value buffer.
  // NOTE: No set_buffer() is provided; see SetBuffer().
  const Buffer& buffer() const {
    return buffer_;
  }

  // Set value buffer and length together.
  // The length must be even (2, 4, 8, etc.).
  // Always set buffer and length together to ensure data consistency.
  void SetBuffer(Buffer buffer, std::size_t length);

  // TODO: Add applicable VR types as comments.
  bool GetString(std::string* value) const;

  bool GetUint16(std::uint16_t* value) const;

  bool GetUint32(std::uint32_t* value) const;

  bool GetInt16(std::int16_t* value) const;

  bool GetInt32(std::int32_t* value) const;

  bool GetFloat32(float32_t* value) const;

  bool GetFloat64(float64_t* value) const;

  // Print value to an output stream.
  void PrintValue(std::ostream& os) const;

  // Print value to a string.
  void PrintValue(std::string* str) const;

  // Visitor design pattern.
  virtual void Accept(Visitor& visitor);

protected:
  // Get number value.
  template <typename T>
  bool GetNumber(T* value) const {
    return GetNumber<T>(value, sizeof(T));
  }

  // Get number value.
  template <typename T>
  bool GetNumber(T* value, std::size_t length) const {
    if (buffer_ && length_ == length) {
      *value = *reinterpret_cast<T*>(buffer_.get());
      return true;
    }
    return false;
  }

  void AdjustBytes16(void* value) const;
  void AdjustBytes32(void* value) const;
  void AdjustBytes64(void* value) const;

protected:
  Tag tag_;
  VR::Type vr_type_;

  // Big endian or little endian.
  Endian endian_;

  // Value length.
  // Undefined length for SQ element is 0xFFFFFFFF.
  std::size_t length_;

  // Value buffer.
  Buffer buffer_;
};

std::ostream& operator<<(std::ostream& os, const DataElement& element);

}  // namespace dcmlite

#endif  // DCMLITE_DATA_ELEMENT_H_
