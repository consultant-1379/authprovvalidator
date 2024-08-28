#include "entities/ValidationData.hpp"
#include "gtest/gtest.h"
#include "ports/HTTPcodes.hpp"
#include "ports/json/JsonConstants.hpp"

TEST(ValidationDataTest, HexToIntConversionOk) {
  unsigned int value =
      entities::ValidationData::fromHexStringToUnsignedInt("B9B9");
  EXPECT_EQ(value, 47545);
}

TEST(ValidationDataTest, IntToHexConversionOk) {
  std::string str =
      entities::ValidationData::fromUnsignedIntToHexString(47545, 4);
  EXPECT_EQ(str, "B9B9");
}

TEST(ValidationDataTest, IntToHexConversionLeadingZeroesOk) {
  std::string str = entities::ValidationData::fromUnsignedIntToHexString(21, 4);
  EXPECT_EQ(str, "0015");
}

TEST(ValidationDataTest, fromHexStringToBitset) {
  std::bitset<48> bits =
      entities::ValidationData::fromHexStringToBitset("0123456789AB");
  EXPECT_EQ(bits.to_string(),
            "000000010010001101000101011001111000100110101011");
}

TEST(ValidationDataTest, computeSqnFromSeqHe) {
  entities::seq_he_t seqHe = "0123456789AB";
  entities::sqn_t sqn = entities::ValidationData::computeSqnFromSeqHe(seqHe);
  EXPECT_EQ(sqn, "2468ACF13560");
}

TEST(ValidationDataTest, computeSqnFromSeqHeZeroesAdded) {
  entities::seq_he_t seqHe = "1032547698BA";
  entities::sqn_t sqn = entities::ValidationData::computeSqnFromSeqHe(seqHe);
  EXPECT_EQ(sqn, "064A8ED31740");
}

TEST(ValidationDataTest, computeSqnFromSeqHeAllZeroes) {
  entities::seq_he_t seqHe = "000000000000";
  entities::sqn_t sqn = entities::ValidationData::computeSqnFromSeqHe(seqHe);
  EXPECT_EQ(sqn, "000000000000");
}

TEST(ValidationDataTest, GetMscIdOk) {
  std::string resourcePath{
      "/subscribers/000aaa/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData"};
  entities::ValidationData record;
  std::string mscId = record.getMscId(resourcePath);
  EXPECT_EQ(mscId, "000aaa");
}

TEST(ValidationDataTest, GetMscIdWrong) {
  std::string resourcePath{
      "/subscribers/000aaa/authSubscription/imsi-123456789012345/"
      "authSubscriptionDynamicData"};
  entities::ValidationData record;
  std::string mscId = record.getMscId(resourcePath);
  EXPECT_EQ(mscId, "");
}

TEST(ValidationDataTest, BuildProvJournalResourcePathOk) {
  std::string mscId{"123abc"};
  entities::ValidationData record;
  std::string resourcePath =
      record.buildProvJournalResourcePathFromMscId(mscId);
  EXPECT_EQ(resourcePath, "/subscribers/123abc/journal/provJournal");
}

TEST(ValidationDataTest, CheckBitIsSet) {
  entities::ValidationData record;
  std::string bitMask{"0b0000000000010000"};
  auto result = record.checkBitIsSet(bitMask, 4);
  EXPECT_EQ(result, true);
}

TEST(ValidationDataTest, CheckBitIsNotSet) {
  entities::ValidationData record;
  std::string bitMask{"0b0000000000010000"};
  auto result = record.checkBitIsSet(bitMask, 3);
  EXPECT_EQ(result, false);
}

TEST(ValidationDataTest, CheckBitPositionBiggerThanBitMask) {
  entities::ValidationData record;
  std::string bitMask{"0b1111111111111111"};
  auto result = record.checkBitIsSet(bitMask, 30);
  EXPECT_EQ(result, false);
}

TEST(ValidationDataTest, ValidateAuthSubscriptionWithAllattributes) {
  entities::ValidationData record;

  entities::Change change;
  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("15");
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");
  authSubscriptionStaticData.a4KeyV.emplace("16");
  authSubscriptionStaticData.akaAlgorithmInd.emplace("0");

  change.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  record.changes.push_back(change);

  entities::ProvJournal journal;
  journal.imsiMask = "0b0000000000010000";
  record.relatedResources.insert(
      {"/subscribers/123abc/journal/provJournal", journal});

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), true);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_OK);

  EXPECT_EQ(record.response.changes.size(), 1);
  EXPECT_EQ(record.response.errors.size(), 0);
  EXPECT_EQ(record.response.changes[0].operation, "CREATE");
  EXPECT_EQ(record.response.changes[0].resourcePath,
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(
      record.changes[0].authSubscription.authSubscriptionStaticData.has_value(),
      true);

  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .authenticationMethod,
            JSON_5G_AKA);
  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .encPermanentKey,
            "2200AA34D40C090D6D4C3B7763854AFB");
  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .authenticationManagementField,
            "B9B9");
  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .algorithmId,
            "15");
  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .a4KeyInd,
            "1");
  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .a4Ind,
            "2");
  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .encOpcKey,
            "7AF98A06EA86AB8B3377D27AE089A3A4");

  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionDynamicData.value()
                .sqnScheme,
            "NON_TIME_BASED");

  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionDynamicData.value()
                .sqn,
            "000000000000");

  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .a4KeyV,
            "16");

  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .akaAlgorithmInd,
            "0");
}

