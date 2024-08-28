#include "Context.hpp"

#include <boost/algorithm/string.hpp>
#include <unordered_set>

namespace entities {

const std::unordered_set<std::string> TRACING_KEYS = {
    "x-request-id", "x-b3-traceid", "x-b3-spanid",      "x-b3-parentspanid",
    "x-b3-sampled", "x-b3-flags",   "x-ot-span-context"};

void Context::update(const std::string& key, const std::string& value) {
  auto it = mTracingHeaders.find(key);
  if (it != mTracingHeaders.end()) {
    it->second = value;
  } else {
    mTracingHeaders.emplace(key, value);
  }
}

const Context::contextdata_t& Context::getTracingHeaders() const {
  return mTracingHeaders;
}

bool Context::isTracingHeader(const std::string& headerKey) const {
  return (TRACING_KEYS.find(boost::algorithm::to_lower_copy(headerKey)) !=
          TRACING_KEYS.end());
}

void Context::copyTracingHeaders(const Context::contextdata_t& headers) {
  for (const auto& [key, value] : headers) {
    if (isTracingHeader(key)) {
      update(key, value);
    }
  }
  return;
}

}  // namespace entities
