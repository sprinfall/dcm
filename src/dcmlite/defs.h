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

// Return the endian type of the current platform.
Endian PlatformEndian();

const std::uint32_t kUndefinedLength = 0xFFFFFFFF;

// Reverse the byte order of a 16-bit unsigned int.
inline std::uint16_t ReverseBytesUint16(std::uint16_t value) {
  return (((value & 0xff00) >> 8) | ((value & 0xff) << 8));
}

// Reverse the byte order of a 32-bit unsigned int.
inline std::uint32_t ReverseBytesUint32(std::uint32_t value) {
  return (((value & 0xff) << 24) |
          ((value & 0xff00) << 8) |
          ((value & 0xff0000) >> 8) |
          ((value & 0xff000000) >> 24));
}

}  // namespace dcmlite

#endif  // DEFS_H_
