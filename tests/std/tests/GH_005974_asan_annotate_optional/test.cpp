// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// REQUIRES: x64 || x86 || arm64

#if defined(__clang__) && defined(_M_ARM64) // TRANSITION, LLVM-184902, fixed in Clang 23
#pragma comment(linker, "/INFERASANLIBS")
int main() {}
#else // ^^^ workaround / no workaround vvv

#include <cassert>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
using namespace std;

#if _HAS_CXX20
#define CONSTEXPR20 constexpr
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
#define CONSTEXPR20 inline
#endif // ^^^ !_HAS_CXX20 ^^^

#ifdef __SANITIZE_ADDRESS__
extern "C" int __cdecl __asan_address_is_poisoned(const volatile void* addr);
#define ASAN_VERIFY_POISONED(addr)   assert(__asan_address_is_poisoned((addr)) != 0)
#define ASAN_VERIFY_UNPOISONED(addr) assert(__asan_address_is_poisoned((addr)) == 0)
#else // ^^^ defined(__SANITIZE_ADDRESS__) / !defined(__SANITIZE_ADDRESS__) vvv
#define ASAN_VERIFY_POISONED(addr)   ((void) (addr))
#define ASAN_VERIFY_UNPOISONED(addr) ((void) (addr))
#endif // ^^^ !defined(__SANITIZE_ADDRESS__) ^^^

template <class StrType>
void test_poisoning() {
    // std::optional<T> is ASan-annotated only for non-trivially destructible T.
    static_assert(!is_trivially_destructible_v<StrType>);
    {
        // Verify layout assumption, specific to our implementation but not a guarantee provided to users.
        // This test assumes that a std::optional's contained value is stored at offset 0, so it has the same address.
        const optional<StrType> opt{"cats"};
        assert(static_cast<const void*>(&opt) == static_cast<const void*>(&opt.value()));
    }
    {
        // Same layout assumption for std::variant.
        const variant<optional<StrType>, int> var{"cats"};
        assert(static_cast<const void*>(&var) == static_cast<const void*>(&get<optional<StrType>>(var).value()));
    }
    {
        optional<StrType> opt;
        ASAN_VERIFY_POISONED(&opt);
        assert(!opt.has_value());

        opt.emplace("cats");
        ASAN_VERIFY_UNPOISONED(&opt);
        assert(opt.value() == "cats");
    }
    {
        optional<StrType> opt{"cats"};
        ASAN_VERIFY_UNPOISONED(&opt);
        assert(opt.value() == "cats");

        opt.reset();
        ASAN_VERIFY_POISONED(&opt);
        assert(!opt.has_value());
    }
    {
        // Verify that std::optional's destructor unpoisons its storage, so the bytes can be reused for another object.
        variant<optional<StrType>, int> var{"cats"};
        ASAN_VERIFY_UNPOISONED(&var);
        assert(get<optional<StrType>>(var).value() == "cats");

        get<optional<StrType>>(var).reset();
        ASAN_VERIFY_POISONED(&var);
        assert(!get<optional<StrType>>(var).has_value());

        var = 1729;
        ASAN_VERIFY_UNPOISONED(&var);
        assert(get<int>(var) == 1729);
    }
}

#if _HAS_CXX20
template <class StrType>
constexpr bool test_constexpr() {
    {
        optional<StrType> opt;
        assert(!opt.has_value());
        opt.emplace("cats");
        assert(opt.value() == "cats");
    }
    {
        optional<StrType> opt{"cats"};
        assert(opt.value() == "cats");
        opt.reset();
        assert(!opt.has_value());
    }
    return true;
}
#endif // _HAS_CXX20

class MyString {
public:
    constexpr MyString(const char* const ptr) : m_sv{ptr} {}

    CONSTEXPR20 ~MyString() {} // non-trivially destructible

    constexpr bool operator==(const char* const ptr) const {
        return m_sv == ptr;
    }

private:
    string_view m_sv{};
};
static_assert(!is_trivially_destructible_v<MyString>);

int main() {
    // Test std::optional with both std::string and MyString as value types.
    // std::string is the realistic scenario.
    // MyString ensures that we're exercising std::optional's ASan annotations instead of std::string's.
    test_poisoning<string>();
    test_poisoning<MyString>();
#if _HAS_CXX20
    static_assert(test_constexpr<string>());
    static_assert(test_constexpr<MyString>());
#endif // _HAS_CXX20
}

#endif // ^^^ no workaround ^^^
