cmake_minimum_required(VERSION 2.6)

project(GREENCOAP)

set(CMAKE_BUILD_TYPE Release)
set(BUILD_SHARED_LIBS On)
set(CMAKE_C_FLAGS_RELEASE "-Wall -O2")
set(CMAKE_C_FLAGS_DEBUG "-Wall -O0 -g")

set(GREENCOAP_INCLUDE ${GREENCOAP_SOURCE_DIR})

# libgreencoap
include_directories(${GREENCOAP_INCLUDE} .)
set(GREENCOAP_HEADER ${GREENCOAP_INCLUDE}/greencoap.h)
add_library(greencoap greencoap.c)
target_link_libraries(greencoap)
set_target_properties(greencoap PROPERTIES VERSION 0.0.1 SOVERSION 1)
install(TARGETS greencoap LIBRARY DESTINATION lib)
install(FILES ${GREENCOAP_HEADER} DESTINATION include)

# test
add_executable(greencoap_test test.c)
target_link_libraries(greencoap_test greencoap)
install(TARGETS greencoap_test RUNTIME DESTINATION bin)
