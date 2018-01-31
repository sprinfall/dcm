#include "dcmlite/binary_file.h"

namespace dcmlite {

BinaryFile::BinaryFile()
    : file_(NULL), mode_(Mode::READ) {
}

bool BinaryFile::Open(const char* filename, Mode mode) {
  Close();

  mode_ = mode;

  if (mode == Mode::READ) {
    file_ = std::fopen(filename, "rb");
  } else {
    file_ = std::fopen(filename, "wb");
  }

  return IsOk();
}

void BinaryFile::Close() {
  if (file_ != NULL) {
    std::fclose(file_);
  }
}

}  // namespace dcmlite
