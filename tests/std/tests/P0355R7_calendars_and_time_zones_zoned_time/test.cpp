// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <string_view>
#include <type_traits>

#include <timezone_data.hpp>

using namespace std;
using namespace std::chrono;
using ZT = zoned_traits<const time_zone*>;

struct time_zone_info {
    time_zone_info(string_view _name_, const time_zone* _tz_, seconds _sys_) : name(_name_), tz(_tz_), sys(_sys_) {}

    string_view name;
    const time_zone* tz;
    sys_seconds sys;
    local_seconds local{tz->to_local(sys)};
    zoned_time<seconds> zone{tz, sys};
};

void zonedtime_constructor_test() {
    const time_zone_info utc{"Etc/UTC", ZT::default_zone(), seconds{42}};
    const time_zone_info syd{Sydney::Tz_name, ZT::locate_zone(Sydney::Tz_name), seconds{20}};

    // ensure local conversions are valid
    assert(utc.tz->get_info(utc.local).result == local_info::unique);
    assert(syd.tz->get_info(syd.local).result == local_info::unique);

    {
        // defaulted copy
        zoned_time<seconds> zone{syd.zone};
        assert(zone.get_time_zone() == syd.tz);
        assert(zone.get_sys_time() == syd.sys);
    }
    {
        // (1) zoned_time();
        zoned_time<seconds> zone{};
        assert(zone.get_time_zone() == utc.tz);
        assert(zone.get_sys_time() == sys_seconds{});
    }
    {
        // (2) zoned_time(const sys_time<Duration>& st);
        zoned_time<seconds> zone{sys_seconds{seconds{1}}};
        assert(zone.get_time_zone() == utc.tz);
        assert(zone.get_sys_time() == sys_seconds{seconds{1}});
    }
    {
        // (3) explicit zoned_time(TimeZonePtr z);
        zoned_time<seconds> zone{&*syd.tz};
        assert(zone.get_time_zone() == syd.tz);
        assert(zone.get_sys_time() == sys_seconds{});

        // (4) explicit zoned_time(string_view name);
        assert(zoned_time<seconds>{syd.name} == zone);
    }
    {
        // (5) template<class Duration2>
        //     zoned_time(const zoned_time<Duration2, TimeZonePtr>& y);
        zoned_time<hours> zone_hours{syd.name, sys_time<hours>{hours{1}}};
        assert(zone_hours.get_time_zone() == syd.tz);
        assert(zone_hours.get_sys_time() == sys_time<seconds>{hours{1}});
    }
    {
        // (6) zoned_time(TimeZonePtr z, const sys_time<Duration>& st);
        zoned_time<seconds> zone{&*syd.tz, syd.sys};
        assert(zone.get_time_zone() == syd.tz);
        assert(zone.get_sys_time() == syd.sys);

        // (7) zoned_time(string_view name, const sys_time<Duration>& st);
        assert((zoned_time<seconds>{syd.name, syd.sys}) == zone);
    }
    {
        // (8) zoned_time(TimeZonePtr z, const local_time<Duration>& tp);
        zoned_time<seconds> zone{&*syd.tz, syd.local};
        assert(zone.get_time_zone() == syd.tz);
        assert(zone.get_sys_time() == syd.sys);

        // (9) zoned_time(string_view name, const local_time<Duration>& st);
        assert((zoned_time<seconds>{syd.name, syd.local}) == zone);
    }
    {
        // (10) zoned_time(TimeZonePtr z, const local_time<Duration>& tp, choose c);
        zoned_time<seconds> zone{&*syd.tz, syd.local, choose::earliest};
        assert(zone.get_time_zone() == syd.tz);
        assert(zone.get_sys_time() == syd.sys);

        // (11) zoned_time(string_view name, const local_time<Duration>& tp, choose c);
        assert((zoned_time<seconds>{syd.name, syd.local, choose::earliest}) == zone);
    }
    {
        // (12) template<class Duration2, class TimeZonePtr2>
        //      zoned_time(TimeZonePtr z, const zoned_time<Duration2, TimeZonePtr2>& y);
        zoned_time<seconds> z1{&*utc.tz, syd.zone};
        zoned_time<seconds> z2{&*syd.tz, utc.zone};
        assert(z1.get_time_zone() == utc.tz);
        assert(z1.get_sys_time() == syd.sys);
        assert(z2.get_time_zone() == syd.tz);
        assert(z2.get_sys_time() == utc.sys);

        // (13) template<class Duration2, class TimeZonePtr2>
        //      zoned_time(TimeZonePtr z, const zoned_time<Duration2, TimeZonePtr2>& y, choose);
        assert((zoned_time<seconds>{&*utc.tz, syd.zone, choose::earliest}) == z1);
        assert((zoned_time<seconds>{&*syd.tz, utc.zone, choose::earliest}) == z2);

        // (14) template<class Duration2, class TimeZonePtr2>
        //      zoned_time(string_view name, const zoned_time<Duration2, TimeZonePtr2>& y);
        assert((zoned_time<seconds>{utc.name, syd.zone}) == z1);
        assert((zoned_time<seconds>{syd.name, utc.zone}) == z2);

        // (15) template<class Duration2, class TimeZonePtr2>
        //      zoned_time(string_view name, const zoned_time<Duration2, TimeZonePtr2>& y, choose);
        assert((zoned_time<seconds>{utc.name, syd.zone, choose::earliest}) == z1);
        assert((zoned_time<seconds>{syd.name, utc.zone, choose::earliest}) == z2);

        // when (_Duration2 != _Duration)
        zoned_time<hours> zone_hours{&*syd.tz, sys_time<hours>{hours{1}}};
        zoned_time<seconds> zone{&*utc.tz, zone_hours};
        assert(zone.get_time_zone() == utc.tz);
        assert(zone.get_sys_time() == sys_time<seconds>{hours{1}});

        assert((zoned_time<seconds>{&*utc.tz, zone_hours, choose::earliest}) == zone);
        assert((zoned_time<seconds>{utc.name, zone_hours}) == zone);
        assert((zoned_time<seconds>{utc.name, zone_hours, choose::earliest}) == zone);
    }
}

