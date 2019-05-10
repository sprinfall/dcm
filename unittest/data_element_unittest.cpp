#include "gtest/gtest.h"

#include "dcm/data_element.h"

TEST(DataElementTest, Constructor) {
  dcm::DataElement element(dcm::tags::kSamplesPerPixel, dcm::ByteOrder::LE);

  EXPECT_EQ(dcm::VR::US, element.vr());
}

TEST(DataElementTest, SetString_AE) {
  dcm::DataElement element(dcm::tags::kRequestingAE, dcm::ByteOrder::LE);

  bool ok = element.SetString("MyStoreScuPC01");
  EXPECT_TRUE(ok);
}

TEST(DataElementTest, SetUint16_LE) {
  dcm::DataElement element(dcm::tags::kSamplesPerPixel, dcm::ByteOrder::LE);

  EXPECT_TRUE(element.SetUint16(1));

  std::uint16_t value = 0;
  EXPECT_TRUE(element.GetUint16(&value));
  EXPECT_EQ(1, value);
}

TEST(DataElementTest, SetUint16_BE) {
  dcm::DataElement element(dcm::tags::kSamplesPerPixel, dcm::ByteOrder::BE);

  EXPECT_TRUE(element.SetUint16(1));

  std::uint16_t value = 0;
  EXPECT_TRUE(element.GetUint16(&value));
  EXPECT_EQ(1, value);
}
