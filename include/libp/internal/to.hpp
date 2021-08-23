#ifndef LIBP_INTERNAL_TO_HPP_GUARD
#define LIBP_INTERNAL_TO_HPP_GUARD

namespace libp { inline namespace internal {

    template<class T, class F>
    T to(const F& from) {
        return static_cast<T>(from);
    }

}}

#endif

