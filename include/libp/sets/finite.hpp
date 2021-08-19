#ifndef LIBP_SETS_FINITE_HPP_GUARD
#define LIBP_SETS_FINITE_HPP_GUARD

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <utility>
#include <set>
#include <libp/sets/conditional.hpp>

namespace libp {

    template<class T>
    using FiniteSet = std::set<T>;

    template<class L, class R>
    auto operator&&(const FiniteSet<L>& lhs, const FiniteSet<R>& rhs) {
        FiniteSet<typename std::common_type<L,R>::type> intersection;
        std::set_intersection(
            lhs.cbegin(), lhs.cend(),
            rhs.cbegin(), rhs.cend(),
            std::inserter(intersection, intersection.begin())
        );
        return intersection;
    }

    template<class T>
    auto finite_set(std::initializer_list<T> il) {
        return FiniteSet<T>(std::move(il));
    }

    template<class L, class R>
    auto operator|(FiniteSet<L> lhs, FiniteSet<R> rhs) {
        return conditional_set(std::move(lhs), std::move(rhs));
    }
};
#endif

