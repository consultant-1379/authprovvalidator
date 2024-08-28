#ifndef __AUTHENTICATION_PROVISIONING_VALIDATOR_ENV_HANDLER__
#define __AUTHENTICATION_PROVISIONING_VALIDATOR_ENV_HANDLER__

#include <cstdlib>
#include <map>
#include <string>

namespace envHandler {

constexpr auto DEFAULT_VALIDATOR_PORT = "9086";

constexpr auto DEFAULT_SERVICE_ID = "eric-udm-validation";
constexpr auto DEFAULT_SERVICE_NAME = "eric-udm-authprovvalidator";

constexpr auto DEFAULT_NETWORK_FUNCTION = "Provisioning Function";

constexpr auto ENV_VALIDATOR_PORT = "VALIDATORPORT";

constexpr auto DEFAULT_HEALTHPROXY_ENDPOINT = "http://localhost:9070";

constexpr auto ENV_HEALTHPROXY_ENDPOINT = "HEALTHPROXYENDPOINT";

constexpr auto ENV_OAISCHEMA_FILE = "OAISCHEMAFILE";
constexpr auto DEFAULT_OAISCHEMA_FILE = "authprovvalidator.yaml";

constexpr auto ENV_CPU_REQUEST_INFO = "CPUREQUESTINFO";
constexpr auto DEFAULT_CPU_REQUEST_INFO = "/etc/podinfo/main_cpu_request";
constexpr auto ENABLED = "on";
constexpr auto DEFAULT_OVERLOAD_PROTECTION_VALUE = ENABLED;
constexpr auto ENV_OVERLOAD_PROTECTION = "OVERLOADPROTECTION";

std::map<std::string, std::string> defaultValues = {
    {ENV_HEALTHPROXY_ENDPOINT, DEFAULT_HEALTHPROXY_ENDPOINT}};

inline const std::string getValidatorPort() {
  std::string port(DEFAULT_VALIDATOR_PORT);
  const char *pPort = std::getenv(ENV_VALIDATOR_PORT);
  if (nullptr != pPort) {
    port = std::string(pPort);
  }
  return port;
}

inline const std::string getOaiSchemaFile() {
  const char *pOaiSchemaFile = std::getenv(ENV_OAISCHEMA_FILE);
  if (nullptr == pOaiSchemaFile) {
    return std::string(DEFAULT_OAISCHEMA_FILE);
  }
  return std::string(pOaiSchemaFile);
}

static inline const std::string getCPURequest() {
  const char *pAux = std::getenv(ENV_CPU_REQUEST_INFO);
  if (nullptr != pAux) {
    return (std::string(pAux));
  }
  return std::string(DEFAULT_CPU_REQUEST_INFO);
}

static inline const bool isOverloadProtected() {
  std::string overloadEnabled{DEFAULT_OVERLOAD_PROTECTION_VALUE};
  const char *pValue = std::getenv(ENV_OVERLOAD_PROTECTION);
  if (nullptr != pValue) {
    overloadEnabled = pValue;
  }
  return ENABLED == overloadEnabled;
}

}  // namespace envHandler
#endif  // __AUTHENTICATION_PROVISIONING_VALIDATOR_ENV_HANDLER__