void zonedtime_operator_test() {
    const auto utc_tz = ZT::default_zone();
    const zoned_time<seconds> utc_zone{};
    assert(utc_zone.get_time_zone() == utc_tz);
    assert(utc_zone.get_sys_time() == sys_seconds{});
    assert(utc_zone.get_local_time() == local_seconds{});

    const auto syd_tz     = ZT::locate_zone(Sydney::Tz_name);
    const auto transition = Sydney::Day_1;
    const auto sys        = transition.begin();
    const auto local      = syd_tz->to_local(transition.begin());
    assert(syd_tz->get_info(local).result == local_info::unique);

    zoned_time<seconds> zone{ZT::locate_zone(Sydney::Tz_name), sys};
    assert(zone.get_time_zone() == syd_tz);
    assert(zone.get_sys_time() == sys);
    assert(zone.get_local_time() == local);
    assert(zone.get_info().begin == sys);

    // set time to be == epoch
    zone = sys_seconds{};
    assert(zone.get_time_zone() == syd_tz);
    assert(zone.get_sys_time() == sys_seconds{});
    assert(zone.get_local_time() == syd_tz->to_local(sys_seconds{}));
    assert(zone.get_info().begin != sys);

    // reset sys_time to transition.begin() via local_time
    zone = local;
    assert(zone.get_time_zone() == syd_tz);
    assert(zone.get_sys_time() == sys);
    assert(zone.get_local_time() == local);
    assert(zone.get_info().begin == sys);
}

