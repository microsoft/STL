// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <deque>
#include <initializer_list>
#include <mdspan>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

#include <test_mdspan_support.hpp>

using namespace std;

class ActionTracker {
public:
    constexpr explicit ActionTracker(int id_) noexcept : id{id_} {}

    constexpr ActionTracker(const ActionTracker& other) noexcept : id{other.id}, copy_constructed{true} {}

    constexpr ActionTracker(ActionTracker&& other) noexcept : id{exchange(other.id, -1)}, move_constructed{true} {}

    constexpr ActionTracker& operator=(const ActionTracker& other) noexcept {
        id            = other.id;
        copy_assigned = true;
        return *this;
    }

    constexpr ActionTracker& operator=(ActionTracker&& other) noexcept {
        id            = exchange(other.id, -1);
        move_assigned = true;
        return *this;
    }

    [[nodiscard]] constexpr int get_id() const noexcept {
        return id;
    }

    [[nodiscard]] constexpr bool is_copy_constructed() const noexcept {
        return copy_constructed;
    }

    [[nodiscard]] constexpr bool is_move_constructed() const noexcept {
        return move_constructed;
    }

    [[nodiscard]] constexpr bool is_copy_assigned() const noexcept {
        return copy_assigned;
    }

    [[nodiscard]] constexpr bool is_move_assigned() const noexcept {
        return move_assigned;
    }

    [[nodiscard]] constexpr bool is_swapped() const noexcept {
        return swapped;
    }

    friend constexpr void swap(ActionTracker& left, ActionTracker& right) noexcept {
        left.swapped  = true;
        right.swapped = true;
        swap(left.id, right.id);
        // leave the other members alone
    }

private:
    int id;
    bool copy_constructed = false;
    bool move_constructed = false;
    bool copy_assigned    = false;
    bool move_assigned    = false;
    bool swapped          = false;
};

enum class RequireId : bool { no, yes };

template <class MpPolicy = layout_right, RequireId ReqId = RequireId::yes>
struct TrackingLayout {
    template <class Extents>
        requires constructible_from<typename MpPolicy::template mapping<Extents>, Extents>
              && (check_layout_mapping_policy_requirements<MpPolicy, Extents>())
    class mapping : public ActionTracker {
    public:
        using extents_type       = Extents;
        using index_type         = extents_type::index_type;
        using rank_type          = extents_type::rank_type;
        using layout_type        = TrackingLayout<MpPolicy>;
        using underlying_mapping = MpPolicy::template mapping<extents_type>;

        constexpr explicit mapping(int id)
            requires default_initializable<underlying_mapping>
            : ActionTracker(id), mp() {}

        constexpr mapping(const extents_type& e, int id) : ActionTracker(id), mp(e) {}

        constexpr mapping(const extents_type& e)
            requires (!to_underlying(ReqId))
            : ActionTracker(-1), constructed_with_extents_only{true}, mp(e) {}

        template <class T>
        mapping(initializer_list<T>) = delete; // we should never use list-initialization in <mdspan>

        constexpr mapping(const mapping& other) : ActionTracker(other), mp(other.mp) {}

        constexpr mapping(mapping&& other) noexcept : ActionTracker(move(other)), mp(move(other.mp)) {}

        // NB: special constructor for check_construction_from_other_mdspan's effects test
        template <class OtherExtents>
            requires is_constructible_v<extents_type, OtherExtents>
        constexpr mapping(const mapping<OtherExtents>& other) : ActionTracker(other), mp(other.get_underlying()) {}

        constexpr mapping& operator=(const mapping&) = default;
        constexpr mapping& operator=(mapping&&)      = default;

        constexpr const extents_type& extents() const {
            return mp.extents();
        }

        template <class... IndexTypes>
        constexpr index_type operator()(IndexTypes... indices) const noexcept(noexcept(mp(indices...))) {
            return mp(indices...);
        }

        constexpr index_type required_span_size() const {
            return mp.required_span_size();
        }

        constexpr bool is_unique() const {
            return mp.is_unique();
        }

        constexpr bool is_exhaustive() const {
            return mp.is_exhaustive();
        }

        constexpr bool is_strided() const {
            return mp.is_strided();
        }

        constexpr index_type stride() const {
            return mp.stride();
        }

        static constexpr bool is_always_unique() {
            return underlying_mapping::is_always_unique();
        }

        static constexpr bool is_always_exhaustive() {
            return underlying_mapping::is_always_exhaustive();
        }

        static constexpr bool is_always_strided() {
            return underlying_mapping::is_always_strided();
        }

        constexpr bool operator==(const mapping& other) const {
            return mp == other.mp;
        }

        constexpr bool is_constructed_with_extents_only() const noexcept
            requires (!to_underlying(ReqId))
        {
            return constructed_with_extents_only;
        }

        constexpr const underlying_mapping& get_underlying() const noexcept {
            return mp;
        }

        friend constexpr void swap(mapping& left, mapping& right) noexcept {
            swap(static_cast<ActionTracker&>(left), static_cast<ActionTracker&>(right));
            swap(left.mp, right.mp);
        }

    private:
        bool constructed_with_extents_only = false;
        underlying_mapping mp;
    };
};

static_assert(check_layout_mapping_policy_requirements<TrackingLayout<layout_right>, dextents<int, 3>>());
static_assert(check_layout_mapping_policy_requirements<TrackingLayout<layout_left>, dextents<int, 8>>());

struct VectorBoolAccessor {
    using offset_policy    = VectorBoolAccessor;
    using element_type     = bool;
    using reference        = vector<bool>::reference;
    using data_handle_type = vector<bool>::iterator;

    constexpr reference access(data_handle_type handle, size_t off) const noexcept {
        return handle[static_cast<ptrdiff_t>(off)];
    }

    constexpr data_handle_type offset(data_handle_type handle, size_t off) const {
        return handle + static_cast<ptrdiff_t>(off);
    }
};

static_assert(check_accessor_policy_requirements<VectorBoolAccessor>());

template <class ElementType>
class TrackingDataHandle : public ActionTracker {
public:
    using data_handle_type = ElementType*;

    constexpr explicit TrackingDataHandle(int id, data_handle_type ptr_) noexcept : ActionTracker(id), ptr{ptr_} {}

    // NB: special constructor for check_construction_from_other_mdspan's effects test
    template <class OtherElementType>
        requires is_convertible_v<OtherElementType*, data_handle_type>
    constexpr TrackingDataHandle(const TrackingDataHandle<OtherElementType>& other) : ActionTracker(other) {}

    template <class T>
    TrackingDataHandle(initializer_list<T>) = delete; // we should never use list-initialization in <mdspan>

    constexpr TrackingDataHandle(const TrackingDataHandle& other) noexcept : ActionTracker(other), ptr{other.ptr} {}

    constexpr TrackingDataHandle(TrackingDataHandle&& other) noexcept
        : ActionTracker(move(other)), ptr{exchange(other.ptr, nullptr)} {}

    constexpr TrackingDataHandle& operator=(const TrackingDataHandle&) noexcept = default;
    constexpr TrackingDataHandle& operator=(TrackingDataHandle&&) noexcept      = default;

    constexpr data_handle_type get_ptr() const noexcept {
        return ptr;
    }

    friend constexpr void swap(TrackingDataHandle& left, TrackingDataHandle& right) noexcept {
        swap(static_cast<ActionTracker&>(left), static_cast<ActionTracker&>(right));
        swap(left.ptr, right.ptr);
    }

private:
    data_handle_type ptr;
};

template <class ElementType>
class AccessorWithTrackingDataHandle {
public:
    using offset_policy    = AccessorWithTrackingDataHandle;
    using element_type     = ElementType;
    using reference        = ElementType&;
    using data_handle_type = TrackingDataHandle<ElementType>;

    constexpr reference access(data_handle_type handle, size_t off) const {
        return handle.get_ptr()[off];
    }

    constexpr data_handle_type offset(data_handle_type handle, size_t off) const {
        return TrackingDataHandle{handle.get_id(), handle.get_ptr() + off};
    }
};

