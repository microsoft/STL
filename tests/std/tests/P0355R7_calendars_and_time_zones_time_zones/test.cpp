// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "timezone_data.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <string_view>

using namespace std;
using namespace std::chrono;

// NOTE:
//     These test suites will assume all data from the IANA database is correct
//     and will not test historical changes in transitions. Instead the focus
//     will be on using a select sample of transitions in both a positive and
//     negative UTC offset zone (different corner cases).

bool operator==(const sys_info& _Left, const sys_info& _Right) {
    return _Left.begin == _Right.begin && _Left.end == _Right.end && _Left.offset == _Right.offset
        && _Left.save == _Right.save && _Left.abbrev == _Right.abbrev;
}

void test_time_zone_and_link(const tzdb& tzdb, string_view tz_name, string_view tz_link_name) {
    const auto orginal_tz = tzdb.locate_zone(tz_name);
    assert(orginal_tz != nullptr);
    assert(orginal_tz->name() == tz_name);

    const auto linked_tz = tzdb.locate_zone(tz_link_name);
    assert(linked_tz != nullptr);
    assert(linked_tz->name() == tz_name);
    assert(orginal_tz == linked_tz);

    const auto tz_link = _Locate_zone_impl(tzdb.links, tz_link_name);
    assert(tz_link != nullptr);
    assert(tz_link->name() == tz_link_name);
    assert(tz_link->target() == tz_name);
    assert(tzdb.locate_zone(tz_link->target()) == orginal_tz);

    assert(_Locate_zone_impl(tzdb.zones, tz_name) != nullptr);
    assert(_Locate_zone_impl(tzdb.zones, tz_link_name) == nullptr);
    assert(_Locate_zone_impl(tzdb.links, tz_name) == nullptr);
}

void try_locate_invalid_zone(const tzdb& tzdb, string_view name) {
    try {
        (void) tzdb.locate_zone(name);
        assert(false);
    } catch (runtime_error) {
    }
}

void timezone_names_test() {
    const auto& tzdb = get_tzdb();

    assert(tzdb.version.empty() == false);

    test_time_zone_and_link(tzdb, "Asia/Thimphu", "Asia/Thimbu");
    test_time_zone_and_link(tzdb, "America/Tijuana", "America/Ensenada");

    const auto current_zone = tzdb.current_zone();
    assert(current_zone != nullptr);
    assert(current_zone->name().empty() == false);

    try_locate_invalid_zone(tzdb, "Non/Existent");

    // Abbreviations should not be time_zones or time_zone_links
    try_locate_invalid_zone(tzdb, "PDT");
    try_locate_invalid_zone(tzdb, "AEST");

    // Comparison operators
    const time_zone tz1{"Earlier"};
    const time_zone tz2{"Earlier"};
    const time_zone tz3{"Later"};
    assert(tz1 == tz2);
    assert(tz1 != tz3);
#ifdef __cpp_lib_concepts
    assert(tz1 <=> tz2 == strong_ordering::equal);
    assert(tz1 <=> tz3 == strong_ordering::less);
    assert(tz3 <=> tz1 == strong_ordering::greater);
#endif // __cpp_lib_concepts

    const time_zone_link link1{"Earlier", "Target"};
    const time_zone_link link2{"Earlier", "Is"};
    const time_zone_link link3{"Later", "Ignored"};
    assert(link1 == link2);
    assert(link1 != link3);
#ifdef __cpp_lib_concepts
    assert(link1 <=> link2 == strong_ordering::equal);
    assert(link1 <=> link3 == strong_ordering::less);
    assert(link3 <=> link1 == strong_ordering::greater);
#endif // __cpp_lib_concepts

    try {
        // ensure locate_zone returns time_zone with given name
        assert(all_of(tzdb.zones.begin(), tzdb.zones.end(),
            [&](const auto& zone) { return tzdb.locate_zone(zone.name())->name() == zone.name(); }));
        // ensure locate_zone returns target of time_zone
        assert(all_of(tzdb.links.begin(), tzdb.links.end(),
            [&](const auto& link) { return tzdb.locate_zone(link.name())->name() == link.target(); }));
        // ensure locate_zone does NOT return time_zone that is also a time_zone_link
        assert(all_of(tzdb.links.begin(), tzdb.links.end(),
            [&](const auto& link) { return tzdb.locate_zone(link.name())->name() != link.name(); }));
    } catch (runtime_error) {
        assert(false);
    }

    // FIXME: add a link to an issue. These may change overtime and might have to be removed from tests.

    // these are some example in which the ICU.dll and IANA database diverge in what they consider a zone or a link
    assert(_Locate_zone_impl(tzdb.links, "Atlantic/Faroe") != nullptr); // is a time_zone in IANA
    assert(_Locate_zone_impl(tzdb.zones, "Africa/Addis_Ababa") != nullptr); // is a time_zone_link in IANA
    assert(_Locate_zone_impl(tzdb.links, "PST") != nullptr); // time_zone_link does not exist in IANA
    assert(_Locate_zone_impl(tzdb.links, "Africa/Asmara") != nullptr); // matches IANA but target is wrong
    assert(_Locate_zone_impl(tzdb.links, "Africa/Asmara")->target() == "Africa/Asmera"); // target == Africa/Nairobi
    assert(_Locate_zone_impl(tzdb.zones, "America/Nuuk") == nullptr); // does not exist in ICU (very rare)
}

