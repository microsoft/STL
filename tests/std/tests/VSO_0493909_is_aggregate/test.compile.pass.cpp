// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <type_traits>

using namespace std;

static_assert(!is_aggregate_v<int>);
static_assert(!is_aggregate_v<const int>);
static_assert(!is_aggregate_v<void>);
static_assert(!is_aggregate_v<volatile void>);
static_assert(!is_aggregate_v<const volatile char>);
static_assert(!is_aggregate_v<bool>);

struct Empty {};
static_assert(is_aggregate_v<Empty>);

static_assert(is_base_of_v<false_type, is_aggregate<int>>);
static_assert(is_base_of_v<true_type, is_aggregate<Empty>>);

struct NonAggDefaultCtor {
    NonAggDefaultCtor() {}
};
static_assert(!is_aggregate_v<NonAggDefaultCtor>);

struct NonAggCustomCtor {
    NonAggCustomCtor(int, int);
};
static_assert(!is_aggregate_v<NonAggCustomCtor>);

struct NonAggCopyCtor {
    NonAggCopyCtor(const NonAggCopyCtor&);
};
static_assert(!is_aggregate_v<NonAggCopyCtor>);

struct NonAggMoveCtor {
    NonAggMoveCtor(NonAggMoveCtor&&);
};
static_assert(!is_aggregate_v<NonAggMoveCtor>);

struct AggCopyAssign {
    AggCopyAssign& operator=(const AggCopyAssign&);
};
static_assert(is_aggregate_v<AggCopyAssign>);

struct AggMoveAssign {
    AggMoveAssign& operator=(AggMoveAssign&&);
};
static_assert(is_aggregate_v<AggMoveAssign>);

struct AggDtor {
    ~AggDtor();
};
static_assert(is_aggregate_v<AggDtor>);

struct NonAggPrivateCtor {
private:
    NonAggPrivateCtor();
};
static_assert(!is_aggregate_v<NonAggPrivateCtor>);

struct NonAggProtectedCtor {
protected:
    NonAggProtectedCtor();
};
static_assert(!is_aggregate_v<NonAggProtectedCtor>);

struct PreCxx20AggDeletedDefaultCtor {
    PreCxx20AggDeletedDefaultCtor() = delete;
};
#if !_HAS_CXX20
static_assert(is_aggregate_v<PreCxx20AggDeletedDefaultCtor>);
#else
static_assert(!is_aggregate_v<PreCxx20AggDeletedDefaultCtor>);
#endif

struct PreCxx20AggDeletedCopyCtor {
    PreCxx20AggDeletedCopyCtor(const PreCxx20AggDeletedCopyCtor&) = delete;
};
#if !_HAS_CXX20
static_assert(is_aggregate_v<PreCxx20AggDeletedCopyCtor>);
#else
static_assert(!is_aggregate_v<PreCxx20AggDeletedCopyCtor>);
#endif

struct PreCxx20AggDefaultedDefaultCtor {
    PreCxx20AggDefaultedDefaultCtor() = default;
};
#if !_HAS_CXX20
static_assert(is_aggregate_v<PreCxx20AggDefaultedDefaultCtor>);
#else
static_assert(!is_aggregate_v<PreCxx20AggDefaultedDefaultCtor>);
#endif

struct NonAggExplicitDefaultCtor {
    explicit NonAggExplicitDefaultCtor() = default;
};
static_assert(!is_aggregate_v<NonAggExplicitDefaultCtor>);

struct PreCxx20AggDefaultedMoveCtor {
    PreCxx20AggDefaultedMoveCtor(PreCxx20AggDefaultedMoveCtor&&) = default;
};
#if !_HAS_CXX20
static_assert(is_aggregate_v<PreCxx20AggDefaultedMoveCtor>);
#else
static_assert(!is_aggregate_v<PreCxx20AggDefaultedMoveCtor>);
#endif

struct PreCxx20AggDeletedMoveCtor {
    PreCxx20AggDeletedMoveCtor(PreCxx20AggDeletedMoveCtor&&) = delete;
};
#if !_HAS_CXX20
static_assert(is_aggregate_v<PreCxx20AggDeletedMoveCtor>);
#else
static_assert(!is_aggregate_v<PreCxx20AggDeletedMoveCtor>);
#endif

class ClassAgg {
public:
    int x = 0;
    NonAggDefaultCtor n;
    int& mem_func();

private:
    void private_mem_func();
    static const int private_static_mem = 0;
};
static_assert(is_aggregate_v<ClassAgg>);

class ClassNonAggPrivate {
private:
    int private_mem;

public:
    void set(int x) {
        private_mem = x;
    }
    int get() const {
        return private_mem;
    }
};
static_assert(!is_aggregate_v<ClassNonAggPrivate>);

class ClassNonAggProtected {
protected:
    int protected_mem;
};
static_assert(!is_aggregate_v<ClassNonAggProtected>);

