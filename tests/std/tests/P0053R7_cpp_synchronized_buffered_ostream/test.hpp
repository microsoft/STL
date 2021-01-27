#include <assert.h>
#include <memory>
#include <string>
#include <syncstream>
#include <type_traits>
#include <utility>

using namespace std;

constexpr size_t _Min_size_allocation = 50;
constexpr size_t _Min_syncbuf_size    = 32;

template <class Ty, bool ThrowOnSync = false>
class string_buffer : public basic_streambuf<Ty, char_traits<Ty>> { // represents the wrapped object in syncbuf
public:
    string_buffer()  = default;
    ~string_buffer() = default;

    streamsize xsputn(const Ty* _Ptr, streamsize _Count) override {
        str.append(_Ptr, static_cast<string::size_type>(_Count));
        return _Count;
    }

    int sync() override {
        if constexpr (ThrowOnSync) {
            return -1;
        } else {
            return 0;
        }
    }

    string str;
};

class small_size_allocation {
public:
    using size_type = size_t;

    _NODISCARD size_type max_size() const noexcept {
        return _Min_size_allocation;
    }
};

template <class Ty>
class fancy_ptr {
private:
    uint64_t ptr = 0;

public:
    fancy_ptr() = default;
    fancy_ptr(std::nullptr_t) {}
    explicit fancy_ptr(Ty* _ptr) : ptr(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(_ptr))) {}
    template <class Other>
    explicit fancy_ptr(const fancy_ptr<Other>& right) : ptr(right.ptr) {}

    Ty& operator*() const {
        return *reinterpret_cast<Ty*>(static_cast<uintptr_t>(ptr));
    }

    template <class Other>
    static fancy_ptr<Other> pointer_to(Other& _ptr) noexcept {
        return fancy_ptr<Other>(addressof(_ptr));
    }
};

template <class Ty>
class fancy_ptr_allocator {
public:
    using value_type                             = Ty;
    using size_type                              = size_t;
    using pointer                                = fancy_ptr<Ty>;
    using propagate_on_container_move_assignment = true_type;
    using propagate_on_container_swap            = true_type;

    constexpr fancy_ptr_allocator() noexcept = default;

    _NODISCARD pointer allocate(_CRT_GUARDOVERFLOW const size_t _Count) {
        auto _Ptr = allocator<value_type>{}.allocate(_Count);
        return static_cast<pointer>(static_cast<Ty*>(_Ptr));
    }

    void deallocate(pointer const _Ptr, const size_t _Count) noexcept {
        allocator<value_type>{}.deallocate(&*_Ptr, _Count);
    }
};

template <class Ty, class Other>
_NODISCARD bool operator==(const fancy_ptr_allocator<Ty>&, const fancy_ptr_allocator<Other>&) noexcept {
    return true;
}

template <class Ty>
class small_size_fancy_ptr_allocator : public small_size_allocation {
public:
    using value_type                             = Ty;
    using pointer                                = fancy_ptr<Ty>;
    using propagate_on_container_move_assignment = true_type;
    using propagate_on_container_swap            = true_type;

    constexpr small_size_fancy_ptr_allocator() noexcept = default;

    _NODISCARD pointer allocate(_CRT_GUARDOVERFLOW const size_t _Count) {
        auto _Ptr = allocator<value_type>{}.allocate(_Count);
        return static_cast<pointer>(static_cast<Ty*>(_Ptr));
    }

    void deallocate(pointer const _Ptr, const size_t _Count) noexcept {
        allocator<value_type>{}.deallocate(&*_Ptr, _Count);
    }
};

template <class Ty, class Other>
_NODISCARD bool operator==(
    const small_size_fancy_ptr_allocator<Ty>&, const small_size_fancy_ptr_allocator<Other>&) noexcept {
    return true;
}

template <class Ty>
class small_size_allocator : public small_size_allocation {
public:
    using value_type                             = Ty;
    using pointer                                = Ty*;
    using propagate_on_container_move_assignment = true_type;
    using propagate_on_container_swap            = true_type;

    constexpr small_size_allocator() noexcept = default;

    _NODISCARD __declspec(allocator) pointer allocate(_CRT_GUARDOVERFLOW const size_t _Count) {
        return static_cast<pointer>(allocator<value_type>{}.allocate(_Count));
    }

    void deallocate(pointer const _Ptr, const size_t _Count) noexcept {
        allocator<value_type>{}.deallocate(_Ptr, _Count);
    }
};

template <class Ty, class Other>
_NODISCARD bool operator==(const small_size_allocator<Ty>&, const small_size_allocator<Other>&) noexcept {
    return true;
}

template <class Ty>
class non_move_assignable_non_equal_allocator {
public:
    using value_type                             = Ty;
    using size_type                              = size_t;
    using pointer                                = Ty*;
    using propagate_on_container_move_assignment = false_type;
    using propagate_on_container_swap            = true_type;
    using is_always_equal                        = false_type;

    constexpr non_move_assignable_non_equal_allocator() noexcept = default;

    _NODISCARD __declspec(allocator) pointer allocate(_CRT_GUARDOVERFLOW const size_t _Count) {
        return static_cast<pointer>(allocator<value_type>{}.allocate(_Count));
    }

    void deallocate(pointer const _Ptr, const size_t _Count) noexcept {
        allocator<value_type>{}.deallocate(_Ptr, _Count);
    }

    _NODISCARD size_type max_size() const noexcept {
        return 50;
    }
};

template <class Ty, class Other>
_NODISCARD bool operator==(const non_move_assignable_non_equal_allocator<Ty>&,
    const non_move_assignable_non_equal_allocator<Other>&) noexcept {
    return false;
}

template <class Ty>
class non_move_assignable_equal_allocator {
public:
    using value_type                             = Ty;
    using size_type                              = size_t;
    using pointer                                = Ty*;
    using propagate_on_container_move_assignment = false_type;
    using propagate_on_container_swap            = true_type;

    constexpr non_move_assignable_equal_allocator() noexcept = default;

    _NODISCARD __declspec(allocator) pointer allocate(_CRT_GUARDOVERFLOW const size_t _Count) {
        return static_cast<pointer>(allocator<value_type>{}.allocate(_Count));
    }

    void deallocate(pointer const _Ptr, const size_t _Count) noexcept {
        allocator<value_type>{}.deallocate(_Ptr, _Count);
    }
};

template <class Ty, class Other>
_NODISCARD bool operator==(
    const non_move_assignable_equal_allocator<Ty>&, const non_move_assignable_equal_allocator<Other>&) noexcept {
    return true;
}

template <class Ty>
class non_swapable_equal_allocator {
public:
    using value_type                             = Ty;
    using size_type                              = size_t;
    using pointer                                = Ty*;
    using propagate_on_container_move_assignment = true_type;
    using propagate_on_container_swap            = false_type;

    constexpr non_swapable_equal_allocator() noexcept = default;

    _NODISCARD __declspec(allocator) pointer allocate(_CRT_GUARDOVERFLOW const size_t _Count) {
        return static_cast<pointer>(allocator<value_type>{}.allocate(_Count));
    }

    void deallocate(pointer const _Ptr, const size_t _Count) noexcept {
        allocator<value_type>{}.deallocate(_Ptr, _Count);
    }
};

template <class Ty, class Other>
_NODISCARD bool operator==(
    const non_swapable_equal_allocator<Ty>&, const non_swapable_equal_allocator<Other>&) noexcept {
    return true;
}
