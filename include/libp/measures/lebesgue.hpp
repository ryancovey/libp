#ifndef LIBP_MEASURES_LEBESGUE_HPP_GUARD
#define LIBP_MEASURES_LEBESGUE_HPP_GUARD

#include <type_traits>
#include <libp/internal/constants.hpp>
#include <libp/internal/max.hpp>
#include <libp/sets/interval.hpp>
#include <libp/measures/measure.hpp>
#include <libp/sets/measurable_set.hpp>
#include <libp/sets/real_numbers.hpp>
#include <libp/sets/universal.hpp>

namespace libp {

    template<class Codomain = double>
    class LebesgueMeasure : public Measure {
        public:
            template<
                class SetType,
                std::enable_if_t<std::is_base_of<MeasurableSetImpl, std::decay_t<SetType>>::value, bool> = true
            >
            auto operator()(const SetType&) {
                return zero<Codomain>();
            }

            template<class RealType>
            Codomain operator()(const Interval<RealType>& I) {
                return max(upper_bound - lower_bound, zero<Codomain>());
            }

            auto operator()(const UniversalSet&) {
                return infinity<Codomain>();
            }

            auto operator()(const RealNumbers&) {
                return infinity<Codomain>();
            }            
    };

    template<class Codomain = double, class SetType>
    auto lebesgue_measure(const SetType& A) {
        return LebesgueMeasure<Codomain>()(A);
    }

};

#endif

