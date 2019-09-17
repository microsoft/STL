# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/inc/__msvc_all_public_headers.hpp
    ${CMAKE_CURRENT_LIST_DIR}/inc/algorithm
    ${CMAKE_CURRENT_LIST_DIR}/inc/any
    ${CMAKE_CURRENT_LIST_DIR}/inc/array
    ${CMAKE_CURRENT_LIST_DIR}/inc/atomic
    ${CMAKE_CURRENT_LIST_DIR}/inc/bitset
    ${CMAKE_CURRENT_LIST_DIR}/inc/cassert
    ${CMAKE_CURRENT_LIST_DIR}/inc/ccomplex
    ${CMAKE_CURRENT_LIST_DIR}/inc/cctype
    ${CMAKE_CURRENT_LIST_DIR}/inc/cerrno
    ${CMAKE_CURRENT_LIST_DIR}/inc/cfenv
    ${CMAKE_CURRENT_LIST_DIR}/inc/cfloat
    ${CMAKE_CURRENT_LIST_DIR}/inc/charconv
    ${CMAKE_CURRENT_LIST_DIR}/inc/chrono
    ${CMAKE_CURRENT_LIST_DIR}/inc/cinttypes
    ${CMAKE_CURRENT_LIST_DIR}/inc/ciso646
    ${CMAKE_CURRENT_LIST_DIR}/inc/climits
    ${CMAKE_CURRENT_LIST_DIR}/inc/clocale
    ${CMAKE_CURRENT_LIST_DIR}/inc/cmath
    ${CMAKE_CURRENT_LIST_DIR}/inc/codecvt
    ${CMAKE_CURRENT_LIST_DIR}/inc/compare
    ${CMAKE_CURRENT_LIST_DIR}/inc/complex
    ${CMAKE_CURRENT_LIST_DIR}/inc/concepts
    ${CMAKE_CURRENT_LIST_DIR}/inc/condition_variable
    ${CMAKE_CURRENT_LIST_DIR}/inc/csetjmp
    ${CMAKE_CURRENT_LIST_DIR}/inc/csignal
    ${CMAKE_CURRENT_LIST_DIR}/inc/cstdalign
    ${CMAKE_CURRENT_LIST_DIR}/inc/cstdarg
    ${CMAKE_CURRENT_LIST_DIR}/inc/cstdbool
    ${CMAKE_CURRENT_LIST_DIR}/inc/cstddef
    ${CMAKE_CURRENT_LIST_DIR}/inc/cstdint
    ${CMAKE_CURRENT_LIST_DIR}/inc/cstdio
    ${CMAKE_CURRENT_LIST_DIR}/inc/cstdlib
    ${CMAKE_CURRENT_LIST_DIR}/inc/cstring
    ${CMAKE_CURRENT_LIST_DIR}/inc/ctgmath
    ${CMAKE_CURRENT_LIST_DIR}/inc/ctime
    ${CMAKE_CURRENT_LIST_DIR}/inc/cuchar
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/8859_1
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/8859_10
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/8859_13
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/8859_14
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/8859_15
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/8859_16
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/8859_2
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/8859_3
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/8859_4
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/8859_5
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/8859_6
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/8859_7
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/8859_8
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/8859_9
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/baltic
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/big5
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp037
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp1006
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp1026
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp1250
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp1251
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp1252
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp1253
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp1254
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp1255
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp1256
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp1257
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp1258
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp424
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp437
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp500
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp737
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp775
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp850
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp852
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp855
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp856
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp857
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp860
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp861
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp862
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp863
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp864
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp865
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp866
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp869
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp874
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp875
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp932
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp936
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp949
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cp950
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/cyrillic
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/ebcdic
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/euc
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/euc_0208
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/gb12345
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/gb2312
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/greek
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/iceland
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/jis
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/jis0201
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/jis_0208
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/ksc5601
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/latin2
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/one_one
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/roman
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/sjis
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/sjis_0208
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/turkish
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/utf16
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/utf8
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/utf8_utf16
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/wbuffer
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/wstring
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/xjis
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/xone_byte
    ${CMAKE_CURRENT_LIST_DIR}/inc/cvt/xtwo_byte
    ${CMAKE_CURRENT_LIST_DIR}/inc/cwchar
    ${CMAKE_CURRENT_LIST_DIR}/inc/cwctype
    ${CMAKE_CURRENT_LIST_DIR}/inc/deque
    ${CMAKE_CURRENT_LIST_DIR}/inc/exception
    ${CMAKE_CURRENT_LIST_DIR}/inc/execution
    ${CMAKE_CURRENT_LIST_DIR}/inc/experimental/coroutine
    ${CMAKE_CURRENT_LIST_DIR}/inc/experimental/deque
    ${CMAKE_CURRENT_LIST_DIR}/inc/experimental/filesystem
    ${CMAKE_CURRENT_LIST_DIR}/inc/experimental/forward_list
    ${CMAKE_CURRENT_LIST_DIR}/inc/experimental/generator
    ${CMAKE_CURRENT_LIST_DIR}/inc/experimental/list
    ${CMAKE_CURRENT_LIST_DIR}/inc/experimental/map
    ${CMAKE_CURRENT_LIST_DIR}/inc/experimental/resumable
    ${CMAKE_CURRENT_LIST_DIR}/inc/experimental/set
    ${CMAKE_CURRENT_LIST_DIR}/inc/experimental/string
    ${CMAKE_CURRENT_LIST_DIR}/inc/experimental/unordered_map
    ${CMAKE_CURRENT_LIST_DIR}/inc/experimental/unordered_set
    ${CMAKE_CURRENT_LIST_DIR}/inc/experimental/vector
    ${CMAKE_CURRENT_LIST_DIR}/inc/experimental/xutility
    ${CMAKE_CURRENT_LIST_DIR}/inc/filesystem
    ${CMAKE_CURRENT_LIST_DIR}/inc/forward_list
    ${CMAKE_CURRENT_LIST_DIR}/inc/fstream
    ${CMAKE_CURRENT_LIST_DIR}/inc/functional
    ${CMAKE_CURRENT_LIST_DIR}/inc/future
    ${CMAKE_CURRENT_LIST_DIR}/inc/hash_map
    ${CMAKE_CURRENT_LIST_DIR}/inc/hash_set
    ${CMAKE_CURRENT_LIST_DIR}/inc/initializer_list
    ${CMAKE_CURRENT_LIST_DIR}/inc/iomanip
    ${CMAKE_CURRENT_LIST_DIR}/inc/ios
    ${CMAKE_CURRENT_LIST_DIR}/inc/iosfwd
    ${CMAKE_CURRENT_LIST_DIR}/inc/iostream
    ${CMAKE_CURRENT_LIST_DIR}/inc/iso646.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/istream
    ${CMAKE_CURRENT_LIST_DIR}/inc/iterator
    ${CMAKE_CURRENT_LIST_DIR}/inc/limits
    ${CMAKE_CURRENT_LIST_DIR}/inc/list
    ${CMAKE_CURRENT_LIST_DIR}/inc/locale
    ${CMAKE_CURRENT_LIST_DIR}/inc/map
    ${CMAKE_CURRENT_LIST_DIR}/inc/memory
    ${CMAKE_CURRENT_LIST_DIR}/inc/memory_resource
    ${CMAKE_CURRENT_LIST_DIR}/inc/mutex
    ${CMAKE_CURRENT_LIST_DIR}/inc/new
    ${CMAKE_CURRENT_LIST_DIR}/inc/numeric
    ${CMAKE_CURRENT_LIST_DIR}/inc/optional
    ${CMAKE_CURRENT_LIST_DIR}/inc/ostream
    ${CMAKE_CURRENT_LIST_DIR}/inc/queue
    ${CMAKE_CURRENT_LIST_DIR}/inc/random
    ${CMAKE_CURRENT_LIST_DIR}/inc/ranges
    ${CMAKE_CURRENT_LIST_DIR}/inc/ratio
    ${CMAKE_CURRENT_LIST_DIR}/inc/regex
    ${CMAKE_CURRENT_LIST_DIR}/inc/scoped_allocator
    ${CMAKE_CURRENT_LIST_DIR}/inc/set
    ${CMAKE_CURRENT_LIST_DIR}/inc/shared_mutex
    ${CMAKE_CURRENT_LIST_DIR}/inc/sstream
    ${CMAKE_CURRENT_LIST_DIR}/inc/stack
    ${CMAKE_CURRENT_LIST_DIR}/inc/stdexcept
    ${CMAKE_CURRENT_LIST_DIR}/inc/streambuf
    ${CMAKE_CURRENT_LIST_DIR}/inc/string
    ${CMAKE_CURRENT_LIST_DIR}/inc/string_view
    ${CMAKE_CURRENT_LIST_DIR}/inc/strstream
    ${CMAKE_CURRENT_LIST_DIR}/inc/system_error
    ${CMAKE_CURRENT_LIST_DIR}/inc/thread
    ${CMAKE_CURRENT_LIST_DIR}/inc/tuple
    ${CMAKE_CURRENT_LIST_DIR}/inc/type_traits
    ${CMAKE_CURRENT_LIST_DIR}/inc/typeindex
    ${CMAKE_CURRENT_LIST_DIR}/inc/typeinfo
    ${CMAKE_CURRENT_LIST_DIR}/inc/unordered_map
    ${CMAKE_CURRENT_LIST_DIR}/inc/unordered_set
    ${CMAKE_CURRENT_LIST_DIR}/inc/use_ansi.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/utility
    ${CMAKE_CURRENT_LIST_DIR}/inc/valarray
    ${CMAKE_CURRENT_LIST_DIR}/inc/variant
    ${CMAKE_CURRENT_LIST_DIR}/inc/vector
    ${CMAKE_CURRENT_LIST_DIR}/inc/version
    ${CMAKE_CURRENT_LIST_DIR}/inc/xatomic.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/xbit_ops.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/xcall_once.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/xcharconv.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/xcharconv_ryu.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/xcharconv_ryu_tables.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/xerrc.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/xfacet
    ${CMAKE_CURRENT_LIST_DIR}/inc/xfilesystem_abi.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/xhash
    ${CMAKE_CURRENT_LIST_DIR}/inc/xiosbase
    ${CMAKE_CURRENT_LIST_DIR}/inc/xkeycheck.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/xlocale
    ${CMAKE_CURRENT_LIST_DIR}/inc/xlocbuf
    ${CMAKE_CURRENT_LIST_DIR}/inc/xlocinfo
    ${CMAKE_CURRENT_LIST_DIR}/inc/xlocinfo.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/xlocmes
    ${CMAKE_CURRENT_LIST_DIR}/inc/xlocmon
    ${CMAKE_CURRENT_LIST_DIR}/inc/xlocnum
    ${CMAKE_CURRENT_LIST_DIR}/inc/xloctime
    ${CMAKE_CURRENT_LIST_DIR}/inc/xmemory
    ${CMAKE_CURRENT_LIST_DIR}/inc/xnode_handle.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/xpolymorphic_allocator.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/xsmf_control.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/xstddef
    ${CMAKE_CURRENT_LIST_DIR}/inc/xstring
    ${CMAKE_CURRENT_LIST_DIR}/inc/xtgmath.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/xthreads.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/xtimec.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/xtr1common
    ${CMAKE_CURRENT_LIST_DIR}/inc/xtree
    ${CMAKE_CURRENT_LIST_DIR}/inc/xutility
    ${CMAKE_CURRENT_LIST_DIR}/inc/ymath.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/yvals.h
    ${CMAKE_CURRENT_LIST_DIR}/inc/yvals_core.h
)

