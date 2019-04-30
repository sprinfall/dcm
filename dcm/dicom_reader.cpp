#include "dcm/dicom_reader.h"

#include <iostream>

#include "dcm/data_dict.h"
#include "dcm/data_element.h"
#include "dcm/data_set.h"
#include "dcm/file_reader.h"
#include "dcm/read_handler.h"
#include "dcm/util.h"

namespace dcm {

////////////////////////////////////////////////////////////////////////////////

static bool CheckVrExplicity(Reader& reader, bool* explicit_vr) {
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

  return true;
}

static bool CheckEndianType(Reader& reader, Endian* endian) {
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

  if (entry_l == NULL && entry_b == NULL) {
    if (element == 0) {
      // Group Length tag not in data dictionary, check the group number.
      // For the first tag, group number is more probable of 0008 than 0800.
      if (tag_l.group() > 0xff && tag_b.group() <= 0xff) {
        *endian = kBigEndian;
      } else {
        *endian = kLittleEndian;
      }
    } else {
      // Both tags show an error, an invalid tag is encountered.
      // Assume that it's Little Endian.
      *endian = kLittleEndian;
    }
  } else {
    if (entry_l == NULL) {
      *endian = kBigEndian;
    } else if (entry_b == NULL) {
      *endian = kLittleEndian;
    } else {
      // Both tags are valid, check the group number.
      // For the first tag, group number is more probable of 0008 than 0800.
      if (tag_l.group() > 0xff && tag_b.group() <= 0xff) {
        *endian = kBigEndian;
      } else {
        *endian = kLittleEndian;
      }
    }
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////

DicomReader::DicomReader(ReadHandler* handler)
    : handler_(handler)
    , endian_(kLittleEndian)
    , explicit_vr_(true) {
}

bool DicomReader::ReadFile(const boost::filesystem::path& path) {
  FileReader reader(path);
  if (!reader.IsOk()) {
    return false;
  }
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

  // NOTE:
  // The 0002 group will always be Little Endian even if the DICOM file
  // is Big Endian. So don't check endian type right now.
  if (!CheckVrExplicity(reader, &explicit_vr_)) {
    return false;
  }

  // Tell read handler.
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

    if (!ReadTag(reader, &tag)) {
      break;
    }

    read_length += 4;

    if (check_endian) {
      // The 0002 group will always be Little Endian even if the DICOM file
      // is Big Endian.
      if (!endian_checked && tag.group() != 2) {
        reader.UndoRead(4);  // Undo read tag.

        CheckEndianType(reader, &endian_);

        // Tell read handler the endian type.
        handler_->OnEndian(endian_);

        // Some DICOM files, VR is explicit in 0x0002 group while implicit
        // in others. E.g., CS7600 generated DICOM files.
        // TODO: Add explicit_vr_0002_?
        CheckVrExplicity(reader, &explicit_vr_);

        // Tell read handler.
        handler_->OnExplicitVR(explicit_vr_);

        // TODO: Check Transfer Syntax UID tag.
        //std::string ts;
        //GetString(Tag(0x0002, 0x0010), &ts);

        endian_checked = true;
        continue;
      }
    }

    // End of sequence itself.
    if (tag == kSeqEndTag) {
      // Skip the 4-byte zero length of this sequence delimitation item.
      //std::cout << "Sequence delimitation item." << std::endl;
      reader.Seek(4, SEEK_CUR);
      read_length += 4;

      if (handler_->OnElementStart(tag)) {
        handler_->OnElementEnd(new DataElement(tag, VR::UNKNOWN, endian_));
      }

      break;
    }

    // End of sequence item.
    if (tag == kSeqItemEndTag) {
      // Skip the 4-byte zero length of this item delimitation item.
      //std::cout << "Item delimitation item." << std::endl;
      reader.Seek(4, SEEK_CUR);
      read_length += 4;

      if (handler_->OnElementStart(tag)) {
        handler_->OnElementEnd(new DataElement(tag, VR::UNKNOWN, endian_));
      }

      continue;
    }

    if (tag == kSeqItemPrefixTag) {
      std::uint32_t item_length = 0;
      ReadUint32(reader, &item_length);
      read_length += 4;

      if (handler_->OnElementStart(tag)) {
        DataElement* element = new DataElement(tag, VR::UNKNOWN, endian_);
        element->set_length(item_length);
        handler_->OnElementEnd(element);
      }

      // NOTE: Ignore item_length because:
      //   if (item_length == kUndefinedLength) {
      //     There will be Item Delimitation tag later.
      //   } else {
      //     if (length == kUndefinedLength) {
      //       There will be Sequence Delimitation tag later.
      //     } else {
      //       Will end when !(read_length < length).
      //     }
      //   }

      continue;
    }

    VR vr = VR::UNKNOWN;

    if (explicit_vr_) {
      std::string vr_str;
      reader.ReadString(&vr_str, 2);
      read_length += 2;

      if (!StringToVR(vr_str, &vr)) {
        std::cerr << vr_str << " is not a VR!" << std::endl;
        break;
      }
    } else {
      if (tag.element() == 0) {
        // Group Length (VR type is always UL).
        vr = VR::UL;
      } else {
        // Query VR type from data dictionary.
        const DataEntry* data_entry = DataDict::GetEntry(tag);
        if (data_entry != NULL) {
          vr = data_entry->vr;
        } else {
          // TODO: How to handle private tags in implicit VR?
          std::cerr << "Error: Private tags in implicit VR." << std::endl;
          break;
        }
      }
    }

    // Value length.
    std::uint32_t vl32 = 0;

    if (explicit_vr_) {
      // For VRs of OB, OD, OF, OL, OW, SQ, UN and UC, UR, UT, the 16 bits
      // following the two character VR Field are reserved for use by later
      // versions of the DICOM Standard.
      // See PS 3.5 Section 7.1.2 - Data Element Structure with Explicit VR.

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
          //std::cout << "2 bytes following VR are reserved." << std::endl;

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

    if (vr == VR::SQ) {
      DataSet* data_set = new DataSet(tag, endian_);
      data_set->set_explicit_vr(explicit_vr_);
      data_set->set_length(vl32);

      handler_->OnSeqElementStart(data_set);

      if (vl32 > 0) {
        read_length += Read(reader, vl32, /*check_endian*/false);
      }

      handler_->OnSeqElementEnd(data_set);

    } else {
      if (vl32 == kUndefinedLength) {
        // Non-SQ element with undefined length.
        std::cerr << "Error: Non-SQ element with undefined length." << std::endl;
        break;  // TODO: return -1 to indicate the error?
      }

      Buffer buffer(vl32);
      if (vl32 > 0) {
        if (reader.ReadBytes(&buffer[0], vl32) != vl32) {
          std::cerr << "Error: Failed to read value of size: "
                    << vl32 << std::endl;
          break;  // TODO: return -1 to indicate the error?
        }
      }

      read_length += vl32;

      if (handler_->OnElementStart(tag)) {
        DataElement* element = new DataElement(tag, vr, endian_);
        element->set_buffer(std::move(buffer), vl32);

        handler_->OnElementEnd(element);
      }
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
  if (endian_ != PlatformEndian()) {
    value = SwapUint16(value);
  }
}

void DicomReader::AdjustBytesUint32(std::uint32_t& value) const {
  if (endian_ != PlatformEndian()) {
    value = SwapUint32(value);
  }
}

}  // namespace dcm