template <class ElementType>
class TrackingAccessor : public ActionTracker {
public:
    using offset_policy    = TrackingAccessor;
    using element_type     = ElementType;
    using reference        = ElementType&;
    using data_handle_type = TrackingDataHandle<ElementType>;

    constexpr explicit TrackingAccessor(int id) noexcept : ActionTracker(id) {}

    // NB: special constructor for check_construction_from_other_mdspan's effects test
    template <class OtherElementType>
        requires is_convertible_v<OtherElementType (*)[], element_type (*)[]>
    constexpr TrackingAccessor(const TrackingAccessor<OtherElementType>& other) : ActionTracker(other) {}

    constexpr reference access(data_handle_type handle, size_t off) const {
        return handle.get_ptr()[off];
    }

    constexpr data_handle_type offset(data_handle_type handle, size_t off) const {
        return data_handle_type{handle.get_id(), handle.get_ptr() + off};
    }

    friend constexpr void swap(TrackingAccessor& left, TrackingAccessor& right) noexcept {
        swap(static_cast<ActionTracker&>(left), static_cast<ActionTracker&>(right));
    }
};

static_assert(check_accessor_policy_requirements<TrackingAccessor<int>>());

template <class ElementType>
struct AccessorWithCustomOffsetPolicy {
    using offset_policy    = default_accessor<ElementType>;
    using element_type     = offset_policy::element_type;
    using reference        = offset_policy::reference;
    using data_handle_type = offset_policy::data_handle_type;

    AccessorWithCustomOffsetPolicy() = default;

    // NB: special constructor for check_construction_from_other_mdspan's explicitness test
    template <class OtherElementType>
        requires is_convertible_v<OtherElementType (*)[], element_type (*)[]>
    constexpr explicit AccessorWithCustomOffsetPolicy(AccessorWithCustomOffsetPolicy<OtherElementType>) noexcept {}

    constexpr operator const offset_policy&() const {
        return offpol;
    }

    constexpr reference access(data_handle_type handle, size_t off) const {
        return offpol(handle, off);
    }

    constexpr data_handle_type offset(data_handle_type handle, size_t off) const {
        return offpol.offset(handle, off);
    }

private:
    offset_policy offpol;
};

static_assert(check_accessor_policy_requirements<AccessorWithCustomOffsetPolicy<int>>());

template <class Ext, class Layout, template <class> class AccessorTemplate>
constexpr void check_modeled_concepts_and_member_types() {
    using Accessor = AccessorTemplate<float>;
    using Mds      = mdspan<float, Ext, Layout, Accessor>;

    // Check modeled concepts
    static_assert(copyable<Mds>);
    static_assert(is_nothrow_move_constructible_v<Mds>);
    static_assert(is_nothrow_move_assignable_v<Mds>);
    static_assert(is_nothrow_swappable_v<Mds>);
    static_assert(
        is_trivially_copyable_v<Mds>
        == (is_trivially_copyable_v<typename Mds::accessor_type> && is_trivially_copyable_v<typename Mds::mapping_type>
            && is_trivially_copyable_v<typename Mds::data_handle_type>) );

    // Check member types
    static_assert(same_as<typename Mds::extents_type, Ext>);
    static_assert(same_as<typename Mds::layout_type, Layout>);
    static_assert(same_as<typename Mds::accessor_type, Accessor>);
    static_assert(same_as<typename Mds::mapping_type, typename Layout::template mapping<Ext>>);
    static_assert(same_as<typename Mds::element_type, float>);
    static_assert(same_as<typename Mds::value_type, float>);
    static_assert(same_as<typename Mds::index_type, typename Ext::index_type>);
    static_assert(same_as<typename Mds::size_type, typename Ext::size_type>);
    static_assert(same_as<typename Mds::rank_type, typename Ext::rank_type>);
    static_assert(same_as<typename Mds::data_handle_type, typename Accessor::data_handle_type>);
    static_assert(same_as<typename Mds::reference, typename Accessor::reference>);
}

constexpr void check_observers() {
    using Ext = extents<int, 4, dynamic_extent, 3, dynamic_extent, 5>;
    using Mds = mdspan<int, Ext, layout_stride, TrackingAccessor<int>>;

    { // Check results
        static_assert(Mds::rank() == Ext::rank());
        static_assert(Mds::rank_dynamic() == Ext::rank_dynamic());
        static_assert(Mds::static_extent(0) == Ext::static_extent(0));
        static_assert(Mds::static_extent(1) == Ext::static_extent(1));
        static_assert(Mds::static_extent(2) == Ext::static_extent(2));
        static_assert(Mds::static_extent(3) == Ext::static_extent(3));
        static_assert(Mds::static_extent(4) == Ext::static_extent(4));
    }

    { // Check return types
        static_assert(same_as<decltype(Mds::rank()), size_t>);
        static_assert(same_as<decltype(Mds::rank_dynamic()), size_t>);
        static_assert(same_as<decltype(Mds::static_extent(0)), size_t>);
    }

    { // Check noexceptness
        static_assert(noexcept(Mds::rank()));
        static_assert(noexcept(Mds::rank_dynamic()));
        static_assert(noexcept(Mds::static_extent(0)));
    }
}

constexpr void check_default_constructor() {
    { // Check constraint: 'rank_dynamic() > 0'
        static_assert(is_nothrow_default_constructible_v<mdspan<const int, dextents<int, 1>>>); // strengthened
        static_assert(!is_default_constructible_v<mdspan<const int, extents<int, 3>>>);
        static_assert(!is_default_constructible_v<mdspan<const int, extents<int>>>);
    }

    { // Check constraints: 'is_default_constructible_v<data_handle_type>'
        static_assert(is_nothrow_default_constructible_v<
            mdspan<bool, dextents<int, 2>, layout_right, VectorBoolAccessor>>); // strengthened
        static_assert(
            !is_default_constructible_v<mdspan<bool, dextents<int, 2>, layout_right, TrackingAccessor<bool>>>);
    }

    { // Check constraint: 'is_default_constructible_v<mapping_type>'
        static_assert(!is_default_constructible_v<mdspan<int, extents<short, 4, 4>, layout_stride>>);
        static_assert(!is_default_constructible_v<mdspan<long, extents<long, 4, 4>, TrackingLayout<>>>);
    }

    { // Check constraint: 'is_default_constructible_v<accessor_type>'
        static_assert(is_nothrow_default_constructible_v<
            mdspan<float, dextents<int, 2>, layout_right, AccessorWithCustomOffsetPolicy<float>>>); // strengthened
        static_assert(
            !is_default_constructible_v<mdspan<float, dextents<int, 2>, layout_right, TrackingAccessor<float>>>);
    }

    { // Check effects
        mdspan<double, dextents<int, 3>, layout_stride, TrivialAccessor<double>> mds{};
        assert(mds.data_handle() == nullptr);
        assert((mds.mapping().strides() == array{0, 0, 1}));
        assert(mds.accessor().member == 0);
    }
}

constexpr void check_defaulted_copy_and_move_constructors() {
    using Ext           = extents<int, 2, 2, 2>;
    using Mds           = mdspan<short, Ext, TrackingLayout<>, TrackingAccessor<short>>;
    short bits_of_218[] = {1, 1, 0, 1, 1, 0, 1, 0};

    { // Check defaulted copy constructor
        Mds mds1{
            TrackingDataHandle<short>{2, bits_of_218}, TrackingLayout<>::mapping<Ext>(4), TrackingAccessor<short>{8}};
        Mds mds2{mds1};
        assert(mds2.data_handle().is_copy_constructed());
        assert(mds2.mapping().is_copy_constructed());
        assert(mds2.accessor().is_copy_constructed());
    }

    { // Check defaulted move constructor
        Mds mds1{
            TrackingDataHandle<short>{2, bits_of_218}, TrackingLayout<>::mapping<Ext>(4), TrackingAccessor<short>{8}};
        Mds mds2{move(mds1)};
        assert(mds2.data_handle().is_move_constructed());
        assert(mds2.mapping().is_move_constructed());
        assert(mds2.accessor().is_move_constructed());
    }
}

