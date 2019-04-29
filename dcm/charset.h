#ifndef DCM_CHARSET_H_
#define DCM_CHARSET_H_

#include <map>
#include <string>

#include "dcm/singleton_base.h"

namespace dcm {

// -----------------------------------------------------------------------------

// DICOM charsets enum type.
enum Charset {
  UNKNOWN = -1,
  ISO_IR_6,
  ISO_IR_100,
  ISO_IR_101,
  ISO_IR_109,
  ISO_IR_110,
  ISO_IR_144,
  ISO_IR_127,
  ISO_IR_126,
  ISO_IR_138,
  ISO_IR_148,
  ISO_IR_149,
  ISO_IR_13,
  ISO_IR_14,
  ISO_IR_166,
  ISO_IR_87,
  ISO_IR_159,
  ISO_2022_IR_6,
  ISO_2022_IR_100,
  ISO_2022_IR_101,
  ISO_2022_IR_109,
  ISO_2022_IR_110,
  ISO_2022_IR_144,
  ISO_2022_IR_127,
  ISO_2022_IR_126,
  ISO_2022_IR_138,
  ISO_2022_IR_148,
  ISO_2022_IR_149,
  ISO_2022_IR_13,
  ISO_2022_IR_14,
  ISO_2022_IR_166,
  ISO_2022_IR_87,
  ISO_2022_IR_159,
  ISO_IR_192,
  GB18030,
};

// -----------------------------------------------------------------------------

struct CharsetInfo {
  std::string dicom_name;         // DICOM name for the charset
  std::string escape_sequence;    // Escape sequence for ISO 2022
  std::string iso_registration;   // ISO registration string
  unsigned int code_page;         // Windows Code Page
};

// -----------------------------------------------------------------------------

// Charset dictionary.
class CharsetDict : public SingletonBase<CharsetDict> {
public:
  ~CharsetDict() = default;

  // Name -> Enum
  // E.g., "ISO_IR_192" -> Charset::ISO_IR_192
  Charset GetEnum(const std::string& dicom_name) const;

  const CharsetInfo& GetInfo(Charset charset) const;

private:
  CharsetDict() {
    Init();
  }

private:
  friend class SingletonBase<CharsetDict>;

  void Init();

  void Register(Charset type,
                const char* dicom_name,
                const char* escape_sequence,
                const char* iso_registration,
                unsigned int code_page);

private:
  std::map<Charset, CharsetInfo> map_;
};

// -----------------------------------------------------------------------------

// Charset converters.

// Convert UTF-8 data to byte stream with the given charset.
// Characters might be lost during the conversion.
std::string Utf8ToBytes(const std::string& utf8_data, Charset charset);

// Convert byte stream with the given charset to UTF-8 data.
std::string BytesToUtf8(const std::string& bytes, Charset charset);

// Convert from UTF-16 string to byte stream using the given charset.
// E.g., Utf16ToBytes(L"<input>", ISO_IR_192). Since ISO_IR_192 is just UTF-8,
// the result bytes is a UTF-8 byte stream.
std::string Utf16ToBytes(const std::wstring& utf16_data, Charset charset);

}  // namespace dcm

#endif  // DCM_CHARSET_H_
