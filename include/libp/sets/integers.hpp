#ifndef LIBP_SETS_INTEGERS_HPP_GUARD
#define LIBP_SETS_INTEGERS_HPP_GUARD

#include <ostream>
#include <type_traits>
#include <libp/sets/measurable_set_impl.hpp>

namespace libp {

    class Integers final : public MeasurableSetCRTP<Integers> { };

}

#include <libp/sets/natural_numbers.hpp>
#include <libp/sets/real_numbers.hpp>

namespace libp {

    inline auto operator&&(const Integers&, const NaturalNumbers& rhs) {
        return rhs;
    }

    inline auto operator||(const Integers& lhs, const NaturalNumbers&) {
        return lhs;
    }

    inline auto operator&&(const Integers& lhs, const RealNumbers&) {
        return lhs;
    }

    inline auto operator||(const Integers&, const RealNumbers& rhs) {
        return rhs;
    }

    inline std::ostream& operator<<(std::ostream& os, const Integers&) {
        os << u8"\u2124"; // This is UTF-8 for the latex character \mathbb{Z}.
        return os;
    }

}

#endif

