#ifndef __FORMATTER_LOG__
#define __FORMATTER_LOG__

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <string>
#include <vector>

namespace anonlog {

constexpr auto DELIM = "-";
constexpr auto JSON_DELIM = ":";
constexpr auto URL_DELIM = "=";

class Formatter {
 private:
 public:
  template <typename Out>
  Out operator()(boost::smatch const &what, Out out) const {
    std::string orig = what[0].str();
    std::vector<std::string> strs;
    boost::split(strs, orig, boost::is_any_of(DELIM));
    std::string anonymized;
    for (auto it = std::next(strs.begin()); it != strs.end(); ++it) {
      std::replace_if(
          it->begin(), it->end(),
          [idx = 0](auto &) mutable { return idx++ % 2 == 0; }, '*');
      anonymized += DELIM + *it;
    }
    anonymized = strs[0] + anonymized;
    out = std::copy(anonymized.begin(), anonymized.end(), out);
    return out;
  }
  Formatter() = default;
  Formatter(const Formatter &) = default;
  Formatter &operator=(const Formatter &) = delete;
};

class JsonFormatter {
 public:
  template <typename Out>
  Out operator()(boost::smatch const &what, Out out) const {
    std::string orig = what[0].str();
    std::vector<std::string> strs;
    boost::split(strs, orig, boost::is_any_of(JSON_DELIM));
    for (auto it = strs.begin(); it != strs.end(); ++it) {
      while (std::find(it->begin(), it->end(), ' ') != it->end())
        it->erase(std::find(it->begin(), it->end(), ' '));
    }
    std::string anonymized;
    for (auto it = std::next(strs.begin()); it != strs.end(); ++it) {
      while (std::find(it->begin(), it->end(), '\"') != it->end())
        it->erase(std::find(it->begin(), it->end(), '\"'));
      std::replace_if(
          it->begin(), it->end(),
          [idx = 0](auto &) mutable { return idx++ % 2 == 0; }, '*');
      anonymized += std::string(JSON_DELIM) + "\"" + *it + "\"";
    }
    anonymized = strs[0] + anonymized;
    out = std::copy(anonymized.begin(), anonymized.end(), out);
    return out;
  }

  JsonFormatter() = default;
  JsonFormatter(const JsonFormatter &) = default;
  JsonFormatter &operator=(const JsonFormatter &) = delete;
};

class UrlFormatter {
 public:
  template <typename Out>
  Out operator()(boost::smatch const &what, Out out) const {
    std::string orig = what[0].str();
    std::vector<std::string> strs;
    boost::split(strs, orig, boost::is_any_of(URL_DELIM));
    std::string anonymized;
    for (auto it = std::next(strs.begin()); it != strs.end(); ++it) {
      std::replace_if(
          it->begin(), it->end(),
          [idx = 0](auto &) mutable { return idx++ % 2 == 0; }, '*');
      anonymized += URL_DELIM + *it;
    }
    anonymized = strs[0] + anonymized;
    out = std::copy(anonymized.begin(), anonymized.end(), out);
    return out;
  }

  UrlFormatter() = default;
  UrlFormatter(const UrlFormatter &) = default;
  UrlFormatter &operator=(const UrlFormatter &) = delete;
};

}  // namespace anonlog

#endif  //__FORMATTER_LOG__
