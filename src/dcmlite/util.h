#ifndef DCMLITE_UTIL_H_
#define DCMLITE_UTIL_H_
#pragma once

#include <chrono>

// Misc utilities.

namespace dcmlite {

// Usage:
// {
//   TimeIt ti;
//   <You code to measure the time>
// }
class TimeIt {
public:
  TimeIt();
  ~TimeIt();

private:
  std::chrono::system_clock::time_point start_;
};

}  // namespace dcmlite

#endif  // DCMLITE_UTIL_H_
