#ifndef LIBP_SETS_NULL_HPP_GUARD
#define LIBP_SETS_NULL_HPP_GUARD

#include <ostream>
#include <type_traits>

namespace libp {

    class NullSet;
    inline NullSet none(void);
    inline NullSet null(void);

}

#include <libp/sets/measurable_set.hpp>

namespace libp {

    class NullSet final : public MeasurableSetCRTP<NullSet> {
        public:
            static void register_type(void) {
                default_intersection_this();
                default_union_other();
            }
    };

    NullSet none(void) { return NullSet(); }
    NullSet null(void) { return NullSet(); }
}

#include <libp/measures/counting.hpp>
#include <libp/sets/universal.hpp>

namespace libp {

    inline auto operator==(const NullSet&, const NullSet&) { return true; }

    template<
        class R,
        std::enable_if_t<
            std::is_base_of<MeasurableSetImpl, std::decay_t<R>>::value &&
            !std::is_same<UniversalSet, std::decay_t<R>>::value,
            bool
        > = true
    >
    auto operator==(const NullSet&, const R& rhs) {
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
    auto operator==(const L& lhs, const NullSet&) {
        return counting_measure(lhs) == 0;
    }

    template<
        class T,
        std::enable_if_t<
            std::is_base_of<MeasurableSetImpl, std::decay_t<T>>::value &&
            !std::is_same<UniversalSet, std::decay_t<T>>::value &&
            !std::is_same<NullSet, std::decay_t<T>>::value,
            bool
        > = true
    >
    auto operator&&(const NullSet& lhs, const T&) {
        return lhs;
    }

    template<
        class T,
        std::enable_if_t<
            std::is_base_of<MeasurableSetImpl, std::decay_t<T>>::value &&
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
            std::is_base_of<MeasurableSetImpl, std::decay_t<T>>::value &&
            !std::is_same<UniversalSet, std::decay_t<T>>::value &&
            !std::is_same<NullSet, std::decay_t<T>>::value,
            bool
        > = true
    >
    auto operator||(const NullSet&, const T& rhs) {
        return rhs;
    }

    template<
        class T,
        std::enable_if_t<
            std::is_base_of<MeasurableSetImpl, std::decay_t<T>>::value &&
            !std::is_same<UniversalSet, std::decay_t<T>>::value &&
            !std::is_same<NullSet, std::decay_t<T>>::value,
            bool
        > = true
    >
    auto operator||(const T& lhs, const NullSet&) {
        return lhs;
    }

    inline std::ostream& operator<<(std::ostream& os, const NullSet&) {
        os << utf8::emptyset;
        return os;
    }

};

#endif