void validate_timezone_transitions(const time_zone* tz, const Transition& data) {
    auto info = tz->get_info(data.begin());
    assert(info.begin == data.begin());
    assert(info.end == data.end());
    assert(info.offset == data.offset());
    assert(info.save == data.save());
    assert(info.abbrev == data.abbrev());
}

void timezone_sys_info_test() {
    const auto& tzdb = get_tzdb();

    auto sydney_tz = tzdb.locate_zone(Sydney::Tz_name);
    assert(sydney_tz != nullptr);
    validate_timezone_transitions(sydney_tz, Sydney::Day_1);
    validate_timezone_transitions(sydney_tz, Sydney::Std_1);
    validate_timezone_transitions(sydney_tz, Sydney::Day_2);

    auto la_tz = tzdb.locate_zone(LA::Tz_name);
    assert(la_tz != nullptr);
    validate_timezone_transitions(la_tz, LA::Day_1);
    validate_timezone_transitions(la_tz, LA::Std_1);
    validate_timezone_transitions(la_tz, LA::Day_2);

    auto begin  = sydney_tz->get_info(Sydney::Std_1.begin());
    auto middle = sydney_tz->get_info(Sydney::Std_1.begin() + days{1});
    auto end    = sydney_tz->get_info(Sydney::Std_1.end());
    assert(begin == middle);
    assert(begin != end);

    auto min_info = sydney_tz->get_info(time_zone::_Min_seconds);
    auto max_info = sydney_tz->get_info(time_zone::_Max_seconds - seconds{1});
    assert(min_info.begin == time_zone::_Min_seconds);
    assert(min_info.end != time_zone::_Max_seconds);
    assert(max_info.begin != time_zone::_Min_seconds);
    assert(max_info.end == time_zone::_Max_seconds);

    auto utc_zone = tzdb.locate_zone("Etc/UTC");
    assert(utc_zone != nullptr);

    auto min_utc = utc_zone->get_info(time_zone::_Min_seconds);
    auto max_utc = utc_zone->get_info(time_zone::_Max_seconds - seconds{1});
    // Only a single transition in UTC
    assert(min_utc == max_utc);
    assert(min_utc.begin < max_utc.end);
    assert(min_utc.begin == time_zone::_Min_seconds);
    assert(min_utc.end == time_zone::_Max_seconds);

    // Test abbrevations other than standard/daylight savings such as war time.
    // These senarios are not handled correctly by icu.dll
    auto war_time = la_tz->get_info(sys_days{year{1942} / April / day{1}});
    assert(war_time.abbrev == "PDT"); // IANA datbase == "PWT"
}

void timezone_to_local_test() {
    const auto& tzdb = get_tzdb();
    {
        using namespace Sydney;
        auto tz = tzdb.locate_zone(Tz_name);
        assert(tz != nullptr);

        const auto& to_standard = Day_to_Std.first.end();
        assert(tz->to_local(to_standard) == Day_to_Std.second.local_begin());
        assert(tz->to_local(to_standard + minutes{30}) == Day_to_Std.second.local_begin() + minutes{30});
        assert(tz->to_local(to_standard - minutes{30}) == Day_to_Std.first.local_end() - minutes{30});

        const auto& to_daylight = Std_to_Day.first.end();
        assert(tz->to_local(to_daylight) == Std_to_Day.second.local_begin());
        assert(tz->to_local(to_daylight + minutes{30}) == Std_to_Day.second.local_begin() + minutes{30});
        assert(tz->to_local(to_daylight - minutes{30}) == Std_to_Day.first.local_end() - minutes{30});
    }
    {
        using namespace LA;
        auto tz = tzdb.locate_zone(Tz_name);
        assert(tz != nullptr);

        const auto& to_standard = Day_to_Std.first.end();
        assert(tz->to_local(to_standard) == Day_to_Std.second.local_begin());
        assert(tz->to_local(to_standard + minutes{30}) == Day_to_Std.second.local_begin() + minutes{30});
        assert(tz->to_local(to_standard - minutes{30}) == Day_to_Std.first.local_end() - minutes{30});

        const auto& to_daylight = Std_to_Day.first.end();
        assert(tz->to_local(to_daylight) == Std_to_Day.second.local_begin());
        assert(tz->to_local(to_daylight + minutes{30}) == Std_to_Day.second.local_begin() + minutes{30});
        assert(tz->to_local(to_daylight - minutes{30}) == Std_to_Day.first.local_end() - minutes{30});
    }
}

