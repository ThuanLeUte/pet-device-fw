set (ALL_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/sys
    ${CMAKE_SOURCE_DIR}/components/platform
    ${CMAKE_SOURCE_DIR}/components/rfal/include
)

idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ${ALL_INCLUDE_DIRS})