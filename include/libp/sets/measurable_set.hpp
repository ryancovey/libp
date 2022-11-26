#ifndef LIBP_SETS_MEASURABLE_SET_HPP_GUARD
#define LIBP_SETS_MEASURABLE_SET_HPP_GUARD

#include <array>
#include <limits>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <libp/internal/function_register.hpp>
#include <libp/internal/type_index.hpp>

namespace libp {

    // See comments at the top of the definition to MeasurableSetHandler below
    // for an explanation for the dummy template argument.

    template<bool Dummy>
    class MeasurableSetHandler;

    using MeasurableSet = MeasurableSetHandler<true>;

    class MeasurableSetImpl {
        friend MeasurableSet;

        public:
            virtual ~MeasurableSetImpl() = default;
    };

    inline namespace internal {

        // See https://stackoverflow.com/a/16824239

        template<class, class>
        class has_register_type { };

        template<class C, class Ret, class... Args>
        class has_register_type<C, Ret(Args...)> {
            private:
                template<class T>
                static constexpr auto check(T*) -> typename std::is_same<decltype(T::register_type(std::declval<Args>()...)), Ret>::type;

                template<class>
                static constexpr std::false_type check(...);

            public:
                typedef decltype(check<C>(0)) type;
                static constexpr bool value = type::value;
        };

        template<class C, class Ret, class... Args>
        using has_register_type_t = typename has_register_type<C, Ret, Args...>::type;

    }

    template<class T>
    class MeasurableSetCRTP : public MeasurableSetImpl {
        template<class U>
        friend class MeasurableSetCRTP;

        friend MeasurableSet;

        protected:
            static void default_intersection_this(void);
            static void default_intersection_other(void);

            static void default_union_this(void);
            static void default_union_other(void);

            template<class U1>
            static void register_operators(std::size_t type_index);

            template<class U1, class U2, class... Us>
            static void register_operators(std::size_t type_index);

        private:
            // If T::register_type(void) exists, call it in MeasurableSetCRTP<T>::register_type(void),
            // otherwise MeasurableSetCRTP<T>::register_type(void) is a noop.
            static void maybe_register_type(std::true_type) { T::register_type(); }
            static void maybe_register_type(std::false_type) { }
            static void maybe_register_type(void) { maybe_register_type(has_register_type_t<T, void(void)>()); }

            // As above, but for T::register_type(std::size_t).
            static void maybe_register_type(std::size_t i, std::true_type) { T::register_type(i); }
            static void maybe_register_type(std::size_t, std::false_type) { }
            static void maybe_register_type(std::size_t i) { maybe_register_type(i, has_register_type_t<T, void(std::size_t)>()); }

            template<class U1>
            static void register_one_operator(std::size_t type_index);

            template<class U1, class U2, class... Us>
            static void register_one_operator(std::size_t type_index);

            template<class U1>
            static void register_operators(void);

            template<class U1, class U2, class... Us>
            static void register_operators(void);

            // The type_index variable being static (and a variable)
            // ensures that operators and types are registered
            // statically, before main runs, and not after.
            static std::size_t registered_type_index;
            static std::size_t get_registered_type_index(void);

            // Due to implicit instantiation rules (see https://en.cppreference.com/w/cpp/language/class_template), this
            // type is only registered (via a call to get_type_index during static initialisation) if type_index is used
            // somewhere, and it is only used by MeasurableSet::MeasurableSet<T>. By setting type_registered to false at
            // instantiation and true in get_type_index, we can query MeasurableSetCRTP<U>::type_registered in
            // MeasurableSetCRTP<T>::register_operators<U> and only register operators between arguments of type T and U
            // when U has already been registered and T is being registered, or vice versa The benefit of this approach
            // is that operators between sets of type U and T are registered if and only if objects of type U and T are
            // used to construct a MeasurableSet object somewhere in the program.
            static bool type_registered;
    };

    template<class T>
    std::size_t MeasurableSetCRTP<T>::registered_type_index = get_registered_type_index();

    template<class T>
    bool MeasurableSetCRTP<T>::type_registered = false;

