#ifndef DCM_DEFS_H_
#define DCM_DEFS_H_

#include <cstdint>

#include "boost/filesystem/path.hpp"

#include "dcm/config.h"

namespace dcm {

// -----------------------------------------------------------------------------

using float32_t = float;
using float64_t = double;

using Path = boost::filesystem::path;

const std::uint32_t kUndefinedLength = 0xFFFFFFFF;

// -----------------------------------------------------------------------------

enum class ByteOrder : char {
  LE = 'l',
  BE = 'b',
};

// The byte order of this OS/platform.
extern const ByteOrder kByteOrderOS;

// -----------------------------------------------------------------------------

#define MAKE_VR_CODE(c0, c1) ((((int)c0) << 8) | (int)c1)

// Value Representation.
// See: PS 3.5 Section 6.2 - Value Representation (VR).
class VR {
public:
  enum Type {
    IMPLICIT,
    EXPLICIT,
  };

  enum Code {
    // Total number: 31
    AE = MAKE_VR_CODE('A', 'E'),  // Application Entity
    AS = MAKE_VR_CODE('A', 'S'),  // Age String
    AT = MAKE_VR_CODE('A', 'T'),  // Attribute Tag
    CS = MAKE_VR_CODE('C', 'S'),  // Code String
    DA = MAKE_VR_CODE('D', 'A'),  // Date
    DS = MAKE_VR_CODE('D', 'S'),  // Decimal String
    DT = MAKE_VR_CODE('D', 'T'),  // Date Time
    FL = MAKE_VR_CODE('F', 'L'),  // Floating Point Single
    FD = MAKE_VR_CODE('F', 'D'),  // Floating Point Double
    IS = MAKE_VR_CODE('I', 'S'),  // Integer String
    LO = MAKE_VR_CODE('L', 'O'),  // Long String
    LT = MAKE_VR_CODE('L', 'T'),  // Long Text
    OB = MAKE_VR_CODE('O', 'B'),  // Other Byte
    OD = MAKE_VR_CODE('O', 'D'),  // Other Double
    OF = MAKE_VR_CODE('O', 'F'),  // Other Float
    OL = MAKE_VR_CODE('O', 'L'),  // Other Long
    OW = MAKE_VR_CODE('O', 'W'),  // Other Word
    PN = MAKE_VR_CODE('P', 'N'),  // Person Name
    SH = MAKE_VR_CODE('S', 'H'),  // Short String
    SL = MAKE_VR_CODE('S', 'L'),  // Signed Long
    SQ = MAKE_VR_CODE('S', 'Q'),  // Sequence of Items
    SS = MAKE_VR_CODE('S', 'S'),  // Signed Short
    ST = MAKE_VR_CODE('S', 'T'),  // Short Text
    TM = MAKE_VR_CODE('T', 'M'),  // Time
    UC = MAKE_VR_CODE('U', 'C'),  // Unlimited Characters
    UI = MAKE_VR_CODE('U', 'I'),  // Unique Identifier (UID)
    UL = MAKE_VR_CODE('U', 'L'),  // Unsigned Long
    UN = MAKE_VR_CODE('U', 'N'),  // Unknown
    UR = MAKE_VR_CODE('U', 'R'),  // Universal Resource Identifier
    US = MAKE_VR_CODE('U', 'S'),  // Unsigned Short
    UT = MAKE_VR_CODE('U', 'T'),  // Unlimited Text
  };

public:
  VR(Code code = UN) : code_(code) {}

  explicit VR(const char bytes[2]);

  Code code() const { return code_; }
  void set_code(Code code) { code_ = code; }

  char byte1() const {
    return (((std::uint16_t)code_) & 0xFF00) >> 8;
  }
  char byte2() const {
    return (((std::uint16_t)code_) & 0x00FF);
  }

  // Set with bytes.
  // Return false (the original code keeps unchanged) if the bytes is
  // not a valid VR.
  bool SetBytes(const char bytes[2]);

