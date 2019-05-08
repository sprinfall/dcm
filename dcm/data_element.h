#ifndef DCM_DATA_ELEMENT_H_
#define DCM_DATA_ELEMENT_H_

#include <cassert>
#include <cstdint>
#include <iosfwd>
#include <vector>

#include "dcm/defs.h"

namespace dcm {

using Buffer = std::vector<char>;

bool CheckStringValue(VR vr, const std::string& value);

class Visitor;

class DataElement {
public:
  DataElement(Tag tag, VR vr, ByteOrder byte_order = ByteOrder::LE);

  // VR will be queried from the data dictionary.
  DataElement(Tag tag, ByteOrder byte_order = ByteOrder::LE);

  virtual ~DataElement() = default;

  virtual void Accept(Visitor& visitor) const;

  Tag tag() const { return tag_; }

  VR vr() const { return vr_; }

  std::size_t length() const { return length_; }
  void set_length(std::size_t length) { length_ = length; }

  // Get raw value buffer.
  const Buffer& buffer() const { return buffer_; }

  // Set raw value buffer.
  // The buffer size must be even: 2, 4, 8, ...
  bool SetBuffer(Buffer&& buffer);

  // TODO: Add applicable VR types as comments.
  bool GetString(std::string* value) const;

  bool SetString(const std::string& value);

  bool GetWString(std::wstring* value) const;

  bool GetUint16(std::uint16_t* value) const;

  bool GetUint32(std::uint32_t* value) const;

  bool GetInt16(std::int16_t* value) const;

  bool GetInt32(std::int32_t* value) const;

  bool GetFloat32(float32_t* value) const;

  bool GetFloat64(float64_t* value) const;

  // Print data element to an output stream.
  void Print(std::ostream& os) const;

  // Print value to an output stream.
  void PrintValue(std::ostream& os) const;

  // Print value to a string.
  std::string PrintValue() const;

protected:
  // Get number value.
  template <typename T>
  bool GetNumber(T* value) const {
    return GetNumber<T>(value, sizeof(T));
  }

  // Get number value.
  template <typename T>
  bool GetNumber(T* value, std::size_t length) const {
    if (length_ == length) {
      *value = *reinterpret_cast<const T*>(&buffer_[0]);
      return true;
    }
    return false;
  }

  void AdjustBytes16(void* value) const;
  void AdjustBytes32(void* value) const;
  void AdjustBytes64(void* value) const;

protected:
  // Tag key.
  Tag tag_;

  // Value Representation.
  VR vr_;

  // Little endian or big endian.
  ByteOrder byte_order_;

  // Value length.
  // Undefined length for SQ element is 0xFFFFFFFF.
  // Identical to the buffer size if the buffer is not empty.
  std::size_t length_;

  // Raw buffer (i.e., bytes) of the value.
  Buffer buffer_;
};

}  // namespace dcm

#endif  // DCM_DATA_ELEMENT_H_
