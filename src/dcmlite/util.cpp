#include "dcmlite/util.h"

#include <iostream>

namespace dcmlite {

TimeIt::TimeIt() {
  start_ = std::chrono::high_resolution_clock::now();
}

TimeIt::~TimeIt() {
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> ms = end - start_;
  std::cout << ms.count() << " ms\n";
}

}  // namespace dcmlite
