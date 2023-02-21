// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <type_traits>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

STATIC_ASSERT(!is_aggregate_v<int>);
STATIC_ASSERT(!is_aggregate_v<const int>);
STATIC_ASSERT(!is_aggregate_v<void>);
STATIC_ASSERT(!is_aggregate_v<volatile void>);
STATIC_ASSERT(!is_aggregate_v<const volatile char>);
STATIC_ASSERT(!is_aggregate_v<bool>);

struct Empty {};
STATIC_ASSERT(is_aggregate_v<Empty>);

STATIC_ASSERT(is_base_of_v<false_type, is_aggregate<int>>);
STATIC_ASSERT(is_base_of_v<true_type, is_aggregate<Empty>>);

struct NonAggDefaultCtor {
    NonAggDefaultCtor() {}
};
STATIC_ASSERT(!is_aggregate_v<NonAggDefaultCtor>);

struct NonAggCustomCtor {
    NonAggCustomCtor(int, int);
};
STATIC_ASSERT(!is_aggregate_v<NonAggCustomCtor>);

struct NonAggCopyCtor {
    NonAggCopyCtor(const NonAggCopyCtor&);
};
STATIC_ASSERT(!is_aggregate_v<NonAggCopyCtor>);

struct NonAggMoveCtor {
    NonAggMoveCtor(NonAggMoveCtor&&);
};
STATIC_ASSERT(!is_aggregate_v<NonAggMoveCtor>);

struct AggCopyAssign {
    AggCopyAssign& operator=(const AggCopyAssign&);
};
STATIC_ASSERT(is_aggregate_v<AggCopyAssign>);

struct AggMoveAssign {
    AggMoveAssign& operator=(AggMoveAssign&&);
};
STATIC_ASSERT(is_aggregate_v<AggMoveAssign>);

struct AggDtor {
    ~AggDtor();
};
STATIC_ASSERT(is_aggregate_v<AggDtor>);

struct NonAggPrivateCtor {
private:
    NonAggPrivateCtor();
};
STATIC_ASSERT(!is_aggregate_v<NonAggPrivateCtor>);

struct NonAggProtectedCtor {
protected:
    NonAggProtectedCtor();
};
STATIC_ASSERT(!is_aggregate_v<NonAggProtectedCtor>);

struct PreCxx20AggDeletedDefaultCtor {
    PreCxx20AggDeletedDefaultCtor() = delete;
};
#if !_HAS_CXX20
STATIC_ASSERT(is_aggregate_v<PreCxx20AggDeletedDefaultCtor>);
#else
STATIC_ASSERT(!is_aggregate_v<PreCxx20AggDeletedDefaultCtor>);
#endif

struct PreCxx20AggDeletedCopyCtor {
    PreCxx20AggDeletedCopyCtor(const PreCxx20AggDeletedCopyCtor&) = delete;
};
#if !_HAS_CXX20
STATIC_ASSERT(is_aggregate_v<PreCxx20AggDeletedCopyCtor>);
#else
STATIC_ASSERT(!is_aggregate_v<PreCxx20AggDeletedCopyCtor>);
#endif

struct PreCxx20AggDefaultedDefaultCtor {
    PreCxx20AggDefaultedDefaultCtor() = default;
};
#if !_HAS_CXX20
STATIC_ASSERT(is_aggregate_v<PreCxx20AggDefaultedDefaultCtor>);
#else
STATIC_ASSERT(!is_aggregate_v<PreCxx20AggDefaultedDefaultCtor>);
#endif

struct NonAggExplicitDefaultCtor {
    explicit NonAggExplicitDefaultCtor() = default;
};
STATIC_ASSERT(!is_aggregate_v<NonAggExplicitDefaultCtor>);

struct PreCxx20AggDefaultedMoveCtor {
    PreCxx20AggDefaultedMoveCtor(PreCxx20AggDefaultedMoveCtor&&) = default;
};
#if !_HAS_CXX20
STATIC_ASSERT(is_aggregate_v<PreCxx20AggDefaultedMoveCtor>);
#else
STATIC_ASSERT(!is_aggregate_v<PreCxx20AggDefaultedMoveCtor>);
#endif

struct PreCxx20AggDeletedMoveCtor {
    PreCxx20AggDeletedMoveCtor(PreCxx20AggDeletedMoveCtor&&) = delete;
};
#if !_HAS_CXX20
STATIC_ASSERT(is_aggregate_v<PreCxx20AggDeletedMoveCtor>);
#else
STATIC_ASSERT(!is_aggregate_v<PreCxx20AggDeletedMoveCtor>);
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
STATIC_ASSERT(is_aggregate_v<ClassAgg>);

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
STATIC_ASSERT(!is_aggregate_v<ClassNonAggPrivate>);

class ClassNonAggProtected {
protected:
    int protected_mem;
};
STATIC_ASSERT(!is_aggregate_v<ClassNonAggProtected>);

union Union {
    int a;
    double b = 3.14;
    Empty c;
};
STATIC_ASSERT(is_aggregate_v<Union>);

enum Enumeration { One };
STATIC_ASSERT(!is_aggregate_v<Enumeration>);

enum class EnumClass { One };
STATIC_ASSERT(!is_aggregate_v<EnumClass>);

