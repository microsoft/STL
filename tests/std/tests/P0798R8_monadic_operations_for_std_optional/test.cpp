// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <exception>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>

using namespace std;

struct Immovable {
    constexpr Immovable(int x) : v(x) {}
    Immovable(const Immovable&)            = delete;
    Immovable(Immovable&&)                 = delete;
    Immovable& operator=(const Immovable&) = delete;
    Immovable& operator=(Immovable&&)      = delete;
    constexpr ~Immovable() {}

    int v;
};

struct Thingy {
    optional<int> x;
    constexpr int member_func() const {
        return 22;
    }
};

template <class T>
struct fn {
    template <class U>
    constexpr auto operator()(U&&) & {
        static_assert(is_same_v<T, U&&>);
        return optional<int>{33};
    }

    template <class U>
    constexpr auto operator()(U&&) && {
        static_assert(is_same_v<T, U&&>);
        return Immovable{44};
    }

    constexpr auto operator()() && {
        return optional<Thingy>{Thingy{55}};
    }
};

template <class Optional>
constexpr void test_impl(Optional&& nonempty, Optional&& empty_optional) {
    assert(nonempty.has_value());
    assert(!empty_optional.has_value());

    using U = decltype(forward<Optional>(nonempty).value());
    {
        fn<U> f{};
        decltype(auto) result = forward<Optional>(nonempty).and_then(f);
        static_assert(is_same_v<decltype(result), optional<int>>);
        assert(result == 33);
    }
    {
        fn<U> f{};
        decltype(auto) result = forward<Optional>(empty_optional).and_then(f);
        assert(!result);
    }
    {
        decltype(auto) result = forward<Optional>(nonempty).and_then(&Thingy::x);
        static_assert(is_same_v<decltype(result), optional<int>>);
        assert(result == 11);
    }
    {
        decltype(auto) result = forward<Optional>(nonempty).transform([](auto&&) { return 66; });
        static_assert(is_same_v<decltype(result), optional<int>>);
        assert(result.value() == 66);
    }
    {
        decltype(auto) result = forward<Optional>(nonempty).transform(fn<U>{});
        static_assert(is_same_v<decltype(result), optional<Immovable>>);
        assert(result.value().v == 44);
    }
    {
        decltype(auto) result = forward<Optional>(empty_optional).transform(fn<U>{});
        assert(!result);
    }
    {
        decltype(auto) result = forward<Optional>(nonempty).transform(&Thingy::member_func);
        static_assert(is_same_v<decltype(result), optional<int>>);
        assert(result == 22);
    }
#ifdef __cpp_lib_concepts
    {
        decltype(auto) result = forward<Optional>(nonempty).or_else(fn<U>{});
        static_assert(is_same_v<decltype(result), optional<Thingy>>);
        assert(result.value().x == 11);
    }
    {
        decltype(auto) result = forward<Optional>(empty_optional).or_else(fn<U>{});
        assert(result.value().x == 55);
    }
#endif // __cpp_lib_concepts
}

constexpr bool test() {
    optional<Thingy> nonempty{Thingy{11}};
    optional<Thingy> empty_optional;
    test_impl(nonempty, empty_optional);
    test_impl(as_const(nonempty), as_const(empty_optional));
    test_impl(move(as_const(nonempty)), move(as_const(empty_optional)));
    test_impl(move(nonempty), move(empty_optional));
    return true;
}

template <class T>
void test_gh_3667() {
    // GH-3667 <optional>: Throwing transformers will cause the program to terminate
    class unique_exception : public exception {};

    try {
        optional<T> opt(in_place);
        opt.transform([](const T&) -> T { throw unique_exception{}; });
    } catch (const unique_exception&) {
        return;
    } catch (...) {
        assert(false); // shouldn't terminate or reach here
    }
    assert(false); // shouldn't terminate or reach here
}

int main() {
    test();
    static_assert(test());

    test_gh_3667<int>(); // trivial destructor
    test_gh_3667<string>(); // non-trivial destructor
}
