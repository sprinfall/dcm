#include <iostream>

#include "dcm/dicom_file.h"
#include "dcm/logger.h"

void CopyDicomFile(const dcm::Path& path, const dcm::Path& path_copy) {
  if (path == path_copy) {
    std::cerr << "Please specify a different copy path." << std::endl;
    return;
  }

  dcm::DicomFile dicom_file(path);
  if (!dicom_file.Load()) {
    std::cerr << "Failed to read DICOM file." << std::endl;
    return;
  }

  // TODO: Add command line parameters.
  std::string transfer_syntax = dcm::transfer_syntax_uids::kImplicitLittleEndian;
  //std::string transfer_syntax = dcm::transfer_syntax_uids::kExplicitBigEndian;
  dicom_file.SetTransferSyntax(transfer_syntax);

  if (!dicom_file.Save(path_copy)) {
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

  DCM_LOG_INIT("", dcm::LOG_CONSOLE);
  
  CopyDicomFile(argv[1], argv[2]);

  return 0;
}
