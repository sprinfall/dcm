

std::wstring Utf8ToUtf16(const std::string& input) {
  return MB2WC(input, CP_UTF8);
}

std::string Utf16ToUtf8(const std::wstring& input) {
  return WC2MB(input, CP_UTF8);
}


std::string Utf16ToAnsi(const std::wstring& input) {
  return WC2MB(input, CP_ACP);
}

std::string Utf8ToAnsi(const std::string& input) {
  return Utf16ToAnsi(MB2WC(input, CP_UTF8));
}

