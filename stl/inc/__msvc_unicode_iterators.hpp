// __msvc_unicode_iterators.hpp internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef __MSVC_UNICODE_ITERATORS_HPP
#define __MSVC_UNICODE_ITERATORS_HPP
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#include <__msvc_format_ucd_tables.hpp>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable: _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN

// _Decode_utf decodes UTF-8 or UTF-16 encoded unsigned char or wchar_t strings respectively
_NODISCARD constexpr const wchar_t* _Decode_utf(const wchar_t* _First, const wchar_t* _Last, char32_t& _Val) noexcept {
    _STL_INTERNAL_CHECK(_First < _Last);
    _Val = static_cast<char32_t>(*_First);
    if (_Val < 0xD800) {
        return _First + 1;
    } else if (_Val <= 0xDBFF) {
        // 0xD800 <= _Val <= 0xDBFF: High surrogate
        if (_First + 1 == _Last) {
            _Val = 0xFFFD;
            return _Last;
        }

        if (_First[1] < 0xDC00 || _First[1] > 0xDFFF) {
            // unpaired high surrogate
            _Val = 0xFFFD;
            return _First + 1;
        }

        _Val = (_Val - 0xD800) << 10;
        _Val += _First[1] - 0xDC00;
        _Val += 0x10000;
        return _First + 2;
    } else if (_Val <= 0xDFFF) {
        // unpaired low surrogate
        _Val = 0xFFFD;
        return _First + 1;
    }

    return _First + 1;
}

