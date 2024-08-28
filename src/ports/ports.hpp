#ifndef __UDM_AUTHENTICATION_PROVISIONING_VALIDATOR_PORTS__
#define __UDM_AUTHENTICATION_PROVISIONING_VALIDATOR_PORTS__

#include <cassert>
#include <memory>
#include <type_traits>
#include <unordered_map>

namespace port {
namespace secondary {

template <typename T>
using interface_t = std::shared_ptr<std::decay_t<T>>;

namespace detail {

struct InterfaceContainer {
 private:
  template <typename T>
  static std::unordered_map<const InterfaceContainer*, interface_t<T>> items;

 public:
  InterfaceContainer() = default;
  template <typename T>
  void store(interface_t<T> a) const {
    items<T>[this] = a;
  }

  template <typename T>
  interface_t<T> get() const {
    auto it = items<T>.find(this);
    return std::end(items<T>) == it ? nullptr : it->second;
  }

  template <typename T>
  void remove() const {
    auto it = items<T>.find(this);
    if (std::end(items<T>) != it) {
      items<T>.erase(this);
    }
  }
};

template <typename T>
std::unordered_map<const InterfaceContainer*, interface_t<T>>
    InterfaceContainer::items;

extern InterfaceContainer container;

}  // namespace detail

template <typename Base, typename Derived, typename... Args,
          typename = std::enable_if_t<std::is_base_of<Base, Derived>::value>>
void registerInterface(Args&&... args) {
  detail::container.store<Base>(
      std::make_shared<Derived>(std::forward<Args>(args)...));
}

template <typename T>
interface_t<T> get() {
  auto r = detail::container.get<T>();
  assert(nullptr != r);
  return r;
}

template <typename T>
void remove() {
  detail::container.remove<T>();
}

}  // namespace secondary
}  // namespace port

#endif
