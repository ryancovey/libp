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

    // For unknown types, output the simplest function call that would produce the set.
    // TODO: replace unknown type T with the actual type in the output. Will need to do
    // something like this
    // https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c.
    // Look at the compile time version.
    template<
        class T,
        std::enable_if_t<
            !std::is_arithmetic<T>::value,
            bool
        > = true
    >
    inline std::ostream& operator<<(std::ostream& os, const All<T>&) {
        os << "all<unknown type>()";
        return os;
    }

    // Output the latex character \mathbb{N} for T representing the naturals.
    template<
        class T,
        std::enable_if_t<
            std::is_integral<T>::value && !std::is_signed<T>::value,
            bool
        > = true
    >
    inline std::ostream& operator<<(std::ostream& os, const All<T>&) {
        os << u8"\u2115"; // This is UTF-8 for the latex character \mathbb{N}.
        return os;
    }

    // Output the latex character \mathbb{Z} for T representing the integers.
    template<
        class T,
        std::enable_if_t<
            std::is_integral<T>::value && std::is_signed<T>::value,
            bool
        > = true
    >
    inline std::ostream& operator<<(std::ostream& os, const All<T>&) {
        os << u8"\u2124"; // This is UTF-8 for the latex character \mathbb{Z}.
        return os;
    }

    // Output the latex character \mathbb{R} for T representing the reals.
    template<
        class T,
        std::enable_if_t<
            std::is_floating_point<T>::value,
            bool
        > = true
    >
    inline std::ostream& operator<<(std::ostream& os, const All<T>&) {
        os << u8"\u211D"; // This is UTF-8 for the latex character \mathbb{Z}.
        return os;
    }


};

#endif

