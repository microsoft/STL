# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(HEADERS
    __msvc_all_public_headers.hpp
    algorithm
    any
    array
    atomic
    bitset
    cassert
    ccomplex
    cctype
    cerrno
    cfenv
    cfloat
    charconv
    chrono
    cinttypes
    ciso646
    climits
    clocale
    cmath
    codecvt
    compare
    complex
    concepts
    condition_variable
    csetjmp
    csignal
    cstdalign
    cstdarg
    cstdbool
    cstddef
    cstdint
    cstdio
    cstdlib
    cstring
    ctgmath
    ctime
    cuchar
    cvt/8859_1
    cvt/8859_10
    cvt/8859_13
    cvt/8859_14
    cvt/8859_15
    cvt/8859_16
    cvt/8859_2
    cvt/8859_3
    cvt/8859_4
    cvt/8859_5
    cvt/8859_6
    cvt/8859_7
    cvt/8859_8
    cvt/8859_9
    cvt/baltic
    cvt/big5
    cvt/cp037
    cvt/cp1006
    cvt/cp1026
    cvt/cp1250
    cvt/cp1251
    cvt/cp1252
    cvt/cp1253
    cvt/cp1254
    cvt/cp1255
    cvt/cp1256
    cvt/cp1257
    cvt/cp1258
    cvt/cp424
    cvt/cp437
    cvt/cp500
    cvt/cp737
    cvt/cp775
    cvt/cp850
    cvt/cp852
    cvt/cp855
    cvt/cp856
    cvt/cp857
    cvt/cp860
    cvt/cp861
    cvt/cp862
    cvt/cp863
    cvt/cp864
    cvt/cp865
    cvt/cp866
    cvt/cp869
    cvt/cp874
    cvt/cp875
    cvt/cp932
    cvt/cp936
    cvt/cp949
    cvt/cp950
    cvt/cyrillic
    cvt/ebcdic
    cvt/euc
    cvt/euc_0208
    cvt/gb12345
    cvt/gb2312
    cvt/greek
    cvt/iceland
    cvt/jis
    cvt/jis0201
    cvt/jis_0208
    cvt/ksc5601
    cvt/latin2
    cvt/one_one
    cvt/roman
    cvt/sjis
    cvt/sjis_0208
    cvt/turkish
    cvt/utf16
    cvt/utf8
    cvt/utf8_utf16
    cvt/wbuffer
    cvt/wstring
    cvt/xjis
    cvt/xone_byte
    cvt/xtwo_byte
    cwchar
    cwctype
    deque
    exception
    execution
    experimental/coroutine
    experimental/deque
    experimental/filesystem
    experimental/forward_list
    experimental/generator
    experimental/list
    experimental/map
    experimental/resumable
    experimental/set
    experimental/string
    experimental/unordered_map
    experimental/unordered_set
    experimental/vector
    experimental/xutility
    filesystem
    forward_list
    fstream
    functional
    future
    hash_map
    hash_set
    initializer_list
    iomanip
    ios
    iosfwd
    iostream
    iso646.h
    istream
    iterator
    limits
    list
    locale
    map
    memory
    memory_resource
    mutex
    new
    numeric
    optional
    ostream
    queue
    random
    ratio
    regex
    scoped_allocator
    set
    shared_mutex
    sstream
    stack
    stdexcept
    streambuf
    string
    string_view
    strstream
    system_error
    thread
    tuple
    type_traits
    typeindex
    typeinfo
    unordered_map
    unordered_set
    use_ansi.h
    utility
    valarray
    variant
    vector
    version
    xatomic.h
    xbit_ops.h
    xcall_once.h
    xcharconv.h
    xcharconv_ryu.h
    xcharconv_ryu_tables.h
    xerrc.h
    xfacet
    xfilesystem_abi.h
    xhash
    xiosbase
    xkeycheck.h
    xlocale
    xlocbuf
    xlocinfo
    xlocinfo.h
    xlocmes
    xlocmon
    xlocnum
    xloctime
    xmemory
    xnode_handle.h
    xpolymorphic_allocator.h
    xsmf_control.h
    xstddef
    xstring
    xtgmath.h
    xthreads.h
    xtimec.h
    xtr1common
    xtree
    xutility
    ymath.h
    yvals.h
    yvals_core.h
)

