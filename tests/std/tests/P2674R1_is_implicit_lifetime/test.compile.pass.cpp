// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <type_traits>

enum UnscopedEnum { Enumerator };
enum struct ScopedEnum { Enumerator };
class Class {};
class IncompleteClass;
class UserProvidedDestructorClass
{
	~UserProvidedDestructorClass() {}
};

using namespace std;

int main()
{
#ifdef __cpp_lib_is_implicit_lifetime 
	// this is a test to test whether
	// is_implicit_lifetime_v produces desired results
	// compiles under std namespace

	// Basics
	static_assert(is_implcit_lifetime_v<int>);
	static_assert(is_implcit_lifetime_v<int*>);
	static_assert(is_implcit_lifetime_v<nullptr_t>);
	static_assert(is_implcit_lifetime_v<UnscopedEnum>);
	static_assert(is_implcit_lifetime_v<ScopedEnum>);
	static_assert(is_implcit_lifetime_v<void(*)()>);
	static_assert(is_implcit_lifetime_v<Class>);
	static_assert(!is_implcit_lifetime_v<UserProvidedDestructorClass>);
	static_assert(!is_implcit_lifetime_v<IncompleteClass[]>);
	static_assert(!is_implcit_lifetime_v<IncompleteClass[20]>);

	// Arrays
	static_assert(is_implcit_lifetime_v<int[]>);
	static_assert(is_implcit_lifetime_v<int[2]>);
	static_assert(is_implcit_lifetime_v<Class[10]>);
	static_assert(!is_implcit_lifetime_v<UserProvidedDestructorClass[12]>);

	static_assert(!is_implcit_lifetime_v<void>);
	static_assert(!is_implcit_lifetime_v<const volatile void>);
	static_assert(!is_implcit_lifetime_v<long&>);
	static_assert(!is_implcit_lifetime_v<long&&>);
#endif
}

class IncompleteClass {};