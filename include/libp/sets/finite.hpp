#ifndef LIBP_SETS_FINITE_HPP_GUARD
#define LIBP_SETS_FINITE_HPP_GUARD

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>
#include <set>
#include <libp/sets/conditional.hpp>
#include <libp/sets/measurable_set.hpp>

namespace libp {

    inline namespace internal {
        template<class T, class... Args>
        using finite_set_default_container = std::vector<T, Args...>;
    }

    template<class T, template<class, class...> class Container = finite_set_default_container>
    class FiniteSet : public MeasurableSetCRTP<FiniteSet<T, Container>> {
        template<class, template<class, class...> class> friend class FiniteSet;

        public:
            FiniteSet() = default;

            FiniteSet(std::initializer_list<T> il): storage(std::move(il)) { make_sorted_unique(storage); }

            FiniteSet(Container<T> c): storage(std::move(c)) { make_sorted_unique(storage); }

            template<class C>
            FiniteSet(const C& c): storage(std::cbegin(c), std::cend(c)) { make_sorted_unique(storage); }

            template<
                class C,
                std::enable_if_t<
                    !std::is_reference<C>::value && // Disable constructor for lvalue arguments.
                    !std::is_base_of<MeasurableSet, std::decay_t<C>>::value,
                    bool
                > = true
            >
            FiniteSet(C&& c) {
                storage.reserve(c.size());
                std::move(std::begin(c), std::end(c), std::inserter(storage, std::begin(storage)));
                make_sorted_unique(storage);
            }

            template<class RT, template<class, class...> class RC>
            auto operator&&(const FiniteSet<RT, RC>& rhs) const {
                FiniteSet<typename std::common_type<T,RT>::type, Container> intersection;
                std::set_intersection(
                    std::cbegin(storage), std::cend(storage),
                    std::cbegin(rhs.storage), std::cend(rhs.storage),
                    std::inserter(intersection.storage, std::begin(intersection.storage))
                );
                return intersection;
            }

            template<class RT, template<class, class...> class RC>
            auto operator||(const FiniteSet<RT, RC>& rhs) const {
                FiniteSet<typename std::common_type<T,RT>::type, Container> finite_set_union;
                std::set_union(
                    std::cbegin(storage), std::cend(storage),
                    std::cbegin(rhs.storage), std::cend(rhs.storage),
                    std::inserter(finite_set_union.storage, std::begin(finite_set_union.storage))
                );
                return finite_set_union;
            }

            auto size(void) const { return storage.size(); }

        private:
            Container<T> storage;

            template<class C>
            void make_sorted(C& c) {
                std::sort(std::begin(c), std::end(c));
            }

            template<class S>
            void make_sorted(std::set<S>& c) { }

            template<class C>
            void remove_consecutive_duplicates(C& c) {
                auto last = std::unique(std::begin(c), std::end(c));
                c.erase(last, std::end(c));
            }

            template<class S>
            void remove_consecutive_duplicates(std::set<S>& c) { }

            template<class C>
            void make_sorted_unique(C& c) {
                make_sorted(c);
                remove_consecutive_duplicates(c);
            }
    };

    template<class T, template<class, class...> class Container = finite_set_default_container>
    auto finite_set(std::initializer_list<T> il) {
        return FiniteSet<T, Container>(std::move(il));
    }

    template<
        class LT,
        template<class, class...> class LC,
        class RT,
        template<class, class...> class RC
    >
    auto operator|(FiniteSet<LT, LC> lhs, FiniteSet<RT, RC> rhs) {
        return conditional_set(std::move(lhs), std::move(rhs));
    }

};
#endif

