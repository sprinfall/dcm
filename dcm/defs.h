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

class Endian {
public:
  Endian() = default;

  Endian(const Endian&) = default;
  Endian& operator=(const Endian&) = default;

  static Endian Little() { return Endian('l'); }
  static Endian Big() { return Endian('b'); }

  bool little() const { return value_ == 'l'; }
  bool big() const { return value_ == 'b'; }

  const char* name() const {
    return value_ == 'l' ? "little" : "big";
  }

private:
  Endian(char value) : value_(value) {}

private:
  char value_ = 'l';  // 'l' or 'b'.
};

inline bool operator==(Endian lhs, Endian rhs) {
  return lhs.little() == rhs.little();
}

inline bool operator!=(Endian lhs, Endian rhs) {
  return !(lhs == rhs);
}

// -----------------------------------------------------------------------------

// Value Representation.
// See: PS 3.5 Section 6.2 - Value Representation (VR).
enum VR {

#define MAKE_VR_ENUM(c0, c1) ((((int)c0) << 8) | (int)c1)

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

}  // namespace dcm

#endif  // DCM_DEFS_H_
