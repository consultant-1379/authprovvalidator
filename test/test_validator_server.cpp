#include <gtest/gtest.h>

#include "ports/ports.hpp"
#include "ports/server/ValidatorHttp2AsyncServer.hpp"

class ValidatorHttp2ServerTest : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(ValidatorHttp2ServerTest,
       GivenAValidatorHttp2AysncServerWhenCreatedThenNoErrorOccurs) {
  auto server = std::make_unique<port::primary::ValidatorHttp2AsyncServer>();
  EXPECT_NE(nullptr, server);
}

TEST_F(
    ValidatorHttp2ServerTest,
    GivenAValidatorHttp2AsyncServerWithWrongInitiatingInfoWhenStartedErrorTakesPlace) {
  auto server = std::make_unique<port::primary::ValidatorHttp2AsyncServer>();
  EXPECT_NE(nullptr, server);
  EXPECT_EQ(server.get()->start("-1"), 1);
  server.get()->stop();
}
