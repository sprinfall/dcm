#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>

#include "dcmlite/dcmlite.h"

const dcmlite::Tag kTagTransferSyntaxUID(0x0002, 0x0010);
const dcmlite::Tag kTagPatientName(0x0010, 0x0010);
const dcmlite::Tag kTagSamplesPerPixel(0x0028, 0x0002);

std::string EndianToString(dcmlite::Endian endian) {
  if (endian == dcmlite::kLittleEndian) {
    return "Little Endian";
  } else {
    return "Big Endian";
  }
}

void CoutKeyValue(const std::string& key, const std::string& value) {
  std::cout << std::left << std::setfill(' ') << std::setw(24)
    << key << ": " << value << std::endl;
}

// Print basic info (Endian type, explicit VR or not, etc.).
void InfoDicomFile(const std::string& file_path) {
  dcmlite::DataSet data_set;
  dcmlite::FullReadHandler read_handler(&data_set);
  dcmlite::DicomReader reader(&read_handler);

  if (!reader.ReadFile(file_path)) {
    std::cerr << "Failed to read file." << std::endl;
    return;
  }

  CoutKeyValue("Endian", EndianToString(data_set.endian()));
  CoutKeyValue("Explicit VR", data_set.explicit_vr() ? "true" : "false");
}

void DumpDicomFile(const std::string& file_path) {
  dcmlite::DumpReadHandler read_handler;
  dcmlite::DicomReader reader(&read_handler);
  reader.ReadFile(file_path);

  std::cout << std::endl;
}

void CopyDicomFile(const std::string& file_path) {
  // Load data set.
  dcmlite::DataSet data_set;
  dcmlite::FullReadHandler read_handler(&data_set);
  dcmlite::DicomReader reader(&read_handler);

  if (!reader.ReadFile(file_path)) {
    std::cerr << "Failed to read file.\n";
    return;
  }

  dcmlite::BinaryFile file;
  if (!file.Open("output.dcm", dcmlite::BinaryFile::Mode::WRITE)) {
    std::cerr << "Failed to open output file.\n";
    return;
  }

  dcmlite::WriteVisitor visitor(&file);
  data_set.Accept(visitor);
}

#if 0
void GetTagsFromDataSet(const dcmlite::DataSet& data_set) {
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

void LoadFullDataSet(const std::string& file_path) {
  std::cout << "Load the full data set." << std::endl;

  dcmlite::DataSet data_set;
  dcmlite::FullReadHandler read_handler(&data_set);
  dcmlite::DicomReader reader(&read_handler);
  {
    dcmlite::TimeIt ti;  // Log the time.
    reader.ReadFile(file_path);
  }

  std::cout << EndianToString(data_set.endian());

  GetTagsFromDataSet(data_set);

  std::cout << std::endl;
}

void ReadSpecificTags(const std::string& file_path) {
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
#endif  // 0

void Help(const char* argv0) {
  std::cout << "Usage:" << std::endl;
  std::cout << "  " << argv0 << " info|dump|copy <file_path>" << std::endl;
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    Help(argv[0]);
    return 1;
  }

  std::cout << "Platform: "
            << EndianToString(dcmlite::PlatformEndian())
            << std::endl << std::endl;

  std::string command = argv[1];
  std::cout << "command: " << command << std::endl;

  std::string file_path = argv[2];
  std::cout << "file path: " << file_path << std::endl;

  std::cout << std::endl;

  if (command == "info") {
    InfoDicomFile(file_path);
  } else if (command == "dump") {
    DumpDicomFile(file_path);
  } else if (command == "copy") {
    CopyDicomFile(file_path);
  }

  return 0;
}
