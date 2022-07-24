// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <memory>
#include <new>
#include <string>
#include <unordered_set>
#include <utility>

using namespace std;

#ifdef _WIN64
const size_t bad_size = 0xCCCC'CCCC'CCCC'CCCCULL;
#else // ^^^ _WIN64 ^^^ // vvv !_WIN64 vvv
const size_t bad_size = 0xCCCC'CCCCUL;
#endif // _WIN64
const unsigned long long default_state = 0xB01DFACEDEBAC1EULL;

template <typename T>
struct stateful_allocator {
    unsigned long long state;

    using value_type = T;
    T* allocate(size_t n) {
        return allocator<T>().allocate(n);
    }

    void deallocate(T* p, size_t n) {
        allocator<T>().deallocate(p, n);
    }

    stateful_allocator() : state(default_state) {}
    explicit stateful_allocator(unsigned long long s) : state(s) {}
    stateful_allocator(const stateful_allocator&) = default;
    template <typename U>
    stateful_allocator(const stateful_allocator<U>& other) : state(other.state) {}

    stateful_allocator& operator=(const stateful_allocator&) = delete;
};

template <typename T, typename U>
inline bool operator==(const stateful_allocator<T>& lhs, const stateful_allocator<U>& rhs) {
    return lhs.state == rhs.state;
}

template <typename T, typename U>
inline bool operator!=(const stateful_allocator<T>& lhs, const stateful_allocator<U>& rhs) {
    return lhs.state != rhs.state;
}

// warning C4582: 'garbage_data<std::weak_ptr<_Ty>>::data': constructor is not implicitly called
// warning C4583: 'garbage_data<std::weak_ptr<_Ty>>::data': destructor is not implicitly called
#pragma warning(push)
#pragma warning(disable : 4582 4583)
template <typename T>
struct garbage_data {
    union {
        T data;
    };

    bool constructed;
    garbage_data() : constructed(false) {
        memset(&data, 0xCC, sizeof(data));
    }

    garbage_data(const garbage_data&)            = delete;
    garbage_data& operator=(const garbage_data&) = delete;

    T& get() {
        return data;
    }

    T* ptr() {
        return &data;
    }

    T* operator->() {
        return ptr();
    }

    ~garbage_data() {
        if (constructed) {
            ptr()->~T();
        }
    }

    template <typename... Args>
    void construct(Args&&... args) {
        assert(!constructed);
        ::new (static_cast<void*>(&data)) T(forward<Args>(args)...);
        constructed = true;
    }
};
#pragma warning(pop)

template <typename Alloc>
void assert_string_invariants(basic_string<char, char_traits<char>, Alloc>& target, const char* const expected) {
    const size_t expectedSize = strlen(expected);
    assert(target.capacity() != bad_size);
    assert(target.capacity() >= 15); // due to the small string optimization
    assert(target.size() == expectedSize);
    assert(target == expected);
    const size_t oldSize = target.size();
    target.push_back('a'); // in case the input is small
    assert(target.size() == oldSize + 1);
    assert(target[oldSize] == 'a');
    const char longer[]      = "some longer text to push over small string limit";
    const size_t longerChars = strlen(longer);
    target.append(longer);
    assert(target.size() == oldSize + 1 + longerChars);
    assert(target.capacity() >= target.size());
    assert(equal(target.end() - static_cast<ptrdiff_t>(longerChars), target.end(), longer,
        longer + static_cast<ptrdiff_t>(longerChars)));
}

using str_data_t       = garbage_data<string>;
using alloc_t          = stateful_allocator<char>;
using str_alloc_t      = basic_string<char, char_traits<char>, alloc_t>;
using str_data_alloc_t = garbage_data<str_alloc_t>;

void test_case_string_construction_no_data() {
    { // basic_string()
        str_data_t testData;
        testData.construct();
        assert_string_invariants(testData.get(), "");

        str_data_alloc_t testDataAlloc;
        testDataAlloc.construct();
        assert(testDataAlloc->get_allocator().state == default_state);
        assert_string_invariants(testDataAlloc.get(), "");
    }

    { // basic_string(const Allocator& a)
        str_data_alloc_t testDataAlloc;
        testDataAlloc.construct(alloc_t{1729});
        assert(testDataAlloc->get_allocator().state == 1729);
        assert_string_invariants(testDataAlloc.get(), "");
    }

    { // basic_string(initializer_list<charT>, const Allocator& = Allocator());
        str_data_t testData;
        testData.construct(initializer_list<char>{'a', 'b'});
        assert_string_invariants(testData.get(), "ab");

        str_data_alloc_t testDataAlloc;
        testDataAlloc.construct(initializer_list<char>{'a', 'b'});
        assert(testDataAlloc->get_allocator().state == default_state);
        assert_string_invariants(testDataAlloc.get(), "ab");

        str_data_alloc_t testDataAlloc2;
        testDataAlloc2.construct(initializer_list<char>{'a', 'b'}, alloc_t{42});
        assert(testDataAlloc2->get_allocator().state == 42);
        assert_string_invariants(testDataAlloc2.get(), "ab");
    }
}

