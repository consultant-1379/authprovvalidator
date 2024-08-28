#ifndef __UDM_AUTHENTICATION_PROVISIONING_VALIDATOR_OAIVALIDATOR_INTERFACE__
#define __UDM_AUTHENTICATION_PROVISIONING_VALIDATOR_OAIVALIDATOR_INTERFACE__

#include <optional>

#include "openapi3/HTTPinfo.hpp"

namespace port::secondary {

using validation_t = std::optional<::httpinfo::Info>;

class OaiValidatorInterface {
 public:
  virtual ~OaiValidatorInterface() = default;
  virtual const validation_t validateRequest(const ::httpinfo::Info &) = 0;
  virtual const validation_t validateResponse(const ::httpinfo::Info &) = 0;
};

}  // namespace port::secondary

#endif  // __UDM_AUTHENTICATION_PROVISIONING_VALIDATOR_OAIVALIDATOR_INTERFACE__
