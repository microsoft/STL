// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std;

template <typename T>
struct soccc_allocator {
    using value_type = T;

    explicit soccc_allocator(const int id_) noexcept : id(id_), soccc_generation(0) {}
    explicit soccc_allocator(const int id_, const int soccc_generation_) noexcept
        : id(id_), soccc_generation(soccc_generation_) {}
    template <typename U>
    soccc_allocator(const soccc_allocator<U>& other) noexcept
        : id(other.id), soccc_generation(other.soccc_generation) {}
    soccc_allocator(const soccc_allocator&) = default;

    soccc_allocator& operator=(const soccc_allocator&) noexcept {
        assert(false && "no tested operations should assign allocators");
        return *this;
    }

    soccc_allocator select_on_container_copy_construction() const noexcept {
        return soccc_allocator(id, soccc_generation + 1);
    }

    template <typename U>
    bool operator==(const soccc_allocator<U>& other) const noexcept {
        return id == other.id;
    }

    template <typename U>
    bool operator!=(const soccc_allocator<U>& other) const noexcept {
        return id != other.id;
    }

    T* allocate(const size_t n) {
        return allocator<T>{}.allocate(n);
    }

    void deallocate(T* const p, const size_t n) noexcept {
        allocator<T>{}.deallocate(p, n);
    }

    int id;
    int soccc_generation;
};

using soccc_string = basic_string<char, char_traits<char>, soccc_allocator<char>>;

void test_lvalue_lvalue() {
    soccc_string lhs(soccc_allocator<char>(11));
    soccc_string rhs(soccc_allocator<char>(22));
    auto result = lhs + rhs; // SOCCC on left
    assert(result[result.size()] == 0);
    assert(result.get_allocator().id == 11);
    assert(result.get_allocator().soccc_generation == 1);
}

void test_lvalue_rvalue() {
    soccc_string lhs(soccc_allocator<char>(11));
    soccc_string rhs(soccc_allocator<char>(22));
    auto result = lhs + move(rhs); // take rvalue's allocator
    assert(result[result.size()] == 0);
    assert(result.get_allocator().id == 22);
    assert(result.get_allocator().soccc_generation == 0);
}

void test_rvalue_lvalue() {
    soccc_string lhs(soccc_allocator<char>(11));
    soccc_string rhs(soccc_allocator<char>(22));
    auto result = move(lhs) + rhs; // take rvalue's allocator
    assert(result[result.size()] == 0);
    assert(result.get_allocator().id == 11);
    assert(result.get_allocator().soccc_generation == 0);
}

void test_rvalue_rvalue() {
    soccc_string lhs(soccc_allocator<char>(11));
    soccc_string rhs(soccc_allocator<char>(22));
    const auto initialRhsPtr = rhs.data();
    auto result              = move(lhs) + move(rhs); // take left's allocator
    assert(result.data() != initialRhsPtr);
    assert(result[result.size()] == 0);
    assert(result.get_allocator().id == 11);
    assert(result.get_allocator().soccc_generation == 0);
}

void test_rvalue_rvalue_big_left() {
    soccc_string lhs(100, 'a', soccc_allocator<char>(11));
    soccc_string rhs(soccc_allocator<char>(22));
    const auto initialRhsPtr = rhs.data();
    auto result              = move(lhs) + move(rhs); // take left's allocator
    assert(result.data() != initialRhsPtr);
    assert(result[result.size()] == 0);
    assert(result.get_allocator().id == 11);
    assert(result.get_allocator().soccc_generation == 0);
}

void test_rvalue_rvalue_big_left_equal_allocator_equal_capacity() {
    // not guaranteed by the standard, but a performance guarantee we want.
    // set up lhs and rhs to have equal capacities such that concat exactly fills that capacity
    soccc_string lhs(100, 'a', soccc_allocator<char>(11));
    soccc_string rhs(100, 'a', soccc_allocator<char>(11));

    // all else being equal, we should prefer the left buffer
    lhs.resize(rhs.capacity() - rhs.size(), 'a');
    assert(lhs.capacity() == rhs.capacity());

    const auto initialLhsBuffer = lhs.data();
    auto result                 = move(lhs) + move(rhs);
    assert(result.data() == initialLhsBuffer);
    assert(result[result.size()] == 0);
    assert(result.get_allocator().id == 11);
    assert(result.get_allocator().soccc_generation == 0);
}

