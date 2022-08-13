// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <ios>
#include <limits>
#include <span>
#include <spanstream>
#include <string_view>
#include <type_traits>
#include <utility>

using namespace std;

template <class CharT>
inline constexpr auto input_ptr = "1 2 3 4 5";
template <>
inline constexpr auto input_ptr<wchar_t> = L"1 2 3 4 5";

template <class CharT>
inline constexpr auto input_view = "1 2 3 4 5"sv;
template <>
inline constexpr auto input_view<wchar_t> = L"1 2 3 4 5"sv;

template <class CharT>
inline constexpr array input_std_array{'1', ' ', '2', ' ', '3', ' ', '4', ' ', '5'};
template <>
inline constexpr array input_std_array<wchar_t>{L'1', L' ', L'2', L' ', L'3', L' ', L'4', L' ', L'5'};

template <class Spanbuf>
class basic_test_buf : public Spanbuf {
public:
    using Spanbuf::Spanbuf;

    using Spanbuf::eback;
    using Spanbuf::egptr;
    using Spanbuf::epptr;
    using Spanbuf::gptr;
    using Spanbuf::pbase;
    using Spanbuf::pptr;

    using Spanbuf::setp;

    using Spanbuf::seekoff;
    using Spanbuf::seekpos;
    using Spanbuf::setbuf;
};

