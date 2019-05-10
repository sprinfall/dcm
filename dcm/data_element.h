#ifndef DCM_DATA_ELEMENT_H_
#define DCM_DATA_ELEMENT_H_

#include <cassert>
#include <cstdint>
#include <iosfwd>
#include <vector>

#include "dcm/defs.h"

namespace dcm {

using Buffer = std::vector<char>;

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

  // ---------------------------------------------------------------------------

  // Get and set values

  // AE, AS, CS, DA, TM, DT, DS, IS, LO, ST, LT, UT, PN, SH, UC, UI, UR.

  bool GetString(std::string* value) const;

  bool SetString(const std::string& value);

  // US (Unsigned Short)

  bool GetUint16(std::uint16_t* value) const {
    return GetNumber(VR::US, 2, value);
  }

  bool SetUint16(std::uint16_t value) {
    return SetNumber(VR::US, 2, &value);
  }

  // UL (Unsigned Long)

  bool GetUint32(std::uint32_t* value) const {
    return GetNumber(VR::UL, 4, value);
  }

  bool SetUint32(std::uint32_t value) {
    return SetNumber(VR::UL, 4, &value);
  }

  // SS (Signed Short)

  bool GetInt16(std::int16_t* value) const {
    return GetNumber(VR::SS, 2, value);
  }

  bool SetInt16(std::int16_t value) {
    return SetNumber(VR::SS, 2, &value);
  }

  // SL (Signed Long)

  bool GetInt32(std::int32_t* value) const {
    return GetNumber(VR::SL, 4, value);
  }

  bool SetInt32(std::int32_t value) {
    return SetNumber(VR::SL, 4, &value);
  }

  // FL (Floating Point Single)

  bool GetFloat32(float32_t* value) const {
    return GetNumber(VR::FL, 4, value);
  }

  bool SetFloat32(float32_t value) {
    return SetNumber(VR::FL, 4, &value);
  }

  // FD (Floating Point Double)

  bool GetFloat64(float64_t* value) const {
    return GetNumber(VR::FD, 8, value);
  }

  bool SetFloat64(float64_t value) {
    return SetNumber(VR::FD, 8, &value);
  }

  // TODO: OD, OF, OL, OW

  // Print data element to an output stream.
  // TODO: Remove
  void Print(std::ostream& os) const;

  // Print value to an output stream.
  // TODO: Remove
  void PrintValue(std::ostream& os) const;

  // Print value to a string.
  // TODO: Remove
  std::string PrintValue() const;

protected:
  bool GetNumber(VR vr, size_t size, void* value) const;
  bool SetNumber(VR vr, size_t size, void* value);

  void GetBytes(void* value, std::size_t length) const;
  void SetBytes(void* value, std::size_t length);

  void AdjustBytes(void* value, std::size_t size) const;

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
