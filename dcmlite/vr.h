#ifndef DCMLITE_VR_H_
#define DCMLITE_VR_H_

// Value Representation.
// See PS 3.5 Section 6.2 - Value Representation (VR).

#include "dcmlite/defs.h"

namespace dcmlite {

#define MAKE_VR_ENUM(c0, c1) ((((int)c0) << 8) | (int)c1)

class VR {
public:
  enum Type {
    UNKNOWN = 0,
    AE = MAKE_VR_ENUM('A', 'E'),  // Application Entity
    AS = MAKE_VR_ENUM('A', 'S'),  // Age String
    AT = MAKE_VR_ENUM('A', 'T'),  // Attribute Tag
    CS = MAKE_VR_ENUM('C', 'S'),  // Code String
    DA = MAKE_VR_ENUM('D', 'A'),  // Date
    DS = MAKE_VR_ENUM('D', 'S'),  // Decimal String
    DT = MAKE_VR_ENUM('D', 'T'),  // Date Time
    FL = MAKE_VR_ENUM('F', 'L'),  // Floating Point Single
    FD = MAKE_VR_ENUM('F', 'D'),  // Floating Point Double
    IS = MAKE_VR_ENUM('I', 'S'),  // Integer String
    LO = MAKE_VR_ENUM('L', 'O'),  // Long String
    LT = MAKE_VR_ENUM('L', 'T'),  // Long Text
    OB = MAKE_VR_ENUM('O', 'B'),  // Other Byte
    OD = MAKE_VR_ENUM('O', 'D'),  // Other Double
    OF = MAKE_VR_ENUM('O', 'F'),  // Other Float
    OL = MAKE_VR_ENUM('O', 'L'),  // Other Long
    OW = MAKE_VR_ENUM('O', 'W'),  // Other Word
    PN = MAKE_VR_ENUM('P', 'N'),  // Person Name
    SH = MAKE_VR_ENUM('S', 'H'),  // Short String
    SL = MAKE_VR_ENUM('S', 'L'),  // Signed Long
    SQ = MAKE_VR_ENUM('S', 'Q'),  // Sequence of Items
    SS = MAKE_VR_ENUM('S', 'S'),  // Signed Short
    ST = MAKE_VR_ENUM('S', 'T'),  // Short Text
    TM = MAKE_VR_ENUM('T', 'M'),  // Time
    UC = MAKE_VR_ENUM('U', 'C'),  // Unlimited Characters
    UI = MAKE_VR_ENUM('U', 'I'),  // Unique Identifier (UID)
    UL = MAKE_VR_ENUM('U', 'L'),  // Unsigned Long
    UN = MAKE_VR_ENUM('U', 'N'),  // Unknown
    UR = MAKE_VR_ENUM('U', 'R'),  // Universal Resource Identifier / Locator (URI/URL)
    US = MAKE_VR_ENUM('U', 'S'),  // Unsigned Short
    UT = MAKE_VR_ENUM('U', 'T'),  // Unlimited Text
  };

  static std::string ToString(Type vr_type);

  // Convert a VR string to VR enum type.
  // Return false if the string is not a valid VR.
  static bool FromString(const std::string& vr_str, Type* vr_type = NULL);

  //static bool IsValidVR();

public:
  // Type enumeration.
  Type type;

  // The length of word; used for byte reversing in little/big endian conversion.
  // For numeric VRs only ((FL, FD, OL, OW, etc.).
  std::uint32_t word_size;

  // The maximum length of the data.
  std::uint32_t max_data_length;
};

}  // namespace dcmlite

#endif  // DCMLITE_VR_H_
