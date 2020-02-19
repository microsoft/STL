// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

using namespace std;

int f0() {
    return 1729;
}

int f1(int x) {
    return x * 10;
}

int f2(int x, int y) {
    return x * 100 + y * 10;
}

int f3(int x, int y, int z) {
    return x * 1000 + y * 100 + z * 10;
}

struct Cat {
    string name;

    string noise(const string& s) const {
        return name + " says " + s;
    }
};

struct DetectQualifiers {
    string operator()() & {
        return "modifiable lvalue";
    }

    string operator()() const& {
        return "const lvalue";
    }

    string operator()() && {
        return "modifiable rvalue";
    }

    string operator()() const&& {
        return "const rvalue";
    }
};

int main() {
    // Test varying numbers of arguments.
    assert(bind_front(f0)() == 1729);

    assert(bind_front(f1)(2) == 20);
    assert(bind_front(f1, 3)() == 30);

    assert(bind_front(f2)(4, 5) == 450);
    assert(bind_front(f2, 6)(7) == 670);
    assert(bind_front(f2, 8, 9)() == 890);

    assert(bind_front(f3)(2, 3, 4) == 2340);
    assert(bind_front(f3, 3)(4, 5) == 3450);
    assert(bind_front(f3, 4, 5)(6) == 4560);
    assert(bind_front(f3, 5, 6, 7)() == 5670);

    // Test function pointers.
    assert(bind_front(&f0)() == 1729);
    assert(bind_front(&f2, 6)(7) == 670);

    // Test stateless lambdas.
    assert(bind_front([] { return 11; })() == 11);
    assert(bind_front([](int x, int y) { return x * 2 + y * 3; }, 100)(10) == 230);

    // Test stateful lambdas.
    int value = 0;

    auto bound0 = bind_front([&value] { ++value; });
    bound0();
    assert(value == 1);
    bound0();
    assert(value == 2);

    auto bound1 = bind_front([&value](int x, int y) { value = value * x + y; }, 10);
    bound1(3);
    assert(value == 23);
    bound1(4);
    assert(value == 234);

    // Test PMFs.
    Cat cat{"Peppermint"};
    auto bound2 = bind_front(&Cat::noise, cat); // stores a copy
    assert(bound2("meow") == "Peppermint says meow");
    cat.name = "Fluffy";
    assert(cat.noise("hiss") == "Fluffy says hiss");
    assert(bound2("purr") == "Peppermint says purr");

    auto bound3 = bind_front(&Cat::noise, &cat); // stores a pointer
    assert(bound3("MEOW") == "Fluffy says MEOW");
    cat.name = "Peppermint";
    assert(bound3("PURR") == "Peppermint says PURR");

    auto bound4 = bind_front(&Cat::noise, ref(cat)); // stores a reference_wrapper, uses LWG-2219
    assert(bound4("Why do you keep renaming me?") == "Peppermint says Why do you keep renaming me?");
    cat.name = "Cat";
    assert(bound4("You can't rename me anymore, Human") == "Cat says You can't rename me anymore, Human");

    // Test "perfect forwarding call wrapper" behavior.
    auto bound5 = bind_front(DetectQualifiers{});
    assert(bound5() == "modifiable lvalue");
    assert(as_const(bound5)() == "const lvalue");
    assert(move(bound5)() == "modifiable rvalue");
    assert(move(as_const(bound5))() == "const rvalue");

    // Test movable-only types.
    auto unique_lambda = [up1 = make_unique<int>(1200)](unique_ptr<int>&& up2) {
        if (up1 && up2) {
            return make_unique<int>(*up1 + *up2);
        } else if (up1) {
            return make_unique<int>(*up1 * -1);
        } else if (up2) {
            return make_unique<int>(*up2 * -10);
        } else {
            return make_unique<int>(-9000);
        }
    };
    auto bound6 = bind_front(move(unique_lambda), make_unique<int>(34));
    assert(*unique_lambda(make_unique<int>(56)) == -560);
    assert(*move(bound6)() == 1234);
    auto bound7 = move(bound6);
    assert(*move(bound6)() == -9000);
    assert(*move(bound7)() == 1234);

    // Test decay when binding.
    const int arr[] = {11, 22, 33};
    const int three = 3;

    auto bound8 = bind_front(
        [](auto&& a, auto&& f, auto&& i) {
            using FP = int (*)(int);
            return is_same_v<decltype(a), const int*&> && is_same_v<decltype(f), FP&> && is_same_v<decltype(i), int&>;
        },
        arr, f1, three);
    assert(bound8());

    // Test forward when calling.
    auto bound9 = bind_front([](auto&& a1, auto&& a2, auto&& a3, auto&& a4) {
        constexpr bool same1 = is_same_v<decltype(a1), int&>;
        constexpr bool same2 = is_same_v<decltype(a2), const int&>;
        constexpr bool same3 = is_same_v<decltype(a3), int&&>;
        constexpr bool same4 = is_same_v<decltype(a4), const int&&>;
        return same1 && same2 && same3 && same4;
    });
    assert(bound9(value, three, 1729, move(three)));
}
