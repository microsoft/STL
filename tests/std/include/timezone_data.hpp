// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <system_error>
#include <utility>

using namespace std;
using namespace std::chrono;

class Transition {
public:
    constexpr Transition(string_view name, seconds offset, minutes save, string_view abbrev, string_view abbrev_alt,
        sys_seconds sys_begin, sys_seconds sys_end)
        : _name(name), _offset(offset), _save(save), _abbrev(abbrev), _abbrev_alt(abbrev_alt), _begin(sys_begin),
          _end(sys_end) {}

    constexpr string_view name() const {
        return _name;
    }

    constexpr seconds offset() const {
        return _offset;
    }

    constexpr minutes save() const {
        return _save;
    }

    constexpr string_view abbrev() const {
        return _abbrev;
    }

    constexpr string_view abbrev_alt() const {
        return _abbrev_alt;
    }

    constexpr bool is_daylight() const {
        return _save != minutes{0};
    }

    template <class Duration = seconds>
    constexpr sys_time<Duration> begin() const {
        return sys_time<Duration>{duration_cast<Duration>(_begin.time_since_epoch())};
    }

    template <class Duration = seconds>
    constexpr sys_time<Duration> end() const {
        return sys_time<Duration>{duration_cast<Duration>(_end.time_since_epoch())};
    }

    template <class Duration = seconds>
    constexpr local_time<Duration> local_begin() const {
        return local_time<Duration>{duration_cast<Duration>(_begin.time_since_epoch() + _offset)};
    }

    template <class Duration = seconds>
    constexpr local_time<Duration> local_end() const {
        return local_time<Duration>{duration_cast<Duration>(_end.time_since_epoch() + _offset)};
    }

private:
    string_view _name;
    seconds _offset;
    minutes _save;
    string_view _abbrev;
    string_view _abbrev_alt;
    sys_seconds _begin;
    sys_seconds _end;
};

// start of ambiguous/nonexistent zone between transitions
template <class Duration = seconds>
constexpr local_time<Duration> get_danger_begin(const Transition& first, const Transition& second) {
    assert(first.end<Duration>() == second.begin<Duration>());
    return first.local_end<Duration>() - first.save();
}

// end of ambiguous/nonexistent zone between transitions
template <class Duration = seconds>
constexpr local_time<Duration> get_danger_end(const Transition& first, const Transition& second) {
    assert(first.end<Duration>() == second.begin<Duration>());
    return second.local_begin<Duration>() + first.save();
}

// Sydney
// Standard time (AEST : UTC+10) -1 @ 3am
// Daylight time (AEDT : UTC+11) +1 @ 2am
namespace Sydney {
    inline constexpr string_view Tz_name{"Australia/Sydney"sv};
    inline constexpr string_view Standard_abbrev{"AEST"sv};
    inline constexpr string_view Daylight_abbrev{"AEDT"sv};
    inline constexpr string_view Standard_abbrev_alt{"GMT+10"sv};
    inline constexpr string_view Daylight_abbrev_alt{"GMT+11"sv};
    inline constexpr seconds Standard_offset{hours{10}};
    inline constexpr seconds Daylight_offset{hours{11}};
    inline constexpr auto Daylight_begin_2019 =
        sys_seconds{sys_days{year{2019} / October / day{6}}} + hours{2} - Standard_offset;
    inline constexpr auto Standard_begin_2020 =
        sys_seconds{sys_days{year{2020} / April / day{5}}} + hours{3} - Daylight_offset;
    inline constexpr auto Daylight_begin_2020 =
        sys_seconds{sys_days{year{2020} / October / day{4}}} + hours{2} - Standard_offset;
    inline constexpr auto Standard_begin_2021 =
        sys_seconds{sys_days{year{2021} / April / day{4}}} + hours{3} - Daylight_offset;

