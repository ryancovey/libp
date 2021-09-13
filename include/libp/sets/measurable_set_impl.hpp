#ifndef LIBP_SETS_MEASURABLE_SET_IMPL_HPP_GUARD
#define LIBP_SETS_MEASURABLE_SET_IMPL_HPP_GUARD

#include <type_traits>

namespace libp {

    class MeasurableSetImpl { };

    template<class T>
    class MeasurableSetCRTP : public MeasurableSetImpl { };

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

