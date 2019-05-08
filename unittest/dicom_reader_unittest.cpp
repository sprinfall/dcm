#include "gtest/gtest.h"

#include "dcm/data_set.h"
#include "dcm/dicom_reader.h"

extern std::string g_data_dir;

TEST(DicomReaderTest, ImplicitLittleNoMeta) {
  dcm::Path path(g_data_dir);
  path /= "Implicit Little NoMeta (CR-MONO1-10-chest).dcm";

  dcm::DataSet data_set;
  dcm::DicomReader reader(&data_set);

  bool ok = reader.ReadFile(path);
  EXPECT_TRUE(ok);

  EXPECT_EQ(dcm::ByteOrder::LE, data_set.byte_order());
  EXPECT_EQ(false, data_set.explicit_vr());

  std::uint32_t generic_group_length;
  data_set.GetUint32(0x00080000, &generic_group_length);
  EXPECT_EQ(286, generic_group_length);

  std::string sop_instance_uid;
  data_set.GetString(0x00080018, &sop_instance_uid);
  EXPECT_EQ("1.2.392.200036.9125.0.19950720112207", sop_instance_uid);

  auto element = data_set.Get(0x7fe00010);
  EXPECT_TRUE(element != nullptr);
  EXPECT_EQ(387200, element->length());
}

TEST(DicomReaderTest, ExplicitBig) {
  dcm::Path path(g_data_dir);
  path /= "Explicit Big (US-RGB-8-epicard).dcm";

  dcm::DataSet data_set;
  dcm::DicomReader reader(&data_set);

  bool ok = reader.ReadFile(path);
  EXPECT_TRUE(ok);

  EXPECT_EQ(dcm::ByteOrder::BE, data_set.byte_order());
  EXPECT_EQ(true, data_set.explicit_vr());

  std::uint32_t generic_group_length;
  data_set.GetUint32(0x00080000, &generic_group_length);
  EXPECT_EQ(308, generic_group_length);

  std::string sop_instance_uid;
  data_set.GetString(0x00080018, &sop_instance_uid);
  EXPECT_EQ("1.2.840.1136190195280574824680000700.3.0.1.19970424140438",
            sop_instance_uid);

  auto element = data_set.Get(0x7fe00010);
  EXPECT_TRUE(element != nullptr);
  EXPECT_EQ(921600, element->length());
}

TEST(DicomReaderTest, ExplicitLittle_CS_Ceph) {
  dcm::Path path(g_data_dir);
  path /= "cs";
  path /= "Explicit Little (Ceph).dcm";

  dcm::DataSet data_set;
  dcm::DicomReader reader(&data_set);

  bool ok = reader.ReadFile(path);
  EXPECT_TRUE(ok);
}
