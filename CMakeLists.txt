cmake_minimum_required(VERSION 2.8.3)

project(kissc C)

option(UT "enable unit tests" ON)

set(SOURCES
    iterator/iterator.c
#     hashtable/hashtable.c
    dynamic_arrays/darray.c
    string/strcmp_l.c string/parsenum.c
)
add_library(kissc SHARED ${SOURCES})

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/cpp/
    ${CMAKE_SOURCE_DIR}/dynamic_arrays/
    ${CMAKE_SOURCE_DIR}/iterator/
    ${CMAKE_SOURCE_DIR}/hashtable/
    ${CMAKE_SOURCE_DIR}/string/
)

if(UT)
    file(GLOB UNITY_SRC unity/*.c)
    add_library(unity SHARED ${UNITY_SRC})

    add_executable(test_strncmp_l tests/strncmp_l.c) # unity/unity.c
    target_link_libraries(test_strncmp_l kissc unity)
    add_test("strncmp_l" test_strncmp_l)

    add_executable(test_parsenum tests/parsenum.c) # unity/unity.c
    target_link_libraries(test_parsenum kissc unity)
    add_test("parsenum" test_parsenum)

    enable_testing()
endif(UT)
