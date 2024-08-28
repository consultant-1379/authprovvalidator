#ifndef __AUTHENTICATION_PROVISIONING_VALIDATOR_IFACE_SERVER_HPP_
#define __AUTHENTICATION_PROVISIONING_VALIDATOR_IFACE_SERVER_HPP_
#include <cstdint>
#include <string>

namespace port {
namespace primary {
class IfaceServer {
 public:
  virtual std::uint32_t start(const std::string&) = 0;
  virtual void stop() = 0;
};

}  // namespace primary
}  // namespace port

#endif  // __AUTHENTICATION_PROVISIONING_VALIDATOR_IFACE_SERVER_HPP_
