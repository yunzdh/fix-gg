cmake_minimum_required(VERSION 3.31)

if (NOT DEFINED CMAKE_ANDROID_NDK)
    set(CMAKE_ANDROID_NDK D:/Android/SDK/ndk/29.0.13113456)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake)
    set(CMAKE_SYSTEM_NAME Android)
    set(CMAKE_SYSTEM_VERSION 24)
    set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(ANDROID_ABI arm64-v8a)
    set(ANDROID_PLATFORM android-28)
    set(ANDROID_NDK ${CMAKE_ANDROID_NDK})
else ()
    SET(ANDROID_STUDIO_BUILD true)
endif ()

project(fix_gg)

set(CMAKE_CXX_STANDARD 20)

add_library(fix_gg SHARED library.cpp)

target_link_libraries(fix_gg
        log
)
