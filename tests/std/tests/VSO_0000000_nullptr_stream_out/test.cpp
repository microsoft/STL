// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <iosfwd>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <typename Istream, typename T, typename = void>
constexpr bool IstreamExtractable = false;
template <typename Istream, typename T>
constexpr bool IstreamExtractable<Istream, T, void_t<decltype(declval<Istream>() >> declval<T>())>> = true;

class PublicIstream : public istream {};
class PrivateIstream : private istream {};
PrivateIstream& operator>>(PrivateIstream&, int&); // highly artificial test code, see below

template <typename Ostream, typename T, typename = void>
constexpr bool OstreamInsertable = false;
template <typename Ostream, typename T>
constexpr bool OstreamInsertable<Ostream, T, void_t<decltype(declval<Ostream>() << declval<T>())>> = true;

class PublicOstream : public ostream {};
class PrivateOstream : private ostream {};
PrivateOstream& operator<<(PrivateOstream&, int); // highly artificial test code, see below

int main() {
#if _HAS_CXX17
    // LWG-2221 "No formatted output operator for nullptr"
    ostringstream os;
    os << nullptr;
    assert(os.str() == "nullptr");
    wostringstream wos;
    wos << nullptr;
    assert(wos.str() == L"nullptr");
#endif // _HAS_CXX17

    // LWG-1203 "More useful rvalue stream insertion"
    assert((ostringstream{} << 42).str() == "42");
    int x;
    assert((istringstream("42 1729") >> x).str() == "42 1729");
    assert(x == 42);

    // Test GH-538 by verifying that LWG-1203 is constrained by "publicly and unambiguously derived".
    // The "highly artificial" operator overloads above are ensuring that the streaming expressions are well-formed
    // for lvalue PrivateIstream/PrivateOstream, so we can specifically test the inheritance constraint.
    STATIC_ASSERT(IstreamExtractable<istream&, int&>);
    STATIC_ASSERT(IstreamExtractable<istream, int&>);
    STATIC_ASSERT(IstreamExtractable<PublicIstream&, int&>);
    STATIC_ASSERT(IstreamExtractable<PublicIstream, int&>);
    STATIC_ASSERT(IstreamExtractable<PrivateIstream&, int&>);
    STATIC_ASSERT(!IstreamExtractable<PrivateIstream, int&>);

    STATIC_ASSERT(OstreamInsertable<ostream&, int>);
    STATIC_ASSERT(OstreamInsertable<ostream, int>);
    STATIC_ASSERT(OstreamInsertable<PublicOstream&, int>);
    STATIC_ASSERT(OstreamInsertable<PublicOstream, int>);
    STATIC_ASSERT(OstreamInsertable<PrivateOstream&, int>);
    STATIC_ASSERT(!OstreamInsertable<PrivateOstream, int>);
}
