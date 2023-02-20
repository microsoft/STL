// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <chrono>
#include <cstddef>
#include <type_traits>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

#ifdef __cpp_lib_byte
STATIC_ASSERT(_Is_trivially_swappable_v<byte>);
#endif // __cpp_lib_byte
STATIC_ASSERT(_Is_trivially_swappable_v<unsigned char>);
STATIC_ASSERT(_Is_trivially_swappable_v<unsigned short>);
STATIC_ASSERT(_Is_trivially_swappable_v<unsigned int>);
STATIC_ASSERT(_Is_trivially_swappable_v<unsigned long>);
STATIC_ASSERT(_Is_trivially_swappable_v<unsigned long long>);

STATIC_ASSERT(_Is_trivially_swappable_v<signed char>);
STATIC_ASSERT(_Is_trivially_swappable_v<short>);
STATIC_ASSERT(_Is_trivially_swappable_v<int>);
STATIC_ASSERT(_Is_trivially_swappable_v<long>);
STATIC_ASSERT(_Is_trivially_swappable_v<long long>);

STATIC_ASSERT(_Is_trivially_swappable_v<char>);
STATIC_ASSERT(_Is_trivially_swappable_v<wchar_t>);

STATIC_ASSERT(_Is_trivially_swappable_v<float>);
STATIC_ASSERT(_Is_trivially_swappable_v<double>);
STATIC_ASSERT(_Is_trivially_swappable_v<long double>);

enum class byte2 : unsigned char {};
STATIC_ASSERT(_Is_trivially_swappable_v<byte2>);

enum class byte3 : unsigned char {};
void swap(byte3& a, byte3& b);
STATIC_ASSERT(!_Is_trivially_swappable_v<byte3>);

struct good {
    int val;
};
STATIC_ASSERT(_Is_trivially_swappable_v<good>);

struct not_trivial_destructor {
    int val;
    ~not_trivial_destructor();
};
STATIC_ASSERT(!_Is_trivially_swappable_v<not_trivial_destructor>);

struct not_trivial_move_constructor {
    int val;
    not_trivial_move_constructor(not_trivial_move_constructor&&);
};
STATIC_ASSERT(!_Is_trivially_swappable_v<not_trivial_move_constructor>);

struct not_trivial_move_assign {
    int val;
    not_trivial_move_assign& operator=(not_trivial_move_assign&&);
};
STATIC_ASSERT(!_Is_trivially_swappable_v<not_trivial_move_assign>);

STATIC_ASSERT(_Is_trivially_swappable_v<chrono::duration<int>>);
STATIC_ASSERT(_Is_trivially_swappable_v<chrono::time_point<chrono::system_clock>>);
