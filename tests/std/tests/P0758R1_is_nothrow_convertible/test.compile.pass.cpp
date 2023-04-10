// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <type_traits>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

struct Cat {};

struct ExplicitlyConvertibleFromCat {
    explicit ExplicitlyConvertibleFromCat(const Cat&) {}
};

struct ImplicitlyConvertibleFromCat {
    ImplicitlyConvertibleFromCat(const Cat&) {}
};

struct ImplicitlyNothrowConvertibleFromCat {
    ImplicitlyNothrowConvertibleFromCat(const Cat&) noexcept {}
};

struct NothrowAssignableFromCat {
    NothrowAssignableFromCat& operator=(const Cat&) noexcept;
};

struct ImplicitlyNothrowConvertibleFromRvalueRefToCat {
    ImplicitlyNothrowConvertibleFromRvalueRefToCat(Cat&&) noexcept;
};

struct NothrowConvertibleToAnything {
    template <class T>
    operator T() noexcept;
};
struct ConvertibleToAnything {
    template <class T>
    operator T();
};

struct Tiger : Cat {};

STATIC_ASSERT(!is_nothrow_convertible<Cat, ExplicitlyConvertibleFromCat>::value);
STATIC_ASSERT(!is_nothrow_convertible_v<Cat, ExplicitlyConvertibleFromCat>);
STATIC_ASSERT(!is_nothrow_convertible<Cat, ImplicitlyConvertibleFromCat>::value);
STATIC_ASSERT(!is_nothrow_convertible_v<Cat, ImplicitlyConvertibleFromCat>);
STATIC_ASSERT(is_nothrow_convertible<Cat, ImplicitlyNothrowConvertibleFromCat>::value);
STATIC_ASSERT(is_nothrow_convertible_v<Cat, ImplicitlyNothrowConvertibleFromCat>);
STATIC_ASSERT(is_nothrow_convertible_v<int, int>);
STATIC_ASSERT(is_nothrow_convertible_v<char, unsigned char>);
STATIC_ASSERT(is_nothrow_convertible_v<char, signed char>);
STATIC_ASSERT(is_nothrow_convertible_v<float, int>);
STATIC_ASSERT(!is_nothrow_convertible_v<float, Cat>);
STATIC_ASSERT(!is_nothrow_convertible_v<ImplicitlyNothrowConvertibleFromCat, Cat>);
STATIC_ASSERT(is_nothrow_convertible_v<Tiger, Cat>);
STATIC_ASSERT(!is_nothrow_convertible_v<Cat, Tiger>);
STATIC_ASSERT(is_nothrow_convertible_v<NothrowConvertibleToAnything, Cat>);
STATIC_ASSERT(!is_nothrow_convertible_v<ConvertibleToAnything, Cat>);
STATIC_ASSERT(is_nothrow_convertible_v<void, void>);
STATIC_ASSERT(is_nothrow_convertible_v<const void, void>);
STATIC_ASSERT(is_nothrow_convertible_v<void, const void>);
STATIC_ASSERT(is_nothrow_convertible_v<const volatile void, void>);
STATIC_ASSERT(is_nothrow_convertible_v<void, const volatile void>);
STATIC_ASSERT(!is_nothrow_convertible_v<Cat, const volatile void>);
STATIC_ASSERT(!is_nothrow_convertible_v<void, Cat>);
STATIC_ASSERT(!is_nothrow_convertible_v<Cat, NothrowAssignableFromCat>);
STATIC_ASSERT(is_nothrow_convertible_v<Cat, ImplicitlyNothrowConvertibleFromRvalueRefToCat>);
STATIC_ASSERT(!is_nothrow_convertible_v<const Cat&, ImplicitlyNothrowConvertibleFromRvalueRefToCat>);
STATIC_ASSERT(!is_nothrow_convertible_v<ImplicitlyNothrowConvertibleFromRvalueRefToCat, Cat>);
STATIC_ASSERT(is_nothrow_convertible_v<int (&)(int, long), int (&)(int, long)>);
STATIC_ASSERT(is_nothrow_convertible_v<int (&)(int, long), int (*)(int, long)>);
STATIC_ASSERT(is_nothrow_convertible_v<int (*)(int, long), int (*)(int, long)>);
STATIC_ASSERT(!is_nothrow_convertible_v<int (*)(int, long), long (*)(int, long)>);
STATIC_ASSERT(!is_nothrow_convertible_v<int (*)(int, long), int (*)(long, long)>);
STATIC_ASSERT(is_nothrow_convertible_v<int(int, long), int (*)(int, long)>);
STATIC_ASSERT(is_nothrow_convertible_v<int(int, long), int (&)(int, long)>);
STATIC_ASSERT(!is_nothrow_convertible_v<int(int, long) const, int(const int, long)>);
STATIC_ASSERT(!is_nothrow_convertible_v<int(int, long) const, int(int, long) const>);
STATIC_ASSERT(is_nothrow_convertible_v<int (Cat::*)(int, int (*)(int, long)), int (Cat::*)(int, int (*)(int, long))>);
#ifndef __EDG__ // TRANSITION, VSO-892694
STATIC_ASSERT(!is_nothrow_convertible_v<Cat, Cat&>);
#endif
STATIC_ASSERT(is_nothrow_convertible_v<Cat&, Cat>);
STATIC_ASSERT(is_nothrow_convertible_v<Cat, const Cat&>);
STATIC_ASSERT(is_nothrow_convertible_v<Cat, const Cat>);
STATIC_ASSERT(is_nothrow_convertible_v<Cat, const volatile Cat>);
STATIC_ASSERT(is_nothrow_convertible_v<Cat, Cat&&>);
STATIC_ASSERT(is_nothrow_convertible_v<Cat, const Cat&&>);
STATIC_ASSERT(is_nothrow_convertible_v<Cat, const volatile Cat&&>);
STATIC_ASSERT(!is_nothrow_convertible_v<Cat, Cat*>);
STATIC_ASSERT(is_nothrow_convertible_v<Cat*, const Cat*>);
STATIC_ASSERT(is_nothrow_convertible_v<int, char>);
STATIC_ASSERT(is_nothrow_convertible_v<int, signed char>);
STATIC_ASSERT(is_nothrow_convertible_v<int, unsigned char>);
STATIC_ASSERT(is_nothrow_convertible_v<double, char>);
STATIC_ASSERT(is_nothrow_convertible_v<double, signed char>);
STATIC_ASSERT(is_nothrow_convertible_v<double, unsigned char>);
STATIC_ASSERT(is_nothrow_convertible_v<int, bool>);
STATIC_ASSERT(is_nothrow_convertible_v<bool, int>);
STATIC_ASSERT(is_nothrow_convertible_v<int, long long>);
#ifndef __EDG__ // TRANSITION, VSO-892705
STATIC_ASSERT(is_nothrow_convertible_v<int[], int*>);
#endif
STATIC_ASSERT(is_nothrow_convertible_v<int[1], int*>);
STATIC_ASSERT(!is_nothrow_convertible_v<int[], int[]>);
STATIC_ASSERT(!is_nothrow_convertible_v<int[], int[1]>);

// VSO_0105317_expression_sfinae and VSO_0000000_type_traits provide
// additional coverage of this machinery
