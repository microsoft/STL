#include <cassert>
#include <tuple>

using namespace std;

struct EvilBool {
    bool b;

    operator bool() const {
        return b;
    }
    EvilBool operator!() const;

    // EVILLLLL
    friend bool operator&&(const EvilBool lhs, bool rhs);
    friend bool operator||(const EvilBool lhs, bool rhs);
};

struct EqLtReturnsEvil {
    friend EvilBool operator==(const EqLtReturnsEvil&, const EqLtReturnsEvil&) noexcept {
        return EvilBool{true};
    }
    friend EvilBool operator<(const EqLtReturnsEvil&, const EqLtReturnsEvil&) noexcept {
        return EvilBool{false};
    }
};

int main() {
    tuple<EqLtReturnsEvil> x, y;
    assert(x == y);
    assert(!(x < y));
    assert(!(x > y));
}
