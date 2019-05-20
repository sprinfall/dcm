#include "gtest/gtest.h"

#include <string>

#include "dcm/defs.h"

std::string g_data_dir;

TEST(DefinitionsTest, TypeSize) {
  EXPECT_EQ(4, sizeof(dcm::float32_t));
  EXPECT_EQ(8, sizeof(dcm::float64_t));
}

TEST(VMTest, Constructor) {
  {
    dcm::VM vm("1");

    EXPECT_EQ(1, vm.min());
    EXPECT_EQ(1, vm.max());
    EXPECT_EQ(1, vm.times());
    EXPECT_EQ(false, vm.IsRange());
  }
  {
    dcm::VM vm("2");

    EXPECT_EQ(2, vm.min());
    EXPECT_EQ(2, vm.max());
    EXPECT_EQ(1, vm.times());
    EXPECT_EQ(false, vm.IsRange());
  }
  {
    dcm::VM vm("10");

    EXPECT_EQ(10, vm.min());
    EXPECT_EQ(10, vm.max());
    EXPECT_EQ(1, vm.times());
    EXPECT_EQ(false, vm.IsRange());
  }
  {
    dcm::VM vm("1-9");

    EXPECT_EQ(1, vm.min());
    EXPECT_EQ(9, vm.max());
    EXPECT_EQ(1, vm.times());
    EXPECT_EQ(true, vm.IsRange());
  }
  {
    dcm::VM vm("1-n");

    EXPECT_EQ(1, vm.min());
    EXPECT_EQ(-1, vm.max());
    EXPECT_EQ(1, vm.times());
    EXPECT_EQ(true, vm.IsRange());
  }
  {
    dcm::VM vm("2-2n");

    EXPECT_EQ(1, vm.min());
    EXPECT_EQ(-1, vm.max());
    EXPECT_EQ(2, vm.times());
    EXPECT_EQ(true, vm.IsRange());
  }
  {
    dcm::VM vm("3-3n");

    EXPECT_EQ(1, vm.min());
    EXPECT_EQ(-1, vm.max());
    EXPECT_EQ(3, vm.times());
    EXPECT_EQ(true, vm.IsRange());
  }

  {
    dcm::VM vm("xxx");

    EXPECT_EQ(0, vm.min());
    EXPECT_EQ(0, vm.max());
    EXPECT_EQ(1, vm.times());
    EXPECT_EQ(false, vm.IsRange());
  }
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <data dir>" << std::endl;
    return 1;
  }

  g_data_dir = argv[1];
  std::cout << "Data dir: " << g_data_dir << std::endl;

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
