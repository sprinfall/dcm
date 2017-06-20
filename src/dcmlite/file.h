#ifndef DCMLITE_FILE_H_
#define DCMLITE_FILE_H_
#pragma once

// A wrapper of C file IO.

#include <cstdio>
#include <cstdint>
#include <string>
#include <cassert>

namespace dcmlite {

class File {
public:
  File() : file_(NULL) {
  }

  ~File() {
    Close();
  }

  // \param mode E.g., "rb".
  bool Open(const char* filename, const char* mode) {
    Close();
    file_ = std::fopen(filename, mode);
    return IsOk();
  }

  void Close() {
    if (file_ != NULL) {
      std::fclose(file_);
    }
  }

  bool IsOk() const {
    return file_ != NULL;
  }

  // \param origin Position to which offset is added (SEEK_SET, SEEK_CUR,
  //               or SEEK_END).
  bool Seek(long offset, int origin = SEEK_SET) {
    assert(IsOk());
    return std::fseek(file_, offset, origin) == 0;
  }

  size_t ReadBytes(void* bytes, size_t count) {
    assert(IsOk());
    return std::fread(bytes, 1, count, file_);
  }

  bool UndoRead(size_t byte_count) {
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

  bool ReadString(std::string* value, size_t count) {
    value->resize(count);

    // NOTE:
    // Don't do like this:
    //    ReadBytes(value, count)
    // It doesn't work!
    return ReadBytes(&(*value)[0], count) == count;
  }

private:
  std::FILE* file_;
};

}  // namespace dcmlite

#endif  // DCMLITE_FILE_H_
