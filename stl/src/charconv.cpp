// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

// clang-format off

#include <charconv>

namespace std {

extern const uint32_t _Large_power_data[] = {0x540be400, 0x00000002, 0x63100000, 0x6bc75e2d, 0x00000005,
    0x40000000, 0x4674edea, 0x9f2c9cd0, 0x0000000c, 0xb9f56100, 0x5ca4bfab, 0x6329f1c3, 0x0000001d, 0xb5640000,
    0xc40534fd, 0x926687d2, 0x6c3b15f9, 0x00000044, 0x10000000, 0x946590d9, 0xd762422c, 0x9a224501, 0x4f272617,
    0x0000009f, 0x07950240, 0x245689c1, 0xc5faa71c, 0x73c86d67, 0xebad6ddc, 0x00000172, 0xcec10000, 0x63a22764,
    0xefa418ca, 0xcdd17b25, 0x6bdfef70, 0x9dea3e1f, 0x0000035f, 0xe4000000, 0xcdc3fe6e, 0x66bc0c6a, 0x2e391f32,
    0x5a450203, 0x71d2f825, 0xc3c24a56, 0x000007da, 0xa82e8f10, 0xaab24308, 0x8e211a7c, 0xf38ace40, 0x84c4ce0b,
    0x7ceb0b27, 0xad2594c3, 0x00001249, 0xdd1a4000, 0xcc9f54da, 0xdc5961bf, 0xc75cabab, 0xf505440c, 0xd1bc1667,
    0xfbb7af52, 0x608f8d29, 0x00002a94, 0x21000000, 0x17bb8a0c, 0x56af8ea4, 0x06479fa9, 0x5d4bb236, 0x80dc5fe0,
    0xf0feaa0a, 0xa88ed940, 0x6b1a80d0, 0x00006323, 0x324c3864, 0x8357c796, 0xe44a42d5, 0xd9a92261, 0xbd3c103d,
    0x91e5f372, 0xc0591574, 0xec1da60d, 0x102ad96c, 0x0000e6d3, 0x1e851000, 0x6e4f615b, 0x187b2a69, 0x0450e21c,
    0x2fdd342b, 0x635027ee, 0xa6c97199, 0x8e4ae916, 0x17082e28, 0x1a496e6f, 0x0002196e, 0x32400000, 0x04ad4026,
    0xf91e7250, 0x2994d1d5, 0x665bcdbb, 0xa23b2e96, 0x65fa7ddb, 0x77de53ac, 0xb020a29b, 0xc6bff953, 0x4b9425ab,
    0x0004e34d, 0xfbc32d81, 0x5222d0f4, 0xb70f2850, 0x5713f2f3, 0xdc421413, 0xd6395d7d, 0xf8591999, 0x0092381c,
    0x86b314d6, 0x7aa577b9, 0x12b7fe61, 0x000b616a, 0x1d11e400, 0x56c3678d, 0x3a941f20, 0x9b09368b, 0xbd706908,
    0x207665be, 0x9b26c4eb, 0x1567e89d, 0x9d15096e, 0x7132f22b, 0xbe485113, 0x45e5a2ce, 0x001a7f52, 0xbb100000,
    0x02f79478, 0x8c1b74c0, 0xb0f05d00, 0xa9dbc675, 0xe2d9b914, 0x650f72df, 0x77284b4c, 0x6df6e016, 0x514391c2,
    0x2795c9cf, 0xd6e2ab55, 0x9ca8e627, 0x003db1a6, 0x40000000, 0xf4ecd04a, 0x7f2388f0, 0x580a6dc5, 0x43bf046f,
    0xf82d5dc3, 0xee110848, 0xfaa0591c, 0xcdf4f028, 0x192ea53f, 0xbcd671a0, 0x7d694487, 0x10f96e01, 0x791a569d,
    0x008fa475, 0xb9b2e100, 0x8288753c, 0xcd3f1693, 0x89b43a6b, 0x089e87de, 0x684d4546, 0xfddba60c, 0xdf249391,
    0x3068ec13, 0x99b44427, 0xb68141ee, 0x5802cac3, 0xd96851f1, 0x7d7625a2, 0x014e718d, 0xfb640000, 0xf25a83e6,
    0x9457ad0f, 0x0080b511, 0x2029b566, 0xd7c5d2cf, 0xa53f6d7d, 0xcdb74d1c, 0xda9d70de, 0xb716413d, 0x71d0ca4e,
    0xd7e41398, 0x4f403a90, 0xf9ab3fe2, 0x264d776f, 0x030aafe6, 0x10000000, 0x09ab5531, 0xa60c58d2, 0x566126cb,
    0x6a1c8387, 0x7587f4c1, 0x2c44e876, 0x41a047cf, 0xc908059e, 0xa0ba063e, 0xe7cfc8e8, 0xe1fac055, 0xef0144b2,
    0x24207eb0, 0xd1722573, 0xe4b8f981, 0x071505ae, 0x7a3b6240, 0xcea45d4f, 0x4fe24133, 0x210f6d6d, 0xe55633f2,
    0x25c11356, 0x28ebd797, 0xd396eb84, 0x1e493b77, 0x471f2dae, 0x96ad3820, 0x8afaced1, 0x4edecddb, 0x5568c086,
    0xb2695da1, 0x24123c89, 0x107d4571, 0x1c410000, 0x6e174a27, 0xec62ae57, 0xef2289aa, 0xb6a2fbdd, 0x17e1efe4,
    0x3366bdf2, 0x37b48880, 0xbfb82c3e, 0x19acde91, 0xd4f46408, 0x35ff6a4e, 0x67566a0e, 0x40dbb914, 0x782a3bca,
    0x6b329b68, 0xf5afc5d9, 0x266469bc, 0xe4000000, 0xfb805ff4, 0xed55d1af, 0x9b4a20a8, 0xab9757f8, 0x01aefe0a,
    0x4a2ca67b, 0x1ebf9569, 0xc7c41c29, 0xd8d5d2aa, 0xd136c776, 0x93da550c, 0x9ac79d90, 0x254bcba8, 0x0df07618,
    0xf7a88809, 0x3a1f1074, 0xe54811fc, 0x59638ead, 0x97cbe710, 0x26d769e8, 0xb4e4723e, 0x5b90aa86, 0x9c333922,
    0x4b7a0775, 0x2d47e991, 0x9a6ef977, 0x160b40e7, 0x0c92f8c4, 0xf25ff010, 0x25c36c11, 0xc9f98b42, 0x730b919d,
    0x05ff7caf, 0xb0432d85, 0x2d2b7569, 0xa657842c, 0xd01fef10, 0xc77a4000, 0xe8b862e5, 0x10d8886a, 0xc8cd98e5,
    0x108955c5, 0xd059b655, 0x58fbbed4, 0x03b88231, 0x034c4519, 0x194dc939, 0x1fc500ac, 0x794cc0e2, 0x3bc980a1,
    0xe9b12dd1, 0x5e6d22f8, 0x7b38899a, 0xce7919d8, 0x78c67672, 0x79e5b99f, 0xe494034e, 0x00000001, 0xa1000000,
    0x6c5cd4e9, 0x9be47d6f, 0xf93bd9e7, 0x77626fa1, 0xc68b3451, 0xde2b59e8, 0xcf3cde58, 0x2246ff58, 0xa8577c15,
    0x26e77559, 0x17776753, 0xebe6b763, 0xe3fd0a5f, 0x33e83969, 0xa805a035, 0xf631b987, 0x211f0f43, 0xd85a43db,
    0xab1bf596, 0x683f19a2, 0x00000004, 0xbe7dfe64, 0x4bc9042f, 0xe1f5edb0, 0x8fa14eda, 0xe409db73, 0x674fee9c,
    0xa9159f0d, 0xf6b5b5d6, 0x7338960e, 0xeb49c291, 0x5f2b97cc, 0x0f383f95, 0x2091b3f6, 0xd1783714, 0xc1d142df,
    0x153e22de, 0x8aafdf57, 0x77f5e55f, 0xa3e7ca8b, 0x032f525b, 0x42e74f3d, 0x0000000a, 0xf4dd1000, 0x5d450952,
    0xaeb442e1, 0xa3b3342e, 0x3fcda36f, 0xb4287a6e, 0x4bc177f7, 0x67d2c8d0, 0xaea8f8e0, 0xadc93b67, 0x6cc856b3,
    0x959d9d0b, 0x5b48c100, 0x4abe8a3d, 0x52d936f4, 0x71dbe84d, 0xf91c21c5, 0x4a458109, 0xd7aad86a, 0x08e14c7c,
    0x759ba59c, 0xe43c8800, 0x00000017, 0x92400000, 0x04f110d4, 0x186472be, 0x8736c10c, 0x1478abfb, 0xfc51af29,
    0x25eb9739, 0x4c2b3015, 0xa1030e0b, 0x28fe3c3b, 0x7788fcba, 0xb89e4358, 0x733de4a4, 0x7c46f2c2, 0x8f746298,
    0xdb19210f, 0x2ea3b6ae, 0xaa5014b2, 0xea39ab8d, 0x97963442, 0x01dfdfa9, 0xd2f3d3fe, 0xa0790280, 0x00000037,
    0x509c9b01, 0xc7dcadf1, 0x383dad2c, 0x73c64d37, 0xea6d67d0, 0x519ba806, 0xc403f2f8, 0xa052e1a2, 0xd710233a,
    0x448573a9, 0xcf12d9ba, 0x70871803, 0x52dc3a9b, 0xe5b252e8, 0x0717fb4e, 0xbe4da62f, 0x0aabd7e1, 0x8c62ed4f,
    0xceb9ec7b, 0xd4664021, 0xa1158300, 0xcce375e6, 0x842f29f2, 0x00000081, 0x7717e400, 0xd3f5fb64, 0xa0763d71,
    0x7d142fe9, 0x33f44c66, 0xf3b8f12e, 0x130f0d8e, 0x734c9469, 0x60260fa8, 0x3c011340, 0xcc71880a, 0x37a52d21,
    0x8adac9ef, 0x42bb31b4, 0xd6f94c41, 0xc88b056c, 0xe20501b8, 0x5297ed7c, 0x62c361c4, 0x87dad8aa, 0xb833eade,
    0x94f06861, 0x13cc9abd, 0x8dc1d56a, 0x0000012d, 0x13100000, 0xc67a36e8, 0xf416299e, 0xf3493f0a, 0x77a5a6cf,
    0xa4be23a3, 0xcca25b82, 0x3510722f, 0xbe9d447f, 0xa8c213b8, 0xc94c324e, 0xbc9e33ad, 0x76acfeba, 0x2e4c2132,
    0x3e13cd32, 0x70fe91b4, 0xbb5cd936, 0x42149785, 0x46cc1afd, 0xe638ddf8, 0x690787d2, 0x1a02d117, 0x3eb5f1fe,
    0xc3b9abae, 0x1c08ee6f, 0x000002be, 0x40000000, 0x8140c2aa, 0x2cf877d9, 0x71e1d73d, 0xd5e72f98, 0x72516309,
    0xafa819dd, 0xd62a5a46, 0x2a02dcce, 0xce46ddfe, 0x2713248d, 0xb723d2ad, 0xc404bb19, 0xb706cc2b, 0x47b1ebca,
    0x9d094bdc, 0xc5dc02ca, 0x31e6518e, 0x8ec35680, 0x342f58a8, 0x8b041e42, 0xfebfe514, 0x05fffc13, 0x6763790f,
    0x66d536fd, 0xb9e15076, 0x00000662, 0x67b06100, 0xd2010a1a, 0xd005e1c0, 0xdb12733b, 0xa39f2e3f, 0x61b29de2,
    0x2a63dce2, 0x942604bc, 0x6170d59b, 0xc2e32596, 0x140b75b9, 0x1f1d2c21, 0xb8136a60, 0x89d23ba2, 0x60f17d73,
    0xc6cad7df, 0x0669df2b, 0x24b88737, 0x669306ed, 0x19496eeb, 0x938ddb6f, 0x5e748275, 0xc56e9a36, 0x3690b731,
    0xc82842c5, 0x24ae798e, 0x00000ede, 0x41640000, 0xd5889ac1, 0xd9432c99, 0xa280e71a, 0x6bf63d2e, 0x8249793d,
    0x79e7a943, 0x22fde64a, 0xe0d6709a, 0x05cacfef, 0xbd8da4d7, 0xe364006c, 0xa54edcb3, 0xa1a8086e, 0x748f459e,
    0xfc8e54c8, 0xcc74c657, 0x42b8c3d4, 0x57d9636e, 0x35b55bcc, 0x6c13fee9, 0x1ac45161, 0xb595badb, 0xa1f14e9d,
    0xdcf9e750, 0x07637f71, 0xde2f9f2b, 0x0000229d, 0x10000000, 0x3c5ebd89, 0xe3773756, 0x3dcba338, 0x81d29e4f,
    0xa4f79e2c, 0xc3f9c774, 0x6a1ce797, 0xac5fe438, 0x07f38b9c, 0xd588ecfa, 0x3e5ac1ac, 0x85afccce, 0x9d1f3f70,
    0xe82d6dd3, 0x177d180c, 0x5e69946f, 0x648e2ce1, 0x95a13948, 0x340fe011, 0xb4173c58, 0x2748f694, 0x7c2657bd,
    0x758bda2e, 0x3b8090a0, 0x2ddbb613, 0x6dcf4890, 0x24e4047e, 0x00005099};

/* static */ const uint64_t _General_precision_tables<double>::_Special_X_table[195] =
                                             {0x3F18E757928E0C9Du, 0x3F4F212D77318FC5u, 0x3F8374BC6A7EF9DBu,
    0x3FB851EB851EB851u, 0x3FEE666666666666u, 0x4022FFFFFFFFFFFFu, 0x3F1A1554FBDAD751u, 0x3F504D551D68C692u,
    0x3F8460AA64C2F837u, 0x3FB978D4FDF3B645u, 0x3FEFD70A3D70A3D7u, 0x4023E66666666666u, 0x4058DFFFFFFFFFFFu,
    0x3F1A3387ECC8EB96u, 0x3F506034F3FD933Eu, 0x3F84784230FCF80Du, 0x3FB99652BD3C3611u, 0x3FEFFBE76C8B4395u,
    0x4023FD70A3D70A3Du, 0x4058FCCCCCCCCCCCu, 0x408F3BFFFFFFFFFFu, 0x3F1A368D04E0BA6Au, 0x3F506218230C7482u,
    0x3F847A9E2BCF91A3u, 0x3FB99945B6C3760Bu, 0x3FEFFF972474538Eu, 0x4023FFBE76C8B439u, 0x4058FFAE147AE147u,
    0x408F3F9999999999u, 0x40C387BFFFFFFFFFu, 0x3F1A36DA54164F19u, 0x3F506248748DF16Fu, 0x3F847ADA91B16DCBu,
    0x3FB99991361DC93Eu, 0x3FEFFFF583A53B8Eu, 0x4023FFF972474538u, 0x4058FFF7CED91687u, 0x408F3FF5C28F5C28u,
    0x40C387F999999999u, 0x40F869F7FFFFFFFFu, 0x3F1A36E20F35445Du, 0x3F50624D49814ABAu, 0x3F847AE09BE19D69u,
    0x3FB99998C2DA04C3u, 0x3FEFFFFEF39085F4u, 0x4023FFFF583A53B8u, 0x4058FFFF2E48E8A7u, 0x408F3FFEF9DB22D0u,
    0x40C387FF5C28F5C2u, 0x40F869FF33333333u, 0x412E847EFFFFFFFFu, 0x3F1A36E2D51EC34Bu, 0x3F50624DC5333A0Eu,
    0x3F847AE136800892u, 0x3FB9999984200AB7u, 0x3FEFFFFFE5280D65u, 0x4023FFFFEF39085Fu, 0x4058FFFFEB074A77u,
    0x408F3FFFE5C91D14u, 0x40C387FFEF9DB22Du, 0x40F869FFEB851EB8u, 0x412E847FE6666666u, 0x416312CFEFFFFFFFu,
    0x3F1A36E2E8E94FFCu, 0x3F50624DD191D1FDu, 0x3F847AE145F6467Du, 0x3FB999999773D81Cu, 0x3FEFFFFFFD50CE23u,
    0x4023FFFFFE5280D6u, 0x4058FFFFFDE7210Bu, 0x408F3FFFFD60E94Eu, 0x40C387FFFE5C91D1u, 0x40F869FFFDF3B645u,
    0x412E847FFD70A3D7u, 0x416312CFFE666666u, 0x4197D783FDFFFFFFu, 0x3F1A36E2EAE3F7A7u, 0x3F50624DD2CE7AC8u,
    0x3F847AE14782197Bu, 0x3FB9999999629FD9u, 0x3FEFFFFFFFBB47D0u, 0x4023FFFFFFD50CE2u, 0x4058FFFFFFCA501Au,
    0x408F3FFFFFBCE421u, 0x40C387FFFFD60E94u, 0x40F869FFFFCB923Au, 0x412E847FFFBE76C8u, 0x416312CFFFD70A3Du,
    0x4197D783FFCCCCCCu, 0x41CDCD64FFBFFFFFu, 0x3F1A36E2EB16A205u, 0x3F50624DD2EE2543u, 0x3F847AE147A9AE94u,
    0x3FB9999999941A39u, 0x3FEFFFFFFFF920C8u, 0x4023FFFFFFFBB47Du, 0x4058FFFFFFFAA19Cu, 0x408F3FFFFFF94A03u,
    0x40C387FFFFFBCE42u, 0x40F869FFFFFAC1D2u, 0x412E847FFFF97247u, 0x416312CFFFFBE76Cu, 0x4197D783FFFAE147u,
    0x41CDCD64FFF99999u, 0x4202A05F1FFBFFFFu, 0x3F1A36E2EB1BB30Fu, 0x3F50624DD2F14FE9u, 0x3F847AE147ADA3E3u,
    0x3FB9999999990CDCu, 0x3FEFFFFFFFFF5014u, 0x4023FFFFFFFF920Cu, 0x4058FFFFFFFF768Fu, 0x408F3FFFFFFF5433u,
    0x40C387FFFFFF94A0u, 0x40F869FFFFFF79C8u, 0x412E847FFFFF583Au, 0x416312CFFFFF9724u, 0x4197D783FFFF7CEDu,
    0x41CDCD64FFFF5C28u, 0x4202A05F1FFF9999u, 0x42374876E7FF7FFFu, 0x3F1A36E2EB1C34C3u, 0x3F50624DD2F1A0FAu,
    0x3F847AE147AE0938u, 0x3FB9999999998B86u, 0x3FEFFFFFFFFFEE68u, 0x4023FFFFFFFFF501u, 0x4058FFFFFFFFF241u,
    0x408F3FFFFFFFEED1u, 0x40C387FFFFFFF543u, 0x40F869FFFFFFF294u, 0x412E847FFFFFEF39u, 0x416312CFFFFFF583u,
    0x4197D783FFFFF2E4u, 0x41CDCD64FFFFEF9Du, 0x4202A05F1FFFF5C2u, 0x42374876E7FFF333u, 0x426D1A94A1FFEFFFu,
    0x3F1A36E2EB1C41BBu, 0x3F50624DD2F1A915u, 0x3F847AE147AE135Au, 0x3FB9999999999831u, 0x3FEFFFFFFFFFFE3Du,
    0x4023FFFFFFFFFEE6u, 0x4058FFFFFFFFFEA0u, 0x408F3FFFFFFFFE48u, 0x40C387FFFFFFFEEDu, 0x40F869FFFFFFFEA8u,
    0x412E847FFFFFFE52u, 0x416312CFFFFFFEF3u, 0x4197D783FFFFFEB0u, 0x41CDCD64FFFFFE5Cu, 0x4202A05F1FFFFEF9u,
    0x42374876E7FFFEB8u, 0x426D1A94A1FFFE66u, 0x42A2309CE53FFEFFu, 0x3F1A36E2EB1C4307u, 0x3F50624DD2F1A9E4u,
    0x3F847AE147AE145Eu, 0x3FB9999999999975u, 0x3FEFFFFFFFFFFFD2u, 0x4023FFFFFFFFFFE3u, 0x4058FFFFFFFFFFDCu,
    0x408F3FFFFFFFFFD4u, 0x40C387FFFFFFFFE4u, 0x40F869FFFFFFFFDDu, 0x412E847FFFFFFFD5u, 0x416312CFFFFFFFE5u,
    0x4197D783FFFFFFDEu, 0x41CDCD64FFFFFFD6u, 0x4202A05F1FFFFFE5u, 0x42374876E7FFFFDFu, 0x426D1A94A1FFFFD7u,
    0x42A2309CE53FFFE6u, 0x42D6BCC41E8FFFDFu, 0x3F1A36E2EB1C4328u, 0x3F50624DD2F1A9F9u, 0x3F847AE147AE1477u,
    0x3FB9999999999995u, 0x3FEFFFFFFFFFFFFBu, 0x4023FFFFFFFFFFFDu, 0x4058FFFFFFFFFFFCu, 0x408F3FFFFFFFFFFBu,
    0x40C387FFFFFFFFFDu, 0x40F869FFFFFFFFFCu, 0x412E847FFFFFFFFBu, 0x416312CFFFFFFFFDu, 0x4197D783FFFFFFFCu,
    0x41CDCD64FFFFFFFBu, 0x4202A05F1FFFFFFDu, 0x42374876E7FFFFFCu, 0x426D1A94A1FFFFFBu, 0x42A2309CE53FFFFDu,
    0x42D6BCC41E8FFFFCu, 0x430C6BF52633FFFBu};

/* static */ const uint64_t _General_precision_tables<double>::_Ordinary_X_table[314]
                                           = {0x3F1A36E2EB1C432Cu, 0x3F50624DD2F1A9FBu, 0x3F847AE147AE147Au,
    0x3FB9999999999999u, 0x3FEFFFFFFFFFFFFFu, 0x4023FFFFFFFFFFFFu, 0x4058FFFFFFFFFFFFu, 0x408F3FFFFFFFFFFFu,
    0x40C387FFFFFFFFFFu, 0x40F869FFFFFFFFFFu, 0x412E847FFFFFFFFFu, 0x416312CFFFFFFFFFu, 0x4197D783FFFFFFFFu,
    0x41CDCD64FFFFFFFFu, 0x4202A05F1FFFFFFFu, 0x42374876E7FFFFFFu, 0x426D1A94A1FFFFFFu, 0x42A2309CE53FFFFFu,
    0x42D6BCC41E8FFFFFu, 0x430C6BF52633FFFFu, 0x4341C37937E07FFFu, 0x4376345785D89FFFu, 0x43ABC16D674EC7FFu,
    0x43E158E460913CFFu, 0x4415AF1D78B58C3Fu, 0x444B1AE4D6E2EF4Fu, 0x4480F0CF064DD591u, 0x44B52D02C7E14AF6u,
    0x44EA784379D99DB4u, 0x45208B2A2C280290u, 0x4554ADF4B7320334u, 0x4589D971E4FE8401u, 0x45C027E72F1F1281u,
    0x45F431E0FAE6D721u, 0x46293E5939A08CE9u, 0x465F8DEF8808B024u, 0x4693B8B5B5056E16u, 0x46C8A6E32246C99Cu,
    0x46FED09BEAD87C03u, 0x4733426172C74D82u, 0x476812F9CF7920E2u, 0x479E17B84357691Bu, 0x47D2CED32A16A1B1u,
    0x48078287F49C4A1Du, 0x483D6329F1C35CA4u, 0x48725DFA371A19E6u, 0x48A6F578C4E0A060u, 0x48DCB2D6F618C878u,
    0x4911EFC659CF7D4Bu, 0x49466BB7F0435C9Eu, 0x497C06A5EC5433C6u, 0x49B18427B3B4A05Bu, 0x49E5E531A0A1C872u,
    0x4A1B5E7E08CA3A8Fu, 0x4A511B0EC57E6499u, 0x4A8561D276DDFDC0u, 0x4ABABA4714957D30u, 0x4AF0B46C6CDD6E3Eu,
    0x4B24E1878814C9CDu, 0x4B5A19E96A19FC40u, 0x4B905031E2503DA8u, 0x4BC4643E5AE44D12u, 0x4BF97D4DF19D6057u,
    0x4C2FDCA16E04B86Du, 0x4C63E9E4E4C2F344u, 0x4C98E45E1DF3B015u, 0x4CCF1D75A5709C1Au, 0x4D03726987666190u,
    0x4D384F03E93FF9F4u, 0x4D6E62C4E38FF872u, 0x4DA2FDBB0E39FB47u, 0x4DD7BD29D1C87A19u, 0x4E0DAC74463A989Fu,
    0x4E428BC8ABE49F63u, 0x4E772EBAD6DDC73Cu, 0x4EACFA698C95390Bu, 0x4EE21C81F7DD43A7u, 0x4F16A3A275D49491u,
    0x4F4C4C8B1349B9B5u, 0x4F81AFD6EC0E1411u, 0x4FB61BCCA7119915u, 0x4FEBA2BFD0D5FF5Bu, 0x502145B7E285BF98u,
    0x50559725DB272F7Fu, 0x508AFCEF51F0FB5Eu, 0x50C0DE1593369D1Bu, 0x50F5159AF8044462u, 0x512A5B01B605557Au,
    0x516078E111C3556Cu, 0x5194971956342AC7u, 0x51C9BCDFABC13579u, 0x5200160BCB58C16Cu, 0x52341B8EBE2EF1C7u,
    0x526922726DBAAE39u, 0x529F6B0F092959C7u, 0x52D3A2E965B9D81Cu, 0x53088BA3BF284E23u, 0x533EAE8CAEF261ACu,
    0x53732D17ED577D0Bu, 0x53A7F85DE8AD5C4Eu, 0x53DDF67562D8B362u, 0x5412BA095DC7701Du, 0x5447688BB5394C25u,
    0x547D42AEA2879F2Eu, 0x54B249AD2594C37Cu, 0x54E6DC186EF9F45Cu, 0x551C931E8AB87173u, 0x5551DBF316B346E7u,
    0x558652EFDC6018A1u, 0x55BBE7ABD3781ECAu, 0x55F170CB642B133Eu, 0x5625CCFE3D35D80Eu, 0x565B403DCC834E11u,
    0x569108269FD210CBu, 0x56C54A3047C694FDu, 0x56FA9CBC59B83A3Du, 0x5730A1F5B8132466u, 0x5764CA732617ED7Fu,
    0x5799FD0FEF9DE8DFu, 0x57D03E29F5C2B18Bu, 0x58044DB473335DEEu, 0x583961219000356Au, 0x586FB969F40042C5u,
    0x58A3D3E2388029BBu, 0x58D8C8DAC6A0342Au, 0x590EFB1178484134u, 0x59435CEAEB2D28C0u, 0x59783425A5F872F1u,
    0x59AE412F0F768FADu, 0x59E2E8BD69AA19CCu, 0x5A17A2ECC414A03Fu, 0x5A4D8BA7F519C84Fu, 0x5A827748F9301D31u,
    0x5AB7151B377C247Eu, 0x5AECDA62055B2D9Du, 0x5B22087D4358FC82u, 0x5B568A9C942F3BA3u, 0x5B8C2D43B93B0A8Bu,
    0x5BC19C4A53C4E697u, 0x5BF6035CE8B6203Du, 0x5C2B843422E3A84Cu, 0x5C6132A095CE492Fu, 0x5C957F48BB41DB7Bu,
    0x5CCADF1AEA12525Au, 0x5D00CB70D24B7378u, 0x5D34FE4D06DE5056u, 0x5D6A3DE04895E46Cu, 0x5DA066AC2D5DAEC3u,
    0x5DD4805738B51A74u, 0x5E09A06D06E26112u, 0x5E400444244D7CABu, 0x5E7405552D60DBD6u, 0x5EA906AA78B912CBu,
    0x5EDF485516E7577Eu, 0x5F138D352E5096AFu, 0x5F48708279E4BC5Au, 0x5F7E8CA3185DEB71u, 0x5FB317E5EF3AB327u,
    0x5FE7DDDF6B095FF0u, 0x601DD55745CBB7ECu, 0x6052A5568B9F52F4u, 0x60874EAC2E8727B1u, 0x60BD22573A28F19Du,
    0x60F2357684599702u, 0x6126C2D4256FFCC2u, 0x615C73892ECBFBF3u, 0x6191C835BD3F7D78u, 0x61C63A432C8F5CD6u,
    0x61FBC8D3F7B3340Bu, 0x62315D847AD00087u, 0x6265B4E5998400A9u, 0x629B221EFFE500D3u, 0x62D0F5535FEF2084u,
    0x630532A837EAE8A5u, 0x633A7F5245E5A2CEu, 0x63708F936BAF85C1u, 0x63A4B378469B6731u, 0x63D9E056584240FDu,
    0x64102C35F729689Eu, 0x6444374374F3C2C6u, 0x647945145230B377u, 0x64AF965966BCE055u, 0x64E3BDF7E0360C35u,
    0x6518AD75D8438F43u, 0x654ED8D34E547313u, 0x6583478410F4C7ECu, 0x65B819651531F9E7u, 0x65EE1FBE5A7E7861u,
    0x6622D3D6F88F0B3Cu, 0x665788CCB6B2CE0Cu, 0x668D6AFFE45F818Fu, 0x66C262DFEEBBB0F9u, 0x66F6FB97EA6A9D37u,
    0x672CBA7DE5054485u, 0x6761F48EAF234AD3u, 0x679671B25AEC1D88u, 0x67CC0E1EF1A724EAu, 0x680188D357087712u,
    0x6835EB082CCA94D7u, 0x686B65CA37FD3A0Du, 0x68A11F9E62FE4448u, 0x68D56785FBBDD55Au, 0x690AC1677AAD4AB0u,
    0x6940B8E0ACAC4EAEu, 0x6974E718D7D7625Au, 0x69AA20DF0DCD3AF0u, 0x69E0548B68A044D6u, 0x6A1469AE42C8560Cu,
    0x6A498419D37A6B8Fu, 0x6A7FE52048590672u, 0x6AB3EF342D37A407u, 0x6AE8EB0138858D09u, 0x6B1F25C186A6F04Cu,
    0x6B537798F428562Fu, 0x6B88557F31326BBBu, 0x6BBE6ADEFD7F06AAu, 0x6BF302CB5E6F642Au, 0x6C27C37E360B3D35u,
    0x6C5DB45DC38E0C82u, 0x6C9290BA9A38C7D1u, 0x6CC734E940C6F9C5u, 0x6CFD022390F8B837u, 0x6D3221563A9B7322u,
    0x6D66A9ABC9424FEBu, 0x6D9C5416BB92E3E6u, 0x6DD1B48E353BCE6Fu, 0x6E0621B1C28AC20Bu, 0x6E3BAA1E332D728Eu,
    0x6E714A52DFFC6799u, 0x6EA59CE797FB817Fu, 0x6EDB04217DFA61DFu, 0x6F10E294EEBC7D2Bu, 0x6F451B3A2A6B9C76u,
    0x6F7A6208B5068394u, 0x6FB07D457124123Cu, 0x6FE49C96CD6D16CBu, 0x7019C3BC80C85C7Eu, 0x70501A55D07D39CFu,
    0x708420EB449C8842u, 0x70B9292615C3AA53u, 0x70EF736F9B3494E8u, 0x7123A825C100DD11u, 0x7158922F31411455u,
    0x718EB6BAFD91596Bu, 0x71C33234DE7AD7E2u, 0x71F7FEC216198DDBu, 0x722DFE729B9FF152u, 0x7262BF07A143F6D3u,
    0x72976EC98994F488u, 0x72CD4A7BEBFA31AAu, 0x73024E8D737C5F0Au, 0x7336E230D05B76CDu, 0x736C9ABD04725480u,
    0x73A1E0B622C774D0u, 0x73D658E3AB795204u, 0x740BEF1C9657A685u, 0x74417571DDF6C813u, 0x7475D2CE55747A18u,
    0x74AB4781EAD1989Eu, 0x74E10CB132C2FF63u, 0x75154FDD7F73BF3Bu, 0x754AA3D4DF50AF0Au, 0x7580A6650B926D66u,
    0x75B4CFFE4E7708C0u, 0x75EA03FDE214CAF0u, 0x7620427EAD4CFED6u, 0x7654531E58A03E8Bu, 0x768967E5EEC84E2Eu,
    0x76BFC1DF6A7A61BAu, 0x76F3D92BA28C7D14u, 0x7728CF768B2F9C59u, 0x775F03542DFB8370u, 0x779362149CBD3226u,
    0x77C83A99C3EC7EAFu, 0x77FE494034E79E5Bu, 0x7832EDC82110C2F9u, 0x7867A93A2954F3B7u, 0x789D9388B3AA30A5u,
    0x78D27C35704A5E67u, 0x79071B42CC5CF601u, 0x793CE2137F743381u, 0x79720D4C2FA8A030u, 0x79A6909F3B92C83Du,
    0x79DC34C70A777A4Cu, 0x7A11A0FC668AAC6Fu, 0x7A46093B802D578Bu, 0x7A7B8B8A6038AD6Eu, 0x7AB137367C236C65u,
    0x7AE585041B2C477Eu, 0x7B1AE64521F7595Eu, 0x7B50CFEB353A97DAu, 0x7B8503E602893DD1u, 0x7BBA44DF832B8D45u,
    0x7BF06B0BB1FB384Bu, 0x7C2485CE9E7A065Eu, 0x7C59A742461887F6u, 0x7C9008896BCF54F9u, 0x7CC40AABC6C32A38u,
    0x7CF90D56B873F4C6u, 0x7D2F50AC6690F1F8u, 0x7D63926BC01A973Bu, 0x7D987706B0213D09u, 0x7DCE94C85C298C4Cu,
    0x7E031CFD3999F7AFu, 0x7E37E43C8800759Bu, 0x7E6DDD4BAA009302u, 0x7EA2AA4F4A405BE1u, 0x7ED754E31CD072D9u,
    0x7F0D2A1BE4048F90u, 0x7F423A516E82D9BAu, 0x7F76C8E5CA239028u, 0x7FAC7B1F3CAC7433u, 0x7FE1CCF385EBC89Fu,
    0x7FEFFFFFFFFFFFFFu};

} // namespace std
