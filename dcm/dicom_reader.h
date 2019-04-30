#ifndef DCM_DICOM_READER_H_
#define DCM_DICOM_READER_H_

#include <string>

#include "dcm/defs.h"

namespace dcm {

class DataElement;
class DataSet;
class Reader;
class ReadHandler;
class Tag;

// DICOM reader reads data set from a DICOM file.
class DicomReader {
public:
  explicit DicomReader(ReadHandler* handler);

  ~DicomReader() = default;

  // Read a DICOM file.
  bool ReadFile(const Path& path);

private:
  bool DoRead(Reader& reader);

  // Read data element sequentially from the reader.
  // \param max_length Maximum value length to read for the current data set.
  //        Could be kUndefinedLength (0xFFFFFFFF).
  // \param check_endian Check endian type during the reading.
  // \return The length read.
  std::uint32_t Read(Reader& reader, std::size_t max_length, bool check_endian);

  bool ReadTag(Reader& reader, Tag* tag);
  bool ReadUint16(Reader& reader, std::uint16_t* value);
  bool ReadUint32(Reader& reader, std::uint32_t* value);

  // Reverse the byte order if endian types are different.
  void AdjustBytesUint16(std::uint16_t& value) const;
  void AdjustBytesUint32(std::uint32_t& value) const;

private:
  ReadHandler* handler_;

  // Endian type of DICOM file.
  Endian endian_;

  // Explicit or implicit VR.
  bool explicit_vr_;
};

}  // namespace dcm

#endif  // DCM_DICOM_READER_H_
