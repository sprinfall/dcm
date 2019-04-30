#include "gtest/gtest.h"

#include "dcm/data_set.h"
#include "dcm/dicom_reader.h"
#include "dcm/read_handler.h"

extern std::string g_data_dir;

TEST(DataSetTest, SetString) {
  boost::filesystem::path path(g_data_dir);
  path /= "cs";
  path /= "H2.ceph";

  dcm::DataSet data_set;
  dcm::FullReadHandler read_handler(&data_set);
  dcm::DicomReader reader(&read_handler);

  bool ok = reader.ReadFile(path);
  EXPECT_TRUE(ok);

  const dcm::Tag kCommentsTag(0x00204000);

  {
    std::string comments(10, 'a');

    ok = data_set.SetString(kCommentsTag, comments);
    EXPECT_TRUE(ok);

    EXPECT_EQ(10, data_set.GetLength(kCommentsTag));

    std::string value;
    EXPECT_TRUE(data_set.GetString(kCommentsTag, &value));
    EXPECT_EQ(comments, value);
  }

  {
    std::string comments(11, 'a');

    ok = data_set.SetString(kCommentsTag, comments);
    EXPECT_TRUE(ok);

    EXPECT_EQ(12, data_set.GetLength(kCommentsTag));

    std::string value;
    EXPECT_TRUE(data_set.GetString(kCommentsTag, &value));
    EXPECT_EQ(comments, value);
  }

  {
    ok = data_set.SetString(kCommentsTag, "");
    EXPECT_TRUE(ok);

    EXPECT_EQ(0, data_set.GetLength(kCommentsTag));

    std::string value;
    EXPECT_TRUE(data_set.GetString(kCommentsTag, &value));
    EXPECT_EQ("", value);
  }
}