TEST(ValidationDataTest,
     ValidateAuthSubscriptionWithOnlyMandatoryAttributeNotValid) {
  entities::ValidationData record;
  entities::Change change;

  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;
  authSubscriptionStaticData.authenticationMethod = "THIS_IS_NOT_VALID";
  change.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  record.changes.push_back(change);

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_CONFLICT);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 1);
  EXPECT_EQ(
      record.response.errors[0].errorDetails.at("description"),
      "\"authenticationMethod\" in \"authSubscriptionStaticData\" has not the "
      "valid format");
}

TEST(ValidationDataTest,
     ValidateAuthSubscriptionWithOnlyMandatoryAttribute5gAka) {
  entities::ValidationData record;
  entities::Change change;

  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;
  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  change.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  record.changes.push_back(change);

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_CONFLICT);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 5);
  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "\"encPermanentKey\" in \"authSubscriptionStaticData\" has not "
            "been defined "
            "with an AKA authentication method");

  EXPECT_EQ(record.response.errors[1].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[1].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[1].errorDetails.at("description"),
            "\"authenticationManagementField\" in "
            "\"authSubscriptionStaticData\" has not "
            "been defined with an AKA authentication method");

  EXPECT_EQ(record.response.errors[2].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[2].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[2].errorDetails.at("description"),
            "\"algorithmId\" in \"authSubscriptionStaticData\" has not been "
            "defined with "
            "an AKA authentication method");

  EXPECT_EQ(record.response.errors[3].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[3].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[3].errorDetails.at("description"),
            "\"a4KeyInd\" in \"authSubscriptionStaticData\" has not been "
            "defined with an "
            "AKA authentication method");

  EXPECT_EQ(record.response.errors[4].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[4].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[4].errorDetails.at("description"),
            "\"a4Ind\" in \"authSubscriptionStaticData\" has not been defined "
            "with an "
            "AKA authentication method");
}

TEST(ValidationDataTest,
     ValidateAuthSubscriptionWithAllPatternsInvalidByLength) {
  entities::ValidationData record;
  entities::Change change;

  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;
  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace(
      "ABCDE");  // length is 4 hex digits
  authSubscriptionStaticData.algorithmId.emplace("11");
  authSubscriptionStaticData.a4KeyInd.emplace("1");

  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");
  change.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  record.changes.push_back(change);

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_CONFLICT);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 1);

  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(
      record.response.errors[0].errorDetails.at("description"),
      "\"authenticationManagementField\" in \"authSubscriptionStaticData\" has "
      "invalid value");
}

TEST(ValidationDataTest,
     ValidateAuthSubscriptionWithAllPatternsInvalidByCharactersAllowed) {
  entities::ValidationData record;
  entities::Change change;

  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "GGGG");  // hex digits by pattern
  authSubscriptionStaticData.authenticationManagementField.emplace(
      "GGGG");  // hex digits by pattern
  authSubscriptionStaticData.algorithmId.emplace(
      "AAAA");  // decimal digits by pattern
  authSubscriptionStaticData.a4KeyInd.emplace(
      "AAAA");  // decimal digits by pattern
  authSubscriptionStaticData.a4Ind.emplace(
      "AAAA");  // decimal digits by pattern
  authSubscriptionStaticData.encOpcKey.emplace(
      "GGGG");  // hex digits by pattern
  authSubscriptionStaticData.encTopcKey.emplace(
      "HHHH");  // hex digits by pattern
  authSubscriptionStaticData.a4KeyV.emplace("A");
  authSubscriptionStaticData.akaAlgorithmInd.emplace("B");

  change.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  record.changes.push_back(change);

  entities::ProvJournal journal;
  journal.imsiMask = "0b0000000000010000";
  record.relatedResources.insert(
      {"/subscribers/123abc/journal/provJournal", journal});

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_CONFLICT);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 8);

  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "\"encPermanentKey\" in \"authSubscriptionStaticData\" has "
            "invalid value");

  EXPECT_EQ(record.response.errors[1].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[1].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(
      record.response.errors[1].errorDetails.at("description"),
      "\"authenticationManagementField\" in \"authSubscriptionStaticData\" has "
      "invalid value");

  EXPECT_EQ(record.response.errors[2].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[2].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(
      record.response.errors[2].errorDetails.at("description"),
      "\"algorithmId\" in \"authSubscriptionStaticData\" has invalid value");

  EXPECT_EQ(record.response.errors[3].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[3].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[3].errorDetails.at("description"),
            "\"a4KeyInd\" in \"authSubscriptionStaticData\" has invalid value");

  EXPECT_EQ(record.response.errors[4].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[4].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[4].errorDetails.at("description"),
            "\"a4Ind\" in \"authSubscriptionStaticData\" has invalid value");

  EXPECT_EQ(record.response.errors[5].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[5].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(
      record.response.errors[5].errorDetails.at("description"),
      "\"encOpcKey\" in \"authSubscriptionStaticData\" has invalid value");

  EXPECT_EQ(record.response.errors[6].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[6].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[6].errorDetails.at("description"),
            "\"a4KeyV\" in \"authSubscriptionStaticData\" has invalid value");

  EXPECT_EQ(record.response.errors[7].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[7].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[7].errorDetails.at("description"),
            "\"akaAlgorithmInd\" in \"authSubscriptionStaticData\" has invalid "
            "value");
}

TEST(ValidationDataTest, ValidateAuthSubscriptionWithAlgorithmIdNotTUAK) {
  entities::ValidationData record;
  entities::Change change;

  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("15");  // not TUAK
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");
  authSubscriptionStaticData.encTopcKey.emplace("9A3A4");

  change.authSubscription.authSubscriptionStaticData =
      authSubscriptionStaticData;
  record.changes.push_back(change);

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), true);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_OK);

  EXPECT_EQ(record.response.changes.size(), 1);
  EXPECT_EQ(record.response.errors.size(), 0);
}

