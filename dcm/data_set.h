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
  DataSet(VR::Type vr_type = VR::EXPLICIT,
          ByteOrder byte_order = ByteOrder::LE,
          Charset charset = Charset::ISO_IR_6);  // TODO: default charset

  ~DataSet();

  void Accept(Visitor& visitor) const;

  VR::Type vr_type() const { return vr_type_; }
  void set_vr_type(VR::Type vr_type) { vr_type_ = vr_type; }

  ByteOrder byte_order() const { return byte_order_; }
  void set_byte_order(ByteOrder byte_order) { byte_order_ = byte_order; }

  Charset charset() const { return charset_; }
  void set_charset(Charset charset) { charset_ = charset; }

  std::size_t size() const { return elements_.size(); }

  // Get the element at the given index.
  const DataElement* operator[](std::size_t index) const;

  // Get the element with the given tag.
  const DataElement* Get(Tag tag) const;

  bool Append(DataElement* element);

  bool Insert(DataElement* element);

  // Reset transfer syntax, clear elements, etc.
  void Clear();

  bool GetString(Tag tag, std::string* value) const;

  bool SetString(Tag tag, const std::string& value);

  bool GetInt16(Tag tag, std::int16_t* value) const;
  bool GetInt32(Tag tag, std::int32_t* value) const;

  bool GetUint16(Tag tag, std::uint16_t* value) const;
  bool GetUint32(Tag tag, std::uint32_t* value) const;

  bool GetFloat32(Tag tag, float32_t* value) const;
  bool GetFloat64(Tag tag, float64_t* value) const;

private:
  using Elements = std::vector<DataElement*>;

  // A wrapper of std::lower_bound.
  Elements::iterator LowerBound(Tag tag);

  DataElement* Find(Tag tag);

private:
  // Explicit or implicit VR.
  VR::Type vr_type_;

  // Little endian or big endian.
  ByteOrder byte_order_;

  // Character set.
  Charset charset_;

  Elements elements_;
};

}  // namespace dcm

#endif  // DCM_DATA_SET_H_
