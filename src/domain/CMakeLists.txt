cmake_minimum_required(VERSION 3.0.1)

hss_add_lib(
    validation
    SRC
        validation.cpp
        ports.cpp
    INCLUDE
        ${BASE_INCLUDES}
        ${HTTP2_INCLUDES}
        ${OAI_INCLUDES}
        ${LOG_INCLUDES}
    STATIC
)
