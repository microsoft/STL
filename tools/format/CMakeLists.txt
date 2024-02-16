# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.28.0)
project(msvc_standard_libraries_format NONE)

set(did_search OFF)

if(NOT DEFINED CLANG_FORMAT)
    message(STATUS "Searching for VS clang-format")
    set(did_search ON)
endif()

if(PROJECT_IS_TOP_LEVEL)
    set(message_level FATAL_ERROR)
else()
    set(message_level WARNING)
endif()

find_program(CLANG_FORMAT
    NAMES clang-format
    PATHS "C:/Program Files/Microsoft Visual Studio/2022/Preview/VC/Tools/Llvm/bin"
    DOC "The clang-format program to use"
    NO_DEFAULT_PATH
    NO_CMAKE_PATH
    NO_CMAKE_ENVIRONMENT_PATH
    NO_SYSTEM_ENVIRONMENT_PATH
    NO_CMAKE_SYSTEM_PATH
)

if(CLANG_FORMAT)
    if(did_search)
        message(STATUS "Searching for VS clang-format - found")
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
        message("${message_level}" "Could not find any files to clang-format!")
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
else()
    if(did_search)
        message("${message_level}" "Searching for VS clang-format - not found.")
    endif()
endif()
