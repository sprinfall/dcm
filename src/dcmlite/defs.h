#ifndef DCMLITE_DEFS_H_
#define DCMLITE_DEFS_H_
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

typedef float   float32_t;
typedef double  float64_t;

std::uint16_t SwapUint16(std::uint16_t value);
std::uint32_t SwapUint32(std::uint32_t value);

void Swap16(void* src);
void Swap32(void* src);
void Swap64(void* src);

}  // namespace dcmlite

#endif  // DCMLITE_DEFS_H_
