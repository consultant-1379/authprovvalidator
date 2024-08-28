#ifndef __UDM_AUTHENTICATION_PROVISIONING_VALIDATOR_OAIVALIDATOR__
#define __UDM_AUTHENTICATION_PROVISIONING_VALIDATOR_OAIVALIDATOR__

#include "openapi3/Swagger.hpp"
#include "ports/oaivalidator/OaiValidatorInterface.hpp"

namespace port::secondary {

class OaiValidator final : public OaiValidatorInterface {
 public:
  OaiValidator() = delete;
  OaiValidator(OaiValidator &&) = delete;
  OaiValidator(const OaiValidator &) = delete;
  OaiValidator(const std::string &);
  ~OaiValidator() noexcept = default;

  const ::port::secondary::validation_t validateRequest(
      const ::httpinfo::Info &) override;
  const ::port::secondary::validation_t validateResponse(
      const ::httpinfo::Info &) override;

 private:
  openapi3::Swagger swagger_;
};

}  // namespace port::secondary

#endif  // __UDM_AUTHENTICATION_PROVISIONING_VALIDATOR_OAIVALIDATOR__
