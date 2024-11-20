// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <random>
#include <ranges>
#include <vector>

// clang-format off
constexpr int tukey_ninther_adversary[] = {
    0, 6, 12, 18, 22, 28, 34, 38, 44, 50, 54, 60, 66, 70, 76, 82, 86, 92, 98, 102, 108,
    114, 118, 124, 130, 134, 140, 146, 150, 156, 162, 166, 172, 178, 182, 188, 194, 198,
    204, 210, 214, 220, 226, 230, 236, 242, 246, 252, 258, 262, 268, 274, 278, 284, 290,
    294, 300, 306, 310, 316, 322, 326, 332, 338, 342, 348, 354, 358, 364, 370, 374, 380,
    386, 390, 396, 402, 406, 412, 418, 422, 428, 434, 438, 444, 450, 454, 460, 466, 470,
    476, 482, 486, 492, 498, 502, 508, 514, 518, 524, 530, 534, 540, 546, 550, 556, 562,
    566, 572, 578, 582, 588, 594, 598, 604, 610, 614, 620, 626, 630, 636, 642, 646, 652,
    658, 662, 668, 674, 678, 1, 7, 13, 684, 19, 23, 29, 690, 35, 39, 45, 694, 51, 55, 61,
    700, 67, 71, 77, 706, 83, 87, 93, 710, 99, 103, 109, 716, 115, 119, 125, 722, 131, 135,
    141, 726, 147, 151, 157, 732, 163, 167, 173, 738, 179, 183, 189, 742, 195, 199, 205,
    748, 211, 215, 221, 754, 227, 231, 237, 758, 243, 247, 253, 764, 259, 263, 269, 770,
    275, 279, 285, 774, 291, 295, 301, 780, 307, 311, 317, 786, 323, 327, 333, 790, 339,
    343, 349, 796, 355, 359, 365, 802, 371, 375, 381, 806, 387, 391, 397, 812, 403, 407,
    413, 818, 419, 423, 429, 822, 435, 439, 445, 828, 451, 455, 461, 834, 467, 471, 477,
    838, 483, 487, 493, 844, 499, 503, 509, 850, 515, 519, 525, 854, 531, 535, 541, 860,
    547, 551, 557, 866, 563, 567, 573, 870, 579, 583, 589, 876, 595, 599, 605, 882, 611,
    615, 621, 886, 627, 631, 637, 892, 643, 647, 653, 898, 659, 663, 669, 902, 675, 679,
    685, 908, 691, 695, 701, 914, 707, 711, 717, 918, 723, 727, 733, 924, 739, 743, 749,
    930, 755, 759, 765, 934, 771, 775, 781, 940, 787, 791, 797, 946, 803, 807, 813, 950,
    819, 823, 829, 956, 835, 839, 845, 962, 851, 855, 861, 966, 867, 871, 877, 972, 883,
    887, 893, 978, 899, 903, 909, 982, 915, 919, 925, 988, 931, 935, 941, 990, 947, 951,
    957, 992, 963, 967, 973, 1024, 979, 983, 993, 994, 995, 996, 997, 998, 999, 1000, 1001,
    1002, 2, 20, 8, 14, 24, 36, 30, 40, 52, 46, 56, 68, 62, 72, 84, 78, 88, 100, 94, 104,
    116, 110, 120, 132, 126, 136, 148, 142, 152, 164, 158, 168, 180, 174, 184, 196, 190,
    200, 212, 206, 216, 228, 222, 232, 244, 238, 248, 260, 254, 264, 276, 270, 280, 292,
    286, 296, 308, 302, 312, 324, 318, 328, 340, 334, 344, 356, 350, 360, 372, 366, 376,
    388, 382, 392, 404, 398, 408, 420, 414, 424, 436, 430, 440, 452, 446, 456, 468, 462,
    472, 484, 478, 488, 500, 494, 504, 516, 510, 520, 532, 526, 536, 548, 542, 552, 564,
    558, 568, 580, 574, 584, 596, 590, 600, 612, 606, 616, 628, 622, 632, 644, 638, 648,
    660, 654, 664, 676, 670, 512, 3, 991, 9, 773, 768, 15, 680, 901, 21, 522, 25, 777, 527,
    31, 692, 965, 37, 528, 41, 960, 778, 47, 686, 905, 53, 538, 57, 936, 543, 63, 696, 1007,
    69, 544, 73, 789, 784, 79, 708, 1014, 85, 554, 89, 793, 559, 95, 702, 969, 101, 560,
    105, 911, 794, 111, 712, 974, 117, 570, 121, 948, 575, 127, 724, 1015, 133, 576, 137,
    805, 800, 143, 718, 917, 149, 586, 153, 809, 591, 159, 728, 1008, 165, 592, 169, 970,
    810, 175, 740, 921, 181, 602, 185, 942, 607, 191, 734, 1009, 197, 608, 201, 821, 816,
    207, 744, 975, 213, 618, 217, 825, 623, 223, 756, 1005, 229, 624, 233, 927, 826, 239,
    750, 984, 245, 634, 249, 952, 639, 255, 760, 1010, 261, 640, 265, 837, 832, 271, 772,
    933, 277, 650, 281, 841, 655, 287, 766, 981, 293, 656, 297, 976, 842, 303, 776, 937, 309,
    666, 313, 964, 671, 319, 788, 1011, 325, 672, 329, 853, 848, 335, 782, 1016, 341, 682,
    345, 857, 687, 351, 792, 985, 357, 688, 361, 943, 858, 367, 804, 1003, 373, 698, 377,
    958, 703, 383, 798, 1017, 389, 704, 393, 869, 864, 399, 808, 949, 405, 714, 409, 873,
    719, 415, 820, 1012, 421, 720, 425, 986, 874, 431, 814, 953, 437, 730, 441, 968, 735,
    447, 824, 1013, 453, 736, 457, 885, 880, 463, 836, 989, 469, 746, 473, 889, 751, 479,
    830, 1006, 485, 752, 489, 959, 890, 495, 840, 1004, 501, 762, 505, 980, 767, 511, 852, 4,
    517, 10, 521, 16, 896, 26, 846, 32, 533, 42, 537, 48, 783, 58, 856, 64, 549, 74, 553, 80,
    906, 90, 868, 96, 565, 106, 569, 112, 799, 122, 862, 128, 581, 138, 585, 144, 912, 154,
    72, 160, 597, 170, 601, 176, 815, 186, 884, 192, 613, 202, 617, 208, 922, 218, 878, 224,
    629, 234, 633, 240, 831, 250, 888, 256, 645, 266, 649, 272, 928, 282, 900, 288, 661, 298,
    665, 304, 847, 314, 894, 320, 677, 330, 681, 336, 938, 346, 904, 352, 693, 362, 697, 368,
    863, 378, 916, 384, 709, 394, 713, 400, 944, 410, 910, 416, 725, 426, 729, 432, 879, 442,
    920, 448, 741, 458, 745, 464, 954, 474, 932, 480, 757, 490, 761, 496, 895, 506, 926, 5,
    11, 17, 27, 33, 43, 49, 59, 65, 75, 81, 91, 97, 107, 113, 123, 129, 139, 145, 155, 161,
    71, 177, 187, 193, 203, 209, 219, 225, 235, 241, 251, 257, 267, 273, 283, 289, 299, 305,
    315, 321, 331, 337, 347, 353, 363, 369, 379, 385, 395, 401, 411, 417, 427, 433, 443, 449,
    459, 465, 475, 481, 491, 497, 507, 513, 523, 529, 539, 545, 555, 561, 571, 577, 587, 593,
    603, 609, 619, 625, 635, 641, 651, 657, 667, 673, 683, 689, 699, 705, 715, 721, 731, 737,
    747, 753, 763, 769, 779, 785, 795, 801, 811, 817, 827, 833, 843, 849, 859, 865, 875, 881,
    891, 897, 907, 913, 923, 929, 939, 945, 955, 961, 971, 977, 987, 1018, 1019, 1020, 1021, 1022, };
