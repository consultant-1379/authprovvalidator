#include <csignal>

#include "cpph2/overload.hpp"
#include "cppmonitor/monitor.hpp"
#include "log/logout.hpp"
#include "ports/oaivalidator/OaiValidator.hpp"
#include "ports/oaivalidator/OaiValidatorInterface.hpp"
#include "ports/ports.hpp"
#include "ports/server/ValidatorHttp2AsyncServer.hpp"
#include "validatorEnvHandler.hpp"

namespace {
void signalHandler(int sig) {
  switch (sig) {
    case SIGTERM: {
      LOG_INFO("Shutting down service. Waiting for pending request");
      ::logout::freeResources();
    }
    default:
      break;
  }
}
}  // namespace

int main(int argc, char *argv[]) {
  std::signal(SIGTERM, signalHandler);
  auto portValidator = envHandler::getValidatorPort();

  // log initialization
  ::logout::setNetworkFunction(envHandler::DEFAULT_NETWORK_FUNCTION);
  ::logout::setServiceId(envHandler::DEFAULT_SERVICE_ID);
  ::logout::setServiceName(envHandler::DEFAULT_SERVICE_NAME);
  ::logout::startLogging();

  // oaivalidator initialization
  auto schemaFilePath = envHandler::getOaiSchemaFile();

  ::port::secondary::registerInterface<::port::secondary::OaiValidatorInterface,
                                       ::port::secondary::OaiValidator>(
      schemaFilePath);

  // cppmonitor initialization
  monitor::initCPU(envHandler::getCPURequest());
  http2::overload::interface::setCPUPercentConsumptionFunction(
      monitor::getPercentCPUConsumption);

  // setting overload protection
  auto overloadProtection = envHandler::isOverloadProtected();
  http2::overload::interface::setOverloadProtection(overloadProtection);

  LOG_INFO("Starting server", "Authentication provisioning validator URI",
           portValidator, "schema", schemaFilePath, "overload",
           overloadProtection ? "on" : "off");

  // cpph2 server start
  ::port::primary::ValidatorHttp2AsyncServer server;
  auto sc = server.start(portValidator);

  return sc;
}
