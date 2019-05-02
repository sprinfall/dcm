#include "gtest/gtest.h"

#include "dcm/charset.h"

// This file must be saved with encoding Unicode LE (Code page 1200).

// The UTF-8 byte streams are generated using an online converter:
//   https://www.branah.com/unicode-converter

TEST(CharsetTest, Utf16ToBytes_Of_Utf8) {
  {
    // Convert UTF-16 data to UTF-8 (ISO_IR_192) bytes stream.

    std::string bytes = dcm::Utf16ToBytes(L"abc123", dcm::Charset::ISO_IR_192);

    EXPECT_EQ("abc123", bytes);
  }

  {
    std::string bytes = dcm::Utf16ToBytes(L"中文", dcm::Charset::ISO_IR_192);

    EXPECT_EQ("\xe4\xb8\xad\xe6\x96\x87", bytes);
  }
}

TEST(CharsetTest, Utf16ToBytes_Of_Ascii) {
  {
    // Convert UTF-16 data to UTF-8 (ISO_IR_192) bytes stream.

    std::string bytes = dcm::Utf16ToBytes(L"abc123", dcm::Charset::ISO_IR_6);

    EXPECT_EQ("abc123", bytes);
  }

#if 0
  {
    // TODO: This is Windows only and really depends on system locale.

    std::string bytes = dcm::Utf16ToBytes(L"中文", dcm::Charset::ISO_IR_6);

    EXPECT_EQ("", bytes);
  }
#endif  // 0
}

TEST(CharsetTest, Utf8ToBytes_Of_Utf8) {
  {
    std::string output = dcm::Utf8ToBytes("abc123", dcm::Charset::ISO_IR_192);

    EXPECT_EQ("abc123", output);
  }
}
