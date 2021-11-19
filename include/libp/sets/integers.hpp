#ifndef LIBP_SETS_INTEGERS_HPP_GUARD
#define LIBP_SETS_INTEGERS_HPP_GUARD

#include <ostream>
#include <type_traits>
#include <libp/internal/utf8.hpp>
#include <libp/sets/measurable_set.hpp>

namespace libp {

    class Integers final : public MeasurableSetCRTP<Integers> {
        public:
            static void register_type(std::size_t);
    };

    auto integers(void) { return Integers(); }

}

#include <libp/sets/natural_numbers.hpp>
#include <libp/sets/real_numbers.hpp>

namespace libp {

    void Integers::register_type(std::size_t i) {
        register_operators<NaturalNumbers>(i);
    }

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
        os << utf8::blackboard_Z;
        return os;
    }

}

#endif

