// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

export module std.compat;

export import std;

// "C++ headers for C library facilities" [tab:headers.cpp.c]

// <cassert>
// Nothing to do, only macros.

// <cctype>
export using std::isalnum;
export using std::isalpha;
export using std::isblank;
export using std::iscntrl;
export using std::isdigit;
export using std::isgraph;
export using std::islower;
export using std::isprint;
export using std::ispunct;
export using std::isspace;
export using std::isupper;
export using std::isxdigit;
export using std::tolower;
export using std::toupper;

// <cerrno>
// Nothing to do, only macros.

// <cfenv>
export using std::fenv_t;
export using std::fexcept_t;
export using std::feclearexcept;
export using std::fegetexceptflag;
export using std::feraiseexcept;
export using std::fesetexceptflag;
export using std::fetestexcept;
export using std::fegetround;
export using std::fesetround;
export using std::fegetenv;
export using std::feholdexcept;
export using std::fesetenv;
export using std::feupdateenv;

// <cfloat>
// Nothing to do, only macros.

// <cinttypes>
export using std::imaxdiv_t;
export using std::imaxabs;
export using std::imaxdiv;
export using std::strtoimax;
export using std::strtoumax;
export using std::wcstoimax;
export using std::wcstoumax;

// <climits>
// Nothing to do, only macros.

// <clocale>
export using std::lconv;
export using std::setlocale;
export using std::localeconv;

// <cmath>
export using std::float_t;
export using std::double_t;
export using std::acos;
export using std::acosf;
export using std::acosl;
export using std::asin;
export using std::asinf;
export using std::asinl;
export using std::atan;
export using std::atanf;
export using std::atanl;
export using std::atan2;
export using std::atan2f;
export using std::atan2l;
export using std::cos;
export using std::cosf;
export using std::cosl;
export using std::sin;
export using std::sinf;
export using std::sinl;
export using std::tan;
export using std::tanf;
export using std::tanl;
export using std::acosh;
export using std::acoshf;
export using std::acoshl;
export using std::asinh;
export using std::asinhf;
export using std::asinhl;
export using std::atanh;
export using std::atanhf;
export using std::atanhl;
export using std::cosh;
export using std::coshf;
export using std::coshl;
export using std::sinh;
export using std::sinhf;
export using std::sinhl;
export using std::tanh;
export using std::tanhf;
export using std::tanhl;
export using std::exp;
export using std::expf;
export using std::expl;
export using std::exp2;
export using std::exp2f;
export using std::exp2l;
export using std::expm1;
export using std::expm1f;
export using std::expm1l;
export using std::frexp;
export using std::frexpf;
export using std::frexpl;
export using std::ilogb;
export using std::ilogbf;
export using std::ilogbl;
export using std::ldexp;
export using std::ldexpf;
export using std::ldexpl;
export using std::log;
export using std::logf;
export using std::logl;
export using std::log10;
export using std::log10f;
export using std::log10l;
export using std::log1p;
export using std::log1pf;
export using std::log1pl;
export using std::log2;
export using std::log2f;
export using std::log2l;
export using std::logb;
export using std::logbf;
export using std::logbl;
export using std::modf;
export using std::modff;
export using std::modfl;
export using std::scalbn;
export using std::scalbnf;
export using std::scalbnl;
export using std::scalbln;
export using std::scalblnf;
export using std::scalblnl;
export using std::cbrt;
export using std::cbrtf;
export using std::cbrtl;
export using std::abs;
export using std::fabs;
export using std::fabsf;
export using std::fabsl;
export using std::_Binary_hypot::hypot; // export only C's hypot(x, y), not C++'s hypot(x, y, z)
export using std::hypotf;
export using std::hypotl;
export using std::pow;
export using std::powf;
export using std::powl;
export using std::sqrt;
export using std::sqrtf;
export using std::sqrtl;
export using std::erf;
export using std::erff;
export using std::erfl;
export using std::erfc;
export using std::erfcf;
export using std::erfcl;
export using std::lgamma;
export using std::lgammaf;
export using std::lgammal;
export using std::tgamma;
export using std::tgammaf;
export using std::tgammal;
export using std::ceil;
export using std::ceilf;
export using std::ceill;
export using std::floor;
export using std::floorf;
export using std::floorl;
export using std::nearbyint;
export using std::nearbyintf;
export using std::nearbyintl;
export using std::rint;
export using std::rintf;
export using std::rintl;
export using std::lrint;
export using std::lrintf;
export using std::lrintl;
export using std::llrint;
export using std::llrintf;
export using std::llrintl;
export using std::round;
export using std::roundf;
export using std::roundl;
export using std::lround;
export using std::lroundf;
export using std::lroundl;
export using std::llround;
export using std::llroundf;
export using std::llroundl;
export using std::trunc;
export using std::truncf;
export using std::truncl;
export using std::fmod;
export using std::fmodf;
export using std::fmodl;
export using std::remainder;
export using std::remainderf;
export using std::remainderl;
export using std::remquo;
export using std::remquof;
export using std::remquol;
export using std::copysign;
export using std::copysignf;
export using std::copysignl;
export using std::nan;
export using std::nanf;
export using std::nanl;
export using std::nextafter;
export using std::nextafterf;
export using std::nextafterl;
export using std::nexttoward;
export using std::nexttowardf;
export using std::nexttowardl;
export using std::fdim;
export using std::fdimf;
export using std::fdiml;
export using std::fmax;
export using std::fmaxf;
export using std::fmaxl;
export using std::fmin;
export using std::fminf;
export using std::fminl;
export using std::fma;
export using std::fmaf;
export using std::fmal;
export using std::fpclassify;
export using std::isfinite;
export using std::isinf;
export using std::isnan;
export using std::isnormal;
export using std::signbit;
export using std::isgreater;
export using std::isgreaterequal;
export using std::isless;
export using std::islessequal;
export using std::islessgreater;
export using std::isunordered;

