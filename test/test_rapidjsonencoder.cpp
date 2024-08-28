#include "gtest/gtest.h"
#include "ports/json/JsonConstants.hpp"
#include "ports/json/ValidatorRapidJsonEncoder.hpp"
#include "ports/ports.hpp"

TEST(ValidatorRapidJsonEncoderTest,
     GivenEmptyDataThenJsonIsGeneratedCorrectly) {
  std::string jsonString{"{}"};
  entities::ValidationData data;
  ::port::secondary::json::ValidatorRapidJsonEncoder encoder;
  EXPECT_EQ(encoder.validatorResponseToJson(data).str(), jsonString);
}

TEST(ValidatorRapidJsonEncoderTest,
     EncodeAuthSubscriptionAllAttributesWithOneChange) {
  std::string jsonString(
      "{\"changes\":[{\"operation\":\"POST\",\"resource_path\":\"/subscribers/"
      "123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData\""
      ",\"data\":{"
      "\"authenticationMethod\":\"5G_AKA\",\"encPermanentKey\":"
      "\"AAAAAAAAAAAAAAAA\",\"authenticationManagementField\":"
      "\"AAAA\",\"algorithmId\":\"1\",\"a4KeyInd\":\"11\",\"a4Ind\":\"111\","
      "\"encOpcKey\":\"1A1A1A1A1A1A1A1A\",\"a4KeyV\":\"16\","
      "\"akaAlgorithmInd\":\"2\"}}]}");

  entities::ValidationData data;
  entities::Change change;
  change.operation = "POST";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace("AAAAAAAAAAAAAAAA");
  authSubscriptionStaticData.authenticationManagementField.emplace("AAAA");
  authSubscriptionStaticData.algorithmId.emplace("1");
  authSubscriptionStaticData.a4KeyInd.emplace("11");
  authSubscriptionStaticData.a4Ind.emplace("111");
  authSubscriptionStaticData.encOpcKey.emplace("1A1A1A1A1A1A1A1A");
  authSubscriptionStaticData.a4KeyV.emplace("16");
  authSubscriptionStaticData.akaAlgorithmInd.emplace("2");

  change.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  data.response.changes.push_back(change);
  ::port::secondary::json::ValidatorRapidJsonEncoder encoder;
  EXPECT_EQ(encoder.validatorResponseToJson(data).str(), jsonString);
}

TEST(ValidatorRapidJsonEncoderTest,
     EncodeAuthSubscriptionAllAttributesWithTwoChanges) {
  std::string jsonString(
      "{\"changes\":[{\"operation\":\"POST\",\"resource_path\":\"/subscribers/"
      "123abc/authSubscription/imsi-111111111111111/"
      "authSubscriptionStaticData\""
      ",\"data\":{"
      "\"authenticationMethod\":\"5G_AKA\",\"encPermanentKey\":"
      "\"AAAAAAAAAAAAAAAA\",\"authenticationManagementField\":"
      "\"AAAA\",\"algorithmId\":\"1\",\"a4KeyInd\":\"11\",\"a4Ind\":\"111\","
      "\"encOpcKey\":\"1A1A1A1A1A1A1A1A\",\"a4KeyV\":\"16\","
      "\"akaAlgorithmInd\":\"0\"}},"
      "{\"operation\":\"POST\",\"resource_path\":\"/subscribers/"
      "123abc/authSubscription/imsi-22222222222222/authSubscriptionStaticData\""
      ",\"data\":{"
      "\"authenticationMethod\":\"5G_AKA\",\"encPermanentKey\":"
      "\"BBBBBBBBBBBBBBBB\",\"authenticationManagementField\":"
      "\"BBBB\",\"algorithmId\":\"2\",\"a4KeyInd\":\"22\",\"a4Ind\":\"222\","
      "\"encOpcKey\":\"2B2B2B2B2B2B2B2B\",\"a4KeyV\":\"17\","
      "\"akaAlgorithmInd\":\"2\"}}]}");

  entities::ValidationData data;
  entities::Change change1;

  change1.operation = "POST";
  change1.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-111111111111111/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;
  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace("AAAAAAAAAAAAAAAA");
  authSubscriptionStaticData.authenticationManagementField.emplace("AAAA");
  authSubscriptionStaticData.algorithmId.emplace("1");
  authSubscriptionStaticData.a4KeyInd.emplace("11");
  authSubscriptionStaticData.a4Ind.emplace("111");
  authSubscriptionStaticData.encOpcKey.emplace("1A1A1A1A1A1A1A1A");
  authSubscriptionStaticData.a4KeyV.emplace("16");
  authSubscriptionStaticData.akaAlgorithmInd.emplace("0");

  change1.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);

  entities::Change change2;
  change2.operation = "POST";
  change2.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-22222222222222/"
      "authSubscriptionStaticData";
  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace("BBBBBBBBBBBBBBBB");
  authSubscriptionStaticData.authenticationManagementField.emplace("BBBB");
  authSubscriptionStaticData.algorithmId.emplace("2");
  authSubscriptionStaticData.a4KeyInd.emplace("22");
  authSubscriptionStaticData.a4Ind.emplace("222");
  authSubscriptionStaticData.encOpcKey.emplace("2B2B2B2B2B2B2B2B");
  authSubscriptionStaticData.a4KeyV.emplace("17");
  authSubscriptionStaticData.akaAlgorithmInd.emplace("2");

  change2.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);

  data.response.changes.push_back(change1);
  data.response.changes.push_back(change2);
  ::port::secondary::json::ValidatorRapidJsonEncoder encoder;
  EXPECT_EQ(encoder.validatorResponseToJson(data).str(), jsonString);
}

