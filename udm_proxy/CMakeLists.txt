cmake_minimum_required(VERSION 3.5)
project(udm_proxy VERSION 0.1.0 LANGUAGES C)

# set build type
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif (NOT CMAKE_BUILD_TYPE)

# set clang/gcc options
if (${CMAKE_C_COMPILER_ID} MATCHES GNU OR ${CMAKE_C_COMPILER_ID} MATCHES Clang)
    set(CMAKE_C_FLAGS         "-fstack-protector -fstack-protector-all -Wall -Wpedantic -Wextra -Werror -Wno-missing-field-initializers" )
    set(CMAKE_C_FLAGS_DEBUG   "-O2 -ggdb" )
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "-Os -ggdb" )
    set(CMAKE_C_FLAGS_RELEASE "-Os" )
endif (${CMAKE_C_COMPILER_ID} MATCHES GNU OR ${CMAKE_C_COMPILER_ID} MATCHES Clang)

# add sources
add_subdirectory(src bin)

