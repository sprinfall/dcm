#include "dcm/tag.h"

#include <istream>
#include <iomanip>

namespace dcm {

const Tag kSeqEndTag(0xFFFE, 0xE0DD);
const Tag kSeqItemEndTag(0xFFFE, 0xE00D);
const Tag kSeqItemPrefixTag(0xFFFE, 0xE000);

std::ostream& operator<<(std::ostream& os, Tag tag) {
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

}  // namespace dcm
