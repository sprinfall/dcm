#include <iomanip>
#include <iostream>
#include <string>

#include "dcm/dicom_file.h"
#include "dcm/logger.h"
#include "dcm/util.h"

static const char* ByteOrderStr(dcm::ByteOrder byte_order) {
  if (byte_order == dcm::ByteOrder::LE) {
    return "Little Endian";
  } else {
    return "Big Endian";
  }
}

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

// Print basic info (VR type, byte order, etc.).
void InfoDicomFile(const dcm::Path& path) {
  dcm::DicomFile dicom_file(path);

  if (!dicom_file.Load()) {
    std::cerr << "Failed to read file." << std::endl;
    return;
  }

  if (dicom_file.vr_type() == dcm::VR::EXPLICIT) {
    PrintKV("VR Type", "Explicit");
  } else {
    PrintKV("VR Type", "Implicit");
  }

  PrintKV("Byte Order", ByteOrderStr(dicom_file.byte_order()));

  std::string charset;
  dicom_file.GetString(dcm::tags::kSpecificCharacterSet, &charset);

  PrintKV("Specific Character Set", charset);
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Usage:" << std::endl;
    std::cout << "  " << argv[0] << " <file path>" << std::endl;
    return 1;
  }

  //DCM_LOG_INIT("", dcm::LOG_CONSOLE);

  const char* file_path = argv[1];

  PrintKV("Byte Order (OS)", ByteOrderStr(dcm::kByteOrderOS));
  PrintKV("DICOM File", file_path);

  std::cout << std::endl;

  InfoDicomFile(file_path);

  return 0;
}
