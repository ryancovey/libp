#ifndef LIBP_SETS_FINITE_HPP_GUARD
#define LIBP_SETS_FINITE_HPP_GUARD

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <ostream>
#include <type_traits>
#include <utility>
#include <vector>
#include <set>
#include <libp/internal/constants.hpp>
#include <libp/sets/all.hpp>
#include <libp/sets/conditional.hpp>
#include <libp/sets/integers.hpp>
#include <libp/sets/measurable_set_impl.hpp>
#include <libp/sets/natural_numbers.hpp>
#include <libp/sets/none.hpp>

namespace libp {

    inline namespace internal {
        template<class T, class... Args>
        using finite_set_default_container = std::vector<T, Args...>;
    }

    template<class T, template<class, class...> class Container>
    class FiniteSet;

    template<class T, template<class, class...> class Container>
    std::ostream& operator<<(std::ostream&, const FiniteSet<T, Container>&);

    template<class T, template<class, class...> class Container = finite_set_default_container>
    class FiniteSet : public MeasurableSetCRTP<FiniteSet<T, Container>> {
        template<class, template<class, class...> class> friend class FiniteSet;
        friend std::ostream& operator<< <> (std::ostream&, const FiniteSet<T, Container>&);

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
                    !std::is_base_of<MeasurableSetImpl, std::decay_t<C>>::value,
                    bool
                > = true
            >
            FiniteSet(C&& c) {
                storage.reserve(c.size());
                std::move(std::begin(c), std::end(c), std::inserter(storage, std::begin(storage)));
                make_sorted_unique(storage);
            }

            // If a MeasurableSetImpl of objects not of a similar type to T participates in an intersection
            // with this FiniteSet, clearly none of the elements represented in the former MeasurableSetImpl
            // can be present in the latter.
            template<
                class R,
                std::enable_if_t<std::is_base_of<MeasurableSetImpl, std::decay_t<R>>::value, bool> = true
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

            // If *this is a subset of the natural numbers and rhs are the natural numbers,
            // then *this && rhs == *this. We create a template NN that must always be
            // NaturalNumbers because enable_if only works on templates.
            template<
                class NN,
                std::enable_if_t<
                    std::is_unsigned<T>::value &&
                    std::is_same<NaturalNumbers, NN>::value,
                    bool
                > = true
            >
            auto operator&&(const NN&) {
                return *this;
            }

            // If *this is a subset of the integers and rhs are the natural numbers,
            // then *this && rhs contain all the elements of *this that are >= 0.
            template<
                class NN,
                std::enable_if_t<
                    std::is_integral<T>::value && std::is_signed<T>::value &&
                    std::is_same<NaturalNumbers, NN>::value,
                    bool
                > = true
            >
            auto operator&&(const NN&) {
                FiniteSet<T, Container> intersection;
                auto zero = internal::zero<T>();
                for (const auto& element : storage) {
                    if (element >= zero) {
                        intersection.storage.insert(std::end(intersection.storage), element);
                    }
                }
                return intersection;
            }

            // If *this is a subset of the real numbers and rhs is the natural numbers,
            // then *this && rhs contain all elements of *this that are natural numbers.
            template<
                class NN,
                std::enable_if_t<
                    std::is_floating_point<T>::value &&
                    std::is_same<NaturalNumbers, NN>::value,
                    bool
                > = true
            >
            auto operator&&(const NN&) {
                FiniteSet<T, Container> intersection;
                auto zero = internal::zero<T>();
                for (const auto& element : storage) {
                    if (element >= zero && static_cast<T>(static_cast<unsigned long long>(element)) == element) {
                        intersection.storage.insert(std::end(intersection.storage), element);
                    }
                }
                return intersection;
            }

            // If *this is a subset of the natural numbers and rhs is the natural numbers,
            // then *this || rhs == rhs.
            template<
                class NN,
                std::enable_if_t<
                    std::is_unsigned<T>::value &&
                    std::is_same<NaturalNumbers, NN>::value,
                    bool
                > = true
            >
            auto operator||(const NN& rhs) {
                return rhs;
            }

            // If *this is a subset of the integers and rhs are the integers,
            // then *this && rhs == *this.
            template<
                class IT,
                std::enable_if_t<
                    std::is_integral<T>::value &&
                    std::is_same<Integers, IT>::value,
                    bool
                > = true
            >
            auto operator&&(const IT&) {
                return *this;
            }

            // If *this is a subset of the real numbers and rhs are the integers,
            // then *this && rhs contain all elements of *this that are integers.
            template<
                class IT,
                std::enable_if_t<
                    std::is_floating_point<T>::value &&
                    std::is_same<Integers, IT>::value,
                    bool
                > = true
            >
            auto operator&&(const IT&) {
                FiniteSet<T, Container> intersection;
                for (const auto& element : storage) {
                    if (static_cast<T>(static_cast<long long>(element)) == element) {
                        intersection.storage.insert(std::end(intersection.storage), element);
                    }
                }
                return intersection;
            }

            // If *this is a subset of the integers and rhs are the integers,
            // then *this || rhs == rhs.
            template<
                class IT,
                std::enable_if_t<
                    std::is_integral<T>::value &&
                    std::is_same<Integers, IT>::value,
                    bool
                > = true
            >
            auto operator||(const IT& rhs) {
                return rhs;
            }

            // If *this is a subset of the real numbers and rhs are the real numbers,
            // then *this && rhs == *this.
            template<
                class RN,
                std::enable_if_t<
                    std::is_arithmetic<T>::value &&
                    std::is_same<RealNumbers, RN>::value,
                    bool
                > = true
            >
            auto operator&&(const RN&) {
                return *this;
            }

            // If *this is a subset of the real numbers and rhs are the real numbers,
            // then *this || rhs == rhs.
            template<
                class RN,
                std::enable_if_t<
                    std::is_arithmetic<T>::value &&
                    std::is_same<RealNumbers, RN>::value,
                    bool
                > = true
            >
            auto operator||(const RN& rhs) {
                return rhs;
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

            template<
                class RT,
                std::enable_if_t<
                    std::is_same<std::decay_t<T>, std::decay_t<RT>>::value &&
                    !std::is_arithmetic<RT>::value,
                    bool
                > = true
            >
            auto operator&&(const All<RT>&) {
                return *this;
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

    template<class T, template<class, class...> class Container = finite_set_default_container>
    std::ostream& operator<<(std::ostream& os, const FiniteSet<T, Container>& A) {
        auto iter = std::cbegin(A.storage);
        auto end = std::cend(A.storage);
        os << '{';
        if (iter != end) {
            os << *iter;
            ++iter;
            for (; iter != end; ++iter) {
                os << ", " << *iter;
            }
        }
        os << '}';
        return os;
    }

}

#endif

