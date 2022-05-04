#pragma once

template <typename Contained>
struct trivial {
    Contained c;
};

template <typename Contained>
inline bool operator==(const trivial<Contained>& lhs, const trivial<Contained>& rhs) {
    return lhs.c == rhs.c;
}

template <typename Contained>
inline bool operator!=(const trivial<Contained>& lhs, const trivial<Contained>& rhs) {
    return lhs.c != rhs.c;
}

template <typename Contained>
struct non_trivial {
    Contained c;
    non_trivial() : c() { /* user provided */
    }
    non_trivial(const Contained& src) : c(src) {}
    non_trivial(const non_trivial& other) : c(other.c) {}
    non_trivial& operator=(const non_trivial& other) {
        c = other.c;
        return *this;
    }

    ~non_trivial() { /* user provided */
    }
};

template <typename Contained>
inline bool operator==(const non_trivial<Contained>& lhs, const non_trivial<Contained>& rhs) {
    return lhs.c == rhs.c;
}

template <typename Contained>
inline bool operator!=(const non_trivial<Contained>& lhs, const non_trivial<Contained>& rhs) {
    return lhs.c != rhs.c;
}
