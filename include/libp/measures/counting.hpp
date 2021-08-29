#ifndef LIBP_MEASURES_COUNTING_HPP_GUARD
#define LIBP_MEASURES_COUNTING_HPP_GUARD

#include <cstddef>
#include <limits>
#include <libp/internal/constants.hpp>
#include <libp/measures/measure.hpp>
#include <libp/sets/finite.hpp>
#include <libp/sets/integers.hpp>
#include <libp/sets/natural_numbers.hpp>
#include <libp/sets/real_numbers.hpp>
#include <libp/sets/universal.hpp>

namespace libp {

    template<class Codomain = std::size_t>
    class CountingMeasure : public Measure {
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

