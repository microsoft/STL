// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#include <cassert>
#include <functional>
#include <hash_map>
#include <hash_set>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

bool g_prohibit_allocations = false;

class prohibit_allocations {
public:
    explicit prohibit_allocations(const bool value) : previous(g_prohibit_allocations) {
        g_prohibit_allocations = value;
    }

    prohibit_allocations(const prohibit_allocations&)            = delete;
    prohibit_allocations& operator=(const prohibit_allocations&) = delete;

    ~prohibit_allocations() {
        g_prohibit_allocations = previous;
    }

private:
    const bool previous;
};

template <typename T>
struct test_allocator {
    using value_type = T;

    test_allocator() = default;
    template <typename U>
    test_allocator(const test_allocator<U>&) {}
    test_allocator(const test_allocator&)            = default;
    test_allocator& operator=(const test_allocator&) = default;

    T* allocate(const size_t n) {
        assert(!g_prohibit_allocations);
        return allocator<T>{}.allocate(n);
    }

    void deallocate(T* const p, const size_t n) {
        allocator<T>{}.deallocate(p, n);
    }

    template <typename U>
    bool operator==(const test_allocator<U>&) const noexcept {
        return true;
    }

    template <typename U>
    bool operator!=(const test_allocator<U>&) const noexcept {
        return false;
    }
};

struct transparent_string_equal {
    using is_transparent = void;
    bool operator()(const string_view lhs, const string_view rhs) const noexcept {
        return lhs == rhs;
    }
};

struct transparent_string_hasher {
    using is_transparent = void;
    size_t operator()(const string_view target) const noexcept {
        return hash<string_view>{}(target);
    }
};

struct string_legacy_traits {
    enum { bucket_size = 1 };

    bool operator()(const string_view lhs, const string_view rhs) const noexcept {
        return lhs < rhs;
    }

    size_t operator()(const string_view target) const noexcept {
        return hash<string_view>{}(target);
    }
};

using test_str = basic_string<char, char_traits<char>, test_allocator<char>>;

STATIC_ASSERT(is_same_v<unordered_set<size_t, hash<size_t>, equal_to<size_t>>::key_equal, equal_to<size_t>>);
STATIC_ASSERT(is_same_v<unordered_map<size_t, int, hash<size_t>, equal_to<size_t>>::key_equal, equal_to<size_t>>);
STATIC_ASSERT(is_same_v<unordered_set<size_t, hash<size_t>, equal_to<>>::key_equal, equal_to<>>);
STATIC_ASSERT(is_same_v<unordered_map<size_t, int, hash<size_t>, equal_to<>>::key_equal, equal_to<>>);

STATIC_ASSERT(is_same_v<unordered_set<string, transparent_string_hasher>::key_equal, equal_to<string>>);
STATIC_ASSERT(is_same_v<unordered_map<string, int, transparent_string_hasher>::key_equal, equal_to<string>>);
STATIC_ASSERT(is_same_v<unordered_set<string, transparent_string_hasher, transparent_string_equal>::key_equal,
    transparent_string_equal>);
STATIC_ASSERT(is_same_v<unordered_map<string, int, transparent_string_hasher, transparent_string_equal>::key_equal,
    transparent_string_equal>);

