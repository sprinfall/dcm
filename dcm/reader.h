#ifndef DCM_READER_H_
#define DCM_READER_H_

#include <cassert>
#include <cstdint>
#include <istream>
#include <string>

namespace dcm {

class Reader {
public:
  Reader(std::istream* istream = nullptr) : istream_(istream) {}

  void Init(std::istream* istream) {
    istream_ = istream;
  }

  bool IsOk() const {
    return istream_ != nullptr && !istream_->bad();
  }

  void Seek(long offset, std::ios::seek_dir dir = std::ios::beg) {
    assert(IsOk());
    istream_->seekg(offset, dir);
  }

  std::streamsize ReadBytes(void* bytes, std::size_t count) {
    assert(IsOk());
    istream_->read(reinterpret_cast<char*>(bytes), count);
    return istream_->gcount();
  }

  void UndoRead(std::size_t byte_count) {
    return Seek(-(long)byte_count, std::ios::cur);
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
    return ReadBytes(&(*value)[0], count) == count;
  }

protected:
  std::istream* istream_;
};

}  // namespace dcm

#endif  // DCM_READER_H_
