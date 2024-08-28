#ifndef __AUTHENTICATION_PROVISIONING_VALIDATOR_ENTITIES_CONTEXT__
#define __AUTHENTICATION_PROVISIONING_VALIDATOR_ENTITIES_CONTEXT__

#include <string>
#include <unordered_map>

namespace entities {

class Context final {
 public:
  using contextdata_t = std::unordered_multimap<std::string, std::string>;
  Context() = default;
  Context(const Context &) = default;
  ~Context() = default;

  void copyTracingHeaders(const contextdata_t &);
  const contextdata_t &getTracingHeaders() const;

 private:
  bool isTracingHeader(const std::string &) const;
  void update(const std::string &, const std::string &);
  contextdata_t mTracingHeaders;
};

}  // namespace entities

#endif  // __AUTHENTICATION_PROVISIONING_VALIDATOR_ENTITIES_CONTEXT__