TEST(ValidationDataTest, ValidateAuthSubscriptionWithA4IndNotAnInteger) {
  entities::ValidationData record;
  entities::Change change;

  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("4");
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("AAA");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");

  change.authSubscription.authSubscriptionStaticData =
      authSubscriptionStaticData;
  record.changes.push_back(change);

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_CONFLICT);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 1);
  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "\"a4Ind\" in \"authSubscriptionStaticData\" has invalid value");
}

TEST(ValidationDataTest, ValidateAuthSubscriptionWithA4KeyIndNotAnInteger) {
  entities::ValidationData record;
  entities::Change change;

  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("4");
  authSubscriptionStaticData.a4KeyInd.emplace("AAA");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");

  change.authSubscription.authSubscriptionStaticData =
      authSubscriptionStaticData;
  record.changes.push_back(change);

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_CONFLICT);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 1);
  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "\"a4KeyInd\" in \"authSubscriptionStaticData\" has invalid value");
}

TEST(ValidationDataTest, ValidateAuthSubscriptionWithAlgorithmIdBelowRange) {
  entities::ValidationData record;
  entities::Change change;

  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("-1");
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");

  change.authSubscription.authSubscriptionStaticData =
      authSubscriptionStaticData;
  record.changes.push_back(change);

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_CONFLICT);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 1);
  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(
      record.response.errors[0].errorDetails.at("description"),
      "\"algorithmId\" in \"authSubscriptionStaticData\" has invalid value");
}

TEST(ValidationDataTest, ValidateAuthSubscriptionWithAlgorithmIdAboveRange) {
  entities::ValidationData record;
  entities::Change change;

  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("16");
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");

  change.authSubscription.authSubscriptionStaticData =
      authSubscriptionStaticData;
  record.changes.push_back(change);

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_CONFLICT);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 2);
  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(
      record.response.errors[0].errorDetails.at("description"),
      "\"algorithmId\" in \"authSubscriptionStaticData\" has invalid value");

  EXPECT_EQ(record.response.errors[1].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[1].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[1].errorDetails.at("description"),
            "\"encOpcKey\" in \"authSubscriptionStaticData\" must not be "
            "present as \"algorithmId\" is not MILLENAGE (0,2-15)");
}

TEST(ValidationDataTest, ValidateAuthSubscriptionWithAlgorithmIdNotAnInteger) {
  entities::ValidationData record;
  entities::Change change;

  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("AAA");
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");

  change.authSubscription.authSubscriptionStaticData =
      authSubscriptionStaticData;
  record.changes.push_back(change);

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_CONFLICT);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 1);
  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(
      record.response.errors[0].errorDetails.at("description"),
      "\"algorithmId\" in \"authSubscriptionStaticData\" has invalid value");
}

TEST(ValidationDataTest,
     ValidateAuthSubscriptionWithEncPermanentKeyInvalidLength) {
  entities::ValidationData record;
  entities::Change change;

  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AF");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("15");
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");

  change.authSubscription.authSubscriptionStaticData =
      authSubscriptionStaticData;
  record.changes.push_back(change);

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_CONFLICT);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 1);
  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(
      record.response.errors[0].errorDetails.at("description"),
      "\"encPermanentKey\" in \"authSubscriptionStaticData\" has invalid size");
}

TEST(ValidationDataTest, ValidateAuthSubscriptionWithA4KeyVBelowRange) {
  entities::ValidationData record;
  entities::Change change;

  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("15");
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");
  authSubscriptionStaticData.a4KeyV.emplace("-1");

  change.authSubscription.authSubscriptionStaticData =
      authSubscriptionStaticData;
  record.changes.push_back(change);

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_CONFLICT);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 1);
  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "\"a4KeyV\" in \"authSubscriptionStaticData\" has invalid value");
}

TEST(ValidationDataTest, ValidateAuthSubscriptionWithA4KeyVAboveRange) {
  entities::ValidationData record;
  entities::Change change;

  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("15");
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");
  authSubscriptionStaticData.a4KeyV.emplace("32");

  change.authSubscription.authSubscriptionStaticData =
      authSubscriptionStaticData;
  record.changes.push_back(change);

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_CONFLICT);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 1);
  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "\"a4KeyV\" in \"authSubscriptionStaticData\" has invalid value");
}

