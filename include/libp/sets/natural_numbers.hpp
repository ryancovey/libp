#ifndef LIBP_SETS_NATURAL_NUMBERS_HPP_GUARD
#define LIBP_SETS_NATURAL_NUMBERS_HPP_GUARD

#include <ostream>
#include <libp/internal/utf8.hpp>
#include <libp/sets/measurable_set.hpp>

namespace libp {

    class NaturalNumbers final : public MeasurableSetCRTP<NaturalNumbers> { };

}

#include <libp/sets/integers.hpp>
#include <libp/sets/real_numbers.hpp>

namespace libp {


    inline auto operator&&(const NaturalNumbers& lhs, const Integers&) {
        return lhs;
    }

    inline auto operator||(const NaturalNumbers&, const Integers& rhs) {
        return rhs;
    }

    inline auto operator&&(const NaturalNumbers& lhs, const RealNumbers&) {
        return lhs;
    }

    inline auto operator||(const NaturalNumbers&, const RealNumbers& rhs) {
        return rhs;
    }

    inline std::ostream& operator<<(std::ostream& os, const NaturalNumbers&) {
        os << utf8::blackboard_N;
        return os;
    }

    auto natural_numbers(void) { return NaturalNumbers(); }

}

#endif

