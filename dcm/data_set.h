#ifndef DCM_DATA_SET_H_
#define DCM_DATA_SET_H_

#include <cstdint>
#include <string>
#include <vector>

#include "dcm/charset.h"
#include "dcm/defs.h"
#include "dcm/data_element.h"

namespace dcm {

class Visitor;

class DataSet {
public:
  // TODO: default charset
  DataSet(Endian endian = Endian::Little(),
          Charset charset = Charset::ISO_IR_6);

  ~DataSet();

  void Accept(Visitor& visitor) const;

  Endian endian() const { return endian_; }
  void set_endian(Endian endian) { endian_ = endian; }

  Charset charset() const { return charset_; }
  void set_charset(Charset charset) { charset_ = charset; }

  bool explicit_vr() const { return explicit_vr_; }
  void set_explicit_vr(bool explicit_vr) { explicit_vr_ = explicit_vr; }

  std::size_t size() const { return elements_.size(); }

  // Get the element at the given index.
  const DataElement* operator[](std::size_t index) const;

  // Get the element with the given tag.
  const DataElement* Get(Tag tag) const;

  bool Append(DataElement* element);

  bool Insert(DataElement* element);

  // Clear data elements, reset endian type, etc.
  void Clear();

  bool GetString(Tag tag, std::string* value) const;

  bool SetString(Tag tag, const std::string& value);

  bool GetInt16(Tag tag, std::int16_t* value) const;
  bool GetUint16(Tag tag, std::uint16_t* value) const;

  bool GetInt32(Tag tag, std::int32_t* value) const;
  bool GetUint32(Tag tag, std::uint32_t* value) const;

  bool GetFloat32(Tag tag, float32_t* value) const;
  bool GetFloat64(Tag tag, float64_t* value) const;

private:
  using Elements = std::vector<DataElement*>;

  // A wrapper of std::lower_bound.
  Elements::iterator LowerBound(Tag tag);

  DataElement* Find(Tag tag);

private:
  Endian endian_;

  bool explicit_vr_;

  Charset charset_;

  Elements elements_;
};

}  // namespace dcm

#endif  // DCM_DATA_SET_H_
