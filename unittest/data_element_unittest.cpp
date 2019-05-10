#include "gtest/gtest.h"

#include "dcm/data_element.h"

const dcm::Tag kSamplesPerPixelTag = 0x00280002;
const dcm::Tag kRequestingAETag = 0x00741236;

TEST(DataElementTest, Constructor) {
  dcm::DataElement element(kSamplesPerPixelTag, dcm::ByteOrder::LE);

  EXPECT_EQ(dcm::VR::US, element.vr());
}

TEST(DataElementTest, SetString_AE) {
  dcm::DataElement element(kRequestingAETag, dcm::ByteOrder::LE);

  bool ok = element.SetString("MyStoreScuPC01");
  EXPECT_TRUE(ok);
}

TEST(DataElementTest, SetUint16) {
  dcm::DataElement element(kSamplesPerPixelTag, dcm::ByteOrder::BE);

  EXPECT_TRUE(element.SetUint16(1));

  std::uint16_t value = 0;
  EXPECT_TRUE(element.GetUint16(&value));
  EXPECT_EQ(1, value);
}
