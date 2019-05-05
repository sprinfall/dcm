#include <iostream>

#include "dcm/data_set.h"
#include "dcm/dicom_reader.h"
#include "dcm/dicom_writer.h"
#include "dcm/full_read_handler.h"
#include "dcm/logger.h"

void CopyDicomFile(const dcm::Path& path, const dcm::Path& path_copy) {
  if (path == path_copy) {
    std::cerr << "Please specify a different copy path." << std::endl;
    return;
  }

  dcm::DataSet data_set;
  dcm::FullReadHandler read_handler(&data_set);
  dcm::DicomReader reader(&read_handler);
  reader.ReadFile(path);

  dcm::DicomWriter writer(&data_set);
  if (!writer.WriteFile(path_copy)) {
    std::cerr << "Failed to copy file!" << std::endl;
  } else {
    std::cout << "File copied." << std::endl;
  }
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Usage:" << std::endl;
    std::cout << "  " << argv[0] << " <file path> <file path copy>" << std::endl;
    return 1;
  }

  //DCM_LOG_INIT("", dcm::LOG_CONSOLE);
  
  CopyDicomFile(argv[1], argv[2]);

  return 0;
}