template <class CharT>
void test_spanbuf() {
    using test_buf = basic_test_buf<basic_spanbuf<CharT>>;
    { // construction
        CharT buffer[10];
        const test_buf default_constructed{};
        assert(default_constructed.span().data() == nullptr);
        assert(default_constructed.eback() == nullptr);
        assert(default_constructed.gptr() == nullptr);
        assert(default_constructed.egptr() == nullptr);
        assert(default_constructed.pbase() == nullptr);
        assert(default_constructed.pptr() == nullptr);
        assert(default_constructed.epptr() == nullptr);

        const test_buf mode_constructed{ios_base::in};
        assert(mode_constructed.span().data() == nullptr);
        assert(mode_constructed.eback() == nullptr);
        assert(mode_constructed.gptr() == nullptr);
        assert(mode_constructed.egptr() == nullptr);
        assert(mode_constructed.pbase() == nullptr);
        assert(mode_constructed.pptr() == nullptr);
        assert(mode_constructed.epptr() == nullptr);

        test_buf span_constructed{span<CharT>{buffer}};
        assert(span_constructed.span().data() == buffer);
        assert(span_constructed.eback() == buffer);
        assert(span_constructed.gptr() == buffer);
        assert(span_constructed.egptr() == end(buffer));
        assert(span_constructed.pbase() == buffer);
        assert(span_constructed.pptr() == buffer);
        assert(span_constructed.epptr() == end(buffer));

        const test_buf span_mode_in_constructed{span<CharT>{buffer}, ios_base::in};
        assert(span_mode_in_constructed.span().data() == buffer);
        assert(span_mode_in_constructed.eback() == buffer);
        assert(span_mode_in_constructed.gptr() == buffer);
        assert(span_mode_in_constructed.egptr() == end(buffer));
        assert(span_mode_in_constructed.pbase() == nullptr);
        assert(span_mode_in_constructed.pptr() == nullptr);
        assert(span_mode_in_constructed.epptr() == nullptr);

        const test_buf span_mode_in_ate_constructed{span<CharT>{buffer}, ios_base::in | ios_base::ate};
        assert(span_mode_in_ate_constructed.span().data() == buffer);
        assert(span_mode_in_ate_constructed.eback() == buffer);
        assert(span_mode_in_ate_constructed.gptr() == buffer);
        assert(span_mode_in_ate_constructed.egptr() == end(buffer));
        assert(span_mode_in_ate_constructed.pbase() == nullptr);
        assert(span_mode_in_ate_constructed.pptr() == nullptr);
        assert(span_mode_in_ate_constructed.epptr() == nullptr);

        const test_buf span_mode_out_constructed{span<CharT>{buffer}, ios_base::out};
        assert(span_mode_out_constructed.span().data() == buffer);
        assert(span_mode_out_constructed.eback() == nullptr);
        assert(span_mode_out_constructed.gptr() == nullptr);
        assert(span_mode_out_constructed.egptr() == nullptr);
        assert(span_mode_out_constructed.pbase() == buffer);
        assert(span_mode_out_constructed.pptr() == buffer);
        assert(span_mode_out_constructed.epptr() == end(buffer));

        const test_buf span_mode_out_ate_constructed{span<CharT>{buffer}, ios_base::out | ios_base::ate};
        assert(span_mode_out_ate_constructed.span().data() == buffer);
        assert(span_mode_out_ate_constructed.eback() == nullptr);
        assert(span_mode_out_ate_constructed.gptr() == nullptr);
        assert(span_mode_out_ate_constructed.egptr() == nullptr);
        assert(span_mode_out_ate_constructed.pbase() == buffer);
        assert(span_mode_out_ate_constructed.pptr() == end(buffer));
        assert(span_mode_out_ate_constructed.epptr() == end(buffer));

        const test_buf span_mode_unknown_constructed{span<CharT>{buffer}, 0};
        assert(span_mode_unknown_constructed.span().data() == buffer);
        assert(span_mode_unknown_constructed.eback() == nullptr);
        assert(span_mode_unknown_constructed.gptr() == nullptr);
        assert(span_mode_unknown_constructed.egptr() == nullptr);
        assert(span_mode_unknown_constructed.pbase() == nullptr);
        assert(span_mode_unknown_constructed.pptr() == nullptr);
        assert(span_mode_unknown_constructed.epptr() == nullptr);

        test_buf move_constructed{move(span_constructed)};
        assert(move_constructed.span().data() == buffer);
        assert(move_constructed.eback() == buffer);
        assert(move_constructed.gptr() == buffer);
        assert(move_constructed.egptr() == end(buffer));
        assert(move_constructed.pbase() == buffer);
        assert(move_constructed.pptr() == buffer);
        assert(move_constructed.epptr() == end(buffer));
        assert(span_constructed.span().data() == nullptr);
        assert(span_constructed.eback() == nullptr);
        assert(span_constructed.gptr() == nullptr);
        assert(span_constructed.egptr() == nullptr);
        assert(span_constructed.pbase() == nullptr);
        assert(span_constructed.pptr() == nullptr);
        assert(span_constructed.epptr() == nullptr);

        test_buf move_assigned;
        move_assigned = move(move_constructed);
        assert(move_assigned.span().data() == buffer);
        assert(move_assigned.eback() == buffer);
        assert(move_assigned.gptr() == buffer);
        assert(move_assigned.egptr() == end(buffer));
        assert(move_assigned.pbase() == buffer);
        assert(move_assigned.pptr() == buffer);
        assert(move_assigned.epptr() == end(buffer));
        assert(move_constructed.span().data() == nullptr);
        assert(move_constructed.eback() == nullptr);
        assert(move_constructed.gptr() == nullptr);
        assert(move_constructed.egptr() == nullptr);
        assert(move_constructed.pbase() == nullptr);
        assert(move_constructed.pptr() == nullptr);
        assert(move_constructed.epptr() == nullptr);
    }

    { // swap
        CharT buffer1[10];
        CharT buffer2[20];
        test_buf first{span<CharT>{buffer1}};
        test_buf second{span<CharT>{buffer2}};
        assert(first.span().data() == buffer1);
        assert(second.span().data() == buffer2);

        first.swap(second);
        assert(first.span().data() == buffer2);
        assert(second.span().data() == buffer1);

        swap(first, second);
        assert(first.span().data() == buffer1);
        assert(second.span().data() == buffer2);
    }

    { // span, span, span, span
        CharT buffer1[10];
        test_buf input_buffer{span<CharT>{buffer1}, ios_base::in};
        assert(input_buffer.span().data() == buffer1);
        assert(input_buffer.span().size() == size(buffer1));

        test_buf output_buffer{span<CharT>{buffer1}, ios_base::out};
        assert(output_buffer.span().data() == buffer1);
        assert(output_buffer.span().size() == 0); // counts the written characters

        // Manually move the written pointer
        output_buffer.setp(buffer1, buffer1 + 5, end(buffer1));
        assert(output_buffer.span().data() == buffer1);
        assert(output_buffer.span().size() == 5);

        CharT buffer2[10];
        input_buffer.span(span<CharT>{buffer2});
        assert(input_buffer.span().data() == buffer2);
        assert(input_buffer.span().size() == size(buffer2));

        output_buffer.span(span<CharT>{buffer2});
        assert(output_buffer.span().data() == buffer2);
        assert(output_buffer.span().size() == 0);

        test_buf hungry_buffer{span<CharT>{buffer1}, ios_base::out | ios_base::ate};
        assert(hungry_buffer.span().data() == buffer1);
        assert(hungry_buffer.span().size() == size(buffer1));

        hungry_buffer.span(span<CharT>{buffer2});
        assert(hungry_buffer.span().data() == buffer2);
        assert(hungry_buffer.span().size() == size(buffer2));
    }

    { // seekoff ios_base::beg
        CharT buffer[10];
        test_buf input_buffer{span<CharT>{buffer}, ios_base::in};
        test_buf output_buffer{span<CharT>{buffer}, ios_base::out};

        auto result = input_buffer.seekoff(0, ios_base::beg, ios_base::in);
        assert(result == 0);

        // pptr not set but off is 0
        result = input_buffer.seekoff(0, ios_base::beg, ios_base::out);
        assert(result == 0);

        // pptr not set and off != 0 -> fail
        result = input_buffer.seekoff(1, ios_base::beg, ios_base::out);
        assert(result == -1);

        // gptr not set but off is 0
        result = output_buffer.seekoff(0, ios_base::beg, ios_base::in);
        assert(result == 0);

        // gptr not set and off != 0 -> fail
        result = output_buffer.seekoff(1, ios_base::beg, ios_base::in);
        assert(result == -1);

        // negative off -> fail
        result = input_buffer.seekoff(-1, ios_base::beg, ios_base::in);
        assert(result == -1);

        // negative off -> fail
        result = output_buffer.seekoff(-1, ios_base::beg, ios_base::out);
        assert(result == -1);

        // off larger than buf -> fail
        result = input_buffer.seekoff(20, ios_base::beg, ios_base::in);
        assert(result == -1);

        // off larger than buf -> fail
        result = output_buffer.seekoff(20, ios_base::beg, ios_base::out);
        assert(result == -1);

        // passes
        result = input_buffer.seekoff(5, ios_base::beg, ios_base::in);
        assert(result == 5);

        result = output_buffer.seekoff(5, ios_base::beg, ios_base::out);
        assert(result == 5);

        // always from front
        result = input_buffer.seekoff(7, ios_base::beg, ios_base::in);
        assert(result == 7);

        result = output_buffer.seekoff(7, ios_base::beg, ios_base::out);
        assert(result == 7);
    }

    { // seekoff ios_base::end
        CharT buffer[10];
        test_buf input_buffer{span<CharT>{buffer}, ios_base::in};
        // all fine we move to end of stream
        auto result = input_buffer.seekoff(0, ios_base::end, ios_base::in);
        assert(result == 10);

        // pptr not set but off is == 0
        result = input_buffer.seekoff(-10, ios_base::end, ios_base::out);
        assert(result == 0);

        // pptr not set and off != 0 -> fail
        result = input_buffer.seekoff(0, ios_base::end, ios_base::out);
        assert(result == -1);

        // negative off -> fail
        result = input_buffer.seekoff(-20, ios_base::end, ios_base::in);
        assert(result == -1);

        // off beyond end of buffer -> fail
        result = input_buffer.seekoff(1, ios_base::end, ios_base::in);
        assert(result == -1);

        // passes and moves to buffer size - off
        result = input_buffer.seekoff(-5, ios_base::end, ios_base::in);
        assert(result == 5);

        // always from front
        result = input_buffer.seekoff(-7, ios_base::end, ios_base::in);
        assert(result == 3);

        // integer overflow due to large off
        result = input_buffer.seekoff(numeric_limits<long long>::max(), ios_base::end, ios_base::in);
        assert(result == -1);

        test_buf output_buffer{span<CharT>{buffer}, ios_base::out};
        // gptr not set but off is 0
        result = output_buffer.seekoff(0, ios_base::end, ios_base::in);
        assert(result == 0);

        // newoff is negative -> fail
        result = output_buffer.seekoff(-10, ios_base::end, ios_base::out);
        assert(result == -1);

        // pptr not set but off == 0
        result = output_buffer.seekoff(0, ios_base::end, ios_base::out);
        assert(result == 0);

        // all fine we stay at end of stream
        result = output_buffer.seekoff(0, ios_base::end, ios_base::in);
        assert(result == 0);

        // gptr not set and off != 0 -> fail
        result = output_buffer.seekoff(1, ios_base::end, ios_base::in);
        assert(result == -1);

        // off + buffer size is negative -> fail
        result = output_buffer.seekoff(-20, ios_base::end, ios_base::out);
        assert(result == -1);

        // off larger than buffer -> fail
        result = output_buffer.seekoff(11, ios_base::end, ios_base::out);
        assert(result == -1);

        // passes and moves to buffer size - off
        result = output_buffer.seekoff(5, ios_base::end, ios_base::out);
        assert(result == 5);

        // passes we are still below buffer size
        result = output_buffer.seekoff(3, ios_base::end, ios_base::out);
        assert(result == 8);

        // moves beyond buffer size -> fails
        result = output_buffer.seekoff(3, ios_base::end, ios_base::out);
        assert(result == -1);

        // integer overflow due to large off
        result = output_buffer.seekoff(numeric_limits<long long>::max(), ios_base::end, ios_base::in);
        assert(result == -1);

        test_buf inout_buffer{span<CharT>{buffer}, ios_base::in | ios_base::out};
        // all fine we move to end of stream
        result = inout_buffer.seekoff(0, ios_base::end, ios_base::in);
        assert(result == 10);

        // we move to front of the buffer
        result = inout_buffer.seekoff(-10, ios_base::end, ios_base::out);
        assert(result == 0);

        // we move to end of buffer
        result = inout_buffer.seekoff(0, ios_base::end, ios_base::out);
        assert(result == 10);

        // negative off -> fail
        result = inout_buffer.seekoff(-20, ios_base::end, ios_base::in);
        assert(result == -1);

        // off beyond end of buffer -> fail
        result = inout_buffer.seekoff(1, ios_base::end, ios_base::in);
        assert(result == -1);

        // passes and moves to buffer size - off
        result = inout_buffer.seekoff(-5, ios_base::end, ios_base::in);
        assert(result == 5);

        // always from front
        result = inout_buffer.seekoff(-7, ios_base::end, ios_base::in);
        assert(result == 3);

        // integer overflow due to large off
        result = inout_buffer.seekoff(numeric_limits<long long>::max(), ios_base::end, ios_base::in);
        assert(result == -1);
    }

    { // seekoff ios_base::cur
        CharT buffer[10];
        test_buf input_buffer{span<CharT>{buffer}, ios_base::in};

        // no mode set -> fail
        auto result = input_buffer.seekoff(0, ios_base::cur, 0);
        assert(result == -1);

        // both in and out modes set -> fail
        result = input_buffer.seekoff(0, ios_base::cur, ios_base::in | ios_base::out);
        assert(result == -1);

        // pptr not set and off is != 0 -> fail
        result = input_buffer.seekoff(1, ios_base::cur, ios_base::out);
        assert(result == -1);

        // off larger than buffer size -> fail
        result = input_buffer.seekoff(20, ios_base::cur, ios_base::out);
        assert(result == -1);

        // off negative -> fail
        result = input_buffer.seekoff(-1, ios_base::cur, ios_base::out);
        assert(result == -1);

        // pptr not set but off is == 0
        result = input_buffer.seekoff(0, ios_base::cur, ios_base::out);
        assert(result == 0);

        // passes and sets position
        result = input_buffer.seekoff(3, ios_base::cur, ios_base::in);
        assert(result == 3);

        // negative off moves back
        result = input_buffer.seekoff(-2, ios_base::cur, ios_base::in);
        assert(result == 1);

        // off + current position is beyond buffer size -> fail
        result = input_buffer.seekoff(10, ios_base::cur, ios_base::in);
        assert(result == -1);

        test_buf output_buffer{span<CharT>{buffer}, ios_base::out};
        // no mode set -> fail
        result = output_buffer.seekoff(0, ios_base::cur, 0);
        assert(result == -1);

        // both in and out modes set -> fail
        result = output_buffer.seekoff(0, ios_base::cur, ios_base::in | ios_base::out);
        assert(result == -1);

        // gptr not set and off is != 0 -> fail
        result = output_buffer.seekoff(1, ios_base::cur, ios_base::in);
        assert(result == -1);

        // off larger than buffer size -> fail
        result = output_buffer.seekoff(20, ios_base::cur, ios_base::out);
        assert(result == -1);

        // off negative -> fail
        result = output_buffer.seekoff(-1, ios_base::cur, ios_base::out);
        assert(result == -1);

        // gptr not set but off is == 0
        result = output_buffer.seekoff(0, ios_base::cur, ios_base::in);
        assert(result == 0);

        // passes and sets position
        result = output_buffer.seekoff(3, ios_base::cur, ios_base::out);
        assert(result == 3);

        // negative off moves back
        result = output_buffer.seekoff(-2, ios_base::cur, ios_base::out);
        assert(result == 1);

        // off + current position is beyond buffer size -> fail
        result = output_buffer.seekoff(10, ios_base::cur, ios_base::out);
        assert(result == -1);

        test_buf inout_buffer{span<CharT>{buffer}, ios_base::in | ios_base::out};
        // no mode set -> fail
        result = inout_buffer.seekoff(0, ios_base::cur, 0);
        assert(result == -1);

        // both in and out modes set -> fail
        result = inout_buffer.seekoff(0, ios_base::cur, ios_base::in | ios_base::out);
        assert(result == -1);

        // off larger than buffer size -> fail
        result = inout_buffer.seekoff(20, ios_base::cur, ios_base::out);
        assert(result == -1);

        // off negative -> fail
        result = inout_buffer.seekoff(-1, ios_base::cur, ios_base::out);
        assert(result == -1);

        // Moves input sequence to position 3
        result = inout_buffer.seekoff(3, ios_base::cur, ios_base::in);
        assert(result == 3);

        // Moves output sequence to position 3
        result = inout_buffer.seekoff(3, ios_base::cur, ios_base::out);
        assert(result == 3);

        // negative off moves back
        result = inout_buffer.seekoff(-2, ios_base::cur, ios_base::in);
        assert(result == 1);

        // negative off moves back
        result = inout_buffer.seekoff(-2, ios_base::cur, ios_base::out);
        assert(result == 1);

        // off + current position is beyond buffer size -> fail
        result = inout_buffer.seekoff(10, ios_base::cur, ios_base::in);
        assert(result == -1);

        // off + current position is beyond buffer size -> fail
        result = inout_buffer.seekoff(10, ios_base::cur, ios_base::out);
        assert(result == -1);

        // off + current position is before buffer size -> fail
        result = inout_buffer.seekoff(-2, ios_base::cur, ios_base::in);
        assert(result == -1);

        // off + current position is before buffer size -> fail
        result = inout_buffer.seekoff(-2, ios_base::cur, ios_base::out);
        assert(result == -1);
    }

    { // seekpos (same as seekoff with ios_base::beg)
        CharT buffer[10];
        test_buf input_buffer{span<CharT>{buffer}, ios_base::in};
        test_buf output_buffer{span<CharT>{buffer}, ios_base::out};

        auto result = input_buffer.seekpos(0, ios_base::in);
        assert(result == 0);

        // pptr not set but off is 0
        result = input_buffer.seekpos(0, ios_base::out);
        assert(result == 0);

        // pptr not set and off != 0 -> fail
        result = input_buffer.seekpos(1, ios_base::out);
        assert(result == -1);

        // gptr not set but off is 0
        result = output_buffer.seekpos(0, ios_base::in);
        assert(result == 0);

        // gptr not set and off != 0 -> fail
        result = output_buffer.seekpos(1, ios_base::in);
        assert(result == -1);

        // negative off -> fail
        result = input_buffer.seekpos(-1, ios_base::in);
        assert(result == -1);

        // negative off -> fail
        result = output_buffer.seekpos(-1, ios_base::out);
        assert(result == -1);

        // off larger than buf -> fail
        result = input_buffer.seekpos(20, ios_base::in);
        assert(result == -1);

        // off larger than buf -> fail
        result = output_buffer.seekpos(20, ios_base::out);
        assert(result == -1);

        // passes
        result = input_buffer.seekpos(5, ios_base::in);
        assert(result == 5);

        result = output_buffer.seekpos(5, ios_base::out);
        assert(result == 5);

        // always from front
        result = input_buffer.seekpos(7, ios_base::in);
        assert(result == 7);

        result = output_buffer.seekpos(7, ios_base::out);
        assert(result == 7);
    }

    { // setbuf
        CharT buffer1[10];
        CharT buffer2[30];
        test_buf input_buffer{span<CharT>{buffer1}, ios_base::in};
        assert(input_buffer.span().data() == buffer1);
        assert(input_buffer.span().size() == size(buffer1));

        auto result = input_buffer.setbuf(buffer2, 5);
        assert(input_buffer.span().data() == buffer2);
        assert(input_buffer.span().size() == 5);
        assert(result == addressof(input_buffer));

        test_buf output_buffer{span<CharT>{buffer1}, ios_base::out};
        assert(output_buffer.span().data() == buffer1);
        assert(output_buffer.span().size() == 0);

        result = output_buffer.setbuf(buffer2, 5);
        assert(output_buffer.span().data() == buffer2);
        assert(output_buffer.span().size() == 0);
        assert(result == addressof(output_buffer));

        test_buf hungry_buffer{span<CharT>{buffer1}, ios_base::out | ios_base::ate};
        assert(hungry_buffer.span().data() == buffer1);
        assert(hungry_buffer.span().size() == size(buffer1));

        result = hungry_buffer.setbuf(buffer2, 5);
        assert(hungry_buffer.span().data() == buffer2);
        assert(hungry_buffer.span().size() == 5);
        assert(result == addressof(hungry_buffer));
    }
}