constexpr string_view testStrings[] = {
    // example inputs from the ordered container transparent comparisons
    "ape", "bat", "cat", "bear", "wolf", "raven", "giraffe", "panther", "unicorn", "direwolf", "elephant",

    // 1000 values generated by default-constructed mt19937
    "D091BB5C", "22AE9EF6", "E7E1FAEE", "D5C31F79", "2082352C", "F807B7DF", "E9D30005", "3895AFE1", "A1E24BBA",
    "4EE4092B", "18F86863", "8C16A625", "474BA8C4", "3039CD1A", "8C006D5F", "FE2D7810", "F51F2AE7", "FF1816E4",
    "F702EF59", "F7BADAFA", "285954A1", "B9D09511", "F878C4B3", "FB2A0137", "F508E4AA", "1C1FE652", "7C419418",
    "CC50AA59", "CCDF2E5C", "4C0A1F3B", "2452A9DC", "01397D8D", "6BF88C31", "1CCA797A", "EA6DA4AE", "A3C78807",
    "CACE1969", "E0E0D4AD", "F5A14BAB", "80F00988", "A7DE9F4C", "CC450CBA", "0924668F", "5C7DC380", "D96089C5",
    "3640AC4C", "EF1A2E6D", "AE6D9426", "ADC1965B", "6613BA46", "C1FB41C2", "BD9B0ECD", "BE3DEDFC", "7989C8EE",
    "6468FD6E", "6C0DF032", "A7CD6634", "2C826D8B", "2BD2E412", "4D4A2DBE", "B4BF6FA7", "CC1A8959", "08263282",
    "51097330", "46E46CB0", "DF577EC2", "0BD1E364", "262C5564", "18DDA0C9", "FE7B45D9", "D2CE21C9", "D268409A",
    "B1E049E1", "200BFA47", "512D6E73", "C3851EEE", "F341C081", "7D973E48", "08D17554", "A9E20D28", "70518CE6",
    "203AC303", "61ADD0AB", "35D0430C", "C3F8E892", "0D1C8509", "CB92388E", "095436BF", "2FD6E208", "68A29AF9",
    "7D61330B", "753EC6FC", "7211EFEA", "7CD15133", "A574C4FF", "CB41F198", "B598EEF6", "EBBE7347", "C1332568",
    "CEBA5A70", "46A99459", "B4AD9F11", "AE00FEAA", "00B8B573", "A7B480B6", "B5F0B06C", "29A0EC27", "A4DAA010",
    "1E76A1C5", "74BE9133", "7F94C950", "C61F6ED6", "F5B1C7A1", "92E195F8", "572384D4", "E0732C88", "95D41B68",
    "CEE496C3", "394BBD52", "048CD47C", "C05309BE", "D23D2D63", "414DE9C5", "D2229F23", "818666A3", "F0A8B109",
    "B2F6B127", "69A48341", "E4123C56", "6C548C8F", "F5941F61", "94B993AA", "8C165134", "2876763C", "237CE42E",
    "C300D11B", "263821CA", "3AEB8202", "41EC0F84", "CF4AC36D", "D7393EE6", "FD0FC06A", "4118A30A", "551B54A4",
    "D074F86F", "4CC1C54A", "3E57A703", "03774CDA", "EDE43895", "379CE627", "59988939", "E8490DDC", "325410E1",
    "D9352F6A", "4047080A", "F47C081D", "9DB51A85", "C765D71F", "79297527", "FCCA2773", "5A065B97", "114DEE4F",
    "D4B12F5F", "CB29360A", "95D3DE16", "983162A8", "8CBAAFB3", "BB98B27F", "EACD3439", "B1FAC842", "492CBEF1",
    "AE08AB78", "C1D7DFD0", "646F1D40", "C0F463C4", "8FC23A81", "6164E623", "3543F2BC", "915CC253", "8701D0DF",
    "136B2FDD", "677A359E", "0DCFACD0", "5A4EA31E", "87E25935", "97C34E42", "C77780F0", "5B396FBA", "EF1B52E6",
    "F7080941", "2141888B", "278946B0", "919E6D64", "6518B459", "7829FC22", "6325D30E", "030C0399", "BA19B463",
    "564DAB75", "63794F97", "2984C787", "ED702BBE", "CB563B4D", "6FA56696", "4FABC9ED", "DCD87A48", "874DF295",
    "9ECFE9F0", "2A67F49F", "1E9AA4E1", "9A1B7D08", "78D22934", "43521602", "5718A361", "A771BA44", "87A3B97C",
    "B0705C82", "B7526048", "BF86DCD7", "FD066EA4", "7356B1BB", "B872426D", "1575515D", "E99EADB3", "3A9E3C0F",
    "8168599C", "E9D07A32", "8EEAB382", "27023EE8", "80D10FAC", "D368BDC2", "7664B5A7", "89D0CF46", "8BED7368",
    "FF02AF49", "7294E430", "14034FBB", "DABD4CC4", "71535CF8", "9AAEEA20", "1B4D989D", "7FA09780", "F63EF3D2",
    "FADC6788", "012FB568", "08C904FA", "C660883F", "FA1CCE2A", "D13AC8B8", "5CF9C9B3", "DE62C6BD", "ADF500AD",
    "159D967E", "58A2C06C", "665827CB", "DB1AA208", "4286DDF1", "0B8905B4", "CCD149A4", "A8FD9757", "6E7122F0",
    "BFFC21B1", "E9203368", "220C0724", "2E8D86CB", "FB7BFB5E", "43889687", "1869325B", "25420AFC", "485D46DB",
    "22D56381", "CD572D60", "DE89EF2B", "13DAC708", "9467851D", "A09C428D", "8CC3A36C", "0212714E", "251BC1F6",
    "AE274AF0", "DA603F48", "88AFD714", "9F3F014D", "704C7830", "59D803FB", "3315C9DC", "83645273", "23540E0E",
    "66DCE437", "61E09244", "13728D90", "C32E0A94", "3D6B2529", "0A5C5094", "1F91D464", "40C1B904", "2F1494B9",
    "8138AC02", "3D6D8755", "D2963CF5", "6AD203B6", "FB5234E0", "0CB62703", "D2CDF95B", "E718672D", "4D448DF1",
    "F1DD92D7", "0C4613A1", "7DA944F1", "3F72F0C3", "7D3FA930", "8B4742BC", "5674C771", "E3420514", "E669EDD5",
    "5805DE29", "5E86F504", "088449F4", "1C77C8A0", "29B1FCCC", "C7BE9961", "E09AEBE7", "63C5ECB9", "35D657E1",
    "3DDF7AE4", "45D3249C", "6766C940", "7E10AD9C", "18B13E7F", "39320CA2", "21C90078", "7D84661C", "F12A3A21",
    "F4772B41", "F4C53BAB", "A6E76B3B", "9340DED2", "C1EBC21F", "0F4DB654", "6F6C42E3", "3C1A8943", "8D899F74",
    "5A6899BA", "0D9B6827", "D239C5CB", "5290106A", "03F17ADB", "67ACDC26", "0B039B90", "E88F1AFA", "2B42EE31",
    "CF239E4D", "A62C6E93", "421FAE11", "BB522891", "213D9F32", "A5D2ADEB", "7E4AAB21", "736FBC75", "60E56773",
    "8C08C64D", "B7EDA589", "4BDBAE33", "49AD9663", "BEA4300C", "9F997956", "305F5B0C", "CFD6F924", "AFD083DB",
    "500085E0", "2EFA9644", "62BDECBD", "5E5501A5", "580BC7BB", "A02889AD", "D0D63DCD", "C7BCFC3C", "AB56454F",
    "14C4A882", "612197C7", "EDEC3D2F", "75600D88", "C6951B28", "4DB9A52D", "7C9E604D", "E91AC974", "6F946DA2",
    "7D160CF0", "72606B78", "C17B7257", "4E6CEB4E", "1BA9F219", "822D9F9A", "64C24DF6", "82C1ECCB", "DEC48F52",
    "D1500CB6", "5C615E22", "CB7A1247", "EAC83D5E", "A4F2087E", "8B36D663", "60EC8B7D", "23E07770", "CFC3BCB6",
    "332ADE50", "886741FC", "F2ECC125", "59C94057", "FD77D893", "F062680F", "3D759E10", "E03DC9C3", "043AB169",
    "8CD70BCE", "637C48AF", "9F5A86F9", "C799B8ED", "96488FC2", "7A04F3E1", "352E9955", "8A467BFE", "4D1E7ADB",
    "04B931C9", "788E6EA0", "E67267F5", "3B0145B6", "31CA7310", "D8249EEC", "E2D0C5D1", "31DC1298", "70F4055D",
    "39D60297", "25D81F3B", "2BB385C9", "3D500890", "3A48350D", "CCB6120B", "6F89F2F9", "791783D6", "4FA4664E",
    "16FEA67F", "EC629BBF", "A5014386", "6E221244", "A21075D9", "2F501F52", "959A12F4", "E7A64774", "BA060582",
    "FAD0CA2F", "5AC9908F", "7059C853", "AE2F213E", "1C724F28", "B51305FC", "42108734", "298E5C9D", "68A1DD2B",
    "223C8C36", "984B1BEF", "73161B54", "43204F20", "0AC40F25", "9A53ECA9", "CC200DD8", "B6123CC0", "2AE4EFAD",
    "38C464C2", "D4CA75A4", "1E0F1559", "5330CF6E", "4BF2F32B", "A7E130FD", "519B7462", "6B919194", "6C963148",
    "C82B32BB", "82030024", "1CFA2FA0", "15E45CE2", "FE584A91", "4332093F", "2E7B9117", "CD0F4B4D", "C34F792C",
    "077AFAF5", "44136041", "EDC96297", "3E7FD864", "BAF6F6BA", "19C9FF6E", "7D157A58", "5825DFDE", "941A37E0",
    "4818BABB", "3CBE9DC1", "F7F8D0CE", "75771DE9", "36B9CF69", "F68CF878", "9B1E1C75", "8BFB75AE", "39FF446D",
    "85692875", "309DA59F", "3B49C509", "66CD636E", "7D286708", "69CD6A2E", "9FC266E3", "2B8F1988", "ADDBD3AF",
    "4C8EA8E7", "65407C37", "A2EF9AA6", "5E10541B", "26CD9065", "FCEC6367", "C4ED1EF8", "09A9410F", "E24BC427",
    "E29A5EDD", "F58F8C10", "E9CD2A63", "049FFDB7", "CBD2B4CC", "95356BB0", "19453535", "C4508CA2", "4309FD7E",
    "53EA8DE9", "55D9F238", "210A7AEA", "AE02A6AB", "4ABDF123", "22F5256B", "D8DC2D8E", "B8A25D9D", "3B13600F",
    "1B54BEDA", "808164B1", "A75CA457", "0068B861", "7E822EE1", "0335BE36", "C76FEF0E", "495DBCE8", "B70CAB9D",
    "19445725", "E75A3B16", "627F5136", "E4137F41", "5AF821C1", "558BB5A6", "B85003E7", "B2E101C2", "10C101FC",
    "32A3AA3D", "838C4690", "07D18800", "6C89D813", "BE7BA68D", "B096D0B9", "8001786A", "41026A58", "7ADC2D65",
    "66C9FC5F", "E79BE068", "43B4BC3B", "9C203868", "EF2B0BE2", "9E1F6267", "78F76A0A", "DC04692D", "9C6500F3",
    "CE348E12", "7D07E7EE", "93A40579", "83B78DE9", "2ED401C8", "3EB994DD", "3D6C2F3D", "553FD4DA", "E2F27231",
    "809AD218", "0757306E", "989740B3", "7D6A2D71", "4F66622B", "2AFD45F7", "F687016B", "FA8AD0B3", "3B7E96F2",
    "B6732508", "BF351D33", "801EE898", "5291851F", "78993F7E", "5177A95E", "0F432E84", "F3D5350C", "AE95B5E0",
    "88E37A7B", "0ADCC451", "DF2E9F5B", "124A3FE8", "EF421E88", "858AD803", "AAC7FD11", "18C34C95", "4A2915BB",
    "D1722F1A", "10C6360B", "D14AC42F", "3B5D0D66", "B8F1CD0D", "B468C613", "265D94FD", "31F28F03", "A8DBE3DD",
    "6C16811B", "84C2A353", "EFA14DFF", "F914DC58", "246858C1", "A6244E68", "A4A782A4", "CCE276F9", "7904F936",
    "742C1628", "6FDAD5D1", "6EB135B6", "3E8BBB60", "D347C3DF", "8183EF4E", "155E4713", "E82D7CF3", "22177EC5",
    "1395C3B0", "2C633227", "B6C26847", "64147FFF", "9E012434", "D4D54D85", "3067D25F", "CDA949FD", "65A09982",
    "0F7B0A12", "4714FDDD", "6635C1C5", "DBAC6554", "86E1558D", "E8B0113E", "6AB35EA6", "9E06008A", "A827F848",
    "2E4A226E", "A0C2DCAB", "83D89830", "4ABF77E8", "D31B1042", "6E80B0F7", "5BFCAFB4", "03F6F6CE", "D0ED0B8A",
    "FBEB99AF", "4B1B7DEF", "2ACB8C9C", "2C0FE9B1", "1B30FE94", "7E277559", "5F563EA0", "3200CDD8", "32B7E32A",
    "DD43417D", "7D5C2B5B", "344643F9", "56E90A5F", "9DC40517", "F39E0DC7", "E0C7EBBF", "EB9AE161", "6478DAA3",
    "0D7C3D4E", "F252E312", "BCE444B9", "2AACBCDB", "44E502AA", "F8CCF513", "6C3EF465", "906000DA", "8C41446A",
    "992EB47B", "F1573608", "2197C153", "6AF14701", "911852C9", "FBA9538F", "8DEBDD8C", "4D2C26D1", "4F7C4522",
    "B37B354F", "1EE2D92C", "AA952ED2", "BCDFDBF3", "8A043123", "3E232325", "B2B70B0D", "D2A6E5A6", "AAA192D4",
    "2F7B5A6E", "2D9A16B1", "500B9889", "20C58D24", "E1B8C1BC", "FFC3BB81", "AC993839", "2BCE9702", "BD429ACC",
    "08588715", "1002D574", "8FAACA38", "21AE04BC", "E1C200DD", "17072760", "AB4F12B3", "0220E53E", "30C03C0A",
    "74DDDDC9", "5E71509D", "A261B3D2", "75F22283", "879E8C26", "FB4C9FE6", "7F745EF0", "280A27B6", "55F6D9E0",
    "DB038AE1", "40B907F0", "A50F49F3", "3BD2275A", "6053602F", "2C546996", "30E06018", "9892367A", "6DA1FCF7",
    "BA15AB48", "7B65CC3D", "E0A8A155", "1EE06716", "B5086B7B", "96E9F667", "3C19993C", "39E76F8F", "7A5DEDE2",
    "62766632", "04A986AE", "953E9871", "BF5118F1", "40765DA0", "A79BF6F2", "4A5A51CC", "13FC31F4", "9DF9AB13",
    "79076552", "43E97309", "FF18FD70", "D30A52A7", "88378B0F", "FB8FD412", "E6417027", "BAF195A7", "C5DCD5CD",
    "580852DA", "3429986E", "95859146", "E838454A", "1B96C004", "5FF3B74D", "E803CF9C", "D91C5A75", "E130FC96",
    "53A28FD0", "D158C194", "53A50385", "42BF11F5", "A8C206F5", "9827BB23", "96363AD9", "05C362A9", "0C8A29C4",
    "6CDDCB78", "C51C7848", "500E585A", "7DE37864", "2957106E", "7CF9A0AB", "2DC39EE9", "6DDB3D2B", "6C423C98",
    "650CE024", "181F69EB", "C6C5AD24", "9938D8C1", "F98980E3", "788E7DE8", "BF9E08BC", "B229BBE9", "A26EB92B",
    "B32BD9AC", "127A67CA", "A376C062", "A22418D6", "089A42D0", "876757B3", "119D46DA", "4BD1A4A8", "51D149D4",
    "5348AFEE", "87E6B8AF", "A191134A", "A789C0F8", "FE97E849", "6859BB5C", "841BB5DB", "D1EA4A06", "B975CFCA",
    "B7E65F3F", "3EDDFC98", "F7F96715", "55142D20", "88057FBB", "BFA4D8D4", "533CBF5E", "AED6E43F", "1B0A83F1",
    "AE67CCF0", "9C67C962", "142F0382", "C75F956A", "757CD939", "6C67691A", "B2B9C0A8", "174031E8", "1160E67B",
    "443779D1", "11B3299A", "27560BE1", "86EC4732", "47EFF6B9", "4315A14C", "70A96B74", "7BBE7982", "86F2D3AE",
    "217C7787", "7519C371", "182D452C", "E0185A62", "1ED1557A", "849F1010", "F8474049", "F1914086", "C19BAC85",
    "A340E755", "44A73507", "F52B6E09", "B9937043", "3D9EF9F4", "6A6F6972", "AD1659E7", "7987252A", "4A0022C0",
    "0E3168C9", "ABFB9EB5", "9D1E560E", "B1F4BA47", "7D993809", "1167F948", "E78BAA36", "4139ED77", "EF6A33D1",
    "395AAFE9", "EB142D32", "AAF715E6", "4DC77E33", "D82A1596", "CA441C2B", "582EB04F", "39EA0A16", "C7D022DF",
    "72EE825E", "ACE28FEF", "82967F6A", "01B81854", "3B0AB1A1", "9A27D844", "B63DD19C", "63036FC5", "FC051613",
    "EA7E66EB", "A7BC0E30", "004B6F8A", "45CD80DE", "76631166", "78116769", "6CA2239F", "00C4AFDE", "75FE9D74",
    "7248D6C0", "C5293013", "F88FCBBB", "528D833B", "4DFE6D59", "C8E4ACA6", "371162E7", "78AADCCB", "3F61BC3B",
    "0927BF11", "CE739311", "2D061B1E", "7C82D0FA", "B8C52266", "68520BA8", "793660D8", "37337B3E", "2718BC8D",
    "8A4DF5F5", "5753F130", "B9181DCE", "9B7DDC13", "48A02A1B", "31163787", "8AF902AA", "BD098A99", "C18447B7",
    "3E2B642E", "54FC76A4", "EADC525A", "CD405695", "44E13850", "DB691951", "C3F7CF36", "03CFF145", "304C26AC",
    "E28B9A3E", "49997AF9", "97D48BB6", "17533654", "16125513", "9382753B", "ECC4AB75", "AEF0E4A3", "E9E9869F",
    "8BED86B3", "FB12E042", "6CFC90A6", "E667D955", "A4FA33B5", "F3207EB6", "A5CA44C1", "EF3CD4B0", "ADD40AC4",
    "02218A17", "A2C2EAEF", "C71D8276", "F1F6EE43", "06A53DB2", "357CC25F", "967F1C3E", "B5937C41", "A6580857",
    "3C799B6C", "54074DE7", "1E90C0A2", "1C6FB273", "9B78456A", "1FD05538", "733C3941", "F9527F4F", "756F08B1",
    "8881BA01", "A975360D", "A98C82E2", "C5316E71", "B5505F48", "59A7E348", "1198DA00", "A9797612", "34B98139",
    "6A895E81", "6EF53F4E", "D788AB48", "02DD8470", "D53A0971", "11BB305A", "41A61DF7", "57D2E616", "9D0BC340",
    "4FEE4F80"};

