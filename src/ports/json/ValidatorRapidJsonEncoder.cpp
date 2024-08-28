#include "ValidatorRapidJsonEncoder.hpp"

#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#include "JsonConstants.hpp"

namespace port {
namespace secondary {
namespace json {

ValidatorRapidJsonEncoder::ValidatorRapidJsonEncoder()
    : rJsonDoc{}, prettyFormat{false} {}

void ValidatorRapidJsonEncoder::authSubscriptionStaticDataToJson(
    rapidjson::Value& data,
    const entities::auth_subscription_t& authSubscription,
    rapidjson::Document::AllocatorType& allocator) {
  if (authSubscription.authSubscriptionStaticData.has_value()) {
    auto authSubscriptionStaticData =
        authSubscription.authSubscriptionStaticData.value();

    vendorSpecificToJson(data, authSubscriptionStaticData.vendorSpecific,
                         allocator);

    strToJson(data, authSubscriptionStaticData.authenticationMethod,
              JSON_AUTHENTICATION_METHOD, allocator);

    if (authSubscriptionStaticData.encPermanentKey.has_value()) {
      strToJson(data, authSubscriptionStaticData.encPermanentKey.value(),
                JSON_ENC_PERMANENT_KEY, allocator);
    }
    if (authSubscriptionStaticData.authenticationManagementField.has_value()) {
      strToJson(
          data,
          authSubscriptionStaticData.authenticationManagementField.value(),
          JSON_AUTHENTICATION_MANAGEMENT_FIELD, allocator);
    }
    if (authSubscriptionStaticData.algorithmId.has_value()) {
      strToJson(data, authSubscriptionStaticData.algorithmId.value(),
                JSON_ALGORITHM_ID, allocator);
    }
    if (authSubscriptionStaticData.a4KeyInd.has_value()) {
      strToJson(data, authSubscriptionStaticData.a4KeyInd.value(),
                JSON_A4_KEY_IND, allocator);
    }
    if (authSubscriptionStaticData.a4Ind.has_value()) {
      strToJson(data, authSubscriptionStaticData.a4Ind.value(), JSON_A4_IND,
                allocator);
    }
    if (authSubscriptionStaticData.encOpcKey.has_value()) {
      strToJson(data, authSubscriptionStaticData.encOpcKey.value(),
                JSON_ENC_OPC_KEY, allocator);
    }
    if (authSubscriptionStaticData.a4KeyV.has_value()) {
      strToJson(data, authSubscriptionStaticData.a4KeyV.value(), JSON_A4_KEY_V,
                allocator);
    }
    if (authSubscriptionStaticData.akaAlgorithmInd.has_value()) {
      strToJson(data, authSubscriptionStaticData.akaAlgorithmInd.value(),
                JSON_AKA_ALGORITHM_IND, allocator);
    }
  }
}

void ValidatorRapidJsonEncoder::authSubscriptionDynamicDataToJson(
    rapidjson::Value& data,
    const entities::auth_subscription_t& authSubscription,
    rapidjson::Document::AllocatorType& allocator) {
  if (authSubscription.authSubscriptionDynamicData.has_value()) {
    auto authSubscriptionDynamicData =
        authSubscription.authSubscriptionDynamicData.value();

    if (authSubscriptionDynamicData.sqnScheme.has_value()) {
      strToJson(data, authSubscriptionDynamicData.sqnScheme.value(),
                JSON_SQN_SCHEME, allocator);
    }

    if (authSubscriptionDynamicData.sqn.has_value()) {
      strToJson(data, authSubscriptionDynamicData.sqn.value(), JSON_SQN,
                allocator);
    }

    rapidjson::Value lastIndexesList(rapidjson::kObjectType);

    if (authSubscriptionDynamicData.lastIndexesList.has_value()) {
      for (entities::last_indexes_list_t::iterator indIt =
               authSubscriptionDynamicData.lastIndexesList.value().begin();
           indIt != authSubscriptionDynamicData.lastIndexesList.value().end();
           ++indIt) {
        rapidjson::Value key(indIt->first.c_str(), allocator);
        lastIndexesList.AddMember(key, rapidjson::Value().SetInt(indIt->second),
                                  allocator);
      }
    }

    data.AddMember(rapidjson::StringRef(JSON_LAST_INDEXES_LIST),
                   lastIndexesList, allocator);
  }
}

std::ostringstream ValidatorRapidJsonEncoder::validatorResponseToJson(
    const ::entities::ValidationData& data) {
  rJsonDoc.SetObject();
  rapidjson::Document::AllocatorType& allocator = rJsonDoc.GetAllocator();

  // Errors
  if (data.response.errors.size()) {
    rapidjson::Value errors(rapidjson::kArrayType);

    for (const auto& elem : data.response.errors) {
      rapidjson::Value error(rapidjson::kObjectType);
      rapidjson::Value keyErrorMessage(JSON_ERROR_MESSAGE, allocator);
      rapidjson::Value valueErrorMessage(elem.errorMessage.c_str(), allocator);
      error.AddMember(keyErrorMessage, valueErrorMessage, allocator);

      rapidjson::Value details(rapidjson::kObjectType);
      for (auto const& it : elem.errorDetails) {
        rapidjson::Value key(it.first.c_str(), allocator);
        rapidjson::Value val(it.second.c_str(), allocator);
        details.AddMember(key, val, allocator);
      }
      error.AddMember(rapidjson::StringRef(JSON_ERROR_DETAILS), details,
                      allocator);

      errors.PushBack(error, allocator);
    }

    rJsonDoc.AddMember(rapidjson::StringRef(JSON_ERRORS), errors, allocator);
  } else {  // Changes
    if (data.response.changes.size()) {
      rapidjson::Value changes(rapidjson::kArrayType);

      for (const auto& elem : data.response.changes) {
        rapidjson::Value change(rapidjson::kObjectType);
        rapidjson::Value keyOperation(JSON_OPERATION, allocator);
        auto op = elem.operation;
        rapidjson::Value valueOperation(op.c_str(), allocator);
        change.AddMember(keyOperation, valueOperation, allocator);

        rapidjson::Value keyResourcePath(JSON_RESOURCE_PATH, allocator);
        rapidjson::Value valueResourcePath(elem.resourcePath.c_str(),
                                           allocator);
        change.AddMember(keyResourcePath, valueResourcePath, allocator);

        std::size_t resourceAuthSubscription =
            elem.resourcePath.find(JSON_AUTH_SUBSCRIPTION);

        if (op.compare(JSON_OPERATION_DELETE)) {
          rapidjson::Value dataJson(rapidjson::kObjectType);

          if (resourceAuthSubscription != std::string::npos) {
            authSubscriptionStaticDataToJson(dataJson, elem.authSubscription,
                                             allocator);
          }

          change.AddMember(rapidjson::StringRef(JSON_DATA), dataJson,
                           allocator);
        }

        changes.PushBack(change, allocator);

        if (not op.compare(JSON_OPERATION_CREATE)) {
          rapidjson::Value changeDyn(rapidjson::kObjectType);
          rapidjson::Value keyDynOperation(JSON_OPERATION, allocator);
          rapidjson::Value valueDynOperation(op.c_str(), allocator);

          changeDyn.AddMember(keyDynOperation, valueDynOperation, allocator);

          rapidjson::Value keyDynResourcePath(JSON_RESOURCE_PATH, allocator);
          auto dynResourcePath =
              entities::ValidationData::createPathFromBasePath(
                  elem.resourcePath, JSON_AUTH_SUBSCRIPTION_DYNAMIC_DATA);
          rapidjson::Value valueDynResourcePath(dynResourcePath.c_str(),
                                                allocator);
          changeDyn.AddMember(keyDynResourcePath, valueDynResourcePath,
                              allocator);
          rapidjson::Value dataJson(rapidjson::kObjectType);
          if (resourceAuthSubscription != std::string::npos) {
            authSubscriptionDynamicDataToJson(dataJson, elem.authSubscription,
                                              allocator);
          }
          changeDyn.AddMember(rapidjson::StringRef(JSON_DATA), dataJson,
                              allocator);

          changes.PushBack(changeDyn, allocator);
        }
      }

      rJsonDoc.AddMember(rapidjson::StringRef(JSON_CHANGES), changes,
                         allocator);
    }
  }

  std::ostringstream os;
  if (!rJsonDoc.IsNull()) {
    acceptWriter(os);
  } else {
    os << "{}";
  }

  return os;
}

std::ostringstream ValidatorRapidJsonEncoder::errorResponseToJson(
    const ::entities::Error& err) {
  rJsonDoc.SetObject();
  rapidjson::Document::AllocatorType& allocator = rJsonDoc.GetAllocator();

  rapidjson::Value keyErrorMessage(JSON_ERROR_MESSAGE, allocator);
  rapidjson::Value valueErrorMessage(err.errorMessage.c_str(), allocator);
  rJsonDoc.AddMember(keyErrorMessage, valueErrorMessage, allocator);

  rapidjson::Value details(rapidjson::kObjectType);
  for (auto const& it : err.errorDetails) {
    rapidjson::Value key(it.first.c_str(), allocator);
    rapidjson::Value val(it.second.c_str(), allocator);
    details.AddMember(key, val, allocator);
  }
  rJsonDoc.AddMember(rapidjson::StringRef(JSON_ERROR_DETAILS), details,
                     allocator);

  std::ostringstream os;
  if (!rJsonDoc.IsNull()) {
    acceptWriter(os);
  } else {
    os << "{}";
  }
  return os;
}

void ValidatorRapidJsonEncoder::strToJson(
    rapidjson::Value& data, const std::string& str, const std::string& keyName,
    rapidjson::Document::AllocatorType& allocator) {
  if (not str.empty()) {
    rapidjson::Value key(keyName.c_str(), allocator);
    rapidjson::Value val(str.c_str(), allocator);
    data.AddMember(key, val, allocator);
  }
}

void ValidatorRapidJsonEncoder::acceptWriter(std::ostringstream& os) const {
  rapidjson::StringBuffer buffer;
  if (prettyFormat) {
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    rJsonDoc.Accept(writer);
  } else {
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    rJsonDoc.Accept(writer);
  }
  os << buffer.GetString();
}

rapidjson::Document ValidatorRapidJsonEncoder::getVendorSpecific(
    const rapidjson::Value& val,
    rapidjson::Document::AllocatorType& allocator) {
  rapidjson::Document doc;
  doc.CopyFrom(val, allocator);
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  if (!doc.IsNull()) {
    doc.Accept(writer);
  }
  return doc;
}

void ValidatorRapidJsonEncoder::vendorSpecificToJson(
    rapidjson::Value& data, const entities::vendorSpecific_t& vendorSpecific,
    rapidjson::Document::AllocatorType& allocator) {
  for (auto const& [key, val] : vendorSpecific) {
    data.AddMember(rapidjson::Value(key.c_str(), allocator).Move(),
                   getVendorSpecific(val, allocator), allocator);
  }
}

}  // namespace json
}  // namespace secondary
}  // namespace port
