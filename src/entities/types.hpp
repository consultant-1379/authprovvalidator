#ifndef __UDM_AUTHENTICATION_PROVISIONING_VALIDATOR_ENTITIES_TYPES__
#define __UDM_AUTHENTICATION_PROVISIONING_VALIDATOR_ENTITIES_TYPES__

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "boost/variant.hpp"
#include "rapidjson/document.h"

namespace entities {

using authenticationMethod_t = std::string;
using encPermanentKey_t = std::string;
using authenticationManagementField_t = std::string;
using algorithmId_t = std::string;
using a4KeyInd_t = std::string;
using a4Ind_t = std::string;
using encOpcKey_t = std::string;
using encTopcKey_t = std::string;
using a4KeyV_t = std::string;
using akaAlgorithmInd_t = std::string;

using genericValue_t = rapidjson::GenericValue<rapidjson::UTF8<> >;
using vendorSpecific_t = std::map<std::string, const genericValue_t&>;

struct AuthSubscriptionStaticData {
  authenticationMethod_t authenticationMethod;
  std::optional<encPermanentKey_t> encPermanentKey;
  std::optional<authenticationManagementField_t> authenticationManagementField;
  std::optional<algorithmId_t> algorithmId;
  std::optional<a4KeyInd_t> a4KeyInd;
  std::optional<a4Ind_t> a4Ind;
  std::optional<encOpcKey_t> encOpcKey;
  std::optional<encTopcKey_t> encTopcKey;
  std::optional<a4KeyV_t> a4KeyV;
  std::optional<akaAlgorithmInd_t> akaAlgorithmInd;
  vendorSpecific_t vendorSpecific;
};
using auth_subscription_static_data_t = AuthSubscriptionStaticData;

using sqnScheme_t = std::string;
using sqn_t = std::string;

using last_indexes_list_t = std::map<std::string, int>;

struct AuthSubscriptionDynamicData {
  std::optional<sqnScheme_t> sqnScheme;
  std::optional<sqn_t> sqn;
  std::optional<last_indexes_list_t> lastIndexesList;
};

using auth_subscription_dynamic_data_t = AuthSubscriptionDynamicData;

struct AuthSubscription {
  std::optional<auth_subscription_static_data_t> authSubscriptionStaticData;
  std::optional<auth_subscription_dynamic_data_t> authSubscriptionDynamicData;
};

using operation_t = std::string;
using resource_path_t = std::string;
using auth_subscription_t = AuthSubscription;

struct Change {
  operation_t operation;
  resource_path_t resourcePath;
  auth_subscription_t authSubscription;
};

using notif_ref_t = std::string;
using imsi_type_t = std::string;
using service_mask_t = std::string;
using ext_service_mask_t = std::string;
using msisdn_type_t = std::string;
using impi_type_t = std::string;
using user_name_t = std::string;
using imsi_cho_status_t = int;
using imsi_expity_date_t = std::string;
using impu_cho_ids_t = std::vector<std::string>;
using msc_id_type_t = std::string;
using notif_info_t = std::string;
using ue_function_mask_t = std::string;
using id_t = std::string;
using identities_id_t = std::vector<id_t>;
using nai_type_t = std::string;
using prefix_t = std::string;

struct SubscriberIdentitiesId {
  id_t id;
  prefix_t prefix;
};

using subscriber_identities_id_t = SubscriberIdentitiesId;
using subs_id_list_t = std::vector<subscriber_identities_id_t>;

struct ProvJournal {
  notif_ref_t notifRef;
  imsi_type_t imsi;
  service_mask_t imsiMask;
  ext_service_mask_t imsiExtMask;
  msisdn_type_t msisdn;
  service_mask_t msisdnMask;
  ext_service_mask_t msisdnExtMask;
  imsi_type_t imsiAux;
  service_mask_t imsiAuxMask;
  ext_service_mask_t imsiAuxExtMask;
  impi_type_t impi;
  service_mask_t impiMask;
  ext_service_mask_t impiExtMask;
  impi_type_t secImpi;
  impi_type_t impiAux;
  user_name_t username;
  service_mask_t usernameMask;
  ext_service_mask_t usernameExtMask;
  imsi_cho_status_t imsiChoStatus;
  imsi_expity_date_t imsiExpiryDate;
  service_mask_t imsiChoExec;
  impu_cho_ids_t impuChoIds;
  msc_id_type_t mscIdAux;
  notif_info_t notifInfo;
  ue_function_mask_t ueFunctionMask;
  identities_id_t extIdList;
  nai_type_t nai;
  service_mask_t naiMask;
  ext_service_mask_t naiExtMask;
  subs_id_list_t subsIdList;
};

using f_set_ind_t = int;
using eki_t = std::string;
using kind_t = int;
using amf_value_t = int;
using eopc_t = std::string;
using seq_he_t = std::string;
using a4Ind_legacy_t = int;
using aka_type_t = int;
using vNumber_t = int;
using akaAlgInd_t = int;

struct AuthSubscriptionLegacy {
  std::optional<f_set_ind_t> fSetInd;
  std::optional<eki_t> eki;
  std::optional<kind_t> kind;
  std::optional<a4Ind_legacy_t> a4Ind;
  std::optional<amf_value_t> amfValue;
  std::optional<eopc_t> eopc;
  std::optional<seq_he_t> seqHe;
  std::optional<aka_type_t> akaType;
  std::optional<vNumber_t> vNumber;
  std::optional<akaAlgInd_t> akaAlgInd;
};

using changes_t = std::vector<Change>;
using prov_journal_t = ProvJournal;
using auth_subscription_legacy_t = AuthSubscriptionLegacy;
using path_t = std::string;
using resource_t = boost::variant<auth_subscription_t, prov_journal_t,
                                  auth_subscription_legacy_t>;
using related_resources_t = std::map<path_t, resource_t>;

using error_details_t = std::map<std::string, std::string>;
using error_message_t = std::string;

struct Error {
  error_message_t errorMessage;
  error_details_t errorDetails;
};

using errors_t = std::vector<Error>;

struct Response {
  changes_t changes;
  errors_t errors;
};

using response_t = Response;

using validation_response_t = std::tuple<bool, int>;

}  // namespace entities

#endif  // __UDM_AUTHENTICATION_PROVISIONING_VALIDATOR_ENTITIES_TYPES__
