cmake_minimum_required(VERSION 3.0.1)

hss_add_lib(
        jsonport
        SRC
                ValidatorRapidJsonParser.cpp
                ValidatorRapidJsonEncoder.cpp
        INCLUDE
                ${BASE_INCLUDES}
                ${CODEC_INCLUDES}
        STATIC
)