void test_case_string_construction(const string& source) {
    const char* const sourceStr = source.c_str();
    const str_alloc_t sourceAlloc(source.begin(), source.end(), alloc_t{1729});
    const char oneChar[] = {source[1], '\0'};


    { // basic_string(const basic_string& str)
        str_data_t testData;
        testData.construct(source);
        assert_string_invariants(testData.get(), sourceStr);

        str_data_alloc_t testDataAlloc;
        testDataAlloc.construct(sourceAlloc);
        assert(testDataAlloc->get_allocator().state == 1729);
        assert_string_invariants(testDataAlloc.get(), sourceStr);
    }

    { // basic_string(const basic_string&, const Allocator&)
        str_data_alloc_t testDataAlloc;
        testDataAlloc.construct(sourceAlloc, alloc_t{42});
        assert(testDataAlloc->get_allocator().state == 42);
        assert_string_invariants(testDataAlloc.get(), sourceStr);
    }

    { // basic_string(basic_string&& str)
        string movedFrom{source};
        str_data_t testData;
        testData.construct(move(movedFrom));
        assert_string_invariants(testData.get(), sourceStr);

        str_alloc_t movedFromAlloc{sourceAlloc};
        str_data_alloc_t testDataAlloc;
        testDataAlloc.construct(move(movedFromAlloc));
        assert(testDataAlloc->get_allocator().state == 1729);
        assert_string_invariants(testDataAlloc.get(), sourceStr);
    }

    { // basic_string(basic_string&&, const Allocator&)
        str_alloc_t movedFromAlloc{sourceAlloc};
        str_data_alloc_t testDataAlloc;
        testDataAlloc.construct(move(movedFromAlloc), alloc_t{42});
        assert(testDataAlloc->get_allocator().state == 42);
        assert_string_invariants(testDataAlloc.get(), sourceStr);
    }

    { // basic_string(const basic_string& str, size_type pos, const Allocator& a = Allocator());
        str_data_t testData;
        testData.construct(source, 1U);
        assert_string_invariants(testData.get(), sourceStr + 1);

        str_data_alloc_t testDataAlloc;
        testDataAlloc.construct(sourceAlloc, 1U);
        assert(testDataAlloc->get_allocator().state == default_state); // note: allocator not transferred
        assert_string_invariants(testDataAlloc.get(), sourceStr + 1);

        str_data_alloc_t testDataAlloc2;
        testDataAlloc2.construct(sourceAlloc, 1U, alloc_t{42});
        assert(testDataAlloc2->get_allocator().state == 42);
        assert_string_invariants(testDataAlloc2.get(), sourceStr + 1);
    }

    { // basic_string(const basic_string& str, size_type pos, size_type n,
        //              const Allocator& a = Allocator());
        str_data_t testData;
        testData.construct(source, 1U, 1U);
        assert_string_invariants(testData.get(), oneChar);

        str_data_alloc_t testDataAlloc;
        testDataAlloc.construct(sourceAlloc, 1U, 1U);
        assert(testDataAlloc->get_allocator().state == default_state); // note: allocator not transferred
        assert_string_invariants(testDataAlloc.get(), oneChar);

        str_data_alloc_t testDataAlloc2;
        testDataAlloc2.construct(sourceAlloc, 1U, 1U, alloc_t{42});
        assert(testDataAlloc2->get_allocator().state == 42);
        assert_string_invariants(testDataAlloc2.get(), oneChar);
    }

    { // basic_string(const charT* s, const Allocator& a = Allocator());
        str_data_t testData;
        testData.construct(sourceStr);
        assert_string_invariants(testData.get(), sourceStr);

        str_data_alloc_t testDataAlloc;
        testDataAlloc.construct(sourceStr);
        assert(testDataAlloc->get_allocator().state == default_state);
        assert_string_invariants(testDataAlloc.get(), sourceStr);

        str_data_alloc_t testDataAlloc2;
        testDataAlloc2.construct(sourceStr, alloc_t{42});
        assert(testDataAlloc2->get_allocator().state == 42);
        assert_string_invariants(testDataAlloc2.get(), sourceStr);
    }

    { // basic_string(const charT* s, size_type n, const Allocator& a = Allocator());
        str_data_t testData;
        testData.construct(sourceStr + 1, 1U);
        assert_string_invariants(testData.get(), oneChar);

        str_data_alloc_t testDataAlloc;
        testDataAlloc.construct(sourceStr + 1, 1U);
        assert(testDataAlloc->get_allocator().state == default_state);
        assert_string_invariants(testDataAlloc.get(), oneChar);

        str_data_alloc_t testDataAlloc2;
        testDataAlloc2.construct(sourceStr + 1, 1U, alloc_t{42});
        assert(testDataAlloc2->get_allocator().state == 42);
        assert_string_invariants(testDataAlloc2.get(), oneChar);
    }

    { // basic_string(size_type n, charT c, const Allocator& a = Allocator());
        str_data_t testData;
        testData.construct(1U, source[1]);
        assert_string_invariants(testData.get(), oneChar);

        str_data_alloc_t testDataAlloc;
        testDataAlloc.construct(1U, source[1]);
        assert(testDataAlloc->get_allocator().state == default_state);
        assert_string_invariants(testDataAlloc.get(), oneChar);

        str_data_alloc_t testDataAlloc2;
        testDataAlloc2.construct(1U, source[1], alloc_t{42});
        assert(testDataAlloc2->get_allocator().state == 42);
        assert_string_invariants(testDataAlloc2.get(), oneChar);
    }

    { // basic_string(InputIterator begin, InputIterator end, const Allocator& a = Allocator());
        str_data_t testData;
        testData.construct(source.begin(), source.end());
        assert_string_invariants(testData.get(), sourceStr);

        str_data_alloc_t testDataAlloc;
        testDataAlloc.construct(source.begin(), source.end());
        assert(testDataAlloc->get_allocator().state == default_state);
        assert_string_invariants(testDataAlloc.get(), sourceStr);

        str_data_alloc_t testDataAlloc2;
        testDataAlloc2.construct(source.begin(), source.end(), alloc_t{42});
        assert(testDataAlloc2->get_allocator().state == 42);
        assert_string_invariants(testDataAlloc2.get(), sourceStr);
    }
}

