# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.22)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
project(msvc_standard_libraries LANGUAGES CXX)

include(CheckCXXSourceCompiles)
check_cxx_source_compiles([=[
#include <sdkddkver.h>
static_assert(WDK_NTDDI_VERSION >= NTDDI_WIN10_CO, "Inspecting WDK_NTDDI_VERSION, the Windows SDK version.");
int main() {}
]=] WINDOWS_SDK_VERSION_CHECK)
if(NOT WINDOWS_SDK_VERSION_CHECK)
    message(FATAL_ERROR "The STL must be built with the Windows 11 SDK (10.0.22000.0) or later. Make sure it's available by selecting it in the Individual Components tab of the VS Installer.")
endif()

if(NOT DEFINED VCLIBS_TARGET_ARCHITECTURE)
    set(VCLIBS_TARGET_ARCHITECTURE "${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}")
endif()
string(TOLOWER "${VCLIBS_TARGET_ARCHITECTURE}" VCLIBS_TARGET_ARCHITECTURE)

if(VCLIBS_TARGET_ARCHITECTURE MATCHES "^x64$|^x86$")
    enable_language(ASM_MASM)
endif()

# add the tools subdirectory _before_ we change all the flags
add_subdirectory(tools EXCLUDE_FROM_ALL)
# these allow the targets to show up in the top-level
# (as opposed to under the tools subdirectory)
if(TARGET run-format)
    add_custom_target(format)
    add_dependencies(format run-format)
endif()
if(TARGET run-validate)
    add_custom_target(validate)
    add_dependencies(validate run-validate)
endif()

option(BUILD_TESTING "Enable testing" ON)
set(VCLIBS_SUFFIX "_oss" CACHE STRING "suffix for built DLL names to avoid conflicts with distributed DLLs")

set(CMAKE_CXX_FLAGS "")
set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_CXX_FLAGS_RELEASE "")
set(CMAKE_CXX_STANDARD_LIBRARIES "kernel32.lib")
set(CMAKE_CXX_STANDARD_LIBRARIES_INIT "kernel32.lib")
set(CMAKE_MSVC_RUNTIME_LIBRARY "")

if("${VCLIBS_TARGET_ARCHITECTURE}" MATCHES "^x86$")
    set(VCLIBS_TARGET_ARCHITECTURE "x86")
    set(VCLIBS_I386_OR_AMD64 "i386")
    set(VCLIBS_X86_OR_X64 "x86")
    # Note that we set _WIN32_WINNT to a high level to make declarations available, but still engage downlevel
    # runtime dynamic linking by setting our own _STL_WIN32_WINNT back to Windows XP.
    add_compile_definitions(_X86_ _VCRT_WIN32_WINNT=0x0501 _STL_WIN32_WINNT=0x0501)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/arch:IA32>)
elseif(VCLIBS_TARGET_ARCHITECTURE MATCHES "^x64$")
    set(VCLIBS_TARGET_ARCHITECTURE "x64")
    set(VCLIBS_I386_OR_AMD64 "amd64")
    set(VCLIBS_X86_OR_X64 "x64")
    add_compile_definitions(_AMD64_ _VCRT_WIN32_WINNT=0x0501 _STL_WIN32_WINNT=0x0501)
elseif(VCLIBS_TARGET_ARCHITECTURE MATCHES "^armv7$")
    set(VCLIBS_TARGET_ARCHITECTURE "arm")
    set(VCLIBS_I386_OR_AMD64 "arm")
    set(VCLIBS_X86_OR_X64 "arm")
    add_compile_definitions(_ARM_ _VCRT_WIN32_WINNT=0x0602 _STL_WIN32_WINNT=0x0602)
    string(APPEND CMAKE_CXX_STANDARD_LIBRARIES " Synchronization.lib")
elseif(VCLIBS_TARGET_ARCHITECTURE MATCHES "^arm64$")
    set(VCLIBS_TARGET_ARCHITECTURE "arm64")
    set(VCLIBS_I386_OR_AMD64 "arm64")
    set(VCLIBS_X86_OR_X64 "arm64")
    add_compile_definitions(_ARM64_ _VCRT_WIN32_WINNT=0x0A00 _STL_WIN32_WINNT=0x0A00)
    string(APPEND CMAKE_CXX_STANDARD_LIBRARIES " Synchronization.lib")
else()
    message(FATAL_ERROR "Could not determine target architecture: VCLIBS_TARGET_ARCHITECTURE: ${VCLIBS_TARGET_ARCHITECTURE}")
endif()

add_compile_definitions(
    _ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH WIN32_LEAN_AND_MEAN STRICT _CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS
    _CRT_DECLARE_NONSTDC_NAMES=1 _WIN32_WINNT=0x0A00 NTDDI_VERSION=NTDDI_WIN10_CO)

add_compile_options(/WX /Gy
    "$<$<COMPILE_LANGUAGE:CXX>:/diagnostics:caret;/W4;/w14265;/w15038;/d1FastFail;/guard:cf;/Z7;/Zp8;/std:c++latest;/permissive-;/Zc:threadSafeInit-;/Zl>"
    # note that /Zi generates debug info inside the object file, it's the same as /Z7 for msvc
    "$<$<COMPILE_LANGUAGE:ASM_MASM>:/Zi;/W3;/nologo>")

set(VCLIBS_DEBUG_OPTIONS "$<$<COMPILE_LANGUAGE:CXX>:/Od>")

# TRANSITION: Potentially remove `/Os` if it results in speed improvements.
# This requires benchmarking!
# Note that in MSVC version 19.32.31328,
# `/Os` results in a binary size difference of 102K
# in the release DLL (a gain of 18.4%).
# So, make certain that that tradeoff is considered when or if `/Os` is removed.
# See GH-2108 for more info.
set(VCLIBS_RELEASE_OPTIONS "$<$<COMPILE_LANGUAGE:CXX>:/O2;/Os>")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/out/lib/${VCLIBS_I386_OR_AMD64}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/out/lib/${VCLIBS_I386_OR_AMD64}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/out/bin/${VCLIBS_I386_OR_AMD64}")

set(CMAKE_STATIC_LINKER_FLAGS "/WX")
set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "")
set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "")
set(CMAKE_SHARED_LINKER_FLAGS "/DEBUG:FULL /WX /RELEASE /SUBSYSTEM:Console /NODEFAULTLIB /INCREMENTAL:NO /MANIFEST:NO /DLL /profile /guard:cf /DEBUGTYPE:cv,fixup /LARGEADDRESSAWARE")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "")

get_filename_component(TOOLSET_BINARIES_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY) # Example: $\VC\Tools\MSVC\14.23.27931\bin\Hostx86\x86
get_filename_component(TOOLSET_ROOT_DIR "${TOOLSET_BINARIES_DIR}" DIRECTORY) # $\VC\Tools\MSVC\14.23.27931\bin\Hostx86
get_filename_component(TOOLSET_ROOT_DIR "${TOOLSET_ROOT_DIR}" DIRECTORY) # $\VC\Tools\MSVC\14.23.27931\bin
get_filename_component(TOOLSET_ROOT_DIR "${TOOLSET_ROOT_DIR}" DIRECTORY) # $\VC\Tools\MSVC\14.23.27931

set(TOOLSET_LIB "${TOOLSET_ROOT_DIR}/lib/${VCLIBS_X86_OR_X64}")

add_subdirectory(boost-math)
add_subdirectory(stl)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
