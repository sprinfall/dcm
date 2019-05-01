#ifndef DCM_DATA_SET_H_
#define DCM_DATA_SET_H_

#include <cstdint>
#include <string>
#include <vector>

#include "dcm/defs.h"
#include "dcm/data_element.h"

namespace dcm {

class Reader;
class Visitor;

class DataSet : public DataElement {
public:
  // NOTE:
  // The data set will be nested (SQ) if the tag is not empty.
  // The VR (UNKNOWN or SQ) is determined by if the tag is empty or not.
  DataSet(Tag tag = Tag(), Endian endian = Endian::Little());

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

  const DataElement* GetElement(Tag tag) const;

  // Return false if element->tag() <= the last element.
  bool AppendElement(DataElement* element);

  bool InsertElement(DataElement* element);

  // Clear data elements, reset endian type, etc.
  void Clear();

  // Get element value length.
  std::size_t GetValueLength(Tag tag) const;

  bool GetString(Tag tag, std::string* value) const;

  bool SetString(Tag tag, const std::string& value);

  bool GetInt16(Tag tag, std::int16_t* value) const;
  bool GetUint16(Tag tag, std::uint16_t* value) const;

  bool GetInt32(Tag tag, std::int32_t* value) const;
  bool GetUint32(Tag tag, std::uint32_t* value) const;

  bool GetFloat32(Tag tag, float32_t* value) const;
  bool GetFloat64(Tag tag, float64_t* value) const;

private:
  DataElement* DoGetElement(Tag tag/*, bool create = false*/);

private:
  bool explicit_vr_;

  // Child elements sorted by tag.
  std::vector<DataElement*> elements_;
};

}  // namespace dcm

#endif  // DCM_DATA_SET_H_
