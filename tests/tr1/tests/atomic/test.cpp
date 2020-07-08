// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <atomic> C++11 header
#define TEST_NAME "<atomic>"

#define _SILENCE_CXX20_ATOMIC_INIT_DEPRECATION_WARNING

#include "tdefs.h"
#include <atomic>
#include <limits>
#include <memory>
#include <type_traits>

// PROPERTIES OF ATOMIC TYPES
template <class Atom>
struct remove_pointer;

template <class Ty>
struct remove_pointer<Ty*> { // struct to define pointer type that has same increment as Ty *
    typedef Ty base_type;
};

int get_lock_free_for_type(char*) { // return lock-free flag for type char
    return ATOMIC_CHAR_LOCK_FREE;
}

int get_lock_free_for_type(signed char*) { // return lock-free flag for type signed char
    return ATOMIC_CHAR_LOCK_FREE;
}

int get_lock_free_for_type(unsigned char*) { // return lock-free flag for type unsigned char
    return ATOMIC_CHAR_LOCK_FREE;
}

int get_lock_free_for_type(char16_t*) { // return lock-free flag for type char16_t
    return ATOMIC_CHAR16_T_LOCK_FREE;
}

int get_lock_free_for_type(char32_t*) { // return lock-free flag for type char32_t
    return ATOMIC_CHAR32_T_LOCK_FREE;
}

int get_lock_free_for_type(wchar_t*) { // return lock-free flag for type wchar_t
    return ATOMIC_WCHAR_T_LOCK_FREE;
}

int get_lock_free_for_type(short*) { // return lock-free flag for type short
    return ATOMIC_SHORT_LOCK_FREE;
}

int get_lock_free_for_type(unsigned short*) { // return lock-free flag for type unsigned short
    return ATOMIC_SHORT_LOCK_FREE;
}

int get_lock_free_for_type(int*) { // return lock-free flag for type int
    return ATOMIC_INT_LOCK_FREE;
}

int get_lock_free_for_type(unsigned int*) { // return lock-free flag for type unsigned int
    return ATOMIC_INT_LOCK_FREE;
}

int get_lock_free_for_type(long*) { // return lock-free flag for type long
    return ATOMIC_LONG_LOCK_FREE;
}

int get_lock_free_for_type(unsigned long*) { // return lock-free flag for type unsigned long
    return ATOMIC_LONG_LOCK_FREE;
}

int get_lock_free_for_type(long long*) { // return lock-free flag for type long long
    return ATOMIC_LLONG_LOCK_FREE;
}

int get_lock_free_for_type(unsigned long long*) { // return lock-free flag for type unsigned long long
    return ATOMIC_LLONG_LOCK_FREE;
}

template <class Ty>
int get_lock_free_for_type(Ty*) { // return lock-free flag for type Ty
    return 1;
}

// TEMPLATE CLASS n_bytes
template <CSTD size_t sz>
struct n_bytes { // struct for testing atomic<UDT>
    unsigned char data[sz];
};

template <CSTD size_t sz>
bool operator==(const n_bytes<sz>& left, const n_bytes<sz>& right) { // compare for equality
    return memcmp(&left.data, &right.data, sz) == 0;
}

// INITIALIZATION
template <class Mtype, class Atom, bool use_atomic_init>
struct initializer_helper;

template <class Mtype, class Atom>
struct initializer_helper<Mtype, Atom, true> { // initialize C-style types
    static void init(Atom& atom, Mtype value) {
        STD atomic_init(&atom, value);
    }
};

template <class Mtype, class Atom>
struct initializer_helper<Mtype, Atom, false> { // initialize C++-style types
    static void init(Atom& atom, Mtype value) {
        Atom temp(value);
        memcpy((void*) &atom, (const void*) &temp, sizeof(atom));
    }
};

template <class Mtype, class Atom>
struct initializer : initializer_helper<Mtype, Atom,
                         STD is_integral<Mtype>::value> { // atomics managing integral types are always C-style types
};

template <class Ty>
struct initializer<Ty*, STD atomic<Ty*>>
    : initializer_helper<Ty*, STD atomic<Ty*>, true> { // atomic<Ty *> is a C-style type
};

template <class Ty>
struct initializer<Ty*, volatile STD atomic<Ty*>>
    : initializer_helper<Ty*, volatile STD atomic<Ty*>, true> { // volatile atomic<Ty *> is a C-style type
};

// ENUM memory_order
static bool check_memory_order(STD memory_order) { // used to test that memory_order is defined
    return true;
}

