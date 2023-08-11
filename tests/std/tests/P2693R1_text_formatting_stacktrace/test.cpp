// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory_resource>
#include <ranges>
#include <stacktrace>
#include <string>
#include <type_traits>

#include "test_format_support.hpp"

using namespace std;

template <template <class> class Fmt>
void check_stacktrace_entry_formatter() {
    Fmt<char> fmt;
    const stacktrace_entry frame = stacktrace::current()[0];
    const auto frame_str         = to_string(frame);

    // empty format-spec
    assert(fmt("{}", frame) == frame_str);
    assert(fmt("{:}", frame) == frame_str);
    assert(fmt("{0:}", frame) == frame_str);

    // align only
    assert(fmt("{:<}", frame) == frame_str);
    assert(fmt("{:^}", frame) == frame_str);
    assert(fmt("{:>}", frame) == frame_str);

    // fill-and-align only
    assert(fmt("{::<}", frame) == frame_str);
    assert(fmt("{:*^}", frame) == frame_str);
    assert(fmt("{:=>}", frame) == frame_str);

    { // width only
        constexpr int width  = 560;
        const int fill_width = static_cast<int>(width - clamp(frame_str.size(), size_t{0}, size_t{width}));

        const auto s  = fmt("{:560}", frame);
        const auto it = ranges::mismatch(views::reverse(s), views::repeat(' ', fill_width)).in1;
        assert(it.base() == s.end() - fill_width);
        assert(ranges::equal(ranges::subrange{s.begin(), it.base()}, frame_str));
    }

    { // width only (replacement field)
        constexpr int width  = 470;
        const int fill_width = static_cast<int>(width - clamp(frame_str.size(), size_t{0}, size_t{width}));

        const auto s  = fmt("{:{}}", frame, width);
        const auto it = ranges::mismatch(views::reverse(s), views::repeat(' ', fill_width)).in1;
        assert(it.base() == s.end() - fill_width);
        assert(ranges::equal(ranges::subrange{s.begin(), it.base()}, frame_str));
    }

    { // fill-and-align with width
        constexpr int width  = 400;
        const int fill_width = static_cast<int>(width - clamp(frame_str.size(), size_t{0}, size_t{width}));

        {
            const auto s  = fmt("{:=<400}", frame);
            const auto it = ranges::mismatch(views::reverse(s), views::repeat('=', fill_width)).in1;
            assert(it.base() == s.end() - fill_width);
            assert(ranges::equal(ranges::subrange{s.begin(), it.base()}, frame_str));
        }

        {
            const auto s              = fmt("{::^400}", frame);
            const int left_fill_width = fill_width / 2;
            const auto it1            = ranges::mismatch(s, views::repeat(':', left_fill_width)).in1;
            assert(it1 == s.begin() + left_fill_width);

            const int right_fill_width = fill_width - left_fill_width;
            const auto it2             = ranges::mismatch(views::reverse(s), views::repeat(':', right_fill_width)).in1;
            assert(it2.base() == s.end() - right_fill_width);

            assert(ranges::equal(ranges::subrange{it1, it2.base()}, frame_str));
        }

        {
            const auto s  = fmt("{:*>400}", frame);
            const auto it = ranges::mismatch(s, views::repeat('*', fill_width)).in1;
            assert(it == s.begin() + fill_width);
            assert(ranges::equal(ranges::subrange{it, s.end()}, frame_str));
        }
    }

    { // fill-and-align with width (replacement field)
        constexpr int width  = 390;
        const int fill_width = static_cast<int>(width - clamp(frame_str.size(), size_t{0}, size_t{width}));

        {
            const auto s  = fmt("{:/<{}}", frame, width);
            const auto it = ranges::mismatch(views::reverse(s), views::repeat('/', fill_width)).in1;
            assert(it.base() == s.end() - fill_width);
            assert(ranges::equal(ranges::subrange{s.begin(), it.base()}, frame_str));
        }

        {
            const auto s              = fmt("{::^{}}", frame, width);
            const int left_fill_width = fill_width / 2;
            const auto it1            = ranges::mismatch(s, views::repeat(':', left_fill_width)).in1;
            assert(it1 == s.begin() + left_fill_width);

            const int right_fill_width = fill_width - left_fill_width;
            const auto it2             = ranges::mismatch(views::reverse(s), views::repeat(':', right_fill_width)).in1;
            assert(it2.base() == s.end() - right_fill_width);

            assert(ranges::equal(ranges::subrange{it1, it2.base()}, frame_str));
        }

        {
            const auto s  = fmt("{0:_>{1}}", frame, width);
            const auto it = ranges::mismatch(s, views::repeat('_', fill_width)).in1;
            assert(it == s.begin() + fill_width);
            assert(ranges::equal(ranges::subrange{it, s.end()}, frame_str));
        }
    }
}

