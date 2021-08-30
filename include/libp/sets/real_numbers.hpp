#ifndef LIBP_SETS_REAL_NUMBERS_HPP_GUARD
#define LIBP_SETS_REAL_NUMBERS_HPP_GUARD

#include <ostream>
#include <libp/sets/measurable_set_impl.hpp>

namespace libp {

    class NaturalNumbers;
    class Integers;

    class RealNumbers final : public MeasurableSetCRTP<RealNumbers> {
        public:
            inline NaturalNumbers operator&&(const NaturalNumbers&) const;
            inline RealNumbers operator||(const NaturalNumbers&) const;
            inline Integers operator&&(const Integers&) const;
            inline RealNumbers operator||(const Integers&) const;
            inline RealNumbers operator&&(const RealNumbers&) const;
            inline RealNumbers operator||(const RealNumbers&) const;
    };

}

#include <libp/sets/natural_numbers.hpp>
#include <libp/sets/integers.hpp>

namespace libp {

    NaturalNumbers RealNumbers::operator&&(const NaturalNumbers& rhs) const {
        return rhs;
    }

    RealNumbers RealNumbers::operator||(const NaturalNumbers&) const {
        return *this;
    }

    Integers RealNumbers::operator&&(const Integers& rhs) const {
        return rhs;
    }

    RealNumbers RealNumbers::operator||(const Integers&) const {
        return *this;
    }

    RealNumbers RealNumbers::operator&&(const RealNumbers&) const {
        return *this;
    }

    RealNumbers RealNumbers::operator||(const RealNumbers&) const {
        return *this;
    }

    std::ostream& operator<<(std::ostream& os, const RealNumbers&) {
        os << u8"\u211D"; // This is UTF-8 for the latex character \mathbb{R}.
        return os;
    }

}

#endif

