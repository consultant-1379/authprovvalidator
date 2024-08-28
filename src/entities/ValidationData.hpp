#ifndef __UDM_AUTHENTICATION_PROVISIONING_VALIDATOR_VALIDATION_DATA__
#define __UDM_AUTHENTICATION_PROVISIONING_VALIDATOR_VALIDATION_DATA__

#include <boost/regex.hpp>

#include "entities/types.hpp"

namespace entities {

auto constexpr AUTH_SUBSCRIPTION_URI_PATTERN =
    "^/subscribers/(.*?)/authSubscription$";
static const auto authSubscriptionUriRegex =
    boost::regex{AUTH_SUBSCRIPTION_URI_PATTERN};
auto constexpr AUTH_SUBSCRIPTION_PRIV_ID_URI_PATTERN =
    "^/subscribers/(.*?)/authSubscription/[^/]*$";
static const auto authSubscriptionPrivIdUriRegex =
    boost::regex{AUTH_SUBSCRIPTION_PRIV_ID_URI_PATTERN};
auto constexpr AUTH_SUBSCRIPTION_STATIC_DATA_URI_PATTERN =
    "^/subscribers/(.*?)/authSubscription/(.*?)/authSubscriptionStaticData$";
static const auto authSubscriptionStaticDataUriRegex =
    boost::regex{AUTH_SUBSCRIPTION_STATIC_DATA_URI_PATTERN};
auto constexpr IMSI_PATTERN =
    "^/subscribers/(.*?)/authSubscription/imsi-([0-9]{5,15})$|^/subscribers/"
    "(.*?)/authSubscription/imsi-([0-9]{5,15})/[^/]*$";
static const auto imsiRegex = boost::regex{IMSI_PATTERN};
auto constexpr POS_IMSI = 4;
auto constexpr LEGACY_BASE_PATH = "/legacy/serv=Auth/IMSI=";
auto constexpr LEGACY_PATTERN = "^/legacy/serv=Auth/[^/]*$";
static const auto legacyRegex = boost::regex{LEGACY_PATTERN};
auto constexpr ENC_PERMANENT_KEY_PATTERN = "^[A-Fa-f0-9]*$";
static const auto encPermanentKeyRegex =
    boost::regex{ENC_PERMANENT_KEY_PATTERN};
auto constexpr AUTHENTICATION_MANAGEMENT_FIELD_PATTERN = "^[A-Fa-f0-9]{4}$";
static const auto authenticationManagementFieldRegex =
    boost::regex{AUTHENTICATION_MANAGEMENT_FIELD_PATTERN};
auto constexpr ALGORITHM_ID_PATTERN = "^[0-9]+$";
static const auto algorithmIdRegex = boost::regex{ALGORITHM_ID_PATTERN};
auto constexpr A4_KEY_IND_PATTERN = "^[0-9]+$";
static const auto a4KeyIndRegex = boost::regex{A4_KEY_IND_PATTERN};
auto constexpr A4_KEY_IND_MIN = 0;
auto constexpr A4_KEY_IND_MAX = 511;
auto constexpr A4_IND_PATTERN = "^[0-9]+$";
static const auto a4IndRegex = boost::regex{A4_IND_PATTERN};
auto constexpr A4_IND_MIN = 0;
auto constexpr A4_IND_MAX = 2;
auto constexpr ENC_OPC_KEY_PATTERN = "^[A-Fa-f0-9]*$";
static const auto encOpcKeyRegex = boost::regex{ENC_OPC_KEY_PATTERN};
auto constexpr MIN_TUAK = 16;
auto constexpr MAX_TUAK = 31;
auto constexpr MIN_ALGORITHM_ID = 0;
auto constexpr MAX_ALGORITHM_ID = 15;
auto constexpr LENGTH_ENC_PERMANENT_KEY = 32;
auto constexpr TEST_ALGORITHM_ID = 1;
auto constexpr AKATYPE_ALLOWED_VALUE = 1;
auto constexpr SEQHE_INVALID_VALUE = "FFFFFFFFFFFF";
auto constexpr SEQHE_BITS_LENGTH = 48;
auto constexpr BITS_PER_CHAR = 4;
auto constexpr A4_KEY_V_PATTERN = "^[0-9]$|^[1-2][0-9]$|^[3][0-1]$";
static const auto a4KeyVRegex = boost::regex{A4_KEY_V_PATTERN};
auto constexpr AKA_ALGORITHM_IND_PATTERN = "^[0-2]$";
static const auto akaAlgorithmIndRegex =
    boost::regex{AKA_ALGORITHM_IND_PATTERN};
static const auto POS_MSCID = 1;
static const auto POS_AUC_IN_IMSI_MASK = 4;  // 0 is the least significative

auto constexpr SQN_MUTATION_VALUE = "000000000000";
auto constexpr SQN_SCHEME_NON_TIME_BASED = "NON_TIME_BASED";
auto constexpr SQN_SCHEME_TIME_BASED = "TIME_BASED";

auto constexpr VALIDATION = 0;
auto constexpr CODE = 1;

class ValidationData final {
 public:
  ValidationData() = default;
  ValidationData(const changes_t &chs, const related_resources_t &relRsrcs)
      : changes{chs}, relatedResources{relRsrcs} {};
  ~ValidationData() = default;

