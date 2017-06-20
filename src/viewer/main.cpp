#include <iostream>
#include <string>
#include "dcmlite/dcmlite.h"

static void PrintTags(const dcmlite::DataSet& data_set) {
  std::string transfer_syntax_uid;
  if (data_set.GetString(dcmlite::Tag(0x0002, 0x0010), &transfer_syntax_uid)) {
    std::cout << "Transfer Syntax UID: " << transfer_syntax_uid << std::endl;
  }

  std::string patient_name;
  if (data_set.GetString(dcmlite::Tag(0x0010, 0x0010), &patient_name)) {
    std::cout << "Patient Name: " << patient_name << std::endl;
  }

  std::uint16_t samples_per_pixel;
  if (data_set.GetUint16(dcmlite::Tag(0x0028, 0x0002), &samples_per_pixel)) {
    std::cout << "Samples Per Pixel: " << samples_per_pixel << std::endl;
  }
}

static void Dump(const std::string& file_path) {
  dcmlite::DumpReadHandler read_handler;
  dcmlite::DicomReader reader(&read_handler);
  reader.ReadFile(file_path);
}


int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << argv[0] << " <file path>" << std::endl;
    return 1;
  }

  std::cout << "Platform endian: ";
  if (dcmlite::PlatformEndian() == dcmlite::kLittleEndian) {
    std::cout << "Little Endian" << std::endl;
  } else {
    std::cout << "Big Endian" << std::endl;
  }

  std::cout << std::endl;

  std::string file_path = argv[1];
  std::cout << "File path: " << file_path << std::endl << std::endl;

  Dump(file_path);
  std::cout << std::endl;

  dcmlite::DataSet data_set;
  dcmlite::FullReadHandler read_handler(&data_set);
  dcmlite::DicomReader reader(&read_handler);
  reader.ReadFile(file_path);

  dcmlite::PrintVisitor print_visitor(std::cout);
  data_set.Accept(print_visitor);
  std::cout << std::endl;

  PrintTags(data_set);

  return 0;
}
