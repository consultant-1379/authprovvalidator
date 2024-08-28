#include <cstdlib>
#include <sstream>

#include "gtest/gtest.h"
#include "validatorEnvHandler.hpp"

TEST(validatorEnvHandler, checkCorrectReturnOfEndpoints) {
  EXPECT_EQ(envHandler::getOaiSchemaFile(), envHandler::DEFAULT_OAISCHEMA_FILE);
  EXPECT_EQ(envHandler::getValidatorPort(), envHandler::DEFAULT_VALIDATOR_PORT);
}

TEST(validatorEnvHandler, cpuRequest) {
  EXPECT_EQ(envHandler::getCPURequest(),
            ::envHandler::DEFAULT_CPU_REQUEST_INFO);
}

TEST(validatorEnvHandler, overloadProtectionEnabledCorrectlyReturned) {
  EXPECT_EQ(envHandler::isOverloadProtected(), true);
}
