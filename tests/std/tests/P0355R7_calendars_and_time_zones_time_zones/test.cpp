// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <string_view>

using namespace std;
using namespace std::chrono;

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

    // FIXME: add a link to an issue. These may change overtime and might have to be removed from tests.

    // these are some example in which the ICU.dll and IANA database diverge in what they consider a zone or a link
    assert(_Locate_zone_impl(tzdb.links, "Atlantic/Faroe") != nullptr); // is a time_zone in IANA
    assert(_Locate_zone_impl(tzdb.zones, "Africa/Addis_Ababa") != nullptr); // is a time_zone_link in IANA
    assert(_Locate_zone_impl(tzdb.links, "PST") != nullptr); // time_zone_link does not exist in IANA
    assert(_Locate_zone_impl(tzdb.links, "Africa/Asmara") != nullptr); // matches IANA but target is wrong
    assert(_Locate_zone_impl(tzdb.links, "Africa/Asmara")->target() == "Africa/Asmera"); // target == Africa/Nairobi
    assert(_Locate_zone_impl(tzdb.zones, "America/Nuuk") == nullptr); // does not exist in ICU (very rare)
}

bool test() {
    try {
        timezone_names_test();
    } catch (exception& ex) {
        cerr << "Test threw exception: " << ex.what() << "\n";
        assert(false);
    }

    return true;
}

int main() {
    test();
}
