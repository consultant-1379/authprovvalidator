#include "ValidatorHttp2AsyncServer.hpp"

#include "domain/validation.hpp"
#include "log/logout.hpp"
#include "openapi3/HTTPinfo.hpp"
#include "ports/HTTPcodes.hpp"
#include "ports/json/ValidatorRapidJsonEncoder.hpp"
#include "ports/json/ValidatorRapidJsonParser.hpp"
#include "ports/logs/logwrapper.hpp"
#include "ports/oaivalidator/OaiValidatorInterface.hpp"
#include "ports/ports.hpp"

namespace port {
namespace primary {

constexpr auto READINESS_PROBE_URI = "/healthz";

void handleHttp2RequestHealthy(std::shared_ptr<http2::Stream> stream) {
  stream->end(::port::HTTP_OK, {}, "");
};

static entities::Error composeError(
    const std::string &message,
    const std::initializer_list<std::pair<std::string, std::string>> args) {
  entities::Error err;
  err.errorMessage = message;
  for (auto &i : args) {
    err.errorDetails.insert(i);
  }
  return err;
}

const http2::headers_t toHttp2Headers(const http2::headers_t &headers) {
  http2::headers_t http2Headers;
  for (const auto &[key, value] : headers) {
    auto headName = key;
    std::transform(headName.begin(), headName.end(), headName.begin(),
                   ::tolower);
    http2Headers.emplace(key, value);
  }
  return http2Headers;
}

void setHTTPInfoRequest(const std::shared_ptr<http2::Stream> &stream,
                        ::httpinfo::Info &httpInfo) {
  httpInfo.headers = toHttp2Headers(stream->requestHeaders());
  httpInfo.json = stream->requestBody();
  httpInfo.uri = stream->requestUri().path();
  httpInfo.query = stream->requestUri().query();
  httpInfo.method = stream->method();
}

void setHTTPInfoResponse(std::uint32_t statusCode, const std::string &uri,
                         const std::string &method, const std::string &query,
                         const std::string &bodyResponse,
                         const http2::headers_t &headers,
                         httpinfo::Info &httpInfo) {
  httpInfo.headers = toHttp2Headers(headers);
  httpInfo.json = bodyResponse;
  httpInfo.uri = uri;
  httpInfo.method = method;
  httpInfo.query = query;
  httpInfo.statusCode = statusCode;
}

bool checkInvalidRequest(const entities::Context &ctxResponse,
                         const httpinfo::Info &httpInfo,
                         const std::shared_ptr<http2::Stream> &stream) {
  ::port::secondary::validation_t resultError =
      ::domain::validation::validateRequest(httpInfo);
  port::secondary::json::ValidatorRapidJsonEncoder encoder;

  if (resultError) {
    entities::Error error = composeError(
        "Malformed request", {{"description", resultError->reason}});
    sendResponse(ctxResponse, stream, ::port::HTTP_BAD_REQUEST,
                 encoder.errorResponseToJson(error).str(), false);
    return true;
  }
  return false;
}

bool checkInvalidResponse(const entities::Context &ctxResponse,
                          const httpinfo::Info &httpInfo,
                          const std::shared_ptr<http2::Stream> &stream) {
  ::port::secondary::validation_t resultError =
      ::domain::validation::validateResponse(httpInfo);
  port::secondary::json::ValidatorRapidJsonEncoder encoder;

  if (resultError) {
    entities::Error error = composeError(
        "Malformed response", {{"description", resultError->reason}});
    sendResponse(ctxResponse, stream, ::port::HTTP_INTERNAL_SERVER_ERROR,
                 encoder.errorResponseToJson(error).str(), false);
    return true;
  }
  return false;
}

void sendResponse(const entities::Context &ctxResponse,
                  std::shared_ptr<http2::Stream> stream,
                  const std::uint32_t &status, const std::string &json,
                  const bool validateResponse) {
  http2::headers_t headers = ctxResponse.getTracingHeaders();

  headers.emplace("content-type", "application/json");

  httpinfo::Info httpInfoRes;
  setHTTPInfoResponse(status, stream->requestUri().path(), stream->method(),
                      stream->requestUri().query(), json, headers, httpInfoRes);

  if (validateResponse) {
    if (checkInvalidResponse(ctxResponse, httpInfoRes, stream)) {
      LOG_ERR("Invalid Response. Could not be validated");
      return;
    }
  }
  LOG_DEBUG("filling sucessfull response", "status_code",
            std::to_string(status), "data", ::anonlog::anonymizeJson(json));
  stream->end(status, headers, json);
}

void handleHttp2Request(std::shared_ptr<http2::Stream> stream) {
  httpinfo::Info httpInfo;
  setHTTPInfoRequest(stream, httpInfo);

  entities::Context contextRequest;
  contextRequest.copyTracingHeaders(stream->requestHeaders());

  LOG_DEBUG("Handling validation request", "uri", httpInfo.uri, "method",
            httpInfo.method, "data", ::anonlog::anonymizeJson(httpInfo.json));

  if (checkInvalidRequest(contextRequest, httpInfo, stream)) {
    LOG_ERR("Invalid Request. Could not be validated");
    return;
  }

  ::port::secondary::json::ValidatorRapidJsonParser parser(httpInfo.json);
  ::entities::ValidationData reqData;
  port::secondary::json::ValidatorRapidJsonEncoder encoder;

  if (not parser.getValidationData(reqData)) {
    LOG_ERR("Could not parse json data");

    entities::Error error = composeError(
        "Malformed request", {{"description", parser.errorString()}});
    sendResponse(contextRequest, stream, ::port::HTTP_BAD_REQUEST,
                 encoder.errorResponseToJson(error).str(), true);
    return;
  }

  if (reqData.response.errors.size()) {
    LOG_ERR("Validation errors found on parsing data");
    sendResponse(contextRequest, stream, ::port::HTTP_CONFLICT,
                 encoder.validatorResponseToJson(reqData).str(), true);
    return;
  }

  auto resp = reqData.applyValidationRules();
  auto isValidated = std::get<entities::VALIDATION>(resp);
  auto code = std::get<entities::CODE>(resp);

  if (not isValidated) {
    LOG_ERR("Validation not successful");
    sendResponse(contextRequest, stream, code,
                 encoder.validatorResponseToJson(reqData).str(), true);
    return;
  }

  sendResponse(contextRequest, stream, ::port::HTTP_OK,
               encoder.validatorResponseToJson(reqData).str(), true);
  return;
}

std::uint32_t ValidatorHttp2AsyncServer::start(const std::string &port) {
  server.handle(READINESS_PROBE_URI, handleHttp2RequestHealthy);
  server.handle("/", handleHttp2Request);
  auto startError = server.listenAndServe(port);
  if (startError) {
    LOG_ERR("Unable to start server", "port", port);
    return 1;
  }
  server.join();
  return 0;
}

}  // namespace primary
}  // namespace port
