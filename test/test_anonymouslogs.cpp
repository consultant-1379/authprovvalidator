#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "ports/logs/logwrapper.hpp"

TEST(ImsiAnonPositiveTest, StringAnonimizationTests) {
  /* IMSI - International Moobile Subscriber Identity */
  std::string imsi_min = "imsi-123456789012345";
  std::string imsi_may = "IMSI-123456789012345";

  std::string anon_imsi_min = ::anonlog::anonymizeString(imsi_min);
  std::string anon_imsi_may = ::anonlog::anonymizeString(imsi_may);

  ASSERT_STREQ("imsi-*2*4*6*8*0*2*4*", anon_imsi_min.c_str());
  ASSERT_STREQ("IMSI-*2*4*6*8*0*2*4*", anon_imsi_may.c_str());
}

TEST(ImsiInJsonAnonPositiveTest, StringAnonimizationTests) {
  /* IMSI - International Moobile Subscriber Identity */
  std::string imsi_min = "\"imsi\" : \"123456789012345\"";
  std::string imsi_may = "\"IMSI\" : \"123456789012345\"";

  std::string anon_imsi_min = ::anonlog::anonymizeJson(imsi_min);
  std::string anon_imsi_may = ::anonlog::anonymizeJson(imsi_may);

  ASSERT_STREQ("\"imsi\":\"*2*4*6*8*0*2*4*\"", anon_imsi_min.c_str());
  ASSERT_STREQ("\"IMSI\":\"*2*4*6*8*0*2*4*\"", anon_imsi_may.c_str());
}

TEST(ImsiInUrlAnonPositiveTest, StringAnonimizationTests) {
  /* IMSI - International Moobile Subscriber Identity */
  std::string imsi_may = "/this/is/a/IMSI=123456789012345";
  std::string anon_imsi_may = ::anonlog::anonymizeJson(imsi_may);
  ASSERT_STREQ("/this/is/a/IMSI=*2*4*6*8*0*2*4*", anon_imsi_may.c_str());
}