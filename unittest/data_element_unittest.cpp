#include "gtest/gtest.h"

#include "dcm/data_element.h"

TEST(CheckStringValueTest, AE) {
  EXPECT_TRUE(dcm::CheckStringValue(dcm::VR::AE, "MyStoreScuPC01"));

  EXPECT_FALSE(dcm::CheckStringValue(dcm::VR::AE, ""));
}