    inline namespace internal {

        inline MeasurableSet get_this_set(const MeasurableSet& this_set, const MeasurableSet& other_set);
        inline MeasurableSet get_other_set(const MeasurableSet& this_set, const MeasurableSet& other_set);

    }

    template<bool Dummy = true>
    class MeasurableSetHandler {
        // We need this class to be a template to take advantage of implicit instantiation rules applicable to class
        // templates, but not ordinary classes (see https://en.cppreference.com/w/cpp/language/class_template). In
        // particular, functions and static member variables of template classes that are not used are not instantiated.
        // This means that, to a first approximation, function calls used to initialise static member variables of
        // template classes are run (and hence their side effects occur) if and only if the resulting program actually
        // depends on the function call or static member variable in question.
        //
        // The key static member variable here is MeasurableSetCRTP<std::decay_t<T>>>::register_type_index and is
        // referred to in the public constructor of MeasurableSetHandler. This variable is initialised by a call to
        // get_registered_type_index, which does the work of registering std::decay_t<T> for multiple dispatch. Thus,
        // registration of std::decay_t<T> for multiple dispatch occurs if and only if the constructor
        // MeasurableSetHandler::MeasurableSetHandler<T> is used, which occurs if and only if a MeasurableSetHandler
        // is constructed somewhere in the program with a set of type std::decay_t<T>. By only refering to the variable
        // MeasurableSetCRTP<std::decay_t<T>>::registered_type_index in the public constructor of MeasurableSetHandler,
        // we can ensure that std::decay_t<T> is not registered for multiple dispatch when the program does not use
        // MeasurableSetHandler to hold a set of type std::decay_t<T>.
        // 
        // If MeasurableSetHandler were not a template, then MeasurableSetHandler::operator&& could be instantiated
        // even if it is not used, which for the call "lhs && rhs" returns a MeasurableSetHandler holding a NullSet
        // if lhs and rhs are otherwise unrelated. This triggers the registration of NullSet for multiple dispatch.
        // That this would occur even for programs that don't use MeasurableSetHandler at all is a violation of the
        // C++ maxim: "You don't pay for what you don't use." Fortunately, we can avoid this redundant registration
        // by making MeasurableSetHandler a template class with a dummy template argument, and defining an alias
        // declaration MeasurableSet that resolves to an instantiation of the class MeasurableSetHandler with the
        // (unique) correct template argument. With this fix, programs that don't use MeasurableSetHandler at all
        // don't incur the cost of registering NullSet for multiple dispatch.

        static_assert(
            Dummy,
            "MeasuarbleSetHandler<false> undefined, use MeasurableSet, which is an alias for MeasurableSetHandler<true>. "
            "The template argument is a dummy."
        );

        template<class T>
        friend class MeasurableSetCRTP;

        friend std::ostream& operator<<(std::ostream&, const MeasurableSet&);

        public:
            // Use of this public constructor ensures std::decay_t<T> is registered for multiple dispatch via referring to
            // MeasurableSetCRTP<std::decay_t<T>>::registered_type_index, which triggers implicit instantiation of the
            // static member variable MeasurableSetCRTP<std::decay_t<T>>::registered_type_index, which is initialised
            // via a call to the static member function MeasurableSetCRTP<std::decay_t<T>>::get_registered_type_index(),
            // which does the registration.
            template<
                class T//,
                /*
                std::enable_if_t<
                    std::is_base_of<MeasurableSetImpl, std::decay_t<T>>::value &&
                    !std::is_same<MeasurableSetImpl, std::decay_t<T>>::value,
                    bool
                > = true
                */
                // Uncommenting this template argument causes "error: invalid use of incomplete type 'class libp::EmptySet'".
                // Since we're planning on expressing everything as an interval, this file is likely to be deleted in the near
                // future, so no need to address the error.
            >
            MeasurableSetHandler(T&& set):
                MeasurableSetHandler(std::forward<T>(set), MeasurableSetCRTP<std::decay_t<T>>::registered_type_index)
            { }

            // operator&& depends on NullSet, so we define it later, after including libp/sets/null.hpp.
            MeasurableSet operator&&(const MeasurableSet& rhs) const;

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
                auto T_type_index = get_type_index<MeasurableSetImpl, T>();
                auto U_type_index = get_type_index<MeasurableSetImpl, U>();
                register_operators_onesided<T, U>(T_type_index, U_type_index);
                register_operators_onesided<U, T>(U_type_index, T_type_index);
            }

