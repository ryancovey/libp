#ifndef LIBP_SETS_FINITE_HPP_GUARD
#define LIBP_SETS_FINITE_HPP_GUARD

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>
#include <set>
#include <libp/internal/constants.hpp>
#include <libp/sets/all.hpp>
#include <libp/sets/conditional.hpp>
#include <libp/sets/measurable_set.hpp>
#include <libp/sets/none.hpp>

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

            // If a MeasurableSet of objects not of a similar type to T participates in an intersection
            // with this FiniteSet, clearly none of the elements represented in the former MeasurableSet
            // can be present in the latter.
            template<
                class R,
                std::enable_if_t<std::is_base_of<MeasurableSet, std::decay_t<R>>::value, bool> = true
            >
            auto operator&&(const R&) const {
                return none();
            }

            // We have not yet implemented a set that can represent the union of
            // sets with elements of distinct types.
            /*
            template<class R>
            auto operator||(const R&) const {
                // todo.
            }
            */

            auto operator&&(const UniversalSet&) const {
                return *this;
            }

            auto operator||(const UniversalSet&) const {
                return all();
            }

            // If objects of type T cannot be converted to RT and back, then surely
            // *this && all<RT>() == none().
            template<
                class RT,
                std::enable_if_t<
                    (!std::is_convertible<T, RT>::value || !std::is_convertible<RT, T>::value) &&
                    (!std::is_arithmetic<T>::value || !std::is_arithmetic<RT>::value),
                    bool
                > = true
            >
            auto operator&&(const All<RT>&) {
                return none();
            }

            // If *this is a subset of all<RT>(), *this && all<RT>() == *this. Right now we're only doing
            // this for standard arithmetic types.
            template<
                class RT,
                std::enable_if_t<
                    (std::is_arithmetic<T>::value && std::is_floating_point<RT>::value) ||  // RT are the reals
                    (
                        std::is_integral<T>::value && !std::is_signed<T>::value &&          // T and RT are the integers.
                        std::is_integral<RT>::value && !std::is_signed<RT>::value
                    ) ||
                    (std::is_signed<T>::value && std::is_arithmetic<RT>::value),            // T are the naturals and zero.
                    bool
                > = true
            >
            auto operator&&(const All<RT>&) {
                return *this;
            }

            // If *this is a subset of the integers and all<RT>() contains the naturals and zero,
            // then *this && all<RT>() contain all elements of *this that are >= 0.
            template<
                class RT,
                std::enable_if_t<
                    std::is_integral<T>::value && std::is_unsigned<T>::value &&
                    std::is_integral<RT>::value && std::is_signed<RT>::value,
                    bool
                > = true
            >
            auto operator&&(const All<RT>&) {
                FiniteSet<T, Container> intersection;
                auto zero = internal::zero<T>();
                for (const auto& element : storage) {
                    if (element >= zero) {
                        intersection.storage.insert(std::end(intersection.storage), element);
                    }
                }
                return intersection;
            }

            // If *this is a subset of the reals and all<RT>() contains the naturals and zero,
            // then *this && all<RT>() contain all elements of *this that are naturals or zero.
            template<
                class RT,
                std::enable_if_t<
                    std::is_floating_point<T>::value &&
                    std::is_signed<RT>::value,
                    bool
                > = true
            >
            auto operator&&(const All<RT>&) {
                FiniteSet<T, Container> intersection;
                auto zero = internal::zero<T>();
                for (const auto& element : storage) {
                    if (element >= zero && static_cast<T>(static_cast<unsigned long long>(element)) == element) {
                        intersection.storage.insert(std::end(intersection.storage), element);
                    }
                }
                return intersection;
            }

            // If *this is a subset of the reals and all<RT>() contains the integers,
            // then *this && all<RT>() contain all elements of *this that are integers.
            template<
                class RT,
                std::enable_if_t<
                    std::is_floating_point<T>::value &&
                    std::is_integral<RT>::value && std::is_signed<RT>::value,
                    bool
                > = true
            >
            auto operator&&(const All<RT>&) {
                FiniteSet<T, Container> intersection;
                for (const auto& element : storage) {
                    if (static_cast<T>(static_cast<long long>(element)) == element) {
                        intersection.storage.insert(std::end(intersection.storage), element);
                    }
                }
                return intersection;
            }

            // Elements of *this that do not change when cast to RT and back to
            // T surely belong in *this && all<RT>().
            template<
                class RT,
                std::enable_if_t<
                    std::is_convertible<T, RT>::value &&
                    std::is_convertible<RT, T>::value &&
                    !(std::is_arithmetic<T>::value && std::is_arithmetic<RT>::value),
                    bool
                > = true
            >
            auto operator&&(const All<RT>&) {
                FiniteSet<T, Container> intersection;
                for (const auto& element : storage) {
                    if (static_cast<T>(static_cast<RT>(element)) == element) {
                        intersection.storage.insert(std::end(intersection.storage), element);
                    }
                }
                return intersection;
            }

            auto operator&&(const All<T>&) {
                return *this;
            }

            // If *this is a subset of all<RT>(), then *this || all<RT>() == all<RT>(). Like
            // for operator&&, we're currently only doing this for arithmetic types.
            template<
                class RT,
                std::enable_if_t<
                    (std::is_arithmetic<T>::value && std::is_floating_point<RT>::value) ||  // RT are the reals
                    (
                        std::is_integral<T>::value && !std::is_signed<T>::value &&          // T and RT are the integers.
                        std::is_integral<RT>::value && !std::is_signed<RT>::value
                    ) ||
                    (std::is_signed<T>::value && std::is_arithmetic<RT>::value),            // T are the naturals and zero.
                    bool
                > = true
            >
            auto operator||(const All<RT>&) {
                return all<RT>();
            }

            template<class RT, template<class, class...> class RC>
            auto operator&&(const FiniteSet<RT, RC>& rhs) const {
                FiniteSet<T, Container> intersection;
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

}

#endif

