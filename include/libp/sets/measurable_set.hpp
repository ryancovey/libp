#ifndef LIBP_SETS_MEASURABLE_SET_HPP_GUARD
#define LIBP_SETS_MEASURABLE_SET_HPP_GUARD

#include <array>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <libp/internal/function_register.hpp>
#include <libp/internal/type_index.hpp>

namespace libp {

    class MeasurableSet;

    class MeasurableSetImpl {
        friend class MeasurableSet;

        public:
            virtual ~MeasurableSetImpl() = default;
    };

    template<class T>
    class MeasurableSetCRTP : public MeasurableSetImpl {
        friend class MeasurableSet;

        public:
            static void register_type(void) {
                // If the concrete MeasurableSetImpl does not override this function, it
                // means that the only known relationships are between the set represented
                // by T, and itself. By default, it is assumed that intersections between
                // types of unknown relationships equals null.
            }

        protected:
            static void default_intersection_this(void);
            static void default_intersection_other(void);

            static void default_union_this(void);
            static void default_union_other(void);

            template<class U1>
            static void register_operators(void);

            template<class U1, class U2, class... Us>
            static void register_operators(void);

        private:
            // The type_index variable being static (and a variable)
            // ensures that operators and types are registered
            // statically, before main runs, and not after.
            static std::size_t type_index;
            static std::size_t get_type_index(void);
    };

    inline namespace internal {

        inline MeasurableSet get_this_set(const MeasurableSet& this_set, const MeasurableSet& other_set);
        inline MeasurableSet get_other_set(const MeasurableSet& this_set, const MeasurableSet& other_set);

        // Defined in libp/sets/null.hpp, included below.
        inline MeasurableSet abstract_none(void);

    }

    class MeasurableSet {
        template<class T>
        friend class MeasurableSetCRTP;

        friend std::ostream& operator<<(std::ostream&, const MeasurableSet&);

        public:
            template<
                class T,
                std::enable_if_t<
                    std::is_base_of<MeasurableSetImpl, std::decay_t<T>>::value &&
                    !std::is_same<MeasurableSetImpl, std::decay_t<T>>::value,
                    bool
                > = true
            >
            MeasurableSet(T&& set):
                pImpl(new std::decay_t<T>(std::forward<T>(set))),
                type_index(MeasurableSetCRTP<std::decay_t<T>>::type_index)
            { }

            MeasurableSet operator&&(const MeasurableSet& rhs) const {
                union { SetValuedBivariateImpl pairwise_intersection; SetValuedBivariate default_intersection; };
                if (pairwise_intersection = pairwise_intersection_register().get_function({type_index, rhs.type_index})) {
                    return pairwise_intersection(*pImpl, *rhs.pImpl);
                } else if (default_intersection = default_intersection_register().get_function({type_index})) {
                    return default_intersection(*this, rhs);
                } else if (default_intersection = default_intersection_register().get_function({rhs.type_index})) {
                    return default_intersection(rhs, *this);
                } else {
                    return abstract_none();
                }
            }

            MeasurableSet operator||(const MeasurableSet& rhs) const {
                union { SetValuedBivariateImpl pairwise_union; SetValuedBivariate default_union; };
                if (pairwise_union = pairwise_union_register().get_function({type_index, rhs.type_index})) {
                    return pairwise_union(*pImpl, *rhs.pImpl);
                } else if (default_union = default_union_register().get_function({type_index})) {
                    return default_union(*this, rhs);
                } else if (default_union = default_union_register().get_function({rhs.type_index})) {
                    return default_union(rhs, *this);
                } else {
                    throw std::logic_error(
                        "Unknown MeasurableSet union. "
                        "Have you called MeasurableSet::register_operators for all interacting pairs of abstract MeasurableSets with templated concrete types?"
                    );
                }
            }

