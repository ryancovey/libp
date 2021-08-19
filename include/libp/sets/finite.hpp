#ifndef LIBP_SETS_FINITE_HPP_GUARD
#define LIBP_SETS_FINITE_HPP_GUARD

#include <algorithm>
#include <iterator>
#include <type_traits>
#include <set>

#include <iostream>

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

};
#endif