constexpr auto testNotInString = "frobinate"sv;

template <typename Container>
void emplace_test_strings(Container& c) {
    const auto startSize = c.size();
    for (const auto& view : testStrings) {
        if constexpr (is_same_v<typename Container::key_type, typename Container::value_type>) {
            c.emplace(view);
        } else {
            c.emplace(view, view.size());
        }
    }

    assert(c.size() == startSize + size(testStrings));
}

template <typename StringLike>
void assert_string(const StringLike& value, const string_view example) {
    assert(value == example);
}

template <typename StringLike>
void assert_string(const pair<const StringLike, size_t>& value, const string_view example) {
    assert(value.first == example);
    assert(value.second == example.size());
}

template <typename It>
void assert_range_empty(const pair<It, It>& p) {
    assert(p.first == p.second);
}

template <typename Container>
void assert_unique() {
    Container cRaw;
#if _HAS_CXX23
    Container cRawToErase;
    Container cRawToExtract;
    Container cRawExtractTarget;
#endif // _HAS_CXX23

    emplace_test_strings(cRaw);
#if _HAS_CXX23
    emplace_test_strings(cRawToErase);
    emplace_test_strings(cRawToExtract);
#endif // _HAS_CXX23

    // Test that transparent containers pass through the string_view; non-transparent containers
    // are only passed in here with string_view value_type, so they also don't allocate.
    [[maybe_unused]] prohibit_allocations prohibitor(true);
    const Container& c = cRaw;
    assert(c.find(testNotInString) == c.end());
    assert(c.contains(testNotInString) == false);
    assert(c.count(testNotInString) == 0);
    assert_range_empty(c.equal_range(testNotInString));

    for (const auto& example : testStrings) {
        const auto target = c.find(example);
        assert(target != c.end());
        assert_string(*target, example);
        assert(1 == c.count(example));
        assert(c.contains(example));
        const auto rangeTarget = c.equal_range(example);
        assert(rangeTarget.first == target);
        assert(next(rangeTarget.first) == rangeTarget.second);
    }

#if _HAS_CXX23
    assert(cRawToErase.erase(testNotInString) == 0);
    for (const auto& example : testStrings) {
        assert(cRawToErase.erase(example) == 1);
    }
    assert(cRawToErase.empty());

    assert(!cRawToExtract.extract(testNotInString));
    for (const auto& example : testStrings) {
        auto n = cRawToExtract.extract(example);
        [[maybe_unused]] prohibit_allocations unprohibitor(false);
        cRawExtractTarget.insert(move(n));
    }
    assert(cRawToExtract.empty());
    assert(cRawExtractTarget.size() == size(testStrings));
#endif // _HAS_CXX23
}

