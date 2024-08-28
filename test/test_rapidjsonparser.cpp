#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#include "gtest/gtest.h"
#include "ports/json/JsonConstants.hpp"
#include "ports/json/ValidatorRapidJsonParser.hpp"
#include "ports/ports.hpp"

TEST(ValidationDataTest, sortLDAPstring) {
  std::string orderedStr =
      port::secondary::json::ValidatorRapidJsonParser::sortLDAPoctetString(
          "23016745AB89");
  EXPECT_EQ(orderedStr, "0123456789AB");
}

TEST(ValidatorRapidJsonParserTest, ParseAuthSubscriptionAllAttributes) {
  std::string jsonString(
      "{\"changes\":[{\"operation\":\"CREATE\",\"resource_path\":\"/"
      "subscribers/"
      "123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData\""
      ",\"data\":{"
      "\"authenticationMethod\":\"5G_AKA\",\"encPermanentKey\":"
      "\"2200AA34D40C090D6D4C3B7763854AFB\",\"authenticationManagementField\":"
      "\"B9B9\",\"algorithmId\":\"11\",\"a4KeyInd\":\"1\",\"a4Ind\":\"2\","
      "\"encOpcKey\":\"7AF98A06EA86AB8B3377D27AE089A3A4\",\"encTopcKey\":"
      "\"ABCDEF\",\"a4KeyV\":\"15\",\"akaAlgorithmInd\":\"1\"}}]}");

  ::port::secondary::json::ValidatorRapidJsonParser parser(jsonString);

  entities::ValidationData record;
  EXPECT_EQ(parser.getValidationData(record), true);
  EXPECT_EQ(parser.error(), false);

  EXPECT_EQ(record.changes.size(), 1);
  EXPECT_EQ(record.changes[0].operation, "CREATE");
  EXPECT_EQ(record.changes[0].resourcePath,
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");

  EXPECT_EQ(
      record.changes[0].authSubscription.authSubscriptionStaticData.has_value(),
      true);

  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .authenticationMethod,
            JSON_5G_AKA);

  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .encPermanentKey.has_value(),
            true);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .encPermanentKey.value(),
            "2200AA34D40C090D6D4C3B7763854AFB");

  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .authenticationManagementField.has_value(),
            true);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .authenticationManagementField.value(),
            "B9B9");

  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .algorithmId.has_value(),
            true);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .algorithmId.value(),
            "11");

  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .a4KeyInd.has_value(),
            true);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .a4KeyInd.value(),
            "1");

  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .a4Ind.has_value(),
            true);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .a4Ind.value(),
            "2");

  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .encOpcKey.has_value(),
            true);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .encOpcKey.value(),
            "7AF98A06EA86AB8B3377D27AE089A3A4");

  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .encTopcKey.has_value(),
            true);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .encTopcKey.value(),
            "ABCDEF");
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .a4KeyV.value(),
            "15");
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .akaAlgorithmInd.value(),
            "1");
}

TEST(ValidatorRapidJsonParserTest,
     ParseAuthSubscriptionOnlyMandatoryAttributes) {
  std::string jsonString(
      "{\"changes\":[{\"operation\":\"CREATE\",\"resource_path\":\"/"
      "subscribers/"
      "123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData\""
      ",\"data\":{"
      "\"authenticationMethod\":\"5G_AKA\"}}]}");

  ::port::secondary::json::ValidatorRapidJsonParser parser(jsonString);

  entities::ValidationData record;
  EXPECT_EQ(parser.getValidationData(record), true);
  EXPECT_EQ(parser.error(), false);

  EXPECT_EQ(record.changes.size(), 1);
  EXPECT_EQ(record.changes[0].operation, "CREATE");
  EXPECT_EQ(record.changes[0].resourcePath,
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");

  EXPECT_EQ(
      record.changes[0].authSubscription.authSubscriptionStaticData.has_value(),
      true);

  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .authenticationMethod,
            JSON_5G_AKA);

  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .encPermanentKey.has_value(),
            false);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .authenticationManagementField.has_value(),
            false);

  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .algorithmId.has_value(),
            false);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .a4KeyInd.has_value(),
            false);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .a4Ind.has_value(),
            false);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .encOpcKey.has_value(),
            false);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .encTopcKey.has_value(),
            false);
}

