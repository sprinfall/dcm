#include "gtest/gtest.h"

#include "dcm/data_element.h"

TEST(DataElementTest, Constructor) {
  dcm::DataElement element(dcm::tags::kSamplesPerPixel, dcm::ByteOrder::LE);

  EXPECT_EQ(dcm::VR::US, element.vr());
}

TEST(DataElementTest, SetString_AE) {
  dcm::DataElement element(dcm::tags::kRequestingAE);

  bool ok = element.SetString("MyStoreScuPC01");
  EXPECT_TRUE(ok);
}

TEST(DataElementTest, GetString_AE) {
  const std::string requesting_ae = "MyStoreScuPC01";

  dcm::DataElement element(dcm::tags::kRequestingAE);

  bool ok = element.SetString(requesting_ae);
  EXPECT_TRUE(ok);

  std::string value;
  ok = element.GetString(&value);
  EXPECT_TRUE(ok);

  EXPECT_EQ(requesting_ae, value);
}

TEST(DataElementTest, GetString_NotAString) {
  dcm::DataElement element(dcm::tags::kSamplesPerPixel, dcm::ByteOrder::LE);
  element.SetUint16(1);

  std::string value;
  bool ok = element.GetString(&value);
  EXPECT_FALSE(ok);
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