void check_stacktrace_entry_formatter_with_invalid_fields() {
    ExpectFormatError<char> fmt;
    const stacktrace_entry frame;

    { // damaged fields
        fmt("{", frame);
        fmt("{:", frame);
        fmt("{}}", frame);
        fmt("}", frame);
    }

    { // fill-and-align should not contain '{' or '}' characters
        fmt("{:{^}", frame);
        fmt("{:{<}", frame);
    }

    { // sign, #, 0, precision, L, or type options are not allowed in format-specs
        fmt("{:+}", frame);
        fmt("{:#}", frame);
        fmt("{:0}", frame);
        fmt("{:.5}", frame);
        fmt("{:.{}}", frame, 5);
        fmt("{:L}", frame);
        fmt("{:d}", frame);
    }

    { // invalid width replacement field
        fmt("{0:{0}}", frame, 5);
        fmt("{0:{2}}", frame, 5);
    }

    { // mixed invalid format-specs
        fmt("{:+#0}", frame);
        fmt("{:=^8.8}", frame);
        fmt("{0:{1}.{1}}", frame, 5);
        fmt("{:Lx}", frame);
    }
}

template <template <class> class Fmt>
void check_basic_stacktrace_formatter() {
    Fmt<char> fmt;

    { // Allocator == allocator
        stacktrace empty_trace;
        assert(fmt("{}", empty_trace) == to_string(empty_trace));

        auto current_trace = stacktrace::current();
        assert(fmt("{}", current_trace) == to_string(current_trace));
    }

    { // Allocator == pmr::polymorphic_allocator
        pmr::stacktrace empty_trace{pmr::new_delete_resource()};
        assert(fmt("{}", empty_trace) == to_string(empty_trace));

        pmr::synchronized_pool_resource res;
        auto current_trace = pmr::stacktrace::current(&res);
        assert(fmt("{}", current_trace) == to_string(current_trace));
    }
}

void check_basic_stacktrace_formatter_with_invalid_specs() {
    ExpectFormatError<char> fmt;
    stacktrace trace;

    { // damaged fields
        fmt("{", trace);
        fmt("{:", trace);
        fmt("{}}", trace);
        fmt("}", trace);
    }

    { // format-spec should be empty
        fmt("{:{^}", trace);
        fmt("{:{<}", trace);
        fmt("{:+}", trace);
        fmt("{:#}", trace);
        fmt("{:0}", trace);
        fmt("{:.5}", trace);
        fmt("{:.{}}", trace, 8);
        fmt("{:L}", trace);
        fmt("{:d}", trace);
        fmt("{:+#0}", trace);
        fmt("{:=^5.5}", trace);
        fmt("{0:{1}.{1}}", trace, 9);
        fmt("{:Lx}", trace);
    }
}

int main() {
    try {
        check_stacktrace_entry_formatter<FormatFn>();
        check_stacktrace_entry_formatter<VFormatFn>();
        check_stacktrace_entry_formatter<MoveOnlyFormat>();
        check_stacktrace_entry_formatter_with_invalid_fields();
        check_basic_stacktrace_formatter<FormatFn>();
        check_basic_stacktrace_formatter<VFormatFn>();
        check_basic_stacktrace_formatter<MoveOnlyFormat>();
        check_basic_stacktrace_formatter_with_invalid_specs();
    } catch (const exception& e) {
        cerr << "Exception caught: " << e.what() << endl;
        exit(EXIT_FAILURE);
    } catch (...) {
        cerr << "Unknown exception caught." << endl;
        exit(EXIT_FAILURE);
    }
}
