#ifndef LIBP_INTERNAL_CONSTANTS_HPP_GUARD
#define LIBP_INTERNAL_CONSTANTS_HPP_GUARD

#include <limits>
#include <type_traits>
#include <libp/internal/to.hpp>

namespace libp { inline namespace internal {

    template<class T>
    T truthy(void) {
        return to<T>(true);
    }

    template<class T>
    T falsey(void) {
        return to<T>(false);
    }

    template<class T>
    T zero(void) {
        return to<T>(0);
    }

    template<class T>
    T one(void) {
        return to<T>(1);
    }

    template<
        class T,
        std::enable_if_t<std::is_integral<T>::value, bool> = true
    >
    T infinity(void) {
        return std::numeric_limits<T>::max();
    }

    template<
        class T,
        std::enable_if_t<std::is_floating_point<T>::value, bool> = true
    >
    T infinity(void) {
        return std::numeric_limits<T>::infinity();
    }

}}

#endif