template <class CharT>
void test_ispanstream() {
    using test_buf = basic_test_buf<basic_spanbuf<CharT>>;
    { // construction
        CharT buffer[10];
        basic_ispanstream<CharT> span_constructed{span<CharT>{buffer}};
        assert(span_constructed.span().data() == buffer);
        assert(static_cast<test_buf*>(span_constructed.rdbuf())->eback() == buffer);
        assert(static_cast<test_buf*>(span_constructed.rdbuf())->gptr() == buffer);
        assert(static_cast<test_buf*>(span_constructed.rdbuf())->egptr() == end(buffer));
        assert(static_cast<test_buf*>(span_constructed.rdbuf())->pbase() == nullptr);
        assert(static_cast<test_buf*>(span_constructed.rdbuf())->pptr() == nullptr);
        assert(static_cast<test_buf*>(span_constructed.rdbuf())->epptr() == nullptr);

        basic_ispanstream<CharT> span_mode_constructed{span<CharT>{buffer}, ios_base::out};
        assert(span_mode_constructed.span().data() == buffer);
        assert(static_cast<test_buf*>(span_mode_constructed.rdbuf())->eback() == buffer);
        assert(static_cast<test_buf*>(span_mode_constructed.rdbuf())->gptr() == buffer);
        assert(static_cast<test_buf*>(span_mode_constructed.rdbuf())->egptr() == end(buffer));
        assert(static_cast<test_buf*>(span_mode_constructed.rdbuf())->pbase() == buffer);
        assert(static_cast<test_buf*>(span_mode_constructed.rdbuf())->pptr() == buffer);
        assert(static_cast<test_buf*>(span_mode_constructed.rdbuf())->epptr() == end(buffer));

#ifdef __cpp_lib_concepts
        auto input_range = input_view<CharT>;
        basic_ispanstream<CharT> range_constructed{input_view<CharT>};
        assert(range_constructed.span().data() == input_range.data());
        assert(static_cast<test_buf*>(range_constructed.rdbuf())->eback() == input_range.data());
        assert(static_cast<test_buf*>(range_constructed.rdbuf())->gptr() == input_range.data());
        assert(static_cast<test_buf*>(range_constructed.rdbuf())->egptr() == input_range.data() + input_range.size());
        assert(static_cast<test_buf*>(range_constructed.rdbuf())->pbase() == nullptr);
        assert(static_cast<test_buf*>(range_constructed.rdbuf())->pptr() == nullptr);
        assert(static_cast<test_buf*>(range_constructed.rdbuf())->epptr() == nullptr);
#endif // __cpp_lib_concepts
    }

    { // span
        CharT buffer[10];
        basic_ispanstream<CharT> is{span<CharT>{buffer}};
        assert(is.span().data() == buffer);
        assert(is.span().size() == size(buffer));

        // ensure the underlying span is *not* mutable
        static_assert(is_same_v<decltype(is.span()), span<const CharT>>);
        static_assert(is_same_v<decltype(as_const(is).span()), span<const CharT>>);

        CharT other_buffer[20];
        is.span(span<CharT>{other_buffer});
        assert(is.span().data() == other_buffer);
        assert(is.span().size() == size(other_buffer));

#ifdef __cpp_lib_concepts
        auto input_range = input_view<CharT>;
        is.span(input_view<CharT>);
        assert(is.span().data() == input_range.data());
        assert(is.span().size() == input_range.size());

        if constexpr (is_same_v<CharT, char>) {
            const char const_buffer[] = "1 2 3 4 5";
            basic_ispanstream<CharT> is_const_buffer{span<const CharT>{const_buffer}};
            int read = 0;
            for (int expected = 1; expected <= 5; ++expected) {
                assert(is_const_buffer.good());
                is_const_buffer >> read;
                assert(read == expected);
            }
        }
#endif // __cpp_lib_concepts
    }

    { // swap
        CharT buffer1[10];
        CharT buffer2[20];
        basic_ispanstream<CharT> first{span<CharT>{buffer1}};
        basic_ispanstream<CharT> second{span<CharT>{buffer2}, ios_base::out};
        assert(first.span().data() == buffer1);
        assert(second.span().data() == buffer2);
        assert(static_cast<test_buf*>(first.rdbuf())->eback() == buffer1);
        assert(static_cast<test_buf*>(first.rdbuf())->gptr() == buffer1);
        assert(static_cast<test_buf*>(first.rdbuf())->egptr() == end(buffer1));
        assert(static_cast<test_buf*>(first.rdbuf())->pbase() == nullptr);
        assert(static_cast<test_buf*>(first.rdbuf())->pptr() == nullptr);
        assert(static_cast<test_buf*>(first.rdbuf())->epptr() == nullptr);
        assert(static_cast<test_buf*>(second.rdbuf())->eback() == buffer2);
        assert(static_cast<test_buf*>(second.rdbuf())->gptr() == buffer2);
        assert(static_cast<test_buf*>(second.rdbuf())->egptr() == end(buffer2));
        assert(static_cast<test_buf*>(second.rdbuf())->pbase() == buffer2);
        assert(static_cast<test_buf*>(second.rdbuf())->pptr() == buffer2);
        assert(static_cast<test_buf*>(second.rdbuf())->epptr() == end(buffer2));

        first.swap(second);
        assert(first.span().data() == buffer2);
        assert(second.span().data() == buffer1);
        assert(static_cast<test_buf*>(second.rdbuf())->eback() == buffer1);
        assert(static_cast<test_buf*>(second.rdbuf())->gptr() == buffer1);
        assert(static_cast<test_buf*>(second.rdbuf())->egptr() == end(buffer1));
        assert(static_cast<test_buf*>(second.rdbuf())->pbase() == nullptr);
        assert(static_cast<test_buf*>(second.rdbuf())->pptr() == nullptr);
        assert(static_cast<test_buf*>(second.rdbuf())->epptr() == nullptr);
        assert(static_cast<test_buf*>(first.rdbuf())->eback() == buffer2);
        assert(static_cast<test_buf*>(first.rdbuf())->gptr() == buffer2);
        assert(static_cast<test_buf*>(first.rdbuf())->egptr() == end(buffer2));
        assert(static_cast<test_buf*>(first.rdbuf())->pbase() == buffer2);
        assert(static_cast<test_buf*>(first.rdbuf())->pptr() == buffer2);
        assert(static_cast<test_buf*>(first.rdbuf())->epptr() == end(buffer2));

        swap(first, second);
        assert(first.span().data() == buffer1);
        assert(second.span().data() == buffer2);
        assert(static_cast<test_buf*>(first.rdbuf())->eback() == buffer1);
        assert(static_cast<test_buf*>(first.rdbuf())->gptr() == buffer1);
        assert(static_cast<test_buf*>(first.rdbuf())->egptr() == end(buffer1));
        assert(static_cast<test_buf*>(first.rdbuf())->pbase() == nullptr);
        assert(static_cast<test_buf*>(first.rdbuf())->pptr() == nullptr);
        assert(static_cast<test_buf*>(first.rdbuf())->epptr() == nullptr);
        assert(static_cast<test_buf*>(second.rdbuf())->eback() == buffer2);
        assert(static_cast<test_buf*>(second.rdbuf())->gptr() == buffer2);
        assert(static_cast<test_buf*>(second.rdbuf())->egptr() == end(buffer2));
        assert(static_cast<test_buf*>(second.rdbuf())->pbase() == buffer2);
        assert(static_cast<test_buf*>(second.rdbuf())->pptr() == buffer2);
        assert(static_cast<test_buf*>(second.rdbuf())->epptr() == end(buffer2));
    }

    // rdbuf already tested above

    { // read from stream
        basic_ispanstream<CharT> is{span<const CharT>{input_ptr<CharT>, 9}};
        int read = 0;
        for (int expected = 1; expected <= 5; ++expected) {
            assert(is.good());
            is >> read;
            assert(read == expected);
        }
        assert(!is.good());
        assert(!is.fail());
        assert(!is.bad());
        is >> read;

        assert(!is.good());
        assert(is.fail());
        assert(!is.bad());
    }
}

