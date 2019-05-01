#include "gtest/gtest.h"

#include "dcm/data_set.h"
#include "dcm/dicom_reader.h"
#include "dcm/read_handler.h"

extern std::string g_data_dir;

TEST(DataSetTest, AppendElement) {
  dcm::DataSet data_set;

  EXPECT_EQ(0, data_set.GetSize());

  data_set.AppendElement(new dcm::DataElement(0x00080005));
  data_set.AppendElement(new dcm::DataElement(0x00080008));
  data_set.AppendElement(new dcm::DataElement(0x00100010));

  EXPECT_EQ(3, data_set.GetSize());

  EXPECT_EQ(0x00080005, data_set[0]->tag());
  EXPECT_EQ(0x00080008, data_set[1]->tag());
  EXPECT_EQ(0x00100010, data_set[2]->tag());

  dcm::DataElement* element = new dcm::DataElement(0x00020001);
  EXPECT_FALSE(data_set.AppendElement(element));
  EXPECT_EQ(3, data_set.GetSize());
}

TEST(DataSetTest, InsertElement) {
  dcm::DataSet data_set;

  data_set.InsertElement(new dcm::DataElement(0x00080005));
  data_set.InsertElement(new dcm::DataElement(0x00080008));
  data_set.InsertElement(new dcm::DataElement(0x00100010));

  EXPECT_EQ(3, data_set.GetSize());

  EXPECT_EQ(0x00080005, data_set[0]->tag());
  EXPECT_EQ(0x00080008, data_set[1]->tag());
  EXPECT_EQ(0x00100010, data_set[2]->tag());
}

TEST(DataSetTest, InsertElement2) {
  dcm::DataSet data_set;

  data_set.InsertElement(new dcm::DataElement(0x00080008));
  data_set.InsertElement(new dcm::DataElement(0x00100010));
  data_set.InsertElement(new dcm::DataElement(0x00080005));

  EXPECT_EQ(3, data_set.GetSize());

  EXPECT_EQ(0x00080005, data_set[0]->tag());
  EXPECT_EQ(0x00080008, data_set[1]->tag());
  EXPECT_EQ(0x00100010, data_set[2]->tag());
}

TEST(DataSetTest, InsertElement3) {
  dcm::DataSet data_set;
  bool ok = false;

  ok = data_set.InsertElement(new dcm::DataElement(0x00080008));
  EXPECT_TRUE(ok);

  ok = data_set.InsertElement(new dcm::DataElement(0x00080008));
  EXPECT_FALSE(ok);

  ok = data_set.InsertElement(new dcm::DataElement(0x00080008));
  EXPECT_FALSE(ok);

  EXPECT_EQ(1, data_set.GetSize());

  EXPECT_EQ(0x00080008, data_set[0]->tag());
}

TEST(DataSetTest, GetElement) {
  dcm::DataSet data_set;

  EXPECT_TRUE(data_set.GetElement(0x00080008) == nullptr);

  data_set.AppendElement(new dcm::DataElement(0x00080005));
  data_set.AppendElement(new dcm::DataElement(0x00080008));
  data_set.AppendElement(new dcm::DataElement(0x00100010));

  EXPECT_TRUE(data_set.GetElement(0x00080008) != nullptr);
  EXPECT_TRUE(data_set.GetElement(0x00080005) != nullptr);
  EXPECT_TRUE(data_set.GetElement(0x00100010) != nullptr);
  EXPECT_TRUE(data_set.GetElement(0x00020001) == nullptr);
}

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

    EXPECT_EQ(10, data_set.GetValueLength(kCommentsTag));

    std::string value;
    EXPECT_TRUE(data_set.GetString(kCommentsTag, &value));
    EXPECT_EQ(comments, value);
  }

  {
    std::string comments(11, 'a');

    ok = data_set.SetString(kCommentsTag, comments);
    EXPECT_TRUE(ok);

    EXPECT_EQ(12, data_set.GetValueLength(kCommentsTag));

    std::string value;
    EXPECT_TRUE(data_set.GetString(kCommentsTag, &value));
    EXPECT_EQ(comments, value);
  }

  {
    ok = data_set.SetString(kCommentsTag, "");
    EXPECT_TRUE(ok);

    EXPECT_EQ(0, data_set.GetValueLength(kCommentsTag));

    std::string value;
    EXPECT_TRUE(data_set.GetString(kCommentsTag, &value));
    EXPECT_EQ("", value);
  }
}

TEST(DataSetTest, SetString_NoExist) {
  const dcm::Tag kSourceAETitleTag(0x00020016);
  const std::string kAETitle("MYSTORESCU");

  dcm::DataSet data_set;

  bool ok = data_set.SetString(kSourceAETitleTag, kAETitle);
  EXPECT_TRUE(ok);

  EXPECT_EQ(1, data_set.GetSize());

  std::string ae_title;
  data_set.GetString(kSourceAETitleTag, &ae_title);
  EXPECT_EQ(kAETitle, ae_title);
}

TEST(DataSetTest, SetString_InvalidValue) {
  const dcm::Tag kSourceAETitleTag(0x00020016);
  const std::string kAETitle("MYSTORESCU_MORETHAN_MAX_LENGTH_16");

  dcm::DataSet data_set;

  bool ok = data_set.SetString(kSourceAETitleTag, kAETitle);
  EXPECT_FALSE(ok);

  EXPECT_EQ(0, data_set.GetSize());
}
