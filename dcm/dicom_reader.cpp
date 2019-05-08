#include "dcm/dicom_reader.h"

#include "boost/filesystem/fstream.hpp"

#include "dcm/data_dict.h"
#include "dcm/data_sequence.h"
#include "dcm/data_set.h"
#include "dcm/full_read_handler.h"
#include "dcm/logger.h"
#include "dcm/reader.h"
#include "dcm/util.h"

namespace bfs = boost::filesystem;

namespace dcm {

// -----------------------------------------------------------------------------

namespace {

bool CheckVrExplicity(Reader& reader, bool* explicit_vr) {
  // Skip the 4 tag bytes.
  reader.Seek(4, std::ios::cur);

  std::string vr_str;
  if (!reader.ReadString(&vr_str, 2)) {
    reader.UndoRead(4);  // Put 4 tag bytes back.
    return false;
  }

  reader.UndoRead(6);  // Put it back.

  // Check to see if the 2 bytes following the tag field represents a valid VR.
  if (StringToVR(vr_str, nullptr)) {
    *explicit_vr = true;
  } else {
    *explicit_vr = false;
  }

  LOG_INFO("Explicit VR: %d.", *explicit_vr);

  return true;
}

bool CheckByteOrder(Reader& reader, ByteOrder* byte_order) {
  std::uint8_t tag_bytes[4] = { 0 };
  if (!reader.ReadBytes(&tag_bytes, 4)) {
    return false;
  }

  reader.UndoRead(4);  // Put it back.

  std::uint16_t group = (tag_bytes[0] & 0xff) + ((tag_bytes[1] & 0xff) << 8);
  std::uint16_t element = (tag_bytes[2] & 0xff) + ((tag_bytes[3] & 0xff) << 8);
  Tag tag_l(group, element);  // Little Endian
  Tag tag_b = tag_l.SwapBytes();  // Big Endian

  const DataEntry* entry_l = DataDict::GetEntry(tag_l);
  const DataEntry* entry_b = DataDict::GetEntry(tag_b);

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

DicomReader::DicomReader(DataSet* data_set)
    : handler_(new FullReadHandler(data_set)), own_handler_(true),
      transfer_syntax_checked_(false),
      explicit_vr_(true) {
}

DicomReader::DicomReader(ReadHandler* handler)
    : handler_(handler), own_handler_(false),
      transfer_syntax_checked_(false),
      explicit_vr_(true) {
}

DicomReader::~DicomReader() {
  if (own_handler_) {
    delete handler_;
  }
}

bool DicomReader::ReadFile(const Path& path) {
  bfs::ifstream stream(path, std::ios::binary);
  if (stream.bad()) {
    return false;
  }

  Reader reader(&stream);
  return DoRead(reader);
}

bool DicomReader::DoRead(Reader& reader) {
  // Preamble
  reader.Seek(128);

  std::string prefix;
  if (!reader.ReadString(&prefix, 4)) {
    return false;  // TODO: Use exception?
  }

  if (prefix != "DICM") {
    // Preamble is omitted.
    reader.UndoRead(132);
  }

  // Group 0002 is always Explicit VR Little Endian.
  byte_order_ = ByteOrder::LE;
  explicit_vr_ = true;

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

        handler_->OnEndian(byte_order_);
        handler_->OnExplicitVR(explicit_vr_);

        // Go back to read the tag again.
        continue;
      }
    }

    // End of sequence itself.
    if (tag == kSeqEndTag) {
      ReadSeqEndTag(reader, tag, read_length);
      break;
    }

    // End of sequence item.
    if (tag == kSeqItemEndTag) {
      ReadSeqItemEndTag(reader, tag, read_length);
      // The reading of the sequence item has ended.
      // The value length of this sequence item must be undefined, so it is
      // ended with a delimitation tag.
      // See ReadSeqItemPrefixTag().
      break;
    }

    if (tag == kSeqItemPrefixTag) {
      ReadSeqItemPrefixTag(reader, tag, read_length);
      continue;
    }

