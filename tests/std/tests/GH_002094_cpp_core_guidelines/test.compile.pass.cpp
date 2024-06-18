// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <__msvc_all_public_headers.hpp>

using namespace std;

void instantiate_regex_machinery() {
    const regex r{R"(.+meow.+)"};
    assert(regex_match("homeowner", r));
}

#if _HAS_CXX20
void instantiate_chrono_parse_machinery() {
    istringstream iss{"10:02:07"};
    chrono::seconds s{};
    iss >> chrono::parse("%H:%M:%S", s);
    assert(s == 36127s);
}
#endif // ^^^ _HAS_CXX20 ^^^

#if _HAS_CXX23
template <class T>
struct WrappedVector : vector<T> {
    using vector<T>::vector;
};

template <class T>
struct std::formatter<WrappedVector<T>, char> {
public:
    template <class ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return underlying.parse(ctx);
    }

    template <class FormatContext>
    auto format(const WrappedVector<T>& rng, FormatContext& ctx) const {
        return underlying.format(rng, ctx);
    }

private:
    range_formatter<T, char> underlying;
};

void instantiate_range_formatter_machinery() {
    const WrappedVector<int> v{11, 22, 33, 44};
    assert(format("{}", v) == "[11, 22, 33, 44]");
    assert(format("{:}", v) == "[11, 22, 33, 44]");
    assert(format("{:n}", v) == "11, 22, 33, 44");
}
#endif // ^^^ _HAS_CXX23 ^^^
