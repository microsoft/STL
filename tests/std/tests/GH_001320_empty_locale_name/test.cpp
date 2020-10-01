// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <exception>
#include <iostream>
#include <locale>
#include <type_traits>

using namespace std;
namespace {
    struct deletable_facet : public codecvt_byname<wchar_t, char, mbstate_t> {
        deletable_facet(const std::string& name) : codecvt_byname<wchar_t, char, mbstate_t>(name.c_str()) {}
        ~deletable_facet() = default;
    };
} // namespace

wstring FromNarrowString(const char* from, const char* to, const locale& l) {
    const deletable_facet cvt{l.name()};
    mbstate_t mbstate{};
    const size_t externalSize = to - from;
    wstring resultWStr(externalSize, '\0');
    const char* from_next;
    wchar_t* to_next;

    // Issue number 3, cvt.in returns an error.
    codecvt_base::result result =
        cvt.in(mbstate, from, to, from_next, &resultWStr[0], &resultWStr[resultWStr.size()], to_next);
    if (result != codecvt_base::ok) {
        throw std::runtime_error("Error converting locale multibyte string to UNICODE");
    }
    resultWStr.resize(to_next - &resultWStr[0]);
    return resultWStr;
}

int main() {
    {
        // Issue number 1. The locale name should not be empty according to the reporter. Other compilers returns "C".
        assert(!locale("").name().empty());
    }

    {
        // Issue number 2. Microsoft's STL throws "bad locale name" for valid locales that ends with ".utf8" or "UTF-8".
        for (const char* localName : {"en_US.utf8", "en_US.UTF-8"}) {
            try {
                const string localMBString = "\x7a\xc3\x9f\xe6\xb0\xb4\xf0\x9d\x84\x8b";
                const wstring wideStr      = L"z\u00df\u6c34\U0001d10b";

                locale l{localName};
                const wstring result =
                    FromNarrowString(localMBString.c_str(), localMBString.c_str() + localMBString.length(), l);
                assert(result == wideStr);
            } catch (const std::exception& ex) {
                std::cout << ex.what() << std::endl;
                return -1;
            }
        }
    }
}