    VR vr = VR::UNKNOWN;
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
      explicit_vr_ = false;
      byte_order_ = ByteOrder::LE;
    } else if (transfer_syntax_uid_ ==
               transfer_syntax_uids::kExplicitLittleEndian) {
      explicit_vr_ = true;
      byte_order_ = ByteOrder::LE;
    } else if (transfer_syntax_uid_ ==
               transfer_syntax_uids::kDeflatedExplicitLittleEndian) {
      explicit_vr_ = true;
      byte_order_ = ByteOrder::LE;
    } else if (transfer_syntax_uid_ ==
               transfer_syntax_uids::kExplicitBigEndian) {
      explicit_vr_ = true;
      byte_order_ = ByteOrder::BE;
    } else {
      // Compressed pixel data transfer syntax are always Explicit VR Little
      // Endian (so you can call JPEG baseline 1.2.840.10008.1.2.4.50 for
      // example "explicit little endian jpeg baseline").
      explicit_vr_ = true;
      byte_order_ = ByteOrder::LE;
    }

    return;
  }

  LOG_INFO("Check transfer syntax by reading some bytes then figure out with "
           "the help of VR and data dictionaries.");

  CheckByteOrder(reader, &byte_order_);
  CheckVrExplicity(reader, &explicit_vr_);
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
      Swap16(value);
    }
    return true;
  }
  return false;
}

bool DicomReader::ReadUint32(Reader& reader, std::uint32_t* value) {
  if (reader.ReadUint32(value)) {
    if (byte_order_ != kByteOrderOS) {
      Swap32(value);
    }
    return true;
  }
  return false;
}

void DicomReader::ReadSeqEndTag(Reader& reader, Tag tag,
                                std::uint32_t& read_length) {
  LOG_INFO("Read sequence end/delimitation tag.");

  // Skip the 4-byte zero length of this sequence delimitation item.
  reader.Seek(4, std::ios::cur);
  read_length += 4;

  auto element = new DataElement(tag, VR::UNKNOWN, byte_order_);
  handler_->OnSequenceEnd(element);
}

void DicomReader::ReadSeqItemEndTag(Reader& reader, Tag tag,
                                    std::uint32_t& read_length) {
  LOG_INFO("Read sequence item end/delimitation tag.");

  // Skip the 4-byte zero length of this item delimitation item.
  reader.Seek(4, std::ios::cur);
  read_length += 4;

  auto element = new DataElement(tag, VR::UNKNOWN, byte_order_);
  handler_->OnSequenceItemEnd(element);
}

void DicomReader::ReadSeqItemPrefixTag(Reader& reader, Tag tag,
                                       std::uint32_t& read_length) {
  LOG_INFO("Read sequence item start/prefix tag.");

  std::uint32_t item_length = 0;
  ReadUint32(reader, &item_length);

  read_length += 4;

  auto element = new DataElement(tag, VR::UNKNOWN, byte_order_);

  // If item length is undefined, this item will be ended with a delimitation.
  element->set_length(item_length);

  handler_->OnSequenceItemStart(element);

  read_length += Read(reader, item_length);

  handler_->OnSequenceItemEnd();
}

bool DicomReader::ReadVR(Reader& reader, Tag tag, std::uint32_t& read_length,
                         VR* vr) {
  if (explicit_vr_) {
    std::string vr_str;
    reader.ReadString(&vr_str, 2);
    read_length += 2;

    if (!StringToVR(vr_str, vr)) {
      LOG_ERRO("%s is not a VR!", vr_str.c_str());
      return false;
    }
  } else {
    if (tag.element() == 0) {
      // Group Length (VR type is always UL).
      *vr = VR::UL;
    } else {
      // Query VR type from data dictionary.
      *vr = DataDict::GetVR(tag);

      // TODO: Private tags in implicit VR?
      if (*vr == VR::UNKNOWN) {
        LOG_ERRO("Private tags in implicit VR.");
        return false;
      }
    }
  }

  return true;
}

std::uint32_t DicomReader::ReadValueLength(Reader& reader, VR vr,
                                           std::uint32_t& read_length) {
  std::uint32_t vl32 = 0;

  if (explicit_vr_) {
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
      if (Is16BitsFollowingVrReversed(vr)) {
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
    auto data_sequence = new DataSequence(tag, explicit_vr_, byte_order_);
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

      if (transfer_syntax_uid_.empty() && tag == kTransferSyntaxTag) {
        element->GetString(&transfer_syntax_uid_);
      }

      // Call handler as the last step since it might delete the element.
      handler_->OnElementEnd(element);

    } else {
      if (transfer_syntax_uid_.empty() && tag == kTransferSyntaxTag) {
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
