#ifndef __LOGWRAPPER__
#define __LOGWRAPPER__

#include <string>

namespace anonlog {

std::string anonymizeString(const std::string &str);

std::string anonymizeJson(const std::string &str);

}  // namespace anonlog

#endif  // __LOGWRAPPER__