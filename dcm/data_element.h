#ifndef DCM_DATA_ELEMENT_H_
#define DCM_DATA_ELEMENT_H_

#include <cassert>
#include <cstdint>
#include <cstring>  // std::memcpy
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

  // Get value length.
  std::size_t GetVL() const {
    return length_;
  }

  // Get value multiplicity.
  std::size_t GetVM() const;

  // ---------------------------------------------------------------------------

  // Get string value.
  // Return false if the VR is not a string (see VR::IsString).
  bool GetString(std::string* value) const;

  // Get string multiple values.
  // Split the original string by back slash (\).
  // Return false if the VR is not a string (see VR::IsString).
  bool GetStringArray(std::vector<std::string>* values) const;

  bool SetString(const std::string& value);

  bool SetStringArray(const std::vector<std::string>& values);

  // US (Unsigned Short)

  bool GetUint16(std::uint16_t* value) const {
    return GetNumber(VR::US, 2, value);
  }

  bool SetUint16(std::uint16_t value) {
    return SetNumber(VR::US, 2, &value);
  }

  bool GetUint16Array(std::vector<std::uint16_t>* values) const {
    const std::size_t count = GetVM();
    values->resize(count);
    return GetNumberArray(VR::US, 2, count, &(*values)[0]);
  }
  
  bool SetUint16Array(const std::vector<std::uint16_t>& values) {
    return SetNumberArray(VR::US, 2, values.size(), &values[0]);
  }

  // SS (Signed Short)

  bool GetInt16(std::int16_t* value) const {
    return GetNumber(VR::SS, 2, value);
  }

  bool SetInt16(std::int16_t value) {
    return SetNumber(VR::SS, 2, &value);
  }

  bool GetInt16Array(std::vector<std::int16_t>* values) const {
    const std::size_t count = GetVM();
    values->resize(count);
    return GetNumberArray(VR::SS, 2, count, &(*values)[0]);
  }

  bool SetInt16Array(const std::vector<std::int16_t>& values) {
    return SetNumberArray(VR::SS, 2, values.size(), &values[0]);
  }

  // UL (Unsigned Long)

  bool GetUint32(std::uint32_t* value) const {
    return GetNumber(VR::UL, 4, value);
  }

  bool SetUint32(std::uint32_t value) {
    return SetNumber(VR::UL, 4, &value);
  }

  bool GetUint32Array(std::vector<std::uint32_t>* values) const {
    std::size_t count = GetVM();
    values->resize(count);
    return GetNumberArray(VR::UL, 4, count, &(*values)[0]);
  }

  bool SetUint32Array(const std::vector<std::uint32_t>& values) {
    return SetNumberArray(VR::UL, 4, values.size(), &values[0]);
  }

  // SL (Signed Long)

  bool GetInt32(std::int32_t* value) const {
    return GetNumber(VR::SL, 4, value);
  }

  bool SetInt32(std::int32_t value) {
    return SetNumber(VR::SL, 4, &value);
  }

  bool GetInt32Array(std::vector<std::int32_t>* values) const {
    std::size_t count = GetVM();
    values->resize(count);
    return GetNumberArray(VR::SL, 4, count, &(*values)[0]);
  }

  bool SetInt32Array(const std::vector<std::int32_t>& values) {
    return SetNumberArray(VR::SL, 4, values.size(), &values[0]);
  }

  // FL (Floating Point Single)

  bool GetFloat32(float32_t* value) const {
    return GetNumber(VR::FL, 4, value);
  }

  bool SetFloat32(float32_t value) {
    return SetNumber(VR::FL, 4, &value);
  }

  bool GetFloat32Array(std::vector<float32_t>* values) const {
    std::size_t count = GetVM();
    values->resize(count);
    return GetNumberArray(VR::FL, 4, count, &(*values)[0]);
  }

  bool SetFloat32Array(const std::vector<float32_t>& values) {
    return SetNumberArray(VR::FL, 4, values.size(), &values[0]);
  }

  // FD (Floating Point Double)

  bool GetFloat64(float64_t* value) const {
    return GetNumber(VR::FD, 8, value);
  }

  bool SetFloat64(float64_t value) {
    return SetNumber(VR::FD, 8, &value);
  }

  bool GetFloat64Array(std::vector<float64_t>* values) const {
    std::size_t count = GetVM();
    values->resize(count);
    return GetNumberArray(VR::FD, 8, count, &(*values)[0]);
  }

  bool SetFloat64Array(const std::vector<float64_t>& values) {
    return SetNumberArray(VR::FD, 8, values.size(), &values[0]);
  }

private:
  bool GetNumber(VR vr, std::size_t size, void* value) const;

  bool SetNumber(VR vr, std::size_t size, void* value);

  bool GetNumberArray(VR vr, std::size_t size, std::size_t count,
                      void* values) const;

  bool SetNumberArray(VR vr, std::size_t size, std::size_t count,
                      const void* values);

  void SwapBytes(void* value, std::size_t size) const;

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

private:
  // Raw buffer (i.e., bytes) of the value.
  Buffer buffer_;
};

}  // namespace dcm

#endif  // DCM_DATA_ELEMENT_H_
