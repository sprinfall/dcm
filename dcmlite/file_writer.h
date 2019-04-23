#ifndef DCMLITE_FILE_WRITER_H_
#define DCMLITE_FILE_WRITER_H_

#include "dcmlite/writer.h"

#include "boost/filesystem/fstream.hpp"

namespace dcmlite {

class FileWriter : public Writer {
public:
  using Path = boost::filesystem::path;

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

}  // namespace dcmlite

#endif  // DCMLITE_FILE_WRITER_H_
