#include <iomanip>
#include <iostream>
#include <string>

#include "dcmlite/dcmlite.h"

std::string EndianToString(dcmlite::Endian endian) {
  if (endian == dcmlite::kLittleEndian) {
    return "Little Endian";
  } else {
    return "Big Endian";
  }
}

void DumpDicomFile(const boost::filesystem::path& path) {
  dcmlite::DumpReadHandler read_handler;
  dcmlite::DicomReader reader(&read_handler);
  reader.ReadFile(path);

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

  const char* file_path = argv[1];
  std::cout << "File path: " << file_path << std::endl;
  std::cout << std::endl;

  DumpDicomFile(file_path);

  return 0;
}
