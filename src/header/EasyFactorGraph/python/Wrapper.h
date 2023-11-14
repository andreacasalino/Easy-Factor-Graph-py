#pragma once

#include <memory>

namespace EFG::python {
template <typename T> class Wrapper {
public:
  Wrapper(std::shared_ptr<T> wrapped) : wrapped_{wrapped} {}

  template <typename... Args> Wrapper(Args &&...args) {
    wrapped_ = std::make_shared<T>(std::forward<Args>(args)...);
  }

  static Wrapper make(std::shared_ptr<T> wrapped) {
    Wrapper res;
    res.wrapped_ = wrapped;
    return res;
  }

  auto get() const { return wrapped_; }

protected:
  std::shared_ptr<T> wrapped_;
};
} // namespace EFG::python
