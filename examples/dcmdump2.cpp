#include <iostream>

#include "dcm/dicom_file.h"
#include "dcm/logger.h"
#include "dcm/print_visitor.h"

void DumpDicomFile(const dcm::Path& path) {
  dcm::DicomFile dicom_file(path);

  if (!dicom_file.Load()) {
    std::cerr << "Failed to load file." << std::endl;
    return;
  }

  dcm::PrintVisitor visitor(std::cout);
  dicom_file.Accept(visitor);
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Dump a DICOM file by firstly loading the whole data set."
              << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  " << argv[0] << " <file path>" << std::endl;
    return 1;
  }

  //DCM_LOG_INIT("", dcm::LOG_CONSOLE);

  DumpDicomFile(argv[1]);

  return 0;
}
