#ifndef LIBP_MEASURES_COUNTING_HPP_GUARD
#define LIBP_MEASURES_COUNTING_HPP_GUARD

template<class Codomain = std::size_t, class T>
auto counting_measure(const T& A);

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <libp/internal/constants.hpp>
#include <libp/internal/where.hpp>
#include <libp/measures/measure.hpp>
#include <libp/sets/finite.hpp>
#include <libp/sets/integers.hpp>
#include <libp/sets/interval.hpp>
#include <libp/sets/natural_numbers.hpp>
#include <libp/sets/real_numbers.hpp>
#include <libp/sets/universal.hpp>

namespace libp {

    template<class Codomain = std::size_t>
    class CountingMeasure : public Measure {
        public:
            template<
                class SetType,
                std::enable_if_t<std::is_base_of<MeasurableSetImpl, std::decay_t<SetType>>::value, bool> = true
            >
            CountingMeasure operator()(const SetType&) {
                return zero<Codomain>();
            }

            template<class RealType, class BooleanType>
            Codomain operator()(const Interval<RealType, BooleanType>& I) {
                return where(
                    I.lower_bound < I.upper_bound,
                    infinity<Codomain>(),
                    where(
                        I.lower_bound == I.upper_bound && I.closed() && I.lower_bound != -infinity<RealType>(),
                        one<Codomain>(),
                        zero<Codomain>()
                    )
                );
            }

            auto operator()(const UniversalSet&) {
                return infinity<Codomain>();
            }

            auto operator()(const NaturalNumbers&) {
                return infinity<Codomain>();
            }

            auto operator()(const Integers&) {
                return infinity<Codomain>();
            }

            auto operator()(const RealNumbers&) {
                return infinity<Codomain>();
            }

            template<class T, template<class, class...> class C>
            Codomain operator()(const FiniteSet<T,C>& A) {
                return A.size();
            }
    };

    template<class Codomain = std::size_t, class T>
    auto counting_measure(const T& A) {
        return CountingMeasure<Codomain>()(A);
    }

};

#endif