// clang-format on

using namespace std;

enum class alg_type { std_fn, rng };

template <alg_type Type>
void bm_uniform(benchmark::State& state) {
    vector<int> src(static_cast<size_t>(state.range()));
    mt19937 gen(84710);
    uniform_int_distribution<int> dis(1, 580);
    ranges::generate(src, [&] { return dis(gen); });

    vector<int> v;
    v.reserve(src.size());

    for (auto _ : state) {
        v = src;
        benchmark::DoNotOptimize(v);
        if constexpr (Type == alg_type::std_fn) {
            nth_element(v.begin(), v.begin() + (v.size() / 2), v.end());
        } else {
            ranges::nth_element(v.begin(), v.begin() + (v.size() / 2), v.end());
        }
        benchmark::DoNotOptimize(*(v.begin() + (v.size() / 2)));
    }
}

template <alg_type Type>
void bm_tukey_adversary(benchmark::State& state) {
    vector<int> src(begin(tukey_ninther_adversary), end(tukey_ninther_adversary));

    vector<int> v;
    v.reserve(src.size());

    for (auto _ : state) {
        v = src;
        benchmark::DoNotOptimize(v);
        if constexpr (Type == alg_type::std_fn) {
            nth_element(v.begin(), v.begin() + (v.size() >> 1) + (v.size() >> 2), v.end());
        } else {
            ranges::nth_element(v.begin(), v.begin() + (v.size() >> 1) + (v.size() >> 2), v.end());
        }
        benchmark::DoNotOptimize(*(v.begin() + (v.size() >> 1) + (v.size() >> 2)));
    }
}

BENCHMARK(bm_uniform<alg_type::std_fn>)->Arg(1024)->Arg(2048)->Arg(4096)->Arg(8192);
BENCHMARK(bm_uniform<alg_type::rng>)->Arg(1024)->Arg(2048)->Arg(4096)->Arg(8192);

BENCHMARK(bm_tukey_adversary<alg_type::std_fn>);
BENCHMARK(bm_tukey_adversary<alg_type::rng>);

BENCHMARK_MAIN();
