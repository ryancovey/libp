#ifndef LIBP_SETS_NATURAL_NUMBERS_HPP_GUARD
#define LIBP_SETS_NATURAL_NUMBERS_HPP_GUARD

#include <ostream>
#include <libp/sets/measurable_set.hpp>

namespace libp {

    class NaturalNumbers final : public MeasurableSetCRTP<NaturalNumbers> {
        public:
            static void register_type(void);
    };

}

#include <libp/sets/integers.hpp>
#include <libp/sets/real_numbers.hpp>

namespace libp {

    void NaturalNumbers::register_type(void) {
        register_operators<Integers, RealNumbers>();
    }


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
        os << u8"\u2115"; // This is UTF-8 for the latex character \mathbb{N}.
        return os;
    }

    auto natural_numbers(void) { return NaturalNumbers(); }

}

#endif

