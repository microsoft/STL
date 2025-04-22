// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <string>

#if _HAS_CXX23
#include <ranges>
#endif // _HAS_CXX23

using namespace std;

template <class Str>
void test() {
    using CharT = typename Str::value_type;
    {
        volatile CharT arr[1]{CharT{'*'}};

        Str s(begin(arr), end(arr));
        assert(s.size() == 1);
        assert(s[0] == CharT{'*'});

        s.append(begin(arr), end(arr));
        assert(s.size() == 2);
        assert(s[0] == CharT{'*'});
        assert(s[1] == CharT{'*'});

        s.assign(begin(arr), end(arr));
        assert(s.size() == 1);
        assert(s[0] == CharT{'*'});

        s.insert(s.begin(), begin(arr), end(arr));
        assert(s.size() == 2);
        assert(s[0] == CharT{'*'});
        assert(s[1] == CharT{'*'});

        s.replace(s.begin(), s.end(), begin(arr), end(arr));
        assert(s.size() == 1);
        assert(s[0] == CharT{'*'});
    }
    {
        const volatile CharT arr[1]{CharT{'*'}};

        Str s(begin(arr), end(arr));
        assert(s.size() == 1);
        assert(s[0] == CharT{'*'});

        s.append(begin(arr), end(arr));
        assert(s.size() == 2);
        assert(s[0] == CharT{'*'});
        assert(s[1] == CharT{'*'});

        s.assign(begin(arr), end(arr));
        assert(s.size() == 1);
        assert(s[0] == CharT{'*'});

        s.insert(s.begin(), begin(arr), end(arr));
        assert(s.size() == 2);
        assert(s[0] == CharT{'*'});
        assert(s[1] == CharT{'*'});

        s.replace(s.begin(), s.end(), begin(arr), end(arr));
        assert(s.size() == 1);
        assert(s[0] == CharT{'*'});
    }
#if _HAS_CXX23
    {
        volatile CharT arr[1]{CharT{'*'}};

        Str s = {from_range, arr};
        assert(s.size() == 1);
        assert(s[0] == CharT{'*'});

        s.append_range(arr);
        assert(s.size() == 2);
        assert(s[0] == CharT{'*'});
        assert(s[1] == CharT{'*'});

        s.assign_range(arr);
        assert(s.size() == 1);
        assert(s[0] == CharT{'*'});

        s.insert_range(s.begin(), arr);
        assert(s.size() == 2);
        assert(s[0] == CharT{'*'});
        assert(s[1] == CharT{'*'});

        s.replace_with_range(s.begin(), s.end(), arr);
        assert(s.size() == 1);
        assert(s[0] == CharT{'*'});
    }
    {
        const volatile CharT arr[1]{CharT{'*'}};

        Str s = {from_range, arr};
        assert(s.size() == 1);
        assert(s[0] == CharT{'*'});

        s.append_range(arr);
        assert(s.size() == 2);
        assert(s[0] == CharT{'*'});
        assert(s[1] == CharT{'*'});

        s.assign_range(arr);
        assert(s.size() == 1);
        assert(s[0] == CharT{'*'});

        s.insert_range(s.begin(), arr);
        assert(s.size() == 2);
        assert(s[0] == CharT{'*'});
        assert(s[1] == CharT{'*'});

        s.replace_with_range(s.begin(), s.end(), arr);
        assert(s.size() == 1);
        assert(s[0] == CharT{'*'});
    }
#endif // _HAS_CXX23
}

int main() {
    test<string>();
#ifdef __cpp_char8_t
    test<u8string>();
#endif // defined(__cpp_char8_t)
    test<u16string>();
    test<u32string>();
    test<wstring>();

#if _HAS_CXX17
    test<pmr::string>();
#ifdef __cpp_char8_t
    test<pmr::u8string>();
#endif // defined(__cpp_char8_t)
    test<pmr::u16string>();
    test<pmr::u32string>();
    test<pmr::wstring>();
#endif // _HAS_CXX17
}
