#include <iomanip>
#include <iostream>
#include <string>

#include "dcm/data_set.h"
#include "dcm/dicom_reader.h"
#include "dcm/read_handler.h"
#include "dcm/util.h"

static void DumpDicomFile(const dcm::Path& path) {
  dcm::DumpReadHandler read_handler;
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

  DumpDicomFile(argv[1]);

  return 0;
}
