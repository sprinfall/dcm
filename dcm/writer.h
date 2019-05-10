#ifndef DCM_WRITER_H_
#define DCM_WRITER_H_

#include <cassert>
#include <cstdint>
#include <ostream>

namespace dcm {

class Writer {
public:
  Writer(std::ostream* ostream = nullptr) : ostream_(ostream) {}

  void Init(std::ostream* ostream) {
    ostream_ = ostream;
  }

  bool IsOk() const {
    return ostream_ != nullptr && !ostream_->bad();
  }

  void WriteByte(char byte) {
    assert(IsOk());
    ostream_->put(byte);
  }

  void WriteBytes(const void* bytes, std::size_t count) {
    assert(IsOk());
    ostream_->write(reinterpret_cast<const char*>(bytes), count);
  }

  void WriteUint8(std::uint8_t value) {
    WriteBytes(&value, 1);
  }

  // NOTE: Byte order is not considered.
  void WriteUint16(std::uint16_t value) {
    WriteBytes(&value, 2);
  }

  // NOTE: Byte order is not considered.
  void WriteUint32(std::uint32_t value) {
    WriteBytes(&value, 4);
  }

protected:
  std::ostream* ostream_;
};

}  // namespace dcm

#endif  // DCM_WRITER_H_
