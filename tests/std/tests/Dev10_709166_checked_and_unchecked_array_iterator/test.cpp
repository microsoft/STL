// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <class T>
void check_checked_array_iterator_category_and_convertibility() {
    STATIC_ASSERT(std::is_same_v<typename stdext::checked_array_iterator<T*>::iterator_category,
        std::random_access_iterator_tag>);

    STATIC_ASSERT(std::is_same_v<typename stdext::checked_array_iterator<T*>::value_type, std::remove_cv_t<T>>);

    STATIC_ASSERT(std::is_same_v<typename stdext::checked_array_iterator<T*>::difference_type, std::ptrdiff_t>);

    STATIC_ASSERT(std::is_same_v<typename stdext::checked_array_iterator<T*>::pointer, T*>);

    STATIC_ASSERT(std::is_same_v<typename stdext::checked_array_iterator<T*>::reference, T&>);

    STATIC_ASSERT(std::is_convertible_v<stdext::checked_array_iterator<T*>, stdext::checked_array_iterator<const T*>>);

#if _HAS_CXX20
    STATIC_ASSERT(
        std::is_same_v<typename stdext::checked_array_iterator<T*>::iterator_concept, std::contiguous_iterator_tag>);

    STATIC_ASSERT(std::contiguous_iterator<stdext::checked_array_iterator<T*>>);
#endif // _HAS_CXX20
}

template <class T>
void check_unchecked_array_iterator_category_and_convertibility() {
    STATIC_ASSERT(std::is_same_v<typename stdext::unchecked_array_iterator<T*>::iterator_category,
        std::random_access_iterator_tag>);

    STATIC_ASSERT(std::is_same_v<typename stdext::unchecked_array_iterator<T*>::value_type, std::remove_cv_t<T>>);

    STATIC_ASSERT(std::is_same_v<typename stdext::unchecked_array_iterator<T*>::difference_type, std::ptrdiff_t>);

    STATIC_ASSERT(std::is_same_v<typename stdext::unchecked_array_iterator<T*>::pointer, T*>);

    STATIC_ASSERT(std::is_same_v<typename stdext::unchecked_array_iterator<T*>::reference, T&>);

    STATIC_ASSERT(
        std::is_convertible_v<stdext::unchecked_array_iterator<T*>, stdext::unchecked_array_iterator<const T*>>);

#if _HAS_CXX20
    STATIC_ASSERT(
        std::is_same_v<typename stdext::unchecked_array_iterator<T*>::iterator_concept, std::contiguous_iterator_tag>);

    STATIC_ASSERT(std::contiguous_iterator<stdext::unchecked_array_iterator<T*>>);
#endif // _HAS_CXX20
}

