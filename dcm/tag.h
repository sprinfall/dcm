#ifndef DCM_TAG_H_
#define DCM_TAG_H_

// Data Element Tag:
// A unique identifier for a Data Element composed of an ordered pair of numbers
// (a Group Number followed by an Element Number).
// See DICOM Part 3 / 3.8 or Part 5 / 3.10.

#include <cstdint>
#include <iosfwd>

#include "dcm/defs.h"

namespace dcm {

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

  Tag SwapBytes() const;

  // Convert to a 4-byte unsigned integer.
  // Group in high two bytes, element in low two bytes.
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
  return (lhs.group() == rhs.group() && lhs.element() == rhs.element());
}

inline bool operator!=(Tag lhs, Tag rhs) {
  return !(lhs == rhs);
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

// TODO: Move to another place.
const Tag kTransferSyntaxTag = 0x00020010;
const Tag kSeqEndTag = 0xFFFEE0DD;
const Tag kSeqItemEndTag = 0xFFFEE00D;
const Tag kSeqItemPrefixTag = 0xFFFEE000;

}  // namespace dcm

#endif  // DCM_TAG_H_
