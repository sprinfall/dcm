#include <iostream>

#include "dcm/dicom_reader.h"
#include "dcm/dump_read_handler.h"
#include "dcm/logger.h"

void DumpDicomFile(const dcm::Path& path) {
  dcm::DumpReadHandler read_handler(std::cout);
  dcm::DicomReader reader(&read_handler);

  if (!reader.ReadFile(path)) {
    std::cerr << "Failed to read file." << std::endl;
  }
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Usage:" << std::endl;
    std::cout << "  " << argv[0] << " <file path>" << std::endl;
    return 1;
  }

  //DCM_LOG_INIT("", dcm::LOG_CONSOLE);

  DumpDicomFile(argv[1]);

  return 0;
}
