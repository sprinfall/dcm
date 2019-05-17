#include "gtest/gtest.h"

#include "dcm/dicom_file.h"

extern std::string g_data_dir;

TEST(DicomFileTest, ImplicitLittleNoMeta) {
  dcm::Path path(g_data_dir);
  path /= "Implicit Little NoMeta (CR-MONO1-10-chest).dcm";

  dcm::DicomFile dicom_file(path);

  bool ok = dicom_file.Load();
  EXPECT_TRUE(ok);

  EXPECT_EQ(dcm::ByteOrder::LE, dicom_file.byte_order());
  EXPECT_EQ(dcm::VR::IMPLICIT, dicom_file.vr_type());

  std::uint32_t generic_group_length;
  dicom_file.GetUint32(0x00080000, &generic_group_length);
  EXPECT_EQ(286, generic_group_length);

  std::string sop_instance_uid;
  dicom_file.GetString(dcm::tags::kSOPInstanceUID, &sop_instance_uid);
  EXPECT_EQ("1.2.392.200036.9125.0.19950720112207", sop_instance_uid);

  EXPECT_EQ(387200, dicom_file.GetVL(dcm::tags::kPixelData));
}

TEST(DicomFileTest, ExplicitBig) {
  dcm::Path path(g_data_dir);
  path /= "Explicit Big (US-RGB-8-epicard).dcm";

  dcm::DicomFile dicom_file(path);

  bool ok = dicom_file.Load();
  EXPECT_TRUE(ok);

  EXPECT_EQ(dcm::ByteOrder::BE, dicom_file.byte_order());
  EXPECT_EQ(dcm::VR::EXPLICIT, dicom_file.vr_type());

  std::uint32_t generic_group_length;
  dicom_file.GetUint32(0x00080000, &generic_group_length);
  EXPECT_EQ(308, generic_group_length);

  std::string sop_instance_uid;
  dicom_file.GetString(dcm::tags::kSOPInstanceUID, &sop_instance_uid);
  EXPECT_EQ("1.2.840.1136190195280574824680000700.3.0.1.19970424140438",
            sop_instance_uid);

  EXPECT_EQ(921600, dicom_file.GetVL(dcm::tags::kPixelData));
}

TEST(DicomFileTest, ExplicitLittle_CS_Ceph) {
  dcm::Path path(g_data_dir);
  path /= "cs";
  path /= "Explicit Little (Ceph).dcm";

  dcm::DicomFile dicom_file(path);

  bool ok = dicom_file.Load();
  EXPECT_TRUE(ok);

  std::string sop_instance_uid;
  dicom_file.GetString(dcm::tags::kSOPInstanceUID, &sop_instance_uid);
  EXPECT_EQ("1.2.250.1.90.3.348752952.1313490280.15", sop_instance_uid);

  EXPECT_EQ(6981640, dicom_file.GetVL(dcm::tags::kPixelData));

  // Private tags
  {
    std::string value;
    ok = dicom_file.GetString(0x00090011, &value);
    EXPECT_TRUE(ok);
    EXPECT_EQ("TROPHY", value);
  }
  {
    std::uint32_t value;
    ok = dicom_file.GetUint32(0x00091109, &value);
    EXPECT_TRUE(ok);
    EXPECT_EQ(32837, value);
  }

  {
    // Multi-values (VR: US; VM: 2)

    const dcm::Tag kHeadClampRingPosition = 0x00091159;

    std::vector<std::uint16_t> values;
    ok = dicom_file.GetUint16Array(kHeadClampRingPosition, &values);
    EXPECT_TRUE(ok);
    EXPECT_EQ(2, values.size());
    EXPECT_EQ(567, values[0]);
    EXPECT_EQ(1177, values[1]);
  }

  {
    // Multi-values (VR: CS; VM: ?)

    std::string value;
    ok = dicom_file.GetString(dcm::tags::kImageType, &value);
    EXPECT_TRUE(ok);
    EXPECT_EQ("ORIGINAL\\PRIMARY\\", value);

    std::vector<std::string> values;
    ok = dicom_file.GetStringArray(dcm::tags::kImageType, &values);
    EXPECT_TRUE(ok);
    EXPECT_EQ(3, values.size());
    EXPECT_EQ("ORIGINAL", values[0]);
    EXPECT_EQ("PRIMARY", values[1]);
    EXPECT_EQ("", values[2]);
  }

  {
    // Multi-values (VR: DS; VM: 2)

    std::string value;
    ok = dicom_file.GetString(dcm::tags::kPixelSpacing, &value);
    EXPECT_TRUE(ok);
    EXPECT_EQ("0.127000\\0.127000", value);

    std::vector<std::string> values;
    ok = dicom_file.GetStringArray(dcm::tags::kPixelSpacing, &values);
    EXPECT_TRUE(ok);
    EXPECT_EQ(2, values.size());
    EXPECT_EQ("0.127000", values[0]);
    EXPECT_EQ("0.127000", values[1]);
  }
}

