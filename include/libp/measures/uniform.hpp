#ifndef LIBP_MEASURES_UNIFORM_HPP_GUARD
#define LIBP_MEASURES_UNIFORM_HPP_GUARD

#include <initializer_list>
#include <type_traits>
#include <utility>
#include <libp/measures/counting.hpp>
#include <libp/measures/lebesgue.hpp>

namespace libp {

    template<
        class SampleSpaceType,
        class RealType = double
    >
    class UniformDistribution {
        public:
            UniformDistribution(SampleSpaceType sample_space_in):
                sample_space(std::move(sample_space_in))
            { }

            template<class EventType>
            RealType operator()(const EventType& event) {
                auto NS = CountingMeasure(sample_space);
                if (NS == 0) {
                    return 0;
                } else {
                    auto LS = LebesgueMeasure(sample_space);
                    if (LS == 0) {
                        return static_cast<RealType>(CountingMeasure(event && sample_space))/NS;
                    } else {
                        return static_cast<RealType>(LebesgueMeasure(event && sample_space))/LS;
                    }
                }
            }

            template<class T>
            auto operator()(std::initializer_list<T> il) {
                return operator()(FiniteSet<T>(std::move(il)));
            }

        private:
            SampleSpaceType sample_space;
    };

};

#endif

