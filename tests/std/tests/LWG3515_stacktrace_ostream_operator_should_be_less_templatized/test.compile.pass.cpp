// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <memory>
#include <memory_resource>
#include <ostream>
#include <sstream>
#include <stacktrace>
#include <string>

using namespace std;

template <class Ostream, class Alloc = allocator<stacktrace_entry>>
concept CanPrintStacktrace = requires(Ostream& os, const stacktrace_entry& f, const basic_stacktrace<Alloc>& st) {
                                 os << f;
                                 os << st;
                             };

template <class CharT>
struct FancyCharTraits : char_traits<CharT> {};

static_assert(CanPrintStacktrace<ostream>);
static_assert(CanPrintStacktrace<ostringstream>);
static_assert(CanPrintStacktrace<ostream, pmr::polymorphic_allocator<stacktrace_entry>>);

static_assert(!CanPrintStacktrace<wostream>);
static_assert(!CanPrintStacktrace<wostringstream>);
static_assert(!CanPrintStacktrace<wostream, pmr::polymorphic_allocator<stacktrace_entry>>);

using FancyCharStream = basic_ostream<char, FancyCharTraits<char>>;
static_assert(!CanPrintStacktrace<FancyCharStream>);
static_assert(!CanPrintStacktrace<FancyCharStream, pmr::polymorphic_allocator<stacktrace_entry>>);

using FancyWcharStream = basic_ostream<wchar_t, FancyCharTraits<wchar_t>>;
static_assert(!CanPrintStacktrace<FancyWcharStream>);
static_assert(!CanPrintStacktrace<FancyWcharStream, pmr::polymorphic_allocator<stacktrace_entry>>);

int main() {} // COMPILE-ONLY
