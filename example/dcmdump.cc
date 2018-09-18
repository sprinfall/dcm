#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>

#include "dcmlite/dcmlite.h"

std::string EndianToString(dcmlite::Endian endian) {
  if (endian == dcmlite::kLittleEndian) {
    return "Little Endian";
  } else {
    return "Big Endian";
  }
}

void DumpDicomFile(const std::string& file_path) {
  dcmlite::DumpReadHandler read_handler;
  dcmlite::DicomReader reader(&read_handler);
  reader.ReadFile(file_path);

  std::cout << std::endl;
}

void Help(const char* argv0) {
  std::cout << "Usage:" << std::endl;
  std::cout << "  " << argv0 << " <file_path>" << std::endl;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    Help(argv[0]);
    return 1;
  }

  std::cout << "Platform: "
            << EndianToString(dcmlite::PlatformEndian())
            << std::endl << std::endl;

  std::string file_path = argv[1];
  std::cout << "file path: " << file_path << std::endl;
  std::cout << std::endl;

  DumpDicomFile(file_path);

  return 0;
}