TEST(ValidationDataTest,
     ValidateAuthSubscriptionWithAkaAlgorithmIndVBelowRange) {
  entities::ValidationData record;
  entities::Change change;

  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("15");
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");
  authSubscriptionStaticData.a4KeyV.emplace("0");
  authSubscriptionStaticData.akaAlgorithmInd.emplace("-1");

  change.authSubscription.authSubscriptionStaticData =
      authSubscriptionStaticData;
  record.changes.push_back(change);

  entities::ProvJournal journal;
  journal.imsiMask = "0b0000000000010000";
  record.relatedResources.insert(
      {"/subscribers/123abc/journal/provJournal", journal});

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_CONFLICT);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 1);
  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "\"akaAlgorithmInd\" in \"authSubscriptionStaticData\" has invalid "
            "value");
}

TEST(ValidationDataTest,
     ValidateAuthSubscriptionWithAkaAlgorithmIndVAboveRange) {
  entities::ValidationData record;
  entities::Change change;

  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("15");
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");
  authSubscriptionStaticData.a4KeyV.emplace("31");
  authSubscriptionStaticData.akaAlgorithmInd.emplace("3");

  change.authSubscription.authSubscriptionStaticData =
      authSubscriptionStaticData;
  record.changes.push_back(change);

  entities::ProvJournal journal;
  journal.imsiMask = "0b0000000000010000";
  record.relatedResources.insert(
      {"/subscribers/123abc/journal/provJournal", journal});

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_CONFLICT);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 1);
  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "\"akaAlgorithmInd\" in \"authSubscriptionStaticData\" has invalid "
            "value");
}

TEST(ValidationDataTest,
     ValidateAuthSubscriptionWithAkaAlgorithmIndButNoProvJournal) {
  entities::ValidationData record;

  entities::Change change;
  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("15");
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");
  authSubscriptionStaticData.a4KeyV.emplace("16");
  authSubscriptionStaticData.akaAlgorithmInd.emplace("0");

  change.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  record.changes.push_back(change);

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_UNPROCESSABLE_ENTITY);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 1);

  EXPECT_EQ(record.response.errors[0].errorMessage, "Unprocessable entity");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "provJournal for subscriber mscId=123abc not included. Needed to "
            "check if user is defined in AuC when attribute "
            "\"akaAlgorithmInd\" is present");
}

TEST(ValidationDataTest,
     ValidateAuthSubscriptionWithAkaAlgorithmIndButUserIsNotAuC) {
  entities::ValidationData record;

  entities::Change change;
  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("15");
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");
  authSubscriptionStaticData.a4KeyV.emplace("16");
  authSubscriptionStaticData.akaAlgorithmInd.emplace("0");

  change.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  record.changes.push_back(change);

  entities::ProvJournal journal;
  journal.imsiMask = "0b0000000000101000";
  record.relatedResources.insert(
      {"/subscribers/123abc/journal/provJournal", journal});

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_CONFLICT);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 1);

  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "It is not allowed to create or update a subscriber with "
            "\"akaAlgorithmInd\" "
            "in \"authSubscriptionStaticData\" if not defined in AuC");
}

TEST(ValidationDataTest, ValidateAuthSubscriptionUpdateNoRelatedResource) {
  entities::ValidationData record;
  entities::Change change;

  change.operation = "UPDATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("15");  // not TUAK
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");

  change.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  record.changes.push_back(change);

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_UNPROCESSABLE_ENTITY);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 1);
  EXPECT_EQ(record.response.errors[0].errorMessage, "Unprocessable entity");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "There is no associated relatedResource: "
            "/subscribers/123abc/authSubscription/imsi-123456789012345");
}

