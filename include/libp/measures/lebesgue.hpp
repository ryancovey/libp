#ifndef LIBP_MEASURES_LEBESGUE_HPP_GUARD
#define LIBP_MEASURES_LEBESGUE_HPP_GUARD

#include <libp/measures/measure.hpp>
#include <libp/sets/measurable_set.hpp>

namespace libp {

    template<class Codomain = double>
    class LebesgueMeasure : public Measure {
        public:
            template<
                class SetType,
                typename = std::enable_if_t<std::is_base_of<MeasurableSet, std::decay_t<SetType>>::value>
            >
            Codomain operator()(const SetType& A) {
                return 0;
            }
    };

    template<class Codomain = double, class SetType>
    auto lebesgue_measure(const SetType& A) {
        return LebesgueMeasure<Codomain>()(A);
    }

};

#endif

