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

owner_type owner{};
constexpr owner_type& owner_lref  = owner;
constexpr owner_type&& owner_rref = move(owner);

constexpr owner_type owner_c{};
constexpr const owner_type& owner_clref  = owner_c;
constexpr const owner_type&& owner_crref = move(owner_c);

using owner_t              = decltype(owner);
using mutobj_from_owner_t  = decltype(forward_like<owner_t>(owner.m_mutobj));
using mutlref_from_owner_t = decltype(forward_like<owner_t>(owner.m_mutlref));
using mutrref_from_owner_t = decltype(forward_like<owner_t>(owner.m_mutrref));
using cobj_from_owner_t    = decltype(forward_like<owner_t>(owner.m_cobj));
using clref_from_owner_t   = decltype(forward_like<owner_t>(owner.m_clref));
using crref_from_owner_t   = decltype(forward_like<owner_t>(owner.m_crref));

static_assert(is_same_v<mutobj_from_owner_t, int&&>);
static_assert(is_same_v<mutlref_from_owner_t, int&&>);
static_assert(is_same_v<mutrref_from_owner_t, int&&>);
static_assert(is_same_v<cobj_from_owner_t, const int&&>);
static_assert(is_same_v<clref_from_owner_t, const int&&>);
static_assert(is_same_v<crref_from_owner_t, const int&&>);

using owner_lref_t              = decltype(owner_lref);
using mutobj_from_owner_lref_t  = decltype(forward_like<owner_lref_t>(owner_lref.m_mutobj));
using mutlref_from_owner_lref_t = decltype(forward_like<owner_lref_t>(owner_lref.m_mutlref));
using mutrref_from_owner_lref_t = decltype(forward_like<owner_lref_t>(owner_lref.m_mutrref));
using cobj_from_owner_lref_t    = decltype(forward_like<owner_lref_t>(owner_lref.m_cobj));
using clref_from_owner_lref_t   = decltype(forward_like<owner_lref_t>(owner_lref.m_clref));
using crref_from_owner_lref_t   = decltype(forward_like<owner_lref_t>(owner_lref.m_crref));

static_assert(is_same_v<mutobj_from_owner_lref_t, int&>);
static_assert(is_same_v<mutlref_from_owner_lref_t, int&>);
static_assert(is_same_v<mutrref_from_owner_lref_t, int&>);
static_assert(is_same_v<cobj_from_owner_lref_t, const int&>);
static_assert(is_same_v<clref_from_owner_lref_t, const int&>);
static_assert(is_same_v<crref_from_owner_lref_t, const int&>);

using owner_rref_t              = decltype(owner_rref);
using mutobj_from_owner_rref_t  = decltype(forward_like<owner_rref_t>(owner_rref.m_mutobj));
using mutlref_from_owner_rref_t = decltype(forward_like<owner_rref_t>(owner_rref.m_mutlref));
using mutrref_from_owner_rref_t = decltype(forward_like<owner_rref_t>(owner_rref.m_mutrref));
using cobj_from_owner_rref_t    = decltype(forward_like<owner_rref_t>(owner_rref.m_cobj));
using clref_from_owner_rref_t   = decltype(forward_like<owner_rref_t>(owner_rref.m_clref));
using crref_from_owner_rref_t   = decltype(forward_like<owner_rref_t>(owner_rref.m_crref));

static_assert(is_same_v<mutobj_from_owner_rref_t, int&&>);
static_assert(is_same_v<mutlref_from_owner_rref_t, int&&>);
static_assert(is_same_v<mutrref_from_owner_rref_t, int&&>);
static_assert(is_same_v<cobj_from_owner_rref_t, const int&&>);
static_assert(is_same_v<clref_from_owner_rref_t, const int&&>);
static_assert(is_same_v<crref_from_owner_rref_t, const int&&>);

using owner_c_t              = decltype(owner_c);
using mutobj_from_owner_c_t  = decltype(forward_like<owner_c_t>(owner_c.m_mutobj));
using mutlref_from_owner_c_t = decltype(forward_like<owner_c_t>(owner_c.m_mutlref));
using mutrref_from_owner_c_t = decltype(forward_like<owner_c_t>(owner_c.m_mutrref));
using cobj_from_owner_c_t    = decltype(forward_like<owner_c_t>(owner_c.m_cobj));
using clref_from_owner_c_t   = decltype(forward_like<owner_c_t>(owner_c.m_clref));
using crref_from_owner_c_t   = decltype(forward_like<owner_c_t>(owner_c.m_crref));

static_assert(is_same_v<mutobj_from_owner_c_t, const int&&>);
static_assert(is_same_v<mutlref_from_owner_c_t, const int&&>);
static_assert(is_same_v<mutrref_from_owner_c_t, const int&&>);
static_assert(is_same_v<cobj_from_owner_c_t, const int&&>);
static_assert(is_same_v<clref_from_owner_c_t, const int&&>);
static_assert(is_same_v<crref_from_owner_c_t, const int&&>);

using owner_clref_t              = decltype(owner_clref);
using mutobj_from_owner_clref_t  = decltype(forward_like<owner_clref_t>(owner_clref.m_mutobj));
using mutlref_from_owner_clref_t = decltype(forward_like<owner_clref_t>(owner_clref.m_mutlref));
using mutrref_from_owner_clref_t = decltype(forward_like<owner_clref_t>(owner_clref.m_mutrref));
using cobj_from_owner_clref_t    = decltype(forward_like<owner_clref_t>(owner_clref.m_cobj));
using clref_from_owner_clref_t   = decltype(forward_like<owner_clref_t>(owner_clref.m_clref));
using crref_from_owner_clref_t   = decltype(forward_like<owner_clref_t>(owner_clref.m_crref));

static_assert(is_same_v<mutobj_from_owner_clref_t, const int&>);
static_assert(is_same_v<mutlref_from_owner_clref_t, const int&>);
static_assert(is_same_v<mutrref_from_owner_clref_t, const int&>);
static_assert(is_same_v<cobj_from_owner_clref_t, const int&>);
static_assert(is_same_v<clref_from_owner_clref_t, const int&>);
static_assert(is_same_v<crref_from_owner_clref_t, const int&>);

using owner_crref_t              = decltype(owner_crref);
using mutobj_from_owner_crref_t  = decltype(forward_like<owner_crref_t>(owner_crref.m_mutobj));
using mutlref_from_owner_crref_t = decltype(forward_like<owner_crref_t>(owner_crref.m_mutlref));
using mutrref_from_owner_crref_t = decltype(forward_like<owner_crref_t>(owner_crref.m_mutrref));
using cobj_from_owner_crref_t    = decltype(forward_like<owner_crref_t>(owner_crref.m_cobj));
using clref_from_owner_crref_t   = decltype(forward_like<owner_crref_t>(owner_crref.m_clref));
using crref_from_owner_crref_t   = decltype(forward_like<owner_crref_t>(owner_crref.m_crref));

static_assert(is_same_v<mutobj_from_owner_crref_t, const int&&>);
static_assert(is_same_v<mutlref_from_owner_crref_t, const int&&>);
static_assert(is_same_v<mutrref_from_owner_crref_t, const int&&>);
static_assert(is_same_v<cobj_from_owner_crref_t, const int&&>);
static_assert(is_same_v<clref_from_owner_crref_t, const int&&>);
static_assert(is_same_v<crref_from_owner_crref_t, const int&&>);

int main() {} // COMPILE-ONLY
