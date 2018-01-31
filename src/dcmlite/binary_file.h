#ifndef DCMLITE_BINARY_FILE_H_
#define DCMLITE_BINARY_FILE_H_
#pragma once

// A wrapper of C file IO for reading and writing in binary mode.

#include <cstdio>
#include <cstdint>
#include <string>
#include <cassert>

namespace dcmlite {

class BinaryFile {
public:
  enum Mode {
    READ,
    WRITE,
  };

public:
  BinaryFile(const BinaryFile& rhs) = delete;
  BinaryFile& operator=(const BinaryFile& rhs) = delete;

  BinaryFile();

  ~BinaryFile() {
    Close();
  }

  bool Open(const char* filename, Mode mode);

  void Close();

  bool IsOk() const {
    return file_ != NULL;
  }

  Mode mode() const {
    return mode_;
  }

  // \param origin Position to which offset is added, could be
  //               SEEK_SET, SEEK_CUR, or SEEK_END.
  bool Seek(long offset, int origin = SEEK_SET) {
    assert(IsOk());
    return std::fseek(file_, offset, origin) == 0;
  }

  std::size_t ReadBytes(void* bytes, std::size_t count) {
    assert(IsOk());
    return std::fread(bytes, 1, count, file_);
  }

  bool UndoRead(std::size_t byte_count) {
    return Seek(- (long)byte_count, SEEK_CUR);
  }

  bool ReadUint8(std::uint8_t* value) {
    return ReadBytes(value, 1) == 1;
  }

  // NOTE: Byte order is not considered.
  bool ReadUint16(std::uint16_t* value) {
    return ReadBytes(value, 2) == 2;
  }

  // NOTE: Byte order is not considered.
  bool ReadUint32(std::uint32_t* value) {
    return ReadBytes(value, 4) == 4;
  }

  bool ReadString(std::string* value, std::size_t count) {
    value->resize(count);
    // NOTE: ReadBytes(value, count) doesn't work!
    return ReadBytes(&(*value)[0], count) == count;
  }

  std::size_t WriteBytes(const void* bytes, std::size_t count) {
    assert(IsOk());
    return std::fwrite(bytes, 1, count, file_);
  }

  bool WriteUint8(std::uint8_t value) {
    return WriteBytes(&value, 1) == 1;
  }

  // NOTE: Byte order is not considered.
  bool WriteUint16(std::uint16_t value) {
    return WriteBytes(&value, 2) == 2;
  }

  // NOTE: Byte order is not considered.
  bool WriteUint32(std::uint32_t value) {
    return WriteBytes(&value, 4) == 4;
  }

  bool WriteString(const std::string& value) {
    return WriteBytes(value.c_str(), value.size()) == value.size();
  }

private:
  std::FILE* file_;
  Mode mode_;
};

}  // namespace dcmlite

#endif  // DCMLITE_BINARY_FILE_H_