TEST(ValidationDataTest, ValidateAuthSubscriptionCheckNoModifiableAttributes) {
  entities::ValidationData record;
  entities::Change change;
  entities::auth_subscription_t authSubscription;
  entities::auth_subscription_static_data_t staticDataRelRes;

  staticDataRelRes.authenticationMethod = JSON_EAP_AKA_PRIME;
  staticDataRelRes.encPermanentKey.emplace("2200AA34D40C090D6D4C3B7763854AFA");
  staticDataRelRes.authenticationManagementField.emplace("C8C8");
  staticDataRelRes.algorithmId.emplace("14");
  staticDataRelRes.a4KeyInd.emplace("2");
  staticDataRelRes.a4Ind.emplace("3");
  staticDataRelRes.encOpcKey.emplace("7AF98A06EA86AB8B3377D27AE089A3A5");
  staticDataRelRes.a4KeyV.emplace("15");
  authSubscription.authSubscriptionStaticData.emplace(staticDataRelRes);

  change.operation = "UPDATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("15");
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");
  authSubscriptionStaticData.a4KeyV.emplace("16");

  change.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  record.changes.push_back(change);
  record.relatedResources.insert(
      {"/subscribers/123abc/authSubscription/imsi-123456789012345",
       authSubscription});

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_CONFLICT);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 6);
  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "\"encPermanentKey\" in \"authSubscriptionStaticData\" cannot be "
            "modified");

  EXPECT_EQ(record.response.errors[1].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[1].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(
      record.response.errors[1].errorDetails.at("description"),
      "\"algorithmId\" in \"authSubscriptionStaticData\" cannot be modified");

  EXPECT_EQ(record.response.errors[2].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[2].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(
      record.response.errors[2].errorDetails.at("description"),
      "\"a4KeyInd\" in \"authSubscriptionStaticData\" cannot be modified");

  EXPECT_EQ(record.response.errors[3].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[3].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[3].errorDetails.at("description"),
            "\"a4Ind\" in \"authSubscriptionStaticData\" cannot be modified");

  EXPECT_EQ(record.response.errors[4].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[4].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(
      record.response.errors[4].errorDetails.at("description"),
      "\"encOpcKey\" in \"authSubscriptionStaticData\" cannot be modified");

  EXPECT_EQ(record.response.errors[5].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[5].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[5].errorDetails.at("description"),
            "\"a4KeyV\" in \"authSubscriptionStaticData\" cannot be modified");
}

TEST(ValidationDataTest, ValidateAuthSubscriptionCheckDynamicDataNotCreated) {
  entities::ValidationData record;
  entities::Change change;
  entities::auth_subscription_t authSubscription;
  entities::auth_subscription_static_data_t staticDataRelRes;
  entities::auth_subscription_dynamic_data_t dynamicDataRelRes;

  staticDataRelRes.authenticationMethod = JSON_EAP_AKA_PRIME;
  staticDataRelRes.encPermanentKey.emplace("2200AA34D40C090D6D4C3B7763854AFB");
  staticDataRelRes.authenticationManagementField.emplace("B9B9");
  staticDataRelRes.algorithmId.emplace("15");
  staticDataRelRes.a4KeyInd.emplace("1");
  staticDataRelRes.a4Ind.emplace("2");
  staticDataRelRes.encOpcKey.emplace("7AF98A06EA86AB8B3377D27AE089A3A4");
  authSubscription.authSubscriptionStaticData.emplace(staticDataRelRes);

  dynamicDataRelRes.sqnScheme.emplace("GENERAL");
  authSubscription.authSubscriptionDynamicData.emplace(dynamicDataRelRes);

  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionDynamicData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;
  entities::auth_subscription_dynamic_data_t authSubscriptionDynamicData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("15");
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");

  change.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);

  authSubscriptionDynamicData.sqnScheme.emplace("NON_TIME_BASED");
  authSubscriptionDynamicData.sqn.emplace("001100110011");

  change.authSubscription.authSubscriptionDynamicData.emplace(
      authSubscriptionDynamicData);

  record.changes.push_back(change);
  record.relatedResources.insert(
      {"/subscribers/123abc/authSubscription/imsi-123456789012345",
       authSubscription});

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_CONFLICT);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 1);

  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionDynamicData");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "\"authSubscriptionDynamicData\" can not be created or updated");
}

TEST(ValidationDataTest, ValidateAuthSubscriptionCheckDynamicDataNotUpdated) {
  entities::ValidationData record;
  entities::Change change;
  entities::auth_subscription_t authSubscription;
  entities::auth_subscription_static_data_t staticDataRelRes;
  entities::auth_subscription_dynamic_data_t dynamicDataRelRes;

  staticDataRelRes.authenticationMethod = JSON_EAP_AKA_PRIME;
  staticDataRelRes.encPermanentKey.emplace("2200AA34D40C090D6D4C3B7763854AFB");
  staticDataRelRes.authenticationManagementField.emplace("B9B9");
  staticDataRelRes.algorithmId.emplace("15");
  staticDataRelRes.a4KeyInd.emplace("1");
  staticDataRelRes.a4Ind.emplace("2");
  staticDataRelRes.encOpcKey.emplace("7AF98A06EA86AB8B3377D27AE089A3A4");
  authSubscription.authSubscriptionStaticData.emplace(staticDataRelRes);

  dynamicDataRelRes.sqnScheme.emplace("GENERAL");
  authSubscription.authSubscriptionDynamicData.emplace(dynamicDataRelRes);

  change.operation = "UPDATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionDynamicData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;
  entities::auth_subscription_dynamic_data_t authSubscriptionDynamicData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("15");
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");

  change.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);

  authSubscriptionDynamicData.sqnScheme.emplace("NON_TIME_BASED");
  authSubscriptionDynamicData.sqn.emplace("001100110011");

  change.authSubscription.authSubscriptionDynamicData.emplace(
      authSubscriptionDynamicData);

  record.changes.push_back(change);
  record.relatedResources.insert(
      {"/subscribers/123abc/authSubscription/imsi-123456789012345",
       authSubscription});

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_CONFLICT);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 1);

  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionDynamicData");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "\"authSubscriptionDynamicData\" can not be created or updated");
}

TEST(ValidationDataTest, CheckAuthSubscriptionUriOk) {
  EXPECT_EQ(entities::ValidationData::checkAuthSubscriptionUri(
                "/subscribers/0a23/authSubscription"),
            true);
}

TEST(ValidationDataTest, CheckAuthSubscriptionUriWrong) {
  EXPECT_EQ(entities::ValidationData::checkAuthSubscriptionUri(
                "/subscribers/0a23/authSubscription/imsi-123456789012345"),
            false);
}

TEST(ValidationDataTest, CheckAuthSubscriptionStaticDataUriOk) {
  EXPECT_EQ(entities::ValidationData::checkAuthSubscriptionStaticDataUri(
                "/subscribers/0a23/authSubscription/imsi-123456789012345/"
                "authSubscriptionStaticData"),
            true);
}

TEST(ValidationDataTest, CheckAuthSubscriptionStaticDataUriWrong) {
  EXPECT_EQ(entities::ValidationData::checkAuthSubscriptionStaticDataUri(
                "/subscribers/0a23/authSubscription/imsi-123456789012345"),
            false);
}

