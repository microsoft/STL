set(did_search ON)
if(NOT DEFINED CLANG_FORMAT)
    message(STATUS "Searching for VS clang-format")
    set(did_search OFF)
endif()

if(ERROR_ON_CLANG_FORMAT_NOT_FOUND)
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
        "${PROJECT_SOURCE_DIR}/stl/inc/*"
        "${PROJECT_SOURCE_DIR}/stl/src/*"
        "${PROJECT_SOURCE_DIR}/tests/*"
        "${PROJECT_SOURCE_DIR}/tools/*"
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
            BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
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

    set(CLANG_FORMAT_TARGETS "${clang-format-targets}" PARENT_SCOPE)
else()
    if(did_search)
        message("${message_level}" "Searching for VS clang-format - not found.")
    endif()
    set(CLANG_FORMAT_TARGETS "" PARENT_SCOPE)
endif()