void zonedtime_exception_tests() {
    const auto syd_tz          = ZT::locate_zone(Sydney::Tz_name);
    const auto ambiguous_local = syd_tz->to_local(Sydney::Standard_begin_2020 - minutes{1});
    assert(syd_tz->get_info(ambiguous_local).result == local_info::ambiguous);

    // unsafe constructors
    try {
        (void) zoned_time<seconds>{ZT::locate_zone(Sydney::Tz_name), ambiguous_local};
        assert(false);
    } catch (const nonexistent_local_time&) {
    } catch (const ambiguous_local_time&) {
    }

    try {
        (void) zoned_time<seconds>{Sydney::Tz_name, ambiguous_local};
        assert(false);
    } catch (const nonexistent_local_time&) {
    } catch (const ambiguous_local_time&) {
    }

    // safe constructors
    try {
        (void) zoned_time<seconds>{ZT::locate_zone(Sydney::Tz_name), ambiguous_local, choose::earliest};
        (void) zoned_time<seconds>{Sydney::Tz_name, ambiguous_local, choose::earliest};
    } catch (const nonexistent_local_time&) {
        assert(false);
    } catch (const ambiguous_local_time&) {
        assert(false);
    }

    // unsafe operator
    try {
        zoned_time<seconds> zone{Sydney::Tz_name};
        zone = ambiguous_local;
        (void) zone;
        assert(false);
    } catch (const nonexistent_local_time&) {
    } catch (const ambiguous_local_time&) {
    }
}

struct Always_zero {
    [[nodiscard]] string_view name() const noexcept {
        return "Zero";
    }

    template <class Duration>
    [[nodiscard]] sys_info get_info(const sys_time<Duration>&) const {
        return {};
    }

    template <class Duration>
    [[nodiscard]] local_info get_info(const local_time<Duration>&) const {
        return {};
    }

    template <class Duration>
    [[nodiscard]] sys_time<common_type_t<Duration, seconds>> to_sys(const local_time<Duration>&) const {
        return sys_time<common_type_t<Duration, seconds>>{};
    }

    template <class Duration>
    [[nodiscard]] sys_time<common_type_t<Duration, seconds>> to_sys(const local_time<Duration>&, choose) const {
        return sys_time<common_type_t<Duration, seconds>>{};
    }

    template <class Duration>
    [[nodiscard]] local_time<common_type_t<Duration, seconds>> to_local(const sys_time<Duration>&) const {
        return local_time<common_type_t<Duration, seconds>>{};
    }
};

struct Has_default : Always_zero {};
struct Has_locate : Always_zero {};

Always_zero zero_zone{};
Has_default has_default_zone{};
Has_locate has_locate_zone{};

template <>
struct zoned_traits<const Has_default*> {
    [[nodiscard]] static const Has_default* default_zone() {
        return &has_default_zone;
    }

    // missing string_view parameter...
    [[nodiscard]] static const Has_locate* locate_zone() {
        return &has_locate_zone;
    }
};

template <>
struct zoned_traits<const Has_locate*> {
    [[nodiscard]] static const Has_locate* locate_zone(string_view) {
        return &has_locate_zone;
    }
};

void zonedtime_traits_test() {
    // operation using timezone should always result in zero
    using Always_zero_ptr = const Always_zero*;

    zoned_time<seconds, Always_zero_ptr> zone{&zero_zone, sys_seconds{seconds{1}}};
    assert(zone.get_time_zone() == &zero_zone);
    assert(zone.get_sys_time() == sys_seconds{seconds{1}});
    assert(sys_seconds{zone} == sys_seconds{seconds{1}});
    assert(zone.get_local_time() == local_seconds{});
    assert(local_seconds{zone} == local_seconds{});
    assert(zone.get_info().begin == sys_seconds{});

    zone = sys_seconds{seconds{2}};
    assert(zone.get_sys_time() == sys_seconds{seconds{2}});
    assert(sys_seconds{zone} == sys_seconds{seconds{2}});
    assert(zone.get_local_time() == local_seconds{});
    assert(local_seconds{zone} == local_seconds{});
    assert(zone.get_info().begin == sys_seconds{});

    zone = local_seconds{seconds{3}};
    assert(zone.get_sys_time() == sys_seconds{}); // zero because timezone is used to compute sys_seconds
    assert(sys_seconds{zone} == sys_seconds{});
    assert(zone.get_local_time() == local_seconds{});
    assert(local_seconds{zone} == local_seconds{});
    assert(zone.get_info().begin == sys_seconds{});
}

