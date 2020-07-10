#include <atomic>
#include <cassert>

struct X1 {
    char x : 6;

    void set(char v) {
        x = v;
    }
};

struct X2 {
    short x : 9;

    void set(char v) {
        x = v;
    }
};

#pragma pack(push,1)
struct X3 {
    char x : 4;
    char : 2;
    char y : 1;
    short z;

    void set(char v) {
        x = v;
        y = 0;
        z = 0;
    }
};
#pragma pack(pop)

#pragma warning(push)
#pragma warning(disable:4324) // '%s': structure was padded due to alignment specifier
struct alignas(4) X4 {
    char x;

    void set(char v) {
        x = v;
    }
};
#pragma warning(pop)

struct X8 {
    char x;
    long y;

    void set(char v) {
        x = v;
        y = 0;
    }
};

struct X16 {
    long x;
    char y;
    long long z;

    void set(char v) {
        x = v;
        y = 0;
        z = 0;
    }
};

struct X20 {
    long x;
    long y[3];
    char z;

    void set(char v) {
        x = v;
        memset(&y, 0, sizeof(y));
        z = 0;
    }
};


template<class X, std::size_t S>
void test() {
    static_assert(sizeof(X) == S, "Unexpected size");
    X x2;
    X x3;
    X x4;
    X x1;
    memset(&x1, 0x00, sizeof(x1));
    memset(&x2, 0xff, sizeof(x1));
    memset(&x3, 0xff, sizeof(x1));
    x1.set(5);
    x2.set(5);
    x3.set(6);
    x4.set(7);

    std::atomic<X> v;
    v.store(x1);
    X x;
    memcpy(&x, &x3, sizeof(x));
    assert(!v.compare_exchange_strong(x, x4));
    assert(v.load().x == 5);

    v.store(x1);
    for (int retry = 0; retry != 10; ++retry) {
        X xw;
        memcpy(&xw, &x3, sizeof(x));
        assert(!v.compare_exchange_weak(xw, x4));
        assert(v.load().x == 5);
    }

    v.store(x1);
    memcpy(&x, &x2, sizeof(x));
    assert(v.compare_exchange_strong(x, x3));
    assert(v.load().x == 6);

    v.store(x1);
    for (;;) {
        X xw;
        memcpy(&xw, &x2, sizeof(x));
        if (v.compare_exchange_weak(xw, x3))
            break;
    }
    assert(v.load().x == 6);
}



int main()
{
    test<X1, 1>();
    test<X2, 2>();
    test<X3, 3>();
    test<X4, 4>();
    test<X8, 8>();
    test<X16, 16>();
    test<X20, 20>();
    return 0;
}