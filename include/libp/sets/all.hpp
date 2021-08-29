#ifndef LIBP_SETS_ALL_HPP_GUARD
#define LIBP_SETS_ALL_HPP_GUARD

#include <libp/sets/universal.hpp>

namespace libp {
    
    template<class T>
    class All final : public MeasurableSetCRTP<All<T>> {
        public:
            // Set intersection for arithmetic sets where lhs is a subset of or equal to rhs.
            template<
                class RT,
                std::enable_if_t<
                    (std::is_unsigned<T>::value && std::is_arithmetic<RT>::value) ||        // lhs is the naturals.
                    (
                        std::is_integral<T>::value && std::is_signed<T>::value &&           // lhs and rhs are the integers.
                        std::is_integral<RT>::value && std::is_signed<RT>::value
                    ) ||
                    (std::is_arithmetic<T>::value && std::is_floating_point<RT>::value),    // rhs is the reals.
                    bool
                > = true
            >
            auto operator&&(const All<RT>&) {
                return *this;
            }

            // Set intersection for arithmetic sets where rhs is a subset of lhs.
            template<
                class RT,
                std::enable_if_t<
                    (std::is_floating_point<T>::value && std::is_integral<RT>::value) ||    // lhs is the reals, rhs is naturals or integers.
                    (
                        std::is_integral<T>::value && std::is_signed<T>::value &&           // lhs are the integers, rhs are the naturals.
                        std::is_integral<RT>::value && std::is_unsigned<RT>::value
                    ),
                    bool
                > = true
            >
            auto operator&&(const All<RT>& rhs) {
                return rhs;
            }

            // Set union for arithmetic sets where lhs is a superset of or equal to rhs.
            template<
                class RT,
                std::enable_if_t<
                    (std::is_floating_point<T>::value && std::is_arithmetic<RT>::value) ||  // lhs is the reals.
                    (
                        std::is_integral<T>::value && std::is_signed<T>::value &&           // lhs and rhs are the integers.
                        std::is_integral<RT>::value && std::is_signed<T>::value
                    ) ||
                    (std::is_arithmetic<T>::value && std::is_unsigned<RT>::value),          // rhs is the naturals.
                    bool
                > = true
            >
            auto operator||(const All<RT>&) {
                return *this;
            }

            // Set union for arithmetic sets where rhs is a superset of lhs.
            template<
                class RT,
                std::enable_if_t<
                    (std::is_integral<T>::value && std::is_floating_point<RT>::value) ||    // lhs is the naturals or integers, rhs is the reals.
                    (
                        std::is_integral<T>::value && std::is_unsigned<T>::value &&         // lhs are the naturals, rhs are the integers.
                        std::is_integral<RT>::value && std::is_signed<RT>::value
                    ),
                    bool
                > = true
            >
            auto operator||(const All<RT>& rhs) {
                return rhs;
            }
    };

    inline UniversalSet all(void) {
        return UniversalSet();
    }

    template<class T>
    auto all(void) {
        return All<T>();
    }

    template<
        class T,
        std::enable_if_t<
            std::is_base_of<MeasurableSet, std::decay_t<T>>::value &&
            !std::is_same<UniversalSet, std::decay_t<T>>::value,
            bool
        > = true
    >
    auto operator&&(const All<void>& lhs, const T&) {
        return lhs;
    }

    template<
        class T,
        std::enable_if_t<
            std::is_base_of<MeasurableSet, std::decay_t<T>>::value &&
            !std::is_same<UniversalSet, std::decay_t<T>>::value &&
            !std::is_same<All<void>, std::decay_t<T>>::value,
            bool
        > = true
    >
    auto operator&&(const T&, const All<void>& rhs) {
        return rhs;
    }

    template<
        class T,
        std::enable_if_t<
            std::is_base_of<MeasurableSet, std::decay_t<T>>::value &&
            !std::is_same<UniversalSet, std::decay_t<T>>::value,
            bool
        > = true
    >
    auto operator||(const All<void>&, const T& rhs) {
        return rhs;
    }

    template<
        class T,
        std::enable_if_t<
            std::is_base_of<MeasurableSet, std::decay_t<T>>::value &&
            !std::is_same<UniversalSet, std::decay_t<T>>::value &&
            !std::is_same<All<void>, std::decay_t<T>>::value,
            bool
        > = true
    >
    auto operator||(const T& lhs, const All<void>&) {
        return lhs;
    }

    // For unknown types, output the simplest function call that would produce the set.
    // TODO: replace unknown type T with the actual type in the output. Will need to do
    // something like this
    // https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c.
    // Look at the compile time version.
    template<
        class T,
        std::enable_if_t<
            !std::is_arithmetic<T>::value,
            bool
        > = true
    >
    std::ostream& operator<<(std::ostream& os, const All<T>&) {
        os << "all<unknown type>()";
        return os;
    }

    // Output the latex character \varnothing for T void, so All<void> represents
    // the null set.
    inline std::ostream& operator<<(std::ostream& os, const All<void>&) {
        os << u8"\u2205"; // This is UTF-8 for the latex character \varnothing.
        return os;
    }

    // Output the latex character \mathbb{N} for T representing the naturals.
    template<
        class T,
        std::enable_if_t<
            std::is_integral<T>::value && std::is_unsigned<T>::value,
            bool
        > = true
    >
    inline std::ostream& operator<<(std::ostream& os, const All<T>&) {
        os << u8"\u2115"; // This is UTF-8 for the latex character \mathbb{N}.
        return os;
    }

    // Output the latex character \mathbb{Z} for T representing the integers.
    template<
        class T,
        std::enable_if_t<
            std::is_integral<T>::value && std::is_signed<T>::value,
            bool
        > = true
    >
    inline std::ostream& operator<<(std::ostream& os, const All<T>&) {
        os << u8"\u2124"; // This is UTF-8 for the latex character \mathbb{Z}.
        return os;
    }

    // Output the latex character \mathbb{R} for T representing the reals.
    template<
        class T,
        std::enable_if_t<
            std::is_floating_point<T>::value,
            bool
        > = true
    >
    inline std::ostream& operator<<(std::ostream& os, const All<T>&) {
        os << u8"\u211D"; // This is UTF-8 for the latex character \mathbb{Z}.
        return os;
    }


};

#endif

