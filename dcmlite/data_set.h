#ifndef DCMLITE_DATA_SET_H_
#define DCMLITE_DATA_SET_H_

#include <cstdint>
#include <string>
#include <vector>

#include "dcmlite/defs.h"
#include "dcmlite/data_element.h"

namespace dcmlite {

class Reader;
class Visitor;

class DataSet : public DataElement {
public:
  // NOTE:
  // The data set will be nested (SQ) if the tag is not empty.
  // The VR (UNKNOWN or SQ) is determined by if the tag is empty or not.
  DataSet(Tag tag = Tag(), Endian endian = kLittleEndian);

  virtual ~DataSet();

  // TODO: const?
  virtual void Accept(Visitor& visitor) override;

  Endian endian() const {
    return endian_;
  }
  void set_endian(Endian endian) {
    endian_ = endian;
  }

  bool explicit_vr() const {
    return explicit_vr_;
  }
  void set_explicit_vr(bool explicit_vr) {
    explicit_vr_ = explicit_vr;
  }

  // Get the number of child elements.
  std::size_t GetSize() const {
    return elements_.size();
  }

  // Get the element at the given index.
  DataElement* operator[](std::size_t index);

  // Get the element at the given index.
  const DataElement* operator[](std::size_t index) const;

  // Get the element at the given index.
  // Return NULL if out of index (
  const DataElement* At(std::size_t index) const;

  void AddElement(DataElement* element);

  const DataElement* GetElement(Tag tag) const;

  // Clear data elements, reset endian type, etc.
  void Clear();

  // Get raw buffer (binary data).
  bool GetBuffer(Tag tag,
                 boost::shared_array<char>* buffer,
                 std::size_t* length) const;

  bool GetString(Tag tag, std::string* value) const;

  bool GetUint16(Tag tag, std::uint16_t* value) const;
  bool GetUint32(Tag tag, std::uint32_t* value) const;

  bool GetInt16(Tag tag, std::int16_t* value) const;
  bool GetInt32(Tag tag, std::int32_t* value) const;

  bool GetFloat32(Tag tag, float32_t* value) const;
  bool GetFloat64(Tag tag, float64_t* value) const;

private:
  bool explicit_vr_;

  // Child elements.
  std::vector<DataElement*> elements_;
};

}  // namespace dcmlite

#endif  // DCMLITE_DATA_SET_H_
