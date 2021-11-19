#ifndef LIBP_MEASURES_COUNTING_HPP_GUARD
#define LIBP_MEASURES_COUNTING_HPP_GUARD

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <libp/internal/constants.hpp>
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

            template<class RealType>
            Codomain operator()(const Interval<RealType>& I) {
                if (I.real()) {
                    if (I.lower_bound < I.upper_bound) {
                        return infinity<Codomain>();
                    } else if (I.lower_bound == I.upper_bound && I.closed() && I.lower_bound != -infinity<RealType>()) {
                        return one<Codomain>();
                    } else {
                        return zero<Codomain>();
                    }
                } else if (I.integer()) {
                    auto ceil_lower_bound = std::llround(std::ceil(I.open_lower_bound ? std::nextafter(I.lower_bound, I.lower_bound + one<RealType>()) : I.lower_bound));
                    auto floor_upper_bound = std::llround(std::floor(I.open_upper_bound() ? std::nextafter(I.upper_bound, I.upper_bound - one<RealType>()) : I.upper_bound));
                    return  to<Codomain>(std::max(floor_upper_bound - ceil_lower_bound + 1, 0));
                } else if (I.integer_complement()) {
                    if (I.lower_bound < I.upper_bound) {
                        return infinity<Codomain>();
                    } else if (I.lower_bound == I.upper_bound && I.closed() && !I.integer_lower_bound()) {
                        return one<Codomain>();
                    } else {
                        return zero<Codomain>();
                    }
                } else {
                    return zero<Codomain>();
                }
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

