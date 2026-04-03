// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <concepts>
#include <memory>
using namespace std;

#ifdef __cpp_lib_start_lifetime_as // TRANSITION
struct Point {
    int x;
    int y;
};
static_assert(alignof(Point) == alignof(int));
static_assert(sizeof(Point) == 2 * sizeof(int));

template <class CvVoidPtr, class CvPointPtr>
void test_scalar() {
    int arr[2]{17, 29};

    const auto cv_void_ptr = static_cast<CvVoidPtr>(arr);

    static_assert(noexcept(start_lifetime_as<Point>(cv_void_ptr)));
    const same_as<CvPointPtr> auto cv_point_ptr = start_lifetime_as<Point>(cv_void_ptr);

    assert(cv_point_ptr->x == 17);
    assert(cv_point_ptr->y == 29);
}

template <class CvVoidPtr, class CvPointPtr>
void test_array() {
    {
        // N5032 [obj.lifetime]/6: "Preconditions: p is suitably aligned for an array of T or is null."
        // N5032 [obj.lifetime]/7: "Effects: If n > 0 is true, [...]. Otherwise, there are no effects."
        const same_as<CvPointPtr> auto nil = start_lifetime_as_array<Point>(static_cast<CvVoidPtr>(nullptr), 0);
        assert(nil == nullptr); // N5032 [obj.lifetime]/8: "otherwise, a pointer that compares equal to p"
    }

    int arr[4]{17, 29, 123, 456};

    const auto cv_void_ptr = static_cast<CvVoidPtr>(arr);

    static_assert(noexcept(start_lifetime_as_array<Point>(cv_void_ptr, 2)));
    const same_as<CvPointPtr> auto cv_point_ptr = start_lifetime_as_array<Point>(cv_void_ptr, 2);

    assert(cv_point_ptr[0].x == 17);
    assert(cv_point_ptr[0].y == 29);
    assert(cv_point_ptr[1].x == 123);
    assert(cv_point_ptr[1].y == 456);
}

template <class CvVoidPtr, class CvPointPtr>
void test() {
    test_scalar<CvVoidPtr, CvPointPtr>();
    test_array<CvVoidPtr, CvPointPtr>();
}

int main() {
    test<void*, Point*>();
    test<const void*, const Point*>();
    test<volatile void*, volatile Point*>();
    test<const volatile void*, const volatile Point*>();
}
#else // ^^^ no workaround / workaround vvv
int main() {}
#endif // ^^^ workaround ^^^