template <class CharT>
void test_ospanstream() {
    using test_buf = basic_test_buf<basic_spanbuf<CharT>>;
    { // construction
        CharT buffer[10];
        basic_ospanstream<CharT> span_constructed{span<CharT>{buffer}};
        assert(span_constructed.span().data() == buffer);
        assert(static_cast<test_buf*>(span_constructed.rdbuf())->eback() == nullptr);
        assert(static_cast<test_buf*>(span_constructed.rdbuf())->gptr() == nullptr);
        assert(static_cast<test_buf*>(span_constructed.rdbuf())->egptr() == nullptr);
        assert(static_cast<test_buf*>(span_constructed.rdbuf())->pbase() == buffer);
        assert(static_cast<test_buf*>(span_constructed.rdbuf())->pptr() == buffer);
        assert(static_cast<test_buf*>(span_constructed.rdbuf())->epptr() == end(buffer));

        basic_ospanstream<CharT> span_mode_constructed{span<CharT>{buffer}, ios_base::in};
        assert(span_mode_constructed.span().data() == buffer);
        assert(static_cast<test_buf*>(span_mode_constructed.rdbuf())->eback() == buffer);
        assert(static_cast<test_buf*>(span_mode_constructed.rdbuf())->gptr() == buffer);
        assert(static_cast<test_buf*>(span_mode_constructed.rdbuf())->egptr() == end(buffer));
        assert(static_cast<test_buf*>(span_mode_constructed.rdbuf())->pbase() == buffer);
        assert(static_cast<test_buf*>(span_mode_constructed.rdbuf())->pptr() == buffer);
        assert(static_cast<test_buf*>(span_mode_constructed.rdbuf())->epptr() == end(buffer));
    }

    { // span
        CharT buffer[10];
        basic_ospanstream<CharT> os{span<CharT>{buffer}};
        assert(os.span().data() == buffer);
        assert(os.span().size() == 0);

        // ensure the underlying span is mutable
        static_assert(is_same_v<decltype(os.span()), span<CharT>>);
        static_assert(is_same_v<decltype(as_const(os).span()), span<CharT>>);

        CharT other_buffer[20];
        os.span(span<CharT>{other_buffer});
        assert(os.span().data() == other_buffer);
        assert(os.span().size() == 0);
    }

    { // swap
        CharT buffer1[10];
        CharT buffer2[20];
        basic_ospanstream<CharT> first{span<CharT>{buffer1}};
        basic_ospanstream<CharT> second{span<CharT>{buffer2}, ios_base::in};
        assert(first.span().data() == buffer1);
        assert(second.span().data() == buffer2);
        assert(static_cast<test_buf*>(first.rdbuf())->eback() == nullptr);
        assert(static_cast<test_buf*>(first.rdbuf())->gptr() == nullptr);
        assert(static_cast<test_buf*>(first.rdbuf())->egptr() == nullptr);
        assert(static_cast<test_buf*>(first.rdbuf())->pbase() == buffer1);
        assert(static_cast<test_buf*>(first.rdbuf())->pptr() == buffer1);
        assert(static_cast<test_buf*>(first.rdbuf())->epptr() == end(buffer1));
        assert(static_cast<test_buf*>(second.rdbuf())->eback() == buffer2);
        assert(static_cast<test_buf*>(second.rdbuf())->gptr() == buffer2);
        assert(static_cast<test_buf*>(second.rdbuf())->egptr() == end(buffer2));
        assert(static_cast<test_buf*>(second.rdbuf())->pbase() == buffer2);
        assert(static_cast<test_buf*>(second.rdbuf())->pptr() == buffer2);
        assert(static_cast<test_buf*>(second.rdbuf())->epptr() == end(buffer2));

        first.swap(second);
        assert(first.span().data() == buffer2);
        assert(second.span().data() == buffer1);
        assert(static_cast<test_buf*>(second.rdbuf())->eback() == nullptr);
        assert(static_cast<test_buf*>(second.rdbuf())->gptr() == nullptr);
        assert(static_cast<test_buf*>(second.rdbuf())->egptr() == nullptr);
        assert(static_cast<test_buf*>(second.rdbuf())->pbase() == buffer1);
        assert(static_cast<test_buf*>(second.rdbuf())->pptr() == buffer1);
        assert(static_cast<test_buf*>(second.rdbuf())->epptr() == end(buffer1));
        assert(static_cast<test_buf*>(first.rdbuf())->eback() == buffer2);
        assert(static_cast<test_buf*>(first.rdbuf())->gptr() == buffer2);
        assert(static_cast<test_buf*>(first.rdbuf())->egptr() == end(buffer2));
        assert(static_cast<test_buf*>(first.rdbuf())->pbase() == buffer2);
        assert(static_cast<test_buf*>(first.rdbuf())->pptr() == buffer2);
        assert(static_cast<test_buf*>(first.rdbuf())->epptr() == end(buffer2));

        swap(first, second);
        assert(first.span().data() == buffer1);
        assert(second.span().data() == buffer2);
        assert(static_cast<test_buf*>(first.rdbuf())->eback() == nullptr);
        assert(static_cast<test_buf*>(first.rdbuf())->gptr() == nullptr);
        assert(static_cast<test_buf*>(first.rdbuf())->egptr() == nullptr);
        assert(static_cast<test_buf*>(first.rdbuf())->pbase() == buffer1);
        assert(static_cast<test_buf*>(first.rdbuf())->pptr() == buffer1);
        assert(static_cast<test_buf*>(first.rdbuf())->epptr() == end(buffer1));
        assert(static_cast<test_buf*>(second.rdbuf())->eback() == buffer2);
        assert(static_cast<test_buf*>(second.rdbuf())->gptr() == buffer2);
        assert(static_cast<test_buf*>(second.rdbuf())->egptr() == end(buffer2));
        assert(static_cast<test_buf*>(second.rdbuf())->pbase() == buffer2);
        assert(static_cast<test_buf*>(second.rdbuf())->pptr() == buffer2);
        assert(static_cast<test_buf*>(second.rdbuf())->epptr() == end(buffer2));
    }

    // rdbuf already tested above

    { // write to stream with sufficient space
        CharT output_buffer[30];
        basic_ospanstream<CharT> os{span<CharT>{output_buffer}};

        assert(os.good());
        assert(!os.fail());
        assert(!os.bad());
        os << 10 << 20 << 30;
        assert(os.good());
        assert(!os.fail());
        assert(!os.bad());

        const auto expected = "102030"sv;
        assert(os.span().size() == 6);
        assert(equal(begin(os.span()), end(os.span()), begin(expected), end(expected)));
        assert(os.span().data() == output_buffer);
    }

    { // write to stream with overflow
        CharT output_buffer[30];
        basic_ospanstream<CharT> os{span<CharT>{output_buffer}};

        os << 10 << 20 << 30;
        assert(os.good());
        assert(!os.fail());
        assert(!os.bad());
        if constexpr (is_same_v<CharT, char>) {
            os << "hello world and a long string with more than 30 chars";
        } else {
            os << L"hello world and a long string with more than 30 chars";
        }
        assert(!os.good());
        assert(os.fail());
        assert(os.bad());

        const auto expected = "102030hello world and a long s"sv;
        assert(os.span().size() == size(output_buffer));
        assert(equal(begin(output_buffer), end(output_buffer), begin(expected), end(expected)));
    }
}

