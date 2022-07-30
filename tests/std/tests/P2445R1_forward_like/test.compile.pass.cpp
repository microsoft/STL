// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <type_traits>
#include <utility>

using namespace std;

struct owner_type {
    inline static constinit int imut = 0;
    static constexpr int iconst      = 0;

    int m_mutobj    = 0;
    int& m_mutlref  = imut;
    int&& m_mutrref = move(imut);

    const int m_cobj    = 0;
    const int& m_clref  = iconst;
    const int&& m_crref = move(iconst);
};

constinit owner_type owner{};
constexpr owner_type& owner_lref  = owner;
constexpr owner_type&& owner_rref = move(owner);

constexpr owner_type owner_c{};
constexpr const owner_type& owner_clref  = owner_c;
constexpr const owner_type&& owner_crref = move(owner_c);

static_assert(is_same_v<decltype(forward_like<decltype(owner)>(owner.m_mutobj)), int&&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner)>(owner.m_mutlref)), int&&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner)>(owner.m_mutrref)), int&&>);

static_assert(is_same_v<decltype(forward_like<decltype(owner)>(owner.m_cobj)), const int&&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner)>(owner.m_clref)), const int&&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner)>(owner.m_crref)), const int&&>);

static_assert(is_same_v<decltype(forward_like<decltype(owner_lref)>(owner_lref.m_mutobj)), int&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner_lref)>(owner_lref.m_mutlref)), int&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner_lref)>(owner_lref.m_mutrref)), int&>);

static_assert(is_same_v<decltype(forward_like<decltype(owner_lref)>(owner_lref.m_cobj)), const int&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner_lref)>(owner_lref.m_clref)), const int&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner_lref)>(owner_lref.m_crref)), const int&>);

static_assert(is_same_v<decltype(forward_like<decltype(owner_rref)>(owner_rref.m_mutobj)), int&&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner_rref)>(owner_rref.m_mutlref)), int&&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner_rref)>(owner_rref.m_mutrref)), int&&>);

static_assert(is_same_v<decltype(forward_like<decltype(owner_rref)>(owner_rref.m_cobj)), const int&&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner_rref)>(owner_rref.m_clref)), const int&&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner_rref)>(owner_rref.m_crref)), const int&&>);

static_assert(is_same_v<decltype(forward_like<decltype(owner_c)>(owner_c.m_mutobj)), const int&&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner_c)>(owner_c.m_mutlref)), const int&&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner_c)>(owner_c.m_mutrref)), const int&&>);

static_assert(is_same_v<decltype(forward_like<decltype(owner_c)>(owner_c.m_cobj)), const int&&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner_c)>(owner_c.m_clref)), const int&&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner_c)>(owner_c.m_crref)), const int&&>);

static_assert(is_same_v<decltype(forward_like<decltype(owner_clref)>(owner_clref.m_mutobj)), const int&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner_clref)>(owner_clref.m_mutlref)), const int&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner_clref)>(owner_clref.m_mutrref)), const int&>);

static_assert(is_same_v<decltype(forward_like<decltype(owner_clref)>(owner_clref.m_cobj)), const int&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner_clref)>(owner_clref.m_clref)), const int&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner_clref)>(owner_clref.m_crref)), const int&>);

static_assert(is_same_v<decltype(forward_like<decltype(owner_crref)>(owner_crref.m_mutobj)), const int&&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner_crref)>(owner_crref.m_mutlref)), const int&&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner_crref)>(owner_crref.m_mutrref)), const int&&>);

static_assert(is_same_v<decltype(forward_like<decltype(owner_crref)>(owner_crref.m_cobj)), const int&&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner_crref)>(owner_crref.m_clref)), const int&&>);
static_assert(is_same_v<decltype(forward_like<decltype(owner_crref)>(owner_crref.m_crref)), const int&&>);

int main() {} // COMPILE-ONLY
