// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <format>
#include <ranges>

#include <range_algorithm_support.hpp>
#include <test_death.hpp>

using namespace std;

struct based_huge_iterator {
    const char* base_;
    _Signed128 offset_;

    using iterator_category = contiguous_iterator_tag;
    using difference_type   = _Signed128;
    using value_type        = char;

    const char& operator*() const noexcept {
        return base_[static_cast<ptrdiff_t>(offset_)];
    }

    based_huge_iterator& operator++() noexcept {
        ++offset_;
        return *this;
    }
    based_huge_iterator operator++(int) noexcept {
        auto old = *this;
        ++*this;
        return old;
    }

    based_huge_iterator& operator--() noexcept {
        --offset_;
        return *this;
    }
    based_huge_iterator operator--(int) noexcept {
        auto old = *this;
        --*this;
        return old;
    }

    based_huge_iterator& operator+=(difference_type n) noexcept {
        offset_ += n;
        return *this;
    }

    based_huge_iterator& operator-=(difference_type n) noexcept {
        offset_ -= n;
        return *this;
    }

    const char* operator->() const noexcept {
        return base_ + static_cast<ptrdiff_t>(offset_);
    }

    const char& operator[](difference_type n) const noexcept {
        return base_[static_cast<ptrdiff_t>(offset_ + n)];
    }

    friend based_huge_iterator operator+(based_huge_iterator i, difference_type n) noexcept {
        return {i.base_, i.offset_ + n};
    }
    friend based_huge_iterator operator+(difference_type n, based_huge_iterator i) noexcept {
        return {i.base_, i.offset_ + n};
    }

    friend based_huge_iterator operator-(based_huge_iterator i, difference_type n) noexcept {
        return {i.base_, i.offset_ - n};
    }
    friend difference_type operator-(based_huge_iterator i, based_huge_iterator j) noexcept {
        assert(i.base_ == j.base_);
        return i.offset_ - j.offset_;
    }

    friend auto operator<=>(const based_huge_iterator&, const based_huge_iterator&) = default;
};

struct based_huge_view : ranges::view_interface<based_huge_view> {
    const char* base_;
    _Signed128 size_;

    based_huge_iterator begin() const noexcept {
        return {base_, 0};
    }

    based_huge_iterator end() const noexcept {
        return {base_, size_};
    }
};

static_assert(ranges::contiguous_range<based_huge_view>);

void test_case_invalid_range_size() {
    based_huge_view v{.base_ = "", .size_ = static_cast<size_t>(PTRDIFF_MAX) + 1};
    (void) format("{:s}", v);
}

int main(int argc, char** argv) {
    std_testing::death_test_executive exec;

#ifdef _DEBUG
    exec.add_death_tests({
        test_case_invalid_range_size,
    });
#endif // defined(_DEBUG)

    return exec.run(argc, argv);
}