// <csetjmp>
export using std::jmp_buf;
export using std::longjmp;

// <csignal>
export using std::sig_atomic_t;
export using std::signal;
export using std::raise;

// <cstdarg>
export using std::va_list;

// <cstddef>
export using std::ptrdiff_t;
export using std::size_t;
export using std::max_align_t;
export using std::nullptr_t; // added to the C Standard Library in C23

// <cstdint>
export using std::int8_t;
export using std::int16_t;
export using std::int32_t;
export using std::int64_t;
export using std::int_fast8_t;
export using std::int_fast16_t;
export using std::int_fast32_t;
export using std::int_fast64_t;
export using std::int_least8_t;
export using std::int_least16_t;
export using std::int_least32_t;
export using std::int_least64_t;
export using std::intmax_t;
export using std::intptr_t;
export using std::uint8_t;
export using std::uint16_t;
export using std::uint32_t;
export using std::uint64_t;
export using std::uint_fast8_t;
export using std::uint_fast16_t;
export using std::uint_fast32_t;
export using std::uint_fast64_t;
export using std::uint_least8_t;
export using std::uint_least16_t;
export using std::uint_least32_t;
export using std::uint_least64_t;
export using std::uintmax_t;
export using std::uintptr_t;

// <cstdio>
// already exported std::size_t, see <cstddef>
export using std::FILE;
export using std::fpos_t;
export using std::remove;
export using std::rename;
export using std::tmpfile;
export using std::tmpnam;
export using std::fclose;
export using std::fflush;
export using std::fopen;
export using std::freopen;
export using std::setbuf;
export using std::setvbuf;
export using std::fprintf;
export using std::fscanf;
export using std::printf;
export using std::scanf;
export using std::snprintf;
export using std::sprintf;
export using std::sscanf;
export using std::vfprintf;
export using std::vfscanf;
export using std::vprintf;
export using std::vscanf;
export using std::vsnprintf;
export using std::vsprintf;
export using std::vsscanf;
export using std::fgetc;
export using std::fgets;
export using std::fputc;
export using std::fputs;
export using std::getc;
export using std::getchar;
export using std::putc;
export using std::putchar;
export using std::puts;
export using std::ungetc;
export using std::fread;
export using std::fwrite;
export using std::fgetpos;
export using std::fseek;
export using std::fsetpos;
export using std::ftell;
export using std::rewind;
export using std::clearerr;
export using std::feof;
export using std::ferror;
export using std::perror;

