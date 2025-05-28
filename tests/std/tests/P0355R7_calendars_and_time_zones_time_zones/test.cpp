// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <chrono>
#include <ratio>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include <timezone_data.hpp>

using namespace std;
using namespace std::chrono;

// NOTE:
//     These test suites will assume all data from the IANA database is correct
//     and will not test historical changes in transitions. Instead the focus
//     will be on using a select sample of transitions in both a positive and
//     negative UTC offset zone (different corner cases).

constexpr bool sys_equal(const sys_info& left, const sys_info& right) {
    return left.begin == right.begin && left.end == right.end && left.offset == right.offset && left.save == right.save
        && left.abbrev == right.abbrev;
}

void test_time_zone_and_link(const tzdb& my_tzdb, string_view tz_name, string_view tz_link_name) {
    const auto original_tz = my_tzdb.locate_zone(tz_name);
    assert(original_tz != nullptr);
    assert(original_tz->name() == tz_name);

    const auto linked_tz = my_tzdb.locate_zone(tz_link_name);
    assert(linked_tz != nullptr);
    assert(linked_tz->name() == tz_name);
    assert(original_tz == linked_tz);

    const auto tz_link = _Locate_zone_impl(my_tzdb.links, tz_link_name);
    assert(tz_link != nullptr);
    assert(tz_link->name() == tz_link_name);
    assert(tz_link->target() == tz_name);
    assert(my_tzdb.locate_zone(tz_link->target()) == original_tz);

    assert(_Locate_zone_impl(my_tzdb.zones, tz_name) != nullptr);
    assert(_Locate_zone_impl(my_tzdb.zones, tz_link_name) == nullptr);
    assert(_Locate_zone_impl(my_tzdb.links, tz_name) == nullptr);
}

void try_locate_invalid_zone(const tzdb& my_tzdb, string_view name) {
    try {
        (void) my_tzdb.locate_zone(name);
        assert(false);
    } catch (const runtime_error&) {
    }
}

void timezone_tzdb_list_test() {
    const auto& my_tzdb_list = get_tzdb_list();

    // only one entry in the list unless leap seconds were to change
    assert(&my_tzdb_list.front() == &get_tzdb());
    assert(&my_tzdb_list.front() == &reload_tzdb());
    assert(++my_tzdb_list.begin() == my_tzdb_list.end());
    assert(++my_tzdb_list.cbegin() == my_tzdb_list.cend());
}

void timezone_version_test() {
    const auto& my_tzdb = get_tzdb();
    assert(my_tzdb.version.empty() == false);

    // version should end in .X where X == number of leap seconds
    const auto pos = my_tzdb.version.find_last_of('.');
    assert(pos != decltype(my_tzdb.version)::npos);
    const string leap_seconds{my_tzdb.version, pos + 1};
    assert(leap_seconds.empty() == false);
    assert(leap_seconds == to_string(my_tzdb.leap_seconds.size()));

    // remote version will only differ if leap seconds info changes, will not occur in tests
    const auto& reloaded_tzdb = reload_tzdb();
    assert(reloaded_tzdb.version.empty() == false);
    assert(&reloaded_tzdb == &my_tzdb);

    const auto& remote_ver = remote_version();
    assert(remote_ver.empty() == false);
    assert(remote_ver == my_tzdb.version);
}