template <class CharT>
void test_spanstream() {
    using test_buf = basic_test_buf<basic_spanbuf<CharT>>;
    { // construction
        CharT buffer[10];
        basic_spanstream<CharT> span_constructed{span<CharT>{buffer}};
        assert(span_constructed.span().data() == buffer);
        assert(static_cast<test_buf*>(span_constructed.rdbuf())->eback() == buffer);
        assert(static_cast<test_buf*>(span_constructed.rdbuf())->gptr() == buffer);
        assert(static_cast<test_buf*>(span_constructed.rdbuf())->egptr() == end(buffer));
        assert(static_cast<test_buf*>(span_constructed.rdbuf())->pbase() == buffer);
        assert(static_cast<test_buf*>(span_constructed.rdbuf())->pptr() == buffer);
        assert(static_cast<test_buf*>(span_constructed.rdbuf())->epptr() == end(buffer));

        basic_spanstream<CharT> span_mode_constructed{span<CharT>{buffer}, ios_base::in};
        assert(span_mode_constructed.span().data() == buffer);
        assert(static_cast<test_buf*>(span_mode_constructed.rdbuf())->eback() == buffer);
        assert(static_cast<test_buf*>(span_mode_constructed.rdbuf())->gptr() == buffer);
        assert(static_cast<test_buf*>(span_mode_constructed.rdbuf())->egptr() == end(buffer));
        assert(static_cast<test_buf*>(span_mode_constructed.rdbuf())->pbase() == nullptr);
        assert(static_cast<test_buf*>(span_mode_constructed.rdbuf())->pptr() == nullptr);
        assert(static_cast<test_buf*>(span_mode_constructed.rdbuf())->epptr() == nullptr);
    }

    { // span
        CharT buffer[10];
        basic_spanstream<CharT> s{span<CharT>{buffer}};
        assert(s.span().data() == buffer);
        assert(s.span().size() == 0);

        // ensure the underlying span is mutable
        static_assert(is_same_v<decltype(s.span()), span<CharT>>);
        static_assert(is_same_v<decltype(as_const(s).span()), span<CharT>>);

        CharT other_buffer[20];
        s.span(span<CharT>{other_buffer});
        assert(s.span().data() == other_buffer);
        assert(s.span().size() == 0);
    }

    { // swap
        CharT buffer1[10];
        CharT buffer2[20];
        basic_spanstream<CharT> first{span<CharT>{buffer1}, ios_base::out};
        basic_spanstream<CharT> second{span<CharT>{buffer2}, ios_base::in};
        assert(first.span().data() == buffer1);
        assert(second.span().data() == buffer2);
        assert(static_cast<test_buf*>(first.rdbuf())->eback() == nullptr);
        assert(static_cast<test_buf*>(first.rdbuf())->gptr() == nullptr);
        assert(static_cast<test_buf*>(first.rdbuf())->egptr() == nullptr);
        assert(static_cast<test_buf*>(first.rdbuf())->pbase() == buffer1);
        assert(static_cast<test_buf*>(first.rdbuf())->pptr() == buffer1);
        assert(static_cast<test_buf*>(first.rdbuf())->epptr() == end(buffer1));
        assert(static_cast<test_buf*>(second.rdbuf())->eback() == buffer2);
        assert(static_cast<test_buf*>(second.rdbuf())->gptr() == buffer2);
        assert(static_cast<test_buf*>(second.rdbuf())->egptr() == end(buffer2));
        assert(static_cast<test_buf*>(second.rdbuf())->pbase() == nullptr);
        assert(static_cast<test_buf*>(second.rdbuf())->pptr() == nullptr);
        assert(static_cast<test_buf*>(second.rdbuf())->epptr() == nullptr);

        first.swap(second);
        assert(first.span().data() == buffer2);
        assert(second.span().data() == buffer1);
        assert(static_cast<test_buf*>(second.rdbuf())->eback() == nullptr);
        assert(static_cast<test_buf*>(second.rdbuf())->gptr() == nullptr);
        assert(static_cast<test_buf*>(second.rdbuf())->egptr() == nullptr);
        assert(static_cast<test_buf*>(second.rdbuf())->pbase() == buffer1);
        assert(static_cast<test_buf*>(second.rdbuf())->pptr() == buffer1);
        assert(static_cast<test_buf*>(second.rdbuf())->epptr() == end(buffer1));
        assert(static_cast<test_buf*>(first.rdbuf())->eback() == buffer2);
        assert(static_cast<test_buf*>(first.rdbuf())->gptr() == buffer2);
        assert(static_cast<test_buf*>(first.rdbuf())->egptr() == end(buffer2));
        assert(static_cast<test_buf*>(first.rdbuf())->pbase() == nullptr);
        assert(static_cast<test_buf*>(first.rdbuf())->pptr() == nullptr);
        assert(static_cast<test_buf*>(first.rdbuf())->epptr() == nullptr);

        swap(first, second);
        assert(first.span().data() == buffer1);
        assert(second.span().data() == buffer2);
        assert(static_cast<test_buf*>(first.rdbuf())->eback() == nullptr);
        assert(static_cast<test_buf*>(first.rdbuf())->gptr() == nullptr);
        assert(static_cast<test_buf*>(first.rdbuf())->egptr() == nullptr);
        assert(static_cast<test_buf*>(first.rdbuf())->pbase() == buffer1);
        assert(static_cast<test_buf*>(first.rdbuf())->pptr() == buffer1);
        assert(static_cast<test_buf*>(first.rdbuf())->epptr() == end(buffer1));
        assert(static_cast<test_buf*>(second.rdbuf())->eback() == buffer2);
        assert(static_cast<test_buf*>(second.rdbuf())->gptr() == buffer2);
        assert(static_cast<test_buf*>(second.rdbuf())->egptr() == end(buffer2));
        assert(static_cast<test_buf*>(second.rdbuf())->pbase() == nullptr);
        assert(static_cast<test_buf*>(second.rdbuf())->pptr() == nullptr);
        assert(static_cast<test_buf*>(second.rdbuf())->epptr() == nullptr);
    }

    // rdbuf already tested above

    { // read from stream
        auto arr = input_std_array<CharT>;
        basic_spanstream<CharT> s{span<CharT>{arr.data(), 9}};
        int read = 0;
        for (int expected = 1; expected <= 5; ++expected) {
            assert(s.good());
            s >> read;
            assert(read == expected);
        }
        assert(!s.good());
        assert(!s.fail());
        assert(!s.bad());
        s >> read;

        assert(!s.good());
        assert(s.fail());
        assert(!s.bad());
    }

    { // write to stream with sufficient space
        CharT output_buffer[30];
        basic_spanstream<CharT> s{span<CharT>{output_buffer}};

        assert(s.good());
        assert(!s.fail());
        assert(!s.bad());
        s << 10 << 20 << 30;
        assert(s.good());
        assert(!s.fail());
        assert(!s.bad());

        const auto expected = "102030"sv;
        assert(s.span().size() == 6);
        assert(equal(begin(s.span()), end(s.span()), begin(expected), end(expected)));
        assert(s.span().data() == output_buffer);
    }

    { // write to stream with overflow
        CharT output_buffer[30];
        basic_spanstream<CharT> s{span<CharT>{output_buffer}};

        s << 10 << 20 << 30;
        assert(s.good());
        assert(!s.fail());
        assert(!s.bad());
        if constexpr (is_same_v<CharT, char>) {
            s << "hello world and a long string with more than 30 chars";
        } else {
            s << L"hello world and a long string with more than 30 chars";
        }
        assert(!s.good());
        assert(s.fail());
        assert(s.bad());

        const auto expected = "102030hello world and a long s"sv;
        assert(s.span().size() == size(output_buffer));
        assert(equal(begin(output_buffer), end(output_buffer), begin(expected), end(expected)));
    }
}

template <class CharT>
void run_test() {
    test_spanbuf<CharT>();
    test_ispanstream<CharT>();
    test_ospanstream<CharT>();
    test_spanstream<CharT>();
}

int main() {
    run_test<char>();
    run_test<wchar_t>();
}
