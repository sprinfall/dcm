#ifndef DCM_DATA_ELEMENT_H_
#define DCM_DATA_ELEMENT_H_

#include <cassert>
#include <cstdint>
#include <vector>

#include "dcm/defs.h"

namespace dcm {

using Buffer = std::vector<char>;

class Visitor;

class DataElement {
public:
  // VR will be queried from the data dictionary.
  DataElement(Tag tag, ByteOrder byte_order = ByteOrder::LE);

  DataElement(Tag tag, VR vr, ByteOrder byte_order = ByteOrder::LE);

  virtual ~DataElement() = default;

  virtual void Accept(Visitor& visitor) const;

  // ---------------------------------------------------------------------------

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

  // Get value multiplicity.
  std::size_t GetVM() const;

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

  bool GetMultiUint16(std::vector<std::uint16_t>* values) const {
    return GetMultiNumbers(VR::US, values);
  }

  // SS (Signed Short)

  bool GetInt16(std::int16_t* value) const {
    return GetNumber(VR::SS, 2, value);
  }

  bool SetInt16(std::int16_t value) {
    return SetNumber(VR::SS, 2, &value);
  }

  bool GetMultiInt16(std::vector<std::int16_t>* values) const {
    return GetMultiNumbers(VR::SS, values);
  }

  // UL (Unsigned Long)

  bool GetUint32(std::uint32_t* value) const {
    return GetNumber(VR::UL, 4, value);
  }

  bool SetUint32(std::uint32_t value) {
    return SetNumber(VR::UL, 4, &value);
  }

  bool GetMultiUint32(std::vector<std::uint32_t>* values) const {
    return GetMultiNumbers(VR::UL, values);
  }

  // SL (Signed Long)

  bool GetInt32(std::int32_t* value) const {
    return GetNumber(VR::SL, 4, value);
  }

  bool SetInt32(std::int32_t value) {
    return SetNumber(VR::SL, 4, &value);
  }

  bool GetMultiInt32(std::vector<std::int32_t>* values) const {
    return GetMultiNumbers(VR::SL, values);
  }

  // FL (Floating Point Single)

  bool GetFloat32(float32_t* value) const {
    return GetNumber(VR::FL, 4, value);
  }

  bool SetFloat32(float32_t value) {
    return SetNumber(VR::FL, 4, &value);
  }

  bool GetMultiFloat32(std::vector<float32_t>* values) const {
    return GetMultiNumbers(VR::FL, values);
  }

  // FD (Floating Point Double)

  bool GetFloat64(float64_t* value) const {
    return GetNumber(VR::FD, 8, value);
  }

  bool SetFloat64(float64_t value) {
    return SetNumber(VR::FD, 8, &value);
  }

  bool GetMultiFloat64(std::vector<float64_t>* values) const {
    return GetMultiNumbers(VR::FD, values);
  }

  // TODO: OD, OF, OL, OW

private:
  bool GetNumber(VR vr, size_t size, void* value) const;
  bool SetNumber(VR vr, size_t size, void* value);

  template <typename T>
  bool GetMultiNumbers(VR vr, std::vector<T>* values) const {
    if (vr != vr_) {
      return false;
    }

    const std::size_t vm = GetVM();
    values->resize(vm);

    const std::size_t size = sizeof(T);

    const char* src = &buffer_[0];
    T* dst = &(*values)[0];

    for (std::size_t i = 0; i < vm; ++i, src += size, ++dst) {
      std::memcpy(dst, src, size);
      AdjustBytes(dst, size);
    }

    return true;
  }

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