union Union {
    int a;
    double b = 3.14;
    Empty c;
};
static_assert(is_aggregate_v<Union>);

enum Enumeration { One };
static_assert(!is_aggregate_v<Enumeration>);

enum class EnumClass { One };
static_assert(!is_aggregate_v<EnumClass>);

struct AggRef {
    const int& ref_mem;
};
static_assert(is_aggregate_v<AggRef>);

struct AggBitField {
    unsigned int a : 3;
    unsigned int : 5;
    char b : 8;
    int : 0;
};
static_assert(is_aggregate_v<AggBitField>);

struct AggAnonymousUnion {
    union {
        int c;
        double d;
    };
};
static_assert(is_aggregate_v<AggAnonymousUnion>);

struct NonAggVirtualFunc {
    int a;
    virtual void virt_func();
};
static_assert(!is_aggregate_v<NonAggVirtualFunc>);

struct NonAggPureVirtualFunc {
    virtual int pure_virt_f() = 0;
};
static_assert(!is_aggregate_v<NonAggPureVirtualFunc>);

struct Cxx14Agg {
    int x;
    Empty e;
    NonAggDefaultCtor n;
    Union u;
    Empty* ep;
    Enumeration em;
};
static_assert(is_aggregate_v<Cxx14Agg>);

struct Cxx17Agg : Cxx14Agg, ClassAgg {
    int x;
};
static_assert(is_aggregate_v<Cxx17Agg>);

struct Cxx17AggNonInheritCtor : NonAggDefaultCtor {};
static_assert(is_aggregate_v<Cxx17AggNonInheritCtor>);

struct NonAggInheritCtor : NonAggDefaultCtor {
    using NonAggDefaultCtor::NonAggDefaultCtor;
};
static_assert(!is_aggregate_v<NonAggInheritCtor>);

struct NonAggPrivateBase : private Cxx14Agg {};
static_assert(!is_aggregate_v<NonAggPrivateBase>);

struct NonAggProtectedBase : protected Cxx14Agg {};
static_assert(!is_aggregate_v<NonAggProtectedBase>);

struct NonAggVirtualBase : virtual Cxx14Agg {};
static_assert(!is_aggregate_v<NonAggVirtualBase>);

struct NonAggVirtualBaseFunc : NonAggVirtualFunc {};
static_assert(!is_aggregate_v<NonAggVirtualBaseFunc>);

struct NonAggVirtualBaseIndirect : NonAggVirtualBase {};
static_assert(!is_aggregate_v<NonAggVirtualBaseIndirect>);

struct Incomplete;

static_assert(is_aggregate_v<int[10]>);
static_assert(is_aggregate_v<int[]>);
static_assert(is_aggregate_v<Cxx14Agg[]>);
static_assert(is_aggregate_v<Cxx17Agg[]>);
static_assert(is_aggregate_v<NonAggDefaultCtor[]>);
static_assert(is_aggregate_v<int* [10]>);
static_assert(is_aggregate_v<NonAggDefaultCtor[2][3]>);
static_assert(is_aggregate_v<Empty[10]>);
static_assert(is_aggregate_v<Incomplete[]>);
static_assert(is_aggregate_v<Incomplete[10]>);

static_assert(!is_aggregate_v<int&>);
static_assert(!is_aggregate_v<const int*>);
static_assert(!is_aggregate_v<const Cxx14Agg&>);
static_assert(!is_aggregate_v<Cxx14Agg*>);
static_assert(!is_aggregate_v<Incomplete*>);

template <typename T>
struct AggTemplateClass {
    T value;
};
static_assert(is_aggregate_v<AggTemplateClass<int>>);

template <typename T>
struct NonAggTemplateClassCtor {
    NonAggTemplateClassCtor() : value{} {}
    T value;
};
static_assert(!is_aggregate_v<NonAggTemplateClassCtor<int>>);

using AliasedAgg = Cxx14Agg;
static_assert(is_aggregate_v<AliasedAgg>);

using AliasedAggCxx17 = Cxx17Agg;
static_assert(is_aggregate_v<AliasedAggCxx17>);

using AliasedNonAgg = NonAggDefaultCtor;
static_assert(!is_aggregate_v<AliasedNonAgg>);

template <typename T>
using AggAliasedTemplateClass = AggTemplateClass<T>;
static_assert(is_aggregate_v<AggAliasedTemplateClass<Cxx14Agg>>);

template <typename T>
using NonAggAliasedTemplateClass = NonAggTemplateClassCtor<T>;
static_assert(!is_aggregate_v<NonAggAliasedTemplateClass<Cxx14Agg>>);

int intObj;
static_assert(!is_aggregate_v<decltype(intObj)>);

Cxx17Agg cxx17AggObj;
static_assert(is_aggregate_v<decltype(cxx17AggObj)>);

Cxx14Agg cxx14AggObj;
static_assert(is_aggregate_v<decltype(cxx14AggObj)>);
