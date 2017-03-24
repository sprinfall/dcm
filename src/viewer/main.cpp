#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <string>

#include "dcmlite/data_set.h"
#include "dcmlite/tag.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << argv[0] << " <file path>" << std::endl;
    return 1;
  }

  const char* file_path = argv[1];

  dcmlite::Endian platform_endian = dcmlite::GetPlatformEndian();

  std::cout << "Platform endian: ";
  if (platform_endian == dcmlite::kLittleEndian) {
    std::cout << "Little Endian" << std::endl;
  } else {
    std::cout << "Big Endian" << std::endl;
  }

  dcmlite::DataSet data_set;
  data_set.LoadFile(file_path);

  data_set.Dump();

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

  return 0;
}

// Example output:
// Transfer Syntax UID: 1.2.840.10008.1.2.1
// Patient Name : Gu^Adam
// Samples Per Pixel : 1