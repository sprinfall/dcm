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

  bool Open(const char* filename) {
    Close();
    file_ = std::fopen(filename, "rb");
    return IsOk();
  }

  void Close() {
    if (file_ != NULL) {
      std::fclose(file_);
    }
  }

  inline bool IsOk() const {
    return file_ != NULL;
  }

  // \param origin Position to which offset is added (SEEK_SET, SEEK_CUR,
  //               or SEEK_END).
  inline bool Seek(long offset, int origin = SEEK_SET) {
    assert(IsOk());
    return std::fseek(file_, offset, origin) == 0;
  }

  inline size_t ReadBytes(void* bytes, size_t count) {
    assert(IsOk());
    return std::fread(bytes, 1, count, file_);
  }

  inline bool UndoRead(size_t byte_count) {
    return Seek(- (long)byte_count, SEEK_CUR);
  }

  inline bool ReadUint8(std::uint8_t* value) {
    return ReadBytes(value, 1) == 1;
  }

  // NOTE: Byte order is not considered.
  inline bool ReadUint16(std::uint16_t* value) {
    return ReadBytes(value, 2) == 2;
  }

  // NOTE: Byte order is not considered.
  inline bool ReadUint32(std::uint32_t* value) {
    return ReadBytes(value, 4) == 4;
  }

  inline bool ReadString(std::string* value, size_t count) {
    value->resize(count);
    // NOTE: ReadBytes(value, count) doesn't work!
    return ReadBytes(&(*value)[0], count) == count;
  }

private:
  std::FILE* file_;
};

}  // namespace dcmlite

#endif  // DCMLITE_FILE_H_
