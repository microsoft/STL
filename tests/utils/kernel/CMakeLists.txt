# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(STL_KERNEL_LOADER_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/stl_kernel_loader/testapp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/stl_kernel_loader/install.cpp
)

set(STL_KERNEL_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/stl_kernel/doAssert.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/stl_kernel/stl_kernel.cpp
)

add_library(stl_kernel STATIC ${STL_KERNEL_SOURCES})
set_target_properties(stl_kernel PROPERTIES LINKER_LANGUAGE CXX)
target_compile_definitions(stl_kernel PRIVATE "__STL_IS_KERNEL")

# ${WDK_INCLUDE_DIRECTORY}/km/crt isn't in the files under test include paths.
# doAssert.cpp wants to use RtlStringCbPrintfExA, and that requires pulling in
# km/crt.  If we don't pull in km/crt here, we erroneously pull in user mode
# crt headers, and that gets us linker errors dealing with missing user mode
# symbols in this kernel mode binary.
target_include_directories(stl_kernel PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/inc
    ${WDK_INCLUDE_DIRECTORY}/km
    ${WDK_INCLUDE_DIRECTORY}/km/crt
    ${WDK_INCLUDE_DIRECTORY}/shared
)
target_compile_options(stl_kernel PRIVATE /kernel)

add_executable(stl_kernel_loader ${STL_KERNEL_LOADER_SOURCES})
target_include_directories(stl_kernel_loader PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/inc
    ${WDK_INCLUDE_DIRECTORY}/ucrt
    ${WDK_INCLUDE_DIRECTORY}/um
    ${WDK_INCLUDE_DIRECTORY}/shared
)
target_compile_options(stl_kernel_loader PRIVATE /EHsc)
target_link_libraries(stl_kernel_loader PRIVATE
    msvcrt.lib
    kernel32.lib
    ucrt.lib
    AdvApi32.lib
)

