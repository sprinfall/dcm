#include <iomanip>
#include <iostream>
#include <string>

#include "dcm/data_set.h"
#include "dcm/dicom_reader.h"
#include "dcm/read_handler.h"
#include "dcm/util.h"

static void PrintKV(const char* key, const char* value) {
  std::cout << std::left << std::setfill(' ') << std::setw(24)
            << key << ": " << value << std::endl;
}

static void PrintKV(const char* key, const std::string& value) {
  std::cout << std::left << std::setfill(' ') << std::setw(24)
            << key << ": " << value << std::endl;
}

static void PrintKV(const char* key, bool value) {
  std::cout << std::left << std::setfill(' ') << std::setw(24)
    << key << ": " << std::boolalpha << value << std::endl;
}

// Print basic info (Endian type, explicit VR or not, etc.).
void InfoDicomFile(const dcm::Path& path) {
  dcm::DataSet data_set;
  dcm::FullReadHandler read_handler(&data_set);
  dcm::DicomReader reader(&read_handler);

  if (!reader.ReadFile(path)) {
    std::cerr << "Failed to read file." << std::endl;
    return;
  }

  PrintKV("Endian", data_set.endian().name());
  PrintKV("Explicit VR", data_set.explicit_vr());

  std::string charset;
  data_set.GetString(0x00080005, &charset);
  PrintKV("Specific Character Set", charset);
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Usage:" << std::endl;
    std::cout << "  " << argv[0] << " <file path>" << std::endl;
    return 1;
  }

  std::string file_path = argv[1];

  PrintKV("Platform Endian", dcm::PlatformEndian().name());
  PrintKV("DICOM File", file_path);

  std::cout << std::endl;

  InfoDicomFile(file_path);

  return 0;
}