    inline constexpr Transition Day_1{Tz_name, Daylight_offset, hours{1}, Daylight_abbrev, Daylight_abbrev_alt,
        Daylight_begin_2019, Standard_begin_2020};
    inline constexpr Transition Std_1{Tz_name, Standard_offset, hours{0}, Standard_abbrev, Standard_abbrev_alt,
        Standard_begin_2020, Daylight_begin_2020};
    inline constexpr Transition Day_2{Tz_name, Daylight_offset, hours{1}, Daylight_abbrev, Daylight_abbrev_alt,
        Daylight_begin_2020, Standard_begin_2021};

    inline constexpr pair<Transition, Transition> Day_to_Std{Day_1, Std_1};
    inline constexpr pair<Transition, Transition> Std_to_Day{Std_1, Day_2};

} // namespace Sydney

// Los Angeles
// Standard time (PST : UTC-8) +1 @ 2am
// Daylight time (PDT : UTC-7) -1 @ 2am
namespace LA {
    inline constexpr string_view Tz_name{"America/Los_Angeles"sv};
    inline constexpr string_view Standard_abbrev{"PST"sv};
    inline constexpr string_view Daylight_abbrev{"PDT"sv};
    inline constexpr string_view Standard_abbrev_alt{"GMT-8"sv};
    inline constexpr string_view Daylight_abbrev_alt{"GMT-7"sv};
    inline constexpr seconds Standard_offset{hours{-8}};
    inline constexpr seconds Daylight_offset{hours{-7}};
    inline constexpr auto Daylight_begin_2020 =
        sys_seconds{sys_days{year{2020} / March / day{8}}} + hours{2} - Standard_offset;
    inline constexpr auto Standard_begin_2020 =
        sys_seconds{sys_days{year{2020} / November / day{1}}} + hours{2} - Daylight_offset;
    inline constexpr auto Daylight_begin_2021 =
        sys_seconds{sys_days{year{2021} / March / day{14}}} + hours{2} - Standard_offset;
    inline constexpr auto Standard_begin_2021 =
        sys_seconds{sys_days{year{2021} / November / day{7}}} + hours{2} - Daylight_offset;

    inline constexpr Transition Day_1{Tz_name, Daylight_offset, hours{1}, Daylight_abbrev, Daylight_abbrev_alt,
        Daylight_begin_2020, Standard_begin_2020};
    inline constexpr Transition Std_1{Tz_name, Standard_offset, hours{0}, Standard_abbrev, Standard_abbrev_alt,
        Standard_begin_2020, Daylight_begin_2021};
    inline constexpr Transition Day_2{Tz_name, Daylight_offset, hours{1}, Daylight_abbrev, Daylight_abbrev_alt,
        Daylight_begin_2021, Standard_begin_2021};

    inline constexpr pair<Transition, Transition> Day_to_Std{Day_1, Std_1};
    inline constexpr pair<Transition, Transition> Std_to_Day{Std_1, Day_2};

} // namespace LA

template <class TestFunction>
void run_tz_test(TestFunction test_function) {
    try {
#ifdef _MSVC_INTERNAL_TESTING
        try {
            (void) get_tzdb();
        } catch (const system_error& ex) {
            if (ex.code() == error_code{126 /* ERROR_MOD_NOT_FOUND */, system_category()}) {
                // Skip testing when we can't load icu.dll on an internal test machine running an older OS.
                exit(EXIT_SUCCESS);
            }

            throw; // Report any other errors.
        }
#endif // _MSVC_INTERNAL_TESTING

        test_function();

    } catch (const system_error& ex) {
        cerr << "Test threw system_error: " << ex.what() << "\n";
        cerr << "With error_code: " << ex.code() << "\n";
        assert(false);
    } catch (const runtime_error& ex) {
        cerr << "Test threw runtime_error: " << ex.what() << "\n";
        assert(false);
    } catch (const exception& ex) {
        cerr << "Test threw exception: " << ex.what() << "\n";
        assert(false);
    }
}
