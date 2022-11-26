#ifndef LIBP_INTERNAL_MAX_HPP_GUARD
#define LIBP_INTERNAL_MAX_HPP_GUARD

#include <algorithm>

namespace libp { inline namespace internal {
    
    template<class T>
    decltype(auto) max(const T& a, const T& b) {
        return std::max(a, b);
    }

}}

#endif
