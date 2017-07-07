#include <iostream>
#include <string>

#include "dcmlite/dcmlite.h"

// Test tags.
static const dcmlite::Tag kTransferSyntaxUidTag(0x0002, 0x0010);
static const dcmlite::Tag kPatientNameTag(0x0010, 0x0010);
static const dcmlite::Tag kSamplesPerPixelTag(0x0028, 0x0002);


static void PrintEndian(const char* title, dcmlite::Endian endian) {
  std::cout << title;

  if (endian == dcmlite::kLittleEndian) {
    std::cout << ": Little Endian" << std::endl;
  } else {
    std::cout << ": Big Endian" << std::endl;
  }
}

static void GetTagsFromDataSet(const dcmlite::DataSet& data_set) {
  std::string transfer_syntax_uid;
  if (data_set.GetString(kTransferSyntaxUidTag, &transfer_syntax_uid)) {
    std::cout << "Transfer Syntax UID: " << transfer_syntax_uid << std::endl;
  }

  std::string patient_name;
  if (data_set.GetString(kPatientNameTag, &patient_name)) {
    std::cout << "Patient Name: " << patient_name << std::endl;
  }

  std::uint16_t samples_per_pixel;
  if (data_set.GetUint16(kSamplesPerPixelTag, &samples_per_pixel)) {
    std::cout << "Samples Per Pixel: " << samples_per_pixel << std::endl;
  }
}

static void DumpDicomFile(const std::string& file_path) {
  dcmlite::DumpReadHandler read_handler;
  dcmlite::DicomReader reader(&read_handler);
  reader.ReadFile(file_path);

  std::cout << std::endl;
}

static void LoadFullDataSet(const std::string& file_path) {
  std::cout << "Load the full data set." << std::endl;

  dcmlite::DataSet data_set;
  dcmlite::FullReadHandler read_handler(&data_set);
  dcmlite::DicomReader reader(&read_handler);
  {
    dcmlite::TimeIt ti;  // Log the time.
    reader.ReadFile(file_path);
  }

  PrintEndian("Data set endian", data_set.endian());

  GetTagsFromDataSet(data_set);

  std::cout << std::endl;
}

static void ReadSpecificTags(const std::string& file_path) {
  std::cout << "Read the specific tags." << std::endl;

  dcmlite::DataSet data_set;
  dcmlite::TagsReadHandler read_handler(&data_set);

  // NOTE: Add order doesn't matter.
  read_handler.AddTag(kPatientNameTag).
               AddTag(kTransferSyntaxUidTag).
               AddTag(kSamplesPerPixelTag);

  {
    dcmlite::TimeIt ti;  // Log the time.
    dcmlite::DicomReader reader(&read_handler);
    reader.ReadFile(file_path);
  }

  GetTagsFromDataSet(data_set);

  std::cout << std::endl;
}


// TODO: Add sub-command.
int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << argv[0] << " <file path>" << std::endl;
    return 1;
  }

  PrintEndian("Platform endian", dcmlite::PlatformEndian());

  std::cout << std::endl;

  std::string file_path = argv[1];
  std::cout << "File path: " << file_path << std::endl << std::endl;

  LoadFullDataSet(file_path);

  ReadSpecificTags(file_path);

  return 0;
}
