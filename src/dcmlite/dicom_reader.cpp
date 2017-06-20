#include "dcmlite/dicom_reader.h"

#include <iostream>

#include "dcmlite/data_element.h"
#include "dcmlite/data_dictionary.h"
#include "dcmlite/data_set.h"
#include "dcmlite/file.h"
#include "dcmlite/read_handler.h"

namespace dcmlite {

////////////////////////////////////////////////////////////////////////////////

static const Tag kSeqEndTag(0xFFFE, 0xE0DD);
static const Tag kSeqItemEndTag(0xFFFE, 0xE00D);
static const Tag kSeqItemPrefixTag(0xFFFE, 0xE000);

static bool Is16BitsFollowingVrReversed(VR::Type vr_type) {
  static const VR::Type kVRs[] = {
    VR::OB,
    VR::OD,
    VR::OF,
    VR::OL,
    VR::OW,
    VR::SQ,
    VR::UN,
    VR::UC,
    VR::UR,
    VR::UT,
  };

  size_t size = sizeof(kVRs) / sizeof(VR::Type);

  for (size_t i = 0; i < size; ++i) {
    if (vr_type == kVRs[i]) {
      return true;
    }
  }

  return false;
}

static bool CheckVrExplicity(File& file, bool* explicit_vr) {
  // Skip the 4 tag bytes.
  if (!file.Seek(4, SEEK_CUR)) {
    return false;
  }

  std::string vr_str;
  if (!file.ReadString(&vr_str, 2)) {
    file.UndoRead(4);  // Put 4 tag bytes back.
    return false;
  }

  file.UndoRead(6);  // Put it back.

  // Check to see if the 2 bytes following the tag field represents a valid VR.
  if (VR::FromString(vr_str, NULL)) {
    *explicit_vr = true;
  } else {
    *explicit_vr = false;
  }

  return true;
}

static bool CheckEndianType(File& file, Endian* endian) {
  std::uint8_t tag_bytes[4] = { 0 };
  if (!file.ReadBytes(&tag_bytes, 4)) {
    return false;
  }

  file.UndoRead(4);  // Put it back.

  std::uint16_t group = (tag_bytes[0] & 0xff) + ((tag_bytes[1] & 0xff) << 8);
  std::uint16_t element = (tag_bytes[2] & 0xff) + ((tag_bytes[3] & 0xff) << 8);
  Tag tag_l(group, element);  // Little endian
  Tag tag_b = tag_l.ReverseBytes();  // Big endian

  const DataEntry* entry_l = DataDictionary::Get().FindEntry(tag_l);
  const DataEntry* entry_b = DataDictionary::Get().FindEntry(tag_b);

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

DicomReader::DicomReader() {
}

bool DicomReader::ReadFile(const std::string& file_path) {
  File file;
  if (!file.Open(file_path.c_str(), "rb")) {
    return false;
  }

  // Preamble

  file.Seek(128);

  std::string prefix;
  if (!file.ReadString(&prefix, 4)) {
    return false;  // TODO: Use exception?
  }

  if (prefix != "DICM") {
    // Preamble is omitted.
    file.UndoRead(132);
  }

  // NOTE:
  // The 0002 group will always be Little Endian even if the DICOM file
  // is Big Endian. So don't check endian type right now.
  if (!CheckVrExplicity(file, &explicit_vr_)) {
    return false;
  }

  ReadFile(file, kUndefinedLength, true);

  return true;
}

std::uint32_t DicomReader::ReadFile(File& file,
                                    size_t max_length,
                                    bool check_endian) {
  std::uint32_t read_length = 0;

  bool endian_checked = false;

  Tag tag;

  while (read_length < max_length) {
    if (!ReadTag(file, &tag)) {
      break;
    }

    read_length += 4;

    if (check_endian) {
      if (!endian_checked && tag.group() != 2) {
        file.UndoRead(4);  // Undo read tag.

        CheckEndianType(file, &endian_);

        // Some DICOM files, VR is explicit in 0x0002 group while implicit
        // in others. E.g., CS7600 generated DICOM files.
        CheckVrExplicity(file, &explicit_vr_);

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
      file.Seek(4, SEEK_CUR);
      read_length += 4;

      handler_->OnElement(new DataElement(tag, VR::UNKNOWN));

      break;
    }

    // End of sequence item.
    if (tag == kSeqItemEndTag) {
      // Skip the 4-byte zero length of this item delimitation item.
      //std::cout << "Item delimitation item." << std::endl;
      file.Seek(4, SEEK_CUR);
      read_length += 4;

      handler_->OnElement(new DataElement(tag, VR::UNKNOWN));

      continue;
    }

    if (tag == kSeqItemPrefixTag) {
      std::uint32_t item_length = 0;
      ReadUint32(file, &item_length);
      read_length += 4;

      handler_->OnElement(new DataElement(tag, VR::UNKNOWN));

      // NOTE: Ignore item_length because:
      // if (item_length == kUndefinedLength) {
      //   There will be Item Delimitation tag later.
      // } else {
      //   if (length == kUndefinedLength) {
      //     There will be Sequence Delimitation tag later.
      //   } else {
      //     Will end when !(read_length < length).
      //   }
      // }

      continue;
    }

    VR::Type vr_type = VR::UNKNOWN;

    if (explicit_vr_) {
      std::string vr_str;
      file.ReadString(&vr_str, 2);
      read_length += 2;

      if (!VR::FromString(vr_str, &vr_type)) {
        std::cerr << vr_str << " isn't a valid VR!" << std::endl;
        break;
      }
    } else {
      if (tag.element() == 0) {
        // Group Length (VR type is always UL).
        vr_type = VR::UL;
      } else {
        // Query VR type from data dictionary.
        const DataEntry* data_entry = DataDictionary::Get().FindEntry(tag);
        if (data_entry != NULL) {
          vr_type = data_entry->vr_type;
        } else {
          // TODO: How to handle private tags in implicit VR?
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
      ReadUint16(file, &vl16);
      read_length += 2;

      if (vl16 != 0) {
        vl32 = vl16;
      } else {
        // The 16 bits after VR Field are 0, but we can't conclude that they
        // are reserved since the value length might actually be 0.
        // Check the VR to confirm it.
        if (Is16BitsFollowingVrReversed(vr_type)) {
          //std::cout << "2 bytes following VR are reserved." << std::endl;

          // This 2 bytes are reserved, read the 4-byte value length.
          ReadUint32(file, &vl32);
          read_length += 4;
        } else {
          // Value length is 0.
          vl32 = 0;
        }
      }
    } else {  // Implicit VR
      ReadUint32(file, &vl32);
      read_length += 4;
    }

    if (vr_type == VR::SQ) {
      DataSet* data_set = new DataSet(tag);
      data_set->set_endian(endian_);
      data_set->set_explicit_vr(explicit_vr_);
      data_set->set_length(vl32);

      handler_->OnSeqElementStart(data_set);

      if (vl32 > 0) {
        read_length += ReadFile(file, vl32, /*check_endian*/false);
      }

      handler_->OnSeqElementEnd(data_set);

    } else {
      if (vl32 == kUndefinedLength) {
        // Non-SQ element with undefined length.
        std::cerr << "Error: Non-SQ element with undefined length." << std::endl;
        break;  // TODO: return -1 to indicate the error?
      }

      boost::shared_array<char> buffer(new char[vl32]);
      if (file.ReadBytes(buffer.get(), vl32) != vl32) {
        std::cerr << "Error: Failed to read value of size: " << vl32 << std::endl;
        break;  // TODO: return -1 to indicate the error?
      }

      read_length += vl32;

      DataElement* element = new DataElement(tag, vr_type);
      element->SetBuffer(buffer, vl32);

      handler_->OnElement(element);
    }
  }

  return read_length;
}

bool DicomReader::ReadTag(File& file, Tag* tag) {
  std::uint16_t group = 0;
  std::uint16_t element = 0;

  if (!ReadUint16(file, &group)) {
    return false;
  }

  if (!ReadUint16(file, &element)) {
    return false;
  }

  tag->set_group(group);
  tag->set_element(element);

  return true;
}

bool DicomReader::ReadUint16(File& file, std::uint16_t* value) {
  if (file.ReadUint16(value)) {
    AdjustBytesUint16(*value);
    return true;
  }
  return false;
}

bool DicomReader::ReadUint32(File& file, std::uint32_t* value) {
  if (file.ReadUint32(value)) {
    AdjustBytesUint32(*value);
    return true;
  }
  return false;
}

void DicomReader::AdjustBytesUint16(std::uint16_t& value) const {
  if (endian_ != PlatformEndian()) {
    value = ReverseBytesUint16(value);
  }
}

void DicomReader::AdjustBytesUint32(std::uint32_t& value) const {
  if (endian_ != PlatformEndian()) {
    value = ReverseBytesUint32(value);
  }
}

}  // namespace dcmlite
