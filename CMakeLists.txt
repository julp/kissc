cmake_minimum_required(VERSION 2.8.3)

project(kissc C)

if(CMAKE_VERSION VERSION_GREATER "3.1.0")
    set(CMAKE_C_STANDARD 99)
    set(CMAKE_C_STANDARD_REQUIRED TRUE)
else(CMAKE_VERSION VERSION_GREATER "3.1.0")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99") # "poor" fallback
endif(CMAKE_VERSION VERSION_GREATER "3.1.0")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wwrite-strings -Wstrict-prototypes -Wuninitialized -Wunreachable-code -Wno-comment -Wnonnull -Wunreachable-code")

option(UT "enable unit tests" ON)

include(CheckFunctionExists)
check_function_exists("reallocarray" HAVE_REALLOCARRAY)

if(HAVE_REALLOCARRAY)
    # TODO: use it
    # TODO: better to do this with configure_file
    add_definitions(-DHAVE_REALLOCARRAY)
endif(HAVE_REALLOCARRAY)

# configure_file(
#     "config.h.in"
#     "config.h"
#     @ONLY
# )

set(SOURCES
    iterator/iterator.c
    rbtree/rbtree.c
    hashtable/hashtable.c
    dynamic_arrays/darray.c dynamic_arrays/dptrarray.c
    unicode/utf8.c unicode/nfd.c
    string/ascii_case.c string/memcmp_l.c string/memncmp_l.c string/memstr.c string/parsenum.c string/strcpy_s.c string/strcat_s.c string/stpcpy_s.c string/stpncpy_s.c
)
add_library(kissc SHARED ${SOURCES})

add_executable(nfd bin/nfd.c)
target_link_libraries(nfd kissc)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/public/
    ${CMAKE_SOURCE_DIR}/private/
)

if(UT)
    file(GLOB UNITY_SRC unity/*.c)
    add_library(unity SHARED ${UNITY_SRC})

    add_executable(test_memncmp_l tests/memncmp_l.c) # unity/unity.c
    target_link_libraries(test_memncmp_l kissc unity)
    add_test("memncmp_l" test_memncmp_l)

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
