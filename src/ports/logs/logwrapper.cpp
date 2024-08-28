#include "logwrapper.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <filesystem>

#include "formatter.hpp"

namespace anonlog {

static boost::regex userRegExp{"((imsi|IMSI)-[0-9]{5,15})"};

static boost::regex userUrlRegExp{"((imsi|IMSI) *= *[0-9]{5,15})"};

static boost::regex userJsonRegExp{"(\"(imsi|IMSI)\" *: *\"[0-9]{5,15}\")"};

std::string anonymizeString(const std::string &str) {
  static Formatter fmt;
  return ::boost::regex_replace(str, userRegExp, fmt,
                                ::boost::match_default | ::boost::format_all);
}

std::string anonymizeJson(const std::string &str) {
  static JsonFormatter json_fmt;
  static UrlFormatter url_fmt;
  std::string def_filter = anonymizeString(str);
  std::string url_filter =
      ::boost::regex_replace(def_filter, userUrlRegExp, url_fmt,
                             ::boost::match_default | ::boost::format_all);
  return ::boost::regex_replace(url_filter, userJsonRegExp, json_fmt,
                                ::boost::match_default | ::boost::format_all);
}

}  // namespace anonlog