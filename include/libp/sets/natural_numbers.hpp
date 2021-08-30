#ifndef LIBP_SETS_NATURAL_NUMBERS_HPP_GUARD
#define LIBP_SETS_NATURAL_NUMBERS_HPP_GUARD

#include <ostream>
#include <libp/sets/measurable_set_impl.hpp>

namespace libp {

    class Integers;
    class RealNumbers;

    class NaturalNumbers final : public MeasurableSetCRTP<NaturalNumbers> {
        public:
            inline NaturalNumbers operator&&(const NaturalNumbers&) const;
            inline NaturalNumbers operator||(const NaturalNumbers&) const;
            inline NaturalNumbers operator&&(const Integers&) const;
            inline Integers operator||(const Integers&) const;
            inline NaturalNumbers operator&&(const RealNumbers&) const;
            inline RealNumbers operator||(const RealNumbers& rhs) const;
    };

}

#include <libp/sets/integers.hpp>
#include <libp/sets/real_numbers.hpp>

namespace libp {

    NaturalNumbers NaturalNumbers::operator&&(const NaturalNumbers&) const {
        return *this;
    }

    NaturalNumbers NaturalNumbers::operator||(const NaturalNumbers&) const {
        return *this;
    }

    NaturalNumbers NaturalNumbers::operator&&(const Integers&) const {
        return *this;
    }

    Integers NaturalNumbers::operator||(const Integers& rhs) const {
        return rhs;
    }

    NaturalNumbers NaturalNumbers::operator&&(const RealNumbers&) const {
        return *this;
    }

    RealNumbers NaturalNumbers::operator||(const RealNumbers& rhs) const {
        return rhs;
    }

    inline std::ostream& operator<<(std::ostream& os, const NaturalNumbers&) {
        os << u8"\u2115"; // This is UTF-8 for the latex character \mathbb{N}.
        return os;
    }

}

#endif