TEST(ValidatorRapidJsonParserTest,
     ParseAuthSubscriptionAuthenticationMethodIsNotAString) {
  std::string jsonString(
      "{\"changes\":[{\"operation\":\"CREATE\",\"resource_path\":\"/"
      "subscribers/"
      "123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData\""
      ",\"data\":{"
      "\"authenticationMethod\":1234}}]}");

  ::port::secondary::json::ValidatorRapidJsonParser parser(jsonString);

  entities::ValidationData record;
  EXPECT_EQ(parser.getValidationData(record), true);
  EXPECT_EQ(parser.error(), false);

  EXPECT_EQ(record.changes.size(), 1);
  EXPECT_EQ(record.changes[0].operation, "CREATE");
  EXPECT_EQ(record.changes[0].resourcePath,
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");

  EXPECT_EQ(
      record.changes[0].authSubscription.authSubscriptionStaticData.has_value(),
      true);

  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .encPermanentKey.has_value(),
            false);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .authenticationManagementField.has_value(),
            false);

  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .algorithmId.has_value(),
            false);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .a4KeyInd.has_value(),
            false);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .a4Ind.has_value(),
            false);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .encOpcKey.has_value(),
            false);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .encTopcKey.has_value(),
            false);

  EXPECT_EQ(record.response.errors.size(), 1);
  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "Field:[authenticationMethod] is not string");
}

TEST(ValidatorRapidJsonParserTest,
     ParseAuthSubscriptionSeveralFieldsNotPresentAndOthersAreNotAString) {
  std::string jsonString(
      "{\"changes\":[{\"operation\":\"CREATE\",\"resource_path\":\"/"
      "subscribers/"
      "123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData\""
      ",\"data\":{"
      "\"algorithmId\":1234,\"a4KeyInd\":1234,\"a4Ind\":1234,"
      "\"encOpcKey\":1234,\"encTopcKey\":1234,\"a4KeyV\":15,"
      "\"akaAlgorithmInd\":1}}]}");

  ::port::secondary::json::ValidatorRapidJsonParser parser(jsonString);

  entities::ValidationData record;
  EXPECT_EQ(parser.getValidationData(record), true);
  EXPECT_EQ(parser.error(), false);

  EXPECT_EQ(record.changes.size(), 1);
  EXPECT_EQ(record.changes[0].operation, "CREATE");
  EXPECT_EQ(record.changes[0].resourcePath,
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");

  EXPECT_EQ(
      record.changes[0].authSubscription.authSubscriptionStaticData.has_value(),
      true);

  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .encPermanentKey.has_value(),
            false);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .authenticationManagementField.has_value(),
            false);

  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .algorithmId.has_value(),
            false);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .a4KeyInd.has_value(),
            false);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .a4Ind.has_value(),
            false);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .encOpcKey.has_value(),
            false);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .encTopcKey.has_value(),
            false);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .a4KeyV.has_value(),
            false);
  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .akaAlgorithmInd.has_value(),
            false);

  EXPECT_EQ(record.response.errors.size(), 8);
  EXPECT_EQ(record.response.errors[0].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[0].errorDetails.at("description"),
            "Mandatory field:[authenticationMethod] not found as child "
            "of:[authSubscriptionStaticData]");
  EXPECT_EQ(record.response.errors[1].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[1].errorDetails.at("description"),
            "Field:[algorithmId] is not string");
  EXPECT_EQ(record.response.errors[2].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[2].errorDetails.at("description"),
            "Field:[a4KeyInd] is not string");
  EXPECT_EQ(record.response.errors[3].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[3].errorDetails.at("description"),
            "Field:[a4Ind] is not string");
  EXPECT_EQ(record.response.errors[4].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[4].errorDetails.at("description"),
            "Field:[encOpcKey] is not string");
  EXPECT_EQ(record.response.errors[5].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[5].errorDetails.at("description"),
            "Field:[encTopcKey] is not string");
  EXPECT_EQ(record.response.errors[6].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[6].errorDetails.at("description"),
            "Field:[a4KeyV] is not string");
  EXPECT_EQ(record.response.errors[7].errorMessage, "Constraint Violation");
  EXPECT_EQ(record.response.errors[7].errorDetails.at("description"),
            "Field:[akaAlgorithmInd] is not string");
}

