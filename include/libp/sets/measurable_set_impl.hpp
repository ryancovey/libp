#ifndef LIBP_SETS_MEASURABLE_SET_IMPL_HPP_GUARD
#define LIBP_SETS_MEASURABLE_SET_IMPL_HPP_GUARD

#include <array>
#include <memory>
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
            void register_type(void) {
                // todo: throw an exception here, since this function should be overridden by
                // derived classes, and this function runs during static initialisation if
                // it is not.
            }

        protected:
            template<class U>
            static void register_operators(void);

        private:
            // The type_index variable being static (and a variable)
            // ensures that operators and types are registered
            // statically, before main runs, and not after.
            static std::size_t type_index;
            static std::size_t get_type_index(void);
    };

    class MeasurableSet {
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

            MeasurableSet operator&&(const MeasurableSet& rhs) {
                return intersection_register().execute_function({type_index, rhs.type_index}, *pImpl, *(rhs.pImpl));
            }

            MeasurableSet operator||(const MeasurableSet& rhs) {
                return union_register().execute_function({type_index, rhs.type_index}, *pImpl, *(rhs.pImpl));
            }

        private:
            std::shared_ptr<MeasurableSetImpl> pImpl;
            std::size_t type_index;

            using BinaryOperator = MeasurableSet (*)(const MeasurableSetImpl&, const MeasurableSetImpl&);

            static MeasurableSet unknown_operator(const MeasurableSetImpl&, const MeasurableSetImpl&) {
                throw std::logic_error(
                    "Unknown binary MeasurableSet operator. "
                    "Have you called MeasurableSet::register_operators for all interacting pairs of abstract MeasurableSets with templated concrete types?"
                );
            }

            template<int N>
            static auto& get_register(void) {
                static auto& reg = *new FunctionRegister<BinaryOperator>({8, 8}, unknown_operator);
                return reg;
            }

            static FunctionRegister<BinaryOperator>& intersection_register(void) { return get_register<0>(); }
            static FunctionRegister<BinaryOperator>& union_register(void) { return get_register<1>(); }

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
                intersection_register().register_function(
                    {L_type_index, R_type_index},
                    [](const MeasurableSetImpl& lhs, const MeasurableSetImpl& rhs) -> MeasurableSet {
                        return static_cast<const L&>(lhs) && static_cast<const R&>(rhs);
                    }
                );
                
                union_register().register_function(
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

            template<class T, class U>
            static void register_operators(void) {
                auto T_type_index = get_type_index<MeasurableSet, T>();
                auto U_type_index = get_type_index<MeasurableSet, U>();
                register_operators_onesided<T, U>(T_type_index, U_type_index);
                register_operators_onesided<U, T>(U_type_index, T_type_index);
            }

            template<class T>
            static std::size_t register_operators(void) {
                auto T_type_index = get_type_index<MeasurableSet, T>();
                register_operators_onesided<T, T>(T_type_index, T_type_index);
                return T_type_index;
            }

    };

    template<class T>
    template<class U>
    void MeasurableSetCRTP<T>::register_operators(void) {
        MeasurableSet::register_operators<T,U>();
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

