#ifndef LIBP_SETS_FINITE_HPP_GUARD
#define LIBP_SETS_FINITE_HPP_GUARD

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <utility>
#include <set>
#include <libp/sets/conditional.hpp>
#include <libp/sets/measurable_set.hpp>

namespace libp {

    template<class T>
    class FiniteSet : public MeasurableSetCRTP<FiniteSet<T>> {
        public:
            FiniteSet() = default;

            FiniteSet(std::initializer_list<T> il): storage(std::move(il)) { }

            template<class R>
            auto operator&&(const FiniteSet<R>& rhs) const {
                FiniteSet<typename std::common_type<T,R>::type> intersection;
                std::set_intersection(
                    storage.cbegin(), storage.cend(),
                    rhs.storage.cbegin(), rhs.storage.cend(),
                    std::inserter(intersection.storage, intersection.storage.begin())
                );
                return intersection;
            }

            auto size(void) const { return storage.size(); }

        private:
            std::set<T> storage;
    };

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

