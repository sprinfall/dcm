#ifndef DCM_FILE_READER_H_
#define DCM_FILE_READER_H_

#include "dcm/reader.h"

#include "boost/filesystem/fstream.hpp"

namespace dcm {

class FileReader : public Reader {
public:
  using Path = boost::filesystem::path;

  FileReader(const Path& path) : stream_(path, std::ios::binary) {
    Reader::Init(&stream_);
  }

  ~FileReader() {
    Close();
  }

  FileReader(const FileReader& rhs) = delete;
  FileReader& operator=(const FileReader& rhs) = delete;

  bool Open(const Path& path) {
    stream_.open(path);
    return !stream_.bad();
  }

  void Close() {
    stream_.close();
  }

private:
  boost::filesystem::ifstream stream_;
};

}  // namespace dcm

#endif  // DCM_FILE_READER_H_
