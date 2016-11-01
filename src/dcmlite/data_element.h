#ifndef DCMLITE_DATA_ELEMENT_H_
#define DCMLITE_DATA_ELEMENT_H_
#pragma once

#include <vector>
#include "dcmlite/vr.h"

namespace dcmlite {

class DataElement {
public:
  DataElement();

  VR::Type vr_type() const {
    return vr_type_;
  }

private:
  VR::Type vr_type_;

  // Data element value in bytes.
  std::vector<char> buffer_;
};

}  // namespace dcmlite

#endif  // DCMLITE_DATA_ELEMENT_H_
