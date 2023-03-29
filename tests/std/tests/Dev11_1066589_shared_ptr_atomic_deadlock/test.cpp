// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_CXX20_OLD_SHARED_PTR_ATOMIC_SUPPORT_DEPRECATION_WARNING

#include <cstdlib>
#include <memory>

#ifndef _M_CEE_PURE
#include <atomic>
#endif // _M_CEE_PURE

using namespace std;

shared_ptr<int> g_sp;

struct Noisy {
    Noisy() = default;

    ~Noisy() {
        (void) atomic_load(&g_sp);
    }

    Noisy(const Noisy&)            = delete;
    Noisy& operator=(const Noisy&) = delete;
};

int main() {
    {
        shared_ptr<Noisy> dest;
        shared_ptr<Noisy> src;
        atomic_store(&dest, src);
    }

    {
        shared_ptr<Noisy> dest;
        shared_ptr<Noisy> src = make_shared<Noisy>();
        atomic_store(&dest, src);
    }

    {
        shared_ptr<Noisy> dest = make_shared<Noisy>();
        shared_ptr<Noisy> src;
        atomic_store(&dest, src);
    }

    {
        shared_ptr<Noisy> dest = make_shared<Noisy>();
        shared_ptr<Noisy> src  = dest;
        atomic_store(&dest, src);
    }

    {
        shared_ptr<Noisy> dest = make_shared<Noisy>();
        shared_ptr<Noisy> src  = make_shared<Noisy>();
        atomic_store(&dest, src);
    }

    // **********

    {
        shared_ptr<Noisy> dest;
        shared_ptr<Noisy> src;
        atomic_exchange(&dest, src);
    }

    {
        shared_ptr<Noisy> dest;
        shared_ptr<Noisy> src = make_shared<Noisy>();
        atomic_exchange(&dest, src);
    }

    {
        shared_ptr<Noisy> dest = make_shared<Noisy>();
        shared_ptr<Noisy> src;
        atomic_exchange(&dest, src);
    }

    {
        shared_ptr<Noisy> dest = make_shared<Noisy>();
        shared_ptr<Noisy> src  = dest;
        atomic_exchange(&dest, src);
    }

    {
        shared_ptr<Noisy> dest = make_shared<Noisy>();
        shared_ptr<Noisy> src  = make_shared<Noisy>();
        atomic_exchange(&dest, src);
    }

    // **********

    {
        shared_ptr<Noisy> object;
        shared_ptr<Noisy> expected = make_shared<Noisy>();
        shared_ptr<Noisy> desired;
        atomic_compare_exchange_weak(&object, &expected, desired);
    }

    {
        // Nothing forbids aliasing here.
        shared_ptr<Noisy> object = make_shared<Noisy>();
        shared_ptr<Noisy> desired;
        atomic_compare_exchange_weak(&object, &object, desired);
    }

    // **********

    {
        shared_ptr<Noisy> object;
        shared_ptr<Noisy> expected = make_shared<Noisy>();
        shared_ptr<Noisy> desired;
        atomic_compare_exchange_strong(&object, &expected, desired);
    }

    {
        // Nothing forbids aliasing here.
        shared_ptr<Noisy> object = make_shared<Noisy>();
        shared_ptr<Noisy> desired;
        atomic_compare_exchange_strong(&object, &object, desired);
    }

    // Also test VSO-911206 "Error specifying explicit template argument of abstract type for atomic non-member
    // functions"
    {
        struct Base {
            virtual ~Base()     = default;
            virtual void test() = 0;
        };

        struct Derived final : Base {
            virtual void test() override {}
        };

        shared_ptr<Base> object;

        atomic_store<Base>(&object, make_shared<Derived>());
        atomic_store_explicit<Base>(&object, make_shared<Derived>(), memory_order_seq_cst);

        atomic_exchange<Base>(&object, make_shared<Derived>());
        atomic_exchange_explicit<Base>(&object, make_shared<Derived>(), memory_order_seq_cst);

        atomic_compare_exchange_weak<Base>(&object, &object, make_shared<Derived>());
        atomic_compare_exchange_weak_explicit<Base>(
            &object, &object, make_shared<Derived>(), memory_order_seq_cst, memory_order_seq_cst);

        atomic_compare_exchange_strong<Base>(&object, &object, make_shared<Derived>());
        atomic_compare_exchange_strong_explicit<Base>(
            &object, &object, make_shared<Derived>(), memory_order_seq_cst, memory_order_seq_cst);
    }
}
