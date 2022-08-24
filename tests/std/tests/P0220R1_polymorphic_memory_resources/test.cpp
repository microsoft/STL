// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_CXX17_POLYMORPHIC_ALLOCATOR_DESTROY_DEPRECATION_WARNING
#define _SILENCE_CXX23_ALIGNED_UNION_DEPRECATION_WARNING

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <forward_list>
#include <functional>
#include <limits>
#include <list>
#include <malloc.h>
#include <map>
#include <memory>
#include <memory_resource>
#include <new>
#include <numeric>
#include <regex>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

constexpr auto size_max = std::numeric_limits<std::size_t>::max();

#pragma warning(disable : 6326) // Potential comparison of a constant with another constant.
#pragma warning(disable : 28251) // Inconsistent annotation for 'new': this instance has no annotations.

static_assert(__STDCPP_DEFAULT_NEW_ALIGNMENT__ == 2 * sizeof(void*));

namespace {
    constexpr std::size_t operator""_zu(unsigned long long value) noexcept {
        return static_cast<std::size_t>(value);
    }

    struct allocation {
        void* ptr;
        std::size_t size;
        std::size_t align;

        friend constexpr bool operator==(allocation const& x, allocation const& y) noexcept {
            return x.ptr == y.ptr && x.size == y.size && x.align == y.align;
        }
        [[maybe_unused]] friend constexpr bool operator!=(allocation const& x, allocation const& y) noexcept {
            return !(x == y);
        }
    };

    bool out_of_memory = false;
} // unnamed namespace

void* operator new(std::size_t size) {
    if (!out_of_memory) {
        if (auto ptr = std::malloc(size)) {
            return ptr;
        }
    }
    throw std::bad_alloc{};
}

void* operator new(std::size_t size, std::align_val_t align) {
    if (!out_of_memory) {
        if (auto ptr = ::_aligned_malloc(size, static_cast<std::size_t>(align))) {
            return ptr;
        }
    }
    throw std::bad_alloc{};
}

void operator delete(void* ptr, std::size_t) noexcept {
    std::free(ptr);
}

void operator delete(void* ptr) noexcept {
    ::operator delete(ptr, 0_zu);
}

void operator delete(void* ptr, std::size_t, std::align_val_t) noexcept {
    ::_aligned_free(ptr);
}