static void test_memory_order_enum() { // test that all required values exist and are distinct
    CHECK_INT(check_memory_order(STD memory_order_relaxed), true);
    CHECK_INT(check_memory_order(STD memory_order_consume), true);
    CHECK_INT(check_memory_order(STD memory_order_acquire), true);
    CHECK_INT(check_memory_order(STD memory_order_release), true);
    CHECK_INT(check_memory_order(STD memory_order_acq_rel), true);
    CHECK_INT(check_memory_order(STD memory_order_seq_cst), true);
    CHECK_INT(STD memory_order_relaxed != STD memory_order_consume, true);
    CHECK_INT(STD memory_order_relaxed != STD memory_order_acquire, true);
    CHECK_INT(STD memory_order_relaxed != STD memory_order_release, true);
    CHECK_INT(STD memory_order_relaxed != STD memory_order_acq_rel, true);
    CHECK_INT(STD memory_order_relaxed != STD memory_order_seq_cst, true);
    CHECK_INT(STD memory_order_consume != STD memory_order_acquire, true);
    CHECK_INT(STD memory_order_consume != STD memory_order_release, true);
    CHECK_INT(STD memory_order_consume != STD memory_order_acq_rel, true);
    CHECK_INT(STD memory_order_consume != STD memory_order_seq_cst, true);
    CHECK_INT(STD memory_order_acquire != STD memory_order_release, true);
    CHECK_INT(STD memory_order_acquire != STD memory_order_acq_rel, true);
    CHECK_INT(STD memory_order_acquire != STD memory_order_seq_cst, true);
    CHECK_INT(STD memory_order_release != STD memory_order_acq_rel, true);
    CHECK_INT(STD memory_order_release != STD memory_order_seq_cst, true);
    CHECK_INT(STD memory_order_acq_rel != STD memory_order_seq_cst, true);
}

static void test_kill_dependency() { // test kill_dependency template function
    CHECK_TYPE(int, STD kill_dependency(1));
    CHECK_INT(1, STD kill_dependency(1));
    CHECK_INT(-1, STD kill_dependency(-1));
    CHECK_TYPE(double, STD kill_dependency(1.0));
    CHECK_INT(1.0, STD kill_dependency(1.0));
    CHECK_INT(-1.0, STD kill_dependency(-1.0));
}

