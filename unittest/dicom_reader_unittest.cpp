#include "gtest/gtest.h"

#include "dcm/data_set.h"
#include "dcm/dicom_reader.h"
#include "dcm/read_handler.h"

extern std::string g_data_dir;

TEST(DicomReaderTest, ReadFile) {
  boost::filesystem::path path(g_data_dir);
  path /= "cs";
  path /= "H2.ceph";

  dcm::DataSet data_set;
  dcm::FullReadHandler read_handler(&data_set);
  dcm::DicomReader reader(&read_handler);

  bool ok = reader.ReadFile(path);

  EXPECT_TRUE(ok);
}