void timezone_names_test() {
    const auto& my_tzdb = get_tzdb();

    test_time_zone_and_link(my_tzdb, "Asia/Thimphu", "Asia/Thimbu");
    test_time_zone_and_link(my_tzdb, "America/Tijuana", "America/Ensenada");

    const auto curr_zone = current_zone();
    assert(curr_zone != nullptr);
    assert(curr_zone->name().empty() == false);
    assert(curr_zone == my_tzdb.current_zone());

    const auto located_zone = locate_zone("UTC");
    assert(located_zone != nullptr);
    assert(located_zone->name() == "Etc/UTC");
    assert(located_zone == my_tzdb.locate_zone("UTC"));

    try_locate_invalid_zone(my_tzdb, "Non/Existent");

    // Abbreviations should not be time_zones or time_zone_links
    try_locate_invalid_zone(my_tzdb, "PDT");
    try_locate_invalid_zone(my_tzdb, "AEST");

    // Comparison operators
    const time_zone tz1{_Secret_time_zone_construct_tag{}, "Earlier"};
    const time_zone tz2{_Secret_time_zone_construct_tag{}, "Earlier"};
    const time_zone tz3{_Secret_time_zone_construct_tag{}, "Later"};
    assert(tz1 == tz2);
    assert(tz1 != tz3);
    assert(tz1 <=> tz2 == strong_ordering::equal);
    assert(tz1 <=> tz3 == strong_ordering::less);
    assert(tz3 <=> tz1 == strong_ordering::greater);

    const time_zone_link link1{_Secret_time_zone_link_construct_tag{}, "Earlier", "Target"};
    const time_zone_link link2{_Secret_time_zone_link_construct_tag{}, "Earlier", "Is"};
    const time_zone_link link3{_Secret_time_zone_link_construct_tag{}, "Later", "Ignored"};
    assert(link1 == link2);
    assert(link1 != link3);
    assert(link1 <=> link2 == strong_ordering::equal);
    assert(link1 <=> link3 == strong_ordering::less);
    assert(link3 <=> link1 == strong_ordering::greater);

    try {
        // ensure locate_zone returns time_zone with given name
        assert(all_of(my_tzdb.zones.begin(), my_tzdb.zones.end(),
            [&](const auto& zone) { return my_tzdb.locate_zone(zone.name())->name() == zone.name(); }));
        // ensure locate_zone returns correct target of time_zone_link
        assert(all_of(my_tzdb.links.begin(), my_tzdb.links.end(),
            [&](const auto& link) { return my_tzdb.locate_zone(link.name())->name() == link.target(); }));
        // ensure locate_zone does NOT return time_zone that is also a time_zone_link
        assert(all_of(my_tzdb.links.begin(), my_tzdb.links.end(),
            [&](const auto& link) { return my_tzdb.locate_zone(link.name())->name() != link.name(); }));
    } catch (const runtime_error&) {
        assert(false);
    }

    // See GH-1786. These may change over time and might have to be removed from this test.
    // These are some examples in which the ICU.dll and IANA database diverge in what they consider a zone or a link.
    assert(_Locate_zone_impl(my_tzdb.links, "Atlantic/Faroe") != nullptr); // is a time_zone in IANA
    assert(_Locate_zone_impl(my_tzdb.zones, "Africa/Addis_Ababa") != nullptr); // is a time_zone_link in IANA
    assert(_Locate_zone_impl(my_tzdb.links, "PST") != nullptr); // time_zone_link does not exist in IANA
    assert(_Locate_zone_impl(my_tzdb.links, "Africa/Asmara") != nullptr); // matches IANA but target is different
    assert(_Locate_zone_impl(my_tzdb.links, "Africa/Asmara")->target() == "Africa/Asmera"); // target == Africa/Nairobi
    assert(_Locate_zone_impl(my_tzdb.zones, "America/Nuuk") == nullptr); // added in ICU 68, update test when it arrives
}

void validate_timezone_transitions(const time_zone* tz, const Transition& transition) {
    auto info = tz->get_info(transition.begin());
    assert(info.begin == transition.begin());
    assert(info.end == transition.end());
    assert(info.offset == transition.offset());
    assert(info.save == transition.save());
    assert(info.abbrev == transition.abbrev() || info.abbrev == transition.abbrev_alt());
}

void timezone_sys_info_test() {
    const auto& my_tzdb = get_tzdb();

    auto sydney_tz = my_tzdb.locate_zone(Sydney::Tz_name);
    assert(sydney_tz != nullptr);
    validate_timezone_transitions(sydney_tz, Sydney::Day_1);
    validate_timezone_transitions(sydney_tz, Sydney::Std_1);
    validate_timezone_transitions(sydney_tz, Sydney::Day_2);

    auto la_tz = my_tzdb.locate_zone(LA::Tz_name);
    assert(la_tz != nullptr);
    validate_timezone_transitions(la_tz, LA::Day_1);
    validate_timezone_transitions(la_tz, LA::Std_1);
    validate_timezone_transitions(la_tz, LA::Day_2);

    auto begin_info  = sydney_tz->get_info(Sydney::Std_1.begin());
    auto middle_info = sydney_tz->get_info(Sydney::Std_1.begin() + days{1});
    auto end_info    = sydney_tz->get_info(Sydney::Std_1.end());
    assert(sys_equal(begin_info, middle_info));
    assert(!sys_equal(begin_info, end_info));

    auto min_info = sydney_tz->get_info(time_zone::_Min_seconds);
    auto max_info = sydney_tz->get_info(time_zone::_Max_seconds - seconds{1});
    assert(min_info.begin == time_zone::_Min_seconds);
    assert(min_info.end != time_zone::_Max_seconds);
    assert(max_info.begin != time_zone::_Min_seconds);
    assert(max_info.end == time_zone::_Max_seconds);

    auto utc_zone = my_tzdb.locate_zone("Etc/UTC");
    assert(utc_zone != nullptr);

    auto min_utc = utc_zone->get_info(time_zone::_Min_seconds);
    auto max_utc = utc_zone->get_info(time_zone::_Max_seconds - seconds{1});
    // Only a single transition in UTC
    assert(sys_equal(min_utc, max_utc));
    assert(min_utc.begin < max_utc.end);
    assert(min_utc.begin == time_zone::_Min_seconds);
    assert(min_utc.end == time_zone::_Max_seconds);

    // Test abbreviations other than standard/daylight savings such as war time.
    // These scenarios are not handled correctly by icu.dll
    auto war_time = la_tz->get_info(sys_days{year{1942} / April / day{1}});
    assert(war_time.abbrev == "PDT" || war_time.abbrev == "GMT-7"); // IANA database == "PWT"
}

