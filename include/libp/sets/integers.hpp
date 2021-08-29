#ifndef LIBP_SETS_INTEGERS_HPP_GUARD
#define LIBP_SETS_INTEGERS_HPP_GUARD

#include <ostream>
#include <libp/sets/measurable_set.hpp>

namespace libp {

    class NaturalNumbers;
    class RealNumbers;

    class Integers final : public MeasurableSetCRTP<Integers> {
        public:
            inline NaturalNumbers operator&&(const NaturalNumbers&) const;
            inline Integers operator||(const NaturalNumbers&) const;
            inline Integers operator&&(const Integers&) const;
            inline Integers operator||(const Integers&) const;
            inline Integers operator&&(const RealNumbers&) const;
            inline RealNumbers operator||(const RealNumbers&) const;
    };

}

#include <libp/sets/natural_numbers.hpp>
#include <libp/sets/real_numbers.hpp>

namespace libp {

    NaturalNumbers Integers::operator&&(const NaturalNumbers& rhs) const {
        return rhs;
    }

    Integers Integers::operator||(const NaturalNumbers&) const {
        return *this;
    }

    Integers Integers::operator&&(const Integers&) const {
        return *this;
    }

    Integers Integers::operator||(const Integers&) const {
        return *this;
    }

    Integers Integers::operator&&(const RealNumbers&) const {
        return *this;
    }

    RealNumbers Integers::operator||(const RealNumbers& rhs) const {
        return rhs;
    }

    std::ostream& operator<<(std::ostream& os, const Integers&) {
        os << u8"\u2124"; // This is UTF-8 for the latex character \mathbb{Z}.
        return os;
    }

}

#endif

