#include "validation.hpp"

#include "ports/ports.hpp"

namespace domain {
namespace validation {

const ::port::secondary::validation_t validateRequest(
    const ::httpinfo::Info &request) {
  auto oaivalidator =
      ::port::secondary::get<::port::secondary::OaiValidatorInterface>();
  return (oaivalidator->validateRequest(request));
}

const ::port::secondary::validation_t validateResponse(
    const ::httpinfo::Info &response) {
  auto oaivalidator =
      ::port::secondary::get<::port::secondary::OaiValidatorInterface>();
  return (oaivalidator->validateResponse(response));
}

}  // namespace validation
}  // namespace domain
