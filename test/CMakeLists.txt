cmake_minimum_required(VERSION 3.0.1)

hss_add_unit_test(
    test_authenticationprovisioningvalidator_ut
    SRC
      test_entity_queue.cpp
      test_envhandler.cpp
      test_validator_server.cpp
      test_rapidjsonparser.cpp
      test_rapidjsonencoder.cpp
      test_validationdata.cpp
      test_anonymouslogs.cpp
    INCLUDE
      ${PROJECT_SOURCE_DIR}/src/
      ${PROJECT_BINARY_DIR}/src/
    PUBLIC
      serverport
      logwrapper
      validation
      entities
      cpph2
      jsonport
      codec
      gtest
      gmock
      log
      pthread
      nghttp2_asio
      nghttp2
      boost_system
      boost_regex
      ssl
      crypto
)
