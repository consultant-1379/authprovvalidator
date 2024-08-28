cmake_minimum_required(VERSION 3.0.1)

hss_add_lib(
    oaivalidatorport
    SRC
      OaiValidator.cpp
    INCLUDE
      ${BASE_INCLUDES}
      ${OAI_INCLUDES}
    STATIC
)