void test_rvalue_rvalue_one_more_than_capacity() {
    // ensure that a result exactly one larger than the capacities of either of the inputs causes reallocation
    soccc_string lhs(100, 'a', soccc_allocator<char>(11));
    soccc_string rhs(100, 'a', soccc_allocator<char>(11));

    lhs.resize(rhs.capacity() - rhs.size() + 1, 'a');
    assert(lhs.capacity() == rhs.capacity());

    const auto initialLhsBuffer = lhs.data();
    const auto initialRhsBuffer = rhs.data();
    auto result                 = move(lhs) + move(rhs); // take left's allocator
    assert(result.data() != initialLhsBuffer);
    assert(result.data() != initialRhsBuffer);
    assert(result[result.size()] == 0);
    assert(result.get_allocator().id == 11);
    assert(result.get_allocator().soccc_generation == 0);
}

void test_rvalue_rvalue_big_right() {
    soccc_string lhs(soccc_allocator<char>(11));
    soccc_string rhs(100, 'a', soccc_allocator<char>(22));
    const auto initialRhsPtr = rhs.data();
    auto result              = move(lhs) + move(rhs); // take left's allocator
    assert(result.data() != initialRhsPtr);
    assert(result[result.size()] == 0);
    assert(result.get_allocator().id == 11);
    assert(result.get_allocator().soccc_generation == 0);
}

void test_rvalue_rvalue_big_right_equal_allocator() {
    // not technically guaranteed by the standard, but an optimization we want to preserve
    soccc_string lhs(1, 'b', soccc_allocator<char>(11));
    soccc_string rhs(100, 'a', soccc_allocator<char>(11));
    const auto initialRhsPtr = rhs.data();
    auto result              = move(lhs) + move(rhs); // take left's allocator
    assert(result.data() == initialRhsPtr); // took rhs's buffer
    assert(result[result.size()] == 0);
    assert(result.get_allocator().id == 11);
    assert(result.get_allocator().soccc_generation == 0);
}

void test_rvalue_rvalue_big_both() {
    soccc_string lhs(100, 'a', soccc_allocator<char>(11));
    lhs.resize(lhs.capacity(), 'a');
    soccc_string rhs(100, 'a', soccc_allocator<char>(22));
    rhs.resize(rhs.capacity(), 'a');
    const auto initialRhsPtr = rhs.data();
    auto result              = move(lhs) + move(rhs); // take left's allocator
    assert(result.data() != initialRhsPtr);
    assert(result[result.size()] == 0);
    assert(result.get_allocator().id == 11);
    assert(result.get_allocator().soccc_generation == 0);
}

void test_lvalue_ntbs() {
    soccc_string lhs(soccc_allocator<char>(11));
    auto result = lhs + "testString"; // SOCCC on lvalue std::string
    assert(result[result.size()] == 0);
    assert(result.get_allocator().id == 11);
    assert(result.get_allocator().soccc_generation == 1);
}

void test_lvalue_char() {
    soccc_string lhs(soccc_allocator<char>(11));
    auto result = lhs + 'c'; // SOCCC on lvalue
    assert(result[result.size()] == 0);
    assert(result.get_allocator().id == 11);
    assert(result.get_allocator().soccc_generation == 1);
}

void test_rvalue_ntbs() {
    soccc_string lhs(soccc_allocator<char>(11));
    auto result = move(lhs) + "testString"; // take rvalue's allocator
    assert(result[result.size()] == 0);
    assert(result.get_allocator().id == 11);
    assert(result.get_allocator().soccc_generation == 0);
}

void test_rvalue_char() {
    soccc_string lhs(soccc_allocator<char>(11));
    auto result = move(lhs) + 'c'; // take rvalue
    assert(result[result.size()] == 0);
    assert(result.get_allocator().id == 11);
    assert(result.get_allocator().soccc_generation == 0);
}

void test_ntbs_lvalue() {
    soccc_string rhs(soccc_allocator<char>(11));
    auto result = "testString" + rhs; // SOCCC on lvalue std::string
    assert(result[result.size()] == 0);
    assert(result.get_allocator().id == 11);
    assert(result.get_allocator().soccc_generation == 1);
}

void test_char_lvalue() {
    soccc_string rhs(soccc_allocator<char>(11));
    auto result = 'c' + rhs; // SOCCC on lvalue
    assert(result[result.size()] == 0);
    assert(result.get_allocator().id == 11);
    assert(result.get_allocator().soccc_generation == 1);
}

void test_ntbs_rvalue() {
    soccc_string rhs(soccc_allocator<char>(11));
    auto result = "testString" + move(rhs); // take rvalue's allocator
    assert(result[result.size()] == 0);
    assert(result.get_allocator().id == 11);
    assert(result.get_allocator().soccc_generation == 0);
}

void test_char_rvalue() {
    soccc_string rhs(soccc_allocator<char>(11));
    auto result = 'c' + move(rhs); // take rvalue's allocator
    assert(result[result.size()] == 0);
    assert(result.get_allocator().id == 11);
    assert(result.get_allocator().soccc_generation == 0);
}

