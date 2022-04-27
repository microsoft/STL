cmake_minimum_required(VERSION 3.22)
project(stl-format NONE)

set(did_search ON)
if(NOT DEFINED CLANG_FORMAT)
    message(STATUS "Searching for VS clang-format")
    set(did_search OFF)
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

    file(GLOB_RECURSE maybe-clang-format-files
        "../../stl/inc/*"
        "../../stl/src/*"
        "../../tests/*"
        "../../tools/*"
    )
    set(clang-format-files "")
    foreach(maybe-file IN LISTS maybe-clang-format-files)
        cmake_path(GET maybe-file EXTENSION LAST_ONLY extension)
        if(extension MATCHES [[^(|\.cpp|\.h|\.hpp)$]])
            list(APPEND clang-format-files "${maybe-file}")
        endif()
    endforeach()

    if(NOT clang-format-files)
        message("${message_level}" "Could not find any files to clang-format!")
    endif()

    set(clang-format-targets "")
    foreach(file IN LISTS clang-format-files)
        cmake_path(RELATIVE_PATH file
            BASE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../.."
            OUTPUT_VARIABLE relative-file
        )
        string(REPLACE "/" "_" relative-file "${relative-file}")
        set(target_name "clang-format.${relative-file}")
        add_custom_target("${target_name}"
            COMMAND "${CLANG_FORMAT}" -style=file -i "${file}"
            WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        )
        list(APPEND clang-format-targets "${target_name}")
    endforeach()

    if(PROJECT_IS_TOP_LEVEL)
        add_custom_target(format ALL)
        add_dependencies(format ${clang-format-targets})
    else()
        set(CLANG_FORMAT_TARGETS "${clang-format-targets}" PARENT_SCOPE)
    endif()
else()
    if(did_search)
        message("${message_level}" "Searching for VS clang-format - not found.")
    endif()

    if(NOT PROJECT_IS_TOP_LEVEL)
        set(CLANG_FORMAT_TARGETS "" PARENT_SCOPE)
    endif()
endif()
