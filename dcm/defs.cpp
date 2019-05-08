#include "dcm/defs.h"

#include <iomanip>

#include "dcm/util.h"

namespace dcm {

// -----------------------------------------------------------------------------

static union {
  char c[4];
  unsigned long mylong;
} byte_order_test = { { 'l', '?', '?', 'b' } };

const ByteOrder kByteOrderOS =
    ((char)byte_order_test.mylong == 'l') ? ByteOrder::LE : ByteOrder::BE;

// -----------------------------------------------------------------------------

Tag Tag::SwapBytes() const {
  return Tag(SwapUint16(group_), SwapUint16(element_));
}

void Tag::Print(std::ostream& os, bool uppercase, bool as_uint32,
                const char* separator) const {
  std::ios::fmtflags old_flags = os.flags(std::ios::right | std::ios::hex);

  if (uppercase) {
    os.setf(std::ios::uppercase);
  }

  os << std::setfill('0');

  if (as_uint32) {
    os << std::setw(8) << ToUint32();
  } else {
    os << std::setw(4) << group_;
    os << separator;
    os << std::setw(4) << element_;
  }

  os.flags(old_flags);
}

}  // namespace dcm
