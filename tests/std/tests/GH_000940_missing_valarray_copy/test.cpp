#include <cassert>
#include <iostream>
#include <valarray>

int main() {
    std::valarray<int> v({1, 2, 3, 4, 5});

    std::slice_array<int> slice_array      = v[std::slice(2, 2, 2)];
    std::slice_array<int> slice_array_copy = slice_array;

    std::slice_array<int> other_array_slice = v[std::slice(1, 1, 1)];
    other_array_slice                       = slice_array;

    std::gslice gslice(1, std::valarray<std::size_t>{2, 1}, std::valarray<std::size_t>{1, 2});
    std::gslice_array<int> gslice_array      = v[gslice];
    std::gslice_array<int> gslice_array_copy = gslice_array;

    std::gslice other_gslice(1, std::valarray<std::size_t>{1, 1}, std::valarray<std::size_t>{1, 1});
    std::gslice_array<int> other_gslice_array = v[other_gslice];
    other_gslice_array                        = gslice_array;

    std::valarray<bool> mask({true, false, false, true, true});
    std::mask_array<int> mask_array      = v[mask];
    std::mask_array<int> mask_array_copy = mask_array;

    std::valarray<std::size_t> indices({2, 4});
    std::indirect_array<int> indirect_array      = v[indices];
    std::indirect_array<int> indirect_array_copy = indirect_array;

    std::valarray<std::size_t> other_indices({2, 4});
    std::indirect_array<int> other_indirect_array = v[other_indices];
    other_indirect_array                          = indirect_array;

    return 0;
}
