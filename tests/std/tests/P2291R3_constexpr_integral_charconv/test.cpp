// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <charconv>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <limits>
#include <optional>
#include <span>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <utility>

using namespace std;

template <typename T>
constexpr void test_integer_to_chars(const T value, const optional<int> opt_base, const string_view correct) {
    const char* const correct_first = correct.data();
    const char* const correct_last  = correct_first + correct.size();

    T dest = 0;

    const from_chars_result from_res =
        (opt_base.has_value() ? from_chars(correct_first, correct_last, dest, opt_base.value())
                              : from_chars(correct_first, correct_last, dest));

    assert(from_res.ptr == correct_last);
    assert(from_res.ec == errc{});
    assert(dest == value);
}

// https://www.wolframalpha.com : Table[BaseForm[n * 2 - 1, n], {n, 2, 36}]
constexpr const char* output_max_digit[] = {"skip0", "skip1", "11", "12", "13", "14", "15", "16", "17", "18", "19",
    "1a", "1b", "1c", "1d", "1e", "1f", "1g", "1h", "1i", "1j", "1k", "1l", "1m", "1n", "1o", "1p", "1q", "1r", "1s",
    "1t", "1u", "1v", "1w", "1x", "1y", "1z"};

// https://www.wolframalpha.com : Table[BaseForm[k, n], {k, {MEOW, MEOW, MEOW}}, {n, 2, 36}]
constexpr uint64_t stress_chunks_positive                          = 12000000345000678900ULL;
constexpr pair<uint64_t, array<string_view, 37>> output_positive[] = {
    {123U, {{"skip0", "skip1", "1111011", "11120", "1323", "443", "323", "234", "173", "146", "123", "102", "a3", "96",
               "8b", "83", "7b", "74", "6f", "69", "63", "5i", "5d", "58", "53", "4n", "4j", "4f", "4b", "47", "43",
               "3u", "3r", "3o", "3l", "3i", "3f"}}},
    {uint64_t{INT8_MAX}, {{"skip0", "skip1", "1111111", "11201", "1333", "1002", "331", "241", "177", "151", "127",
                             "106", "a7", "9a", "91", "87", "7f", "78", "71", "6d", "67", "61", "5h", "5c", "57", "52",
                             "4n", "4j", "4f", "4b", "47", "43", "3v", "3s", "3p", "3m", "3j"}}},
    {161U, {{"skip0", "skip1", "10100001", "12222", "2201", "1121", "425", "320", "241", "188", "161", "137", "115",
               "c5", "b7", "ab", "a1", "98", "8h", "89", "81", "7e", "77", "70", "6h", "6b", "65", "5q", "5l", "5g",
               "5b", "56", "51", "4t", "4p", "4l", "4h"}}},
    {UINT8_MAX, {{"skip0", "skip1", "11111111", "100110", "3333", "2010", "1103", "513", "377", "313", "255", "212",
                    "193", "168", "143", "120", "ff", "f0", "e3", "d8", "cf", "c3", "bd", "b2", "af", "a5", "9l", "9c",
                    "93", "8n", "8f", "87", "7v", "7o", "7h", "7a", "73"}}},
    {1729U, {{"skip0", "skip1", "11011000001", "2101001", "123001", "23404", "12001", "5020", "3301", "2331", "1729",
                "1332", "1001", "a30", "8b7", "7a4", "6c1", "5gc", "561", "4f0", "469", "3j7", "3cd", "364", "301",
                "2j4", "2ed", "2a1", "25l", "21i", "1rj", "1oo", "1m1", "1jd", "1gt", "1ee", "1c1"}}},
    {uint64_t{INT16_MAX}, {{"skip0", "skip1", "111111111111111", "1122221121", "13333333", "2022032", "411411",
                              "164350", "77777", "48847", "32767", "22689", "16b67", "11bb7", "bd27", "9a97", "7fff",
                              "6b68", "5b27", "4eeb", "41i7", "3b67", "31f9", "2flf", "28l7", "22ah", "1mc7", "1hpg",
                              "1dm7", "19rq", "16c7", "1330", "vvv", "u2v", "sbp", "qq7", "pa7"}}},
    {57494U, {{"skip0", "skip1", "1110000010010110", "2220212102", "32002112", "3314434", "1122102", "326423", "160226",
                 "86772", "57494", "3a218", "29332", "20228", "16d4a", "1207e", "e096", "bbg0", "9f82", "8750", "73ee",
                 "647h", "58h8", "4gfh", "43je", "3goj", "3718", "2onb", "2h9a", "2aag", "23qe", "1spk", "1o4m", "1jq8",
                 "1fp0", "1bwo", "18d2"}}},
    {UINT16_MAX, {{"skip0", "skip1", "1111111111111111", "10022220020", "33333333", "4044120", "1223223", "362031",
                     "177777", "108806", "65535", "45268", "31b13", "23aa2", "19c51", "14640", "ffff", "d5d0", "b44f",
                     "9aa4", "83gf", "71cf", "638j", "58k8", "4hif", "44la", "3iof", "38o6", "2rgf", "2jqo", "2cof",
                     "2661", "1vvv", "1r5u", "1mnh", "1ihf", "1ekf"}}},
    {71125478U, {{"skip0", "skip1", "100001111010100100111100110", "11221211112210222", "10033110213212",
                    "121202003403", "11020244342", "1522361624", "417244746", "157745728", "71125478", "3716a696",
                    "1b9a06b2", "11973ba8", "9636514", "639e338", "43d49e6", "2g19gfb", "21b9d18", "19dec94", "124addi",
                    "h8f25b", "dhdfa6", "b13hg2", "8m91he", "7720j3", "5pgj58", "4pmelq", "43k17i", "3dg8ek", "2ro898",
                    "2f0et8", "23qif6", "1qw5lh", "1j7l7s", "1cdvli", "16cgrq"}}},
    {uint64_t{INT32_MAX},
        {{"skip0", "skip1", "1111111111111111111111111111111", "12112122212110202101", "1333333333333333",
            "13344223434042", "553032005531", "104134211161", "17777777777", "5478773671", "2147483647", "a02220281",
            "4bb2308a7", "282ba4aaa", "1652ca931", "c87e66b7", "7fffffff", "53g7f548", "3928g3h1", "27c57h32",
            "1db1f927", "140h2d91", "ikf5bf1", "ebelf95", "b5gge57", "8jmdnkm", "6oj8ion", "5ehncka", "4clm98f",
            "3hk7987", "2sb6cs7", "2d09uc1", "1vvvvvv", "1lsqtl1", "1d8xqrp", "15v22um", "zik0zj"}}},
    {3522553278ULL,
        {{"skip0", "skip1", "11010001111101011110010110111110", "100002111022020200020", "3101331132112332",
            "24203233201103", "1341312313010", "153202131426", "32175362676", "10074266606", "3522553278", "1548431462",
            "823842766", "441a34c6a", "255b8d486", "1593b4753", "d1f5e5be", "89ffb3b6", "5da3e606", "3hgbfb5i",
            "2f0fj33i", "1k1ac536", "191b46e2", "10i6fmk8", "ia967l6", "eahia63", "baca9ga", "92d86i6", "78iq4i6",
            "5qlc1dc", "4osos2i", "3u1862s", "38vbpdu", "2o0a7ro", "29hx9e6", "1w2dnod", "1m98ji6"}}},
    {UINT32_MAX,
        {{"skip0", "skip1", "11111111111111111111111111111111", "102002022201221111210", "3333333333333333",
            "32244002423140", "1550104015503", "211301422353", "37777777777", "12068657453", "4294967295", "1904440553",
            "9ba461593", "535a79888", "2ca5b7463", "1a20dcd80", "ffffffff", "a7ffda90", "704he7g3", "4f5aff65",
            "3723ai4f", "281d55i3", "1fj8b183", "1606k7ib", "mb994af", "hek2mgk", "dnchbnl", "b28jpdl", "8pfgih3",
            "76beigf", "5qmcpqf", "4q0jto3", "3vvvvvv", "3aokq93", "2qhxjlh", "2br45qa", "1z141z3"}}},
    {545890816626160ULL,
        {{"skip0", "skip1", "1111100000111110000011100001101100000110111110000", "2122120211122121121021010202111",
            "1330013300130031200313300", "1033022333343024014120", "5213002440142255104", "222661211220253465",
            "17407603415406760", "2576748547233674", "545890816626160", "148a34aa4706535", "51285369b87494",
            "1a57a38b045a95", "98b3383b9766c", "4319d1601875a", "1f07c1c360df0", "ffd471f34f13", "88g09ff9dh84",
            "4d0d5e232c53", "2d63h403i580", "1bf5h8185hdj", "kc3g550fkcg", "d41id5k9984", "8ef5n0him4g", "5i2dijfe1la",
            "3me22fm5fhi", "2hfmhgg73kd", "1ngpfabr53c", "18i7220bh11", "rm0lcjngpa", "kk1elesni1", "fgfge3c3fg",
            "bp4q5l6bjg", "8xna46jp0k", "6wejomvji5", "5di2s1qhv4"}}},
    {uint64_t{INT64_MAX},
        {{"skip0", "skip1", "111111111111111111111111111111111111111111111111111111111111111",
            "2021110011022210012102010021220101220221", "13333333333333333333333333333333",
            "1104332401304422434310311212", "1540241003031030222122211", "22341010611245052052300",
            "777777777777777777777", "67404283172107811827", "9223372036854775807", "1728002635214590697",
            "41a792678515120367", "10b269549075433c37", "4340724c6c71dc7a7", "160e2ad3246366807", "7fffffffffffffff",
            "33d3d8307b214008", "16agh595df825fa7", "ba643dci0ffeehh", "5cbfjia3fh26ja7", "2heiciiie82dh97",
            "1adaibb21dckfa7", "i6k448cf4192c2", "acd772jnc9l0l7", "64ie1focnn5g77", "3igoecjbmca687", "27c48l5b37oaop",
            "1bk39f3ah3dmq7", "q1se8f0m04isb", "hajppbc1fc207", "bm03i95hia437", "7vvvvvvvvvvvv", "5hg4ck9jd4u37",
            "3tdtk1v8j6tpp", "2pijmikexrxp7", "1y2p0ij32e8e7"}}},
    {stress_chunks_positive,
        {{"skip0", "skip1", "1010011010001000100100001011110000101100010101001001010111110100",
            "2221221122020020011022001202200200202200", "22122020210023300230111021113310",
            "1301130403021123030133211100", "2311004450342244200504500", "30325064311430214266301",
            "1232104413605425112764", "87848206138052620680", "12000000345000678900", "2181782a1686924456a",
            "54aa47a9058877b130", "150593a5b002c87b16", "571cad2b93c7760a8", "1c60d2676d4e53e00", "a68890bc2c5495f4",
            "43499224707a4f4g", "1e052gdga1d26f40", "f06dh4g564c8a91", "769df0d9ace4h50", "3ee7bcj1ajghi4f",
            "1k9agc4gfl0l43a", "10id7dakdlcjd22", "dge08fe0l5hl7c", "8184326d31ib60", "4ljbglf3cpim76",
            "2pph66481kiiki", "1niph2ao132e58", "14qgbgk3c3iffg", "mhc35an1bhb00", "f78o8ur705ln5", "ad24gngm595fk",
            "76e1n5i5v0ivl", "50wu8jsnks82g", "3ja41smfvqb1f", "2j64t3qgq0ut0"}}},
    {14454900944617508688ULL,
        {{"skip0", "skip1", "1100100010011010000111111101001011100011011000101000111101010000",
            "10120022020112011211121221212101012220210", "30202122013331023203120220331100",
            "1432224030234034034040234223", "3014532424232535441404120", "34610451042001242144165",
            "1442320775134330507520", "116266464747855335823", "14454900944617508688", "266642a9a9471339935",
            "662251403263939640", "1895280092bc310481", "68cb9c8292557406c", "23023deab20002893", "c89a1fd2e3628f50",
            "50e7147a7db8ef84", "22a34a05086f78ec", "i1dgef04357g7i1", "8g90b882jcj8be8", "49c1kk35i0k24ic",
            "272a16i54ebkacg", "15fdih7l3m7k8md", "gbj7303eg9nge0", "9hckfdkj3kkdmd", "5lc7hifdkl4nne",
            "3f86e4mgpna5ol", "266pj428na273c", "1bomgjbnlg4m3f", "r5tf1f7f009ji", "iarsig29iqhhm", "ch6gvqbhm53qg",
            "8lwtvcdj6rlqr", "61w23lajggp44", "49p1f3dsqqcdx", "31tkqqkxypopc"}}},
    {UINT64_MAX,
        {{"skip0", "skip1", "1111111111111111111111111111111111111111111111111111111111111111",
            "11112220022122120101211020120210210211220", "33333333333333333333333333333333",
            "2214220303114400424121122430", "3520522010102100444244423", "45012021522523134134601",
            "1777777777777777777777", "145808576354216723756", "18446744073709551615", "335500516a429071284",
            "839365134a2a240713", "219505a9511a867b72", "8681049adb03db171", "2c1d56b648c6cd110", "ffffffffffffffff",
            "67979g60f5428010", "2d3fgb0b9cg4bd2f", "141c8786h1ccaagg", "b53bjh07be4dj0f", "5e8g4ggg7g56dif",
            "2l4lf104353j8kf", "1ddh88h2782i515", "l12ee5fn0ji1if", "c9c336o0mlb7ef", "7b7n2pcniokcgf",
            "4eo8hfam6fllmo", "2nc6j26l66rhof", "1n3rsh11f098rn", "14l9lkmo30o40f", "nd075ib45k86f", "fvvvvvvvvvvvv",
            "b1w8p7j5q9r6f", "7orp63sh4dphh", "5g24a25twkwff", "3w5e11264sgsf"}}},
};