TEST(ValidationDataTest, CheckAuthSubscriptionPrivIdUriOk) {
  EXPECT_EQ(entities::ValidationData::checkAuthSubscriptionPrivIdUri(
                "/subscribers/0a23/authSubscription/imsi-123456789012345"),
            true);
}

TEST(ValidationDataTest, CheckAuthSubscriptionPrivIdUriWrong) {
  EXPECT_EQ(entities::ValidationData::checkAuthSubscriptionPrivIdUri(
                "/subscribers/0a23/authSubscription"),
            false);
}

TEST(ValidationDataTest, CheckUriImsiOk) {
  EXPECT_EQ(entities::ValidationData::checkUriImsi(
                "/subscribers/0a23/authSubscription/imsi-123456789012345"),
            true);
}

TEST(ValidationDataTest, CheckUriImsiFullPathOk) {
  EXPECT_EQ(entities::ValidationData::checkUriImsi(
                "/subscribers/0a23/authSubscription/imsi-123456789012345/"
                "authSubscriptionStaticData"),
            true);
}

TEST(ValidationDataTest, CheckUriImsiWrong) {
  EXPECT_EQ(entities::ValidationData::checkUriImsi(
                "/subscribers/0a23/authSubscription/imsi-1234567890123456789"),
            false);
  EXPECT_EQ(entities::ValidationData::checkUriImsi(
                "/subscribers/0a23/authSubscription"),
            false);
}

TEST(ValidationDataTest, GetBasePath) {
  EXPECT_EQ(entities::ValidationData::getBasePath(
                "/subscribers/0a23/authSubscription/imsi-123456789012345/"
                "authSubscriptionStaticData"),
            "/subscribers/0a23/authSubscription/imsi-123456789012345");
}

TEST(ValidationDataTest, CreatePathFromBasePath) {
  EXPECT_EQ(entities::ValidationData::createPathFromBasePath(
                "/subscribers/0a23/authSubscription/imsi-123456789012345/"
                "authSubscriptionStaticData",
                "authSubscriptionDynamicData"),
            "/subscribers/0a23/authSubscription/imsi-123456789012345/"
            "authSubscriptionDynamicData");
}

TEST(ValidationDataTest, CheckAuthSubscriptionLegacyUriOk) {
  EXPECT_EQ(entities::ValidationData::checkAuthSubscriptionLegacyUri(
                "/legacy/serv=Auth/IMSI=123456789012345"),
            true);
}

TEST(ValidationDataTest, CheckAuthSubscriptionLegacyUriWrong) {
  EXPECT_EQ(entities::ValidationData::checkAuthSubscriptionLegacyUri(
                "/legacy/serv=Auth/IMSI=123456789012345/hello"),
            false);
}

TEST(ValidationDataTest, CheckGetImsi) {
  EXPECT_EQ(entities::ValidationData::getImsi(
                "/subscribers/0a23/authSubscription/imsi-123456789012345/"
                "authSubscriptionStaticData"),
            "123456789012345");
}

TEST(ValidationDataTest, ValidateLegacyAuthSubscriptionOk) {
  entities::ValidationData record;

  entities::Change change;
  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;
  entities::auth_subscription_legacy_t authSubscriptionLegacy;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("15");
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");
  authSubscriptionStaticData.akaAlgorithmInd.emplace("0");

  change.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  record.changes.push_back(change);

  entities::ProvJournal journal;
  journal.imsiMask = "0b0000000000010000";
  record.relatedResources.insert(
      {"/subscribers/123abc/journal/provJournal", journal});

  authSubscriptionLegacy.fSetInd.emplace(15);
  authSubscriptionLegacy.eki.emplace("2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionLegacy.kind.emplace(1);
  authSubscriptionLegacy.a4Ind.emplace(2);
  authSubscriptionLegacy.amfValue.emplace(47545);
  authSubscriptionLegacy.eopc.emplace("7AF98A06EA86AB8B3377D27AE089A3A4");
  authSubscriptionLegacy.akaType.emplace(1);
  authSubscriptionLegacy.akaAlgInd.emplace(0);

  record.relatedResources.insert(
      {"/legacy/serv=Auth/IMSI=123456789012345", authSubscriptionLegacy});

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), true);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_OK);

  EXPECT_EQ(record.response.changes.size(), 1);
  EXPECT_EQ(record.response.errors.size(), 0);

  EXPECT_EQ(record.response.changes[0].operation, "CREATE");
  EXPECT_EQ(record.response.changes[0].resourcePath,
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(
      record.changes[0].authSubscription.authSubscriptionStaticData.has_value(),
      true);

  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .authenticationMethod,
            JSON_5G_AKA);
  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .encPermanentKey,
            "2200AA34D40C090D6D4C3B7763854AFB");
  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .authenticationManagementField,
            "B9B9");
  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .algorithmId,
            "15");
  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .a4KeyInd,
            "1");
  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .a4Ind,
            "2");
  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .encOpcKey,
            "7AF98A06EA86AB8B3377D27AE089A3A4");

  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .akaAlgorithmInd,
            "0");

  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionDynamicData.value()
                .sqnScheme,
            "TIME_BASED");
}

