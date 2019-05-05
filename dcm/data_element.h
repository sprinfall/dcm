#ifndef DCM_DATA_ELEMENT_H_
#define DCM_DATA_ELEMENT_H_

#include <cassert>
#include <cstdint>
#include <iosfwd>
#include <vector>

#include "dcm/defs.h"
#include "dcm/tag.h"

namespace dcm {

// TODO: char -> std::uint8
using Buffer = std::vector<char>;

bool CheckStringValue(VR vr, const std::string& value);

class Visitor;

class DataElement {
public:
  DataElement(Tag tag, VR vr, Endian endian = Endian::Little());

  // VR will be queried from the data dictionary.
  DataElement(Tag tag, Endian endian = Endian::Little());

  virtual ~DataElement() = default;

  virtual void Accept(Visitor& visitor) const;

  Tag tag() const { return tag_; }

  VR vr() const { return vr_; }

  std::size_t length() const { return length_; }

  // TODO: Only open to DataSet.
  void set_length(std::size_t length) { length_ = length; }

  // Get the value buffer.
  const Buffer& buffer() const { return buffer_; }

  // Set value buffer and length.
  // The |buffer| will be moved to avoid copy cost.
  // The |length| must be even (2, 4, 8, etc.).
  // Always set buffer and length together to ensure data consistency.
  // TODO: Remove |length| parameter.
  void set_buffer(Buffer&& buffer, std::size_t length) {
    assert(length % 2 == 0);
    buffer_ = std::move(buffer);
    length_ = length;
  }

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
  void PrintValue(std::string* str) const;

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

  // Little or big endian.
  Endian endian_;

  // Value length.
  // Undefined length for SQ element is 0xFFFFFFFF.
  // TODO: Move to DataSet.
  std::size_t length_;

  // Raw buffer (i.e., bytes) of the value.
  Buffer buffer_;
};

}  // namespace dcm

#endif  // DCM_DATA_ELEMENT_H_
