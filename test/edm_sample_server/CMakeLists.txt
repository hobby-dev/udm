cmake_minimum_required(VERSION 3.5)
project(edm_sample_server VERSION 0.1.0 LANGUAGES C)

# set build type
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug
        CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

# set clang/gcc options
if (${CMAKE_C_COMPILER_ID} MATCHES GNU OR ${CMAKE_C_COMPILER_ID} MATCHES Clang)
    set(CMAKE_C_FLAGS         "-fstack-protector -fstack-protector-all -Werror -Wall" )
    set(CMAKE_C_FLAGS_DEBUG   "-O2 -ggdb" )
    set(CMAKE_C_FLAGS_RELEASE "-Os" )
endif (${CMAKE_C_COMPILER_ID} MATCHES GNU OR ${CMAKE_C_COMPILER_ID} MATCHES Clang)

# add sources
add_subdirectory(src bin)