        private:
            // Use of this constructor for a type_index_in other than MeasurableSetCRTP<std::decay_t<T>>::registered_type_index
            // will not trigger registration of std::decay_t<T> for multiple dispatch, which would otherwise happen during
            // static initialisation. This constructor is used in the lambdas in register_operators_onesided to avoid triggering
            // registration when a MeasurableSet holding a std::decay_t<T> (and therefore constructed using the constructor
            // above that registers std::decay_t<T> for multiple dispatch) is not actually used by a user of libp.
            template<
                class T/*,
                std::enable_if_t<
                    std::is_base_of<MeasurableSetImpl, std::decay_t<T>>::value &&
                    !std::is_same<MeasurableSetImpl, std::decay_t<T>>::value,
                    bool
                > = true
                */
            >
            MeasurableSetHandler(T&& set, std::size_t type_index_in):
                pImpl(new std::decay_t<T>(std::forward<T>(set))),
                type_index(type_index_in)
            { }

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
                    {get_type_index<MeasurableSetImpl, T>()},
                    get_this_set
                );
            }

            template<class T>
            static void default_intersection_other(void) {
                default_intersection_register().register_function(
                    {get_type_index<MeasurableSetImpl, T>()},
                    get_other_set
                );
            }

            template<class T>
            static void default_union_this(void) {
                default_union_register().register_function(
                    {get_type_index<MeasurableSetImpl, T>()},
                    get_this_set
                );
            }

            template<class T>
            static void default_union_other(void) {
                default_union_register().register_function(
                    {get_type_index<MeasurableSetImpl, T>()},
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
                // For the below lambdas, return a MeasurableSet constructed with the private constructor,
                // getting the type index of the concrete union or intersection from get_type_index,
                // which does not trigger registration of type of the concrete result for multiple dispatch,
                // instead of MeasurableSetCRTP<decltype(union or intersection)>::registered_type_index, which
                // does. See comments to the MeasurableSet constructors above for more details. The key insight
                // is that we only want to trigger registration of a concrete set type for multiple dispatch
                // if that type is at some point held by a MeasurableSet object of a user of libp, so libp
                // itself should never use the public MeasurableSet constructor, nor refer to registered_type_index.

                pairwise_intersection_register().register_function(
                    {L_type_index, R_type_index},
                    [](const MeasurableSetImpl& lhs, const MeasurableSetImpl& rhs) {
                        auto concrete_intersection = static_cast<const L&>(lhs) && static_cast<const R&>(rhs);
                        return MeasurableSet(std::move(concrete_intersection), get_type_index<MeasurableSetImpl, decltype(concrete_intersection)>());
                    }
                );
                
                pairwise_union_register().register_function(
                    {L_type_index, R_type_index},
                    [](const MeasurableSetImpl& lhs, const MeasurableSetImpl& rhs) {
                        auto concrete_union = static_cast<const L&>(lhs) || static_cast<const R&>(rhs);
                        return MeasurableSet(std::move(concrete_union), get_type_index<MeasurableSetImpl, decltype(concrete_union)>());
                    }
                );
            }

            template<class L, class R>
            static void register_operators_onesided(void) {
                register_operators_onesided<L, R>(
                    get_type_index<MeasurableSetImpl, L>(),
                    get_type_index<MeasurableSetImpl, R>()
                );
            }

            template<class T>
            static std::size_t register_operators(void) {
                auto T_type_index = get_type_index<MeasurableSetImpl, T>();

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

    inline std::ostream& operator<<(std::ostream& os, const MeasurableSet& set) {
        // Need to .get_function rather than .execute_function since std::ostream(const std::ostream&) is deleted.
        return MeasurableSet::output_operator_register().get_function({set.type_index})(os, *set.pImpl);
    }

}

#include <libp/sets/empty.hpp>

namespace libp {

    template<bool Dummy>
    MeasurableSet MeasurableSetHandler<Dummy>::operator&&(const MeasurableSet& rhs) const {
        union { SetValuedBivariateImpl pairwise_intersection; SetValuedBivariate default_intersection; };
        if (pairwise_intersection = pairwise_intersection_register().get_function({type_index, rhs.type_index})) {
            return pairwise_intersection(*pImpl, *rhs.pImpl);
        } else if (default_intersection = default_intersection_register().get_function({type_index})) {
            return default_intersection(*this, rhs);
        } else if (default_intersection = default_intersection_register().get_function({rhs.type_index})) {
            return default_intersection(rhs, *this);
        } else {
            return empty_set();
        }
    }

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
    void MeasurableSetCRTP<T>::register_operators(std::size_t type_index) {
        // Note that we depend on short-circuit behaviour here so that U1 is not allocated a type index
        // (which happens when get_type_index<MeasurableSetImpl, U1>() is called for the first time) until
        // it is registered. The ensures that the type indices of registered types are as low as possible,
        // and since the size of the storage in the registers (see libp/internal/function_register.hpp)
        // increases with the maximum type index, we also ensure that this storage is a compact as possible.
        if (
            MeasurableSetCRTP<U1>::type_registered &&
            (type_index == std::numeric_limits<std::size_t>::max() || type_index == get_type_index<MeasurableSetImpl, U1>())
        ) {
            MeasurableSet::register_operators<T,U1>();
        }
    }

    template<class T>
    template<class U1, class U2, class... Us>
    void MeasurableSetCRTP<T>::register_operators(std::size_t type_index) {
        if (type_index == std::numeric_limits<std::size_t>::max()) {
            register_operators<U1, U2, Us...>();
        } else {
            register_one_operator<U1, U2, Us...>(type_index);
        }
    }

    template<class T>
    template<class U1>
    void MeasurableSetCRTP<T>::register_one_operator(std::size_t type_index) {
        // Note that we depend on short-circuit behaviour here, see comment at the top of
        // MeasurableSetCRTP<T>::register_operators<U1>(std::size_t type_index) above.
        if (MeasurableSetCRTP<U1>::type_registered && type_index == get_type_index<MeasurableSetImpl, U1>()) {
            MeasurableSet::register_operators<T,U1>();
        }
    }

    template<class T>
    template<class U1, class U2, class... Us>
    void MeasurableSetCRTP<T>::register_one_operator(std::size_t type_index) {
        // Note that we depend on short-circuit behaviour here, see comment at the top of
        // MeasurableSetCRTP<T>::register_operators<U1>(std::size_t type_index) above.
        if (MeasurableSetCRTP<U1>::type_registered && type_index == get_type_index<MeasurableSetImpl, U1>()) {
            MeasurableSet::register_operators<T,U1>();
        } else {
            register_one_operator<U2, Us...>(type_index);
        }
    }

    template<class T>
    template<class U1>
    void MeasurableSetCRTP<T>::register_operators(void) {
        if (MeasurableSetCRTP<U1>::type_registered) MeasurableSet::register_operators<T,U1>();
    }

    template<class T>
    template<class U1, class U2, class... Us>
    void MeasurableSetCRTP<T>::register_operators(void) {
        if (MeasurableSetCRTP<U1>::type_registered) MeasurableSet::register_operators<T,U1>();
        register_operators<U2, Us...>();
    }

    inline namespace internal {

        // Used only by MeasurableSetCRTP<T>::get_registered_type_index immediately below.
        inline auto& get_registered_types_register_type(void) {
            static auto& registered_types_register_type = *new std::vector<void(*)(std::size_t)>();
            return registered_types_register_type;
        }

    }

    template<class T>
    std::size_t MeasurableSetCRTP<T>::get_registered_type_index(void) {
        assert(!type_registered);                                       // We intend this function to be called once per T.
        maybe_register_type();                                          // Register defaults.
        maybe_register_type(std::numeric_limits<std::size_t>::max());   // Register non-defaults.
        auto type_idx = MeasurableSet::register_operators<T>();
        type_registered = true;                                         // reg(type_idx) below relies on this.
        auto& registered_types_register_type = get_registered_types_register_type();
        for (auto reg : registered_types_register_type) { reg(type_idx); }
        registered_types_register_type.push_back(&maybe_register_type);
        return type_idx;
    }

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

