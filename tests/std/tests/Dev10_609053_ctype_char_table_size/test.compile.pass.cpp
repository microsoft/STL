// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <locale>

static_assert(std::ctype<char>::table_size == 256, "");
