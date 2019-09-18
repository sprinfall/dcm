#ifndef DCM_SINGLETON_BASE_H_
#define DCM_SINGLETON_BASE_H_

#include <memory>
#include <mutex>

namespace dcm {

// This is a singleton base class.
// The necessary steps when create a new singleton class.
// 1. Inherit from SingletonBase<T>.
// 2. Set default constructor to be private.
// 3. Set SingletonBase<T> class to be a friend class.

// Usage demo:
//
// class XxxSingleton : public SingletonBase<XxxSingleton> {
// public:
//   // Optional initialization if the default construction is not enough.
//   void Init(const std::string& name) {
//     name_ = name;
//   }
//
//   void Print() const {
//     std::cout << name_ << std::endl;
//   }
//
// private:
//   // Only do default initialization in constructor since it's private.
//   XxxSingleton() : age_(0) {
//   }
//
//   friend class SingletonBase<XxxSingleton>;
//
// private:
//   std::string name_;
//   int age_;
// };
//
// int main() {
//   XxxSingleton::Instance()->Init("Name");
//
//   XxxSingleton::Instance()->Print();
//
//   return 0;
// }

template <typename T>
class SingletonBase {
public:
  // NOTE:
  // If you encounter the following issue on Linux:
  //   terminate called after throwing an instance of 'std::system_error'
  //    what():  Unknown error -1
  // Adding -pthread to your compiler flags solves the problem.
  // For CMake users, that would be:
  //   target_link_libraries(<target> pthread)
  // where <target> is your executable or library.
  // Or:
  //   set(THREADS_PREFER_PTHREAD_FLAG ON)
  //   find_package(Threads REQUIRED)
  //   target_link_libraries(<target> "${CMAKE_THREAD_LIBS_INIT}")
  
  static T* Instance() {
    static std::once_flag s_flag;
    std::call_once(s_flag, []() {
      s_instance.reset(new T{});
    });
    return s_instance.get();
  }
  
  // Using Magic Statics with C++11:
  //   static T* Instance() {
  //     static T s_instance;
  //     return &s_instance;
  //   }
  // See: http://www.nuonsoft.com/blog/2017/08/10/implementing-a-thread-safe-singleton-with-c11-using-magic-statics/
  // But for Visual Studio, 2015+ is required.

  // Another implementation:
  //   static T* Instance() {
  //     static std::mutex s_mutex;
  // 
  //     if (!s_instance) {
  //       std::lock_guard<std::mutex> lock{ s_mutex };
  //       if (!s_instance) {
  //         s_instance.reset(new T{});
  //       }
  //     }
  // 
  //     return s_instance.get();
  //   }

protected:
  SingletonBase() = default;
  virtual ~SingletonBase() = default;

  SingletonBase(const SingletonBase&) = delete;
  SingletonBase& operator=(const SingletonBase&) = delete;

private:
  static std::unique_ptr<T> s_instance;
};

template <class T>
std::unique_ptr<T> SingletonBase<T>::s_instance;

}  // namespace dcm

#endif  // DCM_SINGLETON_BASE_H_
