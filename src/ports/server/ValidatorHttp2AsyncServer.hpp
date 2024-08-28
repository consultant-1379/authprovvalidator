#ifndef __AUTHENTICATION_PROVISIONING_VALIDATOR_HTTP2_ASYNC_SERVER__
#define __AUTHENTICATION_PROVISIONING_VALIDATOR_HTTP2_ASYNC_SERVER__

#include "IfaceServer.hpp"
#include "cpph2/server.hpp"
#include "cpph2/stream.hpp"
#include "entities/Context.hpp"

namespace port {
namespace primary {

void sendResponse(const entities::Context &, std::shared_ptr<http2::Stream>,
                  const std::uint32_t &, const std::string &, const bool);

class ValidatorHttp2AsyncServer final : public IfaceServer {
 public:
  ValidatorHttp2AsyncServer() = default;
  ValidatorHttp2AsyncServer(ValidatorHttp2AsyncServer &&) = delete;
  ~ValidatorHttp2AsyncServer() = default;
  std::uint32_t start(const std::string &) override;
  inline void stop() override { server.stop(); }

 private:
  http2::Server server;
};

}  // namespace primary
}  // namespace port

#endif  // __AUTHENTICATION_PROVISIONING_VALIDATOR_HTTP2_ASYNC_SERVER__