TEST(ValidatorRapidJsonParserTest, ParseAuthSubscriptionAsRelatedResource) {
  std::string jsonString(
      "{\"relatedResources\":{\"/subscribers/"
      "123abc/authSubscription\":{\"imsi-123456789012345\""
      ":{\"authSubscriptionStaticData\":{"
      "\"authenticationMethod\":\"5G_AKA\",\"encPermanentKey\":"
      "\"2200AA34D40C090D6D4C3B7763854AFB\",\"authenticationManagementField\":"
      "\"B9B9\",\"algorithmId\":\"11\",\"a4KeyInd\":\"1\",\"a4Ind\":\"2\","
      "\"encOpcKey\":\"7AF98A06EA86AB8B3377D27AE089A3A4\",\"encTopcKey\":"
      "\"ABCDEF\",\"a4KeyV\":\"10\",\"akaAlgorithmInd\":\"1\"},"
      "\"authSubscriptionDynamicData\":{"
      "\"sqnScheme\":\"GENERAL\",\"sqn\":\"000000000020\",\"lastIndexes\":{"
      "\"ind5gAusf\":25,"
      "\"indMme\":26}}}}}}");

  ::port::secondary::json::ValidatorRapidJsonParser parser(jsonString);

  entities::ValidationData record;
  EXPECT_EQ(parser.getValidationData(record), true);
  EXPECT_EQ(parser.error(), false);

  EXPECT_EQ(record.changes.size(), 0);
  EXPECT_EQ(record.relatedResources.size(), 1);

  entities::auth_subscription_t& authSubscription =
      boost::get<entities::auth_subscription_t>(record.relatedResources.at(
          "/subscribers/123abc/authSubscription/imsi-123456789012345"));

  EXPECT_EQ(authSubscription.authSubscriptionStaticData.has_value(), true);

  EXPECT_EQ(
      authSubscription.authSubscriptionStaticData.value().authenticationMethod,
      JSON_5G_AKA);

  EXPECT_EQ(authSubscription.authSubscriptionStaticData.value()
                .encPermanentKey.has_value(),
            true);
  EXPECT_EQ(authSubscription.authSubscriptionStaticData.value()
                .encPermanentKey.value(),
            "2200AA34D40C090D6D4C3B7763854AFB");

  EXPECT_EQ(authSubscription.authSubscriptionStaticData.value()
                .authenticationManagementField.has_value(),
            true);
  EXPECT_EQ(authSubscription.authSubscriptionStaticData.value()
                .authenticationManagementField.value(),
            "B9B9");

  EXPECT_EQ(authSubscription.authSubscriptionStaticData.value()
                .algorithmId.has_value(),
            true);
  EXPECT_EQ(
      authSubscription.authSubscriptionStaticData.value().algorithmId.value(),
      "11");

  EXPECT_EQ(
      authSubscription.authSubscriptionStaticData.value().a4KeyInd.has_value(),
      true);
  EXPECT_EQ(
      authSubscription.authSubscriptionStaticData.value().a4KeyInd.value(),
      "1");

  EXPECT_EQ(
      authSubscription.authSubscriptionStaticData.value().a4Ind.has_value(),
      true);
  EXPECT_EQ(authSubscription.authSubscriptionStaticData.value().a4Ind.value(),
            "2");

  EXPECT_EQ(
      authSubscription.authSubscriptionStaticData.value().encOpcKey.has_value(),
      true);
  EXPECT_EQ(
      authSubscription.authSubscriptionStaticData.value().encOpcKey.value(),
      "7AF98A06EA86AB8B3377D27AE089A3A4");

  EXPECT_EQ(authSubscription.authSubscriptionStaticData.value()
                .encTopcKey.has_value(),
            true);
  EXPECT_EQ(
      authSubscription.authSubscriptionStaticData.value().encTopcKey.value(),
      "ABCDEF");

  EXPECT_EQ(authSubscription.authSubscriptionStaticData.value().a4KeyV.value(),
            "10");

  EXPECT_EQ(authSubscription.authSubscriptionStaticData.value()
                .akaAlgorithmInd.value(),
            "1");

  EXPECT_EQ(
      authSubscription.authSubscriptionDynamicData.value().sqnScheme.value(),
      "GENERAL");

  EXPECT_EQ(authSubscription.authSubscriptionDynamicData.value().sqn.value(),
            "000000000020");

  EXPECT_EQ(authSubscription.authSubscriptionDynamicData.value()
                .lastIndexesList.value()
                .size(),
            2);

  EXPECT_EQ(authSubscription.authSubscriptionDynamicData.value()
                .lastIndexesList.value()
                .at("ind5gAusf"),
            25);

  EXPECT_EQ(authSubscription.authSubscriptionDynamicData.value()
                .lastIndexesList.value()
                .at("indMme"),
            26);
}