            template<class T, class U>
            static void register_operators(void) {
                auto T_type_index = get_type_index<MeasurableSet, T>();
                auto U_type_index = get_type_index<MeasurableSet, U>();
                register_operators_onesided<T, U>(T_type_index, U_type_index);
                register_operators_onesided<U, T>(U_type_index, T_type_index);
            }

        private:
            std::shared_ptr<MeasurableSetImpl> pImpl;
            std::size_t type_index;

            using SetValuedBivariate = MeasurableSet (*)(const MeasurableSet&, const MeasurableSet&);
            using SetValuedBivariateImpl = MeasurableSet (*)(const MeasurableSetImpl&, const MeasurableSetImpl&);
            using BooleanValuedBivariateImpl = bool (*)(const MeasurableSetImpl&, const MeasurableSetImpl&);
            using OutputOperator = std::ostream& (*)(std::ostream& os, const MeasurableSetImpl&);

            template<int N>
            static auto& get_default_register(void) {
                static auto& reg = *new FunctionRegister<SetValuedBivariate, 1>({8});
                return reg;
            }

            template<int N>
            static auto& get_pairwise_register(void) {
                static auto& reg = *new FunctionRegister<SetValuedBivariateImpl, 2>({8, 8});
                return reg;
            }

            static FunctionRegister<SetValuedBivariate, 1>& default_intersection_register(void) { return get_default_register<0>(); }
            static FunctionRegister<SetValuedBivariateImpl, 2>& pairwise_intersection_register(void) { return get_pairwise_register<0>(); }
            static FunctionRegister<SetValuedBivariate, 1>& default_union_register(void) { return get_default_register<1>(); }
            static FunctionRegister<SetValuedBivariateImpl, 2>& pairwise_union_register(void) { return get_pairwise_register<1>(); }

            static FunctionRegister<OutputOperator, 1>& output_operator_register(void) {
                static auto& reg = *new FunctionRegister<OutputOperator, 1>(
                    {8}, 
                    [](std::ostream&, const MeasurableSetImpl&) -> std::ostream& {
                        throw std::logic_error("A MeasurableSetImpl has an undefined output operator.");
                    }
                );
                return reg;
            }

            template<class T>
            static void default_intersection_this(void) {
                default_intersection_register().register_function(
                    {get_type_index<MeasurableSet, T>()},
                    get_this_set
                );
            }

            template<class T>
            static void default_intersection_other(void) {
                default_intersection_register().register_function(
                    {get_type_index<MeasurableSet, T>()},
                    get_other_set
                );
            }

            template<class T>
            static void default_union_this(void) {
                default_union_register().register_function(
                    {get_type_index<MeasurableSet, T>()},
                    get_this_set
                );
            }

            template<class T>
            static void default_union_other(void) {
                default_union_register().register_function(
                    {get_type_index<MeasurableSet, T>()},
                    get_other_set
                );
            }

            template<
                class L,
                class R,
                std::enable_if_t<
                    std::is_base_of<MeasurableSetImpl, std::decay_t<L>>::value &&
                    !std::is_same<MeasurableSetImpl, std::decay_t<L>>::value &&
                    std::is_base_of<MeasurableSetImpl, std::decay_t<R>>::value &&
                    !std::is_same<MeasurableSetImpl, std::decay_t<R>>::value,
                    bool
                > = true
            >
            static void register_operators_onesided(std::size_t L_type_index, std::size_t R_type_index) {
                pairwise_intersection_register().register_function(
                    {L_type_index, R_type_index},
                    [](const MeasurableSetImpl& lhs, const MeasurableSetImpl& rhs) -> MeasurableSet {
                        return static_cast<const L&>(lhs) && static_cast<const R&>(rhs);
                    }
                );
                
                pairwise_union_register().register_function(
                    {L_type_index, R_type_index},
                    [](const MeasurableSetImpl& lhs, const MeasurableSetImpl& rhs) -> MeasurableSet {
                        return static_cast<const L&>(lhs) || static_cast<const R&>(rhs);
                    }
                );
            }

