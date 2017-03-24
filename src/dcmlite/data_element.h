#ifndef DCMLITE_DATA_ELEMENT_H_
#define DCMLITE_DATA_ELEMENT_H_
#pragma once

#include <cstdint>
#include <list>

#include "boost/shared_array.hpp"

#include "dcmlite/tag.h"
#include "dcmlite/vr.h"

namespace dcmlite {

class DataSet;

class DataElement {
public:
  DataElement(const Tag& tag, VR::Type vr_type);

  virtual ~DataElement();

  const Tag& tag() const {
    return tag_;
  }

  // NOTE: VR is not allowed to change after construction.
  VR::Type vr_type() const {
    return vr_type_;
  }

  // NOTE: Setter is provided in DataSet instead of here.
  size_t length() const {
    return length_;
  }

  const boost::shared_array<char>& buffer() const {
    return buffer_;
  }

  // \param length The length of the buffer, must be even.
  // NOTE:
  // Length must be set together with buffer to avoid inconsistent
  // buffer and length.
  void SetBuffer(boost::shared_array<char> buffer, size_t length);

  bool AsString(std::string* value) const;

  bool AsUint16(std::uint16_t* value) const;
  bool AsUint32(std::uint32_t* value) const;

protected:
  Tag tag_;
  VR::Type vr_type_;

  // Value length.
  // Undefined length for SQ element is 0xFFFFFFFF.
  size_t length_;

  boost::shared_array<char> buffer_;
};

}  // namespace dcmlite

#endif  // DCMLITE_DATA_ELEMENT_H_
