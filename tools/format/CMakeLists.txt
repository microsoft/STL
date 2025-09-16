# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.31.0)
project(msvc_standard_libraries_format NONE)

find_program(CLANG_FORMAT
    NAMES clang-format
    HINTS "$ENV{VCINSTALLDIR}/Tools/Llvm/x64/bin"
    DOC "The clang-format program to use"
    REQUIRED
)

execute_process(
    COMMAND "${CLANG_FORMAT}" --version
    OUTPUT_VARIABLE clang_format_version
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(clang_format_version MATCHES "clang-format version ([0-9]+\.[0-9]+\.[0-9]+)")
    set(expected_version "20.1.8")
    if(CMAKE_MATCH_1 VERSION_LESS expected_version)
        message(FATAL_ERROR "Found clang-format: ${CLANG_FORMAT} (\"${CMAKE_MATCH_1}\", older than expected version \"${expected_version}\")")
    elseif(CMAKE_MATCH_1 VERSION_EQUAL expected_version)
        message(STATUS "Found clang-format: ${CLANG_FORMAT} (found expected version \"${CMAKE_MATCH_1}\")")
    elseif(CMAKE_MATCH_1 VERSION_GREATER expected_version)
        message(WARNING "Found clang-format: ${CLANG_FORMAT} (\"${CMAKE_MATCH_1}\", newer than expected version \"${expected_version}\")")
    endif()
else()
    message(FATAL_ERROR "Unexpected `clang-format --version` output: '${clang_format_version}'")
endif()

file(GLOB_RECURSE maybe_clang_format_files
    "../../benchmarks/inc/*"
    "../../benchmarks/src/*"
    "../../stl/inc/*"
    "../../stl/modules/*"
    "../../stl/src/*"
    "../../tests/*"
    "../../tools/*"
)
set(clang_format_files "")

foreach(maybe_file IN LISTS maybe_clang_format_files)
    cmake_path(GET maybe_file FILENAME filename)
    cmake_path(GET maybe_file EXTENSION LAST_ONLY extension)

    if(extension MATCHES [[^(|\.cpp|\.h|\.hpp|\.ixx)$]] AND NOT filename MATCHES [[^\.]])
        list(APPEND clang_format_files "${maybe_file}")
    endif()
endforeach()

if(NOT clang_format_files)
    message(FATAL_ERROR "Could not find any files to clang-format!")
endif()

add_custom_target(run-format)

foreach(file IN LISTS clang_format_files)
    cmake_path(RELATIVE_PATH file
        BASE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../.."
        OUTPUT_VARIABLE relative-file
    )
    string(REPLACE "/" "_" relative-file "${relative-file}")
    set(target_name "clang-format.${relative-file}")
    add_custom_target("${target_name}"
        COMMAND "${CLANG_FORMAT}" -style=file -i "${file}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../.."
    )
    add_dependencies(run-format "${target_name}")
endforeach()