void test_case_VSO_802346_unordered_set() {
    using TestType = garbage_data<unordered_set<int, hash<int>, equal_to<>, stateful_allocator<int>>>;
    TestType source;
    source.construct(size_t{}, stateful_allocator<int>(1234));
    const auto oldBuckets = source.get().bucket_count();
    assert(oldBuckets == unordered_set<int>().bucket_count());
    TestType target;
    target.construct(move(source.get()), stateful_allocator<int>(42));
    assert(target.get().bucket_count() == oldBuckets);
}

// LWG-3195 "What is the stored pointer value of an empty weak_ptr?"
void test_weak_ptr_construction() {
    // NB: THIS IS SUPER NOT PORTABLE.
    // TRANSITION, atomic<weak_ptr<T>>

    int i = 42;
    shared_ptr<int> x(shared_ptr<int>{}, &i);
    const auto all_zero = [](const auto ptr) {
        const auto first = reinterpret_cast<const char*>(ptr);
        const auto last  = reinterpret_cast<const char*>(ptr + 1);
        return all_of(first, last, [](const auto x) { return x == 0; });
    };

    { // Conversion from an empty shared_ptr lvalue with a non-null stored pointer value properly value-initializes
        // both the control block and stored value pointers.
        garbage_data<weak_ptr<int>> testData;
        testData.construct(x);
        assert(all_zero(testData.ptr()));
    }

    { // Conversion from an empty shared_ptr rvalue with a non-null stored pointer value properly value-initializes
        // both the control block and stored value pointers.
        garbage_data<weak_ptr<int>> testData;
        testData.construct(shared_ptr<int>(x));
        assert(all_zero(testData.ptr()));
    }

    x.reset(&i, [](int*) {});

    const auto owner_eq = [](const auto& x, const auto& y) { return !(owner_less<>{}(x, y) || owner_less<>{}(y, x)); };

    { // Conversion from a non-empty shared_ptr lvalue with a non-null stored pointer value properly initializes both
        // the control block and stored value pointers.
        garbage_data<weak_ptr<int>> testData;
        testData.construct(x);
        void* stored;
        memcpy(&stored, testData.ptr(), sizeof(void*));
        assert(stored == &i);
        assert(owner_eq(testData.get(), x));
    }

    { // Conversion from a non-empty shared_ptr rvalue with a non-null stored pointer value properly initializes both
        // the control block and stored value pointers.
        garbage_data<weak_ptr<int>> testData;
        testData.construct(shared_ptr<int>(x));
        void* stored;
        memcpy(&stored, testData.ptr(), sizeof(void*));
        assert(stored == &i);
        assert(owner_eq(testData.get(), x));
    }
}

int main() {
    test_case_string_construction_no_data();
    test_case_string_construction("abc"); // < SSO
    test_case_string_construction("abcd long text that is longer than the small string optimization"); // > SSO
    test_case_VSO_802346_unordered_set();
    test_weak_ptr_construction();
}
