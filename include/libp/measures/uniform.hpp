#ifndef LIBP_MEASURES_UNIFORM_HPP_GUARD
#define LIBP_MEASURES_UNIFORM_HPP_GUARD

#include <initializer_list>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <libp/internal/constants.hpp>
#include <libp/measures/counting.hpp>
#include <libp/measures/lebesgue.hpp>
#include <libp/measures/measure.hpp>
#include <libp/sets/conditional.hpp>
#include <libp/sets/finite.hpp>
#include <libp/sets/measurable_set.hpp>

namespace libp {

    template<
        class SampleSpaceType,
        class Codomain = double,
        typename = std::enable_if_t<std::is_base_of<MeasurableSet, std::decay_t<SampleSpaceType>>::value>
    >
    class UniformDistribution : public Measure {
        public:
            explicit UniformDistribution(SampleSpaceType sample_space_in):
                sample_space(std::move(sample_space_in))
            { }

            template<
                class EventType,
                typename = std::enable_if_t<std::is_base_of<MeasurableSet, std::decay_t<EventType>>::value>
            >
            Codomain operator()(const EventType& event) {
                auto NS = counting_measure(sample_space);
                if (NS == zero) {
                    return zero;
                }

                auto LS = lebesgue_measure(sample_space);
                if (LS == zero) {
                    counting_measure(event && sample_space)/NS;
                } else {
                    lebesgue_measure(event && sample_space)/LS;
                }
            }

            template<class T>
            auto operator()(std::initializer_list<T> il) {
                return operator()(FiniteSet<T>(std::move(il)));
            }

            template<class UnconditionalSetType, class ConditioningSetType>
            auto operator()(const ConditionalSet<UnconditionalSetType, ConditioningSetType>& conditional_event) {
                if (
                    counting_measure(conditional_event.conditioning_set) > zero &&
                    lebesgue_measure(conditional_event.conditioning_set) == zero &&
                    lebesgue_measure(sample_space) > zero
                ) {
                    throw std::runtime_error("Under a continuous uniform distribution, attempted to find the conditional probability of an event given a non-empty set of Lebesgue measure zero. This is yet to be implemented.");
                }

                return operator()(conditional_event.unconditional_set && conditional_event.conditioning_set)/operator()(conditional_event.conditioning_set);
            }

        private:
            SampleSpaceType sample_space;
            CountingMeasure<Codomain> counting_measure;
            LebesgueMeasure<Codomain> lebesgue_measure;
            Codomain zero = internal::zero<Codomain>();
    };

    template<class RealType = double, class SampleSpaceType>
    auto uniform_distribution(SampleSpaceType sample_space) {
        return UniformDistribution<SampleSpaceType, RealType>(std::move(sample_space));
    }

    template<class RealType = double, class T>
    auto uniform_distribution(std::initializer_list<T> il) {
        return uniform_distribution(FiniteSet<T>(std::move(il)));
    }

};

#endif

