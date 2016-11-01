#ifndef DEFS_H_
#define DEFS_H_
#pragma once

#include <cstdint>
#include <string>

namespace dcmlite {

enum Endian {
  kLittleEndian,
  kBigEndian,
};

// Determine the endian type of the current platform.
Endian GetPlatformEndian();

// Reverse the byte order of uint16.
inline std::uint16_t ReverseBytesUint16(std::uint16_t value) {
  return (((value & 0xff00) >> 8) | ((value & 0xff) << 8));
}

}  // namespace dcmlite

#endif  // DEFS_H_
