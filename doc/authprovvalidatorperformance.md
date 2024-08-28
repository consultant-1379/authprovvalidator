<h1>Authentication validator stability Test</h1>

Using injector tool [injector tool](https://gerrit.ericsson.se/plugins/gitiles/HSS/CCSM/cpph2/+/master/test/injector/), a performance test has been made on the eric-udm-authprovvalidator service.
Command used: ./injector.usvc.elf -u 'http://eric-udm-authprovvalidator:9002/validation/v1/validate/validate' -m POST -f 2000 -d 3600 -j AuthValidatorExample.json -M 10
Using the following parameters:
    • Duration: 1 hour
    • Data: {
      "changes": [
          {
            "operation": "UPDATE",
            "resource_path": "/subscribers/123abc/authSubscription/imsi-123456789012345/authSubscriptionStaticData",
            "data": {
              "authenticationMethod": "EAP_AKA_PRIME",
              "encPermanentKey": "2200AA34D40C090D6D4C3B7763854AFB",
              "authenticationManagementField": "B9B9",
              "algorithmId": "11",
              "a4KeyInd": "1",
              "a4Ind": "2"
            }
          }
        ],
        "relatedResources": {
          "/subscribers/123abc/authSubscription": {
            "imsi-123456789012345": {
              "authSubscriptionStaticData": {
                "authenticationMethod": "5G_AKA",
                "encPermanentKey": "2200AA34D40C090D6D4C3B7763854AFB",
                "authenticationManagementField": "B9B9",
                "algorithmId": "11",
                "a4KeyInd": "1",
                "a4Ind": "2"
              },
              "authSubscriptionDynamicData": {
                "sqnScheme": "GENERAL",
                "sqn": "111111111111"
              }
            }
          }
        }
      }
    • fixed Rate: 2000 reqs/s
    • number of parallel streams: 10

Results:

Results of the uservice with log level at “info”:
------------------------------ params ------------------------------
URI                 :http://eric-udm-authprovvalidator:9002/validation/v1/validate/validate
Method              :POST
JSON file           :AuthValidatorExample.json
duration(sg)        :3600
rate(reqs./sg)      :2000
sleep time(us)      :500
max parallel streams:10
stuck timeout(sg)   :1
------------------------------ params ------------------------------
current rate: 002000.79 reqs/sg.
  sending complete!!!
reception complete!!!
------------------------------ stats -------------------------------
  elapsed req ms:  3600006
  elapsed rsp ms:  3600006
mean time req ns:   499.80
mean time rsp ns:   499.80
       (req)./sg:  2000.79
 (req<=>rsp)./sg:  2000.79
 
total rsp:7202841 err:0(0.00%) 2xx:7202841(100.00%) 4xx:0(0.00%) 500:0(0.00%) 501:0(0.00%) 502:0(0.00%) 503:0(0.00%) 504:0(0.00%)
 
mean per response type latency (ms) / standard deviation (ms) :
 2xx:     0.51645 / sd:     2.19261
 4xx:           - / sd:           -
 500:           - / sd:           -
 501:           - / sd:           -
 502:           - / sd:           -
 503:           - / sd:           -
 504:           - / sd:           -
 
mean per response type database latency (ms) / standard deviation (ms) :
 2xx:     0.00000 / sd:     0.00000
 4xx:           - / sd:           -
 500:           - / sd:           -
 
------------------------------ stats -------------------------------

Results with log level “debug”:
------------------------------ params ------------------------------
URI                 :http://eric-udm-authprovvalidator:9002/validation/v1/validate/validate
Method              :POST
JSON file           :authValidatorExample.json
duration(sg)        :3600
rate(reqs./sg)      :2000
sleep time(us)      :500
max parallel streams:10
stuck timeout(sg)   :1
------------------------------ params ------------------------------
current rate: 002000.95 reqs/sg.
  sending complete!!!
reception complete!!!
------------------------------ stats -------------------------------
  elapsed req ms:  3600351
  elapsed rsp ms:  3600351
mean time req ns:   499.76
mean time rsp ns:   499.76
       (req)./sg:  2000.95
 (req<=>rsp)./sg:  2000.95

total rsp:7204108 err:0(0.00%) 2xx:7202794(99.98%) 4xx:0(0.00%) 500:0(0.00%) 501:0(0.00%) 502:0(0.00%) 503:1314(0.02%) 504:0(0.00%)

mean per response type latency (ms) / standard deviation (ms) :
 2xx:     1.38797 / sd:     3.71718
 4xx:           - / sd:           -
 500:           - / sd:           -
 501:           - / sd:           -
 502:           - / sd:           -
 503:     0.82223 / sd:     0.85599
 504:           - / sd:           -

mean per response type database latency (ms) / standard deviation (ms) :
 2xx:     0.00000 / sd:     0.00000
 4xx:           - / sd:           -
 500:           - / sd:           -

Problem Details:
----------------
503:
 Requests:[000001314]; Title:[The NF experiences congestion and performs overload control, which does not allow the request to be processed.]

------------------------------ stats -------------------------------

The specifications of the machine where the test was executed are:

8 cpus Intel(R) Xeon(R) Gold 6132 CPU @ 2.60GHz
32 GB of RAM