// https://www.wolframalpha.com : Table[BaseForm[k, n], {k, {MEOW, MEOW, MEOW}}, {n, 2, 36}]
constexpr int64_t stress_chunks_negative                          = -9000876000000054321LL;
constexpr pair<int64_t, array<string_view, 37>> output_negative[] = {
    {-85, {{"skip0", "skip1", "-1010101", "-10011", "-1111", "-320", "-221", "-151", "-125", "-104", "-85", "-78",
              "-71", "-67", "-61", "-5a", "-55", "-50", "-4d", "-49", "-45", "-41", "-3j", "-3g", "-3d", "-3a", "-37",
              "-34", "-31", "-2r", "-2p", "-2n", "-2l", "-2j", "-2h", "-2f", "-2d"}}},
    {INT8_MIN, {{"skip0", "skip1", "-10000000", "-11202", "-2000", "-1003", "-332", "-242", "-200", "-152", "-128",
                   "-107", "-a8", "-9b", "-92", "-88", "-80", "-79", "-72", "-6e", "-68", "-62", "-5i", "-5d", "-58",
                   "-53", "-4o", "-4k", "-4g", "-4c", "-48", "-44", "-40", "-3t", "-3q", "-3n", "-3k"}}},
    {-1591, {{"skip0", "skip1", "-11000110111", "-2011221", "-120313", "-22331", "-11211", "-4432", "-3067", "-2157",
                "-1591", "-1217", "-b07", "-955", "-819", "-711", "-637", "-58a", "-4g7", "-47e", "-3jb", "-3cg",
                "-367", "-304", "-2i7", "-2dg", "-295", "-24p", "-20n", "-1pp", "-1n1", "-1ka", "-1hn", "-1f7", "-1cr",
                "-1ag", "-187"}}},
    {INT16_MIN, {{"skip0", "skip1", "-1000000000000000", "-1122221122", "-20000000", "-2022033", "-411412", "-164351",
                    "-100000", "-48848", "-32768", "-2268a", "-16b68", "-11bb8", "-bd28", "-9a98", "-8000", "-6b69",
                    "-5b28", "-4eec", "-41i8", "-3b68", "-31fa", "-2flg", "-28l8", "-22ai", "-1mc8", "-1hph", "-1dm8",
                    "-19rr", "-16c8", "-1331", "-1000", "-u2w", "-sbq", "-qq8", "-pa8"}}},
    {-66748412,
        {{"skip0", "skip1", "-11111110100111111111111100", "-11122121011121102", "-3332213333330", "-114041422122",
            "-10342352232", "-1440231533", "-376477774", "-148534542", "-66748412", "-34750085", "-1a42b678",
            "-10aa0803", "-8c1731a", "-5cd7492", "-3fa7ffc", "-2d03163", "-1h5f3b2", "-17i39c6", "-10h3b0c", "-g749jh",
            "-ckkdkg", "-a8c0ak", "-894afk", "-6klmbc", "-5g1i6g", "-4hg4gb", "-3ogi7o", "-37anqb", "-2mc4r2",
            "-2a8h7i", "-1vkvvs", "-1n9ca5", "-1fw8sk", "-19gshh", "-13qnek"}}},
    {INT32_MIN, {{"skip0", "skip1", "-10000000000000000000000000000000", "-12112122212110202102", "-2000000000000000",
                    "-13344223434043", "-553032005532", "-104134211162", "-20000000000", "-5478773672", "-2147483648",
                    "-a02220282", "-4bb2308a8", "-282ba4aab", "-1652ca932", "-c87e66b8", "-80000000", "-53g7f549",
                    "-3928g3h2", "-27c57h33", "-1db1f928", "-140h2d92", "-ikf5bf2", "-ebelf96", "-b5gge58", "-8jmdnkn",
                    "-6oj8ioo", "-5ehnckb", "-4clm98g", "-3hk7988", "-2sb6cs8", "-2d09uc2", "-2000000", "-1lsqtl2",
                    "-1d8xqrq", "-15v22un", "-zik0zk"}}},
    {-297139747082649553LL,
        {{"skip0", "skip1", "-10000011111101001110000011010010001100000101011111111010001",
            "-1222110012002112101210012211022102101", "-100133221300122101200223333101", "-4443033200104011124241203",
            "-21313431255203203120401", "-350320603201030412545", "-20375160322140537721", "-1873162471705738371",
            "-297139747082649553", "-65150976074a24025", "-173522497b5373101", "-5a60a99bc3b71654", "-1ca51a06cc38ba25",
            "-a2a25babe62241d", "-41fa7069182bfd1", "-1d00134fba1769g", "-e4f799fc5f7e81", "-714ebbh8388188",
            "-3cahb17836b3hd", "-1j8659jf5hbg3j", "-112bbb2jege5c5", "-dcjfmk2kjb4cc", "-836bm4klbgl61",
            "-4ofia1416ee73", "-32ommgjef1l2h", "-1qc52eal5m8ba", "-17n53r05a4r15", "-oa88m2qiqjik", "-gn67qoat5r8d",
            "-blgd6n5s90al", "-87t70q8o5fuh", "-5t09hwaqu9qg", "-47vssihaoa4x", "-32p24fbjye7x", "-299r8zck3841"}}},
    {stress_chunks_negative,
        {{"skip0", "skip1", "-111110011101001100010010000100010000111010101111001010000110001",
            "-2012222010200021010000112111002001111200", "-13303221202100202013111321100301",
            "-1101001100304341000003214241", "-1522150121302454031001413", "-22054250360123016161454",
            "-763514220420725712061", "-65863607100474061450", "-9000876000000054321", "-1689813530958833498",
            "-408258185a67069269", "-106b01597a47ba2948", "-41c02922bc776d49b", "-1584cd10979dc84b6",
            "-7ce9890887579431", "-327cf6cbc67023c3", "-1604b5f6a0de8129", "-b50d3ef02f124a4", "-59h9bfif0006fg1",
            "-2g5d8ekh05d2dfi", "-19i418c38g1chfj", "-hjgf7d0k0gla9a", "-a6b21ncehfa3f9", "-61060fnl003bml",
            "-3g88bakondgf8l", "-25q3i730ed21di", "-1al84glo518iip", "-pcli8ig7pjhbo", "-gs31q8id2jnkl",
            "-bd7kaglgdrbgk", "-7pqc9123lf51h", "-5d2sd1r5ms7su", "-3q833s8kdrun3", "-2n7vmqigfueqb",
            "-1wdu892toj0a9"}}},
    {INT64_MIN, {{"skip0", "skip1", "-1000000000000000000000000000000000000000000000000000000000000000",
                    "-2021110011022210012102010021220101220222", "-20000000000000000000000000000000",
                    "-1104332401304422434310311213", "-1540241003031030222122212", "-22341010611245052052301",
                    "-1000000000000000000000", "-67404283172107811828", "-9223372036854775808", "-1728002635214590698",
                    "-41a792678515120368", "-10b269549075433c38", "-4340724c6c71dc7a8", "-160e2ad3246366808",
                    "-8000000000000000", "-33d3d8307b214009", "-16agh595df825fa8", "-ba643dci0ffeehi",
                    "-5cbfjia3fh26ja8", "-2heiciiie82dh98", "-1adaibb21dckfa8", "-i6k448cf4192c3", "-acd772jnc9l0l8",
                    "-64ie1focnn5g78", "-3igoecjbmca688", "-27c48l5b37oaoq", "-1bk39f3ah3dmq8", "-q1se8f0m04isc",
                    "-hajppbc1fc208", "-bm03i95hia438", "-8000000000000", "-5hg4ck9jd4u38", "-3tdtk1v8j6tpq",
                    "-2pijmikexrxp8", "-1y2p0ij32e8e8"}}},
};