foreach(header ${HEADERS})
    configure_file("${CMAKE_CURRENT_LIST_DIR}/inc/${header}" "${PROJECT_BINARY_DIR}/out/inc/${header}" COPYONLY)
endforeach()

# Objs that exist in both libcpmt[d][01].lib and msvcprt[d].lib.
set(IMPLIB_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/filesystem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/locale0_implib.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/nothrow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/parallel_algorithms.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/sharedmutex.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/vector_algorithms.cpp
)

# The following files are linked in msvcp140[d][_clr].dll.
set (DLL_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/dllmain.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/instances.cpp
)

set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/StlCompareStringA.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/StlCompareStringW.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/StlLCMapStringA.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/StlLCMapStringW.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/_tolower.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/_toupper.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/atomic.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/cerr.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/cin.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/clog.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/cond.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/cout.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/cthread.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/filesys.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/fiopen.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/future.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/iomanip.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ios.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/iosptrs.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/iostream.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/locale.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/locale0.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/multprec.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mutex.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/pplerror.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ppltasks.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/raisehan.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/stdhndlr.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/stdthrow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/syserror.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/taskscheduler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/thread0.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/uncaught_exception.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/uncaught_exceptions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ushcerr.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ushcin.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ushclog.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ushcout.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ushiostr.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/wcerr.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/wcin.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/wclog.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/wcout.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/winapinls.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/winapisupp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/wiostrea.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/wlocale.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xalloc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xcosh.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xdateord.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xdint.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xdnorm.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xdscale.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xdtento.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xdtest.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xdunscal.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xexp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xfcosh.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xfdint.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xfdnorm.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xfdscale.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xfdtento.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xfdtest.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xfdunsca.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xferaise.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xfexp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xfprec.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xfsinh.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xfvalues.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xgetwctype.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xlcosh.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xldint.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xldscale.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xldtento.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xldtest.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xldunsca.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xlexp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xlgamma.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xlocale.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xlock.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xlpoly.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xlprec.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xlsinh.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xlvalues.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xmbtowc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xmtx.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xnotify.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xonce.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xpoly.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xprec.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xrngabort.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xrngdev.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xsinh.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xstod.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xstof.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xstoflt.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xstol.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xstold.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xstoll.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xstopfx.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xstoul.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xstoull.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xstoxflt.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xstrcoll.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xstrxfrm.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xthrow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xtime.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xtowlower.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xtowupper.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xvalues.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xwcscoll.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xwcsxfrm.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xwctomb.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xwstod.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xwstof.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xwstoflt.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xwstold.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xwstopfx.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/xwstoxfl.cpp
)

set(EHA_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/excptptr.cpp
)

set(SOURCES_SATELLITE_1
    ${CMAKE_CURRENT_LIST_DIR}/src/memory_resource.cpp
)

set(SOURCES_SATELLITE_2
    ${CMAKE_CURRENT_LIST_DIR}/src/special_math.cpp
)

set(SOURCES_SATELLITE_CODECVT_IDS
    ${CMAKE_CURRENT_LIST_DIR}/src/ulocale.cpp
)

# Objs that exist only in libcpmt[d][01].lib.
set(STATIC_SOURCES
    ${SOURCES_SATELLITE_1}
    ${SOURCES_SATELLITE_2}
    ${SOURCES_SATELLITE_CODECVT_IDS}
)

add_compile_definitions(_CRTBLD _VCRT_ALLOW_INTERNALS _HAS_OLD_IOSTREAMS_MEMBERS=1 _STL_CONCRT_SUPPORT)