// LOCK-FREE PROPERTY
#define CHECK_LOCK_FREE(value)                    \
    if (!terse)                                   \
        printf("%s reports %d\n", #value, value); \
    CHECK_INT(value == 2, 1)

static void test_lock_free_property() { // test that the lock-free property macros have appropriate values
    CHECK_LOCK_FREE(ATOMIC_BOOL_LOCK_FREE);
    CHECK_LOCK_FREE(ATOMIC_CHAR_LOCK_FREE);
    CHECK_LOCK_FREE(ATOMIC_CHAR16_T_LOCK_FREE);
    CHECK_LOCK_FREE(ATOMIC_CHAR32_T_LOCK_FREE);
    CHECK_LOCK_FREE(ATOMIC_WCHAR_T_LOCK_FREE);
    CHECK_LOCK_FREE(ATOMIC_SHORT_LOCK_FREE);
    CHECK_LOCK_FREE(ATOMIC_INT_LOCK_FREE);
    CHECK_LOCK_FREE(ATOMIC_LONG_LOCK_FREE);
    CHECK_LOCK_FREE(ATOMIC_LLONG_LOCK_FREE);
    CHECK_LOCK_FREE(ATOMIC_POINTER_LOCK_FREE);
}

// TEST SEMANTICS OF atomic_flag
static void test_atomic_flag() { // test properties of atomic_flag
    STD atomic_flag flag = ATOMIC_FLAG_INIT;
    bool value           = flag.test_and_set();
    CHECK_INT(value, false);
    value = flag.test_and_set(STD memory_order_acq_rel);
    CHECK_INT(value, true);
    flag.clear();
    value = flag.test_and_set();
    CHECK_INT(value, false);
    flag.clear(STD memory_order_release);
    value = flag.test_and_set();
    CHECK_INT(value, false);

    volatile STD atomic_flag vflag = ATOMIC_FLAG_INIT;
    value                          = vflag.test_and_set();
    CHECK_INT(value, false);
    value = vflag.test_and_set(STD memory_order_release);
    CHECK_INT(value, true);
    vflag.clear();
    value = vflag.test_and_set();
    CHECK_INT(value, false);
    flag.clear(STD memory_order_relaxed);
    value = flag.test_and_set();
    CHECK_INT(value, false);

    value = STD atomic_flag_test_and_set(&flag);
    CHECK_INT(value, true);
    STD atomic_flag_clear(&flag);
    value = STD atomic_flag_test_and_set_explicit(&flag, STD memory_order_relaxed);
    CHECK_INT(value, false);
    value = STD atomic_flag_test_and_set_explicit(&flag, STD memory_order_release);
    CHECK_INT(value, true);
    STD atomic_flag_clear_explicit(&flag, STD memory_order_release);
    value = STD atomic_flag_test_and_set(&flag);
    CHECK_INT(value, false);
}

// TEST SEMANTICS OF ATOMIC TYPES
template <class Mtype>
struct values {
    static Mtype zero() {
        return 0;
    }
    static Mtype not_zero() {
        return STD numeric_limits<Mtype>::max();
    }
};

template <class Ty>
struct values<Ty*> {
    static Ty* zero() {
        return nullptr;
    }
    static Ty* not_zero() {
        return (Ty*) (-1);
    }
};

template <CSTD size_t sz>
struct values<n_bytes<sz>> {
    static n_bytes<sz> zero() {
        n_bytes<sz> tmp = {{0}};
        return tmp;
    }
    static n_bytes<sz> not_zero() {
        n_bytes<sz> tmp = {{static_cast<unsigned char>(-1)}};
        return tmp;
    }
};

template <class Mtype, class Atom>
void test_atomic_aggregate_initialization() { // test aggregate initialization
    Atom atom = ATOMIC_VAR_INIT(0);
    CHECK_INT(STD atomic_load(&atom) == values<Mtype>::zero(), true);
}

template <class Mtype, class Atom>
void test_atomic_ctor_initialization() { // test construction from object of managed type
                                         // note: constructor initialization is not enabled for
                                         // atomic_integral types if the compiler
                                         // does not support =delete and =default
    Mtype object{};
    Atom atom(object);
    CHECK_INT(atom.load() == object, true);
}

template <class Mtype, class Atom>
void test_atomic_core_nonmembers() { // test core non-member functions for all atomic types
    Atom atom{};
    const Atom& c_atom = atom;
    const Mtype zero   = values<Mtype>::zero();
    const Mtype other  = values<Mtype>::not_zero();

    // initialization
    STD atomic_init(&atom, other);
    CHECK_INT(STD atomic_load(&atom) == other, true);
    STD atomic_init(&atom, zero);
    CHECK_INT(STD atomic_load(&atom) == zero, true);

    // atomic_is_lock_free
    switch (get_lock_free_for_type((Mtype*) nullptr)) { // check that atomic_is_lock_free gives result consistent
                                                        // with the corresponding macro
    case 0:
        CHECK_INT(STD atomic_is_lock_free(&c_atom), false);
        break;
    case 1:
        CHECK_INT(STD atomic_is_lock_free(&c_atom) == false || STD atomic_is_lock_free(&c_atom) == true, true);
        break;
    case 2:
        CHECK_INT(STD atomic_is_lock_free(&c_atom), true);
        break;
    }

    // atomic_store/atomic_store_explicit
    STD atomic_store(&atom, other);
    CHECK_INT(STD atomic_load(&atom) == other, true);
    STD atomic_store_explicit(&atom, zero, STD memory_order_relaxed);

    // atomic_load/atomic_load_explicit
    CHECK_INT(STD atomic_load(&c_atom) == zero, true);
    STD atomic_store(&atom, other);
    CHECK_INT(STD atomic_load(&c_atom) == other, true);
    CHECK_INT(STD atomic_load_explicit(&c_atom, STD memory_order_relaxed) == other, true);
    STD atomic_store(&atom, zero);
    CHECK_INT(STD atomic_load_explicit(&c_atom, STD memory_order_seq_cst) == zero, true);

    // atomic_exchange/atomic_exchange_explicit
    CHECK_INT(STD atomic_exchange(&atom, other) == zero, true);
    CHECK_INT(STD atomic_exchange(&atom, zero) == other, true);
    CHECK_INT(STD atomic_exchange_explicit(&atom, other, STD memory_order_relaxed) == zero, true);
    CHECK_INT(STD atomic_exchange_explicit(&atom, zero, STD memory_order_seq_cst) == other, true);

    // atomic_compare_exchange_weak/atomic_compare_exchange_weak_explicit
    Mtype value = zero;
    STD atomic_store(&atom, zero);
    CHECK_INT(STD atomic_compare_exchange_weak(&atom, &value, other) == true, true);
    CHECK_INT(value == zero, true);
    CHECK_INT(STD atomic_load(&atom) == other, true);
    CHECK_INT(STD atomic_compare_exchange_weak(&atom, &value, zero), false);
    CHECK_INT(value == other, true);
    CHECK_INT(STD atomic_load(&atom) == other, true);

    value = zero;
    STD atomic_store(&atom, zero);
    CHECK_INT(STD atomic_compare_exchange_weak_explicit(
                  &atom, &value, other, STD memory_order_acq_rel, STD memory_order_relaxed),
        true);
    CHECK_INT(value == zero, true);
    CHECK_INT(STD atomic_load(&atom) == other, true);
    CHECK_INT(STD atomic_compare_exchange_weak_explicit(
                  &atom, &value, zero, STD memory_order_seq_cst, STD memory_order_relaxed),
        false);
    CHECK_INT(value == other, true);
    CHECK_INT(STD atomic_load(&atom) == other, true);

    // atomic_compare_exchange_strong/atomic_compare_exchange_strong_explicit
    value = zero;
    STD atomic_store(&atom, zero);
    CHECK_INT(STD atomic_compare_exchange_strong(&atom, &value, other), true);
    CHECK_INT(value == zero, true);
    CHECK_INT(STD atomic_load(&atom) == other, true);
    CHECK_INT(STD atomic_compare_exchange_strong(&atom, &value, zero), false);
    CHECK_INT(value == other, true);
    CHECK_INT(STD atomic_load(&atom) == other, true);

    value = zero;
    STD atomic_store(&atom, zero);
    CHECK_INT(STD atomic_compare_exchange_strong_explicit(
                  &atom, &value, other, STD memory_order_acq_rel, STD memory_order_relaxed),
        true);
    CHECK_INT(value == zero, true);
    CHECK_INT(STD atomic_load(&atom) == other, true);
    CHECK_INT(STD atomic_compare_exchange_strong_explicit(
                  &atom, &value, zero, STD memory_order_seq_cst, STD memory_order_relaxed),
        false);
    CHECK_INT(value == other, true);
    CHECK_INT(STD atomic_load(&atom) == other, true);
}

template <class Mtype, class Atom>
void test_atomic_core_members() { // test core member functions for all atomic types
    Atom atom;
    const Mtype zero = values<Mtype>::zero();
    const Mtype one  = values<Mtype>::not_zero();
    initializer<Mtype, Atom>::init(atom, zero);

    const Atom& c_atom = static_cast<const Atom&>(atom);
    const Mtype other  = one;

    // is_lock_free
    switch (get_lock_free_for_type((Mtype*) nullptr)) { // check that is_lock_free gives result consistent
                                                        // with the corresponding macro
    case 0:
        CHECK_INT(c_atom.is_lock_free(), false);
        break;
    case 1:
        CHECK_INT(c_atom.is_lock_free() == false || c_atom.is_lock_free() == true, true);
        break;
    case 2:
        CHECK_INT(c_atom.is_lock_free(), true);
        break;
    }

    // load/store/operator Ty
    CHECK_INT(c_atom.load() == zero, true);
    CHECK_INT(c_atom.load(STD memory_order_relaxed) == zero, true);
    CHECK_INT(static_cast<Mtype>(c_atom) == zero, true);

    atom.store(other);
    CHECK_INT(static_cast<Mtype>(c_atom) == other, true);
    atom.store(zero, STD memory_order_seq_cst);
    CHECK_INT(c_atom.load() == zero, true);

    // assign from Ty
    atom = other;
    CHECK_INT(c_atom.load() == other, true);

    // exchange
    CHECK_INT(atom.exchange(zero) == other, true);
    CHECK_INT(c_atom.load() == zero, true);

    // compare_exchange_weak
    Mtype value = zero;
    atom        = value;
    CHECK_INT(atom.compare_exchange_weak(value, other), true);
    CHECK_INT(value == zero, true);
    CHECK_INT(atom.load() == other, true);
    CHECK_INT(atom.compare_exchange_weak(value, zero), false);
    CHECK_INT(value == other, true);
    CHECK_INT(atom.load() == other, true);

    value = zero;
    atom  = zero;
    CHECK_INT(atom.compare_exchange_weak(value, other, STD memory_order_seq_cst), true);
    CHECK_INT(value == zero, true);
    CHECK_INT(atom.load() == other, true);
    CHECK_INT(atom.compare_exchange_weak(value, zero, STD memory_order_relaxed, STD memory_order_relaxed), false);
    CHECK_INT(value == other, true);
    CHECK_INT(atom.load() == other, true);

    // compare_exchange_strong
    value = zero;
    atom  = zero;
    CHECK_INT(atom.compare_exchange_strong(value, other), true);
    CHECK_INT(value == zero, true);
    CHECK_INT(atom.load() == other, true);
    CHECK_INT(atom.compare_exchange_strong(value, zero), false);
    CHECK_INT(value == other, true);
    CHECK_INT(atom.load() == other, true);

    value = zero;
    atom  = zero;
    CHECK_INT(atom.compare_exchange_strong(value, other, STD memory_order_seq_cst), true);
    CHECK_INT(value == zero, true);
    CHECK_INT(atom.load() == other, true);
    CHECK_INT(atom.compare_exchange_strong(value, zero, STD memory_order_relaxed, STD memory_order_relaxed), false);
    CHECK_INT(value == other, true);
    CHECK_INT(atom.load() == other, true);
}

template <class Mtype, class Atom>
void test_atomic_arithmetic_nonmembers() { // test arithmetic non-member functions for all supported types
    Atom atom;
    initializer<Mtype, Atom>::init(atom, 0);

    CHECK_INT(STD atomic_fetch_add(&atom, 1), 0);
    CHECK_INT(STD atomic_load(&atom), 1);
    CHECK_INT(STD atomic_fetch_add_explicit(&atom, 1, STD memory_order_relaxed), 1);
    CHECK_INT(STD atomic_load(&atom), 2);

    CHECK_INT(STD atomic_fetch_sub(&atom, 1), 2);
    CHECK_INT(STD atomic_load(&atom), 1);
    CHECK_INT(STD atomic_fetch_sub_explicit(&atom, 1, STD memory_order_relaxed), 1);
    CHECK_INT(STD atomic_load(&atom), 0);

    STD atomic_store(&atom, 0x07);
    CHECK_INT(STD atomic_fetch_and(&atom, 0x0E), 0x07);
    CHECK_INT(STD atomic_load(&atom), 0x06);
    CHECK_INT(STD atomic_fetch_and_explicit(&atom, 0x04, STD memory_order_relaxed), 0x06);
    CHECK_INT(STD atomic_load(&atom), 0x04);

    CHECK_INT(STD atomic_fetch_or(&atom, 0x06), 0x04);
    CHECK_INT(STD atomic_load(&atom), 0x06);
    CHECK_INT(STD atomic_fetch_or_explicit(&atom, 0x03, STD memory_order_relaxed), 0x06);
    CHECK_INT(STD atomic_load(&atom), 0x07);

    CHECK_INT(STD atomic_fetch_xor(&atom, 0x0D), 0x07);
    CHECK_INT(STD atomic_load(&atom), 0x0A);
    CHECK_INT(STD atomic_fetch_xor_explicit(&atom, 0x08, STD memory_order_relaxed), 0x0A);
    CHECK_INT(STD atomic_load(&atom), 0x02);
}

template <class Mtype, class Atom>
void test_atomic_arithmetic_members() { // test arithmetic member functions for all supported types
    Atom atom;
    initializer<Mtype, Atom>::init(atom, 0);

    CHECK_INT(atom++, 0);
    CHECK_INT(atom.load(), 1);
    CHECK_INT(++atom, 2);
    CHECK_INT(atom.load(), 2);

    CHECK_INT(atom--, 2);
    CHECK_INT(atom.load(), 1);
    CHECK_INT(--atom, 0);
    CHECK_INT(atom.load(), 0);

    atom = 0x07;
    CHECK_INT(atom &= 0x0E, 0x06);
    CHECK_INT(atom.load(), 0x06);
    CHECK_INT(atom &= 0x04, 0x04);
    CHECK_INT(atom.load(), 0x04);

    CHECK_INT(atom |= 0x06, 0x06);
    CHECK_INT(atom.load(), 0x06);
    CHECK_INT(atom |= 0x03, 0x07);
    CHECK_INT(atom.load(), 0x07);

    CHECK_INT(atom ^= 0x0D, 0x0A);
    CHECK_INT(atom.load(), 0x0A);
    CHECK_INT(atom ^= 0x08, 0x02);
    CHECK_INT(atom.load(), 0x02);

    atom = 0;
    CHECK_INT(atom.fetch_add(1), 0);
    CHECK_INT(atom.load(), 1);
    CHECK_INT(atom.fetch_add(1, STD memory_order_relaxed), 1);
    CHECK_INT(atom.load(), 2);

    CHECK_INT(atom.fetch_sub(1), 2);
    CHECK_INT(atom.load(), 1);
    CHECK_INT(atom.fetch_sub(1, STD memory_order_relaxed), 1);
    CHECK_INT(atom.load(), 0);

    atom = 0x07;
    CHECK_INT(atom.fetch_and(0x0E), 0x07);
    CHECK_INT(atom.load(), 0x06);
    CHECK_INT(atom.fetch_and(0x04, STD memory_order_relaxed), 0x06);
    CHECK_INT(atom.load(), 0x04);

    CHECK_INT(atom.fetch_or(0x06), 0x04);
    CHECK_INT(atom.load(), 0x06);
    CHECK_INT(atom.fetch_or(0x03, STD memory_order_relaxed), 0x06);
    CHECK_INT(atom.load(), 0x07);

    CHECK_INT(atom.fetch_xor(0x0D), 0x07);
    CHECK_INT(atom.load(), 0x0A);
    CHECK_INT(atom.fetch_xor(0x08, STD memory_order_relaxed), 0x0A);
    CHECK_INT(atom.load(), 0x02);
}

template <class Mtype, class Atom>
void test_atomic_address_arithmetic_nonmembers() { // test arithmetic non-member functions for address types
    typename remove_pointer<Mtype>::base_type data[3];
    Atom atom;
    STD atomic_init(&atom, data);
    CHECK_PTR(STD atomic_load(&atom), data);

    CHECK_PTR(STD atomic_fetch_add(&atom, 1), data);
    CHECK_PTR(STD atomic_load(&atom), data + 1);
    CHECK_PTR(STD atomic_fetch_add_explicit(&atom, 1, STD memory_order_relaxed), data + 1);
    CHECK_PTR(STD atomic_load(&atom), data + 2);

    CHECK_PTR(STD atomic_fetch_sub(&atom, 1), data + 2);
    CHECK_PTR(STD atomic_load(&atom), data + 1);
    CHECK_PTR(STD atomic_fetch_sub_explicit(&atom, 1, STD memory_order_relaxed), data + 1);
    CHECK_PTR(STD atomic_load(&atom), data);
}

template <class Mtype, class Atom>
void test_atomic_address_arithmetic_members() { // test arithmetic non-member functions for address types
    typename remove_pointer<Mtype>::base_type data[2] = {0};
    Atom atom;
    STD atomic_init(&atom, data);
    CHECK_PTR(STD atomic_load(&atom), data);

    // fetch_add/fetch_sub
    atom = &data[0];
    CHECK_PTR(atom.fetch_add(1), data);
    CHECK_PTR(atom.load(), data + 1);
    CHECK_PTR(atom.fetch_add(1, STD memory_order_relaxed), data + 1);
    CHECK_PTR(atom.load(), data + 2);

    CHECK_PTR(atom.fetch_sub(1), data + 2);
    CHECK_PTR(atom.load(), data + 1);
    CHECK_PTR(atom.fetch_sub(1, STD memory_order_relaxed), data + 1);
    CHECK_PTR(atom.load(), data);

    // operator+=/operator-=
    atom = data;
    CHECK_PTR(atom += 1, data + 1);
    CHECK_PTR(atom.load(), data + 1);
    CHECK_PTR(atom += 1, data + 2);
    CHECK_PTR(atom.load(), data + 2);

    CHECK_PTR(atom -= 1, data + 1);
    CHECK_PTR(atom.load(), data + 1);
    CHECK_PTR(atom -= 1, data);
    CHECK_PTR(atom.load(), data);
}

template <class Mtype, class Atom>
void test_atomic_address_template_incr_decr() { // test increment and decrement for all address types
    typename remove_pointer<Mtype>::base_type data[2] = {0};
    Atom atom;
    STD atomic_init(&atom, data);

    CHECK_PTR(atom++, data);
    CHECK_PTR(atom.load(), data + 1);
    CHECK_PTR(++atom, data + 2);
    CHECK_PTR(atom.load(), data + 2);

    CHECK_PTR(atom--, data + 2);
    CHECK_PTR(atom.load(), data + 1);
    CHECK_PTR(--atom, data);
    CHECK_PTR(atom.load(), data);
}

// TEST SEMANTICS OF atomic_bool AND atomic<bool>
template <class Mtype, class Atom>
void do_test_atomic_bool() { // test core operations for atomic bool
    test_atomic_core_nonmembers<Mtype, Atom>();
    test_atomic_core_members<Mtype, Atom>();
}

template <class Mtype, class Atom>
void do_test_atomic_bool_aggregate() { // test atomic_bool
    test_atomic_aggregate_initialization<Mtype, Atom>();
    do_test_atomic_bool<Mtype, Atom>();
}

template <class Mtype, class Atom>
void do_test_atomic_bool_template() { // test atomic<bool>
    test_atomic_ctor_initialization<Mtype, Atom>();
    do_test_atomic_bool<Mtype, Atom>();
}

static void test_atomic_bool() { // test operations on atomic bool
    typedef bool Mtype;
    typedef STD atomic_bool Atom;
    typedef STD atomic<Mtype> AtomTemplate;
    do_test_atomic_bool_aggregate<Mtype, Atom>();
    do_test_atomic_bool_aggregate<Mtype, volatile Atom>();
    do_test_atomic_bool_template<Mtype, AtomTemplate>();
    do_test_atomic_bool_template<Mtype, volatile AtomTemplate>();
}

// TEST SEMANTICS OF atomic_integral AND atomic<integral>
template <class Mtype, class Atom>
void do_test_atomic_integral() { // test core and arithmetic operations for atomic integral types
    test_atomic_core_nonmembers<Mtype, Atom>();
    test_atomic_core_members<Mtype, Atom>();
    test_atomic_arithmetic_nonmembers<Mtype, Atom>();
    test_atomic_arithmetic_members<Mtype, Atom>();
}

template <class Mtype, class Atom>
void do_test_atomic_integral_aggregate() { // test atomic_integral types
    test_atomic_aggregate_initialization<Mtype, Atom>();
    do_test_atomic_integral<Mtype, Atom>();
}

template <class Mtype, class Atom>
void do_test_atomic_integral_template() { // test atomic<integral> types
    test_atomic_ctor_initialization<Mtype, Atom>();
    do_test_atomic_integral<Mtype, Atom>();
}

template <class Mtype, class Atom>
void test_atomic_integral() { // test operations on atomic integral types
    typedef STD atomic<Mtype> AtomTemplate;
    do_test_atomic_integral_aggregate<Mtype, Atom>();
    do_test_atomic_integral_aggregate<Mtype, volatile Atom>();
    do_test_atomic_integral_template<Mtype, AtomTemplate>();
    do_test_atomic_integral_template<Mtype, volatile AtomTemplate>();
}

// TEST SEMANTICS OF atomic<Ty*>
template <class Mtype, class Atom>
void do_test_atomic_address_nonmembers() { // test core and arithmetic non-member operations
                                           // on atomic address types
    test_atomic_core_nonmembers<Mtype, Atom>();
    test_atomic_address_arithmetic_nonmembers<Mtype, Atom>();
}

template <class Mtype, class Atom>
void do_test_atomic_address_members() { // test core and arithmetic member operations
                                        // on atomic address types
    test_atomic_core_members<Mtype, Atom>();
    test_atomic_address_arithmetic_members<Mtype, Atom>();
}

template <class Mtype, class Atom>
void do_test_atomic_address_template() { // test atomic<Ty*>
                                         // note: non-member address operations act on void*, not Ty*
    test_atomic_ctor_initialization<Mtype, Atom>();
    do_test_atomic_address_members<Mtype, Atom>();
    do_test_atomic_address_nonmembers<Mtype, Atom>();
    test_atomic_address_template_incr_decr<Mtype, Atom>();
}

static void test_atomic_address() { // test atomic<void*>, atomic<short*>, atomic<long*>
    test_atomic_core_members<void*, STD atomic<void*>>();
    test_atomic_core_nonmembers<void*, STD atomic<void*>>();
    test_atomic_core_members<void*, volatile STD atomic<void*>>();
    test_atomic_core_nonmembers<void*, volatile STD atomic<void*>>();

    do_test_atomic_address_template<short*, STD atomic<short*>>();
    do_test_atomic_address_template<short*, volatile STD atomic<short*>>();

    do_test_atomic_address_template<long*, STD atomic<long*>>();
    do_test_atomic_address_template<long*, volatile STD atomic<long*>>();
}

// TEST SEMANTICS OF atomic<UDT>
template <class Mtype, class Atom>
void do_test_atomic_udt() { // test core operations on atomic<UDT>
    test_atomic_ctor_initialization<Mtype, Atom>();
    test_atomic_core_members<Mtype, Atom>();
}

template <CSTD size_t sz>
static void test_atomic_n_bytes() { // test core operations on atomic<n_bytes<sz>>
    typedef n_bytes<sz> Mtype;
    typedef STD atomic<Mtype> Atom;
    do_test_atomic_udt<Mtype, Atom>();
}

static void test_atomic_udt() { // test atomic<n_bytes<sz>> for various sizes
    test_atomic_n_bytes<1>();
    test_atomic_n_bytes<2>();
    test_atomic_n_bytes<3>();
    test_atomic_n_bytes<4>();
    test_atomic_n_bytes<5>();
    test_atomic_n_bytes<6>();
    test_atomic_n_bytes<7>();
    test_atomic_n_bytes<8>();
    test_atomic_n_bytes<9>();
    test_atomic_n_bytes<10>();
}

// TEST FENCES
static void test_fences() { // test that fence functions exist
                            // these have no easily testable semantics
    STD atomic_thread_fence(STD memory_order_relaxed);
    STD atomic_signal_fence(STD memory_order_seq_cst);
}

// TEST TYPEDEFS
static void test_typedefs() { // test that atomic typedefs exist
    CHECK_TYPE(STD atomic_int8_t, STD atomic<int8_t>);
    CHECK_TYPE(STD atomic_uint8_t, STD atomic<uint8_t>);
    CHECK_TYPE(STD atomic_int16_t, STD atomic<int16_t>);
    CHECK_TYPE(STD atomic_uint16_t, STD atomic<uint16_t>);
    CHECK_TYPE(STD atomic_int32_t, STD atomic<int32_t>);
    CHECK_TYPE(STD atomic_uint32_t, STD atomic<uint32_t>);
    CHECK_TYPE(STD atomic_int64_t, STD atomic<int64_t>);
    CHECK_TYPE(STD atomic_uint64_t, STD atomic<uint64_t>);

    CHECK_TYPE(STD atomic_int_least8_t, STD atomic<int_least8_t>);
    CHECK_TYPE(STD atomic_uint_least8_t, STD atomic<uint_least8_t>);
    CHECK_TYPE(STD atomic_int_least16_t, STD atomic<int_least16_t>);
    CHECK_TYPE(STD atomic_uint_least16_t, STD atomic<uint_least16_t>);
    CHECK_TYPE(STD atomic_int_least32_t, STD atomic<int_least32_t>);
    CHECK_TYPE(STD atomic_uint_least32_t, STD atomic<uint_least32_t>);
    CHECK_TYPE(STD atomic_int_least64_t, STD atomic<int_least64_t>);
    CHECK_TYPE(STD atomic_uint_least64_t, STD atomic<uint_least64_t>);

    CHECK_TYPE(STD atomic_int_fast8_t, STD atomic<int_fast8_t>);
    CHECK_TYPE(STD atomic_uint_fast8_t, STD atomic<uint_fast8_t>);
    CHECK_TYPE(STD atomic_int_fast16_t, STD atomic<int_fast16_t>);
    CHECK_TYPE(STD atomic_uint_fast16_t, STD atomic<uint_fast16_t>);
    CHECK_TYPE(STD atomic_int_fast32_t, STD atomic<int_fast32_t>);
    CHECK_TYPE(STD atomic_uint_fast32_t, STD atomic<uint_fast32_t>);
    CHECK_TYPE(STD atomic_int_fast64_t, STD atomic<int_fast64_t>);
    CHECK_TYPE(STD atomic_uint_fast64_t, STD atomic<uint_fast64_t>);

    CHECK_TYPE(STD atomic_intptr_t, STD atomic<intptr_t>);
    CHECK_TYPE(STD atomic_uintptr_t, STD atomic<uintptr_t>);
    CHECK_TYPE(STD atomic_size_t, STD atomic<size_t>);
    CHECK_TYPE(STD atomic_ptrdiff_t, STD atomic<ptrdiff_t>);
    CHECK_TYPE(STD atomic_intmax_t, STD atomic<intmax_t>);
    CHECK_TYPE(STD atomic_uintmax_t, STD atomic<uintmax_t>);
}
// RUN TESTS
void test_main() { // test header <atomic>
    test_memory_order_enum();
    test_kill_dependency();
    test_lock_free_property();
    test_atomic_flag();
    test_atomic_bool();
    test_atomic_integral<char, STD atomic_char>();
    test_atomic_integral<signed char, STD atomic_schar>();
    test_atomic_integral<unsigned char, STD atomic_uchar>();
    test_atomic_integral<char16_t, STD atomic_char16_t>();
    test_atomic_integral<char32_t, STD atomic_char32_t>();
    test_atomic_integral<wchar_t, STD atomic_wchar_t>();
    test_atomic_integral<short, STD atomic_short>();
    test_atomic_integral<unsigned short, STD atomic_ushort>();
    test_atomic_integral<int, STD atomic_int>();
    test_atomic_integral<unsigned int, STD atomic_uint>();
    test_atomic_integral<long, STD atomic_long>();
    test_atomic_integral<unsigned long, STD atomic_ulong>();
    test_atomic_integral<long long, STD atomic_llong>();
    test_atomic_integral<unsigned long long, STD atomic_ullong>();
    test_atomic_address();
    test_atomic_udt();
    test_fences();
    test_typedefs();
}