TEST(DicomFileTest, ExplicitLittle_SeqEnd_CS_Ceph) {
  dcm::Path path(g_data_dir);
  path /= "cs";
  path /= "Explicit Little SeqEnd (Ceph).dcm";

  dcm::DicomFile dicom_file(path);

  bool ok = dicom_file.Load();
  EXPECT_TRUE(ok);

  std::string sop_instance_uid;
  ok = dicom_file.GetString(dcm::tags::kSOPInstanceUID, &sop_instance_uid);
  EXPECT_TRUE(ok);
  EXPECT_EQ("1.2.250.1.90.3.348752952.1313490280.15", sop_instance_uid);

  EXPECT_EQ(6981640, dicom_file.GetVL(dcm::tags::kPixelData));

  // Private tags
  {
    std::string value;
    EXPECT_TRUE(dicom_file.GetString(0x00090011, &value));
    EXPECT_EQ("TROPHY", value);
  }
  {
    std::uint32_t value;
    EXPECT_TRUE(dicom_file.GetUint32(0x00091109, &value));
    EXPECT_EQ(32837, value);
  }
}

TEST(DicomFileTest, ImplicitLittle_CS_CR7600) {
  dcm::Path path(g_data_dir);
  path /= "cs";
  path /= "Implicit Little (CR 7600).dcm";

  dcm::DicomFile dicom_file(path);

  bool ok = dicom_file.Load();
  EXPECT_TRUE(ok);

  {
    // Multi-values (VR: CS; VM: ?)

    std::string value;
    ok = dicom_file.GetString(dcm::tags::kImageType, &value);
    EXPECT_TRUE(ok);
    EXPECT_EQ("ORIGINAL\\PRIMARY\\\\FOR PROCESSING", value);

    std::vector<std::string> values;
    ok = dicom_file.GetStringArray(dcm::tags::kImageType, &values);
    EXPECT_TRUE(ok);
    EXPECT_EQ(4, values.size());
    EXPECT_EQ("ORIGINAL", values[0]);
    EXPECT_EQ("PRIMARY", values[1]);
    EXPECT_EQ("", values[2]);
    EXPECT_EQ("FOR PROCESSING", values[3]);
  }

  {
    // Multi-values (VR: DS; VM: 2)

    std::string value;
    ok = dicom_file.GetString(dcm::tags::kPixelSpacing, &value);
    EXPECT_TRUE(ok);
    EXPECT_EQ("0.019973\\0.019973", value);

    std::vector<std::string> values;
    ok = dicom_file.GetStringArray(dcm::tags::kPixelSpacing, &values);
    EXPECT_TRUE(ok);
    EXPECT_EQ(2, values.size());
    EXPECT_EQ("0.019973", values[0]);
    EXPECT_EQ("0.019973", values[1]);
  }
}
