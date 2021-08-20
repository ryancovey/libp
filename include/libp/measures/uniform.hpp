#ifndef LIBP_MEASURES_UNIFORM_HPP_GUARD
#define LIBP_MEASURES_UNIFORM_HPP_GUARD

#include <initializer_list>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <libp/measures/counting.hpp>
#include <libp/measures/lebesgue.hpp>
#include <libp/measures/measure.hpp>
#include <libp/sets/conditional.hpp>
#include <libp/sets/finite.hpp>
#include <libp/sets/measurable_set.hpp>

namespace libp {

    template<
        class SampleSpaceType,
        class RealType = double,
        typename = std::enable_if_t<std::is_base_of<MeasurableSet, std::decay_t<SampleSpaceType>>::value>
    >
    class UniformDistribution : public Measure {
        public:
            UniformDistribution(SampleSpaceType sample_space_in):
                sample_space(std::move(sample_space_in))
            { }

            template<
                class EventType,
                typename = std::enable_if_t<std::is_base_of<MeasurableSet, std::decay_t<EventType>>::value>
            >
            RealType operator()(const EventType& event) {
                auto NS = CountingMeasure(sample_space);
                if (NS == 0) {
                    return 0;
                }

                auto LS = LebesgueMeasure(sample_space);
                if (LS == 0) {
                    return static_cast<RealType>(CountingMeasure(event && sample_space))/NS;
                } else {
                    return static_cast<RealType>(LebesgueMeasure(event && sample_space))/LS;
                }
            }

            template<class T>
            auto operator()(std::initializer_list<T> il) {
                return operator()(FiniteSet<T>(std::move(il)));
            }

            template<class UnconditionalSetType, class ConditioningSetType>
            auto operator()(const ConditionalSet<UnconditionalSetType, ConditioningSetType>& conditional_event) {
                if (
                    CountingMeasure(conditional_event.conditioning_set) > 0 &&
                    LebesgueMeasure(conditional_event.conditioning_set) == 0 &&
                    LebesgueMeasure(sample_space) > 0
                ) {
                    throw std::runtime_error("Under a continuous uniform distribution, attempted to find the conditional probability of an event given a non-empty set of Lebesgue measure zero. This is yet to be implemented.");
                }

                return operator()(conditional_event.unconditional_set && conditional_event.conditioning_set)/operator()(conditional_event.conditioning_set);
            }

        private:
            SampleSpaceType sample_space;
    };

};

#endif

