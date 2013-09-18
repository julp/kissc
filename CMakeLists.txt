cmake_minimum_required(VERSION 2.8.3)

project(kissc C)

option(UT "enable unit tests" ON)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
)

set(SOURCES string/strcmp_l.c)
add_library(kissc SHARED ${SOURCES})

if(UT)
    file(GLOB UNITY_SRC unity/*.c)
    add_library(unity SHARED ${UNITY_SRC})

    add_executable(test_strncmp_l unity/unity.c tests/strncmp_l.c)
    target_link_libraries(test_strncmp_l kissc)
    add_test("strncmp_l" test_strncmp_l)

    enable_testing()
endif(UT)
