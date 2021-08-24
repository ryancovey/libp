#ifndef LIBP_MEASURES_COUNTING_HPP_GUARD
#define LIBP_MEASURES_COUNTING_HPP_GUARD

#include <cstddef>
#include <limits>
#include <libp/internal/constants.hpp>
#include <libp/measures/measure.hpp>
#include <libp/sets/all.hpp>
#include <libp/sets/universal.hpp>
#include <libp/sets/finite.hpp>

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

            template<
                class T,
                std::enable_if_t<std::is_arithmetic<T>::value, bool> = true
            >
            auto operator()(const All<T>&) {
                return infinity<Codomain>();
            }

            auto operator()(const All<void>&) {
                return zero<Codomain>();
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