TEST(ValidationDataTest, ValidateLegacyAuthSubscriptionWrong) {
  entities::ValidationData record;

  entities::Change change;
  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;
  entities::auth_subscription_legacy_t authSubscriptionLegacy;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("15");
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");
  authSubscriptionStaticData.akaAlgorithmInd.emplace("1");

  change.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  record.changes.push_back(change);

  entities::ProvJournal journal;
  journal.imsiMask = "0b0000000000010000";
  record.relatedResources.insert(
      {"/subscribers/123abc/journal/provJournal", journal});

  authSubscriptionLegacy.fSetInd.emplace(14);
  authSubscriptionLegacy.eki.emplace("2200AA34D40C090D6D4C3B7763854AFC");
  authSubscriptionLegacy.kind.emplace(2);
  authSubscriptionLegacy.a4Ind.emplace(3);
  authSubscriptionLegacy.amfValue.emplace(100);
  authSubscriptionLegacy.eopc.emplace("7AF98A06EA86AB8B3377D27AE089A3A5");
  authSubscriptionLegacy.akaType.emplace(1);
  authSubscriptionLegacy.akaAlgInd.emplace(0);

  record.relatedResources.insert(
      {"/legacy/serv=Auth/IMSI=123456789012345", authSubscriptionLegacy});

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_CONFLICT);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 7);

  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "\"encPermanentKey\" is not equal to \"EKI\" attribute of 4G "
            "legacy subscription");

  EXPECT_EQ(record.response.errors[1].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[1].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[1].errorDetails.at("description"),
            "\"a4KeyInd\" is not equal to \"KIND\" attribute of 4G legacy "
            "subscription");

  EXPECT_EQ(record.response.errors[2].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[2].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[2].errorDetails.at("description"),
            "\"a4Ind\" is not equal to \"A4IND\" attribute of 4G legacy "
            "subscription");

  EXPECT_EQ(record.response.errors[3].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[3].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(
      record.response.errors[3].errorDetails.at("description"),
      "\"algorithmId\" is not equal to \"FSETIND\" attribute of 4G legacy "
      "subscription");

  EXPECT_EQ(record.response.errors[4].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[4].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[4].errorDetails.at("description"),
            "\"authenticationManagementField\" is not equal to \"AMFVALUE\" "
            "attribute of 4G legacy subscription");

  EXPECT_EQ(record.response.errors[5].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[5].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[5].errorDetails.at("description"),
            "\"encOpcKey\" is not equal to \"EOPC\" attribute of 4G legacy "
            "subscription");

  EXPECT_EQ(record.response.errors[6].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[6].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[6].errorDetails.at("description"),
            "\"akaAlgorithmInd\" is not equal to \"AKAALGIND\" attribute of 4G "
            "legacy "
            "subscription");
}

TEST(ValidationDataTest, ValidateLegacyAuthSubscriptionAkaTypeNotDefined) {
  entities::ValidationData record;

  entities::Change change;
  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;
  entities::auth_subscription_legacy_t authSubscriptionLegacy;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("15");
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");

  change.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  record.changes.push_back(change);

  authSubscriptionLegacy.eki.emplace("2200AA34D40C090D6D4C3B7763854AFB");

  record.relatedResources.insert(
      {"/legacy/serv=Auth/IMSI=123456789012345", authSubscriptionLegacy});

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_CONFLICT);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 1);
  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "\"AKATYPE\" for 4G legacy subscription is not defined");
}

TEST(ValidationDataTest, ValidateLegacyAuthSubscriptionAkaTypeInvalidValue) {
  entities::ValidationData record;

  entities::Change change;
  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;
  entities::auth_subscription_legacy_t authSubscriptionLegacy;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;

  change.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  record.changes.push_back(change);

  authSubscriptionLegacy.akaType.emplace(2);

  record.relatedResources.insert(
      {"/legacy/serv=Auth/IMSI=123456789012345", authSubscriptionLegacy});

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_CONFLICT);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 1);
  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "\"AKATYPE\" for 4G legacy subscription has an invalid value");
}

TEST(ValidationDataTest,
     ValidateLegacyAuthSubscriptionFillOptionalAttributesInMutation) {
  entities::ValidationData record;

  entities::Change change;
  change.operation = "CREATE";
  change.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;
  entities::auth_subscription_legacy_t authSubscriptionLegacy;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;

  change.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  record.changes.push_back(change);

  authSubscriptionLegacy.fSetInd.emplace(15);
  authSubscriptionLegacy.eki.emplace("2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionLegacy.kind.emplace(1);
  authSubscriptionLegacy.a4Ind.emplace(2);
  authSubscriptionLegacy.amfValue.emplace(47545);
  authSubscriptionLegacy.eopc.emplace("7AF98A06EA86AB8B3377D27AE089A3A4");
  authSubscriptionLegacy.akaType.emplace(1);
  authSubscriptionLegacy.vNumber.emplace(20);
  authSubscriptionLegacy.akaAlgInd.emplace(0);

  record.relatedResources.insert(
      {"/legacy/serv=Auth/IMSI=123456789012345", authSubscriptionLegacy});

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), true);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_OK);

  EXPECT_EQ(record.response.changes.size(), 1);
  EXPECT_EQ(record.response.errors.size(), 0);

  EXPECT_EQ(record.response.changes[0].operation, "CREATE");
  EXPECT_EQ(record.response.changes[0].resourcePath,
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(
      record.changes[0].authSubscription.authSubscriptionStaticData.has_value(),
      true);

  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .authenticationMethod,
            JSON_5G_AKA);
  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .encPermanentKey.value(),
            "2200AA34D40C090D6D4C3B7763854AFB");
  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .authenticationManagementField.value(),
            "B9B9");
  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .algorithmId.value(),
            "15");
  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .a4KeyInd.value(),
            "1");
  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .a4Ind.value(),
            "2");
  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .encOpcKey.value(),
            "7AF98A06EA86AB8B3377D27AE089A3A4");

  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionDynamicData.value()
                .sqnScheme.value(),
            "TIME_BASED");

  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .a4KeyV.value(),
            "20");

  EXPECT_EQ(record.response.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .akaAlgorithmInd.value(),
            "0");
}

