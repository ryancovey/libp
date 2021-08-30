#ifndef LIBP_INTERNAL_ID_HPP_GUARD
#define LIBP_INTERNAL_ID_HPP_GUARD

#include <cstddef>

namespace libp { inline namespace internal {

    template<class Base, class IndexType = std::size_t>
    auto reserve_thread_local_index(void) {
        thread_local IndexType next_index = 0;
        return next_index++;
    }

    template<class Base, class Derived, class IndexType = std::size_t>
    auto thread_local_index(void) {
        thread_local IndexType index_derived = reserve_index<Base>();
        return index_derived;
    }

}}

#endif