  bool IsUnknown() const { return code_ == UN; }

  // For OB, OD, OF, OL, OW, SQ, UN and UC, UR, UT, the 16 bits following the
  // two character VR Field are reserved for use by later versions of the DICOM
  // Standard.
  // See: PS 3.5 Section 7.1.2 - Data Element Structure with Explicit VR
  bool Is16BitsFollowingReversed() const;

  bool IsString() const;

  // Is VM determined by back slash?
  bool IsBackSlashVM() const;

private:
  Code code_;
};

inline bool operator==(VR lhs, VR rhs) {
  return lhs.code() == rhs.code();
}

inline bool operator!=(VR lhs, VR rhs) {
  return lhs.code() != rhs.code();
}

// -----------------------------------------------------------------------------

class Tag {
public:
  Tag() = default;

  Tag(const Tag&) = default;
  Tag& operator=(const Tag&) = default;

  Tag(std::uint16_t group, std::uint16_t element)
      : group_(group), element_(element) {
  }

  Tag(std::uint32_t tag_key)
      : group_((tag_key >> 16) & 0xFFFF), element_(tag_key & 0xFFFF) {
  }

  std::uint16_t group() const { return group_; }
  void set_group(std::uint16_t group) { group_ = group; }

  std::uint16_t element() const { return element_; }
  void set_element(std::uint16_t element) { element_ = element; }

  bool empty() const { return group_ == 0 && element_ == 0; }

  Tag SwapBytes() const;

  // Convert to a 4-byte unsigned integer.
  std::uint32_t ToUint32() const {
    return (group_ << 16) + element_;
  }

