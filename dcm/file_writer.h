#ifndef DCM_FILE_WRITER_H_
#define DCM_FILE_WRITER_H_

#include "dcm/defs.h"
#include "dcm/writer.h"

#include "boost/filesystem/fstream.hpp"

namespace dcm {

class FileWriter : public Writer {
public:
  FileWriter(const Path& path) : stream_(path, std::ios::binary) {
    Writer::Init(&stream_);
  }

  ~FileWriter() {
    Close();
  }

  FileWriter(const FileWriter& rhs) = delete;
  FileWriter& operator=(const FileWriter& rhs) = delete;

  bool Open(const Path& path) {
    stream_.open(path);
    return !stream_.bad();
  }

  void Close() {
    stream_.close();
  }

private:
  boost::filesystem::ofstream stream_;
};

}  // namespace dcm

#endif  // DCM_FILE_WRITER_H_
