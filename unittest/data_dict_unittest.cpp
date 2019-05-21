#include "gtest/gtest.h"

#include "dcm/data_dict.h"

TEST(DataDictTest, GetEntry) {
  auto entry = dcm::dict::GetEntry(dcm::tags::kSOPInstanceUID);

  EXPECT_TRUE(entry != nullptr);

  EXPECT_EQ(dcm::VR::UI, entry->vr);
  EXPECT_EQ("SOPInstanceUID", std::string(entry->keyword));
}

TEST(DataDictTest, GetVR) {
  EXPECT_EQ(dcm::VR::CS, dcm::dict::GetVR(dcm::tags::kImageType));
  EXPECT_EQ(dcm::VR::UI, dcm::dict::GetVR(dcm::tags::kSOPInstanceUID));
}

TEST(DataDictTest, GetVM_2) {
  auto vm = dcm::dict::GetVM(dcm::tags::kPixelSpacing);

  EXPECT_TRUE(vm != nullptr);
  EXPECT_FALSE(vm->IsRange());

  EXPECT_EQ(2, vm->min());
  EXPECT_EQ(2, vm->max());
  EXPECT_EQ(1, vm->times());
}

TEST(DataDictTest, GetVM_2_N) {
  auto vm = dcm::dict::GetVM(dcm::tags::kImageType);

  EXPECT_TRUE(vm != nullptr);
  EXPECT_TRUE(vm->IsRange());

  EXPECT_EQ(2, vm->min());
  EXPECT_EQ(dcm::VM::N, vm->max());
  EXPECT_EQ(1, vm->times());
}

TEST(DataDictTest, GetVM_3_3N) {
  auto vm = dcm::dict::GetVM(0x00081162);

  EXPECT_TRUE(vm != nullptr);
  EXPECT_TRUE(vm->IsRange());

  EXPECT_EQ(1, vm->min());
  EXPECT_EQ(dcm::VM::N, vm->max());
  EXPECT_EQ(3, vm->times());
}

TEST(DataDictTest, GetVM_1) {
  auto vm = dcm::dict::GetVM(dcm::tags::kSOPInstanceUID);

  EXPECT_EQ(nullptr, vm);
}
