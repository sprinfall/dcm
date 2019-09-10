#include <iostream>

#include "dcm/dicom_file.h"

namespace trophy {

// Trophy/CSD private tags.

const dcm::Tag kCustomDataTag = 0x000911A0;
const dcm::Tag kCustomDataKeyTag = 0x000911A1;
const dcm::Tag kCustomDataLengthTag = 0x000911A2;
const dcm::Tag kCustomDataValueTag = 0x000911A3;

}  // namespace trophy

int main(int argc, const char* argv[]) {
  if (argc < 2) {
    return 1;
  }

  dcm::DicomFile dicom_file(argv[1]);

  if (!dicom_file.Load()) {
    return 1;
  }

  std::cout << "File loaded." << std::endl;

  auto custom_data = dicom_file.GetSequence(trophy::kCustomDataTag);
  if (custom_data == nullptr) {
    return 1;
  }

  std::cout << "Seq items: " << custom_data->size() << std::endl;

  for (std::size_t i = 0; i < custom_data->size(); ++i) {
    auto& item = custom_data->At(i);

    std::string key = item.data_set->GetString(trophy::kCustomDataKeyTag);

    std::uint32_t length = 0;
    item.data_set->GetUint32(trophy::kCustomDataLengthTag, &length);

    if (key == "MESH_TREE_DESCRIPTION") {
      // NOTE: Can't call GetStrig() since the VR is UN.
      auto element = item.data_set->Get(trophy::kCustomDataValueTag);
      if (element != nullptr) {
        auto& buffer = element->buffer();
        std::string value(buffer.begin(), buffer.end());
        std::cout << value << std::endl;
      }
    }
  }

  return 0;
}
