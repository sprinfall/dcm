#include "gtest/gtest.h"

#include "dcm/tag.h"

TEST(TagTest, Constructor1) {
  dcm::Tag tag(0x0008, 0x0016);

  EXPECT_EQ(0x0008, tag.group());
  EXPECT_EQ(0x0016, tag.element());
}

TEST(TagTest, Constructor2) {
  dcm::Tag tag(0x00080016);

  EXPECT_EQ(0x0008, tag.group());
  EXPECT_EQ(0x0016, tag.element());
}

TEST(TagTest, ToUint32) {
  dcm::Tag tag(0x0008, 0x0016);

  EXPECT_EQ(0x00080016, tag.ToUint32());
}

TEST(TagTest, OperatorEqual) {
  EXPECT_TRUE(dcm::Tag(0x0008, 0x0016) == dcm::Tag(0x0008, 0x0016));
  EXPECT_FALSE(dcm::Tag(0x0008, 0x0010) == dcm::Tag(0x0008, 0x0016));
}

TEST(TagTest, OperatorNotEqual) {
  EXPECT_TRUE(dcm::Tag(0x0008, 0x0010) != dcm::Tag(0x0008, 0x0016));
  EXPECT_FALSE(dcm::Tag(0x0008, 0x0016) != dcm::Tag(0x0008, 0x0016));
}
