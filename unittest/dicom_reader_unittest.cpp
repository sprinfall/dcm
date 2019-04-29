#include <iostream>
#include "gtest/gtest.h"

#include "dcm/data_set.h"
#include "dcm/dicom_reader.h"
#include "dcm/read_handler.h"

#include "dcm/charset.h"  // TODO

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

// TODO
TEST(DicomReaderTest, ReadFile_UTF8) {
  boost::filesystem::path path("D:\\test.dcm");

  dcm::DataSet data_set;
  dcm::FullReadHandler read_handler(&data_set);
  dcm::DicomReader reader(&read_handler);

  bool ok = reader.ReadFile(path);

  EXPECT_TRUE(ok);

  std::string value;
  data_set.GetString(dcm::Tag(0x0020, 0x4000), &value);

  //std::cout << dcm::Utf8ToAnsi(value) << std::endl;

  EXPECT_EQ("\xe6\xb3\xa8\xe9\x87\x8a", value);
}
