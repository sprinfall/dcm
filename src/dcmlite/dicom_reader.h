#ifndef DCMLITE_DICOM_READER_H_
#define DCMLITE_DICOM_READER_H_
#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "dcmlite/defs.h"

namespace dcmlite {

class DataElement;
class DataSet;
class BinaryFile;
class ReadHandler;
class Tag;

// DICOM reader reads data set from a DICOM file.
class DicomReader {
public:
  explicit DicomReader(ReadHandler* handler);
  DicomReader();

  // Read a DICOM file.
  bool ReadFile(const std::string& file_path);

private:
  // Read data element sequentially from the file.
  // \param Maximum value length to read for the current data set.
  //        Could be kUndefinedLength (0xFFFFFFFF).
  // \param check_endian Check endian type during the reading.
  // \return The length read.
  std::uint32_t ReadFile(BinaryFile& file, std::size_t max_length, bool check_endian);

  bool ReadTag(BinaryFile& file, Tag* tag);
  bool ReadUint16(BinaryFile& file, std::uint16_t* value);
  bool ReadUint32(BinaryFile& file, std::uint32_t* value);

  // Reverse the byte order if endian types are different.
  void AdjustBytesUint16(std::uint16_t& value) const;
  void AdjustBytesUint32(std::uint32_t& value) const;

private:
  ReadHandler* handler_;

  Endian endian_;  // Endian type of DICOM file.
  bool explicit_vr_;  // Explicit or implicit VR.
};

}  // namespace dcmlite

#endif  // DCMLITE_DICOM_READER_H_
