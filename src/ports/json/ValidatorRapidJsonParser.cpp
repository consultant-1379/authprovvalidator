#include "ValidatorRapidJsonParser.hpp"

#include "JsonConstants.hpp"
#include "codec/Codec.hpp"
#include "entities/ValidationData.hpp"
#include "ports/ports.hpp"

namespace port {
namespace secondary {
namespace json {

static const auto VENDOR_SPECIFIC_REGEXP =
    boost::regex{"^vendorSpecific-(.*?)"};

bool ValidatorRapidJsonParser::isVendorSpecific(const std::string& vs) {
  boost::cmatch cm;
  return boost::regex_match(vs.c_str(), cm, VENDOR_SPECIFIC_REGEXP);
}

ValidatorRapidJsonParser::ValidatorRapidJsonParser(
    const std::string& json_string)
    : rJsonDoc{}, rJsonParseError{false}, rJsonParseErrorStr{} {
  if (not(rapidjson::ParseResult) rJsonDoc.Parse(json_string.c_str())) {
    setParsingError("wrong json format");
  }
}

void ValidatorRapidJsonParser::getAuthSubscription(
    const rapidjson::Value& attrData,
    entities::auth_subscription_t& authSubscription,
    entities::ValidationData& data, const std::string& resourcePath) {
  if (not attrData.HasMember(JSON_AUTH_SUBSCRIPTION_STATIC_DATA)) {
    data.addError("Constraint Violation",
                  {{"resource_path", resourcePath},
                   {"description", data.unfoundMandatoryFieldError(
                                       JSON_AUTH_SUBSCRIPTION_STATIC_DATA)}});
    return;
  }

  if (not attrData[JSON_AUTH_SUBSCRIPTION_STATIC_DATA].IsObject()) {
    data.addError("Constraint Violation",
                  {{"resource_path", resourcePath},
                   {"description", data.notObjectFieldError(
                                       JSON_AUTH_SUBSCRIPTION_STATIC_DATA)}});
    return;
  }

  const rapidjson::Value& authSubscriptionStaticDataJson =
      attrData[JSON_AUTH_SUBSCRIPTION_STATIC_DATA];

  entities::auth_subscription_static_data_t authSubscriptionStaticData;

  getAuthSubscriptionStaticData(authSubscriptionStaticDataJson,
                                authSubscriptionStaticData, data, resourcePath);

  authSubscription.authSubscriptionStaticData = authSubscriptionStaticData;

  if (attrData.HasMember(JSON_AUTH_SUBSCRIPTION_DYNAMIC_DATA)) {
    if (not attrData[JSON_AUTH_SUBSCRIPTION_DYNAMIC_DATA].IsObject()) {
      data.addError(
          "Constraint Violation",
          {{"resource_path", resourcePath},
           {"description",
            data.notObjectFieldError(JSON_AUTH_SUBSCRIPTION_DYNAMIC_DATA)}});
    } else {
      const rapidjson::Value& authSubscriptionDynamicDataJson =
          attrData[JSON_AUTH_SUBSCRIPTION_DYNAMIC_DATA];

      entities::auth_subscription_dynamic_data_t authSubscriptionDynamicData;

      getAuthSubscriptionDynamicData(authSubscriptionDynamicDataJson,
                                     authSubscriptionDynamicData, data,
                                     resourcePath);

      authSubscription.authSubscriptionDynamicData =
          authSubscriptionDynamicData;
    }
  }
}

void ValidatorRapidJsonParser::getAuthSubscriptionStaticData(
    const rapidjson::Value& authSubscriptionStaticDataJson,
    entities::auth_subscription_static_data_t& authSubscriptionStaticData,
    entities::ValidationData& data, const std::string& resourcePath) {
  getVendorSpecific(authSubscriptionStaticDataJson,
                    authSubscriptionStaticData.vendorSpecific);

  if (not authSubscriptionStaticDataJson.HasMember(
          JSON_AUTHENTICATION_METHOD)) {
    data.addError("Constraint Violation",
                  {{"resource_path", resourcePath},
                   {"description", data.unfoundMandatoryFieldError(
                                       JSON_AUTHENTICATION_METHOD,
                                       JSON_AUTH_SUBSCRIPTION_STATIC_DATA)}});
  } else {
    if (not authSubscriptionStaticDataJson[JSON_AUTHENTICATION_METHOD]
                .IsString()) {
      data.addError("Constraint Violation",
                    {{"resource_path", resourcePath},
                     {"description",
                      data.notStringFieldError(JSON_AUTHENTICATION_METHOD)}});

    } else {
      authSubscriptionStaticData.authenticationMethod =
          authSubscriptionStaticDataJson[JSON_AUTHENTICATION_METHOD]
              .GetString();
    }
  }

  if (authSubscriptionStaticDataJson.HasMember(JSON_ENC_PERMANENT_KEY)) {
    if (not authSubscriptionStaticDataJson[JSON_ENC_PERMANENT_KEY].IsString()) {
      data.addError(
          "Constraint Violation",
          {{"resource_path", resourcePath},
           {"description", data.notStringFieldError(JSON_ENC_PERMANENT_KEY)}});

    } else {
      authSubscriptionStaticData.encPermanentKey =
          authSubscriptionStaticDataJson[JSON_ENC_PERMANENT_KEY].GetString();
    }
  }

  if (authSubscriptionStaticDataJson.HasMember(
          JSON_AUTHENTICATION_MANAGEMENT_FIELD)) {
    if (not authSubscriptionStaticDataJson[JSON_AUTHENTICATION_MANAGEMENT_FIELD]
                .IsString()) {
      data.addError(
          "Constraint Violation",
          {{"resource_path", resourcePath},
           {"description",
            data.notStringFieldError(JSON_AUTHENTICATION_MANAGEMENT_FIELD)}});

    } else {
      authSubscriptionStaticData.authenticationManagementField =
          authSubscriptionStaticDataJson[JSON_AUTHENTICATION_MANAGEMENT_FIELD]
              .GetString();
    }
  }

  if (authSubscriptionStaticDataJson.HasMember(JSON_ALGORITHM_ID)) {
    if (not authSubscriptionStaticDataJson[JSON_ALGORITHM_ID].IsString()) {
      data.addError(
          "Constraint Violation",
          {{"resource_path", resourcePath},
           {"description", data.notStringFieldError(JSON_ALGORITHM_ID)}});

    } else {
      authSubscriptionStaticData.algorithmId =
          authSubscriptionStaticDataJson[JSON_ALGORITHM_ID].GetString();
    }
  }

  if (authSubscriptionStaticDataJson.HasMember(JSON_A4_KEY_IND)) {
    if (not authSubscriptionStaticDataJson[JSON_A4_KEY_IND].IsString()) {
      data.addError(
          "Constraint Violation",
          {{"resource_path", resourcePath},
           {"description", data.notStringFieldError(JSON_A4_KEY_IND)}});

    } else {
      authSubscriptionStaticData.a4KeyInd =
          authSubscriptionStaticDataJson[JSON_A4_KEY_IND].GetString();
    }
  }

  if (authSubscriptionStaticDataJson.HasMember(JSON_A4_IND)) {
    if (not authSubscriptionStaticDataJson[JSON_A4_IND].IsString()) {
      data.addError("Constraint Violation",
                    {{"resource_path", resourcePath},
                     {"description", data.notStringFieldError(JSON_A4_IND)}});

    } else {
      authSubscriptionStaticData.a4Ind =
          authSubscriptionStaticDataJson[JSON_A4_IND].GetString();
    }
  }

  if (authSubscriptionStaticDataJson.HasMember(JSON_ENC_OPC_KEY)) {
    if (not authSubscriptionStaticDataJson[JSON_ENC_OPC_KEY].IsString()) {
      data.addError(
          "Constraint Violation",
          {{"resource_path", resourcePath},
           {"description", data.notStringFieldError(JSON_ENC_OPC_KEY)}});

    } else {
      authSubscriptionStaticData.encOpcKey =
          authSubscriptionStaticDataJson[JSON_ENC_OPC_KEY].GetString();
    }
  }

  if (authSubscriptionStaticDataJson.HasMember(JSON_ENC_TOPC_KEY)) {
    if (not authSubscriptionStaticDataJson[JSON_ENC_TOPC_KEY].IsString()) {
      data.addError(
          "Constraint Violation",
          {{"resource_path", resourcePath},
           {"description", data.notStringFieldError(JSON_ENC_TOPC_KEY)}});

    } else {
      authSubscriptionStaticData.encTopcKey =
          authSubscriptionStaticDataJson[JSON_ENC_TOPC_KEY].GetString();
    }
  }

  if (authSubscriptionStaticDataJson.HasMember(JSON_A4_KEY_V)) {
    if (not authSubscriptionStaticDataJson[JSON_A4_KEY_V].IsString()) {
      data.addError("Constraint Violation",
                    {{"resource_path", resourcePath},
                     {"description", data.notStringFieldError(JSON_A4_KEY_V)}});

    } else {
      authSubscriptionStaticData.a4KeyV =
          authSubscriptionStaticDataJson[JSON_A4_KEY_V].GetString();
    }
  }

  if (authSubscriptionStaticDataJson.HasMember(JSON_AKA_ALGORITHM_IND)) {
    if (not authSubscriptionStaticDataJson[JSON_AKA_ALGORITHM_IND].IsString()) {
      data.addError(
          "Constraint Violation",
          {{"resource_path", resourcePath},
           {"description", data.notStringFieldError(JSON_AKA_ALGORITHM_IND)}});

    } else {
      authSubscriptionStaticData.akaAlgorithmInd =
          authSubscriptionStaticDataJson[JSON_AKA_ALGORITHM_IND].GetString();
    }
  }
}

void ValidatorRapidJsonParser::getAuthSubscriptionDynamicData(
    const rapidjson::Value& authSubscriptionDynamicDataJson,
    entities::auth_subscription_dynamic_data_t& authSubscriptionDynamicData,
    entities::ValidationData& data, const std::string& resourcePath) {
  if (authSubscriptionDynamicDataJson.HasMember(JSON_SQN_SCHEME)) {
    if (not authSubscriptionDynamicDataJson[JSON_SQN_SCHEME].IsString()) {
      data.addError(
          "Constraint Violation",
          {{"resource_path", resourcePath},
           {"description", data.notStringFieldError(JSON_SQN_SCHEME)}});
    } else {
      authSubscriptionDynamicData.sqnScheme.emplace(
          authSubscriptionDynamicDataJson[JSON_SQN_SCHEME].GetString());
    }
  }

  if (authSubscriptionDynamicDataJson.HasMember(JSON_SQN)) {
    if (not authSubscriptionDynamicDataJson[JSON_SQN].IsString()) {
      data.addError("Constraint Violation",
                    {{"resource_path", resourcePath},
                     {"description", data.notStringFieldError(JSON_SQN)}});
    } else {
      authSubscriptionDynamicData.sqn.emplace(
          authSubscriptionDynamicDataJson[JSON_SQN].GetString());
    }
  }

  if (authSubscriptionDynamicDataJson.HasMember(JSON_LAST_INDEXES_LIST)) {
    if (not authSubscriptionDynamicDataJson[JSON_LAST_INDEXES_LIST]
                .IsObject()) {
      data.addError(
          "Constraint Violation",
          {{"resource_path", resourcePath},
           {"description", data.notObjectFieldError(JSON_LAST_INDEXES_LIST)}});
    } else {
      entities::last_indexes_list_t lastIndexesList;
      for (auto& v : authSubscriptionDynamicDataJson[JSON_LAST_INDEXES_LIST]
                         .GetObject()) {
        if (not v.name.IsString()) {
          data.addError("Constraint Violation",
                        {{"resource_path", resourcePath},
                         {"description",
                          "key not string in \"" +
                              std::string{JSON_LAST_INDEXES_LIST} + "\""}});
          continue;
        }
        if (not v.value.IsInt()) {
          data.addError("Constraint Violation",
                        {{"resource_path", resourcePath},
                         {"description",
                          "value not integer in \"" +
                              std::string{JSON_LAST_INDEXES_LIST} + "\""}});
          continue;
        }
        lastIndexesList.insert({v.name.GetString(), v.value.GetInt()});
      }

      authSubscriptionDynamicData.lastIndexesList.emplace(lastIndexesList);
    }
  }
}

bool ValidatorRapidJsonParser::getChangesToValidate(
    entities::ValidationData& data) {
  if (rJsonDoc.HasMember(JSON_CHANGES)) {
    rapidjson::Value& changesList = rJsonDoc[JSON_CHANGES];

    if (not changesList.IsArray()) {
      setParsingError(std::string{JSON_CHANGES} + " is not a list");
      return false;
    }

    for (rapidjson::Value::ConstValueIterator itr = changesList.Begin();
         itr != changesList.End(); ++itr) {
      const rapidjson::Value& attribute = *itr;

      if (not attribute.IsObject()) {
        setParsingError("change is not an object");
        return false;
      }

      entities::Change change;

      for (rapidjson::Value::ConstMemberIterator itr2 = attribute.MemberBegin();
           itr2 != attribute.MemberEnd(); ++itr2) {
        if (not itr2->name.IsString()) {
          setParsingError("attribute name is not string");
          return false;
        }
        std::string attr = itr2->name.GetString();

        if (attr == JSON_OPERATION) {
          if (not itr2->value.IsString()) {
            setParsingError(std::string{JSON_OPERATION} + " is not string");
            return false;
          }
          change.operation = itr2->value.GetString();
          if (change.operation.empty()) {
            setEmptyFieldError(JSON_OPERATION);
            return false;
          }
          if (change.operation != JSON_OPERATION_CREATE &&
              change.operation != JSON_OPERATION_UPDATE &&
              change.operation != JSON_OPERATION_DELETE) {
            setParsingError(
                "operation value is not allowed. It should be either: CREATE, "
                "UPDATE or DELETE");
            return false;
          }
        } else if (attr == JSON_RESOURCE_PATH) {
          if (not itr2->value.IsString()) {
            setParsingError(std::string{JSON_RESOURCE_PATH} + " is not string");
            return false;
          }
          change.resourcePath = itr2->value.GetString();
          if (change.resourcePath.empty()) {
            setEmptyFieldError(JSON_RESOURCE_PATH);
            return false;
          }
        } else if (attr == JSON_DATA) {
          const rapidjson::Value& attrData = itr2->value;

          if (not attrData.IsObject()) {
            setParsingError(std::string{JSON_DATA} + " is not an object");
            return false;
          }

          if (entities::ValidationData::checkAuthSubscriptionUri(
                  change.resourcePath)) {
            getAuthSubscription(attrData, change.authSubscription, data,
                                change.resourcePath);
          } else if (entities::ValidationData::
                         checkAuthSubscriptionStaticDataUri(
                             change.resourcePath)) {
            entities::auth_subscription_static_data_t staticData;
            getAuthSubscriptionStaticData(attrData, staticData, data,
                                          change.resourcePath);
            change.authSubscription.authSubscriptionStaticData = staticData;
          }
        }
      }

      data.changes.push_back(change);
    }
  }

  return true;
}

void ValidatorRapidJsonParser::getRelatedResources(
    entities::ValidationData& data) {
  if (rJsonDoc.HasMember(JSON_RELATED_RESOURCES)) {
    rapidjson::Value& resourcesList = rJsonDoc[JSON_RELATED_RESOURCES];

    for (auto it = resourcesList.MemberBegin(); it != resourcesList.MemberEnd();
         ++it) {
      std::string resourcePath = it->name.GetString();

      if (entities::ValidationData::checkAuthSubscriptionUri(resourcePath)) {
        rapidjson::Value& ids = it->value;

        if (ids.HasMember(JSON_AUTH_SUBSCRIPTION_STATIC_DATA)) {
          entities::auth_subscription_t authSubscription;

          getAuthSubscription(ids, authSubscription, data, resourcePath);

          data.relatedResources.insert({resourcePath, authSubscription});
        } else {
          for (auto it2 = ids.MemberBegin(); it2 != ids.MemberEnd(); ++it2) {
            std::string fullResourcePath =
                entities::ValidationData::addSuffixToPath(
                    resourcePath, it2->name.GetString());

            entities::auth_subscription_t authSubscription;

            getAuthSubscription(it2->value, authSubscription, data,
                                fullResourcePath);

            data.relatedResources.insert({fullResourcePath, authSubscription});
          }
        }
      } else if (resourcePath.ends_with(JSON_PROV_JOURNAL)) {
        entities::ProvJournal provJournal;

        getProvJournal(it->value, provJournal);

        data.relatedResources.insert({resourcePath, provJournal});
      } else if (entities::ValidationData::checkAuthSubscriptionLegacyUri(
                     resourcePath)) {
        entities::auth_subscription_legacy_t authSubscriptionLegacy;

        getAuthSubscriptionLegacy(it->value, authSubscriptionLegacy);

        data.relatedResources.insert({resourcePath, authSubscriptionLegacy});
      }
    }
  }
}

void ValidatorRapidJsonParser::getProvJournal(
    const rapidjson::Value& attrData, entities::ProvJournal& provJournal) {
  provJournal.notifRef = getString(attrData, JSON_NOTIF_REF);

  provJournal.imsi = getString(attrData, JSON_IMSI_TYPE);

  provJournal.imsiMask = getString(attrData, JSON_IMSI_MASK);

  provJournal.imsiExtMask = getString(attrData, JSON_IMSI_EXT_MASK);

  provJournal.msisdn = getString(attrData, JSON_MSISDN);

  provJournal.msisdnMask = getString(attrData, JSON_MSISDN_MASK);

  provJournal.msisdnExtMask = getString(attrData, JSON_MSISDN_EXT_MASK);

  provJournal.imsiAux = getString(attrData, JSON_IMSI_AUX);

  provJournal.imsiAuxMask = getString(attrData, JSON_IMSI_AUX_MASK);

  provJournal.imsiAuxExtMask = getString(attrData, JSON_IMSI_AUX_EXT_MASK);

  provJournal.impi = getString(attrData, JSON_IMPI);

  provJournal.impiMask = getString(attrData, JSON_IMPI_MASK);

  provJournal.impiExtMask = getString(attrData, JSON_IMPI_EXT_MASK);

  provJournal.secImpi = getString(attrData, JSON_SEC_IMPI);

  provJournal.impiAux = getString(attrData, JSON_IMPI_AUX);

  provJournal.username = getString(attrData, JSON_USERNAME);

  provJournal.usernameMask = getString(attrData, JSON_USERNAME_MASK);

  provJournal.usernameExtMask = getString(attrData, JSON_USERNAME_EXT_MASK);

  if (attrData.HasMember(JSON_IMSICHO_STATUS)) {
    if (attrData[JSON_IMSICHO_STATUS].IsInt()) {
      provJournal.imsiChoStatus = attrData[JSON_IMSICHO_STATUS].GetInt();
    }
  }

  provJournal.imsiExpiryDate = getString(attrData, JSON_IMSI_EXPIRY_DATE);

  provJournal.imsiChoExec = getString(attrData, JSON_IMSICHO_EXEC);

  if (attrData.HasMember(JSON_IMPUCHO_IDS)) {
    if (attrData[JSON_IMPUCHO_IDS].IsArray()) {
      for (auto& v : attrData[JSON_IMPUCHO_IDS].GetArray()) {
        if (v.IsString()) {
          provJournal.impuChoIds.push_back(v.GetString());
        }
      }
    }
  }

  provJournal.mscIdAux = getString(attrData, JSON_MSC_ID_AUX);

  provJournal.notifInfo = getString(attrData, JSON_NOTIF_INFO);

  provJournal.ueFunctionMask = getString(attrData, JSON_UE_FUNCTION_MASK);

  if (attrData.HasMember(JSON_EXT_ID_LIST)) {
    if (attrData[JSON_EXT_ID_LIST].IsArray()) {
      for (auto& v : attrData[JSON_EXT_ID_LIST].GetArray()) {
        if (v.IsString()) {
          provJournal.extIdList.push_back(v.GetString());
        }
      }
    }
  }

  provJournal.nai = getString(attrData, JSON_NAI);

  provJournal.naiMask = getString(attrData, JSON_NAI_MASK);

  provJournal.naiExtMask = getString(attrData, JSON_NAI_EXT_MASK);

  if (attrData.HasMember(JSON_SUBS_ID_LIST)) {
    if (attrData[JSON_SUBS_ID_LIST].IsArray()) {
      for (auto& v : attrData[JSON_SUBS_ID_LIST].GetArray()) {
        if (v.IsObject()) {
          entities::SubscriberIdentitiesId subsId;
          rapidjson::Value::ConstMemberIterator itr =
              v.FindMember(JSON_SUBS_ID);
          if (itr != v.MemberEnd()) {
            subsId.id = itr->value.GetString();
          }
          itr = v.FindMember(JSON_SUBS_PREFIX);
          if (itr != v.MemberEnd()) {
            subsId.prefix = itr->value.GetString();
          }
          provJournal.subsIdList.push_back(subsId);
        }
      }
    }
  }
}

void ValidatorRapidJsonParser::getAuthSubscriptionLegacy(
    const rapidjson::Value& attrData,
    entities::auth_subscription_legacy_t& authSubscriptionLegacy) {
  if (attrData.HasMember(JSON_F_SET_IND)) {
    if (attrData[JSON_F_SET_IND].IsInt()) {
      authSubscriptionLegacy.fSetInd.emplace(attrData[JSON_F_SET_IND].GetInt());
    }
  }

  auto eki = getString(attrData, JSON_EKI);
  if (eki.empty()) {
    eki = getString(attrData, JSON_EKI_BASE64, false, true);
  }
  if (not eki.empty()) {
    authSubscriptionLegacy.eki.emplace(eki);
  }

  if (attrData.HasMember(JSON_KIND)) {
    if (attrData[JSON_KIND].IsInt()) {
      authSubscriptionLegacy.kind.emplace(attrData[JSON_KIND].GetInt());
    }
  }

  if (attrData.HasMember(JSON_A4_IND_LEGACY)) {
    if (attrData[JSON_A4_IND_LEGACY].IsInt()) {
      authSubscriptionLegacy.a4Ind.emplace(
          attrData[JSON_A4_IND_LEGACY].GetInt());
    }
  }

  if (attrData.HasMember(JSON_AMF_VALUE)) {
    if (attrData[JSON_AMF_VALUE].IsInt()) {
      authSubscriptionLegacy.amfValue.emplace(
          attrData[JSON_AMF_VALUE].GetInt());
    }
  }

  auto eopc = getString(attrData, JSON_EOPC);
  if (eopc.empty()) {
    eopc = getString(attrData, JSON_EOPC_BASE64, false, true);
  }
  if (not eopc.empty()) {
    authSubscriptionLegacy.eopc.emplace(eopc);
  }

  auto seqHe = getString(attrData, JSON_SEQ_HE, true);
  if (seqHe.empty()) {
    seqHe = getString(attrData, JSON_SEQ_HE_BASE64, true, true);
  }
  if (not seqHe.empty()) {
    authSubscriptionLegacy.seqHe.emplace(seqHe);
  }

  if (attrData.HasMember(JSON_AKA_TYPE)) {
    if (attrData[JSON_AKA_TYPE].IsInt()) {
      authSubscriptionLegacy.akaType.emplace(attrData[JSON_AKA_TYPE].GetInt());
    }
  }

  if (attrData.HasMember(JSON_VNUMBER)) {
    if (attrData[JSON_VNUMBER].IsInt()) {
      authSubscriptionLegacy.vNumber.emplace(attrData[JSON_VNUMBER].GetInt());
    }
  }

  if (attrData.HasMember(JSON_AKA_ALG_IND)) {
    if (attrData[JSON_VNUMBER].IsInt()) {
      authSubscriptionLegacy.akaAlgInd.emplace(
          attrData[JSON_AKA_ALG_IND].GetInt());
    }
  }
}

bool ValidatorRapidJsonParser::getValidationData(
    entities::ValidationData& data) {
  if (error()) {
    return false;
  }

  if (not getChangesToValidate(data)) {
    return false;
  }

  getRelatedResources(data);

  return true;
}

std::string ValidatorRapidJsonParser::getString(
    const rapidjson::Value& attrData, const char* key,
    const bool& reverseOrderLDAP, const bool& isBase64Encoded) {
  if (attrData.HasMember(key)) {
    if (attrData[key].IsString()) {
      std::string readenStr = attrData[key].GetString();

      std::string decodedStr = readenStr;
      // Decode it if is encoded in base64
      if (isBase64Encoded) {
        decodedStr = ::codec::decodeFromBase64(readenStr);
      }

      if (not reverseOrderLDAP) {
        return decodedStr;
      }

      // In case is encoded as an octetstring in LDAP, which means the octets
      // are ordered in the following way:
      // octet1 octet2 ... octetN-1 octetN
      // octet1 has most significative chars
      // octetN has least significative chars
      // The characters are circular shifted two times in groups of four
      // Example:
      // original string: 01 23 45 67 89 AB
      // LDAP string:     23 01 67 56 AB 89
      return sortLDAPoctetString(decodedStr);
    }
  }

  return {};
}

std::string ValidatorRapidJsonParser::sortLDAPoctetString(
    const std::string& str) {
  std::string ordered, dest;
  char cPrev0, cPrev1, cPrev2;
  int i = 0, L = str.length();

  // If length is not multiple of four, fill in with zeroes to the left
  dest = str;

  if (L % 4 != 0) {
    dest = std::string(4 - (L % 4), '0').append(str);
  }

  for (const char& c : dest) {
    switch (i % 4) {
      case 0:
        cPrev0 = c;
        break;
      case 1:
        cPrev1 = c;
        break;
      case 2:
        cPrev2 = c;
        break;
      case 3:
        ordered.push_back(cPrev2);
        ordered.push_back(c);
        ordered.push_back(cPrev0);
        ordered.push_back(cPrev1);
        break;
    }
    i += 1;
  }

  return ordered;
}

void ValidatorRapidJsonParser::getVendorSpecific(
    const rapidjson::Value& attrData,
    entities::vendorSpecific_t& vendorSpecific) {
  for (rapidjson::Value::ConstMemberIterator itr = attrData.MemberBegin();
       itr != attrData.MemberEnd(); ++itr) {
    const std::string& attrName = itr->name.GetString();
    const auto& attrValue = itr->value;
    if (isVendorSpecific(attrName)) {
      vendorSpecific.emplace(attrName, attrValue);
    }
  }
}

}  // namespace json
}  // namespace secondary
}  // namespace port
