#include <iomanip>
#include <iostream>
#include <string>

#include "dcm/data_set.h"
#include "dcm/dicom_reader.h"
#include "dcm/read_handler.h"

static std::string EndianToString(dcm::Endian endian) {
  if (endian == dcm::kLittleEndian) {
    return "Little Endian";
  } else {
    return "Big Endian";
  }
}

static void DumpDicomFile(const boost::filesystem::path& path) {
  dcm::DumpReadHandler read_handler;
  dcm::DicomReader reader(&read_handler);
  reader.ReadFile(path);

  std::cout << std::endl;
}

static void Help(const char* argv0) {
  std::cout << "Usage:" << std::endl;
  std::cout << "  " << argv0 << " <file_path>" << std::endl;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    Help(argv[0]);
    return 1;
  }

  std::cout << "Platform: "
            << EndianToString(dcm::PlatformEndian())
            << std::endl << std::endl;

  const char* file_path = argv[1];
  std::cout << "File path: " << file_path << std::endl;
  std::cout << std::endl;

  DumpDicomFile(file_path);

  return 0;
}
