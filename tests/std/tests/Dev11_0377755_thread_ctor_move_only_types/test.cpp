// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

class A {
    std::thread::id parent_id;
    unsigned int copy_cnt = 0u;
    unsigned int move_cnt = 0u;

public:
    explicit A(int ii) : parent_id(std::this_thread::get_id()), i(ii) {}
    A(const A& a) : parent_id(a.parent_id), copy_cnt(a.copy_cnt + 1u), move_cnt(a.move_cnt), i(a.i) {
        assert(std::this_thread::get_id() == a.parent_id);
    }
    A(A&& a) : parent_id(a.parent_id), copy_cnt(a.copy_cnt), move_cnt(a.move_cnt + 1u), i(a.i) {
        assert(std::this_thread::get_id() == a.parent_id);
    }

    bool is_copy_move_correct() const {
        return copy_cnt + move_cnt <= 1u;
    }

    int i;
};

int main() { // DevDiv-377755 "<thread>: thread's ctor doesn't compile with movable-only arguments"
    std::vector<std::thread> t;
    std::unique_ptr<int> p = std::make_unique<int>(-1);
    // Check if std::thread ctor accepts move-only arguments.
    t.emplace_back([](std::unique_ptr<int> p) { assert(*p == -1); }, std::move(p));

    // Check if std::async works with function that returns move-only type
    auto f = std::async([]() { return std::make_unique<int>(42); });
    p      = f.get();
    assert(*p == 42);

    // Check if std::promise works with move-only types.
    std::promise<std::unique_ptr<int>> prom;
    t.emplace_back([&]() { prom.set_value(std::make_unique<int>(43)); });
    p = prom.get_future().get();
    assert(*p == 43);

    // Check if reference unwrapping occurs (it should not).
    int i = -1;
    t.emplace_back([](std::reference_wrapper<int> ri) { assert(ri == -1); }, std::ref(i));

    // Check if the copy occurs on the parent thread, as required by the Standard.
    A a(-1);
    t.emplace_back([](const A& an_a) { assert(an_a.i == -1); }, a);

    // Check if the move occurs on the parent thread, as required by the Standard.
    t.emplace_back([](const A& an_a) { assert(an_a.i == -1); }, std::move(a));

    // Check if we are doing the correct number of copies.
    A b(-1);
    t.emplace_back([](const A& an_a) { assert(an_a.is_copy_move_correct() && an_a.i == -1); }, b);

    // Check if we are doing the correct number of moves.
    t.emplace_back([](const A& an_a) { assert(an_a.is_copy_move_correct() && an_a.i == -1); }, std::move(b));

    for (auto&& th : t) {
        th.join();
    }
}
