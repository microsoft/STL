set(LIBCXX_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../llvm-project/libcxx" CACHE PATH
    "Location of the libcxx sources")
set(LIBCXX_ENVLST "${CMAKE_CURRENT_SOURCE_DIR}/usual_matrix.lst" CACHE PATH
    "Location of the libcxx env_lst file")
set(LIBCXX_SKIP_LIST "${CMAKE_CURRENT_SOURCE_DIR}/skipped_tests.txt" CACHE PATH
    "Path to libcxx skip list")
set(LIBCXX_SKIP_LIST_ROOT "${LIBCXX_SOURCE_DIR}/test/std" CACHE PATH
    "Directory from which all skip list entries are relative")
set(LIBCXX_EXPECTED_FAILURES "${CMAKE_CURRENT_SOURCE_DIR}/expected_failures.txt" CACHE PATH
    "Path to libcxx expected failures list")
set(LIBCXX_EXPECTED_FAILURES_ROOT "${LIBCXX_SOURCE_DIR}/test/std" CACHE PATH
    "Directory from which all expected failure entries are relative")

if (STL_INCLUDE_TESTS)
  include(AddLLVM) # for configure_lit_site_cfg and add_lit_testsuite

  configure_lit_site_cfg(
    ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.in
    ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg)

  add_lit_testsuite(check-libcxx
    "Running libcxx std tests"
    ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS cxx)
endif()
