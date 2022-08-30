// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#if _HAS_CXX20
#include <span>
#endif // _HAS_CXX20

using namespace std;

int main() {} // COMPILE-ONLY

#pragma warning(disable : 4251) // class 'A' needs to have dll-interface to be used by clients of class 'B'
#pragma warning(disable : 4275) // non dll-interface struct 'A' used as base for dll-interface class 'B'

struct __declspec(dllexport) ExportedArray : array<int, 3> {};
struct __declspec(dllexport) ExportedArrayZero : array<int, 0> {};
struct __declspec(dllexport) ExportedDeque : deque<int> {};
struct __declspec(dllexport) ExportedForwardList : forward_list<int> {};
struct __declspec(dllexport) ExportedList : list<int> {};
struct __declspec(dllexport) ExportedVector : vector<int> {};
struct __declspec(dllexport) ExportedVectorBool : vector<bool> {};

struct __declspec(dllexport) ExportedMap : map<int, int> {};
struct __declspec(dllexport) ExportedMultimap : multimap<int, int> {};
struct __declspec(dllexport) ExportedSet : set<int> {};
struct __declspec(dllexport) ExportedMultiset : multiset<int> {};

struct __declspec(dllexport) ExportedUnorderedMap : unordered_map<int, int> {};
struct __declspec(dllexport) ExportedUnorderedMultimap : unordered_multimap<int, int> {};
struct __declspec(dllexport) ExportedUnorderedSet : unordered_set<int> {};
struct __declspec(dllexport) ExportedUnorderedMultiset : unordered_multiset<int> {};

struct __declspec(dllexport) ExportedQueue : queue<int> {};
struct __declspec(dllexport) ExportedPriorityQueue : priority_queue<int> {};
struct __declspec(dllexport) ExportedStack : stack<int> {};

#if _HAS_CXX20
struct __declspec(dllexport) ExportedSpan : span<int> {};
struct __declspec(dllexport) ExportedSpanThree : span<int, 3> {};
#endif // _HAS_CXX20

// Test GH-3013 "<utility>: pair::swap(const pair&) interacts badly with __declspec(dllexport)"
struct __declspec(dllexport) ExportedPair : pair<int, int> {};
struct __declspec(dllexport) ExportedTuple : tuple<int, int, int> {};
