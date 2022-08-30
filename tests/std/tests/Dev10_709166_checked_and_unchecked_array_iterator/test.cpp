// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

int main() {
    {
        int* const p = new int[9];

        for (int i = 0; i < 9; ++i) {
            p[i] = (i + 1) * 11;
        }


        auto cat = stdext::make_checked_array_iterator(p, 9);

        static_assert(std::is_same_v<decltype(cat), stdext::checked_array_iterator<int*>>,
            "stdext::make_checked_array_iterator(p, 9)'s return type is wrong!");


        auto dog = stdext::make_checked_array_iterator(p, 9, 3);

        static_assert(std::is_same_v<decltype(dog), stdext::checked_array_iterator<int*>>,
            "stdext::make_checked_array_iterator(p, 9, 3)'s return type is wrong!");


        static_assert(
            std::is_same_v<stdext::checked_array_iterator<int*>::iterator_category, std::random_access_iterator_tag>,
            "stdext::checked_array_iterator<int *>::iterator_category is wrong!");

        static_assert(std::is_same_v<stdext::checked_array_iterator<int*>::value_type, int>,
            "stdext::checked_array_iterator<int *>::value_type is wrong!");

        static_assert(std::is_same_v<stdext::checked_array_iterator<int*>::difference_type, ptrdiff_t>,
            "stdext::checked_array_iterator<int *>::difference_type is wrong!");

        static_assert(std::is_same_v<stdext::checked_array_iterator<int*>::pointer, int*>,
            "stdext::checked_array_iterator<int *>::pointer is wrong!");

        static_assert(std::is_same_v<stdext::checked_array_iterator<int*>::reference, int&>,
            "stdext::checked_array_iterator<int *>::reference is wrong!");


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
        int* const p = new int[9];

        for (int i = 0; i < 9; ++i) {
            p[i] = (i + 1) * 11;
        }


        auto cat = stdext::make_unchecked_array_iterator(p);

        static_assert(std::is_same_v<decltype(cat), stdext::unchecked_array_iterator<int*>>,
            "stdext::make_unchecked_array_iterator(p)'s return type is wrong!");


        auto dog = stdext::make_unchecked_array_iterator(p + 3);

        static_assert(std::is_same_v<decltype(dog), stdext::unchecked_array_iterator<int*>>,
            "stdext::make_unchecked_array_iterator(p + 3)'s return type is wrong!");


        static_assert(
            std::is_same_v<stdext::unchecked_array_iterator<int*>::iterator_category, std::random_access_iterator_tag>,
            "stdext::unchecked_array_iterator<int *>::iterator_category is wrong!");

        static_assert(std::is_same_v<stdext::unchecked_array_iterator<int*>::value_type, int>,
            "stdext::unchecked_array_iterator<int *>::value_type is wrong!");

        static_assert(std::is_same_v<stdext::unchecked_array_iterator<int*>::difference_type, ptrdiff_t>,
            "stdext::unchecked_array_iterator<int *>::difference_type is wrong!");

        static_assert(std::is_same_v<stdext::unchecked_array_iterator<int*>::pointer, int*>,
            "stdext::unchecked_array_iterator<int *>::pointer is wrong!");

        static_assert(std::is_same_v<stdext::unchecked_array_iterator<int*>::reference, int&>,
            "stdext::unchecked_array_iterator<int *>::reference is wrong!");


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