TEST(ValidatorRapidJsonEncoderTest,
     EncodeAuthSubscriptionOnlyMandatoryAttribute) {
  std::string jsonString(
      "{\"changes\":[{\"operation\":\"POST\",\"resource_path\":\"/subscribers/"
      "123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData\""
      ",\"data\":{"
      "\"authenticationMethod\":\"5G_AKA\"}}]}");

  entities::ValidationData data;
  entities::Change change;
  change.operation = "POST";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  change.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  data.response.changes.push_back(change);
  ::port::secondary::json::ValidatorRapidJsonEncoder encoder;
  EXPECT_EQ(encoder.validatorResponseToJson(data).str(), jsonString);
}

TEST(ValidatorRapidJsonEncoderTest, EncodeOneError) {
  std::string jsonString{
      "{\"errorMessage\":\"Error message "
      "1\",\"errorDetails\":{\"description\":\"Error description 1\"}}"};
  entities::Error err;
  err.errorMessage = "Error message 1";
  err.errorDetails.insert({{"description", "Error description 1"}});
  ::port::secondary::json::ValidatorRapidJsonEncoder encoder;
  EXPECT_EQ(encoder.errorResponseToJson(err).str(), jsonString);
}

TEST(ValidatorRapidJsonEncoderTest, EncodeSeveralErrors) {
  std::string jsonString{
      "{\"errors\":[{\"errorMessage\":\"Error message "
      "1\",\"errorDetails\":{\"description\":\"Error description "
      "1\",\"path\":\"resource1\"}},{\"errorMessage\":\"Error message "
      "2\",\"errorDetails\":{\"description\":\"Error description "
      "2\",\"info\":\"Additional info\",\"path\":\"resource2\"}}]}"};
  entities::ValidationData data;
  entities::Error err1, err2;
  err1.errorMessage = "Error message 1";
  err1.errorDetails.insert(
      {{"path", "resource1"}, {"description", "Error description 1"}});
  err2.errorMessage = "Error message 2";
  err2.errorDetails.insert({{"path", "resource2"},
                            {"description", "Error description 2"},
                            {"info", "Additional info"}});
  data.response.errors.push_back(err1);
  data.response.errors.push_back(err2);
  ::port::secondary::json::ValidatorRapidJsonEncoder encoder;
  EXPECT_EQ(encoder.validatorResponseToJson(data).str(), jsonString);
}

