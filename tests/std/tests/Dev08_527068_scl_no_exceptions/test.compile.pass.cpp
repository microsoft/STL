// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

//////////////////////////////////////////////////////////////////////////////////////////

// Bug : VSW527068
// The Standard c++ library fails to compile when
// 1) _HAS_EXCEPTIONS=0
// AND
// 2) "exception" is included as one of the header files


// Problem :
// When _HAS_EXCEPTIONS=0, the header defines another std::exception class which
// conflicts with the regular (_HAS_EXCEPTIONS=1) std::exception. The fix was
// to move the std::exception class into stdext namespace and use
// "using stdext::exception;" to pull it into the std namespace when _HAS_EXCEPTIONS=0.


// Test :
// Make sure all the combinations of _HAS_EXCEPTIONS=0|1, /MD[d] /MT[d], and
// _STATIC compiles.
//
// Make sure that std::exception is defined when _HAS_EXCEPTIONS is both 0 and 1.
//
// Make sure that stdext::exception is defined when _HAS_EXCEPTIONS is 1.

//////////////////////////////////////////////////////////////////////////////////////////

// compile with: /EHsc

// VSO-115482 - Include <typeinfo> first to ensure when _HAS_EXCEPTIONS=0, it doesn't emit any errors without including
// <exception> beforehand.
#include <typeinfo>

// This comment prevents clang-format from reordering <typeinfo> below the other headers.

#include <exception>
#include <iostream>

// VSO-115482 - Ensure type_info is in namespace std when _HAS_EXCEPTIONS=0.
using check_typeinfo = std::type_info;

void MyFunc() {
    _THROW(std::exception("this is an exception"));
}

void __cdecl handler(const std::exception& e) {
    std::cout << "in handler, exception: " << e.what() << std::endl;
}

void meow() {
#if !_HAS_EXCEPTIONS
    stdext::exception::_Set_raise_handler(&handler);
#endif
    _TRY_BEGIN
    std::cout << "In try block, calling MyFunc()." << std::endl;
    MyFunc();
    _CATCH(std::exception e)
#if _HAS_EXCEPTIONS
    std::cout << "In catch handler." << std::endl;
    std::cout << "Caught CTest exception type: ";
    std::cout << e.what() << std::endl;
#endif
    _CATCH_END
    std::cout << "Back in main. Execution resumes here." << std::endl;
}
