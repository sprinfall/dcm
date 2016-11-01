#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <string>

#include "dcmlite/defs.h"
#include "dcmlite/dicom_file.h"
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

  dcmlite::DicomFile dicom_file;
  dicom_file.Read(file_path);

  return 0;
}
