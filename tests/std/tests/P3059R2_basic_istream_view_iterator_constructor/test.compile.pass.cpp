// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <ranges>
#include <sstream>

using namespace std;

using View     = ranges::basic_istream_view<int, char>;
using Iterator = ranges::iterator_t<View>;

// P3059R2: users must not be able to construct
// basic_istream_view::iterator directly from the parent view.
static_assert(!constructible_from<Iterator, View&>);

// The iterator must remain copyable and movable through its public interface.
static_assert(!copy_constructible<Iterator>);
static_assert(move_constructible<Iterator>);