void assert_local(const time_zone* tz, local_seconds time, int result, const sys_info& first, const sys_info& second) {
    const auto info = tz->get_info(time);
    assert(info.result == result);
    assert(info.first == first);
    assert(info.second == second);

    // time_zone::to_sys depends heavily on local_info so just test it here
    // to exhaust all corner cases.
    sys_seconds sys_earliest{time.time_since_epoch() - info.first.offset};
    sys_seconds sys_latest{time.time_since_epoch() - info.second.offset};
    try {
        assert(tz->to_sys(time) == sys_earliest);
        assert(result == local_info::unique);
    } catch (nonexistent_local_time ex) {
        assert(result == local_info::nonexistent);
    } catch (ambiguous_local_time ex) {
        assert(result == local_info::ambiguous);
    }

    if (result == local_info::unique) {
        assert(tz->to_sys(time, choose::earliest) == sys_earliest);
        assert(tz->to_sys(time, choose::latest) == sys_earliest);
    } else if (result == local_info::nonexistent) {
        assert(tz->to_sys(time, choose::earliest) == info.first.end);
        assert(tz->to_sys(time, choose::latest) == info.first.end);
    } else if (result == local_info::ambiguous) {
        assert(tz->to_sys(time, choose::earliest) == sys_earliest);
        assert(tz->to_sys(time, choose::latest) == sys_latest);
    }
}

void validate_get_local_info(const time_zone* tz, const std::pair<Transition, Transition>& transition, int result) {
    sys_info empty{};
    sys_info first  = tz->get_info(transition.first.begin());
    sys_info second = tz->get_info(transition.second.begin());

    // Get the local time for the beginning of the ambiguous/nonexistent section
    const auto danger_begin = get_danger_begin(transition.first, transition.second);
    const auto danger_end   = get_danger_end(transition.first, transition.second);
    (void) result;
    assert_local(tz, danger_begin - days{2}, local_info::unique, first, empty); // two days before
    assert_local(tz, danger_begin - hours{1}, local_info::unique, first, empty); // one hour before
    assert_local(tz, danger_begin, result, first, second); // danger begin
    assert_local(tz, danger_begin + minutes{30}, result, first, second); // danger middle
    assert_local(tz, danger_end, local_info::unique, second, empty); // danger end
    assert_local(tz, danger_end + hours{2}, local_info::unique, second, empty); // one hour after
    assert_local(tz, danger_end + days{2}, local_info::unique, second, empty); // two days after
}

void timezone_local_info_test() {
    const auto& tzdb = get_tzdb();
    {
        // positive offset (UTC+10/+11) can fall in previous transition
        using namespace Sydney;
        auto tz = tzdb.locate_zone(Tz_name);
        assert(tz != nullptr);
        validate_get_local_info(tz, Day_to_Std, local_info::ambiguous);
        validate_get_local_info(tz, Std_to_Day, local_info::nonexistent);
    }
    {
        // negative offset (UTC-8/-7) can fall in next transition
        using namespace LA;
        auto tz = tzdb.locate_zone(Tz_name);
        assert(tz != nullptr);
        validate_get_local_info(tz, Day_to_Std, local_info::ambiguous);
        validate_get_local_info(tz, Std_to_Day, local_info::nonexistent);
    }
}