template <typename T>
constexpr void test_integer_to_chars() {
    for (int base = 2; base <= 36; ++base) {
        test_integer_to_chars(static_cast<T>(0), base, "0"sv);
        test_integer_to_chars(static_cast<T>(1), base, "1"sv);

        // tests [3, 71]
        test_integer_to_chars(static_cast<T>(base * 2 - 1), base, output_max_digit[base]);

        for (const auto& p : output_positive) {
            if (p.first <= static_cast<uint64_t>(numeric_limits<T>::max())) {
                test_integer_to_chars(static_cast<T>(p.first), base, p.second[static_cast<size_t>(base)]);
            }
        }

        if constexpr (is_signed_v<T>) {
            test_integer_to_chars(static_cast<T>(-1), base, "-1"sv);

            for (const auto& p : output_negative) {
                if (p.first >= static_cast<int64_t>(numeric_limits<T>::min())) {
                    test_integer_to_chars(static_cast<T>(p.first), base, p.second[static_cast<size_t>(base)]);
                }
            }
        }
    }

    test_integer_to_chars(static_cast<T>(42), nullopt, "42"sv);
}

template <typename T, typename BaseOrFmt>
constexpr void test_from_chars(const string_view input, const BaseOrFmt base_or_fmt, const size_t correct_idx,
    const errc correct_ec, const optional<T> opt_correct = nullopt) {

    constexpr T unmodified = 111;

    T dest = unmodified;

    const from_chars_result result = from_chars(input.data(), input.data() + input.size(), dest, base_or_fmt);

    assert(result.ptr == input.data() + correct_idx);
    assert(result.ec == correct_ec);

    if (correct_ec == errc{}) {
        assert(opt_correct.has_value());
        assert(dest == opt_correct.value());
    } else {
        assert(!opt_correct.has_value());
        assert(dest == unmodified);
    }
}

