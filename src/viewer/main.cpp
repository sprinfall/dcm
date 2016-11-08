#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <string>

#include "dcmlite/data_set.h"
#include "dcmlite/tag.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << argv[0] << " <file path>" << std::endl;
    return 1;
  }

  const char* file_path = argv[1];

  dcmlite::Endian platform_endian = dcmlite::GetPlatformEndian();

  std::cout << "Platform endian: ";
  if (platform_endian == dcmlite::kLittleEndian) {
    std::cout << "Little Endian" << std::endl;
  } else {
    std::cout << "Big Endian" << std::endl;
  }

  dcmlite::DataSet data_set;
  data_set.LoadFile(file_path);

  data_set.Dump();

#if 0
  std::string private_creator_id;
  if (data_set.GetString(dcmlite::Tag(0x0009, 0x0011), &private_creator_id)) {
    std::cout << "Private Creator Identifier: " << private_creator_id << std::endl;
  }

  std::uint32_t drawing_buf_len = 0;
  if (data_set.GetUint32(dcmlite::Tag(0x0009, 0x1109), &drawing_buf_len)) {
    std::cout << "DIS drawing buffer length: " << drawing_buf_len << std::endl;
  }
#endif

  return 0;
}
