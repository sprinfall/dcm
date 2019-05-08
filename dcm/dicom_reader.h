#ifndef DCM_DICOM_READER_H_
#define DCM_DICOM_READER_H_

#include <string>

#include "dcm/defs.h"

namespace dcm {

class DataElement;
class DataSet;
class Reader;
class ReadHandler;

class DicomReader {
public:
  explicit DicomReader(DataSet* data_set);

  explicit DicomReader(ReadHandler* handler);

  ~DicomReader();

  // Read a DICOM file.
  bool ReadFile(const Path& path);

private:
  bool DoRead(Reader& reader);

  // Read data element sequentially from the reader.
  // \param max_length Maximum value length to read for the current data set.
  //        Could be kUndefinedLength (0xFFFFFFFF).
  // \return The length read.
  std::uint32_t Read(Reader& reader, std::size_t max_length);

  // Check transfer syntax by Transfer Syntax UID read from 0x00020010.
  // A "smart" algorithm will be used instead if 0x00020010 is absent.
  void CheckTransferSyntax(Reader& reader);

  bool ReadTag(Reader& reader, Tag* tag);

  bool ReadUint16(Reader& reader, std::uint16_t* value);
  bool ReadUint32(Reader& reader, std::uint32_t* value);

  // Read sequence delimitation tag.
  void ReadSeqEndTag(Reader& reader, Tag tag, std::uint32_t& read_length);

  // Read sequence item delimitation tag.
  void ReadSeqItemEndTag(Reader& reader, Tag tag, std::uint32_t& read_length);

  // Read sequence prefix tag.
  void ReadSeqItemPrefixTag(Reader& reader, Tag tag,
                            std::uint32_t& read_length);

  // Read VR code.
  bool ReadVR(Reader& reader, Tag tag, std::uint32_t& read_length, VR* vr);

  std::uint32_t ReadValueLength(Reader& reader, VR vr,
                                std::uint32_t& read_length);

  bool ReadValue(Reader& reader, Tag tag, VR vr, std::uint32_t length,
                 std::uint32_t& read_length);

  DataElement* ReadElement(Reader& reader, Tag tag, VR vr,
                           std::uint32_t length);

private:
  ReadHandler* handler_;

  // If we own (need to delete) the handler or not.
  bool own_handler_;

  // Transfer Syntax UID read from 0x00020010.
  // Please note that some DICOM files don't have group 0002.
  // But even if it's absent, a "smart" algorithm will be used to figure out the
  // transfer syntax of the data set.
  std::string transfer_syntax_uid_;

  // If the transfer syntax has been checked or not.
  bool transfer_syntax_checked_;

  // Explicit or implicit VR.
  VR::Type vr_type_;

  // Little endian or big endian.
  ByteOrder byte_order_;
};

}  // namespace dcm

#endif  // DCM_DICOM_READER_H_
