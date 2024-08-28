#ifndef __JSON_CONSTANTS_HPP__
#define __JSON_CONSTANTS_HPP__

// request
constexpr auto JSON_CHANGES = "changes";
constexpr auto JSON_RELATED_RESOURCES = "relatedResources";
constexpr auto JSON_OPERATION = "operation";
constexpr auto JSON_RESOURCE_PATH = "resource_path";
constexpr auto JSON_DATA = "data";
constexpr auto JSON_OPERATION_CREATE = "CREATE";
constexpr auto JSON_OPERATION_UPDATE = "UPDATE";
constexpr auto JSON_OPERATION_DELETE = "DELETE";

// response
constexpr auto JSON_ERRORS = "errors";
constexpr auto JSON_ERROR_MESSAGE = "errorMessage";
constexpr auto JSON_ERROR_DETAILS = "errorDetails";

constexpr auto JSON_AUTH_SUBSCRIPTION = "authSubscription";

// AuthSubscription Static Data
constexpr auto JSON_AUTH_SUBSCRIPTION_STATIC_DATA =
    "authSubscriptionStaticData";
constexpr auto JSON_AUTHENTICATION_METHOD = "authenticationMethod";
constexpr auto JSON_ENC_PERMANENT_KEY = "encPermanentKey";
constexpr auto JSON_AUTHENTICATION_MANAGEMENT_FIELD =
    "authenticationManagementField";
constexpr auto JSON_ALGORITHM_ID = "algorithmId";
constexpr auto JSON_A4_KEY_IND = "a4KeyInd";
constexpr auto JSON_A4_IND = "a4Ind";
constexpr auto JSON_ENC_OPC_KEY = "encOpcKey";
constexpr auto JSON_ENC_TOPC_KEY = "encTopcKey";
constexpr auto JSON_IMSI = "imsi";
constexpr auto JSON_A4_KEY_V = "a4KeyV";
constexpr auto JSON_AKA_ALGORITHM_IND = "akaAlgorithmInd";

// AuthSubscription Dynamic Data
constexpr auto JSON_AUTH_SUBSCRIPTION_DYNAMIC_DATA =
    "authSubscriptionDynamicData";
constexpr auto JSON_SQN_SCHEME = "sqnScheme";
constexpr auto JSON_SQN = "sqn";
constexpr auto JSON_LAST_INDEXES_LIST = "lastIndexes";
constexpr auto JSON_LAST_INDEX_ID = "lastIndexId";
constexpr auto JSON_LAST_INDEX_VALUE = "lastIndexValue";

// AuthenticationMethod
constexpr auto JSON_5G_AKA = "5G_AKA";
constexpr auto JSON_EAP_AKA_PRIME = "EAP_AKA_PRIME";

// ProvJournal
constexpr auto JSON_IMSI_TYPE = "imsi";
constexpr auto JSON_PROV_JOURNAL = "provJournal";
constexpr auto JSON_NOTIF_REF = "notifRef";
constexpr auto JSON_IMSI_MASK = "imsiMask";
constexpr auto JSON_IMSI_EXT_MASK = "imsiExtMask";
constexpr auto JSON_MSISDN = "msisdn";
constexpr auto JSON_MSISDN_MASK = "msisdnMask";
constexpr auto JSON_MSISDN_EXT_MASK = "msisdnExtMask";
constexpr auto JSON_IMSI_AUX = "imsiAux";
constexpr auto JSON_IMSI_AUX_MASK = "imsiAuxMask";
constexpr auto JSON_IMSI_AUX_EXT_MASK = "imsiAuxExtMask";
constexpr auto JSON_IMPI = "impi";
constexpr auto JSON_IMPI_MASK = "impiMask";
constexpr auto JSON_IMPI_EXT_MASK = "impiExtMask";
constexpr auto JSON_SEC_IMPI = "secImpi";
constexpr auto JSON_IMPI_AUX = "impiAux";
constexpr auto JSON_USERNAME = "username";
constexpr auto JSON_USERNAME_MASK = "usernameMask";
constexpr auto JSON_USERNAME_EXT_MASK = "usernameExtMask";
constexpr auto JSON_IMSICHO_STATUS = "imsiChoStatus";
constexpr auto JSON_IMSI_EXPIRY_DATE = "imsiExpiryDate";
constexpr auto JSON_IMSICHO_EXEC = "imsiChoExec";
constexpr auto JSON_IMPUCHO_IDS = "impuChoIds";
constexpr auto JSON_MSC_ID_AUX = "mscIdAux";
constexpr auto JSON_NOTIF_INFO = "notifInfo";
constexpr auto JSON_UE_FUNCTION_MASK = "ueFunctionMask";
constexpr auto JSON_EXT_ID_LIST = "extIdList";
constexpr auto JSON_NAI = "nai";
constexpr auto JSON_NAI_MASK = "naiMask";
constexpr auto JSON_NAI_EXT_MASK = "naiExtMask";
constexpr auto JSON_SUBS_ID_LIST = "subsIdList";
constexpr auto JSON_SUBS_ID = "id";
constexpr auto JSON_SUBS_PREFIX = "prefix";

// AuthSubscriptionLegacy
constexpr auto JSON_F_SET_IND = "FSETIND";
constexpr auto JSON_EKI = "EKI";
constexpr auto JSON_EKI_BASE64 = "EKI:";
constexpr auto JSON_KIND = "KIND";
constexpr auto JSON_A4_IND_LEGACY = "A4IND";
constexpr auto JSON_AMF_VALUE = "AMFVALUE";
constexpr auto JSON_EOPC = "EOPC";
constexpr auto JSON_EOPC_BASE64 = "EOPC:";
constexpr auto JSON_SEQ_HE = "SEQHE";
constexpr auto JSON_SEQ_HE_BASE64 = "SEQHE:";
constexpr auto JSON_AKA_TYPE = "AKATYPE";
constexpr auto JSON_VNUMBER = "VNUMBER";
constexpr auto JSON_AKA_ALG_IND = "AKAALGIND";

#endif  // __JSON_CONSTANTS_HPP__
