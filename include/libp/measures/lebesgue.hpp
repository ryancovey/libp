#ifndef LIBP_MEASURES_LEBESGUE_HPP_GUARD
#define LIBP_MEASURES_LEBESGUE_HPP_GUARD

#include <type_traits>
#include <libp/internal/constants.hpp>
#include <libp/measures/measure.hpp>
#include <libp/sets/all.hpp>
#include <libp/sets/measurable_set.hpp>
#include <libp/sets/universal.hpp>

namespace libp {

    template<class Codomain = double>
    class LebesgueMeasure : public Measure {
        public:
            template<
                class SetType,
                std::enable_if_t<std::is_base_of<MeasurableSet, std::decay_t<SetType>>::value, bool> = true
            >
            auto operator()(const SetType&) {
                return zero<Codomain>();
            }

            auto operator()(const UniversalSet&) {
                return infinity<Codomain>();
            }
            
            template<
                class T,
                std::enable_if_t<std::is_floating_point<T>::value, bool> = true
            >
            auto operator()(const All<T>&) {
                return infinity<Codomain>();
            }

            auto operator()(const All<void>&) {
                return zero<Codomain>();
            }

            template<class T>
            Codomain operator()(const FiniteSet<T>&) {
                return zero<Codomain>();
            }
    };

    template<class Codomain = double, class SetType>
    auto lebesgue_measure(const SetType& A) {
        return LebesgueMeasure<Codomain>()(A);
    }

};

#endif