constexpr void check_data_handle_and_indices_pack_constructor() {
    { // Check constraint: '(is_convertible_v<OtherIndexTypes, index_type> && ...)'
        using Mds = mdspan<const int, dextents<long long, 3>>;
        static_assert(is_nothrow_constructible_v<Mds, int*, signed char, short, int>); // strengthened
        static_assert(is_nothrow_constructible_v<Mds, int*, long, long long, unsigned char>); // strengthened
        static_assert(
            is_nothrow_constructible_v<Mds, const int*, unsigned short, unsigned int, unsigned long>); // strengthened
        static_assert(is_nothrow_constructible_v<Mds, const int*, unsigned long long, int,
            ConvertibleToInt<int>>); // strengthened
        static_assert(!is_constructible_v<Mds, const int*, unsigned long long, int, NonConvertibleToAnything>);
    }

    { // Check constraint: '(is_nothrow_constructible<index_type, OtherIndexTypes> && ...)'
        using Mds = mdspan<double, dextents<int, 2>>;
        static_assert(is_nothrow_constructible_v<Mds, double*, size_t,
            ConvertibleToInt<ptrdiff_t, IsNothrow::yes>>); // strengthened
        static_assert(!is_constructible_v<Mds, double*, size_t, ConvertibleToInt<ptrdiff_t, IsNothrow::no>>);
    }

    { // Check constraint: 'N == rank() || N == rank_dynamic()'
        using Mds = mdspan<int*, extents<short, 2, 3, dynamic_extent, dynamic_extent>>;
        static_assert(!is_constructible_v<Mds, int**, int>);
        static_assert(is_nothrow_constructible_v<Mds, int**, int, int>); // strengthened
        static_assert(!is_constructible_v<Mds, int** const, int, int, int>);
        static_assert(is_nothrow_constructible_v<Mds, int** const, int, int, int, int>); // strengthened
    }

    { // Check constraint: 'is_constructible_v<mapping_type, extents_type>'
        static_assert(is_nothrow_constructible_v<mdspan<const int, dextents<int, 2>, layout_left>, int* const, int,
            int>); // strengthened
        static_assert(!is_constructible_v<mdspan<int, dextents<int, 2>, layout_stride>, int*, int, int>);
        static_assert(!is_constructible_v<mdspan<int, extents<int, 4, 4>, TrackingLayout<>>, int*, int, int>);
    }

    { // Check constraint: 'is_default_constructible_v<accessor_type>'
        static_assert(is_nothrow_constructible_v<mdspan<bool, dextents<int, 2>, layout_right, VectorBoolAccessor>,
            vector<bool>::iterator, int, int>); // strengthened
        static_assert(
            !is_constructible_v<mdspan<int, dextents<int, 2>, layout_right, TrackingAccessor<int>>, int*, int, int>);
    }

    { // Check explicitness
        using Mds = mdspan<bool, dextents<long long, 4>>;
        static_assert(NotImplicitlyConstructibleFrom<Mds, bool*, int, int, int, int>);
        static_assert(NotImplicitlyConstructibleFrom<Mds, bool*, signed char, short, long, long long>);
        static_assert(!NotImplicitlyConstructibleFrom<Mds, bool*, signed char, short, long>);
    }

    { // Check effects: 'direct-non-list-initializes ptr_ with std::move(p)'
        int ints[4] = {1, 2, 3, 4};
        mdspan<int, extents<int, 2, 2>, layout_right, AccessorWithTrackingDataHandle<int>> mds{
            TrackingDataHandle<int>{1, ints}, 2, 2};
        assert(mds.data_handle().is_move_constructed());
    }

    { // Check effects: 'direct-non-list-initializes map_ with
        // extents_type(static_cast<index_type>(std::move(exts))...)'
        using Ext = dextents<signed char, 2>;
        struct FunnyIndex {
            constexpr operator Ext::index_type() const& noexcept {
                return 1;
            }

            constexpr operator integral auto() && noexcept {
                return 1;
            }

            constexpr operator Ext::index_type() && noexcept {
                return 3;
            }
        };

        char digits[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
        FunnyIndex i;
        mdspan<char, Ext> mds{digits, i, i};
        assert(mds.extent(0) == 3);
        assert(mds.extent(1) == 3);
    }

    { // Check effects: 'value-initializes acc_'
        int ints[4] = {2, 4, 8, 16};
        mdspan<int, extents<int, 2, 2>, layout_left, TrivialAccessor<int>> mds{ints, 2, 2};
        assert(mds.accessor().member == 0);
    }
}

constexpr void check_data_handle_and_span_array_constructors() {
    { // Check constraint: 'is_convertible_v<const OtherIndexType&, index_type>'
        using Mds = mdspan<const int, dextents<long long, 3>>;
        static_assert(is_nothrow_constructible_v<Mds, int*, span<int, 3>>); // strengthened
        static_assert(is_nothrow_constructible_v<Mds, int*, array<int, 3>>); // strengthened
        static_assert(is_nothrow_constructible_v<Mds, const int*, span<ConvertibleToInt<short>, 3>>); // strengthened
        static_assert(is_nothrow_constructible_v<Mds, const int*, array<ConvertibleToInt<short>, 3>>); // strengthened
        static_assert(!is_constructible_v<Mds, const int*, span<NonConvertibleToAnything, 3>>);
        static_assert(!is_constructible_v<Mds, const int*, array<NonConvertibleToAnything, 3>>);
    }

    { // Check constraint: 'is_nothrow_constructible<index_type, OtherIndexTypes>'
        using Mds = mdspan<double, dextents<int, 2>>;
        static_assert(is_nothrow_constructible_v<Mds, double*,
            span<ConvertibleToInt<ptrdiff_t, IsNothrow::yes>, 2>>); // strengthened
        static_assert(is_nothrow_constructible_v<Mds, double*,
            array<ConvertibleToInt<ptrdiff_t, IsNothrow::yes>, 2>>); // strengthened
        static_assert(!is_constructible_v<Mds, double*, span<ConvertibleToInt<ptrdiff_t, IsNothrow::no>, 2>>);
        static_assert(!is_constructible_v<Mds, double*, array<ConvertibleToInt<ptrdiff_t, IsNothrow::no>, 2>>);
    }

    { // Check constraint: 'N == rank() || N == rank_dynamic()'
        using Mds = mdspan<int*, extents<short, 2, 3, dynamic_extent, dynamic_extent>>;
        static_assert(!is_constructible_v<Mds, int**, span<int, 1>>);
        static_assert(!is_constructible_v<Mds, int**, array<int, 1>>);
        static_assert(is_nothrow_constructible_v<Mds, int**, span<int, 2>>); // strengthened
        static_assert(is_nothrow_constructible_v<Mds, int**, array<int, 2>>); // strengthened
        static_assert(!is_constructible_v<Mds, int**, span<int, 3>>);
        static_assert(!is_constructible_v<Mds, int**, array<int, 3>>);
        static_assert(is_nothrow_constructible_v<Mds, int**, span<int, 4>>); // strengthened
        static_assert(is_nothrow_constructible_v<Mds, int**, array<int, 4>>); // strengthened
    }

    { // Check constraint: 'is_constructible_v<mapping_type, extents_type>'
        static_assert(is_nothrow_constructible_v<mdspan<const int, dextents<int, 2>, layout_left>, int* const,
            span<int, 2>>); // strengthened
        static_assert(is_nothrow_constructible_v<mdspan<const int, dextents<int, 2>, layout_left>, int* const,
            array<int, 2>>); // strengthened
        static_assert(
            is_constructible_v<mdspan<const int, dextents<int, 2>, TrackingLayout<layout_left, RequireId::no>>,
                int* const, span<int, 2>>);
        static_assert(
            !is_nothrow_constructible_v<mdspan<const int, dextents<int, 2>, TrackingLayout<layout_left, RequireId::no>>,
                int* const, span<int, 2>>); // strengthened
        static_assert(
            is_constructible_v<mdspan<const int, dextents<int, 2>, TrackingLayout<layout_left, RequireId::no>>,
                int* const, array<int, 2>>);
        static_assert(
            !is_nothrow_constructible_v<mdspan<const int, dextents<int, 2>, TrackingLayout<layout_left, RequireId::no>>,
                int* const, array<int, 2>>); // strengthened
        static_assert(!is_constructible_v<mdspan<int, dextents<int, 2>, layout_stride>, int*, span<int, 2>>);
        static_assert(!is_constructible_v<mdspan<int, dextents<int, 2>, layout_stride>, int*, array<int, 2>>);
        static_assert(!is_constructible_v<mdspan<int, dextents<int, 2>, TrackingLayout<>>, int*, span<int, 2>>);
        static_assert(!is_constructible_v<mdspan<int, dextents<int, 2>, TrackingLayout<>>, int*, array<int, 2>>);
    }

    { // Check constraint: 'is_default_constructible_v<accessor_type>'
        static_assert(is_nothrow_constructible_v<mdspan<bool, dextents<int, 2>, layout_right, VectorBoolAccessor>,
            vector<bool>::iterator, span<short, 2>>); // strengthened
        static_assert(is_nothrow_constructible_v<mdspan<bool, dextents<int, 2>, layout_right, VectorBoolAccessor>,
            vector<bool>::iterator, array<short, 2>>); // strengthened
        static_assert(!is_constructible_v<mdspan<int, dextents<int, 2>, layout_right, TrackingAccessor<int>>, int*,
            span<int, 2>>);
        static_assert(!is_constructible_v<mdspan<int, dextents<int, 2>, layout_right, TrackingAccessor<int>>, int*,
            array<int, 2>>);
    }

    { // Check explicitness
        using Mds = mdspan<const float, extents<int, 4, 4, dynamic_extent>>;
        static_assert(NotImplicitlyConstructibleFrom<Mds, float*, span<int, 3>>);
        static_assert(NotImplicitlyConstructibleFrom<Mds, float*, array<int, 3>>);
        static_assert(!NotImplicitlyConstructibleFrom<Mds, float*, span<int, 1>>);
        static_assert(!NotImplicitlyConstructibleFrom<Mds, float*, array<int, 1>>);
        static_assert(NotImplicitlyConstructibleFrom<Mds, const float*, span<ConvertibleToInt<short>, 3>>);
        static_assert(NotImplicitlyConstructibleFrom<Mds, const float*, array<ConvertibleToInt<short>, 3>>);
        static_assert(!NotImplicitlyConstructibleFrom<Mds, const float*, span<ConvertibleToInt<short>, 1>>);
        static_assert(!NotImplicitlyConstructibleFrom<Mds, const float*, array<ConvertibleToInt<short>, 1>>);
    }

    { // Check effects: 'direct-non-list-initializes ptr_ with std::move(p)'
        int ints[4] = {1, 2, 3, 4};
        array<int, 2> indices{2, 2};
        mdspan<int, extents<int, 2, 2>, layout_right, AccessorWithTrackingDataHandle<int>> mds1{
            TrackingDataHandle<int>{1, ints}, indices};
        assert(mds1.data_handle().is_move_constructed());
        span s{indices};
        mdspan<int, extents<int, 2, 2>, layout_right, AccessorWithTrackingDataHandle<int>> mds2{
            TrackingDataHandle<int>{1, ints}, s};
        assert(mds2.data_handle().is_move_constructed());
    }

    { // Check effects: 'direct-non-list-initializes map_ with extents_type(exts)'
        using Ext = dextents<signed char, 2>;
        struct FunnyIndex {
            constexpr operator integral auto() & noexcept {
                return 1;
            }

            constexpr operator integral auto() const& noexcept {
                return 1;
            }

            constexpr operator Ext::index_type() const& noexcept {
                return 3;
            }

            constexpr operator integral auto() && noexcept {
                return 1;
            }
        };

        char digits[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
        array<FunnyIndex, 2> indices;
        mdspan<char, Ext, TrackingLayout<layout_right, RequireId::no>> mds1{digits, indices};
        assert(mds1.extent(0) == 3);
        span s{indices};
        mdspan<char, Ext> mds2{digits, s};
        assert(mds2.extent(0) == 3);
        assert(mds2.extent(1) == 3);
    }

    { // Check effects: 'value-initializes acc_'
        int ints[4] = {1, 3, 7, 15};
        array<short, 2> indices{2, 2};
        mdspan<int, extents<int, 2, 2>, layout_left, TrivialAccessor<int>> mds1{ints, indices};
        assert(mds1.accessor().member == 0);
        span s{indices};
        mdspan<int, extents<int, 2, 2>, layout_left, TrivialAccessor<int>> mds2{ints, s};
        assert(mds2.accessor().member == 0);
    }
}

constexpr void check_data_handle_and_extents_constructor() {
    { // Check constraint: 'is_constructible_v<mapping_type, const extents_type&>'
        static_assert(
            is_nothrow_constructible_v<mdspan<int, dextents<int, 3>>, int*, dextents<int, 3>>); // strengthened
        static_assert(
            is_nothrow_constructible_v<mdspan<int, dextents<int, 3>>, int*, dextents<long, 3>>); // strengthened
        static_assert(!is_constructible_v<mdspan<int, dextents<int, 3>, layout_stride>, int*, dextents<int, 3>>);
        static_assert(!is_constructible_v<mdspan<int, dextents<int, 3>>, int*, dextents<int, 2>>);
        static_assert(!is_constructible_v<mdspan<int, extents<int, 3, 3>>, int*, extents<long, 3, 2>>);
    }

    { // Check constraint: is_default_constructible_v<accessor_type>
        static_assert(is_nothrow_constructible_v<mdspan<bool, dextents<int, 2>, layout_right, VectorBoolAccessor>,
            vector<bool>::iterator, dextents<short, 2>>); // strengthened
        static_assert(is_nothrow_constructible_v<mdspan<bool, dextents<int, 2>, layout_right, VectorBoolAccessor>,
            vector<bool>::iterator, extents<long, 3, 3>>); // strengthened
        static_assert(!is_constructible_v<mdspan<int, dextents<int, 2>, layout_right, TrackingAccessor<int>>, int*,
            dextents<signed char, 2>>);
        static_assert(!is_constructible_v<mdspan<int, dextents<int, 2>, layout_right, TrackingAccessor<int>>, int*,
            extents<unsigned char, 4, 4>>);
    }

    { // Check effects: 'direct-non-list-initializes ptr_ with std::move(p)'
        char physics[4] = {'s', 't', 'v', 'a'};
        mdspan<char, extents<int, 2, 2>, layout_right, AccessorWithTrackingDataHandle<char>> mds{
            TrackingDataHandle<char>{1, physics}, extents<short, 2, 2>{}};
        assert(mds.data_handle().is_move_constructed());
    }

    { // Check effects: "direct-non-list-initializes map_ with ext"
        short lucky_numbers[6] = {2, 15, 17, 31, 34, 35};
        mdspan<short, extents<short, 2, 3>, TrackingLayout<layout_left, RequireId::no>> mds{
            lucky_numbers, extents<signed char, 2, 3>{}};
        assert(mds.mapping().is_constructed_with_extents_only());
    }

    { // Check effects: 'value-initializes acc_'
        int ints[4] = {1, 22, 333, 4444};
        mdspan<int, dextents<int, 2>, layout_left, TrivialAccessor<int>> mds{ints, extents<long, 2, 2>{}};
        assert(mds.accessor().member == 0);
    }
}

constexpr void check_data_handle_and_mapping_constructor() {
    { // Check constraint: 'is_default_constructible_v<accessor_type>'
        static_assert(is_nothrow_constructible_v<mdspan<bool, dextents<int, 4>, layout_left, VectorBoolAccessor>,
            vector<bool>::iterator, layout_left::mapping<dextents<short, 4>>>); // strengthened
        static_assert(is_nothrow_constructible_v<mdspan<int, extents<short, 2, 2>, layout_left>, int* const,
            layout_left::mapping<extents<short, 2, 2>>>); // strengthened
        static_assert(!is_constructible_v<
            mdspan<vector<int>, extents<long, 5, 5>, TrackingLayout<>, TrackingAccessor<vector<int>>>, vector<int>*,
            TrackingLayout<>::mapping<extents<long, 5, 5>>>);
        static_assert(!is_constructible_v<
            mdspan<deque<int>, extents<signed char, 5, 5, 5>, TrackingLayout<>, TrackingAccessor<deque<int>>>,
            deque<int>* const, TrackingLayout<>::mapping<extents<signed char, 5, 5, 5>>>);
    }

    { // Check effect: 'direct-non-list-initializes ptr_ with std::move(p)'
        using Ext      = extents<int, 2, 1, 3>;
        char banana[6] = {'b', 'a', 'n', 'a', 'n', 'a'};
        mdspan<char, Ext, layout_stride, AccessorWithTrackingDataHandle<char>> mds{
            TrackingDataHandle<char>{1, banana}, layout_stride::mapping<Ext>{Ext{}, array{3, 6, 1}}};
        assert(mds.data_handle().is_move_constructed());
    }

    { // Check effect: 'direct-non-list-initializes map_ with m'
        using Ext = extents<signed char, 3, 3>;
        char x[9] = {'\\', ' ', '/', ' ', 'X', ' ', '/', ' ', '\\'};
        mdspan<char, Ext, TrackingLayout<>> mds{x, TrackingLayout<>::mapping<Ext>{Ext{}, 23}};
        assert(mds.mapping().is_copy_constructed());
    }

    { // Check effect: 'value-initializes acc_'
        using Ext    = extents<short, 2, 2, 3>;
        int twelve[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
        mdspan<int, Ext, layout_stride, TrivialAccessor<int>> mds{
            twelve, layout_stride::mapping<Ext>{Ext{}, array{2, 1, 4}}};
        assert(mds.accessor().member == 0);
    }
}

constexpr void check_data_handle_and_mapping_and_accessor_constructor() {
    { // Check effects
        using Ext = extents<signed char, 4, 4>;
        using Mds = mdspan<unsigned int, Ext, TrackingLayout<>, TrackingAccessor<unsigned int>>;

        unsigned int identity_matrix[] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
        Mds mds{TrackingDataHandle<unsigned int>{16, identity_matrix}, TrackingLayout<>::mapping<Ext>(17),
            TrackingAccessor<unsigned int>{18}};

        // Effects:
        // - Direct-non-list-initializes ptr_ with std::move(p),
        // - direct-non-list-initializes map_ with m, and
        // - direct-non-list-initializes acc_ with a.
        assert(mds.data_handle().is_move_constructed());
        assert(mds.data_handle().get_id() == 16);
        assert(mds.mapping().is_copy_constructed());
        assert(mds.mapping().get_id() == 17);
        assert(mds.accessor().is_copy_constructed());
        assert(mds.accessor().get_id() == 18);
    }

    { // Check noexceptness (strengthened)
        using Mds1 = mdspan<short, extents<int, 3, 3, 3>>;
        static_assert(is_nothrow_constructible_v<Mds1, Mds1::data_handle_type, const Mds1::mapping_type&,
            const Mds1::accessor_type&>);

        using Mds2 = mdspan<short, extents<int, 3, 3, 3>, TrackingLayout<>>;
        static_assert(!is_nothrow_constructible_v<Mds2, Mds2::data_handle_type, const Mds2::mapping_type&,
            const Mds2::accessor_type&>);
    }
}

constexpr void check_construction_from_other_mdspan() {
    { // Check constraint: 'is_constructible_v<mapping_type, const OtherLayoutPolicy::template
      // mapping<OtherExtents>&>'
        static_assert(is_nothrow_constructible_v<mdspan<int, extents<int, 4, 4, 4>, layout_stride>,
            mdspan<int, dextents<long, 3>, layout_right>>); // strengthened
        static_assert(!is_constructible_v<mdspan<float, dextents<long long, 2>, layout_left>,
            mdspan<float, extents<signed char, 3, 3>, layout_right>>);
        static_assert(!is_constructible_v<mdspan<double, dextents<unsigned int, 2>, layout_left>,
            mdspan<double, extents<unsigned short, 5, 5, 5>, layout_left>>);
    }

    { // Check constraint: 'is_constructible_v<accessor_type, const OtherAccessor&>'
        using Ext = extents<long, 8, 8, 8>;
        static_assert(
            is_nothrow_constructible_v<mdspan<const double, Ext, layout_right, default_accessor<const double>>,
                mdspan<double, Ext, layout_right, default_accessor<double>>>); // strengthened
        static_assert(!is_constructible_v<mdspan<const double, Ext, layout_right, TrivialAccessor<const double>>,
            mdspan<double, Ext, layout_right, TrivialAccessor<double>>>);
    }

    { // Check explicitness
        static_assert(NotImplicitlyConstructibleFrom<mdspan<int, extents<int, 4, 4>>, mdspan<int, dextents<long, 2>>>);
        static_assert(NotImplicitlyConstructibleFrom<mdspan<int, extents<int, 4, 4>, layout_left>,
            mdspan<int, dextents<long, 2>, layout_stride>>);
        static_assert(!NotImplicitlyConstructibleFrom<mdspan<int, dextents<long, 2>, layout_stride>,
            mdspan<int, extents<int, 4, 4>, layout_left>>);
        static_assert(NotImplicitlyConstructibleFrom<
            mdspan<const int, extents<int, 4, 4>, layout_left, AccessorWithCustomOffsetPolicy<const int>>,
            mdspan<int, extents<long, 4, 4>, layout_left, AccessorWithCustomOffsetPolicy<int>>>);
        static_assert(!NotImplicitlyConstructibleFrom<
            mdspan<const int, extents<int, 4, 4>, layout_left, default_accessor<const int>>,
            mdspan<int, extents<long, 4, 4>, layout_left, default_accessor<int>>>);
    }

    { // Check effects
        int data[] = {
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27};

        using Ext = dextents<int, 3>;
        mdspan<int, Ext, TrackingLayout<>, TrackingAccessor<int>> mds1{TrackingDataHandle<int>{4, data},
            TrackingLayout<>::mapping<Ext>{Ext{3, 3, 3}, 4}, TrackingAccessor<int>{4}};
        mdspan<const int, extents<int, 3, 3, 3>, TrackingLayout<>, TrackingAccessor<const int>> mds2{mds1};
        assert(mds2.data_handle().is_copy_constructed());
        assert(mds2.mapping().is_copy_constructed());
        assert(mds2.accessor().is_copy_constructed());
    }
}

constexpr void check_defaulted_copy_and_move_assignment_operators() {
    using Ext           = extents<int, 2, 2, 2>;
    using Mds           = mdspan<short, Ext, TrackingLayout<>, TrackingAccessor<short>>;
    short bits_of_218[] = {1, 1, 0, 1, 1, 0, 1, 0};
    short bits_of_248[] = {1, 1, 1, 1, 1, 0, 0, 0};

    { // Check defaulted copy assignment operator
        Mds mds1{
            TrackingDataHandle<short>{2, bits_of_218}, TrackingLayout<>::mapping<Ext>(4), TrackingAccessor<short>{8}};
        Mds mds2{
            TrackingDataHandle<short>{3, bits_of_248}, TrackingLayout<>::mapping<Ext>(5), TrackingAccessor<short>{9}};
        mds1 = mds2;
        assert(mds1.data_handle().is_copy_assigned());
        assert(mds1.mapping().is_copy_assigned());
        assert(mds1.accessor().is_copy_assigned());
    }

    { // Check defaulted move assignment operator
        Mds mds1{
            TrackingDataHandle<short>{2, bits_of_218}, TrackingLayout<>::mapping<Ext>(4), TrackingAccessor<short>{8}};
        Mds mds2{
            TrackingDataHandle<short>{3, bits_of_248}, TrackingLayout<>::mapping<Ext>(5), TrackingAccessor<short>{9}};
        mds1 = move(mds2);
        assert(mds1.data_handle().is_move_assigned());
        assert(mds1.mapping().is_move_assigned());
        assert(mds1.accessor().is_move_assigned());
    }
}

#ifdef __cpp_multidimensional_subscript // TRANSITION, P2128R6
template <class Mds, class... IndexTypes>
concept CanCallMultidimSubscriptOp = requires(const Mds& mds, IndexTypes... indices) {
    { mds[indices...] } -> same_as<typename Mds::reference>;
};

constexpr void check_multidimensional_subscript_operator() {
    { // Check constraint: '(is_convertible_v<OtherIndexTypes, index_type> && ...)'
        using Mds = mdspan<int, dextents<int, 5>>;
        static_assert(CanCallMultidimSubscriptOp<Mds, int, int, int, int, int>);
        static_assert(CanCallMultidimSubscriptOp<Mds, signed char, short, int, long, long long>);
        static_assert(CanCallMultidimSubscriptOp<Mds, unsigned char, unsigned short, unsigned int, unsigned long,
            unsigned long long>);
        static_assert(CanCallMultidimSubscriptOp<Mds, unsigned char, short, unsigned int, long, unsigned long long>);
        static_assert(CanCallMultidimSubscriptOp<Mds, ConvertibleToInt<int>, int, int, int, int>);
        static_assert(!CanCallMultidimSubscriptOp<Mds, int, int, int, int, NonConvertibleToAnything>);
    }

    { // Check constraint: '(is_nothrow_constructible_v<index_type, OtherIndexTypes> && ...)'
        using Mds = mdspan<char, dextents<signed char, 2>>;
        static_assert(CanCallMultidimSubscriptOp<Mds, ConvertibleToInt<int, IsNothrow::yes>, int>);
        static_assert(!CanCallMultidimSubscriptOp<Mds, ConvertibleToInt<int, IsNothrow::no>, int>);
    }

    { // Check constraint: 'sizeof...(OtherIndexTypes) == rank()'
        using Mds = mdspan<float, dextents<unsigned short, 3>>;
        static_assert(CanCallMultidimSubscriptOp<Mds, int, int, int>);
        static_assert(!CanCallMultidimSubscriptOp<Mds, int, int>);
        static_assert(!CanCallMultidimSubscriptOp<Mds, int, int, int, int>);
    }

    { // Check correctness
        using Ext = extents<unsigned char, 2, 2>;
        vector<bool> bools{true, false, false, true};
        mdspan<bool, Ext, TrackingLayout<layout_left>, VectorBoolAccessor> mds{
            bools.begin(), TrackingLayout<layout_left>::mapping<Ext>(5)};
        same_as<vector<bool>::reference> decltype(auto) r1 = mds[1, 1];
        assert(r1);
        same_as<vector<bool>::reference> decltype(auto) r2 = as_const(mds)[0, 1];
        assert(!r2);

        static_assert(noexcept(mds[1, 1])); // strengthened
        static_assert(noexcept(as_const(mds)[0, 1])); // strengthened
    }

    { // Check that indices are moved and then casted to 'index_type'
        using Ext = dextents<short, 2>;
        struct FunnyIndex {
            constexpr operator integral auto() const& noexcept {
                return 0;
            }

            constexpr operator integral auto() && noexcept {
                return 0;
            }

            constexpr operator Ext::index_type() && noexcept {
                return 1;
            }
        };

        int mat2x2[4] = {0, 0, 0, 1};
        mdspan<int, Ext> mds{mat2x2, 2, 2};
        FunnyIndex i;
        assert((mds[i, i] == 1));
    }

    { // Check that indices are passed by value
        struct WeirdIndex {
            WeirdIndex() = default;
            constexpr WeirdIndex(const WeirdIndex&) : val{1} {}
            constexpr WeirdIndex(WeirdIndex&&) : val{2} {}

            constexpr operator int() const noexcept {
                return val;
            }

            int val = 0;
        };

        int ten2x2x3[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
        mdspan<int, extents<unsigned char, 2, 2, 3>> mds{ten2x2x3};
        WeirdIndex i;
        assert((mds[i, i, move(i)] == 1));
    }
}
#endif // __cpp_multidimensional_subscript

template <class Mds, class IndexType, size_t Rank = Mds::rank()>
concept CanCallSubscriptOp = requires(const Mds& mds, span<IndexType, Rank> s, const array<IndexType, Rank>& a) {
    { mds[s] } -> same_as<typename Mds::reference>;
    { mds[a] } -> same_as<typename Mds::reference>;
};

constexpr void check_span_array_subscript_operator() {
    { // Check constraint: 'is_convertible_v<const OtherIndexType&, index_type>
        using Mds = mdspan<int, dextents<unsigned char, 3>>;
        static_assert(CanCallSubscriptOp<Mds, signed char>);
        static_assert(CanCallSubscriptOp<Mds, short>);
        static_assert(CanCallSubscriptOp<Mds, int>);
        static_assert(CanCallSubscriptOp<Mds, long>);
        static_assert(CanCallSubscriptOp<Mds, long long>);
        static_assert(CanCallSubscriptOp<Mds, unsigned char>);
        static_assert(CanCallSubscriptOp<Mds, unsigned short>);
        static_assert(CanCallSubscriptOp<Mds, unsigned int>);
        static_assert(CanCallSubscriptOp<Mds, unsigned long>);
        static_assert(CanCallSubscriptOp<Mds, unsigned long long>);
        static_assert(CanCallSubscriptOp<Mds, ConvertibleToInt<int>>);
        static_assert(!CanCallSubscriptOp<Mds, NonConvertibleToAnything>);
    }

    { // Check constraint: 'is_nothrow_constructible_v<index_type, const OtherIndexType&>'
        using Mds = mdspan<float, dextents<short, 3>>;
        static_assert(CanCallSubscriptOp<Mds, ConvertibleToInt<unsigned long long, IsNothrow::yes>>);
        static_assert(!CanCallSubscriptOp<Mds, ConvertibleToInt<unsigned short, IsNothrow::no>>);
    }

    { // Check function argument: '(span|array)::size() == rank()'
        using Mds = mdspan<float, extents<long, 2, 3, 5>>;
        static_assert(!CanCallSubscriptOp<Mds, int, 2>);
        static_assert(!CanCallSubscriptOp<Mds, int, 4>);
    }

    { // Check correctness
        using Ext = extents<unsigned short, 3, 3>;
        vector<bool> bools{true, true, true, true, false, true, true, true, true};
        mdspan<bool, Ext, TrackingLayout<>, VectorBoolAccessor> mds{bools.begin(), TrackingLayout<>::mapping<Ext>(99)};

        array<signed char, 2> a1{1, 1};
        same_as<vector<bool>::reference> decltype(auto) ar1 = mds[a1];
        assert(!ar1);
        span s1{a1};
        same_as<vector<bool>::reference> decltype(auto) sr1 = mds[s1];
        assert(!sr1);

        array<unsigned long long, 2> a2{2, 2};
        same_as<vector<bool>::reference> decltype(auto) ar2 = as_const(mds)[a2];
        assert(ar2);
        span s2{a2};
        same_as<vector<bool>::reference> decltype(auto) sr2 = as_const(mds)[s2];
        assert(sr2);
    }

    { // Check that indices are expanded in as_const function and passed to multidimensional subscript operator
        using Ext = dextents<unsigned int, 2>;
        struct FunkyIndex {
            FunkyIndex()                  = default;
            FunkyIndex(const FunkyIndex&) = delete;

            constexpr operator integral auto() const& noexcept {
                return 0;
            }

            constexpr operator integral auto() && noexcept {
                return 0;
            }

            constexpr operator Ext::index_type() const& noexcept {
                return 1;
            }
        };

        char alpha[4] = {'a', 'b', 'c', 'd'};
        mdspan<char, Ext> mds{alpha, 2, 2};
        array<FunkyIndex, 2> a;
        assert(mds[a] == 'd');
        span s{a};
        assert(mds[s] == 'd');
    }
}

constexpr void check_size() {
    const int some_data[60] = {};

    { // Not empty mdspan
        mdspan mds1{some_data, extents<int, 3, 4, 5>{}};
        assert(mds1.size() == 60);
        mdspan mds2{some_data, extents<int, dynamic_extent, 5>{12}};
        assert(mds2.size() == 60);
        mdspan mds3{some_data, dextents<int, 3>{2, 3, 5}};
        assert(mds3.size() == 30);
    }

    { // Empty mdspan
        mdspan mds1{some_data, extents<int, 12, 5, 0>{}};
        assert(mds1.size() == 0);
        mdspan mds2{some_data, extents<int, 3, dynamic_extent, 4>{0}};
        assert(mds2.size() == 0);
        mdspan mds3{some_data, extents<int, dynamic_extent, 3, 4>{0}};
        assert(mds3.size() == 0);
    }

    { // mdspan with 'rank() == 0'
        mdspan mds{some_data, extents<signed char>{}};
        assert(mds.size() == 1);
    }

    { // mdspan whose index space size would not be representable as index_type if 0 wasn't there
        mdspan mds1{some_data, extents<signed char, 127, 2, 0>{}};
        assert(mds1.size() == 0);
        mdspan mds2{some_data, dextents<short, 3>{32767, 3, 0}};
        assert(mds2.size() == 0);
    }

    { // Other properties
        mdspan mds{some_data, extents<short, 2, 2, 3, 5>{}};
        same_as<unsigned short> decltype(auto) s1 = mds.size();
        assert(s1 == 60);
        static_assert(noexcept(mds.size()));
        same_as<unsigned short> decltype(auto) s2 = as_const(mds).size();
        assert(s2 == 60);
        static_assert(noexcept(as_const(mds).size()));
    }
}

constexpr void check_empty() {
    const int some_data[24] = {};

    { // Not empty mdspan
        mdspan mds1{some_data, extents<int, 3, 3, 4>{}};
        assert(!mds1.empty());
        mdspan mds2{some_data, extents<int, 3, dynamic_extent, 4>{3}};
        assert(!mds2.empty());
        mdspan mds3{some_data, dextents<int, 3>{3, 3, 4}};
        assert(!mds3.empty());
    }

    { // Empty mdspan
        mdspan mds1{some_data, extents<int, 3, 3, 0>{}};
        assert(mds1.empty());
        mdspan mds2{some_data, extents<int, 3, dynamic_extent, 4>{0}};
        assert(mds2.empty());
        mdspan mds3{some_data, extents<int, dynamic_extent, 3, 4>{0}};
        assert(mds3.empty());
    }

    { // mdspan with 'rank() == 0'
        mdspan mds{some_data, extents<signed char>{}};
        assert(!mds.empty());
    }

    { // mdspan whose index space size would not be representable as index_type if 0 wasn't there
        mdspan mds1{some_data, extents<signed char, 127, 2, 0>{}};
        assert(mds1.empty());
        mdspan mds2{some_data, dextents<short, 3>{32767, 3, 0}};
        assert(mds2.empty());
    }

    { // Other properties
        mdspan mds{some_data, extents<int, 2, 3, 0>{}};
        same_as<bool> decltype(auto) b1 = mds.empty();
        assert(b1);
        static_assert(noexcept(mds.empty()));
        same_as<bool> decltype(auto) b2 = as_const(mds).empty();
        assert(b2);
        static_assert(noexcept(as_const(mds).empty()));
    }
}

constexpr void check_swap() {
    { // Check swapping with tracking types
        using E   = extents<int, 3, 3>;
        using Mds = mdspan<int, E, TrackingLayout<>, TrackingAccessor<int>>;
        static_assert(is_nothrow_swappable_v<Mds>);
        static_assert(!is_swappable_v<const Mds>);

        int a1[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
        Mds mds1{TrackingDataHandle<int>{1, a1}, TrackingLayout<>::mapping<E>(1), TrackingAccessor<int>{1}};
        int a2[9] = {3, 0, 0, 0, 3, 0, 0, 0, 3};
        Mds mds2{TrackingDataHandle<int>{3, a2}, TrackingLayout<>::mapping<E>(3), TrackingAccessor<int>{3}};
        swap(mds1, mds2);
        static_assert(is_void_v<decltype(swap(mds1, mds2))>);

        assert(mds1.data_handle().get_id() == 3);
        assert(mds1.data_handle().is_swapped());
        assert(mds1.mapping().get_id() == 3);
        assert(mds1.mapping().is_swapped());
        assert(mds1.accessor().get_id() == 3);
        assert(mds1.accessor().is_swapped());
        assert((mds1[array{1, 1}] == 3));
        assert((mds1[array{0, 1}] == 0));

        assert(mds2.data_handle().get_id() == 1);
        assert(mds2.data_handle().is_swapped());
        assert(mds2.mapping().get_id() == 1);
        assert(mds2.mapping().is_swapped());
        assert(mds2.accessor().get_id() == 1);
        assert(mds2.accessor().is_swapped());
        assert((mds2[array{1, 1}] == 1));
        assert((mds2[array{0, 1}] == 0));
    }

    { // Check swapping with standard layout and accessor
        using Mds = mdspan<int, extents<int, 2, 2>>;
        static_assert(is_nothrow_swappable_v<Mds>);
        static_assert(!is_swappable_v<const Mds>);

        int diag[] = {1, 0, 0, 1};
        Mds mds1{diag};
        int revdiag[] = {0, 1, 1, 0};
        Mds mds2{revdiag};

        swap(mds1, mds2);
        assert(mds1.data_handle() == revdiag);
        assert(mds2.data_handle() == diag);
    }
}

constexpr void check_getters() {
    int data[6] = {1, 2, 3, 4, 5, 6};
    auto mds    = mdspan(data, 2, 3);

    { // Check 'extents()'
        same_as<const dextents<size_t, 2>&> decltype(auto) e = mds.extents();
        assert((e == dextents<int, 2>{2, 3}));
        assert(&e == &mds.mapping().extents());
        static_assert(noexcept(mds.extents()));
        same_as<const dextents<size_t, 2>&> decltype(auto) ce = as_const(mds).extents();
        assert(&ce == &e);
        static_assert(noexcept(as_const(mds).extents()));
    }

    { // Check 'data_handle()'
        same_as<int* const&> decltype(auto) dh = mds.data_handle();
        assert(dh == data);
        static_assert(noexcept(mds.data_handle()));
        same_as<int* const&> decltype(auto) cdh = as_const(mds).data_handle();
        assert(&cdh == &dh);
        static_assert(noexcept(as_const(mds).data_handle()));
    }

    { // Check 'mapping()'
        using E = dextents<size_t, 2>;

        same_as<const layout_right::mapping<E>&> decltype(auto) mp = mds.mapping();
        assert((mp == layout_stride::mapping<E>{E{2, 3}, array{3, 1}}));
        static_assert(noexcept(mds.mapping()));
        same_as<const layout_right::mapping<E>&> decltype(auto) cmp = as_const(mds).mapping();
        assert(&cmp == &mp);
        static_assert(noexcept(as_const(mds).mapping()));
    }

    { // Check 'accessor()'
        same_as<const default_accessor<int>&> decltype(auto) acc = mds.accessor();
        static_assert(noexcept(mds.accessor()));
        same_as<const default_accessor<int>&> decltype(auto) cacc = as_const(mds).accessor();
        assert(&cacc == &acc);
        static_assert(noexcept(as_const(mds).accessor()));
    }
}

constexpr void check_is_always_functions() {
    using Mds = mdspan<double, extents<long long, 4, 5, dynamic_extent>, layout_stride, TrivialAccessor<double>>;

    { // Check results
        static_assert(Mds::is_always_unique() == Mds::mapping_type::is_always_unique());
        static_assert(Mds::is_always_exhaustive() == Mds::mapping_type::is_always_exhaustive());
        static_assert(Mds::is_always_strided() == Mds::mapping_type::is_always_strided());
    }

    { // Check types
        static_assert(same_as<decltype(Mds::is_always_unique()), bool>);
        static_assert(same_as<decltype(Mds::is_always_exhaustive()), bool>);
        static_assert(same_as<decltype(Mds::is_always_strided()), bool>);
    }

    { // Check noexceptness (strengthened)
        static_assert(noexcept(Mds::is_always_unique()) == noexcept(Mds::mapping_type::is_always_unique()));
        static_assert(noexcept(Mds::is_always_exhaustive()) == noexcept(Mds::mapping_type::is_always_exhaustive()));
        static_assert(noexcept(Mds::is_always_strided()) == noexcept(Mds::mapping_type::is_always_strided()));
    }
}

constexpr void check_is_functions() {
    using E            = extents<int, 2, 2, 3>;
    vector<bool> bools = {true, false, true, true, true, true, true, false, true, false, false, false};
    mdspan<bool, E, layout_stride, VectorBoolAccessor> mds{
        bools.begin(), layout_stride::mapping<E>{E{}, array{6, 1, 2}}};

    { // Check results
        assert(mds.is_unique() == mds.mapping().is_unique());
        assert(mds.is_exhaustive() == mds.mapping().is_exhaustive());
        assert(mds.is_strided() == mds.mapping().is_strided());
    }

    { // Check types
        static_assert(same_as<decltype(mds.is_unique()), bool>);
        static_assert(same_as<decltype(mds.is_exhaustive()), bool>);
        static_assert(same_as<decltype(mds.is_strided()), bool>);
    }

    { // Check noexceptness (strengthened)
        static_assert(noexcept(mds.is_unique()) == noexcept(mds.mapping().is_unique()));
        static_assert(noexcept(mds.is_exhaustive()) == noexcept(mds.mapping().is_exhaustive()));
        static_assert(noexcept(mds.is_strided()) == noexcept(mds.mapping().is_strided()));
    }
}

constexpr void check_stride_function() {
    using E   = extents<signed char, 2, 3, 5>;
    using Mds = mdspan<int, E, layout_stride>;

    int data[] = {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
    Mds mds{data, layout_stride::mapping<E>{E{}, array{15, 1, 3}}};
    same_as<Mds::index_type> decltype(auto) s1 = mds.stride(0);
    assert(s1 == 15);
    same_as<Mds::index_type> decltype(auto) s2 = as_const(mds).stride(1);
    assert(s2 == 1);

    struct ConvertibleToRankType {
        constexpr operator integral auto() const {
            return 1;
        }

        constexpr operator Mds::rank_type() const {
            return 2;
        }
    };

    same_as<Mds::index_type> decltype(auto) s3 = as_const(mds).stride(ConvertibleToRankType{});
    assert(s3 == 3);
}

constexpr void check_deduction_guides() {
    { // CArray
        int carray[10]{};
        mdspan mds{carray};
        static_assert(same_as<decltype(mds), mdspan<int, extents<size_t, 10>>>);
    }

    { // Pointer&&
        float carray[20]{};
        float* ptr = carray;
        mdspan mds{ptr};
        static_assert(same_as<decltype(mds), mdspan<float, extents<size_t>>>);
    }

    { // ElementType*, Integrals...
        byte carray[30]{};
        byte* ptr = carray;
        mdspan mds1{ptr, 6, 5};
        static_assert(same_as<decltype(mds1), mdspan<byte, dextents<size_t, 2>>>);
        mdspan mds2{ptr, 2, 3, 5};
        static_assert(same_as<decltype(mds2), mdspan<byte, dextents<size_t, 3>>>);
    }

    { // ElementType*, span<OtherIndexType, N>
        const int carray[40]{};
        const int* ptr    = carray;
        const int exts1[] = {2, 4, 5};
        span s1{exts1};
        mdspan mds1{ptr, s1};
        static_assert(same_as<decltype(mds1), mdspan<const int, dextents<size_t, 3>>>);
        const long exts2[] = {2, 2, 5, 2};
        span s2{exts2};
        mdspan mds2{ptr, s2};
        static_assert(same_as<decltype(mds2), mdspan<const int, dextents<size_t, 4>>>);
    }

    { // ElementType*, const array<OtherIndexType, N>&
        const char carray[50]{};
        const char* ptr = carray;
        array a1{5, 10};
        mdspan mds1{ptr, a1};
        static_assert(same_as<decltype(mds1), mdspan<const char, dextents<size_t, 2>>>);
        array a2{2, 5, 5};
        mdspan mds2{ptr, a2};
        static_assert(same_as<decltype(mds2), mdspan<const char, dextents<size_t, 3>>>);
    }

    { // ElementType*, const extents<IndexType, ExtentsPack...>&
        const double carray[60]{};
        const double* ptr = carray;
        extents<size_t, 6, 10> exts1;
        mdspan mds1{ptr, exts1};
        static_assert(same_as<decltype(mds1), mdspan<const double, extents<size_t, 6, 10>>>);
        extents<unsigned char, 4, dynamic_extent> exts2{4};
        mdspan mds2{ptr, exts2};
        static_assert(same_as<decltype(mds2), mdspan<const double, extents<unsigned char, 4, dynamic_extent>>>);
    }

    { // ElementType*, const MappingType&
        const long carray[70]{};
        const long* ptr = carray;
        mdspan mds1{ptr, layout_left::mapping<extents<int, 4, 4>>{}};
        static_assert(same_as<decltype(mds1), mdspan<const long, extents<int, 4, 4>, layout_left>>);
        mdspan mds2{ptr, layout_stride::mapping<dextents<int, 3>>{dextents<int, 3>{2, 3, 5}, array{3, 1, 6}}};
        static_assert(same_as<decltype(mds2), mdspan<const long, dextents<int, 3>, layout_stride>>);
    }

    { // const typename AccessorType::data_handle_type&, const MappingType&, const AccessorType&
        vector<bool> bools = {true, false, true, false};
        mdspan mds{bools.begin(), TrackingLayout<>::mapping<extents<int, 2, 2>>(1), VectorBoolAccessor{}};
        static_assert(same_as<decltype(mds), mdspan<bool, extents<int, 2, 2>, TrackingLayout<>, VectorBoolAccessor>>);
    }
}

constexpr bool test() {
    check_modeled_concepts_and_member_types<dextents<unsigned long long, 3>, layout_left, default_accessor>();
    check_modeled_concepts_and_member_types<dextents<unsigned long long, 3>, layout_right, default_accessor>();
    check_modeled_concepts_and_member_types<dextents<unsigned long long, 3>, layout_stride, default_accessor>();
    check_modeled_concepts_and_member_types<dextents<unsigned long long, 3>, layout_left, TrackingAccessor>();
    check_modeled_concepts_and_member_types<extents<signed char, 2, 3, 5>, layout_stride, TrivialAccessor>();
    check_modeled_concepts_and_member_types<extents<unsigned short, 2, 4, dynamic_extent>, TrackingLayout<>,
        AccessorWithTrackingDataHandle>();
    check_observers();
    check_default_constructor();
    check_defaulted_copy_and_move_constructors();
    check_data_handle_and_indices_pack_constructor();
    check_data_handle_and_span_array_constructors();
    check_data_handle_and_extents_constructor();
    check_data_handle_and_mapping_constructor();
    check_data_handle_and_mapping_and_accessor_constructor();
    check_construction_from_other_mdspan();
    check_defaulted_copy_and_move_assignment_operators();
#ifdef __cpp_multidimensional_subscript // TRANSITION, P2128R6
    check_multidimensional_subscript_operator();
#endif // __cpp_multidimensional_subscript
    check_span_array_subscript_operator();
    check_size();
    check_empty();
    check_swap();
    check_getters();
    check_is_always_functions();
    check_is_functions();
    check_stride_function();
    check_deduction_guides();
    return true;
}

int main() {
    static_assert(test());
    test();
}