foreach(header ${HEADERS})
    file(RELATIVE_PATH _header_path "${CMAKE_CURRENT_LIST_DIR}/inc" "${header}")
    configure_file("${header}" "${PROJECT_BINARY_DIR}/out/inc/${_header_path}" COPYONLY)
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
    target_link_libraries(msvcp${D_SUFFIX} PRIVATE msvcp${D_SUFFIX}_eha_objects msvcp${D_SUFFIX}_objects "${TOOLSET_LIB}/vcruntime${D_SUFFIX}.lib" "${TOOLSET_LIB}/msvcrt${D_SUFFIX}.lib" "ucrt${D_SUFFIX}.lib" "${TOOLSET_LIB}/concrt${D_SUFFIX}.lib" "delayimp.lib")
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
    target_link_libraries(msvcp_1${D_SUFFIX} PRIVATE msvcp_1${D_SUFFIX}_objects "msvcp${D_SUFFIX}" "${TOOLSET_LIB}/vcruntime${D_SUFFIX}.lib" "${TOOLSET_LIB}/msvcrt${D_SUFFIX}.lib" "ucrt${D_SUFFIX}.lib")
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
    target_link_libraries(msvcp_2${D_SUFFIX} PRIVATE msvcp_2${D_SUFFIX}_objects msvcp${D_SUFFIX}_implib_objects "msvcp${D_SUFFIX}" "${TOOLSET_LIB}/vcruntime${D_SUFFIX}.lib" "${TOOLSET_LIB}/msvcrt${D_SUFFIX}.lib" "ucrt${D_SUFFIX}.lib")
    set_target_properties(msvcp_2${D_SUFFIX} PROPERTIES ARCHIVE_OUTPUT_NAME "msvcp140_2${D_SUFFIX}${VCLIBS_SUFFIX}")
    set_target_properties(msvcp_2${D_SUFFIX} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    set_target_properties(msvcp_2${D_SUFFIX} PROPERTIES OUTPUT_NAME "msvcp140_2${D_SUFFIX}${VCLIBS_SUFFIX}")
    target_link_options(msvcp_2${D_SUFFIX} PRIVATE "${THIS_CONFIG_LINK_OPTIONS}")

    # msvcp140_codecvt_ids.dll
    add_library(msvcp${D_SUFFIX}_codecvt_ids_objects OBJECT ${SOURCES_SATELLITE_CODECVT_IDS})
    target_compile_definitions(msvcp${D_SUFFIX}_codecvt_ids_objects PRIVATE "_BUILDING_SATELLITE_CODECVT_IDS;_DLL;${THIS_CONFIG_DEFINITIONS}")
    target_compile_options(msvcp${D_SUFFIX}_codecvt_ids_objects PRIVATE "${THIS_CONFIG_COMPILE_OPTIONS};${GL_FLAG};/EHsc")

    add_library(msvcp${D_SUFFIX}_codecvt_ids SHARED)
    target_link_libraries(msvcp${D_SUFFIX}_codecvt_ids PRIVATE msvcp${D_SUFFIX}_codecvt_ids_objects "msvcp${D_SUFFIX}" "${TOOLSET_LIB}/vcruntime${D_SUFFIX}.lib" "${TOOLSET_LIB}/msvcrt${D_SUFFIX}.lib" "ucrt${D_SUFFIX}.lib")
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

    add_library(libcpmt${FLAVOR_SUFFIX} STATIC ${HEADERS} ${IMPLIB_SOURCES} ${SOURCES} ${STATIC_SOURCES})
    target_compile_definitions(libcpmt${FLAVOR_SUFFIX} PRIVATE "${THIS_CONFIG_DEFINITIONS}")
    target_compile_options(libcpmt${FLAVOR_SUFFIX} PRIVATE "${THIS_CONFIG_COMPILE_OPTIONS};/EHsc")
    target_link_libraries(libcpmt${FLAVOR_SUFFIX} PRIVATE Boost::headers Boost::disable_autolinking libcpmt${FLAVOR_SUFFIX}_eha)
endfunction()

add_stl_statics("" "_ITERATOR_DEBUG_LEVEL=0" "${VCLIBS_RELEASE_OPTIONS}")
add_stl_statics("1" "_ITERATOR_DEBUG_LEVEL=1" "${VCLIBS_RELEASE_OPTIONS}")
add_stl_statics("d" "_DEBUG;_ITERATOR_DEBUG_LEVEL=2" "${VCLIBS_DEBUG_OPTIONS}")
add_stl_statics("d1" "_DEBUG;_ITERATOR_DEBUG_LEVEL=1" "${VCLIBS_DEBUG_OPTIONS}")
add_stl_statics("d0" "_DEBUG;_ITERATOR_DEBUG_LEVEL=0" "${VCLIBS_DEBUG_OPTIONS}")
