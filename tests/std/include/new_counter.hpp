#ifdef _STD_TEST_NEW_COUNTER
#error new_counter.hpp defines non-inline functions and thus must be used only once.
#else // ^^^ _STD_TEST_NEW_COUNTER // !_STD_TEST_NEW_COUNTER vvv
#define _STD_TEST_NEW_COUNTER
#endif // ^^^ !_STD_TEST_NEW_COUNTER

#include <assert.h>
#include <new>
#include <stdlib.h>

#pragma once
#pragma warning(push)
#pragma warning(disable : 28251) // Inconsistent annotation for 'new': this instance has no annotations.

namespace std_testing {
    size_t g_total_news    = 0;
    size_t g_total_deletes = 0;
    size_t g_maximum_news  = 0;

    void reset_new_counters(size_t new_maximum_news) {
        assert(g_total_news == g_total_deletes);
        g_total_news    = 0;
        g_total_deletes = 0;
        g_maximum_news  = new_maximum_news;
    }
} // namespace std_testing

void* operator new(size_t size) {
    void* const p = ::operator new(size, std::nothrow);
    if (p == nullptr) {
        throw std::bad_alloc();
    }

    return p;
}

void* operator new(size_t size, const std::nothrow_t&) noexcept {
    if (std_testing::g_total_news == std_testing::g_maximum_news) {
        return nullptr;
    }
    if (size == 0) {
        ++size;
    }
    ++std_testing::g_total_news;
    return malloc(size);
}

void operator delete(void* ptr) noexcept {
    ::operator delete(ptr, std::nothrow);
}

void operator delete(void* ptr, const std::nothrow_t&) noexcept {
    if (ptr) {
        ++std_testing::g_total_deletes;
        assert(std_testing::g_total_deletes <= std_testing::g_total_news);
        free(ptr);
    }
}

void* operator new[](size_t size) {
    return ::operator new(size);
}

void* operator new[](size_t size, const std::nothrow_t&) noexcept {
    return ::operator new(size, std::nothrow);
}

void operator delete[](void* ptr) noexcept {
    ::operator delete(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t&) noexcept {
    ::operator delete(ptr, std::nothrow);
}

#pragma warning(pop)
