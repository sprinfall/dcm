#ifndef DCMLITE_DATA_SET_H_
#define DCMLITE_DATA_SET_H_
#pragma once

#include <cstdint>
#include <list>
#include <string>

#include "boost/shared_array.hpp"

#include "dcmlite/defs.h"
#include "dcmlite/data_element.h"

namespace dcmlite {

class File;
class Visitor;

class DataSet : public DataElement {
public:
  // NOTE:
  // The data set will be nested (SQ) if the tag is not empty.
  // The VR (UNKNOWN or SQ) is determined by if the tag is empty or not.
  DataSet(const Tag& tag = Tag());

  virtual ~DataSet();

  virtual void Accept(Visitor& visitor) const;

  void set_endian(Endian endian) {
    endian_ = endian;
  }

  void set_explicit_vr(bool explicit_vr) {
    explicit_vr_ = explicit_vr;
  }

  void set_length(size_t length) {
    length_ = length;
  }

  const std::list<DataElement*>& elements() const {
    return elements_;
  }

  void AddElement(DataElement* element);

  // Clear data elements, reset endian type, etc.
  void Clear();

  bool GetBuffer(const Tag& tag,
                 boost::shared_array<char>* buffer,
                 size_t* len) const;

  // Get string value of a tag.
  bool GetString(const Tag& tag, std::string* value) const;

  // Get 16-bit unsigned int value of a tag.
  bool GetUint16(const Tag& tag, std::uint16_t* value) const;

  // Get 32-bit unsigned int value of a tag.
  bool GetUint32(const Tag& tag, std::uint32_t* value) const;

private:
  // Reverse the byte order if endian types are different.
  void AdjustBytesUint16(std::uint16_t& value) const;
  void AdjustBytesUint32(std::uint32_t& value) const;

  DataElement* GetElement(const Tag& tag) const;

private:
  Endian endian_;  // Endian type of DICOM file.
  bool explicit_vr_;  // Explicit or implicit VR.

  std::list<DataElement*> elements_;
};

}  // namespace dcmlite

#endif  // DCMLITE_DATA_SET_H_