constexpr errc inv_arg = errc::invalid_argument;
constexpr errc out_ran = errc::result_out_of_range;

constexpr auto hundred_ones = []() {
    array<char, 100> result;
    fill(result.begin(), result.end(), '1');
    return result;
}();

constexpr auto hundred_ones_and_atatat = []() {
    array<char, 103> result;
    fill(result.begin(), result.end(), '1');
    result[100] = '@';
    result[101] = '@';
    result[102] = '@';
    return result;
}();

constexpr auto minus_hundred_ones = []() {
    array<char, 101> result;
    fill(result.begin(), result.end(), '1');
    result[0] = '-';
    return result;
}();

constexpr auto minus_hundred_ones_and_atatat = []() {
    array<char, 104> result;
    fill(result.begin(), result.end(), '1');
    result[0]   = '-';
    result[101] = '@';
    result[102] = '@';
    result[103] = '@';
    return result;
}();

template <size_t N>
constexpr string_view array_to_sv(const array<char, N>& arr) {
    return string_view(arr.data(), arr.size());
}

template <typename T>
constexpr void test_integer_from_chars() {
    for (int base = 2; base <= 36; ++base) {
        test_from_chars<T>(""sv, base, 0, inv_arg); // no characters
        test_from_chars<T>("@1"sv, base, 0, inv_arg); // '@' is bogus
        test_from_chars<T>(".1"sv, base, 0, inv_arg); // '.' is bogus, for integers
        test_from_chars<T>("+1"sv, base, 0, inv_arg); // '+' is bogus, N4713 23.20.3 [charconv.from.chars]/3
                                                      // "a minus sign is the only sign that may appear"
        test_from_chars<T>(" 1"sv, base, 0, inv_arg); // ' ' is bogus, no whitespace in subject sequence

        if constexpr (is_unsigned_v<T>) { // N4713 23.20.3 [charconv.from.chars]/3
            test_from_chars<T>("-1"sv, base, 0, inv_arg); // "and only if value has a signed type"
        }

        // N4713 23.20.3 [charconv.from.chars]/1 "[ Note: If the pattern allows for an optional sign,
        // but the string has no digit characters following the sign, no characters match the pattern. -end note ]"
        test_from_chars<T>("-"sv, base, 0, inv_arg); // '-' followed by no characters
        test_from_chars<T>("-@1"sv, base, 0, inv_arg); // '-' followed by bogus '@'
        test_from_chars<T>("-.1"sv, base, 0, inv_arg); // '-' followed by bogus '.'
        test_from_chars<T>("-+1"sv, base, 0, inv_arg); // '-' followed by bogus '+'
        test_from_chars<T>("- 1"sv, base, 0, inv_arg); // '-' followed by bogus ' '
        test_from_chars<T>("--1"sv, base, 0, inv_arg); // '-' can't be repeated

        const array<char, 3> bogus_digits1 = {static_cast<char>('0' + base), 'A', 'a'};
        const array<char, 2> bogus_digits2 = {
            static_cast<char>('A' + (base - 10)), static_cast<char>('a' + (base - 10))};
        span<const char> bogus_digits = base < 10 ? span<const char>(bogus_digits1) : span<const char>(bogus_digits2);

        for (const auto& bogus : bogus_digits) {
            const char str1[] = {bogus, '1'};
            const char str2[] = {'-', bogus, '1'};
            test_from_chars<T>(string_view(str1, 2), base, 0, inv_arg); // bogus digit (for this base)
            test_from_chars<T>(string_view(str2, 3), base, 0, inv_arg); // '-' followed by bogus digit
        }

        // Test leading zeroes.
        test_from_chars<T>("0000000000"sv, base, 10, errc{}, static_cast<T>(0));
        test_from_chars<T>("000000000011"sv, base, 12, errc{}, static_cast<T>(base + 1));

        // Test negative zero and negative leading zeroes.
        if constexpr (is_signed_v<T>) {
            test_from_chars<T>("-0"sv, base, 2, errc{}, static_cast<T>(0));
            test_from_chars<T>("-0000000000"sv, base, 11, errc{}, static_cast<T>(0));
            test_from_chars<T>("-000000000011"sv, base, 13, errc{}, static_cast<T>(-base - 1));
        }

        // N4713 23.20.3 [charconv.from.chars]/1 "The member ptr of the return value points to the
        // first character not matching the pattern, or has the value last if all characters match."
        test_from_chars<T>("11"sv, base, 2, errc{}, static_cast<T>(base + 1));
        test_from_chars<T>("11@@@"sv, base, 2, errc{}, static_cast<T>(base + 1));

        // When overflowing, we need to keep consuming valid digits, in order to return ptr correctly.
        // This is really expensive so only test a few bases
        if (base == 2 || base == 10 || base == 16 || base == 27 || base == 36) {
            test_from_chars<T>(array_to_sv(hundred_ones), base, 100, out_ran);
            test_from_chars<T>(array_to_sv(hundred_ones_and_atatat), base, 100, out_ran);

            if constexpr (is_signed_v<T>) {
                test_from_chars<T>(array_to_sv(minus_hundred_ones), base, 101, out_ran);
                test_from_chars<T>(array_to_sv(minus_hundred_ones_and_atatat), base, 101, out_ran);
            }
        }
    }

    // N4713 23.20.3 [charconv.from.chars]/3 "The pattern is the expected form of the subject sequence
    // in the "C" locale for the given nonzero base, as described for strtol"
    // C11 7.22.1.4/3 "The letters from a (or A) through z (or Z) are ascribed the values 10 through 35"
    for (int i = 0; i < 26; ++i) {
        const char str1[] = {static_cast<char>('A' + i)};
        const char str2[] = {static_cast<char>('a' + i)};
        test_from_chars<T>(string_view(str1, 1), 36, 1, errc{}, static_cast<T>(10 + i));
        test_from_chars<T>(string_view(str2, 1), 36, 1, errc{}, static_cast<T>(10 + i));
    }

    // N4713 23.20.3 [charconv.from.chars]/3 "no "0x" or "0X" prefix shall appear if the value of base is 16"
    test_from_chars<T>("0x1729"sv, 16, 1, errc{}, static_cast<T>(0)); // reads '0', stops at 'x'
    test_from_chars<T>("0X1729"sv, 16, 1, errc{}, static_cast<T>(0)); // reads '0', stops at 'X'

    if constexpr (is_signed_v<T>) {
        test_from_chars<T>("-0x1729"sv, 16, 2, errc{}, static_cast<T>(0)); // reads "-0", stops at 'x'
        test_from_chars<T>("-0X1729"sv, 16, 2, errc{}, static_cast<T>(0)); // reads "-0", stops at 'X'
    }
}

