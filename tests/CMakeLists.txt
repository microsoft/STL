set(STL_SOURCE_DIR "${msvc_standard_libraries_SOURCE_DIR}" CACHE PATH
    "Location of the stl source tree")
set(STL_TEST_UTILS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/utils" CACHE PATH
    "Location of the stl test utilities")
set(STL_BUILD_ROOT "${msvc_standard_libraries_BINARY_DIR}/out" CACHE PATH
    "Location of the output of the STL build")
set(STL_HEADERS_DIR "${STL_BUILD_ROOT}/inc" CACHE PATH
    "Location of the STL headers")
set(LLVM_PROJECT_SOURCE_DIR "${STL_SOURCE_DIR}/llvm-project" CACHE PATH
    "Location of the llvm-project source tree")
set(LLVM_SOURCE_DIR "${LLVM_PROJECT_SOURCE_DIR}/llvm" CACHE PATH
    "Location of the llvm source tree")
set(LLVM_LIT_SOURCE_DIR "${LLVM_SOURCE_DIR}/utils/llvm-lit" CACHE PATH
    "Location of the lit source tree")
set(LLVM_LIT_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/llvm-lit" CACHE PATH
    "Output location of llvm-lit.py")
set(LLVM_LIT_LOCATION "" CACHE PATH "Location of llvm-lit.py")

list(APPEND CMAKE_MODULE_PATH "${STL_SOURCE_DIR}/llvm-project/llvm/cmake/modules")
include(AddLLVM) # for configure_lit_site_cfg and add_lit_testsuite

if(LLVM_LIT_LOCATION STREQUAL "")
  get_llvm_lit_path(LIT_BASE_DIR LIT_FILE_NAME)

  set(LLVM_LIT_LOCATION "${LIT_BASE_DIR}/${LIT_FILE_NAME}" CACHE PATH
      "Location of llvm-lit.py" FORCE)
endif()

add_subdirectory(${LLVM_LIT_SOURCE_DIR} llvm-lit)
add_subdirectory(libcxx)