TEST(ValidatorRapidJsonParserTest, ParseProvJournalRelatedResources) {
  std::string jsonString(
      "{\"relatedResources\":{\"/subscribers/2208a/journal/"
      "provJournal\":{\"notifRef\":\"notifRef1\",\"imsi\":\"IMSI1\","
      "\"imsiMask\":\"imsiMask1\",\"imsiExtMask\":\"imsiExtMask1\",\"msisdn\":"
      "\"MSISDN1\",\"msisdnMask\":\"msisdnMask1\",\"msisdnExtMask\":"
      "\"msisdnExtMask1\",\"imsiAux\":\"IMSIAux1\",\"imsiAuxMask\":"
      "\"imsiAuxMask1\",\"imsiAuxExtMask\":\"imsiAuxExtMask1\",\"impi\":"
      "\"IMPI1\",\"impiMask\":\"impiMask1\",\"impiExtMask\":\"impiExtMask1\","
      "\"secImpi\":\"secImpi1\",\"impiAux\":\"IMPIAux1\",\"username\":"
      "\"username1\",\"usernameMask\":\"usernameMask1\",\"usernameExtMask\":"
      "\"usernameExtMask1\"},\"/subscribers/3319b/journal/"
      "provJournal\":{\"imsiChoStatus\":2,\"imsiExpiryDate\":\"expiryDate2\","
      "\"imsiChoExec\":\"exec2\",\"impuChoIds\":[\"impuchoid1\","
      "\"impuchoid2\"],\"mscIdAux\":\"mscIdAux2\",\"notifInfo\":\"notifInfo2\","
      "\"ueFunctionMask\":\"ueFunctionMask2\",\"extIdList\":[\"extId1\","
      "\"extId2\"],\"nai\":\"NAI2\",\"naiMask\":\"naiMask2\",\"naiExtMask\":"
      "\"naiExtMask2\",\"subsIdList\":[{\"id\":\"i3\",\"prefix\":\"p3\"},{\"id"
      "\":\"i4\"}]}}}");

  ::port::secondary::json::ValidatorRapidJsonParser parser(jsonString);
  EXPECT_EQ(parser.error(), false);
  entities::ValidationData record;
  EXPECT_EQ(parser.getValidationData(record), true);

  EXPECT_EQ(record.changes.size(), 0);
  EXPECT_EQ(record.relatedResources.size(), 2);

  entities::ProvJournal& journal1 = boost::get<entities::ProvJournal>(
      record.relatedResources.at("/subscribers/2208a/journal/provJournal"));
  EXPECT_EQ(journal1.notifRef, "notifRef1");
  EXPECT_EQ(journal1.imsi, "IMSI1");
  EXPECT_EQ(journal1.imsiMask, "imsiMask1");
  EXPECT_EQ(journal1.imsiExtMask, "imsiExtMask1");
  EXPECT_EQ(journal1.msisdn, "MSISDN1");
  EXPECT_EQ(journal1.msisdnMask, "msisdnMask1");
  EXPECT_EQ(journal1.msisdnExtMask, "msisdnExtMask1");
  EXPECT_EQ(journal1.imsiAux, "IMSIAux1");
  EXPECT_EQ(journal1.imsiAuxMask, "imsiAuxMask1");
  EXPECT_EQ(journal1.imsiAuxExtMask, "imsiAuxExtMask1");
  EXPECT_EQ(journal1.impi, "IMPI1");
  EXPECT_EQ(journal1.impiMask, "impiMask1");
  EXPECT_EQ(journal1.impiExtMask, "impiExtMask1");
  EXPECT_EQ(journal1.secImpi, "secImpi1");
  EXPECT_EQ(journal1.impiAux, "IMPIAux1");
  EXPECT_EQ(journal1.username, "username1");
  EXPECT_EQ(journal1.usernameMask, "usernameMask1");
  EXPECT_EQ(journal1.usernameExtMask, "usernameExtMask1");

  entities::ProvJournal& journal2 = boost::get<entities::ProvJournal>(
      record.relatedResources.at("/subscribers/3319b/journal/provJournal"));
  EXPECT_EQ(journal2.notifRef, "");
  EXPECT_EQ(journal2.imsiChoStatus, 2);
  EXPECT_EQ(journal2.imsiExpiryDate, "expiryDate2");
  EXPECT_EQ(journal2.imsiChoExec, "exec2");
  EXPECT_EQ(journal2.impuChoIds.size(), 2);
  EXPECT_EQ(journal2.impuChoIds[0], "impuchoid1");
  EXPECT_EQ(journal2.impuChoIds[1], "impuchoid2");
  EXPECT_EQ(journal2.mscIdAux, "mscIdAux2");
  EXPECT_EQ(journal2.notifInfo, "notifInfo2");
  EXPECT_EQ(journal2.ueFunctionMask, "ueFunctionMask2");
  EXPECT_EQ(journal2.extIdList.size(), 2);
  EXPECT_EQ(journal2.extIdList[0], "extId1");
  EXPECT_EQ(journal2.extIdList[1], "extId2");
  EXPECT_EQ(journal2.nai, "NAI2");
  EXPECT_EQ(journal2.naiMask, "naiMask2");
  EXPECT_EQ(journal2.naiExtMask, "naiExtMask2");
  EXPECT_EQ(journal2.subsIdList.size(), 2);
  EXPECT_EQ(journal2.subsIdList[0].id, "i3");
  EXPECT_EQ(journal2.subsIdList[0].prefix, "p3");
  EXPECT_EQ(journal2.subsIdList[1].id, "i4");
  EXPECT_EQ(journal2.subsIdList[1].prefix, "");
}

