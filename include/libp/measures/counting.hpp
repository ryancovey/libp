#ifndef LIBP_MEASURES_COUNTING_HPP_GUARD
#define LIBP_MEASURES_COUNTING_HPP_GUARD

#include <cstddef>
#include <libp/measures/measure.hpp>
#include <libp/sets/finite.hpp>

namespace libp {

    template<class Codomain = std::size_t>
    class CountingMeasure : public Measure {
        public:
            template<class T>
            Codomain operator()(const FiniteSet<T>& A) {
                return A.size();
            }

            template<class T>
            Codomain operator()(const T& A) {
                return 0;
            }
    };

    template<class Codomain = std::size_t, class T>
    auto counting_measure(const T& A) {
        return CountingMeasure<Codomain>()(A);
    }

};

#endif

