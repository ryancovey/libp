#ifndef LIBP_MEASURES_COUNTING_HPP_GUARD
#define LIBP_MEASURES_COUNTING_HPP_GUARD

#include <libp/sets/finite.hpp>

namespace libp {

    template<class T>
    auto CountingMeasure(const FiniteSet<T>& A) {
        return A.size();
    }

};

#endif

