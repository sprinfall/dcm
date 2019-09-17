#include "dcm/defs.h"

#include <array>
#include <iomanip>
#include <set>

#include "dcm/singleton_base.h"
#include "dcm/util.h"

namespace dcm {

// -----------------------------------------------------------------------------

static union {
  char c[4];
  unsigned long mylong;
} byte_order_test = { { 'l', '?', '?', 'b' } };

const ByteOrder kByteOrderOS =
    ((char)byte_order_test.mylong == 'l') ? ByteOrder::LE : ByteOrder::BE;

// -----------------------------------------------------------------------------

// VR dictionary.
// A singleton to cache all valid VRs.
class VRDict : public SingletonBase<VRDict> {
public:
  ~VRDict() = default;

  bool IsValid(int code) const {
    return code_set_.find(code) != code_set_.end();
  }

private:
  friend class SingletonBase<VRDict>;

  VRDict() {
    code_set_.insert(VR::AE);
    code_set_.insert(VR::AS);
    code_set_.insert(VR::AT);
    code_set_.insert(VR::CS);
    code_set_.insert(VR::DA);
    code_set_.insert(VR::DS);
    code_set_.insert(VR::DT);
    code_set_.insert(VR::FL);
    code_set_.insert(VR::FD);
    code_set_.insert(VR::IS);
    code_set_.insert(VR::LO);
    code_set_.insert(VR::LT);
    code_set_.insert(VR::OB);
    code_set_.insert(VR::OD);
    code_set_.insert(VR::OF);
    code_set_.insert(VR::OL);
    code_set_.insert(VR::OW);
    code_set_.insert(VR::PN);
    code_set_.insert(VR::SH);
    code_set_.insert(VR::SL);
    code_set_.insert(VR::SQ);
    code_set_.insert(VR::SS);
    code_set_.insert(VR::ST);
    code_set_.insert(VR::TM);
    code_set_.insert(VR::UC);
    code_set_.insert(VR::UI);
    code_set_.insert(VR::UL);
    code_set_.insert(VR::UN);
    code_set_.insert(VR::UR);
    code_set_.insert(VR::US);
    code_set_.insert(VR::UT);
  }

private:
  std::set<int> code_set_;
};

VR::VR(const char bytes[2]) {
  int code = MAKE_VR_CODE(bytes[0], bytes[1]);

  if (VRDict::Instance()->IsValid(code)) {
    code_ = static_cast<VR::Code>(code);
  } else {
    code_ = UN;
  }
}

bool VR::SetBytes(const char bytes[2]) {
  int code = MAKE_VR_CODE(bytes[0], bytes[1]);

  if (!VRDict::Instance()->IsValid(code)) {
    return false;
  }

  code_ = static_cast<VR::Code>(code);
  return true;
}

static bool Find(const VR::Code* codes, std::size_t size, VR::Code code) {
  const auto end = codes + size;
  return std::find(codes, end, code) != end;
}

bool VR::Is16BitsFollowingReversed() const {
  static const Code kCodes[] = {
    OB, OD, OF, OL, OW, SQ, UN, UC, UR, UT
  };

  return Find(kCodes, ARRAY_SIZE(kCodes), code_);
}

bool VR::IsString() const {
  static const Code kCodes[] = {
    AE, AS, CS, DA, TM, DT, DS, IS, LO, ST, LT, UT, PN, SH, UC, UI, UR,
  };

  return Find(kCodes, ARRAY_SIZE(kCodes), code_);
}

// TODO: Add an optional output parameter |size|.
bool VR::IsNumber() const {
  static const Code kCodes[] = {
    AT, US, SS, UL, SL, FL, FD, OW, OF, OL, OD,
  };

  return Find(kCodes, ARRAY_SIZE(kCodes), code_);
}

// TODO: Merge to IsString() as an optional output parameter?
bool VR::IsBackSlashVM() const {
  static const Code kVRCodes[] = {
    AE, AS, CS, DA, DS, DT, TM, IS, UI,
    SH, LO, PN, UC,
  };

  const auto end = kVRCodes + ARRAY_SIZE(kVRCodes);

  return std::find(kVRCodes, end, code_) != end;
}

// -----------------------------------------------------------------------------

const std::size_t VM::N = -1;

VM::VM(const char* str) : min_(0), max_(0), times_(1) {
  if (!Parse(str)) {
    min_ = max_ = 0;
    times_ = 1;
  }
}

bool VM::Check(std::size_t n) const {
  if (!IsRange()) {
    return n == value();
  }

  if (times_ != 1) {
    if (n % times_ != 0) {
      return false;
    }
    n /= times_;
  }

  return n >= min_ && n <= max_;
}

static bool StrToSize(const std::string& str, std::size_t* size) {
  try {
    *size = std::stoul(str);
  } catch (const std::exception&) {
    return false;
  }
  return true;
}

bool VM::Parse(const std::string& str) {
  std::size_t pos = str.find('-');
  if (pos == std::string::npos) {
    if (!StrToSize(str, &min_)) {
      return false;
    }
    max_ = min_;
    return true;
  }

  if (!StrToSize(str.substr(0, pos), &min_)) {
    return false;
  }

  std::string max_str = str.substr(pos + 1);
  if (max_str.empty()) {
    return false;
  }

  if (max_str.back() != 'n') {
    if (!StrToSize(max_str, &max_)) {
      return false;
    }
  } else {
    max_ = -1;

    if (max_str.size() > 2) {
      return false;
    }
    if (max_str.size() == 2) {
      if (max_str[0] == '2' && min_ == 2) {  // "2-2n"
        times_ = 2;
        min_ = 1;
      } else if (max_str[0] == '3' && min_ == 3) {  // "3-3n"
        times_ = 3;
        min_ = 1;
      } else {
        return false;
      }
    }
  }

  return true;
}

// -----------------------------------------------------------------------------

Tag Tag::SwapBytes() const {
  return Tag(util::SwapUint16(group_), util::SwapUint16(element_));
}

void Tag::Print(std::ostream& os, bool uppercase, bool as_uint32,
                const char* separator) const {
  std::ios::fmtflags old_flags = os.flags(std::ios::right | std::ios::hex);

  if (uppercase) {
    os.setf(std::ios::uppercase);
  }

  os << std::setfill('0');

  if (as_uint32) {
    os << std::setw(8) << ToUint32();
  } else {
    os << std::setw(4) << group_;
    os << separator;
    os << std::setw(4) << element_;
  }

  os.flags(old_flags);
}

}  // namespace dcm
