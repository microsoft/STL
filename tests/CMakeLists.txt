# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(STL_BUILD_ROOT "${PROJECT_BINARY_DIR}/out")
set(STL_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
set(STL_TEST_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(STL_TEST_UTILS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/utils")
set(STL_TESTED_HEADERS_DIR "${STL_BUILD_ROOT}/inc")
set(STL_TESTED_MODULES_DIR "${STL_BUILD_ROOT}/modules")

set(LLVM_PROJECT_SOURCE_DIR "${STL_SOURCE_DIR}/llvm-project" CACHE PATH
    "Location of the llvm-project source tree")
set(LIBCXX_SOURCE_DIR "${LLVM_PROJECT_SOURCE_DIR}/libcxx" CACHE PATH
    "Location of the libcxx source tree")

option(TESTS_BUILD_ONLY "Only run the build steps of tests" OFF)

add_subdirectory(libcxx)
add_subdirectory(std)
add_subdirectory(tr1)

# Add the stl-lit subdirectory last so all the test directories have had a
# chance to add to the config map and test directory global properties.
add_subdirectory(utils/stl-lit)

find_package(Python "3.12" REQUIRED COMPONENTS Interpreter)

if(NOT DEFINED LIT_FLAGS)
    list(APPEND LIT_FLAGS "-o" "${CMAKE_CURRENT_BINARY_DIR}/test_results.json")
endif()

get_property(STL_LIT_TEST_DIRS GLOBAL PROPERTY STL_LIT_TEST_DIRS)
list(APPEND STL_LIT_COMMAND "${STL_LIT_OUTPUT}"
                            "${LIT_FLAGS}"
                            "-D" "notags=ASAN"
                            "${STL_LIT_TEST_DIRS}")
list(APPEND STLASAN_LIT_COMMAND "${STL_LIT_OUTPUT}"
                                "${LIT_FLAGS}"
                                "-D" "tags=ASAN"
                                "${STL_LIT_TEST_DIRS}")

# libcxx will be handled differently, because they use their own runner
foreach(DIR std tr1)
    file(GLOB globbed_dirs
         LIST_DIRECTORIES ON
         "${DIR}/tests/*")
    foreach(test_dir ${globbed_dirs})
        # There are some .lst files alongside the directories
        if (NOT IS_DIRECTORY ${test_dir})
            continue()
        endif()
        get_filename_component(just_folder ${test_dir} NAME)
        add_test(NAME "${DIR}::${just_folder}"
          COMMAND ${Python_EXECUTABLE} ${STL_LIT_OUTPUT} ${LIT_FLAGS} "-o" "${CMAKE_CURRENT_BINARY_DIR}/test_results_${DIR}_${just_folder}.json" "-D" "notags=ASAN" ${test_dir})
        set_tests_properties("${DIR}::${just_folder}"
          PROPERTIES
            RUN_SERIAL ON
            LABELS "${DIR}"
        )
        add_test(NAME "${DIR}::${just_folder}::ASAN"
          COMMAND ${Python_EXECUTABLE} ${STL_LIT_OUTPUT} ${LIT_FLAGS} "-o" "${CMAKE_CURRENT_BINARY_DIR}/test_results_${DIR}_${just_folder}_asan.json" "-D" "tags=ASAN" ${test_dir})
        set_tests_properties("${DIR}::${just_folder}::ASAN"
          PROPERTIES
            RUN_SERIAL ON
            LABELS "${DIR};asan"
        )
    endforeach()
endforeach()


# TODO: explicitly add libcxx the way it was done before
#add_test(NAME stl COMMAND ${Python_EXECUTABLE} ${STL_LIT_COMMAND} COMMAND_EXPAND_LISTS)
#add_test(NAME stlasan COMMAND ${Python_EXECUTABLE} ${STLASAN_LIT_COMMAND} COMMAND_EXPAND_LISTS)
#set_tests_properties(stl stlasan PROPERTIES RUN_SERIAL ON)
