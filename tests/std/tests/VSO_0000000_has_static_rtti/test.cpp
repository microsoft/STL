// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <exception>
#include <functional>
#include <memory>
#include <regex>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>
#include <utility>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <typename T, typename = void>
struct HasRethrowIfNested : false_type {};

template <typename T>
struct HasRethrowIfNested<T, void_t<decltype(rethrow_if_nested(declval<T>()))>> : true_type {};


template <typename T, typename U, typename = void>
struct HasDynamicPointerCast : false_type {};

template <typename T, typename U>
struct HasDynamicPointerCast<T, U, void_t<decltype(dynamic_pointer_cast<T>(declval<U>()))>> : true_type {};


template <typename T, typename U, typename = void>
struct HasGetDeleter : false_type {};

template <typename T, typename U>
struct HasGetDeleter<T, U, void_t<decltype(get_deleter<T>(declval<U>()))>> : true_type {};


template <typename T, typename = void>
struct HasTargetType : false_type {};

template <typename T>
struct HasTargetType<T, void_t<decltype(declval<T>().target_type())>> : true_type {};


template <typename T, typename U, typename = void>
struct HasTarget : false_type {};

template <typename T, typename U>
struct HasTarget<T, U, void_t<decltype(declval<T>().template target<U>())>> : true_type {};


int main() {
    {
#ifdef _CPPRTTI
        const runtime_error re("WOOF");
        rethrow_if_nested(re);

        STATIC_ASSERT(HasRethrowIfNested<logic_error>::value);
#else // _CPPRTTI
        STATIC_ASSERT(!HasRethrowIfNested<logic_error>::value);
#endif // _CPPRTTI
    }

    {
#ifdef _CPPRTTI
        const shared_ptr<exception> base      = make_shared<regex_error>(regex_constants::error_paren);
        const shared_ptr<regex_error> derived = dynamic_pointer_cast<regex_error>(base);
        assert(derived && derived->code() == regex_constants::error_paren);

        STATIC_ASSERT(HasDynamicPointerCast<logic_error, shared_ptr<exception>>::value);
#else // _CPPRTTI
        STATIC_ASSERT(!HasDynamicPointerCast<logic_error, shared_ptr<exception>>::value);
#endif // _CPPRTTI
    }

    {
        shared_ptr<int> sp1(new int(11));
        shared_ptr<int> sp2(new int(22), default_delete<int>{});
        shared_ptr<int> sp3(new int(33), default_delete<int>{}, allocator<int>{});

#if _HAS_STATIC_RTTI
        assert(!get_deleter<default_delete<int>>(sp1));
        assert(get_deleter<default_delete<int>>(sp2) != nullptr);
        assert(get_deleter<default_delete<int>>(sp3) != nullptr);

        STATIC_ASSERT(HasGetDeleter<default_delete<short>, shared_ptr<short>>::value);
#else // _HAS_STATIC_RTTI
        STATIC_ASSERT(!HasGetDeleter<default_delete<short>, shared_ptr<short>>::value);
#endif // _HAS_STATIC_RTTI
    }

    {
        function<int(int, int)> f(plus<>{});
        const auto& c = f;

        assert(f(123, 456) == 579);
        assert(c(123, 456) == 579);

#if _HAS_STATIC_RTTI
        assert(f.target_type() == typeid(plus<>));
        assert(c.target_type() == typeid(plus<>));
        assert(f.target<plus<>>() != nullptr);
        assert(c.target<plus<>>() != nullptr);
        assert(!f.target<minus<>>());
        assert(!c.target<minus<>>());

        STATIC_ASSERT(HasTargetType<function<long(long, long)>>::value);
        STATIC_ASSERT(HasTarget<function<long(long, long)>, multiplies<>>::value);
        STATIC_ASSERT(HasTarget<const function<long(long, long)>, multiplies<>>::value);
#else // _HAS_STATIC_RTTI
        STATIC_ASSERT(!HasTargetType<function<long(long, long)>>::value);
        STATIC_ASSERT(!HasTarget<function<long(long, long)>, multiplies<>>::value);
        STATIC_ASSERT(!HasTarget<const function<long(long, long)>, multiplies<>>::value);
#endif // _HAS_STATIC_RTTI
    }
}
