#ifndef LIBP_SETS_REAL_NUMBERS_HPP_GUARD
#define LIBP_SETS_REAL_NUMBERS_HPP_GUARD

#include <ostream>
#include <libp/internal/utf8.hpp>
#include <libp/sets/measurable_set.hpp>

namespace libp {

    class RealNumbers final : public MeasurableSetCRTP<RealNumbers> {
        public:
            static void register_type(std::size_t);
    };

}

#include <libp/sets/natural_numbers.hpp>
#include <libp/sets/integers.hpp>

namespace libp {

    inline void RealNumbers::register_type(std::size_t i) {
        register_operators<NaturalNumbers, Integers>(i);
    }

    inline auto operator&&(const RealNumbers&, const NaturalNumbers& rhs) {
        return rhs;
    }

    inline auto operator||(const RealNumbers& lhs, const NaturalNumbers&) {
        return lhs;
    }

    inline auto operator&&(const RealNumbers&, const Integers& rhs) {
        return rhs;
    }

    inline auto operator||(const RealNumbers& lhs, const Integers&) {
        return lhs;
    }

    inline std::ostream& operator<<(std::ostream& os, const RealNumbers&) {
        os << utf8::blackboard_R;
        return os;
    }

    inline auto real_numbers(void) { return RealNumbers(); }

}

#endif

