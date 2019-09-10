
#include <chrono>


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


TimeIt::TimeIt() {
  start_ = std::chrono::high_resolution_clock::now();
}

TimeIt::~TimeIt() {
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> ms = end - start_;
  std::cout << ms.count() << " ms\n";
}
