#ifndef LIBP_SETS_ALL_HPP_GUARD
#define LIBP_SETS_ALL_HPP_GUARD

#include <type_traits>
#include <libp/sets/integers.hpp>
#include <libp/sets/measurable_set_impl.hpp>
#include <libp/sets/natural_numbers.hpp>
#include <libp/sets/null.hpp>
#include <libp/sets/real_numbers.hpp>
#include <libp/sets/universal.hpp>

namespace libp {

    template<class T>
    class All;

    template<class T, bool>
    All<T> all(void);
    
    template<class T>
    class All final : public MeasurableSetCRTP<All<T>> {
        static_assert(
            !std::is_arithmetic<T>::value,
            "Use of the class libp::All<T> is forbidden for T a built-in arithmetic type (int, float, unsigned, double, long, etc.). "
            "Use the classes libp::NaturalNumbers, libp::Integers and libp::RealNumbers instead. "
            "Calling the function libp::all<T>() will return an object of type libp::NaturalNumbers for built-in unsigned integer T, "
            "libp::Integers for built-in signed integer T, and libp::RealNumbers for built-in floating-point T."
        );

        static_assert(
            !std::is_void<T>::value,
            "Use of the class libp:All<void> is forbidden. Use libp::NullSet instead."
        );
    };

    inline auto all(void) {
        return UniversalSet();
    }

    template<
        class T,
        std::enable_if_t<
            !std::is_arithmetic<T>::value &&
            !std::is_void<T>::value,
            bool
        > = true
    >
    auto all(void) {
        return All<T>();
    }

    template<
        class T,
        std::enable_if_t<std::is_void<T>::value, bool> = true
    >
    auto all(void) {
        return NullSet();
    }

    template<
        class T,
        std::enable_if_t<std::is_unsigned<T>::value, bool> = true
    >
    auto all(void) {
        return NaturalNumbers();
    }

    template<
        class T,
        std::enable_if_t<
            std::is_integral<T>::value &&
            std::is_signed<T>::value,
            bool
        > = true
    >
    auto all(void) {
        return Integers();
    }

    template<
        class T,
        std::enable_if_t<std::is_floating_point<T>::value, bool> = true
    >
    auto all(void) {
        return RealNumbers();
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
    std::ostream& operator<<(std::ostream& os, const All<T>&) {
        os << "all<unknown type>()";
        return os;
    }

};

#endif

