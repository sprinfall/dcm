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
    return GetNumber(VR::US, value);
  }

  bool SetUint16(std::uint16_t value) {
    return SetNumber(VR::US, value);
  }

  bool GetUint16Array(std::vector<std::uint16_t>* values) const {
    return GetNumberArray(VR::US, values);
  }
  
  bool SetUint16Array(const std::vector<std::uint16_t>& values) {
    return SetNumberArray(VR::US, values);
  }

  // SS (Signed Short)

  bool GetInt16(std::int16_t* value) const {
    return GetNumber(VR::SS, value);
  }

  bool SetInt16(std::int16_t value) {
    return SetNumber(VR::SS, value);
  }

  bool GetInt16Array(std::vector<std::int16_t>* values) const {
    return GetNumberArray(VR::SS, values);
  }

  bool SetInt16Array(const std::vector<std::int16_t>& values) {
    return SetNumberArray(VR::SS, values);
  }

  // UL (Unsigned Long)

  bool GetUint32(std::uint32_t* value) const {
    return GetNumber(VR::UL, value);
  }

  bool SetUint32(std::uint32_t value) {
    return SetNumber(VR::UL, value);
  }

  bool GetUint32Array(std::vector<std::uint32_t>* values) const {
    return GetNumberArray(VR::UL, values);
  }

  bool SetUint32Array(const std::vector<std::uint32_t>& values) {
    return SetNumberArray(VR::UL, values);
  }

  // SL (Signed Long)

  bool GetInt32(std::int32_t* value) const {
    return GetNumber(VR::SL, value);
  }

  bool SetInt32(std::int32_t value) {
    return SetNumber(VR::SL, value);
  }

  bool GetInt32Array(std::vector<std::int32_t>* values) const {
    return GetNumberArray(VR::SL, values);
  }

  bool SetInt32Array(const std::vector<std::int32_t>& values) {
    return SetNumberArray(VR::SL, values);
  }

  // FL (Floating Point Single)

  bool GetFloat32(float32_t* value) const {
    return GetNumber(VR::FL, value);
  }

  bool SetFloat32(float32_t value) {
    return SetNumber(VR::FL, value);
  }

  bool GetFloat32Array(std::vector<float32_t>* values) const {
    return GetNumberArray(VR::FL, values);
  }

  bool SetFloat32Array(const std::vector<float32_t>& values) {
    return SetNumberArray(VR::FL, values);
  }

  // FD (Floating Point Double)

  bool GetFloat64(float64_t* value) const {
    return GetNumber(VR::FD, value);
  }

  bool SetFloat64(float64_t value) {
    return SetNumber(VR::FD, value);
  }

  bool GetFloat64Array(std::vector<float64_t>* values) const {
    return GetNumberArray(VR::FD, values);
  }

  bool SetFloat64Array(const std::vector<float64_t>& values) {
    return SetNumberArray(VR::FD, values);
  }

private:

  // If VM > 1, only the first number is retrieved.
  template <typename T>
  bool GetNumber(VR vr, T* value) const {
    if (vr_ != vr) {
      return false;
    }

    const std::size_t size = sizeof(T);

    // length_ > size when VM > 1.
    assert(length_ >= size);

    std::memcpy(value, &buffer_[0], size);

    if (byte_order_ != kByteOrderOS) {
      SwapBytes(value, size);
    }

    return true;
  }

  // If VM is supposed to be > 1, setting a single number should fail.
  template <typename T>
  bool SetNumber(VR vr, const T& value) {
    if (vr_ != vr) {
      return false;
    }

    const std::size_t size = sizeof(T);

    length_ = size;
    buffer_.resize(size);

    std::memcpy(&buffer_[0], &value, size);

    if (byte_order_ != kByteOrderOS) {
      SwapBytes(&buffer_[0], size);
    }

    return true;
  }

  template <typename T>
  bool GetNumberArray(VR vr, std::vector<T>* values) const {
    if (vr != vr_) {
      return false;
    }

    const std::size_t size = sizeof(T);
    const std::size_t vm = GetVM();

    values->resize(vm);

    T* dst = &(*values)[0];

    std::memcpy(dst, &buffer_[0], length_);

    if (byte_order_ != kByteOrderOS) {
      for (std::size_t i = 0; i < vm; ++i, ++dst) {
        SwapBytes(dst, size);
      }
    }

    return true;
  }

  template <typename T>
  bool SetNumberArray(VR vr, const std::vector<T>& values) {
    if (vr != vr_) {
      return false;
    }

    const std::size_t size = sizeof(T);
    const std::size_t vm = values.size();

    buffer_.resize(size * vm);

    char* dst = &buffer_[0];

    std::memcpy(dst, &values[0], buffer_.size());

    if (byte_order_ != kByteOrderOS) {
      for (std::size_t i = 0; i < vm; ++i, dst += size) {
        SwapBytes(dst, size);
      }
    }

    return true;
  }

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
