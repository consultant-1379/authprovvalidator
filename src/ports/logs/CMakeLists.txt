cmake_minimum_required(VERSION 3.0.1)

hss_add_lib(
    logwrapper
    SRC
      logwrapper.cpp
    INCLUDE
      ${BASE_INCLUDES}
    STATIC
)