TEST(ValidationDataTest, ValidateAuthSubscriptionFirstChangeIsWrongSecondIsOk) {
  entities::ValidationData record;

  entities::Change change1, change2;

  change1.operation = "UPDATE";
  change1.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012377/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("15");
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");

  record.changes.push_back(change1);

  change2.operation = "CREATE";
  change2.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  change2.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  record.changes.push_back(change2);

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_UNPROCESSABLE_ENTITY);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 1);

  EXPECT_EQ(record.response.errors[0].errorMessage, "Unprocessable entity");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012377/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "There is no associated relatedResource: "
            "/subscribers/123abc/authSubscription/imsi-123456789012377");
}

TEST(ValidationDataTest,
     ValidateAuthSubscriptionFirstChangeIsWrongSecondIsWrong) {
  entities::ValidationData record;

  entities::Change change1, change2;

  change1.operation = "UPDATE";
  change1.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012377/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;
  authSubscriptionStaticData.encPermanentKey.emplace(
      "2200AA34D40C090D6D4C3B7763854AFB");
  authSubscriptionStaticData.authenticationManagementField.emplace("B9B9");
  authSubscriptionStaticData.algorithmId.emplace("15");
  authSubscriptionStaticData.a4KeyInd.emplace("1");
  authSubscriptionStaticData.a4Ind.emplace("2");
  authSubscriptionStaticData.encOpcKey.emplace(
      "7AF98A06EA86AB8B3377D27AE089A3A4");

  record.changes.push_back(change1);

  change2.operation = "UPDATE";
  change2.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  change2.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  record.changes.push_back(change2);

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_UNPROCESSABLE_ENTITY);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 2);

  EXPECT_EQ(record.response.errors[0].errorMessage, "Unprocessable entity");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012377/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "There is no associated relatedResource: "
            "/subscribers/123abc/authSubscription/imsi-123456789012377");

  EXPECT_EQ(record.response.errors[1].errorMessage, "Unprocessable entity");
  EXPECT_EQ(record.response.errors[1].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[1].errorDetails.at("description"),
            "There is no associated relatedResource: "
            "/subscribers/123abc/authSubscription/imsi-123456789012345");
}

TEST(ValidationDataTest,
     ValidateAuthSubscriptionFirstChangeIsWrongSecondValidationIsWrong) {
  entities::ValidationData record;

  entities::Change change1, change2;

  change1.operation = "UPDATE";
  change1.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012377/"
      "authSubscriptionStaticData";

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  authSubscriptionStaticData.authenticationMethod = JSON_5G_AKA;

  record.changes.push_back(change1);

  change2.operation = "CREATE";
  change2.resourcePath =
      "/subscribers/123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData";

  change2.authSubscription.authSubscriptionStaticData.emplace(
      authSubscriptionStaticData);
  record.changes.push_back(change2);

  entities::validation_response_t resp = record.applyValidationRules();

  EXPECT_EQ(std::get<entities::VALIDATION>(resp), false);
  EXPECT_EQ(std::get<entities::CODE>(resp), ::port::HTTP_UNPROCESSABLE_ENTITY);

  EXPECT_EQ(record.response.changes.size(), 0);
  EXPECT_EQ(record.response.errors.size(), 6);

  EXPECT_EQ(record.response.errors[0].errorMessage, "Unprocessable entity");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012377/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "There is no associated relatedResource: "
            "/subscribers/123abc/authSubscription/imsi-123456789012377");

  EXPECT_EQ(record.response.errors[1].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[1].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[1].errorDetails.at("description"),
            "\"encPermanentKey\" in \"authSubscriptionStaticData\" has not "
            "been defined "
            "with an AKA authentication method");

  EXPECT_EQ(record.response.errors[2].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[2].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[2].errorDetails.at("description"),
            "\"authenticationManagementField\" in "
            "\"authSubscriptionStaticData\" has not "
            "been defined with an AKA authentication method");

  EXPECT_EQ(record.response.errors[3].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[3].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[3].errorDetails.at("description"),
            "\"algorithmId\" in \"authSubscriptionStaticData\" has not been "
            "defined with "
            "an AKA authentication method");

  EXPECT_EQ(record.response.errors[4].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[4].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[4].errorDetails.at("description"),
            "\"a4KeyInd\" in \"authSubscriptionStaticData\" has not been "
            "defined with an "
            "AKA authentication method");

  EXPECT_EQ(record.response.errors[5].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[5].errorDetails.at("resource_path"),
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");
  EXPECT_EQ(record.response.errors[5].errorDetails.at("description"),
            "\"a4Ind\" in \"authSubscriptionStaticData\" has not been defined "
            "with an "
            "AKA authentication method");
}