void timezone_to_local_test() {
    const auto& my_tzdb = get_tzdb();
    {
        using namespace Sydney;
        auto tz = my_tzdb.locate_zone(Tz_name);
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
        auto tz = my_tzdb.locate_zone(Tz_name);
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

void assert_local(const time_zone* tz, local_seconds local, int result, const sys_info& first, const sys_info& second) {
    const auto info = tz->get_info(local);
    assert(info.result == result);
    assert(sys_equal(info.first, first));
    assert(sys_equal(info.second, second));

    // time_zone::to_sys depends heavily on local_info so just test it here
    // to exhaust all corner cases.
    sys_seconds sys_earliest{local.time_since_epoch() - info.first.offset};
    sys_seconds sys_latest{local.time_since_epoch() - info.second.offset};
    try {
        assert(tz->to_sys(local) == sys_earliest);
        assert(result == local_info::unique);
    } catch (const nonexistent_local_time&) {
        assert(result == local_info::nonexistent);
    } catch (const ambiguous_local_time&) {
        assert(result == local_info::ambiguous);
    }

    if (result == local_info::unique) {
        assert(tz->to_sys(local, choose::earliest) == sys_earliest);
        assert(tz->to_sys(local, choose::latest) == sys_earliest);
    } else if (result == local_info::nonexistent) {
        assert(tz->to_sys(local, choose::earliest) == info.first.end);
        assert(tz->to_sys(local, choose::latest) == info.first.end);
    } else if (result == local_info::ambiguous) {
        assert(tz->to_sys(local, choose::earliest) == sys_earliest);
        assert(tz->to_sys(local, choose::latest) == sys_latest);
    }
}

void validate_get_local_info(const time_zone* tz, const pair<Transition, Transition>& transition, int result) {
    sys_info default_info{};
    sys_info first  = tz->get_info(transition.first.begin());
    sys_info second = tz->get_info(transition.second.begin());

    // Get the local time for the beginning of the ambiguous/nonexistent section
    const auto danger_begin = get_danger_begin(transition.first, transition.second);
    const auto danger_end   = get_danger_end(transition.first, transition.second);
    assert_local(tz, danger_begin - days{2}, local_info::unique, first, default_info); // two days before
    assert_local(tz, danger_begin - hours{1}, local_info::unique, first, default_info); // one hour before
    assert_local(tz, danger_begin, result, first, second); // danger begin
    assert_local(tz, danger_begin + minutes{30}, result, first, second); // danger middle
    assert_local(tz, danger_end, local_info::unique, second, default_info); // danger end
    assert_local(tz, danger_end + hours{1}, local_info::unique, second, default_info); // one hour after
    assert_local(tz, danger_end + days{2}, local_info::unique, second, default_info); // two days after
}

void timezone_local_info_test() {
    const auto& my_tzdb = get_tzdb();
    {
        // positive offset (UTC+10/+11) can fall in previous transition
        using namespace Sydney;
        auto tz = my_tzdb.locate_zone(Tz_name);
        assert(tz != nullptr);
        validate_get_local_info(tz, Day_to_Std, local_info::ambiguous);
        validate_get_local_info(tz, Std_to_Day, local_info::nonexistent);
    }
    {
        // negative offset (UTC-8/-7) can fall in next transition
        using namespace LA;
        auto tz = my_tzdb.locate_zone(Tz_name);
        assert(tz != nullptr);
        validate_get_local_info(tz, Day_to_Std, local_info::ambiguous);
        validate_get_local_info(tz, Std_to_Day, local_info::nonexistent);
    }
}

template <typename Dur>
void validate_precision(const time_zone* tz, const pair<Transition, Transition>& transition_pair, Dur precision) {
    const auto& first       = transition_pair.first;
    const auto& second      = transition_pair.second;
    const auto transition   = first.end<Dur>();
    const auto danger_begin = get_danger_begin<Dur>(first, second);
    const auto danger_end   = get_danger_end<Dur>(first, second);
    sys_info first_info     = tz->get_info(first.begin<Dur>());
    sys_info second_info    = tz->get_info(second.begin<Dur>());

    // test correct transition is picked
    assert(sys_equal(tz->get_info(transition), second_info));
    assert(sys_equal(tz->get_info(transition - precision), first_info));

    // test ambiguous/nonexistent info is handled
    assert(tz->get_info(danger_end).result == local_info::unique); // exact end of danger zone
    assert(tz->get_info(danger_end - precision).result != local_info::unique); // just inside danger zone
    assert(tz->get_info(danger_begin).result != local_info::unique); // exact start of danger zone
    assert(tz->get_info(danger_begin - precision).result == local_info::unique); // just before danger zone

    // test precision is not lost when converting to local
    assert(tz->to_local(transition) == second.local_begin<Dur>());
    assert(tz->to_local(transition + precision) == second.local_begin<Dur>() + precision);
    assert(tz->to_local(transition - precision) == first.local_end<Dur>() - precision);

    // test precision is not lost when converting to sys
    try {
        const sys_time<Dur> sys_danger_begin = transition - first.save();
        const sys_time<Dur> sys_danger_end   = transition + first.save();
        assert(tz->to_sys(danger_end) == sys_danger_end);
        assert(tz->to_sys(danger_end + precision) == sys_danger_end + precision);
        assert(tz->to_sys(danger_begin - precision) == sys_danger_begin - precision);
    } catch (const nonexistent_local_time&) {
        assert(false);
    } catch (const ambiguous_local_time&) {
        assert(false);
    }

    try {
        // test ambiguous/nonexistent info is found
        (void) tz->to_sys(danger_end - precision);
        assert(false);
    } catch (const nonexistent_local_time&) {
    } catch (const ambiguous_local_time&) {
    }
}

void timezone_precision_test() {
    const auto& my_tzdb = get_tzdb();

    {
        using namespace Sydney;
        auto tz = my_tzdb.locate_zone(Tz_name);
        validate_precision(tz, Std_to_Day, sys_seconds::duration{1});

        validate_precision(tz, Std_to_Day, milliseconds{100});
        validate_precision(tz, Std_to_Day, milliseconds{10});
        validate_precision(tz, Std_to_Day, milliseconds{1});

        validate_precision(tz, Std_to_Day, microseconds{100});
        validate_precision(tz, Std_to_Day, microseconds{10});
        validate_precision(tz, Std_to_Day, microseconds{1});

        // validate opposite transition
        validate_precision(tz, Day_to_Std, milliseconds{1});
        validate_precision(tz, Day_to_Std, microseconds{1});
    }
    {
        using namespace LA;
        auto tz = my_tzdb.locate_zone(Tz_name);
        validate_precision(tz, Std_to_Day, sys_seconds::duration{1});

        validate_precision(tz, Std_to_Day, milliseconds{100});
        validate_precision(tz, Std_to_Day, milliseconds{10});
        validate_precision(tz, Std_to_Day, milliseconds{1});

        validate_precision(tz, Std_to_Day, microseconds{100});
        validate_precision(tz, Std_to_Day, microseconds{10});
        validate_precision(tz, Std_to_Day, microseconds{1});

        // validate opposite transition
        validate_precision(tz, Day_to_Std, milliseconds{1});
        validate_precision(tz, Day_to_Std, microseconds{1});
    }
}

void timezone_sorted_vectors_test() {
    // N5008 [time.zone.db.tzdb]/1: "Each vector in a tzdb object is sorted to enable fast lookup."
    const auto& my_tzdb = get_tzdb();

    assert(ranges::is_sorted(my_tzdb.zones));
    assert(ranges::is_sorted(my_tzdb.links));
    assert(ranges::is_sorted(my_tzdb.leap_seconds));
}

void test() {
    timezone_tzdb_list_test();
    timezone_version_test();
    timezone_names_test();
    timezone_sys_info_test();
    timezone_to_local_test();
    timezone_local_info_test();
    timezone_precision_test();
    timezone_sorted_vectors_test();
}

int main() {
    run_tz_test([] { test(); });
}
