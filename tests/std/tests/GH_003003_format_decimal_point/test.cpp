// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <format>

int main() {
    assert(std::format("{:#.0}", 0.0) == "0.");
    assert(std::format("{:#.1}", 0.0) == "0.");
    assert(std::format("{:#.2}", 0.0) == "0.");

    assert(std::format("{:#.0}", 1200.0) == "1.e+03");
    assert(std::format("{:#.1}", 1200.0) == "1.e+03");
    assert(std::format("{:#.2}", 1200.0) == "1.2e+03");
    assert(std::format("{:#.3}", 1200.0) == "1.2e+03");
    assert(std::format("{:#.4}", 1200.0) == "1200.");
    assert(std::format("{:#.5}", 1200.0) == "1200.");
    assert(std::format("{:#.6}", 1200.0) == "1200.");

    assert(std::format("{:#.0}", 0.123) == "0.1");
    assert(std::format("{:#.1}", 0.123) == "0.1");
    assert(std::format("{:#.2}", 0.123) == "0.12");
    assert(std::format("{:#.3}", 0.123) == "0.123");
    assert(std::format("{:#.4}", 0.123) == "0.123");
    assert(std::format("{:#.5}", 0.123) == "0.123");

    assert(std::format("{:#.0}", 10.1) == "1.e+01");
    assert(std::format("{:#.1}", 10.1) == "1.e+01");
    assert(std::format("{:#.2}", 10.1) == "10.");
    assert(std::format("{:#.3}", 10.1) == "10.1");
    assert(std::format("{:#.4}", 10.1) == "10.1");
    assert(std::format("{:#.5}", 10.1) == "10.1");
}