TEST(ValidatorRapidJsonParserTest,
     ParseAuthSubscriptionLegacyAsRelatedResource) {
  std::string jsonString(
      "{\"relatedResources\":{\"/legacy/"
      "serv=Auth/IMSI=123456789012345\":{\"FSETIND\":0,"
      "\"EKI\":\"1032547698BADCFE\","
      "\"KIND\":11,"
      "\"A4IND\":2,"
      "\"AMFVALUE\":1,"
      "\"EOPC\":\"32\","
      "\"SEQHE\":\"20\","
      "\"VNUMBER\":16,"
      "\"AKAALGIND\":0}}}");

  ::port::secondary::json::ValidatorRapidJsonParser parser(jsonString);

  entities::ValidationData record;
  EXPECT_EQ(parser.getValidationData(record), true);
  EXPECT_EQ(parser.error(), false);

  EXPECT_EQ(record.changes.size(), 0);
  EXPECT_EQ(record.relatedResources.size(), 1);

  entities::auth_subscription_legacy_t& authSubscriptionLegacy =
      boost::get<entities::auth_subscription_legacy_t>(
          record.relatedResources.at("/legacy/serv=Auth/IMSI=123456789012345"));

  EXPECT_EQ(authSubscriptionLegacy.fSetInd.value(), 0);

  EXPECT_EQ(authSubscriptionLegacy.eki.value(), "1032547698BADCFE");

  EXPECT_EQ(authSubscriptionLegacy.kind.value(), 11);

  EXPECT_EQ(authSubscriptionLegacy.a4Ind.value(), 2);

  EXPECT_EQ(authSubscriptionLegacy.amfValue.value(), 1);

  EXPECT_EQ(authSubscriptionLegacy.eopc.value(), "32");

  EXPECT_EQ(authSubscriptionLegacy.seqHe.value(), "2000");

  EXPECT_EQ(authSubscriptionLegacy.vNumber.value(), 16);

  EXPECT_EQ(authSubscriptionLegacy.akaAlgInd.value(), 0);
}