template <typename Container>
void assert_multi() {
    Container cRaw;
#if _HAS_CXX23
    Container cRawToErase;
    Container cRawToExtract;
    Container cRawExtractTarget;
#endif // _HAS_CXX23

    const int dupes = 5;
    for (int dupe = 0; dupe < dupes; ++dupe) {
        emplace_test_strings(cRaw);
#if _HAS_CXX23
        emplace_test_strings(cRawToErase);
        emplace_test_strings(cRawToExtract);
#endif // _HAS_CXX23
    }

    // Test that transparent containers pass through the string_view; non-transparent containers
    // are only passed in here with string_view value_type, so they also don't allocate.
    [[maybe_unused]] prohibit_allocations prohibitor(true);
    const Container& c = cRaw;
    assert(c.find(testNotInString) == c.end());
    assert(c.contains(testNotInString) == false);
    assert(c.count(testNotInString) == 0);
    assert_range_empty(c.equal_range(testNotInString));

    for (const auto& example : testStrings) {
        const auto target = c.find(example);
        assert(target != c.end());
        auto last = target;
        for (int dupe = 0; dupe < dupes; ++dupe, ++last) {
            assert_string(*last, example);
        }

        assert(dupes == c.count(example));
        assert(c.contains(example));
        const auto rangeTarget = c.equal_range(example);
        assert(rangeTarget.first == target);
        assert(rangeTarget.second == last);
    }

#if _HAS_CXX23
    assert(cRawToErase.erase(testNotInString) == 0);
    for (const auto& example : testStrings) {
        assert(cRawToErase.erase(example) == dupes);
    }
    assert(cRawToErase.empty());

    assert(!cRawToExtract.extract(testNotInString));
    for (const auto& example : testStrings) {
        auto n = cRawToExtract.extract(example);
        [[maybe_unused]] prohibit_allocations unprohibitor(false);
        cRawExtractTarget.insert(move(n));
    }
    assert(cRawToExtract.size() == size(testStrings) * (dupes - 1));
    assert(cRawExtractTarget.size() == size(testStrings));
#endif // _HAS_CXX23
}

