#include "gtest/gtest.h"

#include "dcm/data_set.h"
#include "dcm/dicom_reader.h"
#include "dcm/file_writer.h"
#include "dcm/read_handler.h"
#include "dcm/visitor.h"

extern std::string g_data_dir;

TEST(WriterVisitorTest, WriteToFile) {
  boost::filesystem::path path(g_data_dir);
  path /= "cs";
  path /= "H2.ceph";

  dcm::DataSet data_set;
  dcm::FullReadHandler read_handler(&data_set);
  dcm::DicomReader reader(&read_handler);

  bool ok = reader.ReadFile(path);
  EXPECT_TRUE(ok);

  const dcm::Tag kCommentsTag(0x00204000);
  std::string comments(11, 'a');

  data_set.SetString(kCommentsTag, comments);

  dcm::FileWriter writer("D:/test.dcm");
  dcm::WriteVisitor visitor(&writer);
  data_set.Accept(visitor);
}