struct AggRef {
    const int& ref_mem;
};
STATIC_ASSERT(is_aggregate_v<AggRef>);

struct AggBitField {
    unsigned int a : 3;
    unsigned int : 5;
    char b : 8;
    int : 0;
};
STATIC_ASSERT(is_aggregate_v<AggBitField>);

struct AggAnonymousUnion {
    union {
        int c;
        double d;
    };
};
STATIC_ASSERT(is_aggregate_v<AggAnonymousUnion>);

struct NonAggVirtualFunc {
    int a;
    virtual void virt_func();
};
STATIC_ASSERT(!is_aggregate_v<NonAggVirtualFunc>);

struct NonAggPureVirtualFunc {
    virtual int pure_virt_f() = 0;
};
STATIC_ASSERT(!is_aggregate_v<NonAggPureVirtualFunc>);

struct Cxx14Agg {
    int x;
    Empty e;
    NonAggDefaultCtor n;
    Union u;
    Empty* ep;
    Enumeration em;
};
STATIC_ASSERT(is_aggregate_v<Cxx14Agg>);

struct Cxx17Agg : Cxx14Agg, ClassAgg {
    int x;
};
STATIC_ASSERT(is_aggregate_v<Cxx17Agg>);

struct Cxx17AggNonInheritCtor : NonAggDefaultCtor {};
STATIC_ASSERT(is_aggregate_v<Cxx17AggNonInheritCtor>);

struct NonAggInheritCtor : NonAggDefaultCtor {
    using NonAggDefaultCtor::NonAggDefaultCtor;
};
STATIC_ASSERT(!is_aggregate_v<NonAggInheritCtor>);

struct NonAggPrivateBase : private Cxx14Agg {};
STATIC_ASSERT(!is_aggregate_v<NonAggPrivateBase>);

struct NonAggProtectedBase : protected Cxx14Agg {};
STATIC_ASSERT(!is_aggregate_v<NonAggProtectedBase>);

struct NonAggVirtualBase : virtual Cxx14Agg {};
STATIC_ASSERT(!is_aggregate_v<NonAggVirtualBase>);

struct NonAggVirtualBaseFunc : NonAggVirtualFunc {};
STATIC_ASSERT(!is_aggregate_v<NonAggVirtualBaseFunc>);

struct NonAggVirtualBaseIndirect : NonAggVirtualBase {};
STATIC_ASSERT(!is_aggregate_v<NonAggVirtualBaseIndirect>);

struct Incomplete;

STATIC_ASSERT(is_aggregate_v<int[10]>);
STATIC_ASSERT(is_aggregate_v<int[]>);
STATIC_ASSERT(is_aggregate_v<Cxx14Agg[]>);
STATIC_ASSERT(is_aggregate_v<Cxx17Agg[]>);
STATIC_ASSERT(is_aggregate_v<NonAggDefaultCtor[]>);
STATIC_ASSERT(is_aggregate_v<int* [10]>);
STATIC_ASSERT(is_aggregate_v<NonAggDefaultCtor[2][3]>);
STATIC_ASSERT(is_aggregate_v<Empty[10]>);
STATIC_ASSERT(is_aggregate_v<Incomplete[]>);
STATIC_ASSERT(is_aggregate_v<Incomplete[10]>);

STATIC_ASSERT(!is_aggregate_v<int&>);
STATIC_ASSERT(!is_aggregate_v<const int*>);
STATIC_ASSERT(!is_aggregate_v<const Cxx14Agg&>);
STATIC_ASSERT(!is_aggregate_v<Cxx14Agg*>);
STATIC_ASSERT(!is_aggregate_v<Incomplete*>);

template <typename T>
struct AggTemplateClass {
    T value;
};
STATIC_ASSERT(is_aggregate_v<AggTemplateClass<int>>);

template <typename T>
struct NonAggTemplateClassCtor {
    NonAggTemplateClassCtor() : value{} {}
    T value;
};
STATIC_ASSERT(!is_aggregate_v<NonAggTemplateClassCtor<int>>);

using AliasedAgg = Cxx14Agg;
STATIC_ASSERT(is_aggregate_v<AliasedAgg>);

using AliasedAggCxx17 = Cxx17Agg;
STATIC_ASSERT(is_aggregate_v<AliasedAggCxx17>);

using AliasedNonAgg = NonAggDefaultCtor;
STATIC_ASSERT(!is_aggregate_v<AliasedNonAgg>);

template <typename T>
using AggAliasedTemplateClass = AggTemplateClass<T>;
STATIC_ASSERT(is_aggregate_v<AggAliasedTemplateClass<Cxx14Agg>>);

template <typename T>
using NonAggAliasedTemplateClass = NonAggTemplateClassCtor<T>;
STATIC_ASSERT(!is_aggregate_v<NonAggAliasedTemplateClass<Cxx14Agg>>);

int intObj;
STATIC_ASSERT(!is_aggregate_v<decltype(intObj)>);

Cxx17Agg cxx17AggObj;
STATIC_ASSERT(is_aggregate_v<decltype(cxx17AggObj)>);

Cxx14Agg cxx14AggObj;
STATIC_ASSERT(is_aggregate_v<decltype(cxx14AggObj)>);
