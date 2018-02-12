#include "dcmlite/tag.h"

#include <istream>
#include <iomanip>

namespace dcmlite {

const Tag kSeqEndTag(0xFFFE, 0xE0DD);
const Tag kSeqItemEndTag(0xFFFE, 0xE00D);
const Tag kSeqItemPrefixTag(0xFFFE, 0xE000);

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
