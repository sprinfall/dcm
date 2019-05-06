#include <iostream>

#include "dcm/data_set.h"
#include "dcm/dicom_reader.h"
#include "dcm/full_read_handler.h"
#include "dcm/logger.h"
#include "dcm/print_visitor.h"

void DumpDicomFile(const dcm::Path& path) {
  dcm::DataSet data_set;
  dcm::FullReadHandler read_handler(&data_set);
  dcm::DicomReader reader(&read_handler);

  if (!reader.ReadFile(path)) {
    std::cerr << "Failed to read file." << std::endl;
    return;
  }

  dcm::PrintVisitor visitor(std::cout);
  data_set.Accept(visitor);
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
