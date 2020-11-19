cmake_minimum_required(VERSION 3.9)
project(json VERSION 1.0.0 DESCRIPTION "Json Library")
include(GNUInstallDirs)
add_library(json SHARED json.c)
set(MY_PUBLIC_HEADERS 
    ${PROJECT_SOURCE_DIR}/json.h
)
set_target_properties(json PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${MY_PUBLIC_HEADERS}")
configure_file(json.pc.in json.pc @ONLY)
target_include_directories(json PRIVATE .)
install(TARGETS json
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/json.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
