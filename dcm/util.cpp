#include "dcm/util.h"

#include <map>

#include "dcm/singleton_base.h"

namespace dcm {

// -----------------------------------------------------------------------------

std::uint16_t SwapUint16(std::uint16_t value) {
  return (((value & 0xFF00) >> 8) | ((value & 0x00FF) << 8));
}

std::uint32_t SwapUint32(std::uint32_t value) {
  return (((value & 0x000000FF) << 24) |
          ((value & 0x0000FF00) << 8) |
          ((value & 0x00FF0000) >> 8) |
          ((value & 0xFF000000) >> 24));
}

void Swap16(void* src) {
  std::uint16_t* p = (std::uint16_t*)src;
  *p = SwapUint16(*p);
}

void Swap32(void* src) {
  std::uint32_t* p = (std::uint32_t*)src;
  *p = SwapUint32(*p);
}

void Swap64(void* src) {
  std::uint32_t* p = (std::uint32_t*)src;
  std::uint32_t c = p[0];

  p[0] = SwapUint32(p[1]);
  p[1] = SwapUint32(c);
}

// -----------------------------------------------------------------------------

class VRInfo {
public:
  VR vr;

  // The length of word; used for byte reversing in little/big endian
  // conversion. For numeric VRs only ((FL, FD, OL, OW, etc.).
  std::uint32_t word_size;

  // The maximum length of the data.
  std::uint32_t max_data_length;
};

// VR dictionary.
// A singleton to cache all valid VRs.
class VRDict : public SingletonBase<VRDict> {
public:
  ~VRDict() = default;

  bool IsValidVR(int vr) const {
    return vr_map_.find(static_cast<VR>(vr)) != vr_map_.end();
  }

private:
  friend class SingletonBase<VRDict>;

  VRDict() {
    RegisterVR(VR::AE, 0, 16);
    RegisterVR(VR::AS, 0, 0);
    RegisterVR(VR::AT, 2, 0);
    RegisterVR(VR::CS, 0, 16);
    RegisterVR(VR::DA, 0, 0);
    RegisterVR(VR::DS, 0, 16);
    RegisterVR(VR::DT, 0, 26);
    RegisterVR(VR::FL, 4, 0);
    RegisterVR(VR::FD, 8, 0);
    RegisterVR(VR::IS, 0, 12);
    RegisterVR(VR::LO, 0, 64);
    RegisterVR(VR::LT, 0, 10240);
    RegisterVR(VR::OB, 0, 0);
    RegisterVR(VR::OF, 4, 0);
    RegisterVR(VR::OL, 4, 0);
    RegisterVR(VR::OW, 2, 0);
    RegisterVR(VR::PN, 0, 64);
    RegisterVR(VR::SH, 0, 16);
    RegisterVR(VR::SL, 4, 0);
    RegisterVR(VR::SQ, 0, 0);
    RegisterVR(VR::SS, 2, 0);
    RegisterVR(VR::ST, 0, 1024);
    RegisterVR(VR::TM, 0, 16);
    RegisterVR(VR::UI, 0, 64);
    RegisterVR(VR::UL, 4, 0);
    RegisterVR(VR::UN, 0, 0);
    RegisterVR(VR::US, 2, 0);
    RegisterVR(VR::UT, 0, 0);
  }

  void RegisterVR(VR vr, std::uint32_t word_size,
                  std::uint32_t max_data_length) {
    VRInfo vr_info;
    vr_info.vr = vr;
    vr_info.word_size = word_size;
    vr_info.max_data_length = max_data_length;

    vr_map_[vr] = vr_info;
  }

private:
  std::map<VR, VRInfo> vr_map_;
};

std::string VRToString(VR vr) {
  if (vr == UNKNOWN) {
    return "UNKNOWN";
  }

  std::string str(2, ' ');
  str[0] = (((std::uint16_t)vr) & 0xFF00) >> 8;
  str[1] = (((std::uint16_t)vr) & 0x00FF);
  return str;
}

bool StringToVR(const std::string& str, VR* vr) {
  if (str.size() != 2) {
    return false;
  }

  int vr_enum = MAKE_VR_ENUM(str[0], str[1]);

  if (!VRDict::Instance()->IsValidVR(vr_enum)) {
    return false;
  }

  if (vr != nullptr) {
    *vr = static_cast<VR>(vr_enum);
  }

  return true;
}

bool Is16BitsFollowingVrReversed(VR vr) {
  static const VR kVRs[] = {
    VR::OB,
    VR::OD,
    VR::OF,
    VR::OL,
    VR::OW,
    VR::SQ,
    VR::UN,
    VR::UC,
    VR::UR,
    VR::UT,
  };

  std::size_t size = sizeof(kVRs) / sizeof(VR);

  for (std::size_t i = 0; i < size; ++i) {
    if (vr == kVRs[i]) {
      return true;
    }
  }

  return false;
}

}  // namespace dcm
