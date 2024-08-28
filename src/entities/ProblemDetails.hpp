#ifndef _PROBLEM_DETAILS_ENTITIES_
#define _PROBLEM_DETAILS_ENTITIES_

#include <cstdint>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace entities {

using problem_details_raw_type_t = std::string;
using problem_details_raw_title_t = std::string;
using problem_details_raw_status_t = std::uint32_t;
using problem_details_raw_instance_t = std::string;
using problem_details_raw_cause_t = std::string;
using problem_details_raw_invalid_param_reason_t = std::string;

using problem_details_type_t = std::shared_ptr<std::string>;
using problem_details_title_t = std::shared_ptr<std::string>;
using problem_details_status_t = std::shared_ptr<std::uint32_t>;
using problem_details_instance_t = std::shared_ptr<std::string>;
using problem_details_cause_t = std::shared_ptr<std::string>;
using problem_details_invalid_param_param_t = std::string;
using problem_details_invalid_param_reason_t = std::shared_ptr<std::string>;

struct invalidParam {
  problem_details_invalid_param_param_t param;
  problem_details_invalid_param_reason_t reason;
};
using problem_details_invalid_param_t = invalidParam;
using problem_details_invalid_params_t =
    std::vector<problem_details_invalid_param_t>;

class ProblemDetails {
 public:
  ProblemDetails() = default;
  ProblemDetails(const problem_details_type_t& type,
                 const problem_details_title_t& title,
                 const problem_details_status_t& status,
                 const problem_details_instance_t& instance,
                 const problem_details_cause_t& cause,
                 const problem_details_invalid_params_t& invalidParams)
      : type{type},
        title{title},
        status{status},
        instance{instance},
        cause{cause},
        invalidParams{invalidParams} {}

  ProblemDetails(const problem_details_status_t& status,
                 const problem_details_title_t& title)
      : type{},
        title{title},
        status{status},
        instance{},
        cause{},
        invalidParams{} {}

  problem_details_type_t getType() const { return type; }
  problem_details_title_t getTitle() const { return title; }
  problem_details_status_t getStatus() const { return status; }
  problem_details_instance_t getInstance() const { return instance; }
  problem_details_cause_t getCause() const { return cause; }
  problem_details_invalid_params_t getInvalidParams() const {
    return invalidParams;
  }

  problem_details_type_t type;
  problem_details_title_t title;
  problem_details_status_t status;
  problem_details_instance_t instance;
  problem_details_cause_t cause;
  problem_details_invalid_params_t invalidParams;
};

}  // namespace entities
#endif  // _PROBLEM_DETAILS_ENTITIES_
