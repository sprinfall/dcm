#include "dcmlite/dicom_file.h"

#include <iostream>
#include <vector>

#include "dcmlite/defs.h"
#include "dcmlite/data_dictionary.h"
#include "dcmlite/file.h"
#include "dcmlite/tag.h"
#include "dcmlite/vr.h"

namespace dcmlite {

static const std::uint32_t kUndefinedLength = 0xFFFFFFFF;

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

DicomFile::DicomFile()
    : platform_endian_(GetPlatformEndian())
    , endian_(kLittleEndian)  // Little Endian is DICOM default.
    , explicit_vr_(true) {  // Explicit VR by default.
}  

DicomFile::~DicomFile() {
}

bool DicomFile::Read(const std::string& filename) {
  File file;
  if (!file.Open(filename.c_str(), "rb")) {
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

  ReadDataSet(file);

  return true;
}

// About Undefined Length
// - Sequence level: begin with SQ tag, end with (FFFE, E0DD).
// - Object/item level: begin with (FFFE, E000), end with (FFFE, E00D).

// 1. DICOM objects in SQ sequence are encoded as sequence items.
// 2. Each object-item in the SQ sequence is preceded by the (FFFE, E000) tag
//    (known as the item delimitation item). This item is followed by one of the
//    following:
//    i   Explicit length of the DICOM item (for example, first and second items
//        in example 1, Table 11). This length defines the number of bytes to be
//        read to get the following DICOM object.
//    ii  Implicit (undefined) length, set to hexadecimal FFFFFFFF (second item
//        in example 3, Table 11). In this case, we need to mark the end of the
//        DICOM object item with the item delimitation item (FFFE, E00D). This
//        delimiter item is followed by its zero length because it only marks the
//        end of the DICOM object and has no data of its own.
// 3. Similar to item, the entire SQ sequence can have explicit or undefined
//    length:
//    i   If explicit length is used (example 2, length 00000A00), this length is
//        equal to the total length of the encoded DICOM object sequence (immediately
//        following the length tag). In example 2 (Table 11), we read
//        00000A00 bytes and start breaking them into DICOM objects based on
//        (FFFE, E000) tags.
//    ii  We could use implicit (undefined) length instead, which in DICOM is
//        marked with FFFFFFFF. Because the length is not known, we have to
//        mark the end of the SQ sequence with the sequence delimitation item
//        (FFFE, E0DD) followed by its zero length.

void DicomFile::ReadDataSet(File& file) {
  // Check endian type and explicit VR or not.
  CheckTransferSyntax(file);

  Tag tag;

  while (true) {
    if (!ReadTag(file, &tag)) {
      // End of file.
      // TODO: Check EOF directly?
      break;
    }

    std::cout << tag << std::endl;

#if 0
    // TODO
    if (tag == Tag(0x0002, 0x0010)) {  // Transfer Syntax UID
    }
#endif

    // End of the sequence item.
    if (tag == Tag(0xFFFE, 0xE00D)) {
      // Skip the 4-byte zero length of this item delimitation item.
      file.Seek(4, SEEK_CUR);
      break;
    }

    // End of sequence itself.
    if (tag == Tag(0xFFFE, 0xE0DD)) {
      // Skip the 4-byte zero length of this sequence delimitation item.
      file.Seek(4, SEEK_CUR);
      break;
    }

    VR::Type vr_type = VR::UNKNOWN;

    if (explicit_vr_) {
      std::string vr_str;
      file.ReadString(&vr_str, 2);

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

    std::cout << "VR: " << VR::ToString(vr_type) << std::endl;

    // Value length.
    std::uint32_t vl32 = 0;

    if (explicit_vr_) {
      // For VRs of OB, OD, OF, OL, OW, SQ, UN and UC, UR, UT, the 16 bits
      // following the two character VR Field are reserved for use by later
      // versions of the DICOM Standard.
      // See PS 3.5 Section 7.1.2 - Data Element Structure with Explicit VR.

      std::uint16_t vl16 = 0;
      file.ReadUInt16(&vl16);

      if (vl16 != 0) {
        vl32 = vl16;
      } else {
        // The 16 bits after VR Field are 0, but we can't conclude that they
        // are reserved since the value length might actually be 0.
        // Check the VR to confirm it.
        if (Is16BitsFollowingVrReversed(vr_type)) {
          std::cout << "2 bytes following VR are reserved." << std::endl;

          // This 2 bytes are reserved, read the 4-byte value length.
          file.ReadUInt32(&vl32);
        } else {
          // Value length is 0.
          vl32 = 0;
        }
      }

    } else {  // Implicit VR
      file.ReadUInt32(&vl32);
    }

    if (vl32 == kUndefinedLength) {  // TODO
      std::cout << "Value length is undefined. (TODO)" << std::endl;
      break;
    } else {
      std::cout << "Value length: " << vl32 << std::endl;

      if (vl32 > 0) {
        // TEST
        if (vr_type == VR::LO) {
          std::string str;
          file.ReadString(&str, vl32);
          std::cout << "Value: " << str << std::endl;
        } else {
          file.Seek(vl32, SEEK_CUR);
        }
      }
    }

    std::cout << std::endl;
  }
}

bool DicomFile::CheckTransferSyntax(File& file) {
  std::uint8_t tag_bytes[4] = { 0 };
  std::string vr_str;

  if (!file.ReadBytes(&tag_bytes, 4)) {
    return false;
  }
  if (!file.ReadString(&vr_str, 2)) {
    return false;
  }

  file.UndoRead(6);  // Put it back.

  // Check to see if the 2 bytes following the tag field represents a valid VR.
  if (VR::FromString(vr_str, NULL)) {
    explicit_vr_ = true;
  } else {
    explicit_vr_ = false;
  }

  std::uint16_t group = (tag_bytes[0] & 0xff) + ((tag_bytes[1] & 0xff) << 8);
  std::uint16_t element = (tag_bytes[2] & 0xff) + ((tag_bytes[3] & 0xff) << 8);
  Tag tag_l(group, element);  // Little endian
  Tag tag_b = tag_l.ReverseBytes();  // Big endian

  const DataEntry* entry_l = DataDictionary::Get().FindEntry(tag_l);
  const DataEntry* entry_b = DataDictionary::Get().FindEntry(tag_b);

  if (entry_l == NULL && entry_b == NULL) {
    // Both tags show an error, an invalid tag is encountered.
    // Assume that it's Little Endian.
    endian_ = kLittleEndian;
  } else {
    if (entry_l == NULL) {
      endian_ = kBigEndian;
    } else if (entry_b == NULL) {
      endian_ = kLittleEndian;
    } else {
      // Both tags are valid, check the group number.
      // For the first tag, group number is more probable of 0008 than 0800.
      if (tag_l.group() > 0xff && tag_b.group() <= 0xff) {
        endian_ = kBigEndian;
      } else {
        endian_ = kLittleEndian;
      }
    }
  }

  return true;
}

bool DicomFile::ReadTag(File& file, Tag* tag) {
  std::uint16_t group = 0;
  std::uint16_t element = 0;

  if (!ReadUInt16(file, &group)) {
    return false;
  }

  if (!ReadUInt16(file, &element)) {
    return false;
  }

  tag->set_group(group);
  tag->set_element(element);

  return true;
}

bool DicomFile::ReadUInt16(File& file, std::uint16_t* value) {
  if (file.ReadBytes(value, 2) != 2) {
    return false;
  }

  if (endian_ != platform_endian_) {
    ReverseBytesUint16(*value);
  }

  return true;
}

//// NOTE: Byte order is not considered.
//bool DicomFile::ReadUInt32(std::uint32_t* value) {
//  return ReadBytes((char*)value, 4) == 4;
//}
//
//bool DicomFile::ReadString(std::string* value, size_t count) {
//  value->resize(count);
//  return file_.ReadBytes(&(*value)[0], count) == count;
//}

}  // namespace dcmlite
