#pragma once
#include <mutex>
#include <thread>

template <class T>
class Destroyer {
 public:
  Destroyer(T* d = 0);

  ~Destroyer();

  void SetDoomed(T* d);

 private:
  // Prevent users from making copies of a
  // Destroyer to avoid double deletion:
  Destroyer(const Destroyer&) = delete;
  void operator=(const Destroyer&) = delete;

 private:
  T* _doomed;
};

template <class T>
Destroyer<T>::Destroyer(T* d) {
  _doomed = d;
}

template <class T>
Destroyer<T>::~Destroyer() {
  delete _doomed;
}

template <class T>
void Destroyer<T>::SetDoomed(T* d) {
  _doomed = d;
}

/**
 *@brief Singleton template class
 **/
template <class T>
class Singleton {
  friend class Destroyer<T>;

 protected:
  Singleton(){};
  virtual ~Singleton(){};
  virtual bool Init();

  static void Set(T* newInstance);
  static void Unset();

 public:
  /**
   *@brief Return current instance of Singleton or create if not existant
   *@return nullptr of creation failed
   **/
  static T* Get();

 private:
  static T* Instance_;
  static std::mutex Lock_;
  static Destroyer<T> Destroyer_;
};

template <typename T>
Destroyer<T> Singleton<T>::Destroyer_ = Destroyer<T>();

template <typename T>
T* Singleton<T>::Instance_ = nullptr;

template <typename T>
std::mutex Singleton<T>::Lock_ = std::mutex();

template <class T>
inline bool Singleton<T>::Init() {
  return true;
}

template <class T>
void Singleton<T>::Set(T* newInstance) {
  std::unique_lock<std::mutex> Lock(Lock_);
  Instance_ = newInstance;
  Destroyer_.SetDoomed(Instance_);
}

template <class T>
void Singleton<T>::Unset() {
  std::unique_lock<std::mutex> Lock(Lock_);
  if (!Instance_) delete Instance_;
  Instance_ = nullptr;
  Destroyer_.SetDoomed(nullptr);
}

template <typename T>
T* Singleton<T>::Get() {
  if (!Instance_) {
    std::unique_lock<std::mutex> Lock(Lock_);
    if (!Instance_) {
      auto tmp = new T();
      if (!tmp->Init()) return nullptr;
      Instance_ = tmp;
      Destroyer_.SetDoomed(Instance_);
    }
  }
  return Instance_;
}
