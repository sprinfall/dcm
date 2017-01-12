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

class DataSet : public DataElement {
public:
  // NOTE:
  // The data set will be nested if the tag is not empty.
  // The VR (UNKNOWN or SQ) is determined by if the tag is empty or not.
  DataSet(const Tag& tag = Tag());

  virtual ~DataSet();

  void set_endian(Endian endian) {
    endian_ = endian;
  }

  void set_explicit_vr(bool explicit_vr) {
    explicit_vr_ = explicit_vr;
  }

  void set_length(size_t length) {
    length_ = length;
  }

  void AddElement(DataElement* element);

  void Clear();

  void Dump(size_t indent = 0);

  // Load data set from a file.
  bool LoadFile(const std::string& file_path);

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
  // Read data set from file.
  // \param check_syntax Check transfer syntax during the reading.
  // \return The length read.
  std::uint32_t Read(File& file, bool check_syntax);

  bool CheckTransferSyntax(File& file);

  bool ReadTag(File& file, Tag* tag);
  bool ReadUint16(File& file, std::uint16_t* value);
  bool ReadUint32(File& file, std::uint32_t* value);

  // Reverse the byte order if endian types are different.
  void AdjustBytesUint16(std::uint16_t& value) const;
  void AdjustBytesUint32(std::uint32_t& value) const;

  DataElement* GetElement(const Tag& tag) const;

private:
  Endian platform_endian_;  // Endian type of platform.
  Endian endian_;  // Endian type of DICOM file.

  bool explicit_vr_;  // Explicit or implicit VR.

  std::list<DataElement*> elements_;
};

}  // namespace dcmlite

#endif  // DCMLITE_DATA_SET_H_
