#ifndef LIBP_INTERNAL_WHERE_HPP_GUARD
#define LIBP_INTERNAL_WHERE_HPP_GUARD

namespace libp { inline namespace internal {

    template<class T>
    auto where(bool condition, const T& value_if_true, const T& value_if_false) {
        return condition ? value_if_true : value_if_false;
    }

}}

#endif 
