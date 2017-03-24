#include "dcmlite/vr.h"

#include <map>
#include <memory>  // for unique_ptr

namespace dcmlite {

// VR dictionary.
// A singleton to cache all valid VRs.
class VrDict {
public:
  static const VrDict& Get() {
    static std::unique_ptr<VrDict> s_instance(new VrDict());
    return *s_instance;
  }

  bool IsValidVR(int vr_enum) const {
    return vr_map_.find((VR::Type)vr_enum) != vr_map_.end();
  }

private:
  VrDict() {
    RegisterVR(VR::AE, false, 0, 16);
    RegisterVR(VR::AS, false, 0, 0);
    RegisterVR(VR::AT, false, 2, 0);
    RegisterVR(VR::CS, false, 0, 16);
    RegisterVR(VR::DA, false, 0, 0);
    RegisterVR(VR::DS, false, 0, 16);
    RegisterVR(VR::DT, false, 0, 26);
    RegisterVR(VR::FL, false, 4, 0);
    RegisterVR(VR::FD, false, 8, 0);
    RegisterVR(VR::IS, false, 0, 12);
    RegisterVR(VR::LO, false, 0, 64);
    RegisterVR(VR::LT, false, 0, 10240);
    RegisterVR(VR::OB, true, 0, 0);
    RegisterVR(VR::OF, true, 4, 0);
    RegisterVR(VR::OL, true, 4, 0);
    RegisterVR(VR::OW, true, 2, 0);
    RegisterVR(VR::PN, false, 0, 64);
    RegisterVR(VR::SH, false, 0, 16);
    RegisterVR(VR::SL, false, 4, 0);
    RegisterVR(VR::SQ, true, 0, 0);
    RegisterVR(VR::SS, false, 2, 0);
    RegisterVR(VR::ST, false, 0, 1024);
    RegisterVR(VR::TM, false, 0, 16);
    RegisterVR(VR::UI, false, 0, 64);
    RegisterVR(VR::UL, false, 4, 0);
    RegisterVR(VR::UN, true, 0, 0);
    RegisterVR(VR::US, false, 2, 0);
    RegisterVR(VR::UT, true, 0, 0);
  }

  void RegisterVR(VR::Type vr_type,
                  bool long_length,  // TODO
                  std::uint32_t word_size,
                  std::uint32_t max_data_length) {
    VR vr;
    vr.type = vr_type;
    vr.word_size = word_size;
    vr.max_data_length = max_data_length;

    vr_map_[vr_type] = vr;
  }

private:
  typedef std::map<VR::Type, VR> VrMap;
  VrMap vr_map_;
};

// static
std::string VR::ToString(VR::Type vr_type) {
  if (vr_type == UNKNOWN) {
    return "UNKNOWN";
  }

  std::string str(2, ' ');
  str[0] = (((unsigned short)vr_type) & 0xFF00) >> 8;
  str[1] = (((unsigned short)vr_type) & 0x00FF);
  return str;
}

// static
bool VR::FromString(const std::string& vr_str, VR::Type* vr_type) {
  int vr_enum = MAKE_VR_ENUM(vr_str[0], vr_str[1]);

  if (!VrDict::Get().IsValidVR(vr_enum)) {
    return false;
  }

  if (vr_type != NULL) {
    *vr_type = (VR::Type)vr_enum;
  }

  return true;
}

}  // namespace dcmlite
