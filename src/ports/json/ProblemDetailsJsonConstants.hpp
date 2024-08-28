#ifndef __PROBLEM_DETAILS_JSON_CONSTANTS_HH__
#define __PROBLEM_DETAILS_JSON_CONSTANTS_HH__

namespace port {
namespace secondary {
namespace json {

constexpr auto JSON_PROBLEM_DETAILS_TYPE = "type";
constexpr auto JSON_PROBLEM_DETAILS_TITLE = "title";
constexpr auto JSON_PROBLEM_DETAILS_STATUS = "status";
constexpr auto JSON_PROBLEM_DETAILS_INSTANCE = "instance";
constexpr auto JSON_PROBLEM_DETAILS_CAUSE = "cause";
constexpr auto JSON_PROBLEM_DETAILS_INVALID_PARAMS = "invalidParams";
constexpr auto JSON_PROBLEM_DETAILS_INVALID_PARAMS_PARAM = "param";
constexpr auto JSON_PROBLEM_DETAILS_INVALID_PARAMS_REASON = "reason";

}  // namespace json
}  // namespace secondary
}  // namespace port

#endif  // __PROBLEM_DETAILS_JSON_CONSTANTS_HH__