  entities::validation_response_t applyValidationRules();
  void addError(std::string,
                std::initializer_list<std::pair<std::string, std::string>>);
  inline bool hasErrors();
  inline std::string emptyFieldError(const std::string &);
  inline std::string unfoundMandatoryFieldError(const std::string &);
  inline std::string unfoundMandatoryFieldError(const std::string &,
                                                const std::string &);
  inline std::string notObjectFieldError(const std::string &);
  inline std::string notArrayFieldError(const std::string &);
  inline std::string notStringFieldError(const std::string &);
  inline std::string notIntFieldError(const std::string &);
  inline std::string notBoolFieldError(const std::string &);
  static bool checkAuthSubscriptionUri(const std::string &);
  static bool checkAuthSubscriptionStaticDataUri(const std::string &);
  static bool checkAuthSubscriptionPrivIdUri(const std::string &);
  static bool checkUriImsi(const std::string &);
  static bool checkAuthSubscriptionLegacyUri(const std::string &);
  static std::string getImsi(const std::string &);
  inline static std::string getBasePath(const std::string &);
  inline static std::string createPathFromBasePath(
      const std::string &, const std::string & = std::string());
  inline static std::string addSuffixToPath(const std::string &,
                                            const std::string &);
  static unsigned int fromHexStringToUnsignedInt(const std::string &);
  static std::string fromUnsignedIntToHexString(const unsigned int &,
                                                const unsigned int &);
  static entities::sqn_t computeSqnFromSeqHe(const entities::seq_he_t &);
  static std::bitset<48> fromHexStringToBitset(const entities::seq_he_t &);
  static void leftShift(std::bitset<48> &, int);
  static std::string fromBitsetToHexString(const std::bitset<48> &);
  static std::string getMscId(const std::string &);
  static std::string buildProvJournalResourcePathFromMscId(const std::string &);
  static bool checkBitIsSet(const std::string &, const int);

  changes_t changes;
  related_resources_t relatedResources;
  response_t response;

 private:
  entities::validation_response_t checkForCreationAuthSubscription(
      entities::Change &);
  entities::validation_response_t checkForUpdateAuthSubscription(
      entities::Change &);
  entities::validation_response_t checkForCommonAuthSubscriptionRules(
      entities::Change &);
  bool checkForUpdateAuthSubscriptionRules(entities::Change &);
  bool checkForLegacyAuthSubscriptionRules(entities::Change &);
  static bool checkPatternOnData(const boost::regex &, const std::string &);
  static bool checkA4IndInRange(const std::string &);
  static bool checkA4KeyIndInRange(const std::string &);
  static bool checkAlgorithmIdInRange(const std::string &);
  static bool checkAlgorithmIdIsMillenage(const std::string &);
  void computeMutations(entities::Change &);
  bool optionalAttributeHasChanged(std::optional<std::string>,
                                   std::optional<std::string>,
                                   const std::string &, const std::string &);
  bool checkOptionalAttributeWithLegacy(std::optional<std::string>,
                                        const std::optional<std::string> &,
                                        const std::string &,
                                        const std::string &,
                                        const std::string &);
  bool checkOptionalAttributeWithLegacy(std::optional<std::string>,
                                        const std::optional<int> &,
                                        const std::string &,
                                        const std::string &,
                                        const std::string &);
  bool checkOptionalAttributeWithLegacy(unsigned int,
                                        const std::optional<int> &,
                                        const std::string &,
                                        const std::string &,
                                        const std::string &);
  std::string getLegacyPathFromResourcePath(const std::string &);
  void fillOptionalAuthSubscriptionStaticAttributes(
      entities::Change &, const entities::auth_subscription_legacy_t &);
  void fillOptionalAttribute(std::optional<std::string> &,
                             const std::optional<std::string> &);
  void fillOptionalAttribute(std::optional<std::string> &, const std::string &);
  void fillOptionalAttribute(std::optional<std::string> &,
                             const std::optional<int> &);
  bool hasRelatedResource(entities::resource_path_t &);
};

bool ValidationData::hasErrors() { return response.errors.size(); }

std::string ValidationData::emptyFieldError(const std::string &field) {
  return "Field:[" + field + "] is empty";
}

std::string ValidationData::unfoundMandatoryFieldError(
    const std::string &field) {
  return "Mandatory field:[" + field + "] not found";
}

std::string ValidationData::unfoundMandatoryFieldError(
    const std::string &field, const std::string &parent) {
  return "Mandatory field:[" + field + "] not found as child of:[" + parent +
         "]";
}

std::string ValidationData::notObjectFieldError(const std::string &field) {
  return "Field:[" + field + "] is not an object";
}

std::string ValidationData::notArrayFieldError(const std::string &field) {
  return "Field:[" + field + "] is not an array";
}

std::string ValidationData::notStringFieldError(const std::string &field) {
  return "Field:[" + field + "] is not string";
}

std::string ValidationData::notIntFieldError(const std::string &field) {
  return "Field:[" + field + "] is not integer";
}

std::string ValidationData::notBoolFieldError(const std::string &field) {
  return "Field:[" + field + "] is not boolean";
}

std::string ValidationData::getBasePath(const std::string &path) {
  std::size_t found = path.find_last_of("/");
  return path.substr(0, found);
}

std::string ValidationData::createPathFromBasePath(const std::string &path,
                                                   const std::string &suffix) {
  std::string basePath = getBasePath(path);
  if (suffix.empty()) {
    return basePath;
  } else {
    return ValidationData::addSuffixToPath(basePath, suffix);
  }
}

std::string ValidationData::addSuffixToPath(const std::string &path,
                                            const std::string &suffix) {
  std::string fullPath = path;
  return fullPath.append("/").append(suffix);
}
}  // namespace entities

#endif