_NODISCARD constexpr const char* _Decode_utf(const char* _First, const char* _Last, char32_t& _Val) noexcept {
    _STL_INTERNAL_CHECK(_First < _Last);
    // Decode a UTF-8 encoded codepoint starting at _First and not exceeding _Last, returning
    // one past the end of the character decoded. Any invalid codepoints will result in
    // _Val == U+FFFD and _Decode_utf will return one past the
    // maximal subpart of the ill-formed subsequence. So, most invalid UTF-8 will result in
    // one U+FFFD for each byte of invalid data. Truncated but otherwise valid UTF-8 may
    // result in one U+FFFD for more than one input byte.
    _Val = static_cast<char32_t>(static_cast<unsigned char>(*_First));

    // All UTF-8 text is at least one byte.
    // The zero extended values of the "prefix" bytes for
    // a multi-byte sequence are the lowest numeric value (in two's complement)
    // that any leading byte could have for a code unit of that size, so
    // we just sum the comparisons to get the number of trailing bytes.
    int _Num_bytes;
    if (_Val <= 0x7F) {
        return _First + 1;
    } else if (_Val >= 0xC2 && _Val <= 0xDF) {
        _Num_bytes = 2;
    } else if (_Val >= 0xE0 && _Val <= 0xEF) {
        _Num_bytes = 3;
    } else if (_Val >= 0xF0 && _Val <= 0xF4) {
        _Num_bytes = 4;
    } else {
        // definitely not valid
        _Val = 0xFFFD;
        return _First + 1;
    }

    if (_First + 1 == _Last) {
        // We got a multibyte sequence and the next byte is off the end, we need
        // to check just the next byte here since we need to look for overlong sequences.
        // We want to return one past the end of a truncated sequence if everything is
        // otherwise valid, so we can't check if _First + _Num_bytes is off the end.
        _Val = 0xFFFD;
        return _Last;
    }

    switch (_Val) {
    case 0xE0:
        // we know _First[1] is in range because we just checked above,
        // and a leader of 0xE0 implies _Num_bytes == 3
        if (static_cast<unsigned char>(_First[1]) < 0xA0) {
            // note, we just increment forward one-byte,
            // even though _Num_bytes would imply the next
            // codepoint starts at _First + 2, this is because
            // we don't consume trailing bytes of ill-formed subsequences
            _Val = 0xFFFD;
            return _First + 1;
        }
        break;
    case 0xED:
        if (static_cast<unsigned char>(_First[1]) > 0x9F) {
            _Val = 0xFFFD;
            return _First + 1;
        }
        break;
    case 0xF0:
        if (static_cast<unsigned char>(_First[1]) < 0x90) {
            _Val = 0xFFFD;
            return _First + 1;
        }
        break;
    case 0xF4:
        if (static_cast<unsigned char>(_First[1]) > 0x8F) {
            _Val = 0xFFFD;
            return _First + 1;
        }
        break;
    }

    // mask out the "value bits" in the leading byte,
    // for one-byte codepoints there is no leader,
    // two-byte codepoints have the same number of value
    // bits as trailing bytes (including the leading zero)
    switch (_Num_bytes) {
    case 2:
        _Val &= 0b1'1111u;
        break;
    case 3:
        _Val &= 0b1111u;
        break;
    case 4:
        _Val &= 0b111u;
        break;
    }

    for (int _Idx = 1; _Idx < _Num_bytes; ++_Idx) {
        if (_First + _Idx >= _Last || static_cast<unsigned char>(_First[_Idx]) < 0x80
            || static_cast<unsigned char>(_First[_Idx]) > 0xBF) {
            // truncated sequence
            _Val = 0xFFFD;
            return _First + _Idx;
        }
        // we know we're always in range due to the above check.
        _Val = (_Val << 6) | (static_cast<unsigned char>(_First[_Idx]) & 0b11'1111u);
    }
    return _First + _Num_bytes;
}

_NODISCARD constexpr const char32_t* _Decode_utf(
    const char32_t* _First, const char32_t* _Last, char32_t& _Val) noexcept {
    _STL_INTERNAL_CHECK(_First < _Last);
    (void) _Last;
    _Val = *_First;
    return _First + 1;
}

template <class _CharT>
class _Unicode_codepoint_iterator {
private:
    const _CharT* _First = nullptr;
    const _CharT* _Last  = nullptr;
    const _CharT* _Next  = nullptr;
    char32_t _Val        = 0;

public:
    using value_type      = char32_t;
    using difference_type = ptrdiff_t;

    constexpr _Unicode_codepoint_iterator(const _CharT* _First_val, const _CharT* _Last_val) noexcept
        : _First(_First_val), _Last(_Last_val) {
        _Next = _Decode_utf(_First, _Last, _Val);
    }

    constexpr _Unicode_codepoint_iterator() = default;

    constexpr _Unicode_codepoint_iterator& operator++() noexcept {
        _First = _Next;
        if (_First != _Last) {
            _Next = _Decode_utf(_First, _Last, _Val);
        }

        return *this;
    }
    constexpr _Unicode_codepoint_iterator operator++(int) noexcept {
        auto _Old = *this;
        ++*this;
        return _Old;
    }

    _NODISCARD constexpr value_type operator*() const noexcept {
        return _Val;
    }

    _NODISCARD constexpr const _CharT* _Position() const noexcept {
        return _First;
    }

    _NODISCARD constexpr bool operator==(default_sentinel_t) const noexcept {
        return _First == _Last;
    }
    _NODISCARD constexpr bool operator==(const _Unicode_codepoint_iterator& _Other) const noexcept {
        _STL_INTERNAL_CHECK(_Last == _Other._Last);
        return _First == _Other._First && _Last == _Other._Last;
    }
};

// Implements a DFA matching the regex on the left side of rule GB11. The DFA is:
//
// +---+   ExtPic      +---+    ZWJ        +---+
// | 1 +---------------> 2 +---------------> 3 |
// +---+               ++-^+               +---+
//                      | |
//                      +-+
//                      Extend
//
// Note state 3 is never explicitly transitioned to, since it's the "accept" state, we just
// transition back to state 1 and return true.
class _GB11_LeftHand_regex {
private:
    enum _State_t : bool { _Start, _ExtPic };

    _State_t _State = _Start;

public:
    _NODISCARD constexpr bool operator==(const _GB11_LeftHand_regex&) const noexcept = default;

    _NODISCARD constexpr bool _Match(
        const _Grapheme_Break_property_values _Left_gbp, _Extended_Pictographic_property_values _Left_ExtPic) noexcept {
        switch (_State) {
        case _Start:
            if (_Left_ExtPic == _Extended_Pictographic_property_values::_Extended_Pictographic_value) {
                _State = _ExtPic;
            }
            return false;
        case _ExtPic:
            if (_Left_gbp == _Grapheme_Break_property_values::_ZWJ_value) {
                _State = _Start;
                return true;
            } else if (_Left_gbp != _Grapheme_Break_property_values::_Extend_value) {
                _State = _Start;
                return false;
            }
            return false;
        default:
            _STL_INTERNAL_CHECK(false);
            return false;
        }
    }
};

template <class _CharT>
class _Grapheme_break_property_iterator {
private:
    using _Wrapped_iter_type = _Unicode_codepoint_iterator<_CharT>;

    _Wrapped_iter_type _WrappedIter;
    _GB11_LeftHand_regex _GB11_rx;

public:
    _NODISCARD constexpr bool operator==(default_sentinel_t) const noexcept {
        return _WrappedIter == default_sentinel;
    }

    _NODISCARD constexpr bool operator==(const _Grapheme_break_property_iterator&) const noexcept = default;

    using difference_type = ptrdiff_t;
    using value_type      = iter_value_t<_Wrapped_iter_type>;

    constexpr _Grapheme_break_property_iterator(const _CharT* _First, const _CharT* _Last)
        : _WrappedIter(_First, _Last) {}

    constexpr _Grapheme_break_property_iterator() = default;

    constexpr _Grapheme_break_property_iterator& operator++() noexcept {
        auto _Left_gbp     = _Grapheme_Break_property_data._Get_property_for_codepoint(*_WrappedIter);
        auto _Left_ExtPic  = _Extended_Pictographic_property_data._Get_property_for_codepoint(*_WrappedIter);
        auto _Right_gbp    = _Grapheme_Break_property_values::_No_value;
        auto _Right_ExtPic = _Extended_Pictographic_property_values::_No_value;
        size_t _Num_RIs    = 0;
        for (;; _Left_gbp = _Right_gbp, _Left_ExtPic = _Right_ExtPic) {
            ++_WrappedIter;
            if (_WrappedIter == default_sentinel) {
                return *this; // GB2 Any % eot
            }
            _Right_gbp    = _Grapheme_Break_property_data._Get_property_for_codepoint(*_WrappedIter);
            _Right_ExtPic = _Extended_Pictographic_property_data._Get_property_for_codepoint(*_WrappedIter);
            // match GB11 now, so that we're sure to update it for every character, not just ones where
            // the GB11 rule is considered
            const bool _GB11_Match = _GB11_rx._Match(_Left_gbp, _Left_ExtPic);
            // Also update the number of sequential RIs immediately
            if (_Left_gbp == _Grapheme_Break_property_values::_Regional_Indicator_value) {
                ++_Num_RIs;
            } else {
                _Num_RIs = 0;
            }

            if (_Left_gbp == _Grapheme_Break_property_values::_CR_value
                && _Right_gbp == _Grapheme_Break_property_values::_LF_value) {
                continue; // GB3 CR x LF
            }

            if (_Left_gbp == _Grapheme_Break_property_values::_Control_value
                || _Left_gbp == _Grapheme_Break_property_values::_CR_value
                || _Left_gbp == _Grapheme_Break_property_values::_LF_value) {
                return *this; // GB4 (Control | CR | LF) % Any
            }

            if (_Right_gbp == _Grapheme_Break_property_values::_Control_value
                || _Right_gbp == _Grapheme_Break_property_values::_CR_value
                || _Right_gbp == _Grapheme_Break_property_values::_LF_value) {
                return *this; // GB5 Any % (Control | CR | LF)
            }

            if (_Left_gbp == _Grapheme_Break_property_values::_L_value
                && (_Right_gbp == _Grapheme_Break_property_values::_L_value
                    || _Right_gbp == _Grapheme_Break_property_values::_V_value
                    || _Right_gbp == _Grapheme_Break_property_values::_LV_value
                    || _Right_gbp == _Grapheme_Break_property_values::_LVT_value)) {
                continue; // GB6 L x (L | V | LV | LVT)
            }

            if ((_Left_gbp == _Grapheme_Break_property_values::_LV_value
                    || _Left_gbp == _Grapheme_Break_property_values::_V_value)
                && (_Right_gbp == _Grapheme_Break_property_values::_V_value
                    || _Right_gbp == _Grapheme_Break_property_values::_T_value)) {
                continue; // GB7 (LV | V) x (V | T)
            }

            if ((_Left_gbp == _Grapheme_Break_property_values::_LVT_value
                    || _Left_gbp == _Grapheme_Break_property_values::_T_value)
                && _Right_gbp == _Grapheme_Break_property_values::_T_value) {
                continue; // GB8 (LVT | T) x T
            }

            if (_Right_gbp == _Grapheme_Break_property_values::_Extend_value
                || _Right_gbp == _Grapheme_Break_property_values::_ZWJ_value) {
                continue; // GB9 x (Extend | ZWJ)
            }

            if (_Right_gbp == _Grapheme_Break_property_values::_SpacingMark_value) {
                continue; // GB9a x SpacingMark
            }

            if (_Left_gbp == _Grapheme_Break_property_values::_Prepend_value) {
                continue; // GB9b Prepend x
            }

            if (_GB11_Match && _Right_ExtPic == _Extended_Pictographic_property_values::_Extended_Pictographic_value) {
                continue; // GB11 \p{ExtendedPictographic} Extend* ZWJ x \p{ExtendedPictographic}
            }

            if (_Left_gbp == _Grapheme_Break_property_values::_Regional_Indicator_value
                && _Right_gbp == _Grapheme_Break_property_values::_Regional_Indicator_value && _Num_RIs % 2 != 0) {
                continue; // GB12 and 13, do not break between RIs if there are an odd number of RIs before the
                          // breakpoint
            }
            return *this;
        }
    }
    constexpr _Grapheme_break_property_iterator operator++(int) noexcept {
        auto _Old = *this;
        ++*this;
        return _Old;
    }

    _NODISCARD constexpr const _CharT* _Position() const noexcept {
        return _WrappedIter._Position();
    }

    _NODISCARD constexpr value_type operator*() const noexcept {
        return *_WrappedIter;
    }
};

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // __MSVC_UNICODE_ITERATORS_HPP
