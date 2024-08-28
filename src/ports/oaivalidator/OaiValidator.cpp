#include "ports/oaivalidator/OaiValidator.hpp"

#include "ports/ports.hpp"

namespace port::secondary {

OaiValidator::OaiValidator(const std::string &schema) {
  swagger_.loadDataFromFile(schema);
}

const ::port::secondary::validation_t OaiValidator::validateRequest(
    const ::httpinfo::Info &request) {
  auto httpResult = swagger_.validateRequest(request);
  if (httpResult) {
    return (httpResult.value());
  }
  return std::nullopt;
}

const ::port::secondary::validation_t OaiValidator::validateResponse(
    const ::httpinfo::Info &response) {
  auto httpResult = swagger_.validateResponse(response);
  if (httpResult) {
    return (httpResult.value());
  }
  return std::nullopt;
}

}  // namespace port::secondary
