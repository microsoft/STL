// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <functional>
#include <type_traits>

using namespace std;

// TRANSITION, MSVC and EDG haven't implemented intrinsics needed for P2255R2.
#ifdef __cpp_lib_reference_from_temporary
struct LvalueTempFunctor {
    int operator()() const& noexcept;
    const int& operator()() const&& noexcept;
};

static_assert(is_invocable_r_v<const int&, int& (*) ()>);
static_assert(!is_invocable_r_v<const int&, short& (*) ()>);
static_assert(!is_invocable_r_v<const int&, int (*)()>);
static_assert(!is_invocable_r_v<int&&, int (*)()>);
static_assert(is_invocable_r_v<int, int (*)()>);
static_assert(is_invocable_r_v<const int&, LvalueTempFunctor>);
static_assert(!is_invocable_r_v<const int&, LvalueTempFunctor&>);

static_assert(is_constructible_v<function<const int&()>, int& (*) ()>);
static_assert(!is_constructible_v<function<const int&()>, short& (*) ()>);
static_assert(!is_constructible_v<function<const int&()>, int (*)()>);
static_assert(!is_constructible_v<function<const int&()>, LvalueTempFunctor>);

static_assert(is_constructible_v<move_only_function<const int&()>, int& (*) ()>);
static_assert(!is_constructible_v<move_only_function<const int&()>, short& (*) ()>);
static_assert(!is_constructible_v<move_only_function<const int&()>, int (*)()>);

static_assert(is_constructible_v<move_only_function<const int&() const>, int& (*) ()>);
static_assert(!is_constructible_v<move_only_function<const int&() const>, short& (*) ()>);
static_assert(!is_constructible_v<move_only_function<const int&() const>, int (*)()>);

static_assert(!is_constructible_v<move_only_function<const int&() &>, LvalueTempFunctor>);
static_assert(!is_constructible_v<move_only_function<const int&() const&>, LvalueTempFunctor>);
static_assert(is_constructible_v<move_only_function<const int&() &&>, LvalueTempFunctor>);
static_assert(is_constructible_v<move_only_function<const int&() const&&>, LvalueTempFunctor>);

#ifdef __cpp_noexcept_function_type
static_assert(is_nothrow_invocable_r_v<const int&, int& (*) () noexcept>);
static_assert(!is_nothrow_invocable_r_v<const int&, short& (*) () noexcept>);
static_assert(!is_nothrow_invocable_r_v<const int&, int (*)() noexcept>);
static_assert(!is_nothrow_invocable_r_v<int&&, int (*)() noexcept>);
static_assert(is_nothrow_invocable_r_v<int, int (*)() noexcept>);
static_assert(is_nothrow_invocable_r_v<const int&, LvalueTempFunctor>);
static_assert(!is_nothrow_invocable_r_v<const int&, LvalueTempFunctor&>);

static_assert(is_constructible_v<move_only_function<const int&() noexcept>, int& (*) () noexcept>);
static_assert(!is_constructible_v<move_only_function<const int&() noexcept>, short& (*) () noexcept>);
static_assert(!is_constructible_v<move_only_function<const int&() noexcept>, int (*)() noexcept>);

static_assert(is_constructible_v<move_only_function<const int&() const noexcept>, int& (*) () noexcept>);
static_assert(!is_constructible_v<move_only_function<const int&() const noexcept>, short& (*) () noexcept>);
static_assert(!is_constructible_v<move_only_function<const int&() const noexcept>, int (*)() noexcept>);

static_assert(!is_constructible_v<move_only_function<const int&() & noexcept>, LvalueTempFunctor>);
static_assert(!is_constructible_v<move_only_function<const int&() const & noexcept>, LvalueTempFunctor>);
static_assert(is_constructible_v<move_only_function<const int&() && noexcept>, LvalueTempFunctor>);
static_assert(is_constructible_v<move_only_function<const int&() const && noexcept>, LvalueTempFunctor>);
#endif // defined(__cpp_noexcept_function_type)
#endif // ^^^ no workaround ^^^
