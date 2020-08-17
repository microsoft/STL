#include <cassert>
#include <random>
#include <cstdint>
#include <limits>
#include <cmath>

template <class T>
using lim = std::numeric_limits<T>;

template <class T>
void TruncAndCheck(T i, float fmax) {
    const float x{std::_Trunc_to_float<float, T>(i)};
    const float y{std::nextafter(x, lim<float>::infinity())};

    assert(x < fmax);
    assert(static_cast<T>(x) <= i);
    assert(y <= fmax);
    if (y < fmax)
        assert(static_cast<T>(y) > i);
}

template <class T>
void TestTruncExhaustive() {
    const float fmax{exp2(static_cast<float>(lim<T>::digits))};
    T i{0};
    do {
        TruncAndCheck(i, fmax);
    } while (++i != T{0});
}

template <class T>
constexpr T FillLsb(int n) {
    if (n <= 0)
        return 0;
    T x{T{1} << (n - 1)};
    return (x - 1) ^ x;
}

template <class T>
void TestTruncSelective() {
    const float fmax{exp2(static_cast<float>(lim<T>::digits))};
    TruncAndCheck(T{0}, fmax);
    TruncAndCheck(T{1}, fmax);
    TruncAndCheck(lim<T>::max(), fmax);

    // crossover from ulp < 1 to ulp = 1
    constexpr T a{FillLsb<T>(lim<float>::digits - 1)};
    TruncAndCheck(a - 1, fmax);
    TruncAndCheck(a, fmax);

    // crossover from ulp = 1 to ulp > 1
    constexpr T b{FillLsb<T>(lim<float>::digits)};
    TruncAndCheck(b, fmax);
    TruncAndCheck(b + 1, fmax);
    TruncAndCheck(b + 2, fmax);

    // saturation at the largest representable T
    constexpr int diff{lim<T>::digits - lim<float>::digits};
    constexpr T c{FillLsb<T>(lim<float>::digits) << diff};
    TruncAndCheck(c - 1, fmax);
    TruncAndCheck(c, fmax);
    TruncAndCheck(c + 1, fmax);
}

int main() {
    TestTruncExhaustive<std::uint8_t>();
    TestTruncExhaustive<std::uint16_t>();
    TestTruncSelective<std::uint32_t>();
}