template <typename _Dur>
void validate_precision(const time_zone* tz, const pair<Transition, Transition>& transition_pair, _Dur precision) {
    const auto& first       = transition_pair.first;
    const auto& second      = transition_pair.second;
    const auto transition   = first.end<_Dur>();
    const auto danger_begin = get_danger_begin<_Dur>(first, second);
    const auto danger_end   = get_danger_end<_Dur>(first, second);
    sys_info first_info     = tz->get_info(first.begin<_Dur>());
    sys_info second_info    = tz->get_info(second.begin<_Dur>());

    // test correct transition is picked
    assert(tz->get_info(transition) == second_info);
    assert(tz->get_info(transition - precision) == first_info);

    // test ambiguous/nonexistent info is handled
    assert(tz->get_info(danger_end).result == local_info::unique); // exact end of danger zone
    assert(tz->get_info(danger_end - precision).result != local_info::unique); // just inside danger zone
    assert(tz->get_info(danger_begin).result != local_info::unique); // exact start of danger zone
    assert(tz->get_info(danger_begin - precision).result == local_info::unique); // just before danger zone

    // test precision is not lost when converting to local
    assert(tz->to_local(transition) == second.local_begin<_Dur>());
    assert(tz->to_local(transition + precision) == second.local_begin<_Dur>() + precision);
    assert(tz->to_local(transition - precision) == first.local_end<_Dur>() - precision);

    // test precision is not lost when converting to local
    try {
        const sys_time<_Dur> sys_danger_begin = transition - first.save();
        const sys_time<_Dur> sys_danger_end   = transition + first.save();
        assert(tz->to_sys(danger_end) == sys_danger_end);
        assert(tz->to_sys(danger_end + precision) == sys_danger_end + precision);
        assert(tz->to_sys(danger_begin - precision) == sys_danger_begin - precision);
    } catch (nonexistent_local_time&) {
        assert(false);
    } catch (ambiguous_local_time&) {
        assert(false);
    }

    try {
        // test ambiguous/nonexistent info is found
        (void) tz->to_sys(danger_end - precision);
        assert(false);
    } catch (nonexistent_local_time&) {
    } catch (ambiguous_local_time&) {
    }
}

void timezone_precision_test() {
    const auto& tzdb = get_tzdb();
    using Milli_Dur  = duration<double, milli>;
    using Micro_Dur  = duration<double, micro>;

    {
        using namespace Sydney;
        auto tz = tzdb.locate_zone(Tz_name);
        validate_precision(tz, Std_to_Day, sys_seconds::duration{1});
        validate_precision(tz, Std_to_Day, Milli_Dur{1});
        validate_precision(tz, Std_to_Day, Milli_Dur{0.5});
        validate_precision(tz, Std_to_Day, Milli_Dur{0.05});
        validate_precision(tz, Std_to_Day, Milli_Dur{0.005});
        validate_precision(tz, Std_to_Day, Milli_Dur{0.0005});
        // precision limit...
        // validate_precision(tz, Std_to_Day, Milli_Dur{0.00005});
        // validate_precision(tz, Std_to_Day, Micro_Dur{(numeric_limits<double>::min)()});

        validate_precision(tz, Std_to_Day, Micro_Dur{1});
        validate_precision(tz, Std_to_Day, Micro_Dur{0.5});
        // precision limit...
        // validate_precision(tz, Std_to_Day, Micro_Dur{0.05});
        // validate_precision(tz, Std_to_Day, Micro_Dur{(numeric_limits<double>::min)()});

        validate_precision(tz, Day_to_Std, Milli_Dur{0.0005});
        validate_precision(tz, Day_to_Std, Micro_Dur{0.5});
    }
    {
        using namespace LA;
        auto tz = tzdb.locate_zone(Tz_name);
        validate_precision(tz, Std_to_Day, sys_seconds::duration{1});
        validate_precision(tz, Std_to_Day, Milli_Dur{1});
        validate_precision(tz, Std_to_Day, Milli_Dur{0.5});
        validate_precision(tz, Std_to_Day, Milli_Dur{0.05});
        validate_precision(tz, Std_to_Day, Milli_Dur{0.005});
        validate_precision(tz, Std_to_Day, Milli_Dur{0.0005});
        // precision limit...
        // validate_precision(tz, Std_to_Day, Milli_Dur{0.00005});
        // validate_precision(tz, Std_to_Day, Micro_Dur{(numeric_limits<double>::min)()});

        validate_precision(tz, Std_to_Day, Micro_Dur{1});
        validate_precision(tz, Std_to_Day, Micro_Dur{0.5});
        // precision limit...
        // validate_precision(tz, Std_to_Day, Micro_Dur{0.05});
        // validate_precision(tz, Std_to_Day, Micro_Dur{(numeric_limits<double>::min)()});

        validate_precision(tz, Day_to_Std, Milli_Dur{0.0005});
        validate_precision(tz, Day_to_Std, Micro_Dur{0.5});
    }
}

bool test() {
    try {
        timezone_names_test();
        timezone_sys_info_test();
        timezone_to_local_test();
        timezone_local_info_test();
        timezone_precision_test();
    } catch (exception& ex) {
        cerr << "Test threw exception: " << ex.what() << "\n";
        assert(false);
    }

    return true;
}

int main() {
    test();
}
