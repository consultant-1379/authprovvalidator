#ifndef __UDM_AUTHENTICATION_PROVISIONING_VALIDATION_DOMAIN_VALIDATION__
#define __UDM_AUTHENTICATION_PROVISIONING_VALIDATION_DOMAIN_VALIDATION__

#include "ports/oaivalidator/OaiValidatorInterface.hpp"

namespace domain {
namespace validation {

class Validation {
 public:
  Validation() = default;
  ~Validation() = default;
};

const ::port::secondary::validation_t validateRequest(const ::httpinfo::Info &);
const ::port::secondary::validation_t validateResponse(
    const ::httpinfo::Info &);

}  // namespace validation
}  // namespace domain

#endif
