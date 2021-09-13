#ifndef LIBP_INTERNAL_ID_HPP_GUARD
#define LIBP_INTERNAL_ID_HPP_GUARD

#include <cstddef>

namespace libp { inline namespace internal {

    template<class Base, class IndexType = std::size_t>
    auto reserve_type_index(void) {
        static IndexType next_index = 0;
        return next_index++;
    }

    template<class Base, class Derived, class IndexType = std::size_t>
    auto get_type_index(void) {
        static IndexType index_derived = reserve_type_index<Base, IndexType>();
        return index_derived;
    }

}}

#endif