namespace {
    [[noreturn]] void fail(const char* const message) {
        std::fputs(message, stderr);
        std::abort();
    }

#define STRINGIFY2(X) #X
#define STRINGIFY(X)  STRINGIFY2(X)
#define CHECK(...) \
    ((__VA_ARGS__) ? void(0) : fail(__FILE__ "(" STRINGIFY(__LINE__) "): check failed: " #__VA_ARGS__ "\n"))

    static constexpr std::size_t allocation_sizes[] = {
        1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 13, 15, 16, 42, 64, 137, 256, 384, 512, 4096, 65536, 65537, 1000000};

    struct not_constructible {
        ~not_constructible() = delete;
        not_constructible()  = delete;
    };

    struct default_resource_guard {
        ~default_resource_guard() {
            if (ptr_) {
                std::pmr::set_default_resource(ptr_);
            }
        }

        default_resource_guard(std::pmr::memory_resource* new_default)
            : ptr_{std::pmr::set_default_resource(new_default)} {}
        default_resource_guard(default_resource_guard&& that) noexcept : ptr_{std::exchange(that.ptr_, nullptr)} {}

        std::pmr::memory_resource* resource() const noexcept {
            return ptr_;
        }

    private:
        std::pmr::memory_resource* ptr_;
    };

    struct placement_delete {
        template <class T>
        void operator()(T* ptr) const {
            ptr->~T();
        }
    };
    template <class T>
    using placement_ptr = std::unique_ptr<T, placement_delete>;

    template <class Alloc, bool NeedsAllocatorArgT>
    struct uses_allocator_thing : private Alloc {
        using allocator_type = Alloc;

        int i_ = 0;

        uses_allocator_thing(std::allocator_arg_t, Alloc const& a, int i = 0) : Alloc(a), i_{i} {}
        Alloc get_allocator() const {
            return *this;
        }

        bool operator==(int i) const {
            return i == i_;
        }
        bool operator!=(int i) const {
            return i != i_;
        }
    };
    template <class Alloc>
    struct uses_allocator_thing<Alloc, false> : uses_allocator_thing<Alloc, true> {
        uses_allocator_thing(int i, Alloc const& a) : uses_allocator_thing<Alloc, true>{std::allocator_arg, a, i} {}
        uses_allocator_thing(Alloc const& a) : uses_allocator_thing<Alloc, true>{std::allocator_arg, a} {}
    };

    struct malloc_resource final : std::pmr::memory_resource {
    private:
        virtual void* do_allocate(std::size_t bytes, std::size_t align) override {
            if (!bytes) {
                return nullptr;
            }
            void* result = nullptr;
            if (align > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
                result = _aligned_malloc(bytes, align);
            } else {
                result = std::malloc(bytes);
            }
            if (result) {
                return result;
            }
            throw std::bad_alloc{};
        }

        virtual void do_deallocate(void* ptr, std::size_t, std::size_t align) noexcept override {
            if (align > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
                _aligned_free(ptr);
            } else {
                std::free(ptr);
            }
        }

        virtual bool do_is_equal(const memory_resource& that) const noexcept override {
            return typeid(malloc_resource) == typeid(that);
        }
    };

    struct checked_resource final : std::pmr::memory_resource {
        std::size_t bytes_{};
        std::size_t align_{};
        void* ptr_{};

    private:
        virtual void* do_allocate(std::size_t bytes, std::size_t align) override {
            if (bytes_ != 0) {
                CHECK(bytes == bytes_);
            } else {
                bytes_ = bytes;
            }
            if (align_ != 0) {
                CHECK(align == align_);
            } else {
                align_ = align;
            }
            if (align <= __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
                return ptr_ = ::operator new(bytes);
            } else {
                return ptr_ = ::operator new (bytes, std::align_val_t{align});
            }
        }

        virtual void do_deallocate(void* ptr, std::size_t bytes, std::size_t align) noexcept override {
            if (ptr_) {
                CHECK(ptr == ptr_);
                if (bytes_ != 0) {
                    CHECK(bytes == bytes_);
                } else {
                    bytes_ = bytes;
                }
                if (align_ != 0) {
                    CHECK(align == align_);
                } else {
                    align_ = align;
                }
            } else {
                if (bytes_ != 0) {
                    CHECK(bytes == bytes_);
                }
                if (align_ != 0) {
                    CHECK(align == align_);
                }
            }
            if (align <= __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
                ::operator delete(ptr, bytes);
            } else {
                ::operator delete (ptr, bytes, std::align_val_t{align});
            }
        }

        virtual bool do_is_equal(const memory_resource& that) const noexcept override {
            CHECK(ptr_ == &that);
            return typeid(checked_resource) == typeid(that);
        }
    };

    struct recording_resource : std::pmr::memory_resource {
        std::vector<allocation> allocations_;
        memory_resource* upstream_ = std::pmr::get_default_resource();

        recording_resource() = default;
        recording_resource(memory_resource* upstream) noexcept : upstream_{upstream} {}

        ~recording_resource() {
            CHECK(allocations_.empty());
        }

        recording_resource(recording_resource const&)            = delete;
        recording_resource& operator=(recording_resource const&) = delete;

        void release() noexcept {
            while (!allocations_.empty()) {
                auto& last = allocations_.back();
                upstream_->deallocate(last.ptr, last.size, last.align);
                allocations_.pop_back();
            }
        }

        virtual void* do_allocate(std::size_t const bytes, std::size_t const align) override {
            void* const result = upstream_->allocate(bytes, align);
            allocations_.push_back({result, bytes, align});
            return result;
        }

        virtual void do_deallocate(
            void* const ptr, std::size_t const bytes, std::size_t const align) noexcept override {
            allocation const alloc{ptr, bytes, align};
            auto const end = allocations_.end();
            auto pos       = std::find(allocations_.begin(), end, alloc);
            CHECK(pos != end);
            allocations_.erase(pos);
            upstream_->deallocate(ptr, bytes, align);
        }

        virtual bool do_is_equal(const memory_resource& that) const noexcept override {
            return this == &that;
        }
    };

    void analyze_geometric_growth(size_t const* first, size_t const n) {
        // https://mathworld.wolfram.com/LeastSquaresFittingExponential.html
        // https://en.wikipedia.org/wiki/Pearson_correlation_coefficient#For_a_sample
        double sum_of_x           = 0;
        double sum_of_y           = 0;
        double sum_of_xx          = 0;
        double sum_of_xy          = 0;
        double sum_of_xxy         = 0;
        double sum_of_x_log_y     = 0;
        double sum_of_log_y       = 0;
        double sum_of_y_log_y     = 0;
        double sum_of_log_y_log_y = 0;
        double sum_of_xy_log_y    = 0;
        for (auto i = 0_zu; i < n; ++i) {
            auto const y = static_cast<double>(first[i]);
            sum_of_y += y;
            auto const x = static_cast<double>(i);
            sum_of_x += x;
            sum_of_xx += x * x;
            double const xy = x * y;
            sum_of_xy += xy;
            sum_of_xxy += x * xy;
            double const log_y = std::log(y);
            sum_of_log_y += log_y;
            sum_of_log_y_log_y += log_y * log_y;
            sum_of_x_log_y += x * log_y;
            double const y_log_y = y * log_y;
            sum_of_y_log_y += y_log_y;
            sum_of_xy_log_y += x * y_log_y;
        }
        double const b =
            (sum_of_y * sum_of_xy_log_y - sum_of_xy * sum_of_y_log_y) / (sum_of_y * sum_of_xxy - sum_of_xy * sum_of_xy);
        double const r =
            (n * sum_of_x_log_y - sum_of_x * sum_of_log_y)
            / std::sqrt((n * sum_of_xx - sum_of_x * sum_of_x) * (n * sum_of_log_y_log_y - sum_of_log_y * sum_of_log_y));
        double const factor = std::exp(b);
        CHECK(factor > 1.0);
        CHECK(r > 0.99);
    }

    namespace memory_resource {
        namespace global {
            namespace new_delete_resource {
                void test_eq() {
                    auto& ndr = *std::pmr::new_delete_resource();
                    CHECK(&ndr == std::pmr::new_delete_resource());
                    CHECK(ndr.is_equal(ndr));
                    CHECK(ndr == ndr);
                }

                void test_allocate() {
                    auto& ndr = *std::pmr::new_delete_resource();

                    for (std::size_t size : allocation_sizes) {
                        for (auto align = 1_zu; align <= 512_zu && size % align == 0_zu; align *= 2_zu) {
                            auto ptr = ndr.allocate(size, align);
                            CHECK(ptr != nullptr);
                            void* vp = ptr;
                            size_t n = size;
                            CHECK(std::align(align, size, vp, n) == ptr);
                            CHECK(vp == ptr);
                            CHECK(n == size);
                            if (align <= __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
                                ::operator delete(ptr, size);
                            } else {
                                ::operator delete (ptr, size, std::align_val_t{align});
                            }
                        }
                    }
                }

                void test_deallocate() {
                    auto& ndr = *std::pmr::new_delete_resource();

                    for (std::size_t size : allocation_sizes) {
                        for (auto align = 1_zu; align <= 512_zu && size % align == 0_zu; align *= 2_zu) {
                            auto ptr = (align <= __STDCPP_DEFAULT_NEW_ALIGNMENT__)
                                         ? ::operator new(size)
                                         : ::operator new (size, std::align_val_t{align});
                            ndr.deallocate(ptr, size, align);
                        }
                    }
                }

                void test() {
                    test_eq();
                    test_allocate();
                    test_deallocate();
                }
            } // namespace new_delete_resource

            namespace null_memory_resource {
                void test_eq() {
                    auto& nmr = *std::pmr::null_memory_resource();
                    CHECK(&nmr == std::pmr::null_memory_resource());
                    CHECK(nmr.is_equal(nmr));
                }

                void test_allocate() {
                    auto& nmr = *std::pmr::null_memory_resource();

                    for (std::size_t size : allocation_sizes) {
                        for (auto align = 1_zu; align <= 512_zu && size % align == 0_zu; align *= 2_zu) {
                            try {
                                (void) nmr.allocate(size, align);
                                CHECK(false);
                            } catch (std::bad_alloc&) {
                            }
                        }
                    }
                }

                void test_deallocate() {
                    auto& nmr = *std::pmr::null_memory_resource();

                    for (std::size_t size : allocation_sizes) {
                        for (auto align = 1_zu; align <= 512_zu && size % align == 0_zu; align *= 2_zu) {
                            void* ptr = align <= __STDCPP_DEFAULT_NEW_ALIGNMENT__
                                          ? ::operator new(size)
                                          : ::operator new (size, std::align_val_t{align});
                            nmr.deallocate(ptr, size, align);
                            if (align <= __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
                                ::operator delete(ptr, size);
                            } else {
                                ::operator delete (ptr, size, std::align_val_t{align});
                            }
                        }
                    }
                }

                void test() {
                    test_eq();
                    test_allocate();
                    test_deallocate();
                }
            } // namespace null_memory_resource

            namespace get_set_default_resource {
                void test() {
                    default_resource_guard guard{nullptr};
                    auto const ndr = std::pmr::new_delete_resource();
                    CHECK(*ndr == *guard.resource());

                    malloc_resource mr;
                    CHECK(*ndr == *std::pmr::set_default_resource(&mr));
                    std::pmr::vector<int> vec1{3, 2, 1};
                    CHECK(vec1.get_allocator().resource() == &mr);
                }
            } // namespace get_set_default_resource
        } // namespace global

        template <bool Equal>
        struct compare_resource final : std::pmr::memory_resource {
        private:
            void* do_allocate(std::size_t, std::size_t) override {
                throw std::bad_alloc{};
            }
            void do_deallocate(void*, std::size_t, std::size_t) noexcept override {}
            bool do_is_equal(const memory_resource&) const noexcept override {
                return Equal;
            }
        };

        namespace is_equal {
            void test() {
                {
                    compare_resource<false> r1, r2;
                    // resources with the same address are equal regardless of what do_is_equal returns.
                    CHECK(r1 == r1);
                    CHECK(!(r1 != r1));
                    CHECK(r1 != r2);
                    CHECK(r2 != r1);
                }
                {
                    compare_resource<true> r1, r2;
                    CHECK(r1 == r1);
                    CHECK(!(r1 != r1));
                    CHECK(r1 == r2);
                    CHECK(r2 == r1);
                    CHECK(!(r1 != r2));
                    CHECK(!(r2 != r1));
                }
                {
                    checked_resource r1, r2;
                    CHECK(r1 == r1);
                    CHECK(!(r1 != r1));
                    r1.ptr_ = &r2;
                    r2.ptr_ = &r1;
                    CHECK(r1 == r2);
                    CHECK(r2 == r1);
                    CHECK(!(r1 != r2));
                    CHECK(!(r2 != r1));
                }
            }
        } // namespace is_equal

        namespace allocate_deallocate {
            void test_max_align() {
                // verify that allocate and deallocate default their alignment parameters to alignof(max_align_t)

                struct max_align_checker final : std::pmr::memory_resource {
                private:
                    virtual void* do_allocate(std::size_t bytes, std::size_t align) override {
                        CHECK(align == alignof(std::max_align_t));
                        return std::malloc(bytes);
                    }
                    virtual void do_deallocate(void* ptr, std::size_t, std::size_t align) override {
                        CHECK(align == alignof(std::max_align_t));
                        std::free(ptr);
                    }
                    virtual bool do_is_equal(const memory_resource& that) const noexcept override {
                        return typeid(max_align_checker) == typeid(that);
                    }
                };

                max_align_checker checker;
                constexpr auto N = 42_zu;
                checker.deallocate(checker.allocate(N), N);
            }

            void test() {
                test_max_align();

                checked_resource r;
                for (std::size_t size : allocation_sizes) {
                    for (auto align = 1_zu; align <= 512_zu && size % align == 0_zu; align *= 2_zu) {
                        r.bytes_ = size;
                        r.align_ = align;
                        auto ptr = r.allocate(size, align);
                        CHECK(ptr != nullptr);
                        CHECK(ptr == r.ptr_);
                        r.deallocate(ptr, size, align);
                    }
                }
            }
        } // namespace allocate_deallocate
    } // namespace memory_resource

    namespace polymorphic_allocator {
        namespace value_type {
            void test() {
                static_assert(std::is_same_v<int, std::pmr::polymorphic_allocator<int>::value_type>);
                static_assert(std::is_same_v<double, std::pmr::polymorphic_allocator<double>::value_type>);
            }
        } // namespace value_type

        namespace ctor {
            namespace default_ {
                template <class T>
                void test_one_type(malloc_resource& mr) {
                    std::pmr::polymorphic_allocator<T> alloc{};
                    CHECK(*alloc.resource() == mr);
                }

                void test() {
                    malloc_resource mr;
                    default_resource_guard guard{&mr};
                    test_one_type<char>(mr);
                    test_one_type<int>(mr);
                    test_one_type<std::vector<std::vector<int>>>(mr);
                }
            } // namespace default_

            namespace value {
                template <class T>
                void test_one_type() {
                    malloc_resource mr;
                    std::pmr::polymorphic_allocator<T> alloc = &mr;
                    CHECK(alloc.resource() == &mr);
                }

                void test() {
                    test_one_type<char>();
                    test_one_type<int>();
                    test_one_type<std::vector<std::vector<int>>>();
                }
            } // namespace value

            namespace rebind {
                template <class T, class U>
                void test_one() {
                    checked_resource resource;
                    std::pmr::polymorphic_allocator<T> allocT1 = &resource;
                    std::pmr::polymorphic_allocator<U> allocU  = allocT1;
                    CHECK(allocU.resource() == &resource);
                    std::pmr::polymorphic_allocator<T> allocT2 = allocU;
                    CHECK(allocT2.resource() == &resource);
                }

                void test() {
                    test_one<char, int>();
                    test_one<int, double>();
                    test_one<std::vector<std::vector<int>>, int>();
                }
            } // namespace rebind
        } // namespace ctor

        namespace mem {
            namespace allocate_deallocate {
                template <class T>
                void test_one() {
                    checked_resource checked;
                    std::pmr::polymorphic_allocator<T> alloc = &checked;

                    for (std::size_t n : {0U, 1U, 2U, 3U, 16U, 738219U}) {
                        checked.align_ = alignof(T);
                        checked.bytes_ = n * sizeof(T);
                        T* ptr         = alloc.allocate(n);
                        checked.ptr_   = ptr;
                        alloc.deallocate(ptr, n);
                        checked.ptr_ = nullptr;
                    }
                }

                template <class T>
                void test_throws() {
                    static_assert(sizeof(T) > 1);

                    checked_resource checked;
                    std::pmr::polymorphic_allocator<T> alloc = &checked;

                    try {
                        (void) alloc.allocate(size_max / sizeof(T) + 1);
                        CHECK(false);
                    } catch (std::bad_alloc&) {
                    }
                }

                void test() {
                    struct alignas(32) overaligned {
                        char space_[32];
                    };

                    test_one<char>();
                    test_one<int>();
                    test_one<std::vector<std::vector<int>>>();
                    test_one<overaligned>();

                    test_throws<int>();
                    test_throws<std::vector<std::vector<int>>>();
                    test_throws<overaligned>();
                }
            } // namespace allocate_deallocate

            namespace construct {
                template <class T>
                void test_if_poly(std::true_type, T& t) {
                    CHECK(t.get_allocator().resource() == std::pmr::get_default_resource());
                }

                template <class T>
                void test_if_poly(std::false_type, T&) {}

                template <class T, class... Args>
                void test_one(Args... args) {
                    std::pmr::polymorphic_allocator<not_constructible> alloc;
                    std::aligned_union_t<0, T> space;

                    auto const rawptr = reinterpret_cast<T*>(&space);
                    alloc.construct(rawptr, args...);
                    placement_ptr<T> ptr{rawptr};
                    CHECK(*ptr == T(args...));
                    test_if_poly(std::uses_allocator<T, std::pmr::memory_resource*>(), *ptr);
                }

#pragma warning(push)
#pragma warning(disable : 4702) // unreachable code
                void test_throws() {
                    struct X {
                        X() = default;
                        X(X&&) {
                            throw 42;
                        }
                    };

                    checked_resource checked;
                    std::pmr::polymorphic_allocator<not_constructible> alloc = &checked;
                    std::aligned_union_t<0, X> space;

                    auto const ptr = reinterpret_cast<X*>(&space);
                    try {
                        alloc.construct(ptr, std::move(*ptr));
                        CHECK(false);
                    } catch (int) {
                    }
                }
#pragma warning(pop)

                void test() {
                    test_one<char>('a');
                    test_one<int>(42);
                    test_one<std::vector<std::vector<int>>>(
                        std::initializer_list<std::vector<int>>{{1, 2, 3}, {9, 8, 7, 6}});
                    test_one<std::pmr::vector<int>>(32478_zu, 42);

                    test_throws();
                }
            } // namespace construct

            namespace piecewise_construct {
                void test() {
                    malloc_resource mr;
                    std::pmr::polymorphic_allocator<not_constructible> alloc = &mr;

                    {
                        using P = std::pair<int, int>;
                        std::aligned_union_t<0, P> space;
                        auto const rawptr = reinterpret_cast<P*>(&space);

                        alloc.construct(rawptr, std::piecewise_construct, std::make_tuple(), std::make_tuple(1729));
                        placement_ptr<P> ptr{rawptr};

                        CHECK(ptr->first == 0);
                        CHECK(ptr->second == 1729);
                    }
                    {
                        using P = std::pair<uses_allocator_thing<std::pmr::polymorphic_allocator<int>, true>, int>;
                        std::aligned_union_t<0, P> space;
                        auto const rawptr = reinterpret_cast<P*>(&space);

                        alloc.construct(rawptr, std::piecewise_construct, std::make_tuple(), std::make_tuple(1729));
                        placement_ptr<P> ptr{rawptr};

                        CHECK(ptr->first.get_allocator().resource() == &mr);
                        CHECK(ptr->second == 1729);
                    }
                    {
                        using P = std::pair<uses_allocator_thing<std::pmr::polymorphic_allocator<int>, false>, int>;
                        std::aligned_union_t<0, P> space;
                        auto const rawptr = reinterpret_cast<P*>(&space);

                        alloc.construct(rawptr, std::piecewise_construct, std::make_tuple(), std::make_tuple(1729));
                        placement_ptr<P> ptr{rawptr};

                        CHECK(ptr->first.get_allocator().resource() == &mr);
                        CHECK(ptr->second == 1729);
                    }

                    {
                        using P = std::pair<int, uses_allocator_thing<std::pmr::polymorphic_allocator<int>, true>>;
                        std::aligned_union_t<0, P> space;
                        auto const rawptr = reinterpret_cast<P*>(&space);

                        alloc.construct(rawptr, std::piecewise_construct, std::make_tuple(1729), std::make_tuple());
                        placement_ptr<P> ptr{rawptr};

                        CHECK(ptr->first == 1729);
                        CHECK(ptr->second.get_allocator().resource() == &mr);
                    }
                    {
                        using P = std::pair<int, uses_allocator_thing<std::pmr::polymorphic_allocator<int>, false>>;
                        std::aligned_union_t<0, P> space;
                        auto const rawptr = reinterpret_cast<P*>(&space);

                        alloc.construct(rawptr, std::piecewise_construct, std::make_tuple(1729), std::make_tuple());
                        placement_ptr<P> ptr{rawptr};

                        CHECK(ptr->first == 1729);
                        CHECK(ptr->second.get_allocator().resource() == &mr);
                    }

                    {
                        using P = std::pair<uses_allocator_thing<std::pmr::polymorphic_allocator<int>, true>,
                            uses_allocator_thing<std::pmr::polymorphic_allocator<int>, true>>;
                        std::aligned_union_t<0, P> space;
                        auto const rawptr = reinterpret_cast<P*>(&space);

                        alloc.construct(rawptr, std::piecewise_construct, std::make_tuple(), std::make_tuple());
                        placement_ptr<P> ptr{rawptr};

                        CHECK(ptr->first.get_allocator().resource() == &mr);
                        CHECK(ptr->second.get_allocator().resource() == &mr);
                    }
                    {
                        using P = std::pair<uses_allocator_thing<std::pmr::polymorphic_allocator<int>, false>,
                            uses_allocator_thing<std::pmr::polymorphic_allocator<int>, false>>;
                        std::aligned_union_t<0, P> space;
                        auto const rawptr = reinterpret_cast<P*>(&space);

                        alloc.construct(rawptr, std::piecewise_construct, std::make_tuple(), std::make_tuple());
                        placement_ptr<P> ptr{rawptr};

                        CHECK(ptr->first.get_allocator().resource() == &mr);
                        CHECK(ptr->second.get_allocator().resource() == &mr);
                    }
                }
            } // namespace piecewise_construct

            namespace construct_pair {
                void test() {
                    malloc_resource mr;
                    std::pmr::polymorphic_allocator<not_constructible> alloc = &mr;
                    {
                        using P = std::pair<int, int>;
                        std::aligned_union_t<0, P> space;
                        auto const rawptr = reinterpret_cast<P*>(&space);

                        alloc.construct(rawptr);
                        placement_ptr<P> ptr{rawptr};

                        CHECK(ptr->first == 0);
                        CHECK(ptr->second == 0);
                    }
                    {
                        using P = std::pair<uses_allocator_thing<std::pmr::polymorphic_allocator<int>, true>,
                            uses_allocator_thing<std::pmr::polymorphic_allocator<int>, false>>;
                        std::aligned_union_t<0, P> space;
                        auto const rawptr = reinterpret_cast<P*>(&space);

                        alloc.construct(rawptr);
                        placement_ptr<P> ptr{rawptr};

                        CHECK(ptr->first.get_allocator().resource() == &mr);
                        CHECK(ptr->second.get_allocator().resource() == &mr);
                    }
                }
            } // namespace construct_pair

            namespace construct_pair_U_V {
                void test() {
                    malloc_resource mr;
                    std::pmr::polymorphic_allocator<not_constructible> alloc = &mr;

                    {
                        using P = std::pair<int, int>;
                        std::aligned_union_t<0, P> space;
                        auto const rawptr = reinterpret_cast<P*>(&space);

                        int i = 13;
                        alloc.construct(rawptr, 42, i);
                        placement_ptr<P> ptr{rawptr};

                        CHECK(ptr->first == 42);
                        CHECK(ptr->second == 13);
                    }
                    {
                        using P = std::pair<uses_allocator_thing<std::pmr::polymorphic_allocator<int>, true>,
                            uses_allocator_thing<std::pmr::polymorphic_allocator<int>, false>>;
                        std::aligned_union_t<0, P> space;
                        auto const rawptr = reinterpret_cast<P*>(&space);

                        int i = 13;
                        alloc.construct(rawptr, 42, i);
                        placement_ptr<P> ptr{rawptr};

                        CHECK(ptr->first == 42);
                        CHECK(ptr->first.get_allocator().resource() == &mr);
                        CHECK(ptr->second == 13);
                        CHECK(ptr->second.get_allocator().resource() == &mr);
                    }
                }
            } // namespace construct_pair_U_V

            namespace construct_pair_lvalue_pair {
                void test() {
                    malloc_resource mr;
                    std::pmr::polymorphic_allocator<not_constructible> alloc = &mr;

                    {
                        using P = std::pair<int, int>;
                        std::aligned_union_t<0, P> space;
                        auto const rawptr = reinterpret_cast<P*>(&space);

                        std::pair<int, int> arg{42, 13};
                        alloc.construct(rawptr, arg);
                        placement_ptr<P> ptr{rawptr};

                        CHECK(ptr->first == 42);
                        CHECK(ptr->second == 13);
                    }
                    {
                        using P = std::pair<uses_allocator_thing<std::pmr::polymorphic_allocator<int>, true>,
                            uses_allocator_thing<std::pmr::polymorphic_allocator<int>, false>>;
                        std::aligned_union_t<0, P> space;
                        auto const rawptr = reinterpret_cast<P*>(&space);

                        std::pair<int, int> arg{42, 13};
                        alloc.construct(rawptr, arg);
                        placement_ptr<P> ptr{rawptr};

                        CHECK(ptr->first == 42);
                        CHECK(ptr->first.get_allocator().resource() == &mr);
                        CHECK(ptr->second == 13);
                        CHECK(ptr->second.get_allocator().resource() == &mr);
                    }
                }
            } // namespace construct_pair_lvalue_pair

            namespace construct_pair_rvalue_pair {
                void test() {
                    malloc_resource mr;
                    std::pmr::polymorphic_allocator<not_constructible> alloc = &mr;

                    {
                        using P = std::pair<int, int>;
                        std::aligned_union_t<0, P> space;
                        auto const rawptr = reinterpret_cast<P*>(&space);

                        alloc.construct(rawptr, std::make_pair(42, 13));
                        placement_ptr<P> ptr{rawptr};

                        CHECK(ptr->first == 42);
                        CHECK(ptr->second == 13);
                    }
                    {
                        using P = std::pair<uses_allocator_thing<std::pmr::polymorphic_allocator<int>, true>,
                            uses_allocator_thing<std::pmr::polymorphic_allocator<int>, false>>;
                        std::aligned_union_t<0, P> space;
                        auto const rawptr = reinterpret_cast<P*>(&space);

                        alloc.construct(rawptr, std::make_pair(42, 13));
                        placement_ptr<P> ptr{rawptr};

                        CHECK(ptr->first == 42);
                        CHECK(ptr->first.get_allocator().resource() == &mr);
                        CHECK(ptr->second == 13);
                        CHECK(ptr->second.get_allocator().resource() == &mr);
                    }
                }
            } // namespace construct_pair_rvalue_pair

            namespace select_on_container_copy_construction {
                void test() {
                    malloc_resource mr;
                    std::pmr::polymorphic_allocator<int> a = &mr;
                    std::pmr::polymorphic_allocator<int> b = a.select_on_container_copy_construction();
                    CHECK(b.resource() == std::pmr::get_default_resource());
                    checked_resource checked;
                    default_resource_guard guard{&checked};
                    std::pmr::polymorphic_allocator<int> c = b.select_on_container_copy_construction();
                    CHECK(c.resource() == &checked);
                }
            } // namespace select_on_container_copy_construction

            namespace resource {
                void test() {
                    std::pmr::polymorphic_allocator<int> a{};
                    CHECK(a.resource() == std::pmr::get_default_resource());
                    std::pmr::polymorphic_allocator<int> b = a;
                    CHECK(b.resource() == a.resource());
                    malloc_resource mr;
                    std::pmr::polymorphic_allocator<int> c = &mr;
                    CHECK(c.resource() == &mr);
                }
            } // namespace resource
        } // namespace mem

        namespace eq {
            void test() {
                std::pmr::polymorphic_allocator<int> a{};
                CHECK(a == a);
                CHECK(!(a != a));
                std::pmr::polymorphic_allocator<int> b = a;
                CHECK(b == a);
                CHECK(!(b != a));
                malloc_resource mr;
                std::pmr::polymorphic_allocator<int> c = &mr;
                CHECK(!(a == c));
                CHECK(a != c);
            }
        } // namespace eq

        namespace eq_cvt {
            void test() {
                const auto pres                        = std::pmr::get_default_resource();
                std::pmr::polymorphic_allocator<int> a = pres;
                const auto ra                          = std::ref(a);
                const auto cra                         = std::cref(a);

                CHECK(a == pres);
                CHECK(a == ra);
                CHECK(a == cra);
                CHECK(pres == a);
                CHECK(pres == ra);
                CHECK(pres == cra);
                CHECK(ra == a);
                CHECK(ra == pres);
                CHECK(ra == ra);
                CHECK(ra == cra);
                CHECK(cra == a);
                CHECK(cra == pres);
                CHECK(cra == ra);
                CHECK(cra == cra);

                CHECK(!(a != pres));
                CHECK(!(a != ra));
                CHECK(!(a != cra));
                CHECK(!(pres != a));
                CHECK(!(pres != ra));
                CHECK(!(pres != cra));
                CHECK(!(ra != a));
                CHECK(!(ra != pres));
                CHECK(!(ra != ra));
                CHECK(!(ra != cra));
                CHECK(!(cra != a));
                CHECK(!(cra != pres));
                CHECK(!(cra != ra));
                CHECK(!(cra != cra));
            }
        } // namespace eq_cvt

        namespace destroy {
            void test() {
                bool destroyed = false;
                struct S {
                    bool& destroy_ref;

                    explicit S(bool& _destroy_ref_) : destroy_ref{_destroy_ref_} {}
                    ~S() {
                        destroy_ref = true;
                    }
                    S(const S&)            = delete;
                    S& operator=(const S&) = delete;
                };
                std::pmr::polymorphic_allocator<S> a{};
                S* ptr = a.allocate(1);
                a.construct(ptr, destroyed);
                CHECK(destroyed == false);
                a.destroy(ptr);
                CHECK(destroyed == true);
                a.deallocate(ptr, 1);
            }
        } // namespace destroy
    } // namespace polymorphic_allocator

    namespace monotonic {
        namespace ctor {
            namespace buffer_upstream {
                void test() {
                    std::pmr::memory_resource* const nmr = std::pmr::null_memory_resource();
                    for (std::size_t size : {42U, 128U, 512U, 8192U}) {
                        char buffer[8192];
                        std::pmr::monotonic_buffer_resource mbr{buffer, size, nmr};
                        CHECK(mbr.upstream_resource() == nmr);
                        CHECK(mbr.allocate(size, 1) != nullptr);
                    }
                }
            } // namespace buffer_upstream

            namespace size_upstream {
                void test() {
                    for (std::size_t size : {42U, 128U, 512U, 8192U}) {
                        checked_resource upstream{};
                        std::pmr::monotonic_buffer_resource mbr{size, &upstream};
                        CHECK(mbr.upstream_resource() == &upstream);
                        CHECK(mbr.allocate(1, 1) != nullptr);
                        CHECK(upstream.bytes_ >= size);
                    }
                }
            } // namespace size_upstream

            namespace upstream {
                void test() {
                    malloc_resource upstream{};
                    std::pmr::monotonic_buffer_resource mbr{&upstream};
                    CHECK(mbr.upstream_resource() == &upstream);
                }
            } // namespace upstream
        } // namespace ctor

        namespace mem {
            namespace release {
                void test_destruction() {
                    // Verify that all allocated blocks are released upstream on destruction
                    constexpr int N = 11;
                    recording_resource rr;
                    std::pmr::monotonic_buffer_resource mbr{&rr};
                    for (auto i = 0; i < N; ++i) {
                        auto const n = rr.allocations_.size();
                        do {
                            (void) mbr.allocate(sizeof(void*), alignof(void*));
                        } while (n == rr.allocations_.size());
                    }
                }

                void test_repeated_release() {
                    // Verify that allocations after calling release() allocate from upstream (to guard against
                    // regression of VSO-923042) and that calls to release don't affect the blocksize
                    recording_resource rr;
                    constexpr auto initial_blocksize = 1024_zu;
                    std::pmr::monotonic_buffer_resource mbr{initial_blocksize, &rr};

                    (void) mbr.allocate(sizeof(void*), alignof(void*));
                    auto const [blocksize, baseline] = [&] {
                        auto const tmp = rr.allocations_.size();
                        CHECK(rr.allocations_.back().size >= initial_blocksize);
                        auto const blocksize = rr.allocations_.back().size;
                        mbr.release();
                        CHECK(tmp > rr.allocations_.size());
                        return std::pair(blocksize, rr.allocations_.size());
                    }();

                    constexpr int N = 1024;
                    for (auto i = 0; i < N; ++i) {
                        (void) mbr.allocate(sizeof(void*), alignof(void*));
                        CHECK(baseline + 1 == rr.allocations_.size()); // one block was allocated...
                        CHECK(rr.allocations_.back().size == blocksize); // ...of size blocksize...
                        mbr.release();
                        CHECK(baseline == rr.allocations_.size()); // ...and released again
                    }
                }

                void test() {
                    test_destruction();
                    test_repeated_release();
                }
            } // namespace release

            namespace do_allocate {
                void test_preallocated_buffer() {
                    // Verify that calls to allocate do not hit the upstream resource until the
                    // initial buffer is exhausted.
                    constexpr auto N = 64_zu;
                    alignas(void*) char buffer[N * sizeof(void*)];
                    std::pmr::monotonic_buffer_resource mbr{buffer, sizeof(buffer), std::pmr::null_memory_resource()};
                    for (auto i = N; i-- > 0;) {
                        (void) mbr.allocate(sizeof(void*), alignof(void*));
                    }
                    try {
                        (void) mbr.allocate(1, 1);
                        CHECK(false);
                    } catch (std::bad_alloc&) {
                        // nothing to do
                    }
                }

                void test_size_and_alignment() {
                    // Verify that requests to the upstream allocator are for blocks whose size and
                    // alignment is not less than the size and alignment requested from the
                    // monotonic_buffer_resource.
                    for (std::size_t log : {5U, 6U, 8U, 10U, 12U, 16U, 20U}) {
                        auto const size = 1_zu << log;
                        recording_resource rr;
                        std::pmr::monotonic_buffer_resource mbr(&rr);
                        void* const ptr = mbr.allocate(size, size);
                        CHECK(ptr != nullptr);
                        CHECK(rr.allocations_.size() == 1);
                        CHECK(rr.allocations_[0].ptr == ptr);
                        CHECK(rr.allocations_[0].size >= size);
                        CHECK(rr.allocations_[0].align >= size);
                        void* vp     = ptr;
                        size_t space = size;
                        CHECK(std::align(size, size, vp, space) == ptr);
                    }
                }

                void test_growth() {
                    // Verify that successive allocation requests to the upstream resource form a
                    // geometrically increasing sequence.
                    constexpr auto N = 16_zu;
                    recording_resource rr;
                    std::pmr::monotonic_buffer_resource mbr{&rr};

                    try {
                        do {
                            (void) mbr.allocate(1, 1);
                        } while (rr.allocations_.size() < N);
                    } catch (std::bad_alloc&) {
                    }

                    std::vector<std::size_t> sizes;
                    sizes.reserve(rr.allocations_.size());
                    for (auto const& a : rr.allocations_) {
                        sizes.push_back(a.size);
                    }
                    analyze_geometric_growth(sizes.data(), sizes.size());
                }

                void test() {
                    test_preallocated_buffer();
                    test_size_and_alignment();
                    test_growth();
                }
            } // namespace do_allocate

            namespace do_deallocate {
                void test() {
                    // Verify that calls to deallocate do not result in calls to the upstream.
                    constexpr auto N = 1_zu << 20;
                    std::vector<void*> pointers(N);
                    recording_resource rr;
                    std::pmr::monotonic_buffer_resource mbr{&rr};
                    for (auto i = 0_zu; i < N; ++i) {
                        pointers[i] = mbr.allocate(sizeof(int), alignof(int));
                    }
                    auto allocations = std::exchange(rr.allocations_, {});
                    for (auto const& p : pointers) {
                        mbr.deallocate(p, sizeof(int), alignof(int));
                    }
                    rr.allocations_ = std::move(allocations);
                }
            } // namespace do_deallocate
        } // namespace mem
    } // namespace monotonic

    namespace pool {
        namespace allocate_deallocate {
            template <class T>
            struct drop_wrapper { // contains a constructed object that is never destroyed
                alignas(T) unsigned char space_[sizeof(T)];

                template <class... Args>
                drop_wrapper(Args&&... args) {
                    ::new (space_) T(std::forward<Args>(args)...);
                }

                T& operator*() & {
                    return reinterpret_cast<T&>(space_);
                }
                T const& operator*() const& {
                    return reinterpret_cast<T const&>(space_);
                }
            };

            void test_light_allocation() {
                auto lambda = [](std::pmr::memory_resource* mr) {
                    // perform allocations of various sizes
                    using std::begin;
                    using std::end;
                    drop_wrapper<std::pmr::list<int>> wrapped_l1{mr};
                    auto& l1 = *wrapped_l1;
                    for (int i = 0; i < 42; ++i) {
                        l1.push_back(i);
                    }
                    char const* const strings[] = {"this", "is", "a", "test",
                        "This string is way way way way way way way way way way way way way way way way way way way "
                        "way way way "
                        "way way way way way way way way way way way way way way way way way way way way way way way "
                        "way way way "
                        "way way way way way way way way too long to fit in your SSO buffer."};
                    drop_wrapper<std::pmr::vector<std::pmr::string>> wrapped_v1{begin(strings), end(strings), mr};
                    auto& v1 = *wrapped_v1;
                    drop_wrapper<std::pmr::vector<double>> wrapped_v2(1024_zu, mr);
                    auto& v2 = *wrapped_v2;
                    std::iota(begin(v2), end(v2), 3.25);
                    {
                        int i = 0;
                        for (auto const& v : l1) {
                            CHECK(v == i);
                            ++i;
                        }
                    }
                    CHECK(std::equal(begin(v1), end(v1), begin(strings), end(strings)));
                    CHECK(v2[42] == 45.25);
                };

                {
                    recording_resource rr;
                    std::pmr::unsynchronized_pool_resource upr{{0_zu, 1_zu}, &rr};
                    lambda(&upr);
                }
                {
                    recording_resource rr;
                    std::pmr::unsynchronized_pool_resource upr{{0_zu, 64_zu}, &rr};
                    lambda(&upr);
                }
#ifndef _M_CEE
                {
                    recording_resource rr;
                    std::pmr::synchronized_pool_resource upr{{0_zu, 64_zu}, &rr};
                    lambda(&upr);
                }
#endif // _M_CEE
            }

            void test_medium_allocation() {
                // allocate and deallocate a great many blocks of several blocksizes
                static constexpr auto min_blocksize   = sizeof(void*);
                static constexpr auto max_blocksize   = 1_zu << 24;
                static constexpr auto max_block_count = 1024_zu;

                std::vector<void*> pointers(max_block_count);

                recording_resource rr;
                std::pmr::unsynchronized_pool_resource upr{&rr};

                for (auto blocksize = min_blocksize; blocksize < max_blocksize; blocksize <<= 1) {
                    auto const n_allocs = std::min(max_block_count, max_blocksize / blocksize);

                    for (auto j = 0_zu; j < n_allocs; ++j) {
                        pointers[j] = upr.allocate(blocksize, blocksize);
                    }

                    for (auto j = 0_zu; j < n_allocs / 4 * 3; ++j) {
                        upr.deallocate(pointers[j], blocksize, blocksize);
                    }
                }
            }

            void test_heavy_allocation() {
                // allocate and deallocate a great many blocks of a single blocksize
                static constexpr auto blocksize = 2 * sizeof(void*);
                static constexpr auto n_allocs  = 1024_zu;
                static constexpr auto n_frees   = 512_zu;
                static_assert(n_allocs >= n_frees);
                static constexpr auto n_iterations = 1024_zu;

                std::vector<void*> pointers;
                pointers.reserve(n_iterations * (n_allocs - n_frees) + n_frees);

                recording_resource rr;
                std::pmr::unsynchronized_pool_resource upr{&rr};
                for (auto i = 0_zu; i < n_iterations; ++i) {
                    for (auto j = 0_zu; j < n_allocs; ++j) {
                        pointers.push_back(upr.allocate(blocksize, blocksize));
                    }

                    for (auto j = 0_zu; j < n_frees; ++j) {
                        upr.deallocate(pointers[j], blocksize, blocksize);
                    }
                    pointers.erase(pointers.begin(), pointers.begin() + n_frees);
                }
            }

            void test_growth() {
                // Verify that successive allocation requests for a given block size result
                // in a geometrically increasing sequence of requests to the upstream resource.
                constexpr auto N          = 16_zu;
                constexpr auto block_size = 16_zu;
                recording_resource rr;
                std::pmr::unsynchronized_pool_resource upr{&rr};

                // allocate and release so we ignore bookkeeping/debug allocations
                (void) upr.allocate(block_size, block_size);
                upr.release();
                // In the current ABI, there should be a single two-pointer allocation for the container proxy
                constexpr auto idl = static_cast<size_t>(_ITERATOR_DEBUG_LEVEL != 0);
                CHECK(rr.allocations_.size() == idl);

                try {
                    do {
                        (void) upr.allocate(block_size, block_size);
                    } while (rr.allocations_.size() < N + idl);
                } catch (std::bad_alloc&) {
                }

                auto const n = rr.allocations_.size() - idl;
                std::vector<std::size_t> sizes;
                sizes.reserve(n);
                for (auto i = 0_zu; i < n; ++i) {
                    sizes.push_back(rr.allocations_[i + idl].size);
                }
                analyze_geometric_growth(sizes.data(), sizes.size());
            }

            void test() {
                test_light_allocation();
                test_medium_allocation();
                test_heavy_allocation();
                test_growth();
            }
        } // namespace allocate_deallocate

        namespace release {
            void test() { //
                recording_resource rr;
                std::pmr::unsynchronized_pool_resource upr{{0_zu, sizeof(void*) << 8}, &rr};

                for (auto shift : {0, 2, 4, 8, 10, 12}) {
                    auto const size = sizeof(void*) << shift;
                    auto const n    = (1024 * 1024 / sizeof(void*)) >> shift;
                    for (auto i = 0_zu; i < n; ++i) {
                        (void) upr.allocate(size, alignof(void*));
                    }
                }
                upr.release();
                // In the current ABI, there should be a single two-pointer allocation for the container proxy
                constexpr auto idl = static_cast<size_t>(_ITERATOR_DEBUG_LEVEL != 0);
                CHECK(rr.allocations_.size() == idl);
                if constexpr (idl != 0) {
                    CHECK(rr.allocations_.front().size == 2 * sizeof(void*));
                }
            }
        } // namespace release

        namespace upstream_resource {
            void test() {
                {
                    std::pmr::unsynchronized_pool_resource upr;
                    CHECK(upr.upstream_resource() == std::pmr::get_default_resource());
                }
                {
                    recording_resource rr;
                    std::pmr::unsynchronized_pool_resource upr{&rr};
                    CHECK(upr.upstream_resource() == &rr);
                }
            }
        } // namespace upstream_resource

        namespace options {
            void test() {
                std::pmr::pool_options const defaults = [] {
                    // options default to non-zero values
                    std::pmr::unsynchronized_pool_resource upr;
                    std::pmr::pool_options o = upr.options();
                    CHECK(o.max_blocks_per_chunk != 0);
                    CHECK(o.largest_required_pool_block != 0);
                    return o;
                }();
                {
                    // zero values explicitly select the defaults
                    std::pmr::unsynchronized_pool_resource upr{{0_zu, 0_zu}};
                    std::pmr::pool_options o = upr.options();
                    CHECK(o.max_blocks_per_chunk == defaults.max_blocks_per_chunk);
                    CHECK(o.largest_required_pool_block == defaults.largest_required_pool_block);
                }
                {
                    std::pmr::unsynchronized_pool_resource upr{{1_zu << 12, 1_zu << 18}};
                    std::pmr::pool_options o = upr.options();
                    // "sizes may be rounded to unspecified granularity"
                    CHECK(o.max_blocks_per_chunk > (1_zu << 11));
                    CHECK(o.max_blocks_per_chunk < (1_zu << 13));
                    CHECK(o.largest_required_pool_block > (1_zu << 17));
                    CHECK(o.largest_required_pool_block < (1_zu << 19));
                }
            }
        } // namespace options

        namespace is_equal {
            template <class PoolResource>
            void test_is_equal() {
                PoolResource pr1, pr2;
                CHECK(pr1 == pr1);
                CHECK(!(pr1 != pr1));
                CHECK(pr1.is_equal(pr1));

                CHECK(pr2 == pr2);
                CHECK(!(pr2 != pr2));
                CHECK(pr2.is_equal(pr2));

                CHECK(!(pr1 == pr2));
                CHECK(pr1 != pr2);
                CHECK(!pr1.is_equal(pr2));
                CHECK(!(pr2 == pr1));
                CHECK(pr2 != pr1);
                CHECK(!pr2.is_equal(pr1));

                struct DerivedResource : PoolResource {};
                DerivedResource dr;
                CHECK(dr == dr);
                CHECK(!(dr != dr));
                CHECK(dr.is_equal(dr));

                CHECK(!(dr == pr1));
                CHECK(dr != pr1);
                CHECK(!dr.is_equal(pr1));
                CHECK(!pr1.is_equal(dr));
                CHECK(!(pr1 == dr));
                CHECK(pr1 != dr);
                CHECK(!pr1.is_equal(dr));
                CHECK(!dr.is_equal(pr1));

                PoolResource& base = dr;
                CHECK(dr == base);
                CHECK(!(dr != base));
                CHECK(dr.is_equal(base));

                CHECK(base == dr);
                CHECK(!(base != dr));
                CHECK(base.is_equal(dr));
            }

            void test() {
                test_is_equal<std::pmr::unsynchronized_pool_resource>();
#ifndef _M_CEE
                test_is_equal<std::pmr::synchronized_pool_resource>();
#endif // _M_CEE
            }
        } // namespace is_equal
    } // namespace pool

    namespace containers {
        template <class T>
        void pmr_container_test() {
            CHECK(T{}.get_allocator().resource() == std::pmr::get_default_resource());
            recording_resource rr;
            CHECK(T{&rr}.get_allocator().resource() == &rr);
        }

        void test() {
            pmr_container_test<std::pmr::string>();
#ifdef __cpp_lib_char8_t
            pmr_container_test<std::pmr::u8string>();
#endif // __cpp_lib_char8_t
            pmr_container_test<std::pmr::u16string>();
            pmr_container_test<std::pmr::u32string>();
            pmr_container_test<std::pmr::wstring>();

            pmr_container_test<std::pmr::deque<int>>();
            pmr_container_test<std::pmr::forward_list<int>>();
            pmr_container_test<std::pmr::list<int>>();
            pmr_container_test<std::pmr::map<int, int>>();
            pmr_container_test<std::pmr::multimap<int, int>>();
            pmr_container_test<std::pmr::multiset<int>>();
            pmr_container_test<std::pmr::set<int>>();
            pmr_container_test<std::pmr::unordered_map<int, int>>();
            pmr_container_test<std::pmr::unordered_multimap<int, int>>();
            pmr_container_test<std::pmr::unordered_multiset<int>>();
            pmr_container_test<std::pmr::unordered_set<int>>();
            pmr_container_test<std::pmr::vector<int>>();

            pmr_container_test<std::pmr::cmatch>();
            pmr_container_test<std::pmr::wcmatch>();
            pmr_container_test<std::pmr::smatch>();
            pmr_container_test<std::pmr::wsmatch>();
        }
    } // namespace containers

    namespace map_containers {
        template <class T>
        void pair_conversion_test() {
            struct pair_conv {
                operator std::pair<const int, int>() const {
                    return {};
                }
            };

            struct mem_pair_conv {
                std::pair<const int, int> pair_{1, 42};
                operator const std::pair<const int, int>&() const {
                    return pair_;
                }
            };

            T cont;
            cont.emplace(pair_conv{});
            cont.emplace(mem_pair_conv{});
        }

        void test() {
            pair_conversion_test<std::pmr::map<int, int>>();
            pair_conversion_test<std::pmr::multimap<int, int>>();
            pair_conversion_test<std::pmr::unordered_map<int, int>>();
            pair_conversion_test<std::pmr::unordered_multimap<int, int>>();
        }
    } // namespace map_containers
} // unnamed namespace

int main() {
    // This test MUST BE FIRST; it tests global state
    memory_resource::global::get_set_default_resource::test();
    memory_resource::global::new_delete_resource::test();
    memory_resource::global::null_memory_resource::test();
    memory_resource::is_equal::test();
    memory_resource::allocate_deallocate::test();

    polymorphic_allocator::value_type::test();
    polymorphic_allocator::ctor::default_::test();
    polymorphic_allocator::ctor::value::test();
    polymorphic_allocator::ctor::rebind::test();
    polymorphic_allocator::mem::allocate_deallocate::test();
    polymorphic_allocator::mem::construct::test();
    polymorphic_allocator::mem::piecewise_construct::test();
    polymorphic_allocator::mem::construct_pair::test();
    polymorphic_allocator::mem::construct_pair_U_V::test();
    polymorphic_allocator::mem::construct_pair_lvalue_pair::test();
    polymorphic_allocator::mem::construct_pair_rvalue_pair::test();
    polymorphic_allocator::mem::select_on_container_copy_construction::test();
    polymorphic_allocator::mem::resource::test();
    polymorphic_allocator::eq::test();
    polymorphic_allocator::eq_cvt::test();
    polymorphic_allocator::destroy::test();

    monotonic::ctor::buffer_upstream::test();
    monotonic::ctor::size_upstream::test();
    monotonic::ctor::upstream::test();
    monotonic::mem::release::test();
    monotonic::mem::do_allocate::test();
    monotonic::mem::do_deallocate::test();

    pool::release::test();
    pool::upstream_resource::test();
    pool::options::test();
    pool::is_equal::test();
    pool::allocate_deallocate::test();

    containers::test();

    map_containers::test();
}
