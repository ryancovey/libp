#ifndef LIBP_SETS_UNIVERSAL_HPP_GUARD
#define LIBP_SETS_UNIVERSAL_HPP_GUARD

#include <ostream>
#include <type_traits>
#include <libp/internal/utf8.hpp>
#include <libp/sets/measurable_set.hpp>

namespace libp {

    class UniversalSet final : public MeasurableSetCRTP<UniversalSet> {
        public:
            static void register_type(void) {
                default_intersection_other();
                default_union_this();
            }
    };

    template<
        class T,
        std::enable_if_t<
            std::is_base_of<MeasurableSetImpl, std::decay_t<T>>::value &&
            !std::is_same<UniversalSet, std::decay_t<T>>::value,
            bool
        > = true
    >
    auto operator&&(const UniversalSet&, const T& rhs) {
        return rhs;
    }

    template<
        class T,
        std::enable_if_t<
            std::is_base_of<MeasurableSetImpl, std::decay_t<T>>::value &&
            !std::is_same<UniversalSet, std::decay_t<T>>::value,
            bool
        > = true
    >
    auto operator&&(const T& lhs, const UniversalSet&) {
        return lhs;
    }

    template<
        class T,
        std::enable_if_t<
            std::is_base_of<MeasurableSetImpl, std::decay_t<T>>::value &&
            !std::is_same<UniversalSet, std::decay_t<T>>::value,
            bool
        > = true
    >
    auto operator||(const UniversalSet& lhs, const T&) {
        return lhs;
    }

    template<
        class T,
        std::enable_if_t<
            std::is_base_of<MeasurableSetImpl, std::decay_t<T>>::value &&
            !std::is_same<UniversalSet, std::decay_t<T>>::value,
            bool
        > = true
    >
    auto operator||(const T&, const UniversalSet& rhs) {
        return rhs;
    }

    inline std::ostream& operator<<(std::ostream& os, const UniversalSet&) {
        os << utf8::blackboard_U;
        return os;
    }

};

#endif

