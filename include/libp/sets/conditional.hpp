#ifndef LIBP_SETS_CONDITIONAL_HPP_GUARD
#define LIBP_SETS_CONDITIONAL_HPP_GUARD

#include <type_traits>
#include <utility>
#include <libp/sets/measurable_set.hpp>

namespace libp {

    template<
        class UnconditionalSetType,
        class ConditioningSetType,
        typename = std::enable_if_t<
            std::is_base_of<MeasurableSet, std::decay_t<UnconditionalSetType>>::value &&
            std::is_base_of<MeasurableSet, std::decay_t<ConditioningSetType>>::value
        >
    >
    struct ConditionalSet {
        UnconditionalSetType unconditional_set;
        ConditioningSetType conditioning_set;
    };

    template<class T1, class T2>
    auto conditional_set(const T1& unconditional_set, const T2& conditioning_set) {
        return ConditionalSet<const T1&, const T2&>{unconditional_set, conditioning_set};
    }

    template<class T1, class T2>
    auto conditional_set(const T1& unconditional_set, T2&& conditioning_set) {
        return ConditionalSet<const T1&, const T2>{unconditional_set, std::move(conditioning_set)};
    }

    template<class T1, class T2>
    auto conditional_set(T1&& unconditional_set, const T2& conditioning_set) {
        return ConditionalSet<const T1, const T2&>{std::move(unconditional_set), conditioning_set};
    }

    template<class T1, class T2>
    auto conditional_set(T1&& unconditional_set, T2&& conditioning_set) {
        return ConditionalSet<const T1, const T2>{std::move(unconditional_set), std::move(conditioning_set)};
    }

};

#endif