// Also test P0809 "Comparing unordered containers"
struct offset_hash {
    size_t offset;

    size_t operator()(const size_t target) const noexcept {
        return target + offset;
    }
};

void assert_P0809() {
    unordered_set<size_t, offset_hash> a{8, offset_hash{0}};
    unordered_set<size_t, offset_hash> b{8, offset_hash{10000000}};
    static constexpr size_t testData[] = {1, 10, 1000, 10000, 100000, 1000000, 1729, 42};
    a.insert(begin(testData), end(testData));
    b.insert(begin(testData), end(testData));
    assert(a == b);

    unordered_multiset<size_t, offset_hash> c{8, offset_hash{0}};
    unordered_multiset<size_t, offset_hash> d{8, offset_hash{10000000}};
    for (int i = 0; i < 5; ++i) {
        c.insert(begin(testData), end(testData));
        d.insert(begin(testData), end(testData));
    }

    assert(c == d);
}

int main() {
    assert_unique<stdext::hash_set<string_view, string_legacy_traits, test_allocator<string_view>>>();
    assert_unique<
        stdext::hash_map<string_view, size_t, string_legacy_traits, test_allocator<pair<const string_view, size_t>>>>();
    assert_unique<unordered_set<string_view, hash<string_view>, equal_to<>, test_allocator<string_view>>>();
    assert_unique<unordered_map<string_view, size_t, hash<string_view>, equal_to<>,
        test_allocator<pair<const string_view, size_t>>>>();

    assert_unique<unordered_set<test_str, transparent_string_hasher, equal_to<>, test_allocator<test_str>>>();
    assert_unique<unordered_map<test_str, size_t, transparent_string_hasher, equal_to<>,
        test_allocator<pair<const test_str, size_t>>>>();
    assert_unique<
        unordered_set<test_str, transparent_string_hasher, transparent_string_equal, test_allocator<test_str>>>();
    assert_unique<unordered_map<test_str, size_t, transparent_string_hasher, transparent_string_equal,
        test_allocator<pair<const test_str, size_t>>>>();

    assert_multi<stdext::hash_multiset<string_view, string_legacy_traits, test_allocator<string_view>>>();
    assert_multi<stdext::hash_multimap<string_view, size_t, string_legacy_traits,
        test_allocator<pair<const string_view, size_t>>>>();
    assert_multi<unordered_multiset<string_view, hash<string_view>, equal_to<>, test_allocator<string_view>>>();
    assert_multi<unordered_multimap<string_view, size_t, hash<string_view>, equal_to<>,
        test_allocator<pair<const string_view, size_t>>>>();
    assert_multi<unordered_multiset<test_str, transparent_string_hasher, equal_to<>, test_allocator<test_str>>>();
    assert_multi<unordered_multimap<test_str, size_t, transparent_string_hasher, equal_to<>,
        test_allocator<pair<const test_str, size_t>>>>();
    assert_multi<
        unordered_multiset<test_str, transparent_string_hasher, transparent_string_equal, test_allocator<test_str>>>();
    assert_multi<unordered_multimap<test_str, size_t, transparent_string_hasher, transparent_string_equal,
        test_allocator<pair<const test_str, size_t>>>>();

    assert_P0809();
}