  void Print(std::ostream& os, bool uppercase = true, bool as_uint32 = false,
             const char* separator = ",") const;

private:
  std::uint16_t group_;
  std::uint16_t element_;
};

inline bool operator==(Tag lhs, Tag rhs) {
  return lhs.ToUint32() == rhs.ToUint32();
}

inline bool operator!=(Tag lhs, Tag rhs) {
  return lhs.ToUint32() != rhs.ToUint32();
}

inline bool operator<(Tag lhs, Tag rhs) {
  return lhs.ToUint32() < rhs.ToUint32();
}

inline bool operator>(Tag lhs, Tag rhs) {
  return lhs.ToUint32() > rhs.ToUint32();
}

inline bool operator<=(Tag lhs, Tag rhs) {
  return lhs.ToUint32() <= rhs.ToUint32();
}

inline bool operator>=(Tag lhs, Tag rhs) {
  return lhs.ToUint32() >= rhs.ToUint32();
}

// -----------------------------------------------------------------------------

namespace tags {

// Commonly used tags.
// This is not meant to be a full list.

const Tag kSeqDelimatation            = 0xFFFEE0DD;
const Tag kSeqItemPrefix              = 0xFFFEE000;
const Tag kSeqItemDelimatation        = 0xFFFEE00D;

// 0x0002
const Tag kTransferSyntaxUID          = 0x00020010;  // UI
const Tag kImplementationClassUID     = 0x00020012;  // UI
const Tag kSourceAETitle              = 0x00020016;  // AE
const Tag kSendingAETitle             = 0x00020017;  // AE

// 0x0008
const Tag kSpecificCharacterSet       = 0x00080005;  // CS
const Tag kImageType                  = 0x00080008;  // CS
const Tag kSOPClassUID                = 0x00080016;  // UI
const Tag kSOPInstanceUID             = 0x00080018;  // UI
const Tag kStudyDate                  = 0x00080020;  // DA
const Tag kSeriesDate                 = 0x00080021;  // DA
const Tag kAcquisitionDate            = 0x00080022;  // DA
const Tag kAcquisitionDateTime        = 0x0008002A;  // DT
const Tag kStudyTime                  = 0x00080030;  // TM
const Tag kSeriesTime                 = 0x00080031;  // TM
const Tag kAcquisitionTime            = 0x00080032;  // TM
const Tag kAccessionNumber            = 0x00080050;  // SH
const Tag kQueryRetrieveLevel         = 0x00080052;  // CS
const Tag kModality                   = 0x00080060;  // CS
const Tag kStudyDescription           = 0x00081030;  // LO
const Tag kSeriesDescription          = 0x0008103E;  // LO

// 0x0010
const Tag kPatientName                = 0x00100010;  // PN
const Tag kPatientID                  = 0x00100020;  // LO
const Tag kPatientBirthDate           = 0x00100030;  // DA
const Tag kPatientBirthTime           = 0x00100032;  // TM
const Tag kPatientSex                 = 0x00100040;  // CS
const Tag kPatientAge                 = 0x00101010;  // AS
const Tag kPregnancyStatus            = 0x001021C0;  // US

// 0x0020
const Tag kImageComments              = 0x00204000;  // LT

// 0x0028
const Tag kSamplesPerPixel            = 0x00280002;  // US
const Tag kPhotometricInterpretation  = 0x00280004;  // CS
const Tag kNumberOfFrames             = 0x00280008;  // IS
const Tag kRows                       = 0x00280010;  // US
const Tag kColumns                    = 0x00280011;  // US
const Tag kPixelSpacing               = 0x00280030;  // DS
const Tag kBitsAllocated              = 0x00280100;  // US
const Tag kBitsStored                 = 0x00280101;  // US
const Tag kWindowCenter               = 0x00281050;  // DS
const Tag kWindowWidth                = 0x00281051;  // DS

// 0x0074
const Tag kReceivingAE                = 0x00741234;  // AE
const Tag kRequestingAE               = 0x00741236;  // AE

const Tag kPixelData                  = 0x7FE00010;  // OB

}  // namespace tags

// -----------------------------------------------------------------------------

namespace transfer_syntax_uids {

// NOTE: This is not the full list!

// Implicit VR Little Endian
// Default Transfer Syntax for DICOM
const char* const kImplicitLittleEndian = "1.2.840.10008.1.2";

// Explicit VR Little Endian
const char* const kExplicitLittleEndian = "1.2.840.10008.1.2.1";

// Deflated Explicit VR Little Endian
const char* const kDeflatedExplicitLittleEndian = "1.2.840.10008.1.2.1.99";

// Explicit VR Big Endian - RETIRED
const char* const kExplicitBigEndian = "1.2.840.10008.1.2.2";

// JPEG Baseline (Process 1)
// Default Transfer Syntax for Lossy JPEG 8 Bit Image Compression
const char* const kJpegBaselineProcess1 = "1.2.840.10008.1.2.4.50";

// JPEG Extended (Process 2 & 4)
// Default Transfer Syntax for Lossy JPEG 12 Bit Image Compression (Process
// 4 only)
const char* const kJpegExtendedProcess2_4 = "1.2.840.10008.1.2.4.51";

// JPEG Lossless, Non-Hierarchical (Process 14)
const char* const kJpegLosslessNHProcess14 = "1.2.840.10008.1.2.4.57";

// JPEG Lossless, Non-Hierarchical, First-Order Prediction (Process 14
// [Selection Value 1])
// Default Transfer Syntax for Lossless JPEG Image Compression
const char* const kJpegLosslessNHFOPProcess14SV1 = "1.2.840.10008.1.2.4.70";

// JPEG-LS Lossless Image Compression
const char* const kJpegLSLossless = "1.2.840.10008.1.2.4.80";

// JPEG-LS Lossy (Near-Lossless) Image Compression
const char* const kJpegLSLossy = "1.2.840.10008.1.2.4.81";

// JPEG 2000 Image Compression (Lossless Only)
const char* const kJpeg2000LosslessOnly = "1.2.840.10008.1.2.4.90";

// JPEG 2000 Image Compression (Lossless or Lossy)
const char* const kJpeg2000 = "1.2.840.10008.1.2.4.91";

}  // transfer_syntax_uids

}  // namespace dcm

#endif  // DCM_DEFS_H_
