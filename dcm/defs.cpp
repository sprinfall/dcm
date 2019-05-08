#include "dcm/defs.h"

namespace dcm {

static union {
  char c[4];
  unsigned long mylong;
} byte_order_test = { { 'l', '?', '?', 'b' } };

const ByteOrder kByteOrderOS =
    ((char)byte_order_test.mylong == 'l') ? ByteOrder::LE : ByteOrder::BE;

}  // namespace dcm
