// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <type_traits>

enum UnscopedEnum { Enumerator };
enum struct ScopedEnum { Enumerator };

int main()
{
#ifdef __cpp_lib_is_implicit_lifetime 
	// this is a test to test whether
	// std::is_implicit_lifetime_v produces desired results
	// compiles under std namespace

	// Basics
	static_assert(std::is_implcit_lifetime_v<int>);
	static_assert(std::is_implcit_lifetime_v<std::nullptr_t>);
	static_assert(std::is_implcit_lifetime_v<UnscopedEnum>);
	static_assert(std::is_implcit_lifetime_v<ScopedEnum>);
	static_assert(std::is_implcit_lifetime_v<void(*)()>);

	// Arrays
	int n = 20;
	static_assert(std::is_implcit_lifetime_v<int[]>);
	static_assert(std::is_implcit_lifetime_v<int[2]>);
	static_assert(std::is_implcit_lifetime_v<int[n]>);

	static_assert(!std::is_implcit_lifetime_v<void>);
	static_assert(!std::is_implcit_lifetime_v<const volatile void>);
	static_assert(!std::is_implcit_lifetime_v<long&>);
	static_assert(!std::is_implcit_lifetime_v<long&&>);
#endif
}
