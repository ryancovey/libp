#ifndef LIBP_SETS_NULL_HPP_GUARD
#define LIBP_SETS_NULL_HPP_GUARD

#include <ostream>
#include <type_traits>
#include <libp/sets/measurable_set.hpp>
#include <libp/sets/universal.hpp>

namespace libp {

    class NullSet final : public MeasurableSetCRTP<NullSet> { };

    template<
        class T,
        std::enable_if_t<
            std::is_base_of<MeasurableSet, std::decay_t<T>>::value &&
            !std::is_same<UniversalSet, std::decay_t<T>>::value,
            bool
        > = true
    >
    auto operator&&(const NullSet& lhs, const T&) {
        return lhs;
    }

    template<
        class T,
        std::enable_if_t<
            std::is_base_of<MeasurableSet, std::decay_t<T>>::value &&
            !std::is_same<UniversalSet, std::decay_t<T>>::value &&
            !std::is_same<NullSet, std::decay_t<T>>::value,
            bool
        > = true
    >
    auto operator&&(const T&, const NullSet& rhs) {
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
    auto operator||(const NullSet&, const T& rhs) {
        return rhs;
    }

    template<
        class T,
        std::enable_if_t<
            std::is_base_of<MeasurableSet, std::decay_t<T>>::value &&
            !std::is_same<UniversalSet, std::decay_t<T>>::value &&
            !std::is_same<NullSet, std::decay_t<T>>::value,
            bool
        > = true
    >
    auto operator||(const T& lhs, const NullSet&) {
        return lhs;
    }

    inline std::ostream& operator<<(std::ostream& os, const NullSet&) {
        os << u8"\u2205"; // This is UTF-8 for the latex character \varnothing.
        return os;
    }

};

#endif