int main() {
    {
        check_checked_array_iterator_category_and_convertibility<int>();
        check_checked_array_iterator_category_and_convertibility<const int>();
        check_checked_array_iterator_category_and_convertibility<std::tuple<const char*>>();


        int* const p = new int[9];

        for (int i = 0; i < 9; ++i) {
            p[i] = (i + 1) * 11;
        }


        auto cat = stdext::make_checked_array_iterator(p, 9);

        STATIC_ASSERT(std::is_same_v<decltype(cat), stdext::checked_array_iterator<int*>>);

#if _HAS_CXX20
        assert(std::to_address(cat) == &*cat);
        assert(std::to_address(cat + 8) == &*cat + 8);
        assert(std::to_address(cat + 8) == std::to_address(cat) + 8);
        assert(std::to_address(cat + 9) == std::to_address(cat) + 9);
#endif // _HAS_CXX20


        auto dog = stdext::make_checked_array_iterator(p, 9, 3);

        STATIC_ASSERT(std::is_same_v<decltype(dog), stdext::checked_array_iterator<int*>>);

#if _HAS_CXX20
        assert(std::to_address(dog) == &*dog);
        assert(std::to_address(dog + 5) == &*dog + 5);
        assert(std::to_address(dog + 5) == std::to_address(dog) + 5);
        assert(std::to_address(dog - 3) == &*dog - 3);
        assert(std::to_address(dog - 3) == std::to_address(dog) - 3);
        assert(std::to_address(dog + 6) == std::to_address(dog) + 6);
#endif // _HAS_CXX20


        {
            stdext::checked_array_iterator<int*> zero_arg;
            (void) zero_arg;
            stdext::checked_array_iterator<int*> two_arg(p, 9);
            stdext::checked_array_iterator<int*> three_arg(p, 9, 3);
        }


        assert(cat.base() == p);
        assert(dog.base() == p + 3);


        assert(*p == 11 && *cat == 11);
        *cat = 1729;
        assert(*p == 1729 && *cat == 1729);
        *cat = 11;
        assert(*p == 11 && *cat == 11);

        assert(p[3] == 44 && *dog == 44);
        *dog = 1729;
        assert(p[3] == 1729 && *dog == 1729);
        *dog = 44;
        assert(p[3] == 44 && *dog == 44);


        assert(p[5] == 66 && cat[5] == 66);
        cat[5] = 5678;
        assert(p[5] == 5678 && cat[5] == 5678);
        cat[5] = 66;
        assert(p[5] == 66 && cat[5] == 66);


        {
            std::pair<int, int>* const q = new std::pair<int, int>[3];

            q[0] = std::make_pair(100, 200);
            q[1] = std::make_pair(300, 400);
            q[2] = std::make_pair(500, 600);

            auto iter = stdext::make_checked_array_iterator(q, 3);

            assert(q->second == 200 && iter->second == 200);
            iter->second = 1234;
            assert(q->second == 1234 && iter->second == 1234);
            iter->second = 200;
            assert(q->second == 200 && iter->second == 200);

            delete[] q;
        }


        assert(!(cat == dog));
        assert(!(dog == cat));
        assert(cat == cat);
        assert(dog == dog);

        assert(cat != dog);
        assert(dog != cat);
        assert(!(cat != cat));
        assert(!(dog != dog));

        assert(cat < dog);
        assert(!(dog < cat));
        assert(!(cat < cat));
        assert(!(dog < dog));

        assert(!(cat > dog));
        assert(dog > cat);
        assert(!(cat > cat));
        assert(!(dog > dog));

        assert(cat <= dog);
        assert(!(dog <= cat));
        assert(cat <= cat);
        assert(dog <= dog);

        assert(!(cat >= dog));
        assert(dog >= cat);
        assert(cat >= cat);
        assert(dog >= dog);


        assert(cat + 3 == dog);
        assert(3 + cat == dog);

        assert(dog - 3 == cat);

        assert(dog - cat == 3);
        assert(cat - dog == -3);


        cat += 6;
        assert(*cat == 77);
        cat += -6;
        assert(*cat == 11);

        dog -= 2;
        assert(*dog == 22);
        dog -= -2;
        assert(*dog == 44);


        ++cat;
        assert(*cat == 22);
        --cat;
        assert(*cat == 11);


        auto tiger = cat++;
        assert(*tiger == 11);
        assert(*cat == 22);
        auto kitten = cat--;
        assert(*kitten == 22);
        assert(*cat == 11);


        auto dupe = dog;
        assert(*dupe == 44);
        dupe = cat;
        assert(*dupe == 11);


        std::vector<int> v(9);

        for (int i = 0; i < 9; ++i) {
            v[static_cast<unsigned int>(i)] = 1000 + i;
        }

        std::copy(v.begin(), v.end(), cat);

        assert(*p == 1000);
        assert(p[5] == 1005);
        assert(p[8] == 1008);

        delete[] p;
    }

    {
        check_unchecked_array_iterator_category_and_convertibility<int>();
        check_unchecked_array_iterator_category_and_convertibility<const int>();
        check_unchecked_array_iterator_category_and_convertibility<std::tuple<const char*>>();


        int* const p = new int[9];

        for (int i = 0; i < 9; ++i) {
            p[i] = (i + 1) * 11;
        }


        auto cat = stdext::make_unchecked_array_iterator(p);

        STATIC_ASSERT(std::is_same_v<decltype(cat), stdext::unchecked_array_iterator<int*>>);

#if _HAS_CXX20
        assert(std::to_address(cat) == &*cat);
        assert(std::to_address(cat + 8) == &*cat + 8);
        assert(std::to_address(cat + 8) == std::to_address(cat) + 8);
        assert(std::to_address(cat + 9) == std::to_address(cat) + 9);
#endif // _HAS_CXX20


        auto dog = stdext::make_unchecked_array_iterator(p + 3);

        STATIC_ASSERT(std::is_same_v<decltype(dog), stdext::unchecked_array_iterator<int*>>);

#if _HAS_CXX20
        assert(std::to_address(dog) == &*dog);
        assert(std::to_address(dog + 5) == &*dog + 5);
        assert(std::to_address(dog + 5) == std::to_address(dog) + 5);
        assert(std::to_address(dog - 3) == &*dog - 3);
        assert(std::to_address(dog - 3) == std::to_address(dog) - 3);
        assert(std::to_address(dog + 6) == std::to_address(dog) + 6);
#endif // _HAS_CXX20


        {
            stdext::unchecked_array_iterator<int*> zero_arg;
            (void) zero_arg;
            stdext::unchecked_array_iterator<int*> one_arg(p);
        }


        assert(cat.base() == p);
        assert(dog.base() == p + 3);


        assert(*p == 11 && *cat == 11);
        *cat = 1729;
        assert(*p == 1729 && *cat == 1729);
        *cat = 11;
        assert(*p == 11 && *cat == 11);

        assert(p[3] == 44 && *dog == 44);
        *dog = 1729;
        assert(p[3] == 1729 && *dog == 1729);
        *dog = 44;
        assert(p[3] == 44 && *dog == 44);


        assert(p[5] == 66 && cat[5] == 66);
        cat[5] = 5678;
        assert(p[5] == 5678 && cat[5] == 5678);
        cat[5] = 66;
        assert(p[5] == 66 && cat[5] == 66);


        {
            std::pair<int, int>* const q = new std::pair<int, int>[3];

            q[0] = std::make_pair(100, 200);
            q[1] = std::make_pair(300, 400);
            q[2] = std::make_pair(500, 600);

            auto iter = stdext::make_unchecked_array_iterator(q);

            assert(q->second == 200 && iter->second == 200);
            iter->second = 1234;
            assert(q->second == 1234 && iter->second == 1234);
            iter->second = 200;
            assert(q->second == 200 && iter->second == 200);

            delete[] q;
        }


        assert(!(cat == dog));
        assert(!(dog == cat));
        assert(cat == cat);
        assert(dog == dog);

        assert(cat != dog);
        assert(dog != cat);
        assert(!(cat != cat));
        assert(!(dog != dog));

        assert(cat < dog);
        assert(!(dog < cat));
        assert(!(cat < cat));
        assert(!(dog < dog));

        assert(!(cat > dog));
        assert(dog > cat);
        assert(!(cat > cat));
        assert(!(dog > dog));

        assert(cat <= dog);
        assert(!(dog <= cat));
        assert(cat <= cat);
        assert(dog <= dog);

        assert(!(cat >= dog));
        assert(dog >= cat);
        assert(cat >= cat);
        assert(dog >= dog);


        assert(cat + 3 == dog);
        assert(3 + cat == dog);

        assert(dog - 3 == cat);

        assert(dog - cat == 3);
        assert(cat - dog == -3);


        cat += 6;
        assert(*cat == 77);
        cat += -6;
        assert(*cat == 11);

        dog -= 2;
        assert(*dog == 22);
        dog -= -2;
        assert(*dog == 44);


        ++cat;
        assert(*cat == 22);
        --cat;
        assert(*cat == 11);


        auto tiger = cat++;
        assert(*tiger == 11);
        assert(*cat == 22);
        auto kitten = cat--;
        assert(*kitten == 22);
        assert(*cat == 11);


        auto dupe = dog;
        assert(*dupe == 44);
        dupe = cat;
        assert(*dupe == 11);


        std::vector<int> v(9);

        for (int i = 0; i < 9; ++i) {
            v[static_cast<unsigned int>(i)] = 1000 + i;
        }

        std::copy(v.begin(), v.end(), cat);

        assert(*p == 1000);
        assert(p[5] == 1005);
        assert(p[8] == 1008);

        delete[] p;
    }
}
