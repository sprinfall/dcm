#include "dcm/dicom_reader.h"

#include "boost/filesystem/fstream.hpp"

#include "dcm/data_dict.h"
#include "dcm/data_sequence.h"
#include "dcm/data_set.h"
#include "dcm/logger.h"
#include "dcm/reader.h"
#include "dcm/read_handler.h"
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

bool CheckEndian(Reader& reader, Endian* endian) {
  std::uint8_t tag_bytes[4] = { 0 };
  if (!reader.ReadBytes(&tag_bytes, 4)) {
    return false;
  }

  reader.UndoRead(4);  // Put it back.

  std::uint16_t group = (tag_bytes[0] & 0xff) + ((tag_bytes[1] & 0xff) << 8);
  std::uint16_t element = (tag_bytes[2] & 0xff) + ((tag_bytes[3] & 0xff) << 8);
  Tag tag_l(group, element);  // Little endian
  Tag tag_b = tag_l.SwapBytes();  // Big endian

  const DataEntry* entry_l = DataDict::GetEntry(tag_l);
  const DataEntry* entry_b = DataDict::GetEntry(tag_b);

  if (entry_l == nullptr && entry_b == nullptr) {
    if (element == 0) {
      // Group Length tag not in data dictionary, check the group number.
      // For the first tag, group number is more probable of 0008 than 0800.
      if (tag_l.group() > 0xff && tag_b.group() <= 0xff) {
        *endian = Endian::Big();
      } else {
        *endian = Endian::Little();
      }
    } else {
      // Both tags show an error, an invalid tag is encountered.
      // Assume that it's Little Endian.
      *endian = Endian::Little();
    }
  } else {
    if (entry_l == nullptr) {
      *endian = Endian::Big();
    } else if (entry_b == nullptr) {
      *endian = Endian::Little();
    } else {
      // Both tags are valid, check the group number.
      // For the first tag, group number is more probable of 0008 than 0800.
      if (tag_l.group() > 0xff && tag_b.group() <= 0xff) {
        *endian = Endian::Big();
      } else {
        *endian = Endian::Little();
      }
    }
  }

  LOG_INFO("Endian checked: %s", endian->name());

  return true;
}

}  // namespace

// -----------------------------------------------------------------------------

DicomReader::DicomReader(ReadHandler* handler)
    : handler_(handler), explicit_vr_(true) {
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

  // Group 0x0002 is always little endian, so don't check endian type now.
  if (!CheckVrExplicity(reader, &explicit_vr_)) {
    return false;
  }

  handler_->OnExplicitVR(explicit_vr_);

  Read(reader, kUndefinedLength, true);

  return true;
}

std::uint32_t DicomReader::Read(Reader& reader, std::size_t max_length,
                                bool check_endian) {
  std::uint32_t read_length = 0;
  bool endian_checked = false;

  Tag tag;

  while (read_length < max_length) {
    if (handler_->should_stop()) {
      break;  // Handler required to stop reading.
    }

    // ISSUE:
    // Reading tag depends on the default endian type. If a DICOM file is big
    // endian and without group 0x0002, this doesn't work.
    if (!ReadTag(reader, &tag)) {
      break;
    }

    read_length += 4;

    // TODO: Check Transfer Syntax UID (0x00020010).

    if (check_endian && !endian_checked) {
      // Group 0x0002 is always little endian.
      if (tag.group() != 0x0002) {
        reader.UndoRead(4);  // Undo read tag.

        CheckEndian(reader, &endian_);

        handler_->OnEndian(endian_);

        // Sometimes, VR is explicit in group 0x0002 while implicit in others.
        // E.g., CS7600 generated DICOM files.
        // TODO: Add explicit_vr_0002_?
        CheckVrExplicity(reader, &explicit_vr_);

        handler_->OnExplicitVR(explicit_vr_);

        endian_checked = true;
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

    std::uint32_t vl32 = ReadValueLength(reader, vr, read_length);

    if (!ReadValue(reader, tag, vr, vl32, read_length)) {
      break;
    }
  }

  return read_length;
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
    AdjustBytesUint16(*value);
    return true;
  }
  return false;
}

bool DicomReader::ReadUint32(Reader& reader, std::uint32_t* value) {
  if (reader.ReadUint32(value)) {
    AdjustBytesUint32(*value);
    return true;
  }
  return false;
}

void DicomReader::AdjustBytesUint16(std::uint16_t& value) const {
  if (endian_ != kOSEndian) {
    value = SwapUint16(value);
  }
}

void DicomReader::AdjustBytesUint32(std::uint32_t& value) const {
  if (endian_ != kOSEndian) {
    value = SwapUint32(value);
  }
}

void DicomReader::ReadSeqEndTag(Reader& reader, Tag tag,
                                std::uint32_t& read_length) {
  LOG_INFO("Read sequence end/delimitation tag.");

  // Skip the 4-byte zero length of this sequence delimitation item.
  reader.Seek(4, std::ios::cur);
  read_length += 4;

  DataElement* element = new DataElement(tag, VR::UNKNOWN, endian_);
  handler_->OnSequenceEnd(element);
}

void DicomReader::ReadSeqItemEndTag(Reader& reader, Tag tag,
                                    std::uint32_t& read_length) {
  LOG_INFO("Read sequence item end/delimitation tag.");

  // Skip the 4-byte zero length of this item delimitation item.
  reader.Seek(4, std::ios::cur);
  read_length += 4;

  DataElement* element = new DataElement(tag, VR::UNKNOWN, endian_);
  handler_->OnSequenceItemEnd(element);
}

void DicomReader::ReadSeqItemPrefixTag(Reader& reader, Tag tag,
                                       std::uint32_t& read_length) {
  LOG_INFO("Read sequence item start/prefix tag.");

  std::uint32_t item_length = 0;
  ReadUint32(reader, &item_length);
  read_length += 4;

  DataElement* element = new DataElement(tag, VR::UNKNOWN, endian_);
  element->set_length(item_length);

  handler_->OnSequenceItemStart(element);

  // If the `item_length` is undefined, this item will be ended with a
  // delimitation tag.

  read_length += Read(reader, item_length, false);

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

bool DicomReader::ReadValue(Reader& reader, Tag tag, VR vr, std::uint32_t vl32,
                            std::uint32_t& read_length) {
  if (vr == VR::SQ) {
    DataSequence* data_sequence = new DataSequence(tag, endian_);
    data_sequence->set_length(vl32);

    handler_->OnSequenceStart(data_sequence);

    if (vl32 > 0) {
      read_length += Read(reader, vl32, false);
    }

    handler_->OnSequenceEnd();

  } else {
    if (vl32 == kUndefinedLength) {
      LOG_ERRO("Non-SQ element with undefined length.");
      return false;
    }

    read_length += vl32;

    if (handler_->OnElementStart(tag)) {
      DataElement* element = new DataElement(tag, vr, endian_);

      if (vl32 > 0) {
        Buffer buffer(vl32);

        if (reader.ReadBytes(&buffer[0], vl32) != vl32) {
          LOG_ERRO("Failed to read value of size: %u", vl32);
          delete element;
          return false;
        }

        element->set_buffer(std::move(buffer));
      }

      handler_->OnElementEnd(element);

    } else {
      // Just skip the buffer.
      if (vl32 > 0) {
        reader.Seek(vl32, std::ios::cur);
      }
    }
  }

  return true;
}

}  // namespace dcm
