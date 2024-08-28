cmake_minimum_required(VERSION 3.0.1)

hss_add_lib(
        entities
        SRC
                ValidationData.cpp
                Context.cpp
        INCLUDE
                ${BASE_INCLUDES}
        STATIC
)
