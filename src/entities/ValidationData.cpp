#include "entities/ValidationData.hpp"

#include <bitset>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iomanip>
#include <stdexcept>

#include "ports/HTTPcodes.hpp"
#include "ports/json/JsonConstants.hpp"

namespace entities {

entities::validation_response_t ValidationData::applyValidationRules() {
  bool ret = true;
  auto code = ::port::HTTP_OK;

  for (auto c : changes) {
    entities::validation_response_t resp{true, ::port::HTTP_OK};

    if (checkAuthSubscriptionUri(c.resourcePath)) {
      if (not c.operation.compare(JSON_OPERATION_DELETE)) {
        response.changes.push_back(c);
      }
    } else if (checkAuthSubscriptionStaticDataUri(c.resourcePath) or
               checkAuthSubscriptionPrivIdUri(c.resourcePath)) {
      if (checkUriImsi(c.resourcePath)) {
        if (not c.operation.compare(JSON_OPERATION_CREATE)) {
          resp = checkForCreationAuthSubscription(c);
        } else if (not c.operation.compare(JSON_OPERATION_UPDATE)) {
          resp = checkForUpdateAuthSubscription(c);
        }

        ret &= std::get<entities::VALIDATION>(resp);
        // HTTP_UNPROCESSABLE_ENTITY (422) has preference over HTTP_CONFLICT
        // (409) and HTTP_CONFLICT (409) has preference over HTTP_OK (200)
        code = std::max(code, std::get<entities::CODE>(resp));

        if (ret) {
          entities::Change responseChange = c;
          computeMutations(responseChange);
          response.changes.push_back(responseChange);
        }
      } else {
        ret = false;
        code = std::max(code, ::port::HTTP_CONFLICT);
        addError("Constraint Violation",
                 {{"resource_path", c.resourcePath},
                  {"description", "\"" + std::string{JSON_IMSI} + "\" in \"" +
                                      std::string{JSON_RESOURCE_PATH} +
                                      "\" has not the valid format"}});
      }
    } else if (c.resourcePath.ends_with(JSON_AUTH_SUBSCRIPTION_DYNAMIC_DATA)) {
      ret = false;
      code = std::max(code, ::port::HTTP_CONFLICT);
      addError("Constraint Violation",
               {{"resource_path", c.resourcePath},
                {"description",
                 "\"" + std::string{JSON_AUTH_SUBSCRIPTION_DYNAMIC_DATA} +
                     "\" can not be created or updated"}});
    } else if (c.operation.compare(JSON_OPERATION_DELETE)) {
      ret = false;
      code = ::port::HTTP_UNPROCESSABLE_ENTITY;
      addError("Unprocessable entity", {{"resource_path", c.resourcePath},
                                        {"description",
                                         "Validation could not be performed on "
                                         "the specified resource_path"}});
    } else {
      ret = true;
      code = ::port::HTTP_OK;
      response.changes.push_back(c);
    }
  }

  return {ret, code};
}

bool ValidationData::checkAuthSubscriptionUri(const std::string& resourcePath) {
  return checkPatternOnData(authSubscriptionUriRegex, resourcePath);
}

bool ValidationData::checkAuthSubscriptionStaticDataUri(
    const std::string& resourcePath) {
  return checkPatternOnData(authSubscriptionStaticDataUriRegex, resourcePath);
}

bool ValidationData::checkAuthSubscriptionPrivIdUri(
    const std::string& resourcePath) {
  return checkPatternOnData(authSubscriptionPrivIdUriRegex, resourcePath);
}

bool ValidationData::checkUriImsi(const std::string& resourcePath) {
  return checkPatternOnData(imsiRegex, resourcePath);
}

bool ValidationData::checkAuthSubscriptionLegacyUri(
    const std::string& resourcePath) {
  return checkPatternOnData(legacyRegex, resourcePath);
}

std::string ValidationData::getMscId(const std::string& resourcePath) {
  boost::smatch sm;
  if (boost::regex_search(resourcePath, sm,
                          authSubscriptionStaticDataUriRegex)) {
    return sm[POS_MSCID];
  }
  return {};
}

std::string ValidationData::buildProvJournalResourcePathFromMscId(
    const std::string& mscId) {
  return std::string{"/subscribers/"}.append(mscId).append(
      "/journal/provJournal");
}

bool ValidationData::checkBitIsSet(const std::string& bitMask,
                                   const int bitNumber) {
  auto it = bitMask.rbegin() + bitNumber;
  if (*it == '1') {
    return true;
  }

  return false;
}

bool ValidationData::checkPatternOnData(const boost::regex& pattern,
                                        const std::string& data) {
  boost::cmatch patternMatch;
  if (!boost::regex_match(data.c_str(), patternMatch, pattern)) {
    return false;
  }
  return true;
}

std::string ValidationData::getImsi(const std::string& path) {
  boost::cmatch patternMatch;
  if (!boost::regex_match(path.c_str(), patternMatch, imsiRegex)) {
    return {};
  } else {
    return boost::lexical_cast<std::string>(patternMatch[POS_IMSI]);
  }
}

bool ValidationData::checkA4IndInRange(const std::string& a4ind) {
  int value;
  try {
    value = std::stoi(a4ind);
  } catch (...) {
    return false;
  }
  return ((value >= A4_IND_MIN) && (value <= A4_IND_MAX));
}

bool ValidationData::checkA4KeyIndInRange(const std::string& a4keyind) {
  int value;
  try {
    value = std::stoi(a4keyind);
  } catch (...) {
    return false;
  }
  return ((value >= A4_KEY_IND_MIN) && (value <= A4_KEY_IND_MAX));
}

bool ValidationData::checkAlgorithmIdInRange(const std::string& algorithm) {
  int value;
  try {
    value = std::stoi(algorithm);
  } catch (...) {
    return false;
  }
  return ((value >= MIN_ALGORITHM_ID) && (value <= MAX_ALGORITHM_ID));
}

bool ValidationData::checkAlgorithmIdIsMillenage(const std::string& algorithm) {
  int value;
  try {
    value = std::stoi(algorithm);
  } catch (...) {
    return false;
  }
  return ((value >= MIN_ALGORITHM_ID) && (value <= MAX_ALGORITHM_ID) &&
          (value != TEST_ALGORITHM_ID));
}

unsigned int ValidationData::fromHexStringToUnsignedInt(
    const std::string& str) {
  std::istringstream converter(str);
  unsigned int value;
  converter >> std::hex >> value >> std::dec;
  return value;
}

std::string ValidationData::fromUnsignedIntToHexString(
    const unsigned int& value, const unsigned int& len) {
  std::stringstream str;
  str << std::setfill('0') << std::setw(len) << std::hex << value << std::dec;
  return (boost::to_upper_copy(str.str()));
}

void ValidationData::computeMutations(entities::Change& change) {
  if (checkAuthSubscriptionStaticDataUri(change.resourcePath)) {
    if (not change.operation.compare(JSON_OPERATION_CREATE)) {
      entities::auth_subscription_dynamic_data_t dynData;

      std::string legacyPath =
          getLegacyPathFromResourcePath(change.resourcePath);

      if (relatedResources.contains(legacyPath)) {
        auto authSubscriptionLegacyRelResource =
            boost::get<entities::auth_subscription_legacy_t>(
                relatedResources.at(legacyPath));

        entities::seq_he_t seqHe;
        if (authSubscriptionLegacyRelResource.seqHe.has_value()) {
          seqHe = authSubscriptionLegacyRelResource.seqHe.value();
        }

        if (not authSubscriptionLegacyRelResource.seqHe.has_value() or
            not seqHe.compare(SEQHE_INVALID_VALUE)) {
          dynData.sqnScheme.emplace(SQN_SCHEME_TIME_BASED);
        } else {
          dynData.sqnScheme.emplace(SQN_SCHEME_NON_TIME_BASED);
          entities::sqn_t sqn = computeSqnFromSeqHe(seqHe);
          dynData.sqn.emplace(sqn);
        }
        change.authSubscription.authSubscriptionDynamicData = dynData;

        fillOptionalAuthSubscriptionStaticAttributes(
            change, authSubscriptionLegacyRelResource);

      } else {
        dynData.sqn.emplace(SQN_MUTATION_VALUE);
        dynData.sqnScheme.emplace(SQN_SCHEME_NON_TIME_BASED);
        change.authSubscription.authSubscriptionDynamicData = dynData;
      }
    } else if (not change.operation.compare(JSON_OPERATION_UPDATE)) {
      // Update of dynamic data is not done:
      // In case the user tries to update dynamic data,
      // we should return error
      if (relatedResources.contains(change.resourcePath)) {
        auto authSubscriptionRelResource =
            boost::get<entities::auth_subscription_t>(
                relatedResources.at(change.resourcePath));
        if (not authSubscriptionRelResource.authSubscriptionDynamicData
                    .has_value()) {
          change.authSubscription.authSubscriptionDynamicData.reset();
        }
      }
    }
  }
}

void ValidationData::fillOptionalAuthSubscriptionStaticAttributes(
    entities::Change& c,
    const entities::auth_subscription_legacy_t& relResource) {
  if (c.authSubscription.authSubscriptionStaticData.has_value()) {
    fillOptionalAttribute(
        c.authSubscription.authSubscriptionStaticData.value().encPermanentKey,
        relResource.eki);
    fillOptionalAttribute(
        c.authSubscription.authSubscriptionStaticData.value().a4KeyInd,
        relResource.kind);
    fillOptionalAttribute(
        c.authSubscription.authSubscriptionStaticData.value().a4Ind,
        relResource.a4Ind);
    fillOptionalAttribute(
        c.authSubscription.authSubscriptionStaticData.value().algorithmId,
        relResource.fSetInd);
    // transform AMFVALUE from integer to hex string
    if (relResource.amfValue.has_value()) {
      std::string hexStr =
          fromUnsignedIntToHexString(relResource.amfValue.value(), 4);
      fillOptionalAttribute(
          c.authSubscription.authSubscriptionStaticData.value()
              .authenticationManagementField,
          hexStr);
    }
    fillOptionalAttribute(
        c.authSubscription.authSubscriptionStaticData.value().encOpcKey,
        relResource.eopc);
    fillOptionalAttribute(
        c.authSubscription.authSubscriptionStaticData.value().a4KeyV,
        relResource.vNumber);
    fillOptionalAttribute(
        c.authSubscription.authSubscriptionStaticData.value().akaAlgorithmInd,
        relResource.akaAlgInd);
  }
}

void ValidationData::fillOptionalAttribute(
    std::optional<std::string>& attr,
    const std::optional<std::string>& attrLegacy) {
  if (not attr.has_value() and attrLegacy.has_value()) {
    attr.emplace(attrLegacy.value());
  }
}

void ValidationData::fillOptionalAttribute(std::optional<std::string>& attr,
                                           const std::string& attrLegacy) {
  if (not attr.has_value()) {
    attr.emplace(attrLegacy);
  }
}

void ValidationData::fillOptionalAttribute(
    std::optional<std::string>& attr, const std::optional<int>& attrLegacy) {
  if (not attr.has_value() and attrLegacy.has_value()) {
    attr.emplace(std::to_string(attrLegacy.value()));
  }
}

entities::sqn_t ValidationData::computeSqnFromSeqHe(
    const entities::seq_he_t& seqHe) {
  entities::sqn_t sqn;

  std::bitset<SEQHE_BITS_LENGTH> bits = fromHexStringToBitset(seqHe);

  leftShift(bits, 5);

  sqn = fromBitsetToHexString(bits);

  return sqn;
}

std::bitset<SEQHE_BITS_LENGTH> ValidationData::fromHexStringToBitset(
    const entities::seq_he_t& seqHe) {
  unsigned long tempValue;

  std::istringstream ost(seqHe);
  ost >> std::hex >> tempValue >> std::dec;
  std::bitset<SEQHE_BITS_LENGTH> bits(tempValue);

  return bits;
}

void ValidationData::leftShift(std::bitset<SEQHE_BITS_LENGTH>& bits, int N) {
  bits <<= N;
}

std::string ValidationData::fromBitsetToHexString(
    const std::bitset<SEQHE_BITS_LENGTH>& bits) {
  std::stringstream res;

  res << std::hex << std::uppercase << bits.to_ulong() << std::dec;

  // Add zeroes to the left
  std::string tempStr = res.str();
  auto nZeroesToAdd = SEQHE_BITS_LENGTH / BITS_PER_CHAR - tempStr.length();

  std::string hexStr = tempStr;
  if (nZeroesToAdd > 0) {
    std::string s(nZeroesToAdd, '0');
    hexStr = s.append(tempStr);
  }
  return hexStr;
}

entities::validation_response_t
ValidationData::checkForCommonAuthSubscriptionRules(entities::Change& change) {
  auto code = ::port::HTTP_CONFLICT;

  if (change.authSubscription.authSubscriptionStaticData.has_value()) {
    auto authSubscriptionStaticData =
        change.authSubscription.authSubscriptionStaticData.value();
    auto authenticationMethod = authSubscriptionStaticData.authenticationMethod;
    auto isAuthenticatedMethodValid =
        (JSON_5G_AKA == authenticationMethod) ||
        (JSON_EAP_AKA_PRIME == authenticationMethod);

    if (!isAuthenticatedMethodValid) {
      addError("Constraint Violation",
               {{"resource_path", change.resourcePath},
                {"description",
                 "\"" + std::string{JSON_AUTHENTICATION_METHOD} + "\" in \"" +
                     std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                     "\" has not the valid format"}});
    }

    auto isAuthenticationMethodAKA =
        (JSON_5G_AKA == authenticationMethod) ||
        (JSON_EAP_AKA_PRIME == authenticationMethod);

    std::string legacyPath = getLegacyPathFromResourcePath(change.resourcePath);

    auto has4GLegacy = false;
    if (relatedResources.contains(legacyPath)) {
      has4GLegacy = true;
    }

    if (authSubscriptionStaticData.encPermanentKey.has_value()) {
      if (!checkPatternOnData(
              encPermanentKeyRegex,
              authSubscriptionStaticData.encPermanentKey.value())) {
        addError("Constraint Violation",
                 {{"resource_path", change.resourcePath},
                  {"description",
                   "\"" + std::string{JSON_ENC_PERMANENT_KEY} + "\" in \"" +
                       std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                       "\" has invalid value"}});
      } else if (authSubscriptionStaticData.encPermanentKey.value().length() !=
                 LENGTH_ENC_PERMANENT_KEY) {
        addError("Constraint Violation",
                 {{"resource_path", change.resourcePath},
                  {"description",
                   "\"" + std::string{JSON_ENC_PERMANENT_KEY} + "\" in \"" +
                       std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                       "\" has invalid size"}});
      }
    } else if (isAuthenticationMethodAKA and not has4GLegacy) {
      addError(
          "Constraint Violation",
          {{"resource_path", change.resourcePath},
           {"description",
            "\"" + std::string{JSON_ENC_PERMANENT_KEY} + "\" in \"" +
                std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                "\" has not been defined with an AKA authentication method"}});
    }

    if (authSubscriptionStaticData.authenticationManagementField.has_value()) {
      if (!checkPatternOnData(authenticationManagementFieldRegex,
                              authSubscriptionStaticData
                                  .authenticationManagementField.value())) {
        addError(
            "Constraint Violation",
            {{"resource_path", change.resourcePath},
             {"description",
              "\"" + std::string{JSON_AUTHENTICATION_MANAGEMENT_FIELD} +
                  "\" in \"" + std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                  "\" has invalid value"}});
      }
    } else if (isAuthenticationMethodAKA and not has4GLegacy) {
      addError(
          "Constraint Violation",
          {{"resource_path", change.resourcePath},
           {"description",
            "\"" + std::string{JSON_AUTHENTICATION_MANAGEMENT_FIELD} +
                "\" in \"" + std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                "\" has not been defined with an AKA authentication method"}});
    }

    if (authSubscriptionStaticData.algorithmId.has_value()) {
      if (!checkPatternOnData(algorithmIdRegex,
                              authSubscriptionStaticData.algorithmId.value())) {
        addError("Constraint Violation",
                 {{"resource_path", change.resourcePath},
                  {"description",
                   "\"" + std::string{JSON_ALGORITHM_ID} + "\" in \"" +
                       std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                       "\" has invalid value"}});
      } else if (not checkAlgorithmIdInRange(
                     authSubscriptionStaticData.algorithmId.value())) {
        addError("Constraint Violation",
                 {{"resource_path", change.resourcePath},
                  {"description",
                   "\"" + std::string{JSON_ALGORITHM_ID} + "\" in \"" +
                       std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                       "\" has invalid value"}});
      }
    } else if (isAuthenticationMethodAKA and not has4GLegacy) {
      addError(
          "Constraint Violation",
          {{"resource_path", change.resourcePath},
           {"description",
            "\"" + std::string{JSON_ALGORITHM_ID} + "\" in \"" +
                std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                "\" has not been defined with an AKA authentication method"}});
    }

    if (authSubscriptionStaticData.a4KeyInd.has_value()) {
      if (!checkPatternOnData(a4KeyIndRegex,
                              authSubscriptionStaticData.a4KeyInd.value())) {
        addError("Constraint Violation",
                 {{"resource_path", change.resourcePath},
                  {"description",
                   "\"" + std::string{JSON_A4_KEY_IND} + "\" in \"" +
                       std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                       "\" has invalid value"}});
      } else if (not checkA4KeyIndInRange(
                     authSubscriptionStaticData.a4KeyInd.value())) {
        addError("Constraint Violation",
                 {{"resource_path", change.resourcePath},
                  {"description",
                   "\"" + std::string{JSON_A4_KEY_IND} + "\" in \"" +
                       std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                       "\" has invalid value"}});
      }
    } else if (isAuthenticationMethodAKA and not has4GLegacy) {
      addError(
          "Constraint Violation",
          {{"resource_path", change.resourcePath},
           {"description",
            "\"" + std::string{JSON_A4_KEY_IND} + "\" in \"" +
                std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                "\" has not been defined with an AKA authentication method"}});
    }

    if (authSubscriptionStaticData.a4Ind.has_value()) {
      if (!checkPatternOnData(a4IndRegex,
                              authSubscriptionStaticData.a4Ind.value())) {
        addError("Constraint Violation",
                 {{"resource_path", change.resourcePath},
                  {"description",
                   "\"" + std::string{JSON_A4_IND} + "\" in \"" +
                       std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                       "\" has invalid value"}});
      } else if (not checkA4IndInRange(
                     authSubscriptionStaticData.a4Ind.value())) {
        addError("Constraint Violation",
                 {{"resource_path", change.resourcePath},
                  {"description",
                   "\"" + std::string{JSON_A4_IND} + "\" in \"" +
                       std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                       "\" has invalid value"}});
      }
    } else if (isAuthenticationMethodAKA and not has4GLegacy) {
      addError(
          "Constraint Violation",
          {{"resource_path", change.resourcePath},
           {"description",
            "\"" + std::string{JSON_A4_IND} + "\" in \"" +
                std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                "\" has not been defined with an AKA authentication method"}});
    }

    if (authSubscriptionStaticData.encOpcKey.has_value()) {
      if (!checkPatternOnData(encOpcKeyRegex,
                              authSubscriptionStaticData.encOpcKey.value())) {
        addError("Constraint Violation",
                 {{"resource_path", change.resourcePath},
                  {"description",
                   "\"" + std::string{JSON_ENC_OPC_KEY} + "\" in \"" +
                       std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                       "\" has invalid value"}});
      } else if (not authSubscriptionStaticData.algorithmId.has_value()) {
        addError("Constraint Violation",
                 {{"resource_path", change.resourcePath},
                  {"description",
                   "\"" + std::string{JSON_ENC_OPC_KEY} + "\" in \"" +
                       std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                       "\" must not be present as \"" +
                       std::string{JSON_ALGORITHM_ID} +
                       "\" is not MILLENAGE (0,2-15)"}});
      } else if (checkPatternOnData(
                     algorithmIdRegex,
                     authSubscriptionStaticData.algorithmId.value())) {
        if (!checkAlgorithmIdIsMillenage(
                authSubscriptionStaticData.algorithmId.value())) {
          addError("Constraint Violation",
                   {{"resource_path", change.resourcePath},
                    {"description",
                     "\"" + std::string{JSON_ENC_OPC_KEY} + "\" in \"" +
                         std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                         "\" must not be present as \"" +
                         std::string{JSON_ALGORITHM_ID} +
                         "\" is not MILLENAGE (0,2-15)"}});
        }
      }
    }

    if (authSubscriptionStaticData.a4KeyV.has_value() and
        not checkPatternOnData(a4KeyVRegex,
                               authSubscriptionStaticData.a4KeyV.value())) {
      addError(
          "Constraint Violation",
          {{"resource_path", change.resourcePath},
           {"description", "\"" + std::string{JSON_A4_KEY_V} + "\" in \"" +
                               std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                               "\" has invalid value"}});
    }

    if (authSubscriptionStaticData.akaAlgorithmInd.has_value()) {
      if (not checkPatternOnData(
              akaAlgorithmIndRegex,
              authSubscriptionStaticData.akaAlgorithmInd.value())) {
        addError("Constraint Violation",
                 {{"resource_path", change.resourcePath},
                  {"description",
                   "\"" + std::string{JSON_AKA_ALGORITHM_IND} + "\" in \"" +
                       std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                       "\" has invalid value"}});
      }

      auto mscId = getMscId(change.resourcePath);
      auto pathToFind = buildProvJournalResourcePathFromMscId(mscId);
      if (relatedResources.contains(pathToFind)) {
        auto journal = boost::get<entities::prov_journal_t>(
            relatedResources.at(pathToFind));

        if (journal.imsiMask.empty() or
            not checkBitIsSet(journal.imsiMask, POS_AUC_IN_IMSI_MASK)) {
          addError(
              "Constraint Violation",
              {{"resource_path", change.resourcePath},
               {"description",
                "It is not allowed to create or update a subscriber with \"" +
                    std::string{JSON_AKA_ALGORITHM_IND} + "\" in \"" +
                    std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                    "\" if not defined in AuC"}});
        }
      } else {
        code = ::port::HTTP_UNPROCESSABLE_ENTITY;
        addError("Unprocessable entity",
                 {{"resource_path", change.resourcePath},
                  {"description", "provJournal for subscriber mscId=" + mscId +
                                      " not included. Needed to check if user "
                                      "is defined in AuC when attribute \"" +
                                      std::string{JSON_AKA_ALGORITHM_IND} +
                                      "\" is present"}});
      }
    }
  }

  if (change.authSubscription.authSubscriptionDynamicData.has_value()) {
    addError("Constraint Violation",
             {{"resource_path", change.resourcePath},
              {"description",
               "\"" + std::string{JSON_AUTH_SUBSCRIPTION_DYNAMIC_DATA} +
                   "\" can not be created or updated"}});
  }

  if (hasErrors()) {
    return {false, code};
  }
  return {true, ::port::HTTP_OK};
}

entities::validation_response_t
ValidationData::checkForCreationAuthSubscription(entities::Change& change) {
  bool ret = true;

  auto resp = checkForCommonAuthSubscriptionRules(change);
  ret &= std::get<entities::VALIDATION>(resp);
  if (ret) {
    ret &= checkForLegacyAuthSubscriptionRules(change);
  }

  return {ret, ret ? port::HTTP_OK
                   : std::max(std::get<entities::CODE>(resp),
                              ::port::HTTP_CONFLICT)};
}

entities::validation_response_t ValidationData::checkForUpdateAuthSubscription(
    entities::Change& change) {
  bool ret = true;
  auto path = change.resourcePath;
  auto basePath = ValidationData::getBasePath(path);

  if (not hasRelatedResource(basePath)) {
    addError("Unprocessable entity",
             {{"resource_path", path},
              {"description",
               "There is no associated relatedResource: " + basePath}});
    return {false, ::port::HTTP_UNPROCESSABLE_ENTITY};
  }

  auto resp = checkForCommonAuthSubscriptionRules(change);
  ret &= std::get<entities::VALIDATION>(resp);
  if (ret) {
    ret &= checkForUpdateAuthSubscriptionRules(change);
  }

  return {ret, ret ? port::HTTP_OK
                   : std::max(std::get<entities::CODE>(resp),
                              ::port::HTTP_CONFLICT)};
}

bool ValidationData::checkForUpdateAuthSubscriptionRules(
    entities::Change& change) {
  bool ret = true;
  auto authSubscriptionStaticData =
      change.authSubscription.authSubscriptionStaticData.value();

  auto authSubsResourcePath = ValidationData::getBasePath(change.resourcePath);

  if (relatedResources.contains(authSubsResourcePath)) {
    auto authSubscriptionRelResource =
        boost::get<entities::auth_subscription_t>(
            relatedResources.at(authSubsResourcePath));
    auto authSubscriptionStaticDataRelResource =
        authSubscriptionRelResource.authSubscriptionStaticData.value();

    ret &= optionalAttributeHasChanged(
        authSubscriptionStaticData.encPermanentKey,
        authSubscriptionStaticDataRelResource.encPermanentKey,
        change.resourcePath, std::string{JSON_ENC_PERMANENT_KEY});
    ret &= optionalAttributeHasChanged(
        authSubscriptionStaticData.algorithmId,
        authSubscriptionStaticDataRelResource.algorithmId, change.resourcePath,
        std::string{JSON_ALGORITHM_ID});
    ret &= optionalAttributeHasChanged(
        authSubscriptionStaticData.a4KeyInd,
        authSubscriptionStaticDataRelResource.a4KeyInd, change.resourcePath,
        std::string{JSON_A4_KEY_IND});
    ret &= optionalAttributeHasChanged(
        authSubscriptionStaticData.a4Ind,
        authSubscriptionStaticDataRelResource.a4Ind, change.resourcePath,
        std::string{JSON_A4_IND});
    ret &= optionalAttributeHasChanged(
        authSubscriptionStaticData.encOpcKey,
        authSubscriptionStaticDataRelResource.encOpcKey, change.resourcePath,
        std::string{JSON_ENC_OPC_KEY});
    ret &= optionalAttributeHasChanged(
        authSubscriptionStaticData.a4KeyV,
        authSubscriptionStaticDataRelResource.a4KeyV, change.resourcePath,
        std::string{JSON_A4_KEY_V});
  }

  return ret;
}

std::string ValidationData::getLegacyPathFromResourcePath(
    const std::string& path) {
  return std::string{LEGACY_BASE_PATH}.append(getImsi(path));
}

bool ValidationData::checkForLegacyAuthSubscriptionRules(
    entities::Change& change) {
  bool ret = true;

  std::string legacyPath = getLegacyPathFromResourcePath(change.resourcePath);

  if (relatedResources.contains(legacyPath)) {
    auto authSubscriptionLegacyRelResource =
        boost::get<entities::auth_subscription_legacy_t>(
            relatedResources.at(legacyPath));

    // Legacy.AKATYPE must be defined with value 1
    if (not authSubscriptionLegacyRelResource.akaType.has_value()) {
      ret = false;
      addError("Constraint Violation",
               {{"resource_path", change.resourcePath},
                {"description",
                 "\"" + std::string{JSON_AKA_TYPE} +
                     "\" for 4G legacy subscription is not defined"}});
    } else if (authSubscriptionLegacyRelResource.akaType.value() !=
               AKATYPE_ALLOWED_VALUE) {
      ret = false;
      addError("Constraint Violation",
               {{"resource_path", change.resourcePath},
                {"description",
                 "\"" + std::string{JSON_AKA_TYPE} +
                     "\" for 4G legacy subscription has an invalid value"}});
    }

    if (change.authSubscription.authSubscriptionStaticData.has_value()) {
      // If encPermanentKey is defined, it must be equal to legacy.EKI
      ret &= checkOptionalAttributeWithLegacy(
          change.authSubscription.authSubscriptionStaticData.value()
              .encPermanentKey,
          authSubscriptionLegacyRelResource.eki, change.resourcePath,
          std::string{JSON_ENC_PERMANENT_KEY}, std::string{JSON_EKI});

      // If a4KeyInd is defined, it must be equal to legacy.KIND
      ret &= checkOptionalAttributeWithLegacy(
          change.authSubscription.authSubscriptionStaticData.value().a4KeyInd,
          authSubscriptionLegacyRelResource.kind, change.resourcePath,
          std::string{JSON_A4_KEY_IND}, std::string{JSON_KIND});

      // If a4Ind is defined, it must be equal to legacy.A4IND
      ret &= checkOptionalAttributeWithLegacy(
          change.authSubscription.authSubscriptionStaticData.value().a4Ind,
          authSubscriptionLegacyRelResource.a4Ind, change.resourcePath,
          std::string{JSON_A4_IND}, std::string{JSON_A4_IND_LEGACY});

      // If algorithmId is defined, it must be equal to legacy.FSETIND
      ret &= checkOptionalAttributeWithLegacy(
          change.authSubscription.authSubscriptionStaticData.value()
              .algorithmId,
          authSubscriptionLegacyRelResource.fSetInd, change.resourcePath,
          std::string{JSON_ALGORITHM_ID}, std::string{JSON_F_SET_IND});

      // If authenticationManagementField is defined, it must be equal to
      // legacy.AMFVALUE
      // Consider that authenticationManagementField is hexadecimal format
      // string while AMFVALUE is decimal format integer
      if (change.authSubscription.authSubscriptionStaticData.value()
              .authenticationManagementField.has_value()) {
        unsigned int value = fromHexStringToUnsignedInt(
            change.authSubscription.authSubscriptionStaticData.value()
                .authenticationManagementField.value());
        ret &= checkOptionalAttributeWithLegacy(
            value, authSubscriptionLegacyRelResource.amfValue,
            change.resourcePath,
            std::string{JSON_AUTHENTICATION_MANAGEMENT_FIELD},
            std::string{JSON_AMF_VALUE});
      }

      // If encOpcKey is defined, it must be equal to legacy.EOPC
      ret &= checkOptionalAttributeWithLegacy(
          change.authSubscription.authSubscriptionStaticData.value().encOpcKey,
          authSubscriptionLegacyRelResource.eopc, change.resourcePath,
          std::string{JSON_ENC_OPC_KEY}, std::string{JSON_EOPC});

      // If akaAlgorithmInd is defined, it must be equal to legacy.AKAALGIND
      ret &= checkOptionalAttributeWithLegacy(
          change.authSubscription.authSubscriptionStaticData.value()
              .akaAlgorithmInd,
          authSubscriptionLegacyRelResource.akaAlgInd, change.resourcePath,
          std::string{JSON_AKA_ALGORITHM_IND}, std::string{JSON_AKA_ALG_IND});
    }
  }

  return ret;
}

bool ValidationData::checkOptionalAttributeWithLegacy(
    std::optional<std::string> attr,
    const std::optional<std::string>& attrLegacy,
    const std::string& resourcePath, const std::string& attrName,
    const std::string& attrLegacyName) {
  if (attr.has_value() && attrLegacy.has_value() &&
      attr.value().compare(attrLegacy.value())) {
    addError("Constraint Violation",
             {{"resource_path", resourcePath},
              {"description", "\"" + attrName + "\" is not equal to \"" +
                                  attrLegacyName +
                                  "\" attribute of 4G legacy subscription"}});
    return false;
  }
  return true;
}

bool ValidationData::checkOptionalAttributeWithLegacy(
    std::optional<std::string> attr, const std::optional<int>& attrLegacy,
    const std::string& resourcePath, const std::string& attrName,
    const std::string& attrLegacyName) {
  if (attr.has_value() && attrLegacy.has_value() &&
      attr.value().compare(std::to_string(attrLegacy.value()))) {
    addError("Constraint Violation",
             {{"resource_path", resourcePath},
              {"description", "\"" + attrName + "\" is not equal to \"" +
                                  attrLegacyName +
                                  "\" attribute of 4G legacy subscription"}});
    return false;
  }
  return true;
}

bool ValidationData::checkOptionalAttributeWithLegacy(
    unsigned int attr, const std::optional<int>& attrLegacy,
    const std::string& resourcePath, const std::string& attrName,
    const std::string& attrLegacyName) {
  if (attrLegacy.has_value() && attr != (unsigned int)attrLegacy.value()) {
    addError("Constraint Violation",
             {{"resource_path", resourcePath},
              {"description", "\"" + attrName + "\" is not equal to \"" +
                                  attrLegacyName +
                                  "\" attribute of 4G legacy subscription"}});
    return false;
  }
  return true;
}

bool ValidationData::optionalAttributeHasChanged(
    std::optional<std::string> newValue, std::optional<std::string> oldValue,
    const std::string& resourcePath, const std::string& attrName) {
  if ((newValue.has_value() && not oldValue.has_value()) ||
      (not newValue.has_value() && oldValue.has_value())) {
    addError(
        "Constraint Violation",
        {{"resource_path", resourcePath},
         {"description", "\"" + attrName + "\" in \"" +
                             std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                             "\" cannot be modified"}});
    return false;
  } else if (newValue.has_value() && oldValue.has_value() &&
             newValue.value().compare(oldValue.value())) {
    addError(
        "Constraint Violation",
        {{"resource_path", resourcePath},
         {"description", "\"" + attrName + "\" in \"" +
                             std::string{JSON_AUTH_SUBSCRIPTION_STATIC_DATA} +
                             "\" cannot be modified"}});
    return false;
  }
  return true;
}

void ValidationData::addError(
    std::string message,
    std::initializer_list<std::pair<std::string, std::string>> args) {
  entities::Error err;
  err.errorMessage = message;
  for (auto& i : args) {
    err.errorDetails.insert(i);
  }
  response.errors.push_back(err);
}

bool ValidationData::hasRelatedResource(entities::resource_path_t& path) {
  if (relatedResources.contains(path)) {
    return true;
  }
  return false;
}

}  // namespace entities
