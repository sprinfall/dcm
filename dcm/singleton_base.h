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
  static T* Instance() {
    static std::once_flag s_flag;

    std::call_once(s_flag, [&]() {
      s_instance.reset(new T);
    });

    return s_instance.get();
  }

protected:
  SingletonBase() = default;
  virtual ~SingletonBase() = 0 {
  }

  SingletonBase(const SingletonBase&) = delete;
  SingletonBase& operator=(const SingletonBase&) = delete;

private:
  static std::unique_ptr<T> s_instance;
};

template <class T>
std::unique_ptr<T> SingletonBase<T>::s_instance;

}  // namespace dcm

#endif  // DCM_SINGLETON_BASE_H_
