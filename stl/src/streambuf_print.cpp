// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// streambuf_print.cpp -- support std::ostream overloads of std::vprint_unicode() without RTTI

#include <fstream>
#include <shared_mutex>
#include <streambuf>
#include <unordered_set>

namespace {
    class _Printable_streambuf_cache final {
    private:
        _STD unordered_set<const _STD streambuf*> _Stream_buf_set;
        _STD shared_mutex _Lock;

        _Printable_streambuf_cache() = default;

    public:
        _Printable_streambuf_cache(const _Printable_streambuf_cache&)            = delete;
        _Printable_streambuf_cache& operator=(const _Printable_streambuf_cache&) = delete;

        _Printable_streambuf_cache(_Printable_streambuf_cache&&)            = delete;
        _Printable_streambuf_cache& operator=(_Printable_streambuf_cache&&) = delete;

        static _Printable_streambuf_cache& _Get_instance() {
            static _Printable_streambuf_cache _Instance{};
            return _Instance;
        }


    };
} // unnamed namespace