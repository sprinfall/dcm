#include "gtest/gtest.h"

#include <string>

#include "dcm/defs.h"

std::string g_data_dir;

TEST(DefinitionsTest, TypeSize) {
  EXPECT_EQ(4, sizeof(dcm::float32_t));
  EXPECT_EQ(8, sizeof(dcm::float64_t));
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
