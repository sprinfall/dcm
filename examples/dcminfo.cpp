#include <iomanip>
#include <iostream>
#include <string>

#include "dcm/data_set.h"
#include "dcm/dicom_reader.h"
#include "dcm/full_read_handler.h"
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
  dcm::DataSet data_set;
  dcm::FullReadHandler read_handler(&data_set);
  dcm::DicomReader reader(&read_handler);

  if (!reader.ReadFile(path)) {
    std::cerr << "Failed to read file." << std::endl;
    return;
  }

  if (data_set.vr_type() == dcm::VR::EXPLICIT) {
    PrintKV("VR Type", "Explicit");
  } else {
    PrintKV("VR Type", "Implicit");
  }

  PrintKV("Byte Order", ByteOrderStr(data_set.byte_order()));

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

  //DCM_LOG_INIT("", dcm::LOG_CONSOLE);

  const char* file_path = argv[1];

  PrintKV("Byte Order (OS)", ByteOrderStr(dcm::kByteOrderOS));
  PrintKV("DICOM File", file_path);

  std::cout << std::endl;

  InfoDicomFile(file_path);

  return 0;
}
