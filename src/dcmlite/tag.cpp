#include "tag.h"

#include <istream>
#include <iomanip>

#include "dcmlite/defs.h"

namespace dcmlite {

Tag::Tag()
    : group_(0), element_(0) {
}

Tag::Tag(std::uint16_t group, std::uint16_t element)
    : group_(group), element_(element) {
}

Tag::Tag(std::uint32_t tag_key)
    : group_((tag_key >> 16) & 0xFFFF), element_(tag_key & 0xFFFF) {
}

Tag::Tag(const Tag& rhs)
    : group_(rhs.group_), element_(rhs.element_) {
}

Tag::~Tag() {
}

Tag& Tag::operator=(const Tag& rhs) {
  if (&rhs != this) {
    group_ = rhs.group_;
    element_ = rhs.element_;
  }
  return *this;
}

Tag Tag::ReverseBytes() const {
  return Tag(ReverseBytesUint16(group_), ReverseBytesUint16(element_));
}

bool operator<(const Tag& lhs, const Tag& rhs) {
  if (lhs.group() < rhs.group()) {
    return true;
  } else if (lhs.group() == rhs.group()) {
    return lhs.element() < rhs.element();
  } else {
    return false;
  }
}

bool operator>(const Tag& lhs, const Tag& rhs) {
  if (lhs.group() > rhs.group()) {
    return true;
  } else if (lhs.group() == rhs.group()) {
    return lhs.element() > rhs.element();
  } else {
    return false;
  }
}

bool operator<=(const Tag& lhs, const Tag& rhs) {
  return (lhs < rhs || lhs == rhs);
}

bool operator>=(const Tag& lhs, const Tag& rhs) {
  return (lhs > rhs || lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const Tag& tag) {
  using namespace std;

  ios::fmtflags old_flags = os.flags(ios::right | ios::hex | ios::uppercase);

  os << "(";
  os << setfill('0') << setw(4) << tag.group();
  os << ", ";
  os << setfill('0') << setw(4) << tag.element();
  os << ")";

  os.flags(old_flags);

  return os;
}

}  // namespace dcmlite
