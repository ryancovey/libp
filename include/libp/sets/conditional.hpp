#ifndef LIBP_SETS_CONDITIONAL_HPP_GUARD
#define LIBP_SETS_CONDITIONAL_HPP_GUARD

#include <utility>

namespace libp {

    template<class UnconditionalSetType, class ConditioningSetType>
    struct ConditionalSet {
        UnconditionalSetType unconditional_set;
        ConditioningSetType conditioning_set;
    };

    template<class T1, class T2>
    auto conditional_set(T1 unconditional_set, T2 conditioning_set) {
        return ConditionalSet<T1, T2>{std::move(unconditional_set), std::move(conditioning_set)};
    }

};

#endif