            template<class L, class R>
            static void register_operators_onesided(void) {
                register_operators_onesided<L, R>(
                    get_type_index<MeasurableSet, L>(),
                    get_type_index<MeasurableSet, R>()
                );
            }

            template<class T>
            static std::size_t register_operators(void) {
                auto T_type_index = get_type_index<MeasurableSet, T>();

                register_operators_onesided<T, T>(T_type_index, T_type_index);

                output_operator_register().register_function(
                    {T_type_index},
                    [](std::ostream& os, const MeasurableSetImpl& set) -> std::ostream& {
                        return os << static_cast<const T&>(set);
                    }
                );

                return T_type_index;
            }

    };

    std::ostream& operator<<(std::ostream& os, const MeasurableSet& set) {
        // Need to .get_function rather than .execute_function since std::ostream(const std::ostream&) is deleted.
        return MeasurableSet::output_operator_register().get_function({set.type_index})(os, *set.pImpl);
    }

}

#include <libp/sets/null.hpp>

namespace libp {

    inline namespace internal {

        MeasurableSet get_this_set(const MeasurableSet& this_set, const MeasurableSet& other_set) {
            return this_set;
        }

        MeasurableSet get_other_set(const MeasurableSet& this_set, const MeasurableSet& other_set) {
            return other_set;
        }

    }

    template<class T>
    void MeasurableSetCRTP<T>::default_intersection_this(void) {
        MeasurableSet::default_intersection_this<T>();
    }

    template<class T>
    void MeasurableSetCRTP<T>::default_intersection_other(void) {
        MeasurableSet::default_intersection_other<T>();
    }

    template<class T>
    void MeasurableSetCRTP<T>::default_union_this(void) {
        MeasurableSet::default_union_this<T>();
    }

    template<class T>
    void MeasurableSetCRTP<T>::default_union_other(void) {
        MeasurableSet::default_union_other<T>();
    }

    template<class T>
    template<class U1>
    void MeasurableSetCRTP<T>::register_operators(void) {
        MeasurableSet::register_operators<T,U1>();
    }

    template<class T>
    template<class U1, class U2, class... Us>
    void MeasurableSetCRTP<T>::register_operators(void) {
        MeasurableSet::register_operators<T,U1>();
        register_operators<U2, Us...>();
    }

    template<class T>
    std::size_t MeasurableSetCRTP<T>::get_type_index(void) {
        T::register_type();
        return MeasurableSet::register_operators<T>();
    }

    template<class T>
    std::size_t MeasurableSetCRTP<T>::type_index = get_type_index();

    template<
        class T,
        std::enable_if_t<std::is_base_of<MeasurableSetImpl, std::decay_t<T>>::value, bool> = true
    >
    auto operator==(const T&, const T&) {
        return true;
    }

    template<
        class L,
        class R,
        std::enable_if_t<
            std::is_base_of<MeasurableSetImpl, std::decay_t<L>>::value &&
            std::is_base_of<MeasurableSetImpl, std::decay_t<R>>::value &&
            !std::is_same<std::decay_t<L>, std::decay_t<R>>::value,
            bool
        > = true
    >
    auto operator==(const L&, const R&) {
        return false;
    }

    template<
        class L,
        class R,
        std::enable_if_t<
            std::is_base_of<MeasurableSetImpl, std::decay_t<L>>::value &&
            std::is_base_of<MeasurableSetImpl, std::decay_t<R>>::value,
            bool
        > = true
    >
    bool operator!=(const L& lhs, const R& rhs) {
        return !(lhs == rhs);
    }

    template<
        class T,
        std::enable_if_t<std::is_base_of<MeasurableSetImpl, std::decay_t<T>>::value, bool> = true
    >
    auto operator&&(const T& lhs, const T&) {
        return lhs;
    }

    template<
        class T,
        std::enable_if_t<std::is_base_of<MeasurableSetImpl, std::decay_t<T>>::value, bool> = true
    >
    auto operator||(const T& lhs, const T&) {
        return lhs;
    }

}

#endif

