// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <string>
#include <system_error>

using namespace std;

struct meow_category : error_category {
    constexpr meow_category() = default;

    const char* name() const noexcept override {
        return "meow";
    }

    string message(int) const override {
        return "meow";
    }

    // TRANSITION: As of Boost 1.80.0, boost::system::detail::std_category assigns to _Addr.
    void test_workaround_for_non_standard_code(const unsigned int val) noexcept {
        _Addr = val;
    }
};

#if _HAS_CXX20
[[maybe_unused]] constexpr meow_category meowing_cat;
#endif // _HAS_CXX20
