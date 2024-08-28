# eric-udm-authprovvalidator 1.59.0 ReleaseNotes

[TOC]

## Functional content

- 5G authentication subscription validation for LA UDM NF of 2019 Q4
- 5G authentication subscription validation for an existing legacy AUC subscriber
- https communication is not supported
- Support only for HTTP/2
- V-API version supported is 0.0.2
- P-API version supported is 1.6.0
- Support for create, update and delete operation
- TZ parameter available for setting timezone (UTC by default)
- Logging level can be edited via configmap
- Support to IPv6
- Reply with 422 error code when related resources required for validation are missing
- Compliant with authentication rules document version PA7
- Added mechanism for overload protection
- Mutation that initializes lastIndexes attribute to empty object as part of AuthSubscriptionDynamicData
- Attributes for legacy subscribers, when encoded in base64, are indicated by adding a colon at the end of the attribute name in the JSON request
- Fixes to improve performance
- Adjust number of replicas to 2, enable Kubernetes taint based evictions option and equal requests and limits resources.
- Removed unusued libraries
- Update NF name in logs to Provisioning Function
- Add nodeSelector
- Add annotations in logging config map
- Add CXC product information
- Fixed DR-HC-120 for logging config-map
- Update memory limit and request to 20Mi for healthproxy
- Changed nodeSelector path to activation
- Fixed issue with timezone selection
- Updated 3pp libraries to latest versions
- Updated healthproxy version
- Fix on algorithm for octet reordering on SEQHE attribute
- Updated 3pp libraries to latest versions
- Enabled crash dumps collections by mounting /corefile under an emptyDir volume
- Updated 3pp libraries to latest versions
 Added additional requirements:
    - Operation and resource path must be strings. Otherwise, the service will return HTTP 400.
    - Operation must be either CREATE, UPDATE or DELETE. Otherwise, the service will return HTTP 400.
    - If the resource path for CREATE or UPDATE operations is not for an expected resource, the service will return HTTP 422.
    - For DELETE operations, any resource path is allowed and the service will always return HTTP 200.
- Log anonymization of sensitive information (user identities).
- [HY65156](https://mhweb.ericsson.se/TREditWeb/faces/oo/object.xhtml?eriref=HY65156): The format of the attribute authenticationManagementField returned from Authentication Validator does not match with the defined pattern in convergent AUC-ARPF interface.
- Fixed some security issues and changed fsGroup to 1000.
- [UDM5GP-20916](https://eteamproject.internal.ericsson.com/browse/UDM5GP-20916) Fixed image pull policy configuration
- Updated maxUnavailable value for rollingUpdate.
- Support for vendor specific attributes.
- Updated 3pp libraries and healthproxy sidecar to latest versions.
- [UDM5GP-23850](https://eteamproject.internal.ericsson.com/browse/UDM5GP-23850) eric-udm-authprovvalidator not ready after worker reboot
- Support for A4Key change feature
- Support for MRD Vodafone feature
- [UDM5GP-34850](https://eteamproject.internal.ericsson.com/browse/UDM5GP-34850) akaAlgorithmInd: Fix when imsiMask in provisioning journal is empty
- Updated 3pp librares and healthproxy sidecar to latest versions.

## Deliverables

### Helm Chart

| Product Number | Chart Name | Version | Description |
|--------------|------------|---------|-------------|
| CXD 101 0531 | eric-udm-authprovvalidator | [1.3.28](https://arm.lmera.ericsson.se/artifactory/proj-5g-udm-dev-helm/eric-udm-authprovvalidator-1.3.28.tgz) | Authentication subscription provisioning validator microservice deployment |

### Docker Images

| Product Number | Image Name | Version | Gerrit commit |
|--------------|------------|---------|-------------|
| CXU 101 0356 | eric-ccdmeda-authprovvalidator | [1.3.28](https://arm.epk.ericsson.se/artifactory/docker-v2-global-local/proj_hss/5g/docker_images/eric-ccdmeda-authprovvalidator/1.3.28) | [HSS/CCDM/EDA/authprovvalidator:9a084e7](https://gerrit.ericsson.se/gitweb?p=HSS/CCDM/EDA/authprovvalidator.git;a=commit;h=9a084e7) |
| CXU 101 0241 | eric-udm-healthproxy | [1.12.4](https://arm.epk.ericsson.se/artifactory/docker-v2-global-local/proj_hss/5g/docker_images/eric-udm-healthproxy/1.12.4) | [HSS/CCSM/UDM/healthproxy:a650b7e](https://gerrit.ericsson.se/gitweb?p=HSS/CCSM/UDM/healthproxy.git;a=commit;h=a650b7e) |

<style type="text/css">
/* automatic heading numbering */
  h1 { counter-reset: h2counter; }
  h2 { counter-reset: h3counter; }
  h3 { counter-reset: h4counter; }
  h4 { counter-reset: h5counter; }
  h5 { counter-reset: h6counter; }
  h6 { }
  h2:before {
    counter-increment: h2counter;
    content: counter(h2counter) ".\0000a0\0000a0";
  }
  h3:before {
    counter-increment: h3counter;
    content: counter(h2counter) "."
             counter(h3counter) ".\0000a0\0000a0";
  }
  h4:before {
    counter-increment: h4counter;
    content: counter(h2counter) "."
             counter(h3counter) "."
             counter(h4counter) ".\0000a0\0000a0";
  }
  h5:before {
    counter-increment: h5counter;
    content: counter(h2counter) "."
             counter(h3counter) "."
             counter(h4counter) "."
             counter(h5counter) ".\0000a0\0000a0";
  }
  h6:before {
    counter-increment: h6counter;
    content: counter(h2counter) "."
             counter(h3counter) "."
             counter(h4counter) "."
             counter(h5counter) "."
             counter(h6counter) ".\0000a0\0000a0";
  }
</style>