template <typename T>
constexpr void test_integer() {
    test_integer_to_chars<T>();
    test_integer_from_chars<T>();
}

constexpr bool all_integer_tests() {
#if TEST_PART == 0
    test_integer<char>();
    test_integer<signed char>();
    test_integer<unsigned char>();
    test_integer<short>();
    test_integer<unsigned short>();
    test_integer<int>();
#else // ^^^ TEST_PART == 0 ^^^ / vvv TEST_PART == 1 vvv
    test_integer<unsigned int>();
    test_integer<long>();
    test_integer<unsigned long>();
    test_integer<long long>();
    test_integer<unsigned long long>();

    // Test overflow scenarios.
    test_from_chars<unsigned int>("4294967289"sv, 10, 10, errc{}, 4294967289U); // not risky
    test_from_chars<unsigned int>("4294967294"sv, 10, 10, errc{}, 4294967294U); // risky with good digit
    test_from_chars<unsigned int>("4294967295"sv, 10, 10, errc{}, 4294967295U); // risky with max digit
    test_from_chars<unsigned int>("4294967296"sv, 10, 10, out_ran); // risky with bad digit
    test_from_chars<unsigned int>("4294967300"sv, 10, 10, out_ran); // beyond risky

    test_from_chars<int>("2147483639"sv, 10, 10, errc{}, 2147483639); // not risky
    test_from_chars<int>("2147483646"sv, 10, 10, errc{}, 2147483646); // risky with good digit
    test_from_chars<int>("2147483647"sv, 10, 10, errc{}, 2147483647); // risky with max digit
    test_from_chars<int>("2147483648"sv, 10, 10, out_ran); // risky with bad digit
    test_from_chars<int>("2147483650"sv, 10, 10, out_ran); // beyond risky

    test_from_chars<int>("-2147483639"sv, 10, 11, errc{}, -2147483639); // not risky
    test_from_chars<int>("-2147483647"sv, 10, 11, errc{}, -2147483647); // risky with good digit
    test_from_chars<int>("-2147483648"sv, 10, 11, errc{}, -2147483647 - 1); // risky with max digit
    test_from_chars<int>("-2147483649"sv, 10, 11, out_ran); // risky with bad digit
    test_from_chars<int>("-2147483650"sv, 10, 11, out_ran); // beyond risky
#endif // ^^^ TEST_PART == 1 ^^^

    return true;
}

int main() {
    all_integer_tests();
    static_assert(all_integer_tests());
}