template <class TimeZonePtr, class Duration, bool Has_locate_zone, bool Result>
constexpr void assert_constructible_durations() {
    using Zoned_seconds  = zoned_time<seconds, TimeZonePtr>;
    using Zoned_duration = zoned_time<Duration, TimeZonePtr>;

    static_assert(is_constructible_v<Zoned_seconds, Zoned_duration&> == Result);
    static_assert(is_constructible_v<Zoned_seconds, TimeZonePtr, Zoned_duration&> == Result);
    static_assert(is_constructible_v<Zoned_seconds, string_view, Zoned_duration&> == (Result && Has_locate_zone));
    static_assert(is_constructible_v<Zoned_seconds, TimeZonePtr, Zoned_duration&> == Result);
    static_assert(is_constructible_v<Zoned_seconds, string_view, Zoned_duration&> == (Result && Has_locate_zone));
}

template <class TimeZonePtr, bool Has_default_zone, bool Has_locate_zone>
constexpr void assert_constructible() {
    using Zoned           = zoned_time<seconds, TimeZonePtr>;
    using Zoned_no_locate = zoned_time<seconds, const Always_zero*>;

    static_assert(is_constructible_v<Zoned> == Has_default_zone);
    static_assert(is_constructible_v<Zoned, const sys_time<seconds>&> == Has_default_zone);
    static_assert(is_constructible_v<Zoned, TimeZonePtr> == true);
    static_assert(is_constructible_v<Zoned, string_view> == Has_locate_zone);
    static_assert(is_constructible_v<Zoned, const Zoned&> == true);
    static_assert(is_constructible_v<Zoned, TimeZonePtr, const sys_time<seconds>&> == true);
    static_assert(is_constructible_v<Zoned, string_view, const sys_time<seconds>&> == Has_locate_zone);
    static_assert(is_constructible_v<Zoned, TimeZonePtr, const local_time<seconds>&> == true);
    static_assert(is_constructible_v<Zoned, string_view, const local_time<seconds>&> == Has_locate_zone);
    static_assert(is_constructible_v<Zoned, TimeZonePtr, const local_time<seconds>&, choose> == true);
    static_assert(is_constructible_v<Zoned, string_view, const local_time<seconds>&, choose> == Has_locate_zone);
    static_assert(is_constructible_v<Zoned, TimeZonePtr, const Zoned_no_locate&> == true);
    static_assert(is_constructible_v<Zoned, string_view, const Zoned_no_locate&> == Has_locate_zone);
    static_assert(is_constructible_v<Zoned, TimeZonePtr, const Zoned_no_locate&, choose> == true);
    static_assert(is_constructible_v<Zoned, string_view, const Zoned_no_locate&, choose> == Has_locate_zone);

    // when (_Duration2 != _Duration)
    assert_constructible_durations<TimeZonePtr, seconds, Has_locate_zone, true>();
    static_assert(is_convertible_v<sys_time<hours>, sys_time<seconds>> == true);
    assert_constructible_durations<TimeZonePtr, hours, Has_locate_zone, true>();
    static_assert(is_convertible_v<sys_time<milliseconds>, sys_time<seconds>> == false);
    assert_constructible_durations<TimeZonePtr, milliseconds, Has_locate_zone, false>();
}

constexpr void zonedtime_constraints_test() {
    assert_constructible<const time_zone*, true, true>();
    assert_constructible<const Always_zero*, false, false>();
    assert_constructible<const Has_default*, true, false>();
    assert_constructible<const Has_locate*, false, true>();
}

void test() {
    zonedtime_constructor_test();
    zonedtime_operator_test();
    zonedtime_exception_tests();
    zonedtime_traits_test();
    zonedtime_constraints_test();
}

int main() {
    run_tz_test([] { test(); });
}
