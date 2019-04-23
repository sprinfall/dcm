#include "gtest/gtest.h"

#include "dcmlite/dcmlite.h"

extern std::string g_data_dir;

TEST(DicomReaderTest, ReadFile) {

  boost::filesystem::path path(g_data_dir);
  path /= "cs";
  path /= "H2.ceph";

  dcmlite::DataSet data_set;
  dcmlite::FullReadHandler read_handler(&data_set);
  dcmlite::DicomReader reader(&read_handler);

  bool ok = reader.ReadFile(path);

  EXPECT_TRUE(ok);
}
