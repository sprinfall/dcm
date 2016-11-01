#ifndef DCMLITE_DICOM_FILE_H_
#define DCMLITE_DICOM_FILE_H_
#pragma once

#include <string>
#include "dcmlite/defs.h"

namespace dcmlite {

class File;
class Tag;

class DicomFile {
public:
  DicomFile();
  ~DicomFile();

  // \param file_path DICOM file path.
  bool Read(const std::string& file_path);

private:
  void ReadDataSet(File& file);

  bool CheckTransferSyntax(File& file);

  bool ReadTag(File& file, Tag* tag);

  bool ReadUInt16(File& file, std::uint16_t* value);

private:
  Endian platform_endian_;  // Endian type of platform.
  Endian endian_;  // Endian type of DICOM file.

  bool explicit_vr_;  // Explicit or implicit VR.
};

}  // namespace dcmlite

#endif  // DCMLITE_DICOM_FILE_H_
