#ifndef LIBP_SETS_EMPTY_HPP_GUARD
#define LIBP_SETS_EMPTY_HPP_GUARD

#include <ostream>
#include <type_traits>

namespace libp {

    class EmptySet;
    inline EmptySet empty_set();
    inline EmptySet none();

}

#include <libp/sets/measurable_set.hpp>

namespace libp {

    class EmptySet final : public MeasurableSetCRTP<EmptySet> {
        public:
            static void register_type(void) {
                default_intersection_this();
                default_union_other();
            }
    };

    EmptySet empty_set(void) { return EmptySet(); }
    EmptySet none(void) { return EmptySet(); }
}

#include <libp/measures/counting.hpp>
#include <libp/sets/universal.hpp>

namespace libp {

    inline auto operator==(const EmptySet&, const EmptySet&) { return true; }

    template<
        class R,
        std::enable_if_t<
            std::is_base_of<MeasurableSetImpl, std::decay_t<R>>::value &&
            !std::is_same<UniversalSet, std::decay_t<R>>::value,
            bool
        > = true
    >
    auto operator==(const EmptySet&, const R& rhs) {
        return counting_measure(rhs) == 0;
    }

    template<
        class L,
        std::enable_if_t<
            std::is_base_of<MeasurableSetImpl, std::decay_t<L>>::value &&
            !std::is_same<UniversalSet, std::decay_t<L>>::value,
            bool
        > = true
    >
    auto operator==(const L& lhs, const EmptySet&) {
        return counting_measure(lhs) == 0;
    }

    template<
        class T,
        std::enable_if_t<
            std::is_base_of<MeasurableSetImpl, std::decay_t<T>>::value &&
            !std::is_same<UniversalSet, std::decay_t<T>>::value &&
            !std::is_same<EmptySet, std::decay_t<T>>::value,
            bool
        > = true
    >
    auto operator&&(const EmptySet& lhs, const T&) {
        return lhs;
    }

    template<
        class T,
        std::enable_if_t<
            std::is_base_of<MeasurableSetImpl, std::decay_t<T>>::value &&
            !std::is_same<UniversalSet, std::decay_t<T>>::value &&
            !std::is_same<EmptySet, std::decay_t<T>>::value,
            bool
        > = true
    >
    auto operator&&(const T&, const EmptySet& rhs) {
        return rhs;
    }

    template<
        class T,
        std::enable_if_t<
            std::is_base_of<MeasurableSetImpl, std::decay_t<T>>::value &&
            !std::is_same<UniversalSet, std::decay_t<T>>::value &&
            !std::is_same<EmptySet, std::decay_t<T>>::value,
            bool
        > = true
    >
    auto operator||(const EmptySet&, const T& rhs) {
        return rhs;
    }

    template<
        class T,
        std::enable_if_t<
            std::is_base_of<MeasurableSetImpl, std::decay_t<T>>::value &&
            !std::is_same<UniversalSet, std::decay_t<T>>::value &&
            !std::is_same<EmptySet, std::decay_t<T>>::value,
            bool
        > = true
    >
    auto operator||(const T& lhs, const EmptySet&) {
        return lhs;
    }

    inline std::ostream& operator<<(std::ostream& os, const EmptySet&) {
        os << utf8::emptyset;
        return os;
    }

};

#endif

