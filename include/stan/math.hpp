#ifndef LIBP_STAN_MATH_HPP_GUARD
#define LIBP_STAN_MATH_HPP_GUARD

#include_next <stan/math.hpp>

namespace stan { namespace math {

std::istream& operator>>(std::istream& is, stan::math::var& v) {
    if (double d; is >> d) { v = d; }
    return is;
}

}}

#endif