TEST(ValidatorRapidJsonEncoderTest,
     EncodeAuthSubscriptionDeleteOperationByImsi) {
  std::string jsonString(
      "{\"changes\":[{\"operation\":\"DELETE\",\"resource_path\":\"/"
      "subscribers/"
      "123abc/authSubscription/imsi-123456789012345\""
      "}]}");

  entities::ValidationData data;
  entities::Change change;
  change.operation = "DELETE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace("BBB");
  authSubscriptionStaticData.authenticationManagementField.emplace("BBBB");
  authSubscriptionStaticData.algorithmId.emplace("2");
  authSubscriptionStaticData.a4KeyInd.emplace("12");
  authSubscriptionStaticData.a4Ind.emplace("112");
  authSubscriptionStaticData.encOpcKey.emplace("1A1A1A1A1A1A1A1B");

  change.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  data.response.changes.push_back(change);
  ::port::secondary::json::ValidatorRapidJsonEncoder encoder;
  EXPECT_EQ(encoder.validatorResponseToJson(data).str(), jsonString);
}

TEST(ValidatorRapidJsonEncoderTest, EncodeAuthSubscriptionDeleteOperation) {
  std::string jsonString(
      "{\"changes\":[{\"operation\":\"DELETE\",\"resource_path\":\"/"
      "subscribers/"
      "123abc/authSubscription\""
      "}]}");

  entities::ValidationData data;
  entities::Change change;
  change.operation = "DELETE";
  change.resourcePath = "/subscribers/123abc/authSubscription";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace("BBB");
  authSubscriptionStaticData.authenticationManagementField.emplace("BBBB");
  authSubscriptionStaticData.algorithmId.emplace("2");
  authSubscriptionStaticData.a4KeyInd.emplace("12");
  authSubscriptionStaticData.a4Ind.emplace("112");
  authSubscriptionStaticData.encOpcKey.emplace("1A1A1A1A1A1A1A1B");

  change.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  data.response.changes.push_back(change);
  ::port::secondary::json::ValidatorRapidJsonEncoder encoder;
  EXPECT_EQ(encoder.validatorResponseToJson(data).str(), jsonString);
}

rapidjson::Document strToDocument(const std::string& val) {
  rapidjson::Document vsDoc;
  vsDoc.Parse(val.c_str());
  return vsDoc;
}

TEST(ValidatorRapidJsonEncoderTest,
     EncodeAuthSubscriptionStaticDataWithVendorSpecific) {
  std::string jsonString(
      "{\"changes\":[{\"operation\":\"POST\",\"resource_path\":\"/subscribers/"
      "123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData\""
      ",\"data\":{"
      "\"vendorSpecific-001\":1,"
      "\"authenticationMethod\":\"5G_AKA\",\"encPermanentKey\":"
      "\"AAAAAAAAAAAAAAAA\",\"authenticationManagementField\":"
      "\"AAAA\",\"algorithmId\":\"1\",\"a4KeyInd\":\"11\",\"a4Ind\":\"111\","
      "\"encOpcKey\":\"1A1A1A1A1A1A1A1A\",\"a4KeyV\":\"18\","
      "\"akaAlgorithmInd\":\"2\"}}]}");
  entities::ValidationData data;
  entities::Change change;
  change.operation = "POST";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace("AAAAAAAAAAAAAAAA");
  authSubscriptionStaticData.authenticationManagementField.emplace("AAAA");
  authSubscriptionStaticData.algorithmId.emplace("1");
  authSubscriptionStaticData.a4KeyInd.emplace("11");
  authSubscriptionStaticData.a4Ind.emplace("111");
  authSubscriptionStaticData.encOpcKey.emplace("1A1A1A1A1A1A1A1A");
  authSubscriptionStaticData.a4KeyV.emplace("18");
  authSubscriptionStaticData.akaAlgorithmInd.emplace("2");
  authSubscriptionStaticData.vendorSpecific.emplace("vendorSpecific-001",
                                                    strToDocument("1"));

  change.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  data.response.changes.push_back(change);
  ::port::secondary::json::ValidatorRapidJsonEncoder encoder;
  EXPECT_EQ(encoder.validatorResponseToJson(data).str(), jsonString);
}
