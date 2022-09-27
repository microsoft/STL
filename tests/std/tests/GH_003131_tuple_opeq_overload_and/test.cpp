#include <cassert>
#include <tuple>

using namespace std;

struct WeirdBoolBinOp {
    bool b;

    operator bool() const {
        return b;
    }
    WeirdBoolBinOp operator!() const {
        return WeirdBoolBinOp{!b};
    }

    // EVILLLLL
    friend bool operator&&(const WeirdBoolBinOp& lhs, bool rhs) {
        return !lhs.b && rhs;
    }
    friend bool operator||(const WeirdBoolBinOp& lhs, bool rhs) {
        return !lhs.b || rhs;
    }
};
struct WeirdBoolNot {
    bool b;

    operator bool() const {
        return b;
    }
    // EVILLLLL
    WeirdBoolNot operator!() const {
        return *this;
    }
};


template <class T>
struct EqLtReturnsT {
    friend T operator==(const EqLtReturnsT&, const EqLtReturnsT&) noexcept {
        return T{true};
    }
    friend T operator<(const EqLtReturnsT&, const EqLtReturnsT&) noexcept {
        return T{false};
    }
};

int main() {
    tuple<EqLtReturnsT<WeirdBoolBinOp>> x1, y1;
    assert(x1 == y1);
    assert(!(x1 < y1));
    assert(!(x1 > y1));

    tuple<EqLtReturnsT<WeirdBoolNot>> x2, y2;
    assert(x2 == y2);
    assert(!(x2 < y2));
    assert(!(x2 > y2));
}
