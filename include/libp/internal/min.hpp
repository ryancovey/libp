#ifndef LIBP_INTERNAL_MIN_HPP_GUARD
#define LIBP_INTERNAL_MAX_HPP_GUARD

#include <algorithm>

namespace libp { inline namespace internal {

    template<class T>
    decltype(auto) min(const T& a, const T& b) {
        return std::min(a, b);
    }

}}

#endif