// <cstdlib>
// already exported std::size_t, see <cstddef>
export using std::div_t;
export using std::ldiv_t;
export using std::lldiv_t;
export using std::abort;
export using std::atexit;
export using std::at_quick_exit;
export using std::exit;
export using std::_Exit;
export using std::quick_exit;
export using std::getenv;
export using std::system;
export using std::calloc;
export using std::free;
export using std::malloc;
export using std::realloc;
export using std::atof;
export using std::atoi;
export using std::atol;
export using std::atoll;
export using std::strtod;
export using std::strtof;
export using std::strtold;
export using std::strtol;
export using std::strtoll;
export using std::strtoul;
export using std::strtoull;
export using std::mblen;
export using std::mbtowc;
export using std::wctomb;
export using std::mbstowcs;
export using std::wcstombs;
export using std::bsearch;
export using std::qsort;
export using std::rand;
export using std::srand;
// already exported std::abs, see <cmath>
export using std::labs;
export using std::llabs;
export using std::div;
export using std::ldiv;
export using std::lldiv;

// <cstring>
// already exported std::size_t, see <cstddef>
export using std::memcpy;
export using std::memmove;
export using std::strcpy;
export using std::strncpy;
export using std::strcat;
export using std::strncat;
export using std::memcmp;
export using std::strcmp;
export using std::strcoll;
export using std::strncmp;
export using std::strxfrm;
export using std::memchr;
export using std::strchr;
export using std::strcspn;
export using std::strpbrk;
export using std::strrchr;
export using std::strspn;
export using std::strstr;
export using std::strtok;
export using std::memset;
export using std::strerror;
export using std::strlen;

// <ctime>
// already exported std::size_t, see <cstddef>
export using std::clock_t;
export using std::time_t;
export using std::timespec;
export using std::tm;
export using std::clock;
export using std::difftime;
export using std::mktime;
export using std::time;
export using std::timespec_get;
export using std::asctime;
export using std::ctime;
export using std::gmtime;
export using std::localtime;
export using std::strftime;

// <cuchar>
export using std::mbstate_t;
// already exported std::size_t, see <cstddef>
export using std::mbrtoc16;
export using std::c16rtomb;
export using std::mbrtoc32;
export using std::c32rtomb;

// <cwchar>
// already exported std::size_t, see <cstddef>
// already exported std::mbstate_t, see <cuchar>
export using std::wint_t;
// already exported std::tm, see <ctime>
export using std::fwprintf;
export using std::fwscanf;
export using std::swprintf;
export using std::swscanf;
export using std::vfwprintf;
export using std::vfwscanf;
export using std::vswprintf;
export using std::vswscanf;
export using std::vwprintf;
export using std::vwscanf;
export using std::wprintf;
export using std::wscanf;
export using std::fgetwc;
export using std::fgetws;
export using std::fputwc;
export using std::fputws;
export using std::fwide;
export using std::getwc;
export using std::getwchar;
export using std::putwc;
export using std::putwchar;
export using std::ungetwc;
export using std::wcstod;
export using std::wcstof;
export using std::wcstold;
export using std::wcstol;
export using std::wcstoll;
export using std::wcstoul;
export using std::wcstoull;
export using std::wcscpy;
export using std::wcsncpy;
export using std::wmemcpy;
export using std::wmemmove;
export using std::wcscat;
export using std::wcsncat;
export using std::wcscmp;
export using std::wcscoll;
export using std::wcsncmp;
export using std::wcsxfrm;
export using std::wmemcmp;
export using std::wcschr;
export using std::wcscspn;
export using std::wcspbrk;
export using std::wcsrchr;
export using std::wcsspn;
export using std::wcsstr;
export using std::wcstok;
export using std::wmemchr;
export using std::wcslen;
export using std::wmemset;
export using std::wcsftime;
export using std::btowc;
export using std::wctob;
export using std::mbsinit;
export using std::mbrlen;
export using std::mbrtowc;
export using std::wcrtomb;
export using std::mbsrtowcs;
export using std::wcsrtombs;

// <cwctype>
// already exported std::wint_t, see <cwchar>
export using std::wctrans_t;
export using std::wctype_t;
export using std::iswalnum;
export using std::iswalpha;
export using std::iswblank;
export using std::iswcntrl;
export using std::iswdigit;
export using std::iswgraph;
export using std::iswlower;
export using std::iswprint;
export using std::iswpunct;
export using std::iswspace;
export using std::iswupper;
export using std::iswxdigit;
export using std::iswctype;
export using std::wctype;
export using std::towlower;
export using std::towupper;
export using std::towctrans;
export using std::wctrans;
