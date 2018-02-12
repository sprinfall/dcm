#ifndef DCMLITE_TAG_H_
#define DCMLITE_TAG_H_
#pragma once

#include <cstdint>
#include <iosfwd>

#include "dcmlite/defs.h"

namespace dcmlite {

// Data Element Tag:
// A unique identifier for a Data Element composed of an ordered pair of numbers
// (a Group Number followed by an Element Number).
// See DICOM Part 3 / 3.8 or Part 5 / 3.10.

class Tag;

extern const Tag kSeqEndTag;          // (0xFFFE, 0xE0DD)
extern const Tag kSeqItemEndTag;      // (0xFFFE, 0xE00D)
extern const Tag kSeqItemPrefixTag;   // (0xFFFE, 0xE000)

class Tag {
public:
  Tag() = default;
  Tag(const Tag& rhs) = default;
  Tag& operator=(const Tag& rhs) = default;

  Tag(std::uint16_t group, std::uint16_t element)
      : group_(group), element_(element) {
  }

  Tag(std::uint32_t tag_key)
      : group_((tag_key >> 16) & 0xFFFF), element_(tag_key & 0xFFFF) {
  }

  std::uint16_t group() const {
    return group_;
  }
  void set_group(std::uint16_t group) {
    group_ = group;
  }

  std::uint16_t element() const {
    return element_;
  }
  void set_element(std::uint16_t element) {
    element_ = element;
  }

  bool IsEmpty() const {
    return group_ == 0 && element_ == 0;
  }

  Tag SwapBytes() const {
    return Tag(SwapUint16(group_), SwapUint16(element_));
  }

  // Convert to a 4-byte unsigned integer.
  // Group in high two bytes, element in low two bytes.
  std::uint32_t ToUint32() const {
    return (group_ << 16) + element_;
  }

private:
  std::uint16_t group_;
  std::uint16_t element_;
};

inline bool operator==(const Tag& lhs, const Tag& rhs) {
  return (lhs.group() == rhs.group() && lhs.element() == rhs.element());
}

inline bool operator!=(const Tag& lhs, const Tag& rhs) {
  return !(lhs == rhs);
}

bool operator<(const Tag& lhs, const Tag& rhs);
bool operator>(const Tag& lhs, const Tag& rhs);

bool operator<=(const Tag& lhs, const Tag& rhs);
bool operator>=(const Tag& lhs, const Tag& rhs);

std::ostream& operator<<(std::ostream& os, const Tag& tag);

}  // namespace dcmlite

#endif  // DCMLITE_TAG_H_
