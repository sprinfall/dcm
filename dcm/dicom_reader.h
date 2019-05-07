#ifndef DCM_DICOM_READER_H_
#define DCM_DICOM_READER_H_

#include <string>

#include "dcm/defs.h"
#include "dcm/tag.h"

namespace dcm {

class DataElement;
class DataSet;
class Reader;
class ReadHandler;

class DicomReader {
public:
  explicit DicomReader(ReadHandler* handler);

  explicit DicomReader(DataSet* data_set);

  ~DicomReader();

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

  void ReadSeqEndTag(Reader& reader, Tag tag, std::uint32_t& read_length);
  void ReadSeqItemEndTag(Reader& reader, Tag tag, std::uint32_t& read_length);
  void ReadSeqItemPrefixTag(Reader& reader, Tag tag,
                            std::uint32_t& read_length);

  bool ReadVR(Reader& reader, Tag tag, std::uint32_t& read_length, VR* vr);

  std::uint32_t ReadValueLength(Reader& reader, VR vr,
                                std::uint32_t& read_length);

  bool ReadValue(Reader& reader, Tag tag, VR vr, std::uint32_t vl32,
                 std::uint32_t& read_length);

private:
  ReadHandler* handler_;

  bool own_handler_;

  Endian endian_;

  bool explicit_vr_;
};

}  // namespace dcm

#endif  // DCM_DICOM_READER_H_
