#include "dcm/charset.h"

#if defined(WIN32) || defined(WIN64)
#include <Windows.h>
#endif

namespace dcm {

// -----------------------------------------------------------------------------

Charset CharsetDict::GetEnum(const std::string& dicom_name) const {
  for (const auto& pair : map_) {
    if (pair.second.dicom_name == dicom_name) {
      return pair.first;
    }
  }
  return Charset::UNKNOWN;
}

const CharsetInfo& CharsetDict::GetInfo(Charset charset) const {
  const auto& it = map_.find(charset);
  if (it != map_.end()) {
    return it->second;
  }
  throw std::range_error("");  // TODO
}

void CharsetDict::Init() {
  Register(ISO_IR_6, "ISO_IR 6", "", "ISO-IR-6", 1252);
  Register(ISO_IR_100, "ISO_IR 100", "", "ISO-IR-100", 1252);
  Register(ISO_IR_101, "ISO_IR 101", "", "ISO-IR-101", 28592);
  Register(ISO_IR_109, "ISO_IR 109", "", "ISO-IR-109", 28593);
  Register(ISO_IR_110, "ISO_IR 110", "", "ISO-IR-110", 28594);
  Register(ISO_IR_144, "ISO_IR 144", "", "ISO-IR-144", 28595);
  Register(ISO_IR_127, "ISO_IR 127", "", "ISO-IR-127", 28596);
  Register(ISO_IR_126, "ISO_IR 126", "", "ISO-IR-126", 28597);
  Register(ISO_IR_138, "ISO_IR 138", "", "ISO-IR-138", 28598);
  Register(ISO_IR_148, "ISO_IR 148", "", "ISO-IR-148", 28599);
  Register(ISO_IR_149, "ISO_IR 149", "", "ISO-IR-149", 949);
  Register(ISO_IR_13, "ISO_IR 13", "", "ISO-IR-13", 50930);
  Register(ISO_IR_14, "ISO_IR 14", "", "ISO-IR-14", 932);
  Register(ISO_IR_166, "ISO_IR 166", "", "ISO-IR-166", 874);
  Register(ISO_IR_87, "ISO_IR 87", "", "ISO-IR-87", 20932);
  Register(ISO_IR_159, "ISO_IR 159", "", "ISO-IR-159", 20932);
  Register(ISO_2022_IR_6, "ISO 2022 IR 6", "\x1b\x28\x42", "ISO-IR-6", 1252);
  Register(ISO_2022_IR_100, "ISO 2022 IR 100", "\x1b\x2d\x41", "ISO-IR-100", 1252);
  Register(ISO_2022_IR_101, "ISO 2022 IR 101", "\x1b\x2d\x42", "ISO-IR-101", 28592);
  Register(ISO_2022_IR_109, "ISO 2022 IR 109", "\x1b\x2d\x43", "ISO-IR-109", 28593);
  Register(ISO_2022_IR_110, "ISO 2022 IR 110", "\x1b\x2d\x44", "ISO-IR-110", 28594);
  Register(ISO_2022_IR_144, "ISO 2022 IR 144", "\x1b\x2d\x4c", "ISO-IR-144", 28595);
  Register(ISO_2022_IR_127, "ISO 2022 IR 127", "\x1b\x2d\x47", "ISO-IR-127", 28596);
  Register(ISO_2022_IR_126, "ISO 2022 IR 126", "\x1b\x2d\x46", "ISO-IR-126", 28597);
  Register(ISO_2022_IR_138, "ISO 2022 IR 138", "\x1b\x2d\x48", "ISO-IR-138", 28598);
  Register(ISO_2022_IR_148, "ISO 2022 IR 148", "\x1b\x2d\x4d", "ISO-IR-148", 28599);
  Register(ISO_2022_IR_149, "ISO 2022 IR 149", "\x1b\x24\x29\x43", "ISO-IR-149", 949);
  Register(ISO_2022_IR_13, "ISO 2022 IR 13", "\x1b\x29\x49", "ISO-IR-13", 50930);
  Register(ISO_2022_IR_14, "ISO 2022 IR 14", "\x1b\x28\x4a", "ISO-IR-14", 932);
  Register(ISO_2022_IR_166, "ISO 2022 IR 166", "\x1b\x2d\x54", "ISO-IR-166", 874);
  Register(ISO_2022_IR_87, "ISO 2022 IR 87", "\x1b\x24\x42", "ISO-IR-87", 20932);
  Register(ISO_2022_IR_159, "ISO 2022 IR 159", "\x1b\x24\x28\x44", "ISO-IR-159", 20932);
  Register(ISO_IR_192, "ISO_IR 192", "", "UTF-8", 65001);
  Register(GB18030, "GB18030", "", "GB18030", 54936);
}

void CharsetDict::Register(Charset type,
                           const char* dicom_name,
                           const char* escape_sequence,
                           const char* iso_registration,
                           unsigned int code_page) {
  map_[type] = { dicom_name, escape_sequence, iso_registration, code_page };
}

// -----------------------------------------------------------------------------

// Windows specific helper functions.

#if defined(WIN32) || defined(WIN64)

// Wrapper for Windows API MultiByteToWideChar.
static std::wstring MB2WC(const std::string& input, DWORD code_page) {
  if (input.empty()) {
    return L"";
  }

  int length = ::MultiByteToWideChar(code_page, 0, &input[0],
                                     static_cast<int>(input.size()),
                                     NULL, 0);

  std::wstring output(length, '\0');

  ::MultiByteToWideChar(code_page, 0, &input[0], static_cast<int>(input.size()),
                        &output[0], static_cast<int>(output.size()));

  return output;
}

// Wrapper for Windows API WideCharToMultiByte().
static std::string WC2MB(const std::wstring& input, DWORD code_page) {
  if (input.empty()) {
    return "";
  }

  // There do have other code pages which require the flags to be 0, e.g.,
  // 50220, 50211, and so on. But they are not included in our charset
  // dictionary. So, only consider 65001 (UTF-8) and 54936 (GB18030).
  DWORD flags = 0;
  if (code_page != 65001 && code_page != 54936) {
    flags = WC_NO_BEST_FIT_CHARS | WC_COMPOSITECHECK | WC_DEFAULTCHAR;
  }

  int length = ::WideCharToMultiByte(code_page, flags, &input[0],
                                     static_cast<int>(input.size()), NULL, 0,
                                     NULL, NULL);

  std::string output(length, '\0');

  ::WideCharToMultiByte(code_page, flags, &input[0],
                        static_cast<int>(input.size()), &output[0],
                        static_cast<int>(output.size()), NULL, NULL);

  return output;
}

#endif  // defined(WIN32) || defined(WIN64)

// -----------------------------------------------------------------------------

std::string Utf8ToBytes(const std::string& utf8_data, Charset charset) {
  if (charset == ISO_IR_192) {
    return utf8_data;
  }

  const auto& info = CharsetDict::Instance()->GetInfo(charset);

#if defined(WIN32) || defined(WIN64)
  return WC2MB(MB2WC(utf8_data, CP_UTF8), info.code_page);
#else
  // TODO: libiconv
#endif
}

std::string BytesToUtf8(const std::string& bytes, Charset charset) {
  if (charset == ISO_IR_192) {
    return bytes;
  }

  const auto& info = CharsetDict::Instance()->GetInfo(charset);

#if defined(WIN32) || defined(WIN64)
  return WC2MB(MB2WC(bytes, info.code_page), CP_UTF8);
#else
  // TODO: libiconv
#endif
}

std::string Utf16ToBytes(const std::wstring& utf16_data, Charset charset) {
  const auto& info = CharsetDict::Instance()->GetInfo(charset);

#if defined(WIN32) || defined(WIN64)
  return WC2MB(utf16_data, info.code_page);
#else
  // TODO: libiconv
#endif
}

}  // namespace dcm