include_directories(BEFORE
    "${CMAKE_CURRENT_LIST_DIR}/inc"
    "${TOOLSET_ROOT_DIR}/crt/src/concrt"
    "${TOOLSET_ROOT_DIR}/crt/src/vcruntime"
    )

function(add_stl_dlls D_SUFFIX THIS_CONFIG_DEFINITIONS THIS_CONFIG_COMPILE_OPTIONS GL_FLAG THIS_CONFIG_LINK_OPTIONS)
    # msvcp140.dll
    add_library(msvcp${D_SUFFIX}_objects OBJECT ${DLL_SOURCES} ${SOURCES})
    target_compile_definitions(msvcp${D_SUFFIX}_objects PRIVATE "CRTDLL2;_DLL;${THIS_CONFIG_DEFINITIONS}")
    target_compile_options(msvcp${D_SUFFIX}_objects PRIVATE "${THIS_CONFIG_COMPILE_OPTIONS};${GL_FLAG};/EHsc")

    add_library(msvcp${D_SUFFIX}_eha_objects OBJECT ${EHA_SOURCES})
    target_compile_definitions(msvcp${D_SUFFIX}_eha_objects PRIVATE "CRTDLL2;_DLL;${THIS_CONFIG_DEFINITIONS}")
    target_compile_options(msvcp${D_SUFFIX}_eha_objects PRIVATE "${THIS_CONFIG_COMPILE_OPTIONS};${GL_FLAG};/EHa")

    add_library(msvcp${D_SUFFIX} SHARED)
    target_link_libraries(msvcp${D_SUFFIX} PRIVATE msvcp${D_SUFFIX}_eha_objects msvcp${D_SUFFIX}_objects "${TOOLSET_LIB}/vcruntime${D_SUFFIX}.lib" "${TOOLSET_LIB}/msvcrt${D_SUFFIX}.lib" "ucrt${D_SUFFIX}.lib" "${TOOLSET_LIB}/concrt${D_SUFFIX}.lib" "kernel32.lib" "delayimp.lib")
    set_target_properties(msvcp${D_SUFFIX} PROPERTIES ARCHIVE_OUTPUT_NAME "msvcp140_base${D_SUFFIX}${VCLIBS_SUFFIX}")
    set_target_properties(msvcp${D_SUFFIX} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    set_target_properties(msvcp${D_SUFFIX} PROPERTIES OUTPUT_NAME "msvcp140${D_SUFFIX}${VCLIBS_SUFFIX}")
    target_link_options(msvcp${D_SUFFIX} PRIVATE "${THIS_CONFIG_LINK_OPTIONS};/delayload:concrt140${D_SUFFIX}.dll")

    # import library 'statics'
    add_library(msvcp${D_SUFFIX}_implib_objects OBJECT ${IMPLIB_SOURCES})
    target_compile_definitions(msvcp${D_SUFFIX}_implib_objects PRIVATE "_DLL;${THIS_CONFIG_DEFINITIONS}")
    target_compile_options(msvcp${D_SUFFIX}_implib_objects PRIVATE "${THIS_CONFIG_COMPILE_OPTIONS};/EHsc") # No /GL!

    # msvcp140_1.dll (the memory_resource satellite)
    add_library(msvcp_1${D_SUFFIX}_objects OBJECT ${SOURCES_SATELLITE_1})
    target_compile_definitions(msvcp_1${D_SUFFIX}_objects PRIVATE "_BUILDING_SATELLITE_1;_DLL;${THIS_CONFIG_DEFINITIONS}")
    target_compile_options(msvcp_1${D_SUFFIX}_objects PRIVATE "${THIS_CONFIG_COMPILE_OPTIONS};${GL_FLAG};/EHsc")

    add_library(msvcp_1${D_SUFFIX} SHARED)
    target_link_libraries(msvcp_1${D_SUFFIX} PRIVATE msvcp_1${D_SUFFIX}_objects "msvcp${D_SUFFIX}" "${TOOLSET_LIB}/vcruntime${D_SUFFIX}.lib" "${TOOLSET_LIB}/msvcrt${D_SUFFIX}.lib" "ucrt${D_SUFFIX}.lib" "kernel32.lib")
    set_target_properties(msvcp_1${D_SUFFIX} PROPERTIES ARCHIVE_OUTPUT_NAME "msvcp140_1${D_SUFFIX}${VCLIBS_SUFFIX}")
    set_target_properties(msvcp_1${D_SUFFIX} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    set_target_properties(msvcp_1${D_SUFFIX} PROPERTIES OUTPUT_NAME "msvcp140_1${D_SUFFIX}${VCLIBS_SUFFIX}")
    target_link_options(msvcp_1${D_SUFFIX} PRIVATE "${THIS_CONFIG_LINK_OPTIONS}")

    # msvcp140_2.dll (the special math satellite)
    add_library(msvcp_2${D_SUFFIX}_objects OBJECT ${SOURCES_SATELLITE_2})
    target_compile_definitions(msvcp_2${D_SUFFIX}_objects PRIVATE "_BUILDING_SATELLITE_2;_DLL;${THIS_CONFIG_DEFINITIONS}")
    target_compile_options(msvcp_2${D_SUFFIX}_objects PRIVATE "${THIS_CONFIG_COMPILE_OPTIONS};${GL_FLAG};/EHsc")
    target_link_libraries(msvcp_2${D_SUFFIX}_objects PRIVATE Boost::headers Boost::disable_autolinking)

    add_library(msvcp_2${D_SUFFIX} SHARED)
    target_link_libraries(msvcp_2${D_SUFFIX} PRIVATE msvcp_2${D_SUFFIX}_objects msvcp${D_SUFFIX}_implib_objects "msvcp${D_SUFFIX}" "${TOOLSET_LIB}/vcruntime${D_SUFFIX}.lib" "${TOOLSET_LIB}/msvcrt${D_SUFFIX}.lib" "ucrt${D_SUFFIX}.lib" "kernel32.lib")
    set_target_properties(msvcp_2${D_SUFFIX} PROPERTIES ARCHIVE_OUTPUT_NAME "msvcp140_2${D_SUFFIX}${VCLIBS_SUFFIX}")
    set_target_properties(msvcp_2${D_SUFFIX} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    set_target_properties(msvcp_2${D_SUFFIX} PROPERTIES OUTPUT_NAME "msvcp140_2${D_SUFFIX}${VCLIBS_SUFFIX}")
    target_link_options(msvcp_2${D_SUFFIX} PRIVATE "${THIS_CONFIG_LINK_OPTIONS}")

    # msvcp140_codecvt_ids.dll
    add_library(msvcp${D_SUFFIX}_codecvt_ids_objects OBJECT ${SOURCES_SATELLITE_CODECVT_IDS})
    target_compile_definitions(msvcp${D_SUFFIX}_codecvt_ids_objects PRIVATE "_BUILDING_SATELLITE_CODECVT_IDS;_DLL;${THIS_CONFIG_DEFINITIONS}")
    target_compile_options(msvcp${D_SUFFIX}_codecvt_ids_objects PRIVATE "${THIS_CONFIG_COMPILE_OPTIONS};${GL_FLAG};/EHsc")

    add_library(msvcp${D_SUFFIX}_codecvt_ids SHARED)
    target_link_libraries(msvcp${D_SUFFIX}_codecvt_ids PRIVATE msvcp${D_SUFFIX}_codecvt_ids_objects "msvcp${D_SUFFIX}" "${TOOLSET_LIB}/vcruntime${D_SUFFIX}.lib" "${TOOLSET_LIB}/msvcrt${D_SUFFIX}.lib" "ucrt${D_SUFFIX}.lib" "kernel32.lib")
    set_target_properties(msvcp${D_SUFFIX}_codecvt_ids PROPERTIES ARCHIVE_OUTPUT_NAME "msvcp140_codecvt_ids${D_SUFFIX}${VCLIBS_SUFFIX}")
    set_target_properties(msvcp${D_SUFFIX}_codecvt_ids PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    set_target_properties(msvcp${D_SUFFIX}_codecvt_ids PROPERTIES OUTPUT_NAME "msvcp140${D_SUFFIX}_codecvt_ids${VCLIBS_SUFFIX}")
    target_link_options(msvcp${D_SUFFIX}_codecvt_ids PRIVATE "${THIS_CONFIG_LINK_OPTIONS}")

    # import library
    add_library(msvcp${D_SUFFIX}_implib STATIC ${HEADERS})
    target_link_libraries(msvcp${D_SUFFIX}_implib msvcp${D_SUFFIX}_implib_objects)
    add_dependencies(msvcp${D_SUFFIX}_implib msvcp${D_SUFFIX} msvcp_1${D_SUFFIX} msvcp_2${D_SUFFIX} msvcp${D_SUFFIX}_codecvt_ids)
    set_target_properties(msvcp${D_SUFFIX}_implib PROPERTIES STATIC_LIBRARY_OPTIONS "/NOLOGO;/NODEFAULTLIB;/IGNORE:4006;$<TARGET_LINKER_FILE:msvcp${D_SUFFIX}>;$<TARGET_LINKER_FILE:msvcp_1${D_SUFFIX}>;$<TARGET_LINKER_FILE:msvcp_2${D_SUFFIX}>;$<TARGET_LINKER_FILE:msvcp${D_SUFFIX}_codecvt_ids>")
    set_target_properties(msvcp${D_SUFFIX}_implib PROPERTIES ARCHIVE_OUTPUT_NAME "msvcprt${D_SUFFIX}")
endfunction()

add_stl_dlls("" "" "${VCLIBS_RELEASE_OPTIONS}" "/GL" "/LTCG;/opt:ref,icf")
add_stl_dlls("d" "_DEBUG" "${VCLIBS_DEBUG_OPTIONS}" "" "/opt:ref,noicf")

function(add_stl_statics FLAVOR_SUFFIX THIS_CONFIG_DEFINITIONS THIS_CONFIG_COMPILE_OPTIONS)
    add_library(libcpmt${FLAVOR_SUFFIX}_eha OBJECT ${EHA_SOURCES})
    target_compile_definitions(libcpmt${FLAVOR_SUFFIX}_eha PRIVATE "${THIS_CONFIG_DEFINITIONS}")
    target_compile_options(libcpmt${FLAVOR_SUFFIX}_eha PRIVATE "${THIS_CONFIG_COMPILE_OPTIONS};/EHa")

    add_library(libcpmt${FLAVOR_SUFFIX} STATIC ${IMPLIB_SOURCES} ${SOURCES} ${STATIC_SOURCES})
    target_compile_definitions(libcpmt${FLAVOR_SUFFIX} PRIVATE "${THIS_CONFIG_DEFINITIONS}")
    target_compile_options(libcpmt${FLAVOR_SUFFIX} PRIVATE "${THIS_CONFIG_COMPILE_OPTIONS};/EHsc")
    target_link_libraries(libcpmt${FLAVOR_SUFFIX} PRIVATE Boost::headers Boost::disable_autolinking libcpmt${FLAVOR_SUFFIX}_eha)
endfunction()

add_stl_statics("" "_ITERATOR_DEBUG_LEVEL=0" "${VCLIBS_RELEASE_OPTIONS}")
add_stl_statics("1" "_ITERATOR_DEBUG_LEVEL=1" "${VCLIBS_RELEASE_OPTIONS}")
add_stl_statics("d" "_DEBUG;_ITERATOR_DEBUG_LEVEL=2" "${VCLIBS_DEBUG_OPTIONS}")
add_stl_statics("d1" "_DEBUG;_ITERATOR_DEBUG_LEVEL=1" "${VCLIBS_DEBUG_OPTIONS}")
add_stl_statics("d0" "_DEBUG;_ITERATOR_DEBUG_LEVEL=0" "${VCLIBS_DEBUG_OPTIONS}")
