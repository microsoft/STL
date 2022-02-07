#include <cassert>
#include <utility>

constexpr int test_impl(int arg) {
    switch (arg) {
    case 1:
        return 'x';

    case 2:
        return 'y';

    default:
        std::unreachable();
    }
}

constexpr bool test() {
    assert(test_impl(1) == 'x');
    assert(test_impl(2) == 'y');
    return true;
}

static_assert(test());

int main() {
    test();
}