TEST(ValidatorRapidJsonParserTest,
     ParseAuthSubscriptionLegacyWithBase64EncodedAttributesAsRelatedResource) {
  std::string jsonString(
      "{\"relatedResources\":{\"/legacy/"
      "serv=Auth/IMSI=123456789012345\":{\"FSETIND\":0,"
      "\"EKI:\":\"MTAzMjU0NzY5OEJBRENGRQ==\","
      "\"KIND\":11,"
      "\"A4IND\":2,"
      "\"AMFVALUE\":1,"
      "\"EOPC:\":\"MzI=\","
      "\"SEQHE:\":\"MjA=\"}}}");

  ::port::secondary::json::ValidatorRapidJsonParser parser(jsonString);

  entities::ValidationData record;
  EXPECT_EQ(parser.getValidationData(record), true);
  EXPECT_EQ(parser.error(), false);

  EXPECT_EQ(record.changes.size(), 0);
  EXPECT_EQ(record.relatedResources.size(), 1);

  entities::auth_subscription_legacy_t& authSubscriptionLegacy =
      boost::get<entities::auth_subscription_legacy_t>(
          record.relatedResources.at("/legacy/serv=Auth/IMSI=123456789012345"));

  EXPECT_EQ(authSubscriptionLegacy.fSetInd.value(), 0);

  EXPECT_EQ(authSubscriptionLegacy.eki.value(), "1032547698BADCFE");

  EXPECT_EQ(authSubscriptionLegacy.kind.value(), 11);

  EXPECT_EQ(authSubscriptionLegacy.a4Ind.value(), 2);

  EXPECT_EQ(authSubscriptionLegacy.amfValue.value(), 1);

  EXPECT_EQ(authSubscriptionLegacy.eopc.value(), "32");

  EXPECT_EQ(authSubscriptionLegacy.seqHe.value(), "2000");
}

std::string documentToStr(const entities::genericValue_t& val) {
  rapidjson::Document doc;
  doc.CopyFrom(val, doc.GetAllocator());
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  std::ostringstream os;
  if (!doc.IsNull()) {
    doc.Accept(writer);
    os << buffer.GetString();
  } else {
    os << "";
  }
  return os.str();
}

TEST(ValidatorRapidJsonParserTest,
     ParseAuthSubscriptionStaticDataVendorSpecific) {
  std::string jsonString(
      "{\"changes\":[{\"operation\":\"CREATE\",\"resource_path\":\"/"
      "subscribers/"
      "123abc/authSubscription/imsi-123456789012345/"
      "authSubscriptionStaticData\""
      ",\"data\":{"
      "\"authenticationMethod\":\"5G_AKA\",\"vendorSpecific-001\":{\"key1\":"
      "\"A\",\"key2\":true,\"key3\":[2,3]},\"encPermanentKey\":"
      "\"2200AA34D40C090D6D4C3B7763854AFB\",\"authenticationManagementField\":"
      "\"B9B9\",\"algorithmId\":\"11\",\"a4KeyInd\":\"1\",\"vendorSpecific-"
      "002\":\"hello\",\"a4Ind\":\"2\","
      "\"encOpcKey\":\"7AF98A06EA86AB8B3377D27AE089A3A4\",\"encTopcKey\":"
      "\"ABCDEF\"}}]}");

  ::port::secondary::json::ValidatorRapidJsonParser parser(jsonString);

  entities::ValidationData record;
  EXPECT_EQ(parser.getValidationData(record), true);
  EXPECT_EQ(parser.error(), false);

  EXPECT_EQ(record.changes.size(), 1);
  EXPECT_EQ(record.changes[0].operation, "CREATE");
  EXPECT_EQ(record.changes[0].resourcePath,
            "/subscribers/123abc/authSubscription/imsi-123456789012345/"
            "authSubscriptionStaticData");

  EXPECT_EQ(record.changes[0]
                .authSubscription.authSubscriptionStaticData.value()
                .vendorSpecific.size(),
            2);
  EXPECT_EQ(
      documentToStr(record.changes[0]
                        .authSubscription.authSubscriptionStaticData.value()
                        .vendorSpecific.at("vendorSpecific-001")),
      "{\"key1\":\"A\",\"key2\":true,\"key3\":[2,3]}");
  EXPECT_EQ(
      documentToStr(record.changes[0]
                        .authSubscription.authSubscriptionStaticData.value()
                        .vendorSpecific.at("vendorSpecific-002")),
      "\"hello\"");
}
