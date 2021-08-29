#ifndef LIBP_SETS_UNIVERSAL_HPP_GUARD
#define LIBP_SETS_UNIVERSAL_HPP_GUARD

#include <ostream>
#include <type_traits>
#include <libp/sets/measurable_set.hpp>

namespace libp {

    class UniversalSet final : public MeasurableSetCRTP<UniversalSet> { };

    template<
        class T,
        std::enable_if_t<std::is_base_of<MeasurableSet, std::decay_t<T>>::value, bool> = true
    >
    auto operator&&(const UniversalSet&, const T& rhs) {
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
    auto operator&&(const T& lhs, const UniversalSet&) {
        return lhs;
    }

    template<
        class T,
        std::enable_if_t<std::is_base_of<MeasurableSet, std::decay_t<T>>::value, bool> = true
    >
    auto operator||(const UniversalSet& lhs, const T&) {
        return lhs;
    }

    template<
        class T,
        std::enable_if_t<
            std::is_base_of<MeasurableSet, std::decay_t<T>>::value &&
            !std::is_same<UniversalSet, std::decay_t<T>>::value,
            bool
        > = true
    >
    auto operator||(const T&, const UniversalSet& rhs) {
        return rhs;
    }

    inline std::ostream& operator<<(std::ostream& os, const UniversalSet&) {
        os << u8"\U0001D54C"; // This is UTF-8 for the latex character \mathbb{U}.
        return os;
    }

};

#endif

