#ifndef DCM_DATA_ELEMENT_H_
#define DCM_DATA_ELEMENT_H_

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

  std::uint32_t length() const { return length_; }

  void set_length(std::uint32_t length) { length_ = length; }

  // Get raw value buffer.
  const Buffer& buffer() const { return buffer_; }

  // Set raw value buffer.
  // The buffer size must be even: 2, 4, 8, ...
  bool SetBuffer(Buffer&& buffer);

  // ---------------------------------------------------------------------------

  // Get value multiplicity.
  std::size_t GetVM() const;

  // ---------------------------------------------------------------------------
  // String

  bool GetString(std::string* value) const;

  std::string GetString() const;

  bool GetStringArray(std::vector<std::string>* values) const;

  std::vector<std::string> GetStringArray() const;

  bool SetString(const std::string& value);

  bool SetStringArray(const std::vector<std::string>& values);

  // ---------------------------------------------------------------------------
  // US (Unsigned Short)

  bool GetUint16(std::uint16_t* value) const {
    return GetNumber(VR::US, value);
  }

  std::uint16_t GetUint16(std::uint16_t default_value = 0) const {
    return GetNumber(VR::US, default_value);
  }

  bool SetUint16(std::uint16_t value) {
    return SetNumber(VR::US, value);
  }

  bool GetUint16Array(std::vector<std::uint16_t>* values) const {
    return GetNumberArray(VR::US, values);
  }

  std::vector<std::uint16_t> GetUint16Array() const {
    return GetNumberArray<std::uint16_t>(VR::US);
  }
  
  bool SetUint16Array(const std::vector<std::uint16_t>& values) {
    return SetNumberArray(VR::US, values);
  }

  // ---------------------------------------------------------------------------
  // SS (Signed Short)

  bool GetInt16(std::int16_t* value) const {
    return GetNumber(VR::SS, value);
  }

  std::int16_t GetInt16(std::int16_t default_value = 0) const {
    return GetNumber(VR::SS, default_value);
  }

  bool SetInt16(std::int16_t value) {
    return SetNumber(VR::SS, value);
  }

  bool GetInt16Array(std::vector<std::int16_t>* values) const {
    return GetNumberArray(VR::SS, values);
  }

  std::vector<std::int16_t> GetInt16Array() const {
    return GetNumberArray<std::int16_t>(VR::SS);
  }

  bool SetInt16Array(const std::vector<std::int16_t>& values) {
    return SetNumberArray(VR::SS, values);
  }

  // ---------------------------------------------------------------------------
  // UL (Unsigned Long)

  bool GetUint32(std::uint32_t* value) const {
    return GetNumber(VR::UL, value);
  }

  std::uint32_t GetUint32(std::uint32_t default_value = 0) const {
    return GetNumber(VR::UL, default_value);
  }

  bool SetUint32(std::uint32_t value) {
    return SetNumber(VR::UL, value);
  }

  bool GetUint32Array(std::vector<std::uint32_t>* values) const {
    return GetNumberArray(VR::UL, values);
  }

  std::vector<std::uint32_t> GetUint32Array() const {
    return GetNumberArray<std::uint32_t>(VR::UL);
  }

  bool SetUint32Array(const std::vector<std::uint32_t>& values) {
    return SetNumberArray(VR::UL, values);
  }

  // ---------------------------------------------------------------------------
  // SL (Signed Long)

  bool GetInt32(std::int32_t* value) const {
    return GetNumber(VR::SL, value);
  }

  std::int32_t GetInt32(std::int32_t default_value = 0) const {
    return GetNumber(VR::SL, default_value);
  }

  bool SetInt32(std::int32_t value) {
    return SetNumber(VR::SL, value);
  }

  bool GetInt32Array(std::vector<std::int32_t>* values) const {
    return GetNumberArray(VR::SL, values);
  }

  std::vector<std::int32_t> GetInt32Array() const {
    return GetNumberArray<std::int32_t>(VR::SL);
  }

  bool SetInt32Array(const std::vector<std::int32_t>& values) {
    return SetNumberArray(VR::SL, values);
  }

  // ---------------------------------------------------------------------------
  // FL (Floating Point Single)

  bool GetFloat32(float32_t* value) const {
    return GetNumber(VR::FL, value);
  }

  float32_t GetFloat32(float32_t default_value = 0.0f) const {
    return GetNumber(VR::FL, default_value);
  }

  bool SetFloat32(float32_t value) {
    return SetNumber(VR::FL, value);
  }

  bool GetFloat32Array(std::vector<float32_t>* values) const {
    return GetNumberArray(VR::FL, values);
  }

  std::vector<float32_t> GetFloat32Array() const {
    return GetNumberArray<float32_t>(VR::FL);
  }

  bool SetFloat32Array(const std::vector<float32_t>& values) {
    return SetNumberArray(VR::FL, values);
  }

  // ---------------------------------------------------------------------------
  // FD (Floating Point Double)

  bool GetFloat64(float64_t* value) const {
    return GetNumber(VR::FD, value);
  }

  float64_t GetFloat64(float64_t default_value = 0.0f) const {
    return GetNumber(VR::FD, default_value);
  }

  bool SetFloat64(float64_t value) {
    return SetNumber(VR::FD, value);
  }

  bool GetFloat64Array(std::vector<float64_t>* values) const {
    return GetNumberArray(VR::FD, values);
  }

  std::vector<float64_t> GetFloat64Array() const {
    return GetNumberArray<float64_t>(VR::FD);
  }

  bool SetFloat64Array(const std::vector<float64_t>& values) {
    return SetNumberArray(VR::FD, values);
  }

private:
  void DoSetString(const std::string& value);

  template <typename T>
  bool GetNumber(VR vr, T* value) const {
    return GetNumber(vr, sizeof(T), value);
  }

  template <typename T>
  T GetNumber(VR vr, T default_value) const {
    T value = default_value;
    GetNumber(vr, &value);
    return value;
  }

  template <typename T>
  bool SetNumber(VR vr, T value) {
    return SetNumber(vr, sizeof(T), &value);
  }

  template <typename T>
  bool GetNumberArray(VR vr, std::vector<T>* values) const {
    const std::size_t count = GetVM();
    values->resize(count);
    return GetNumberArray(vr, sizeof(T), count, &(*values)[0]);
  }

  template <typename T>
  std::vector<T> GetNumberArray(VR vr) const {
    std::vector<T> values;
    GetNumberArray(vr, &values);
    return values;
  }

  template <typename T>
  bool SetNumberArray(VR vr, const std::vector<T>& values) {
    // TODO: What if values is empty?
    return SetNumberArray(vr, sizeof(T), values.size(), &values[0]);
  }

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
  // 0xFFFFFFFF (-1) for SQ element.
  // Identical to the buffer size if the buffer is not empty.
  std::uint32_t length_;

private:
  // Raw buffer (i.e., bytes) of the value.
  Buffer buffer_;
};

}  // namespace dcm

#endif  // DCM_DATA_ELEMENT_H_
