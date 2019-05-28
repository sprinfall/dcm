#include "dcm/dicom_reader.h"

#include "boost/filesystem/fstream.hpp"

#include "dcm/data_dict.h"
#include "dcm/data_sequence.h"
#include "dcm/data_set.h"
#include "dcm/logger.h"
#include "dcm/read_handler.h"
#include "dcm/reader.h"
#include "dcm/util.h"

namespace bfs = boost::filesystem;

namespace dcm {

// -----------------------------------------------------------------------------

namespace {

bool CheckVrType(Reader& reader, VR::Type* vr_type) {
  // Skip the 4 tag bytes.
  reader.Seek(4, std::ios::cur);

  char bytes[2];
  if (!reader.ReadBytes(bytes, 2)) {
    reader.UndoRead(4);  // Put 4 tag bytes back.
    return false;
  }

  reader.UndoRead(6);  // Put it back.

  // Check to see if the 2 bytes following the tag field represents a valid VR.
  if (VR(bytes).IsUnknown()) {
    *vr_type = VR::IMPLICIT;
    LOG_INFO("VR type: Implicit.");
  } else {
    *vr_type = VR::EXPLICIT;
    LOG_INFO("VR type: Explicit.");
  }

  return true;
}

bool CheckByteOrder(Reader& reader, ByteOrder* byte_order) {
  std::uint8_t bytes[4] = { 0 };
  if (!reader.ReadBytes(bytes, 4)) {
    return false;
  }

  reader.UndoRead(4);  // Put it back.

  std::uint16_t group = (bytes[0] & 0xff) + ((bytes[1] & 0xff) << 8);
  std::uint16_t element = (bytes[2] & 0xff) + ((bytes[3] & 0xff) << 8);
  Tag tag_l(group, element);  // Little Endian
  Tag tag_b = tag_l.SwapBytes();  // Big Endian

  const dict::Entry* entry_l = dict::GetEntry(tag_l);
  const dict::Entry* entry_b = dict::GetEntry(tag_b);

  if (entry_l == nullptr && entry_b == nullptr) {
    if (element == 0) {
      // Group Length tag not in data dictionary, check the group number.
      // For the first tag, group number is more probable of 0008 than 0800.
      if (tag_l.group() > 0xff && tag_b.group() <= 0xff) {
        *byte_order = ByteOrder::BE;
      } else {
        *byte_order = ByteOrder::LE;
      }
    } else {
      // Both tags show an error, an invalid tag is encountered.
      // Assume that it's Little Endian.
      *byte_order = ByteOrder::LE;
    }
  } else {
    if (entry_l == nullptr) {
      *byte_order = ByteOrder::BE;
    } else if (entry_b == nullptr) {
      *byte_order = ByteOrder::LE;
    } else {
      // Both tags are valid, check the group number.
      // For the first tag, group number is more probable of 0008 than 0800.
      if (tag_l.group() > 0xff && tag_b.group() <= 0xff) {
        *byte_order = ByteOrder::BE;
      } else {
        *byte_order = ByteOrder::LE;
      }
    }
  }

  if (*byte_order == ByteOrder::LE) {
    LOG_INFO("Byte order checked: little endian.", );
  } else {
    LOG_INFO("Byte order checked: big endian.", );
  }

  return true;
}

}  // namespace

// -----------------------------------------------------------------------------

DicomReader::DicomReader(ReadHandler* handler)
    : handler_(handler),
      transfer_syntax_checked_(false),
      vr_type_(VR::EXPLICIT) {
}

DicomReader::~DicomReader() {
}

bool DicomReader::ReadFile(const Path& path) {
  bfs::ifstream stream(path, std::ios::binary);
  if (stream.bad()) {
    LOG_ERRO("Failed to open the file to read: %s", path.string().c_str());
    return false;
  }

  Reader reader(&stream);
  return DoRead(reader);
}

bool DicomReader::DoRead(Reader& reader) {
  // Preamble
  reader.Seek(128);

  char prefix[4];
  if (!reader.ReadBytes(prefix, 4)) {
    LOG_ERRO("Failed to read 4 bytes DICOM prefix.");
    return false;
  }

  if (memcmp(prefix, "DICM", 4) != 0) {
    // Preamble is omitted.
    reader.UndoRead(132);
  }

  // Group 0002 is always Explicit VR Little Endian.
  vr_type_ = VR::EXPLICIT;
  byte_order_ = ByteOrder::LE;

  Read(reader, kUndefinedLength);

  return true;
}

std::uint32_t DicomReader::Read(Reader& reader, std::size_t max_length) {
  std::uint32_t read_length = 0;

  Tag tag;

  while (read_length < max_length) {
    if (handler_->should_stop()) {
      break;  // Handler required to stop reading.
    }

    if (!ReadTag(reader, &tag)) {
      break;
    }

    read_length += 4;

    if (!transfer_syntax_checked_) {
      if (tag.group() != 0x0002) {
        // Undo read tag since the byte order might be wrong.
        reader.UndoRead(4);

        CheckTransferSyntax(reader);

        transfer_syntax_checked_ = true;

        handler_->OnTransferSyntax(vr_type_, byte_order_);

        // Go back to read the tag again.
        continue;
      }
    }

    // End of sequence itself.
    if (tag == tags::kSeqDelimatation) {
      ReadSeqDelimitation(reader, tag, read_length);
      break;
    }

    // End of sequence item.
    if (tag == tags::kSeqItemDelimatation) {
      ReadSeqItemDelimitation(reader, tag, read_length);
      // The reading of the sequence item has ended.
      // The value length of this sequence item must be undefined, so it is
      // ended with a delimitation tag.
      // See ReadSeqItemPrefix().
      break;
    }

    if (tag == tags::kSeqItemPrefix) {
      ReadSeqItemPrefix(reader, tag, read_length);
      continue;
    }

    VR vr = VR::UN;
    if (!ReadVR(reader, tag, read_length, &vr)) {
      break;
    }

    std::uint32_t length = ReadValueLength(reader, vr, read_length);

    if (!ReadValue(reader, tag, vr, length, read_length)) {
      break;
    }
  }

  return read_length;
}

void DicomReader::CheckTransferSyntax(Reader& reader) {
  if (!transfer_syntax_uid_.empty()) {
    // Group 0002 and tag 0x00020010 exist.

    LOG_INFO("Check transfer syntax by 0x00020010 (%s).",
             transfer_syntax_uid_.c_str());

    if (transfer_syntax_uid_ == transfer_syntax_uids::kImplicitLittleEndian) {
      vr_type_ = VR::IMPLICIT;
      byte_order_ = ByteOrder::LE;
    } else if (transfer_syntax_uid_ ==
               transfer_syntax_uids::kExplicitLittleEndian) {
      vr_type_ = VR::EXPLICIT;
      byte_order_ = ByteOrder::LE;
    } else if (transfer_syntax_uid_ ==
               transfer_syntax_uids::kDeflatedExplicitLittleEndian) {
      vr_type_ = VR::EXPLICIT;
      byte_order_ = ByteOrder::LE;
    } else if (transfer_syntax_uid_ ==
               transfer_syntax_uids::kExplicitBigEndian) {
      vr_type_ = VR::EXPLICIT;
      byte_order_ = ByteOrder::BE;
    } else {
      // Compressed pixel data transfer syntax are always Explicit VR Little
      // Endian (so you can call JPEG baseline 1.2.840.10008.1.2.4.50 for
      // example "Explicit Little Endian JPEG Baseline").
      vr_type_ = VR::EXPLICIT;
      byte_order_ = ByteOrder::LE;
    }

    return;
  }

  LOG_INFO("Check transfer syntax by reading some bytes then figure out with "
           "the help of VR and data dictionaries.");

  CheckVrType(reader, &vr_type_);
  CheckByteOrder(reader, &byte_order_);
}

bool DicomReader::ReadTag(Reader& reader, Tag* tag) {
  std::uint16_t group = 0;
  std::uint16_t element = 0;

  if (!ReadUint16(reader, &group)) {
    return false;
  }

  if (!ReadUint16(reader, &element)) {
    return false;
  }

  tag->set_group(group);
  tag->set_element(element);

  return true;
}

bool DicomReader::ReadUint16(Reader& reader, std::uint16_t* value) {
  if (reader.ReadUint16(value)) {
    if (byte_order_ != kByteOrderOS) {
      util::Swap16(value);
    }
    return true;
  }
  return false;
}

bool DicomReader::ReadUint32(Reader& reader, std::uint32_t* value) {
  if (reader.ReadUint32(value)) {
    if (byte_order_ != kByteOrderOS) {
      util::Swap32(value);
    }
    return true;
  }
  return false;
}

void DicomReader::ReadSeqDelimitation(Reader& reader, Tag tag,
                                      std::uint32_t& read_length) {
  LOG_INFO("Read sequence delimitation tag.");

  // Skip the 4-byte zero length of this sequence delimitation item.
  reader.Seek(4, std::ios::cur);
  read_length += 4;

  auto element = new DataElement(tag, VR::UN, byte_order_);
  handler_->OnSequenceEnd(element);
}

void DicomReader::ReadSeqItemDelimitation(Reader& reader, Tag tag,
                                          std::uint32_t& read_length) {
  LOG_INFO("Read sequence item delimitation tag.");

  // Skip the 4-byte zero length of this item delimitation item.
  reader.Seek(4, std::ios::cur);
  read_length += 4;

  auto element = new DataElement(tag, VR::UN, byte_order_);
  handler_->OnSequenceItemEnd(element);
}

void DicomReader::ReadSeqItemPrefix(Reader& reader, Tag tag,
                                    std::uint32_t& read_length) {
  LOG_INFO("Read sequence item prefix tag.");

  std::uint32_t item_length = 0;
  ReadUint32(reader, &item_length);

  read_length += 4;

  auto element = new DataElement(tag, VR::UN, byte_order_);

  // If item length is undefined, this item will be ended with a delimitation.
  element->set_length(item_length);

  handler_->OnSequenceItemStart(element);

  read_length += Read(reader, item_length);

  handler_->OnSequenceItemEnd();
}

bool DicomReader::ReadVR(Reader& reader, Tag tag, std::uint32_t& read_length,
                         VR* vr) {
  if (vr_type_ == VR::EXPLICIT) {
    char bytes[2];
    reader.ReadBytes(bytes, 2);
    read_length += 2;

    if (!vr->SetBytes(bytes)) {
      LOG_ERRO("'%s' is not a VR!", std::string(bytes, 2).c_str());
      return false;
    }
  } else {
    if (tag.element() == 0) {
      // Group Length (VR type is always UL).
      *vr = VR::UL;
    } else {
      if (tag.group() % 2 == 1) {
        // Private tag with Implicit VR.
        // As I know, images generated by Carestream Dental CR CS7600 are really
        // Implicit VR. They do have private tags.
        LOG_WARN("Private tag in Implicit VR.");
      } else {
        // Query VR type from data dictionary.
        *vr = dict::GetVR(tag);
        if (vr->IsUnknown()) {
          LOG_WARN("Unknown tag in Implicit VR.");
        }
      }
    }
  }

  return true;
}

std::uint32_t DicomReader::ReadValueLength(Reader& reader, VR vr,
                                           std::uint32_t& read_length) {
  std::uint32_t vl32 = 0;

  if (vr_type_ == VR::EXPLICIT) {
    // For VRs of OB, OD, OF, OL, OW, SQ, UN and UC, UR, UT, the 16 bits
    // following the two character VR Field are reserved for use by later
    // versions of the DICOM Standard.
    // See: PS 3.5 Section 7.1.2 - Data Element Structure with Explicit VR

    std::uint16_t vl16 = 0;
    ReadUint16(reader, &vl16);
    read_length += 2;

    if (vl16 != 0) {
      vl32 = vl16;
    } else {
      // The 16 bits after VR Field are 0, but we can't conclude that they
      // are reserved since the value length might actually be 0.
      // Check the VR to confirm it.
      if (vr.Is16BitsFollowingReversed()) {
        LOG_INFO("2 bytes following VR are reserved.");

        // This 2 bytes are reserved, read the 4-byte value length.
        ReadUint32(reader, &vl32);
        read_length += 4;
      } else {
        // Value length is 0.
        vl32 = 0;
      }
    }
  } else {  // Implicit VR
    ReadUint32(reader, &vl32);
    read_length += 4;
  }

  return vl32;
}

bool DicomReader::ReadValue(Reader& reader, Tag tag, VR vr,
                            std::uint32_t length, std::uint32_t& read_length) {
  if (vr == VR::SQ) {
    auto data_sequence = new DataSequence(tag);
    data_sequence->set_length(length);

    handler_->OnSequenceStart(data_sequence);

    if (length > 0) {
      read_length += Read(reader, length);
    }

    handler_->OnSequenceEnd();

  } else {
    if (length == kUndefinedLength) {
      LOG_ERRO("Non-SQ element with undefined length.");
      return false;
    }

    if (handler_->OnElementStart(tag)) {
      DataElement* element = ReadElement(reader, tag, vr, length);
      if (element == nullptr) {
        return false;
      }

      if (transfer_syntax_uid_.empty() && tag == tags::kTransferSyntaxUID) {
        element->GetString(&transfer_syntax_uid_);
      }

      // Call handler as the last step since it might delete the element.
      handler_->OnElementEnd(element);

    } else {
      if (transfer_syntax_uid_.empty() && tag == tags::kTransferSyntaxUID) {
        DataElement* element = ReadElement(reader, tag, vr, length);
        if (element == nullptr) {
          return false;
        }

        element->GetString(&transfer_syntax_uid_);
        delete element;

      } else {
        // Just skip the buffer.
        if (length > 0) {
          // TODO: error
          reader.Seek(length, std::ios::cur);
        }
      }
    }

    read_length += length;
  }

  return true;
}

DataElement* DicomReader::ReadElement(Reader& reader, Tag tag, VR vr,
                                      std::uint32_t length) {
  auto element = new DataElement(tag, vr, byte_order_);

  if (length > 0) {
    Buffer buffer(length);

    if (reader.ReadBytes(&buffer[0], length) == length) {
      element->SetBuffer(std::move(buffer));
    } else {
      LOG_ERRO("Failed to read value of size: %u", length);
      delete element;
      element = nullptr;
    }
  }

  return element;
}

}  // namespace dcm
