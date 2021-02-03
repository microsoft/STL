// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <format>
#include <string>
#include <string_view>

// TODO: fill in tests
template std::back_insert_iterator<std::string> std::vformat_to(std::back_insert_iterator<std::string>, const locale&,
    std::string_view, std::format_args_t<std::back_insert_iterator<std::string>, char>);


int main() {}