// also test for length_error edge cases in lvalue concats when resolving GH-456
template <typename T, size_t MaxSize>
struct max_size_allocator {
    using value_type = T;

    template <typename U>
    struct rebind {
        using other = max_size_allocator<U, MaxSize>;
    };

    max_size_allocator() = default;
    template <typename U>
    max_size_allocator(const max_size_allocator<U, MaxSize>&) noexcept {}
    max_size_allocator(const max_size_allocator&) = default;

    max_size_allocator& operator=(const max_size_allocator&) noexcept {
        assert(false && "no tested operations should assign allocators");
        return *this;
    }

    template <typename U>
    bool operator==(const max_size_allocator<U, MaxSize>&) const noexcept {
        return true;
    }

    template <typename U>
    bool operator!=(const max_size_allocator<U, MaxSize>&) const noexcept {
        return false;
    }

    T* allocate(const size_t n) {
        return allocator<T>{}.allocate(n);
    }

    void deallocate(T* const p, const size_t n) noexcept {
        allocator<T>{}.deallocate(p, n);
    }

    size_t max_size() const noexcept {
        return MaxSize;
    }
};

template <typename A, typename B>
void assert_concat(A&& a, B&& b, const string& expected) noexcept {
    const auto& actual = forward<A>(a) + forward<B>(b);
    assert(equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
    assert(actual[actual.size()] == 0);
}

template <typename A, typename B>
void assert_throws_length_error(A&& a, B&& b) noexcept {
    try {
        (void) (forward<A>(a) + forward<B>(b));
        assert(false);
    } catch (const length_error&) {
    }
}

template <size_t N>
using limited_string = basic_string<char, char_traits<char>, max_size_allocator<char, N>>;

template <size_t maxSize>
void test_concat_perf_edge_cases() {
    static_assert(maxSize >= 3, "at least 3 characters needed in test");
    limited_string<maxSize> emptyStr;
    const size_t trueMax = emptyStr.max_size(); // max_size() might be greater than allocator (in which case the small
                                                // string optimization is always engaged)
    string expectedSuffix;
    expectedSuffix.reserve(trueMax);
    expectedSuffix.append(trueMax - 1, 'a');
    expectedSuffix.push_back('c');

    string expectedPrefix;
    expectedPrefix.reserve(trueMax);
    expectedPrefix.push_back('c');
    expectedPrefix.append(trueMax - 1, 'a');

    string expectedFilled(trueMax, 'a');

    limited_string<maxSize> zero(0, 'c');
    limited_string<maxSize> one(1, 'c');
    limited_string<maxSize> two(2, 'c');
    limited_string<maxSize> oneLeft(trueMax - 1, 'a');
    limited_string<maxSize> filled(trueMax, 'a');

    assert_concat(zero, zero, "");
    assert_concat(zero, filled, expectedFilled);
    assert_concat("", filled, expectedFilled);
    assert_concat(filled, zero, expectedFilled);
    assert_concat(filled, "", expectedFilled);

    assert_concat(oneLeft, one, expectedSuffix);
    assert_throws_length_error(filled, one);
    assert_concat(oneLeft, "c", expectedSuffix);
    assert_throws_length_error(filled, "c");
    assert_concat(oneLeft, 'c', expectedSuffix);
    assert_throws_length_error(filled, 'c');
    assert_concat(one, oneLeft, expectedPrefix);
    assert_throws_length_error(one, filled);
    assert_concat("c", oneLeft, expectedPrefix);
    assert_throws_length_error("c", filled);
    assert_concat('c', oneLeft, expectedPrefix);
    assert_throws_length_error('c', filled);

    assert_throws_length_error(oneLeft, two);
    assert_throws_length_error(oneLeft, "cc");
    assert_throws_length_error(two, oneLeft);
    assert_throws_length_error("cc", oneLeft);
}

int main() {
    test_lvalue_lvalue();
    test_lvalue_rvalue();
    test_rvalue_lvalue();
    test_rvalue_rvalue();
    test_rvalue_rvalue_big_left();
    test_rvalue_rvalue_big_left_equal_allocator_equal_capacity();
    test_rvalue_rvalue_one_more_than_capacity();
    test_rvalue_rvalue_big_right();
    test_rvalue_rvalue_big_right_equal_allocator();
    test_rvalue_rvalue_big_both();
    test_lvalue_ntbs();
    test_lvalue_char();
    test_rvalue_ntbs();
    test_rvalue_char();
    test_ntbs_lvalue();
    test_char_lvalue();
    test_ntbs_rvalue();
    test_char_rvalue();
    test_concat_perf_edge_cases<10>(); // test with small string optimization engaged
    test_concat_perf_edge_cases<40>(); // test with small string optimization disengaged
}
