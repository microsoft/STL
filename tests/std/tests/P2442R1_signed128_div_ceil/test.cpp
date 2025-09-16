// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <__msvc_int128.hpp>
#include <cassert>
#include <limits>

constexpr void test() {
    using std::_Signed128;

    assert(_Signed128::_Div_ceil(0, 1) == 0);
    assert(_Signed128::_Div_ceil(1, 1) == 1);
    assert(_Signed128::_Div_ceil(2, 1) == 2);

    assert(_Signed128::_Div_ceil(116, 3) == 39);
    assert(_Signed128::_Div_ceil(205, 24) == 9);
    assert(_Signed128::_Div_ceil(83, 1) == 83);
    assert(_Signed128::_Div_ceil(240, 13) == 19);
    assert(_Signed128::_Div_ceil(100, 13) == 8);

    assert(_Signed128::_Div_ceil(149, 147) == 2);
    assert(_Signed128::_Div_ceil(122, 152) == 1);
    assert(_Signed128::_Div_ceil(154, 236) == 1);
    assert(_Signed128::_Div_ceil(163, 251) == 1);
    assert(_Signed128::_Div_ceil(89, 199) == 1);

    assert(_Signed128::_Div_ceil(1069796746, 23678) == 45182);
    assert(_Signed128::_Div_ceil(1200533627, 8126) == 147740);
    assert(_Signed128::_Div_ceil(1483929008, 31399) == 47261);
    assert(_Signed128::_Div_ceil(40124559, 11161) == 3596);
    assert(_Signed128::_Div_ceil(616936466, 18476) == 33392);

    assert(_Signed128::_Div_ceil(3823598863U, 26233) == 145756);
    assert(_Signed128::_Div_ceil(1748736303, 60486) == 28912);
    assert(_Signed128::_Div_ceil(3727652564U, 11487) == 324511);
    assert(_Signed128::_Div_ceil(443086302, 51740) == 8564);
    assert(_Signed128::_Div_ceil(2309966593U, 45205) == 51100);

    assert(_Signed128::_Div_ceil(1688186001, 2331624069U) == 1);
    assert(_Signed128::_Div_ceil(4072558058U, 202653587) == 21);
    assert(_Signed128::_Div_ceil(551570633, 2681152300U) == 1);
    assert(_Signed128::_Div_ceil(3166723164U, 852231237) == 4);
    assert(_Signed128::_Div_ceil(3238568162U, 1640506845) == 2);

    assert(_Signed128::_Div_ceil(6242708709713706173ULL, 986240334) == 6329804709ULL);
    assert(_Signed128::_Div_ceil(5971429753141872861ULL, 773448951) == 7720522144ULL);
    assert(_Signed128::_Div_ceil(3773313860741815071ULL, 2965323624U) == 1272479615);
    assert(_Signed128::_Div_ceil(5545364762704150466ULL, 264975532) == 20927837076ULL);
    assert(_Signed128::_Div_ceil(3730152929068983090ULL, 2492151938U) == 1496759838);

    assert(_Signed128::_Div_ceil(10191699552117669113ULL, 3524412433243714339ULL) == 3);
    assert(_Signed128::_Div_ceil(3073175754216384423ULL, 8155635103385767810ULL) == 1);
    assert(_Signed128::_Div_ceil(14454347299931791225ULL, 1125386058911220070ULL) == 13);
    assert(_Signed128::_Div_ceil(13659816168029181560ULL, 5789718827007400904ULL) == 3);
    assert(_Signed128::_Div_ceil(1069287642992917145ULL, 738486755903428333ULL) == 2);

    assert((_Signed128::_Div_ceil(_Signed128{1329496395917005583ULL, 4825206483521586190ULL}, 2328480880092472314ULL)
            == _Signed128{1332872165572535026ULL, 2}));
    assert(_Signed128::_Div_ceil(_Signed128{9730685006318077192ULL, 7232491004677529961ULL}, 7551609526229071216ULL)
           == 17667215196349628672ULL);
    assert((_Signed128::_Div_ceil(_Signed128{12172154442470927259ULL, 8777428261169746305ULL}, 8637741546317560676ULL)
            == _Signed128{298314676997257110ULL, 1}));
    assert((_Signed128::_Div_ceil(_Signed128{15481671640377153953ULL, 8903114101091695213ULL}, 6090891903637929972ULL)
            == _Signed128{8517035596026637315ULL, 1}));
    assert(_Signed128::_Div_ceil(_Signed128{15743145484066333195ULL, 805665491188157799ULL}, 6585636204170695090ULL)
           == 2256715169835129064ULL);

    assert(_Signed128::_Div_ceil(_Signed128{10731879507814251049ULL, 3873445590U},
               _Signed128{12450032536891019435ULL, 9105649371241646359ULL})
           == 1);
    assert(_Signed128::_Div_ceil(_Signed128{6629798443635676577ULL, 2327383978U},
               _Signed128{9839716022772647304ULL, 7922581345850742364ULL})
           == 1);
    assert(_Signed128::_Div_ceil(_Signed128{11339831796615584288ULL, 4257443653U},
               _Signed128{4227111794755706639ULL, 670374651106134374ULL})
           == 1);
    assert(_Signed128::_Div_ceil(_Signed128{8670574441769227246ULL, 2507472313U},
               _Signed128{6980882583348139841ULL, 4304389704649692956ULL})
           == 1);
    assert(_Signed128::_Div_ceil(0, _Signed128{6980882583348139841ULL, 4304389704649692956ULL}) == 0);

    assert(_Signed128::_Div_ceil(_Signed128{6864055997479949219ULL, 8622452334377655874ULL},
               _Signed128{2561354846261717143ULL, 1392383021258532795ULL})
           == 7);
    assert(_Signed128::_Div_ceil(_Signed128{939441510092257833ULL, 520549398019160129ULL},
               _Signed128{2009633185566611151ULL, 8480432536590844791ULL})
           == 1);
    assert(_Signed128::_Div_ceil(_Signed128{10105653851413328809ULL, 6012530092550053736ULL},
               _Signed128{5123113788439280060ULL, 6703818145194006067ULL})
           == 1);
    assert(_Signed128::_Div_ceil(_Signed128{10851415781288494064ULL, 6183428757366606812ULL},
               _Signed128{2541106769657089592ULL, 2041369374582624895ULL})
           == 4);
    assert(_Signed128::_Div_ceil(_Signed128{3797056150347910189ULL, 6463249530114157684ULL},
               _Signed128{12268041577665946814ULL, 7107698248126780543ULL})
           == 1);

    constexpr _Signed128 mx = std::numeric_limits<_Signed128>::max();
    assert(_Signed128::_Div_ceil(mx, mx) == 1);
    assert(_Signed128::_Div_ceil(mx - 1, mx) == 1);
    assert(_Signed128::_Div_ceil(mx, mx - 1) == 2);
    assert(_Signed128::_Div_ceil(mx, 1) == mx);
    assert(_Signed128::_Div_ceil(mx, 2) == mx / 2 + 1);
}

int main() {
    static_assert((test(), true));
    test();
}
