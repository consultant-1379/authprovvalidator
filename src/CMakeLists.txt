cmake_minimum_required(VERSION 3.0.1)
set(BASE_INCLUDES ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(ports)
add_subdirectory(domain)
add_subdirectory(entities)

hss_add_microservice(
    Authenticationprovisioningvalidator
    SRC
        main.cpp
    INCLUDE
        ${BASE_INCLUDES}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${HTTP2_INCLUDES}
        ${MONITOR_INCLUDES}
    PUBLIC
        serverport
        validation
        oaivalidatorport
        openapi3
        entities
        log
        jsonport
        logwrapper
        codec
        cpph2
        cppmonitor
        nghttp2_asio
        nghttp2
        boost_system
        boost_regex
        ssl
        crypto
        yaml-cpp
)
