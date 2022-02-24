// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <memory>
#include <utility>

using namespace std;

struct Dummy {
    constexpr Dummy() = default;
    constexpr int test() const {
        return 10;
    }
};

constexpr bool test() {
    // [memory.syn] 20.11.1
    {
        auto p1 = make_unique<int>(42);
        auto p2 = make_unique_for_overwrite<int>();
        swap(p1, p2);
        assert(p1 == p1);
        assert(p1 != p2);

#if defined(__EDG__) || defined(__clang__) // TRANSITION, DevCom-1436243
        auto p3 = make_unique<int[]>(10);
        auto p4 = make_unique_for_overwrite<int[]>(4);
        swap(p3, p4);
        assert(p3 == p3);
        assert(p3 != p4);
#endif // defined(__EDG__) || defined(__clang__)

        auto p5 = unique_ptr<int>{nullptr};
        assert(p5 == nullptr);
#ifndef __EDG__ // TRANSITION, DevCom-1670927
        assert(!(p5 < nullptr));
        assert(p5 <= nullptr);
        assert(!(p5 > nullptr));
        assert(p5 >= nullptr);
        assert((p5 <=> nullptr) == strong_ordering::equal);
#endif // !__EDG__
    }

    // changes in [unique.ptr.dltr.dflt] 20.11.1.1.2 and  [unique.ptr.dltr.dflt1] 20.11.1.1.3
    // will be tested via destructors and copy assign/constructors

    // [unique.ptr.single.general] 20.11.1.3.1
    {
        // constructors
        auto p1 = unique_ptr<int>{new int{}};
        auto d1 = default_delete<int>{};
        auto p2 = unique_ptr<int>{new int{}, d1};
        auto p3 = unique_ptr<int>{new int{}, default_delete<int>{}};
        auto p4 = move(p3);
        auto p5 = unique_ptr<int>{nullptr};
        auto p6 = unique_ptr<int, default_delete<int>&>{new int{}, d1};
        auto p7 = unique_ptr<int>{move(p6)};

        // assignment
        p3      = move(p4);
        auto p8 = unique_ptr<int, default_delete<int>&>{new int{}, d1};
        p7      = move(p8);
        p1      = nullptr;

        // observers
        assert(*p2 == 0);
        auto p9 = unique_ptr<Dummy>{new Dummy};
        assert(p9->test() == 10);
        assert(p2.get() != nullptr);
        [[maybe_unused]] auto& d2 = p2.get_deleter();
        [[maybe_unused]] auto& d3 = as_const(p2).get_deleter();
        auto b1                   = static_cast<bool>(p2);
        assert(b1);

        // modifiers
        p1.reset();
        auto manual_delete = p2.get();
        p2.release();
        delete manual_delete;
        p5.swap(p1);
    }

    // [unique.ptr.runtime.general] 20.11.1.4.1:
    {
        // constructors
        auto p1 = unique_ptr<int[]>{new int[5]};
        auto d1 = default_delete<int[]>{};
        auto p2 = unique_ptr<int[]>{new int[5], d1};
        auto p3 = unique_ptr<int[]>{new int[5], default_delete<int[]>{}};
        auto p4 = move(p1);
        auto p5 = unique_ptr<int[], default_delete<int[]>&>{new int[5], d1};
        auto p6 = unique_ptr<int[]>{move(p5)};

        // assignment
        p1      = move(p4);
        auto p7 = unique_ptr<int[], default_delete<int[]>&>{new int[5], d1};
        p6      = move(p7);
        p4      = nullptr;

        // observers
        p1[0] = 5;
        assert(p1[0] == 5);
        assert(p1.get() != nullptr);
        [[maybe_unused]] auto& d2 = p1.get_deleter();
        [[maybe_unused]] auto& d3 = as_const(p1).get_deleter();
        auto b1                   = static_cast<bool>(p1);
        assert(b1);

        // modifiers
        auto manual_delete = p1.get();
        p1.release();
        delete[] manual_delete;
        p1.reset(new int[3]);
        p1.reset(nullptr);
        p1.swap(p4);
    }

    return true;
}

static_assert(test());

int main() {} // COMPILE-ONLY
