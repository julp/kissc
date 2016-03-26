cmake_minimum_required(VERSION 2.8.3)

project(kissc C)

option(UT "enable unit tests" ON)

set(SOURCES
    iterator/iterator.c
#     hashtable/hashtable.c
    dynamic_arrays/darray.c
    unicode/utf8.c unicode/nfd.c
    string/strcmp_l.c string/parsenum.c string/strcpy_s.c string/strcat_s.c
)
add_library(kissc SHARED ${SOURCES})

add_executable(nfd bin/nfd.c)
target_link_libraries(nfd kissc)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/cpp/
    ${CMAKE_SOURCE_DIR}/public/
    ${CMAKE_SOURCE_DIR}/private/
)

if(UT)
    file(GLOB UNITY_SRC unity/*.c)
    add_library(unity SHARED ${UNITY_SRC})

    add_executable(test_strncmp_l tests/strncmp_l.c) # unity/unity.c
    target_link_libraries(test_strncmp_l kissc unity)
    add_test("strncmp_l" test_strncmp_l)

    add_executable(test_strcpy_s tests/strcpy_s.c) # unity/unity.c
    target_link_libraries(test_strcpy_s kissc unity)
    add_test("strcpy_s" test_strcpy_s)

    add_executable(test_strcat_s tests/strcat_s.c) # unity/unity.c
    target_link_libraries(test_strcat_s kissc unity)
    add_test("strcat_s" test_strcat_s)

    add_executable(test_parsenum tests/parsenum.c) # unity/unity.c
    target_link_libraries(test_parsenum kissc unity)
    add_test("parsenum" test_parsenum)

    add_executable(test_nfd tests/nfd.c) # unity/unity.c
    target_link_libraries(test_nfd kissc)# unity)
    add_test("nfd" test_nfd)

    add_executable(test_utf8 tests/utf8.c) # unity/unity.c
    target_link_libraries(test_utf8 kissc)# unity)
    add_test("utf8" test_utf8)

    enable_testing()
endif(UT)
