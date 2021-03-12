// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <string>
#include <string_view>

using namespace std;
using namespace std::chrono;

class Transition {
public:
    Transition(
        string_view name, seconds offset, minutes save, string_view abbrev, sys_seconds _begin_, sys_seconds _end_)
        : _name(name), _offset(offset), _save(save), _abbrev(abbrev), _begin(_begin_), _end(_end_) {}

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

    constexpr bool is_daylight() const {
        return _save != minutes{0};
    }

    template <class _Duration = seconds>
    constexpr sys_time<_Duration> begin() const {
        return sys_time<_Duration>{duration_cast<_Duration>(_begin.time_since_epoch())};
    }

    template <class _Duration = seconds>
    constexpr sys_time<_Duration> end() const {
        return sys_time<_Duration>{duration_cast<_Duration>(_end.time_since_epoch())};
    }

    template <class _Duration = seconds>
    constexpr local_time<_Duration> local_begin() const {
        return local_time<_Duration>{duration_cast<_Duration>(_begin.time_since_epoch() + _offset)};
    }

    template <class _Duration = seconds>
    constexpr local_time<_Duration> local_end() const {
        return local_time<_Duration>{duration_cast<_Duration>(_end.time_since_epoch() + _offset)};
    }

private:
    string _name;
    seconds _offset;
    minutes _save;
    string _abbrev;
    sys_seconds _begin;
    sys_seconds _end;
};

// start of ambiguous/nonexistent zone between transitions
template <class _Duration = seconds>
constexpr local_time<_Duration> get_danger_begin(const Transition& first, const Transition& second) {
    assert(first.end<_Duration>() == second.begin<_Duration>());
    return first.local_end<_Duration>() - first.save();
}

// end of ambiguous/nonexistent zone between transitions
template <class _Duration = seconds>
constexpr local_time<_Duration> get_danger_end(const Transition& first, const Transition& second) {
    assert(first.end<_Duration>() == second.begin<_Duration>());
    return second.local_begin<_Duration>() + first.save();
}

// Sydney
// Standard time (AEST : UTC+10) -1 @ 3am
// Daylight time (AEDT : UTC+11) +1 @ 2am
namespace Sydney {
    static constexpr std::string_view Tz_name{"Australia/Sydney"sv};
    static constexpr std::string_view Standard_abbrev{"GMT+10"sv}; // IANA database == "AEST"
    static constexpr std::string_view Daylight_abbrev{"GMT+11"sv}; // IANA database == "AEDT"
    static constexpr seconds Standard_offset{hours{10}};
    static constexpr seconds Daylight_offset{hours{11}};
    static constexpr auto Daylight_begin_2019 =
        sys_seconds{sys_days{year{2019} / October / day{6}}} + hours{2} - Standard_offset;
    static constexpr auto Standard_begin_2020 =
        sys_seconds{sys_days{year{2020} / April / day{5}}} + hours{3} - Daylight_offset;
    static constexpr auto Daylight_begin_2020 =
        sys_seconds{sys_days{year{2020} / October / day{4}}} + hours{2} - Standard_offset;
    static constexpr auto Standard_begin_2021 =
        sys_seconds{sys_days{year{2021} / April / day{4}}} + hours{3} - Daylight_offset;

    Transition Day_1{Tz_name, Daylight_offset, hours{1}, Daylight_abbrev, Daylight_begin_2019, Standard_begin_2020};
    Transition Std_1{Tz_name, Standard_offset, hours{0}, Standard_abbrev, Standard_begin_2020, Daylight_begin_2020};
    Transition Day_2{Tz_name, Daylight_offset, hours{1}, Daylight_abbrev, Daylight_begin_2020, Standard_begin_2021};

    std::pair<Transition, Transition> Day_to_Std{Day_1, Std_1};
    std::pair<Transition, Transition> Std_to_Day{Std_1, Day_2};

} // namespace Sydney

// Los Angeles
// Standard time (PST : UTC-8) +1 @ 2am
// Daylight time (PDT : UTC-7) -1 @ 2am
namespace LA {
    static constexpr std::string_view Tz_name{"America/Los_Angeles"sv};
    static constexpr std::string_view Standard_abbrev{"PST"sv};
    static constexpr std::string_view Daylight_abbrev{"PDT"sv};
    static constexpr seconds Standard_offset{hours{-8}};
    static constexpr seconds Daylight_offset{hours{-7}};
    static constexpr auto Daylight_begin_2020 =
        sys_seconds{sys_days{year{2020} / March / day{8}}} + hours{2} - Standard_offset;
    static constexpr auto Standard_begin_2020 =
        sys_seconds{sys_days{year{2020} / November / day{1}}} + hours{2} - Daylight_offset;
    static constexpr auto Daylight_begin_2021 =
        sys_seconds{sys_days{year{2021} / March / day{14}}} + hours{2} - Standard_offset;
    static constexpr auto Standard_begin_2021 =
        sys_seconds{sys_days{year{2021} / November / day{7}}} + hours{2} - Daylight_offset;

    Transition Day_1{Tz_name, Daylight_offset, hours{1}, Daylight_abbrev, Daylight_begin_2020, Standard_begin_2020};
    Transition Std_1{Tz_name, Standard_offset, hours{0}, Standard_abbrev, Standard_begin_2020, Daylight_begin_2021};
    Transition Day_2{Tz_name, Daylight_offset, hours{1}, Daylight_abbrev, Daylight_begin_2021, Standard_begin_2021};

    std::pair<Transition, Transition> Day_to_Std{Day_1, Std_1};
    std::pair<Transition, Transition> Std_to_Day{Std_1, Day_2};

} // namespace LA
