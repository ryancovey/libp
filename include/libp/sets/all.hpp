#ifndef LIBP_SETS_ALL_HPP_GUARD
#define LIBP_SETS_ALL_HPP_GUARD

#include <libp/sets/universal.hpp>

namespace libp {
    
    template<class T>
    class All : public MeasurableSetCRTP<All<T>> { };

    inline UniversalSet all(void) {
        return UniversalSet();
    }

    template<class T>
    auto all(void) {
        return All<T>();
    }

};

#endif

