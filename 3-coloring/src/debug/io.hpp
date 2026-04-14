#pragma once

// source: Benq (https://codeforces.com/contest/2168/submission/370045998)

#include <bits/stdc++.h>

using namespace std;

#define LOCAL

inline namespace IO {

template <class T> concept DefaultI = requires(T a, basic_istream<char>& is) { is >> a; };
template <class T> concept DefaultO = requires(T a, basic_ostream<char>& os) { os << a; };
template <class T> concept TupleLike = requires(T a, size_t n) { std::tuple_size_v<T>; };

template <auto& is> struct Reader {
    template <class T> void Impl(T& t) {
        if constexpr (DefaultI<T>) is >> t;
        else if constexpr (ranges::range<T>) {
            for (auto& x : t) Impl(x);
        } else if constexpr (TupleLike<T>) {
            std::apply([this](auto& ...args) { (Impl(args), ...); }, t);
        } else static_assert(TupleLike<T>, "No matching type for read");
    }
    template <class... Ts> void read(Ts& ...ts) { ((Impl(ts)), ...); }
};

template <class... Ts> void re(Ts& ...ts) { Reader<cin>{}.read(ts...); }
#define def(t, args...)                                                        \
    t args;                                                                    \
    re(args);

#define de(t, name, args...)                                                   \
    t name(args);                                                              \
    re(name);

template <auto& os, bool debug, bool print_nd> struct Writer {
    string comma() const { return debug ? "," : ""; }
    template <class T> constexpr char Space(const T& ) const {
        return print_nd && (ranges::range<T> or TupleLike<T>) ? '\n'
                                                                     : ' ';
    }
    template <class T> void Impl(T const& t) const {
        if constexpr (DefaultO<T>) os << t;
        else if constexpr (ranges::range<T>) {
            if (debug) os << '{';
            int i = 0;
            for (auto &&x : t)
                ((i++) ? (os << comma() << Space(x), Impl(x)) : Impl(x));
            if (debug) os << '}';
        } else if constexpr (TupleLike<T>) {
            if (debug) os << '(';
            std::apply(
                [this](auto const& ...args) {
                    int i = 0;
                    (((i++) ? (os << comma() << " ", Impl(args)) : Impl(args)),
                     ...);
                },
                t);
            if (debug) os << ')';
        } else static_assert(TupleLike<T>, "No matching type for print");
    }
    template <class T> void ImplWrapper(T const& t) const {
        if (debug) os << "\033[0;31m";
        Impl(t);
        if (debug) os << "\033[0m";
    }
    template <class... Ts> void print(Ts const& ...ts) const {
        ((Impl(ts)), ...);
    }
    template <class F, class... Ts>
    void print_with_sep(const std::string& sep, F const& f,
                        Ts const& ...ts) const {
        ImplWrapper(f), ((os << sep, ImplWrapper(ts)), ...), os << '\n';
    }
    void print_with_sep(const std::string& ) const { os << '\n'; }
};

template <class... Ts> void pr(Ts const& ...ts) {
    Writer<cout, false, true>{}.print(ts...);
}

template <class... Ts> void ps(Ts const& ...ts) {
    Writer<cout, false, true>{}.print_with_sep(" ", ts...);
}

template <class... Ts> void pr_no_flush(Ts const& ...ts) {
    pr(ts...);
}

template <class... Ts> void ps_no_flush(Ts const& ...ts) {
    ps(" ", ts...);
}

template <class... Ts> void pr_flush(Ts const& ...ts) {
    pr(ts...);
    cout.flush();
}

template <class... Ts> void ps_flush(Ts const& ...ts) {
    ps(ts...);
    cout.flush();
}

}  // namespace IO