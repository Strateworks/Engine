# Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
# All rights reserved.

cmake_minimum_required(VERSION 3.30)
project(server)

set(CMAKE_CXX_STANDARD 23)
option(ENABLE_TESTS "Build tests" ON)
option(ENABLE_DEBUG "Enable debug" ON)
option(ENABLE_STATIC_LINKING "Enable static linking" OFF)
option(ENABLE_NATIVE_OPTIMIZATION "Enable native CPU optimization" OFF)
option(ENABLE_CI "Enable CI settings" OFF)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g --coverage -fprofile-arcs -ftest-coverage")

if (ENABLE_DEBUG)
    add_definitions(-DDEBUG_ENABLED)
endif ()

if (ENABLE_STATIC_LINKING)
    add_definitions(-DSTATIC_ENABLED)
    set(CURL_USE_STATIC_LIBS ON)
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_STATIC_RUNTIME ON)
    set(BOOST_ALL_NO_LIB ON)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
    set(ZLIB_USE_STATIC_LIBS ON)
    set(OPENSSL_USE_STATIC_LIBS TRUE)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

    if (ENABLE_CI)
        set(OPENSSL_CRYPTO_LIBRARY "/usr/lib/libcrypto.a")
        set(OPENSSL_SSL_LIBRARY "/usr/lib/libssl.a")
        set(CURL_LIBRARY "/usr/lib/libcurl.a")
        set(ZLIB_LIBRARY "/usr/lib/libz.a")
        set(NGHTTP2_LIBRARY "/usr/lib/libnghttp2.a")
    else ()
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
            set(ARCH_LIB_DIR "/usr/lib/aarch64-linux-gnu")
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
            set(ARCH_LIB_DIR "/usr/lib/x86_64-linux-gnu")
        else()
            message(FATAL_ERROR "Arquitectura no soportada: ${CMAKE_SYSTEM_PROCESSOR}")
        endif()
        set(OPENSSL_CRYPTO_LIBRARY "${ARCH_LIB_DIR}/libcrypto.a")
        set(OPENSSL_SSL_LIBRARY "${ARCH_LIB_DIR}/libssl.a")
        set(CURL_LIBRARY "${ARCH_LIB_DIR}/libcurl.a")
        set(ZLIB_LIBRARY "${ARCH_LIB_DIR}/libz.a")
        set(NGHTTP2_LIBRARY "${ARCH_LIB_DIR}/libnghttp2.a")
    endif ()

    set(OpenSSL_USE_STATIC_LIBS TRUE)

    if (ENABLE_NATIVE_OPTIMIZATION)
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -march=native -mtune=native -g0 -static -static-libgcc")
        set(CMAKE_C_FLAGS_RELEASE "-O3 -flto -march=native -mtune=native -g0 -static -static-libgcc")
    else ()
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -g0 -static -static-libgcc")
        set(CMAKE_C_FLAGS_RELEASE "-O3 -flto -g0 -static -static-libgcc")
    endif ()
else ()
    if (ENABLE_NATIVE_OPTIMIZATION)
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -march=native -mtune=native -g0")
        set(CMAKE_C_FLAGS_RELEASE "-O3 -flto -march=native -mtune=native -g0")
    else ()
        set(CMAKE_CXX_FLAGS_RELEASE "-O3")
        set(CMAKE_C_FLAGS_RELEASE "-O3")
    endif ()
endif ()

if (ENABLE_DEBUG)
    add_definitions(-DDEBUG_ENABLED)
endif ()

find_package(Boost REQUIRED COMPONENTS program_options json charconv)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(OpenSSL REQUIRED)

file(GLOB_RECURSE STATE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/objects/*.cpp")

include_directories(src/include)

add_library(objects OBJECT ${STATE_SOURCES})

target_link_libraries(objects PRIVATE
        ${OPENSSL_SSL_LIBRARY}
        ${OPENSSL_CRYPTO_LIBRARY}
        ${Boost_LIBRARIES}
        spdlog::spdlog
        fmt::fmt
)

add_library(dependencies OBJECT deps/asio.cxx deps/beast.cxx)

target_link_libraries(dependencies PRIVATE
        ${OPENSSL_SSL_LIBRARY}
        ${OPENSSL_CRYPTO_LIBRARY}
        ${Boost_LIBRARIES})

add_executable(server main.cpp)
if (ENABLE_STATIC_LINKING)
    target_compile_options(server PRIVATE -static -static-libgcc -static-libstdc++)
    target_link_options(server PRIVATE -static -static-libgcc -static-libstdc++)
endif()

target_link_libraries(server PRIVATE objects
        dependencies
        ${Boost_LIBRARIES}
        spdlog::spdlog
        fmt::fmt
)

if (ENABLE_TESTS)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    file(GLOB_RECURSE STATE_TESTS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cc")

    add_executable(tests ${STATE_TESTS})

    target_link_libraries(tests
            PRIVATE
            objects
            dependencies
            GTest::gtest_main
            ${OPENSSL_SSL_LIBRARY}
            ${OPENSSL_CRYPTO_LIBRARY}
            ${Boost_LIBRARIES}
            spdlog::spdlog
            fmt::fmt
    )

    if (ENABLE_STATIC_LINKING)
        target_compile_options(tests PRIVATE -static -static-libgcc -static-libstdc++)
        target_link_options(tests PRIVATE -static -static-libgcc -static-libstdc++)
    endif()

    include(GoogleTest)
    gtest_discover_tests(tests)
endif()