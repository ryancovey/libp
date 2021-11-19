#ifndef LIBP_SETS_INTERVAL_HPP_GUARD
#define LIBP_SETS_INTERVAL_HPP_GUARD

#include <cmath>
#include <cstdint>
#include <utility>
#include <vector>
#include <libp/sets/counting.hpp>
#include <libp/sets/measurable_set.hpp>

namespace libp {

    template<class RealType>
    struct Interval {
        RealType lower_bound;
        RealType upper_bound;
        uint_least8_t type;
        
        auto lower_bound_openness(void) { return type & Interval::lower_bound_openness; }
        auto lower_bound_inv_openness(void) { return ~(type & Interval::lower_bound_openness) & Interval::lower_bound_openness; }
        bool open_lower_bound(void) { return lower_bound_openness() == Interval::open_lower_bound; }
        bool closed_lower_bound(void) { return lower_bound_openness() == Interval::closed_lower_bound; }
        bool integer_lower_bound(void) { return lower_bound == std::llround(lower_bound); }

        auto upper_bound_openness(void) { return type & Interval::upper_bound_openness; }
        auto upper_bound_inv_openness(void) { return ~(type & Interval::upper_bound_openness) & Interval::upper_bound_openness; }
        bool open_upper_bound(void) { return upper_bound_openness() == Interval::open_upper_bound; }
        bool closed_upper_bound_closed(void) { return upper_bound_openness() == Interval::closed_upper_bound; }
        bool integer_upper_bound(void) { return upper_bound == std::llround(upper_bound); }

        bool openness(void) { return type & Interval::bounds; }
        bool open(void) { return (type & Interval::openness) == Interval::open; }
        bool closed(void) { return (type & Interval::openness) == Interval::closed; }

        auto space(void) { return type & Interval::space; }
        auto space_complementation(void) { return type & Interval::space_complementation; }
        auto space_inv_complementation(void) { return ~(type & Interval::space_complementation) & Interval::space_complementation; }
        auto space_complement(void) { return (type & Interval::space_base) | space_inv_complementation(); }
        bool integer(void) { return space() == (Interval::integer | Interval::noncomplement_space); }
        bool integer_complement(void) { return space() == (Interval::integer | Interval::complement_space); }
        bool real(void) { return space() == (Interval::real | Interval::noncomplement_space); }
        bool real_complement(void) { return space() == (Interval::real | Interval::complement_space); }

        bool contains_integers(void) {
            return !space_complementation() && counting_measure(Interval<RealType>({lower_bound, upper_bound, openness() | Interval::integer}));
        }
        
        // The class IntervalUnion below depends on these specific values. Changing these will
        // require updating IntervalUnion as well. In particular, we use bit shifting to construct
        // new intervals whose lower_bound_openness() depends on the upper_bound_openness() of
        // another interval, and vice versa.

        static const uint_least8_t lower_bound_openness = 0b0001;
        static const uint_least8_t upper_bound_openness = 0b0010;
        static const uint_least8_t openness = lower_bound_openness | upper_bound_openness;
        static const uint_least8_t space_base = 0b0100;
        static const uint_least8_t space_complementation = 0b1000;
        static const uint_least8_t space = space_base | space_complementation;

        static const uint_least8_t open_lower_bound = 0b0000;
        static const uint_least8_t closed_lower_bound = 0b0001;
        static const uint_least8_t open_upper_bound = 0b0000;
        static const uint_least8_t closed_upper_bound = 0b0010;
        static const uint_least8_t open = open_lower_bound | open_upper_bound;
        static const uint_least8_t closed = closed_lower_bound | closed_upper_bound;
        static const uint_least8_t complement_space = 0b1000;
        static const uint_least8_t noncomplement_space = 0b0000;
        static const uint_least8_t real = 0b0000;
        static const uint_least8_t integer = 0b0100;
        static const uint_least8_t real_complement = real | complement_space;
        static const uint_least8_t integer_complement = integer | complement_space;
    };

    template<class RealType>
    class IntervalUnion {
        public:
            IntervalUnion(Interval<RealType> interval) {
                if (counting_measure(interval) > 0) {
                    auto inf = infinity<RealType>();
                    if (interval.lower_bound == -inf && interval.closed_lower_bound()) {
                        interval.type ^= Interval::lower_bound_openness;
                    }
                    if (interval.upper_bound == inf && interval.closed_upper_bound()) {
                        interval.type ^= Interval::upper_bound_openness;
                    }
                    intervals.emplace_back(std::move(interval));
                }
            }

            IntervalUnion operator!(void) {
                IntervalUnion complement; complement.intervals.reserve(2*intervals.size() + 1);
                auto inf = infinity<RealType>();
                if (intervals.size() == 0) {
                    complement.intervals.push_back({-inf, inf, Interval::open | Interval::real});
                } else {
                    auto emplace_complement_within_interval_if_nonempty = [&](const Interval<RealType>& J) {
                        if (!J.real()) {
                            auto complement_within_J = naive_complement_within_interval(J);
                            canonicalise(complement_within_J);
                            complement.intervals.emplace_back(complement_within_J);
                        }
                    }

                    const auto& first_interval = intervals.front();
                    if (first_interval.lower_bound > -inf) {
                        complement.intervals.emplace_back(naive_left_of_interval(first_interval));
                    }
                    
                    for (decltype(intervals.size()) i = 0; i != intervals.size(); ++i) {
                        emplace_complement_within_interval_if_nonempty(intervals[i]);
                    }
                    
                    const auto& last_interval = intervals.back();
                    if (last_interval.upper_bound < inf) {
                        complement.intervals.emplace_back(naive_right_of_interval(I));
                    }
                }
                return complement;
            }

        private:
            std::vector<Interval<RealType>> intervals;

            Interval<RealType> naive_left_of_interval(const Interval<RealType>& I) {
                return {-infinity<RealType>(), I.lower_bound, Interval::open_lower_bound | (I.lower_bound_inv_openness() << 1) | Interval::real};
            }

            Interval<RealType> naive_right_of_interval(const Interval<RealType>& I) {
                return {I.upper_bound, infinity<RealType(), (I.upper_bound_inv_openness() >> 1) | Interval::open_upper_bound | Interval::real};
            }

            Interval<RealType> naive_complement_within_interval(const Interval<RealType>& I) {
                Interval<RealType> complement = {I.lower_bound, I.upper_bound, I.openness() | I.space_complement()};
            }

            Interval<RealType> naive_inbetween(const Interval<RealType>& I, const Interval<RealType>& J) {
                // Assumes i < j for all i in I and j in J.
                return {I.upper_bound, J.lower_bound, (I.upper_bound_inv_openness() >> 1) | (J.lower_bound_inv_openness() << 1)};
            }

            Interval<RealType> canonical_null(void) {
                return {0.0, 0.0, Interval::open | Interval::real};
            }

            void canonicalise_interval(Interval<RealType>& I) {
                auto cardinality = counting_measure(I);
                if (cardinality = 0) {
                    I = canonical_null();
                    return;
                }
                
                if (I.lower_bound == -0.0) I.lower_bound = 0.0;
                if (I.upper_bound == -0.0) I.upper_bound = 0.0;

                if (cardinality = 1) {
                    I = {
                        I.lower_bound,
                        I.upper_bound,
                        Interval::closed | Interval::integer
                    };
                } else if (I.integer()) {
                    I = {
                        std::ceil(I.open_lower_bound() ? std::nextafter(I.lower_bound, I.lower_bound + one<RealType>()) : I.lower_bound),
                        std::floor(I.open_upper_bound() ? std::nextafter(I.upper_bound, I.upper_bound - one<RealType>()) : I.upper_bound),
                        Interval::closed | Interval::integer
                    };
                } else if (I.integer_complement()) {
                    auto integers_to_avoid = Interval<RealType>({I.lower_bound, I.upper_bound, I.openness() | Interval::integer}).contains_integers();
                    I = {
                        I.lower_bound,
                        I.upper_bound,
                        (I.integer_lower_bound() ? Interval::open_lower_bound : I.lower_bound_openness()) |
                        (I.integer_upper_bound() ? Interval::open_upper_bound : I.upper_bound_openness()) |
                        (integers_to_avoid ? Interval::integer_complement : Interval::integer_real)
                    };
                }
            }

            Interval<RealType> canonicalise_interval_union_impl(Interval<RealType>& I, Interval<RealType>& J);
            Interval<RealType> canonicalise_interval_union(Interval<RealType>& I, Interval<RealType>& J) {
                auto new_I = I;
                auto new_J = J;
                Interval<RealType> K;

                // Assumes that I and J are nonempty and in their canonical forms.
                // Assumes that I.lower_bound <= J.lower_bound.
                // Ensures that a < b < c for all A in new_I, b in new_J and c in K on output,
                // where K is the returned interval. Also ensures I U J = new_I U new_J U K.
                
                canonicalise_interval(new_I);
                canonicalise_interval(new_J);
                if (counting_measure(new_I) == 0) {
                    std::swap(new_I, new_J);
                    K = canonical_null();
                } else {
                    if (counting_measure(new_J) != 0) {
                        if (new_I.lower_bound > new_J.lower_bound) { std::swap(new_I, new_J); }
                        K = canonicalise_interval_union_impl(new_I,new_J);
                    } else {
                        K = canonical_null();
                    }
                }

                I = new_I;
                J = new_J;
                return K;
            }

            Interval<RealType> negative_interval(const Interval<RealType>& I) {
                return {
                    I.upper_bouned == -0.0 ? 0.0 : -I.upper_bound,
                    I.lower_bound == -0.0 ? 0.0 : -I.lower_bound,
                    (I.upper_bound_openness() >> 1) | (I.lower_bound_openness() << 1) | I.space()
                };
            }

            Interval<RealType> canonicalise_interval_union_impl(Interval<RealType>& I, Interval<RealType>& J) {
                Interval<RealType> new_I, new_J, K;

                // We don't need to consider I.real_complement() or J.real_complement(),
                // because we assume that I and J are nonempty.
                
                // We rely on the function canonicalise defined above, so provided
                // new_I, new_J and K contain the correct elements before canonicalisation, we're
                // okay.

                // For these kinds of intervals I and J, use symmetry. Now we
                // only need to explicitely handle the others.
                if ((!I.real() && J.real()) || (I.integer() && !J.integer())) {
                    new_I = negative_interval(I); new_J = negative_interval(J);
                    K = canonicalise_interval_union_impl(new_J, new_I);
                    I = negative_interval(new_I); J = negative_interval(new_J); K = negative_interval(K);
                    return K;
                }

                // one_real_interval is true if and only if the union of I and J can be expressed as a single real interval.
                bool one_real_interval =
                    (I.real() && J.real() && (J.lower_bound < I.upper_bound || (J.lower_bound == I.upper_bound && (I.closed_upper_bound() || J.closed_lower_bound())))) ||
                    (!I.integer() && J.integer && I.lower_bound = J.lower_bound && I.upper_bound == J.upper_bound);

                // one_integer_complement_interval is true if and only if the union of I and J can be expressed as a single integer complement interval.
                bool one_integer_complement_interval = !I.integer() && !J.integer() && I.upper_bound == J.lower_bound &&
                    I.integer_upper_bound() && !I.contains_integer() && !J.contains_integer();

                bool I_contracts = I.integer_complement() && J.integer() && I.lower_bound < J.lower_bound;

                // one_integer_interval is true if and only if the union of I and J can be expressed as a single integer interval.
                bool one_integer_interval = I.integer() && J.integer() && std::round(J.lower_bound - I.upper_bound) < 1.1;

                bool one_interval = one_real_interval || one_integer_complement_interval || one_integer_interval;
                bool three_intervals = I_contracts && J.upper_bound < I.upper_bound;
                bool two_intervals = !(one_interval || three_intervals);

                // I_swallows_J is true if we will extend I's upper bound to cover J.
                bool I_swallows_J = one_interval && I.upper_bound < J.upper_bound;

                // I_rounds_up is true if we will round I's upper bound to the next highest integer.
                bool I_rounds_up = I.real() && J.integer_complement() && I.upper_bound == J.lower_bound;

                auto new_I_lower_bound = I.lower_bound;

                auto new_I_lower_bound_openness = I.lower_bound_openness();
                if (I.lower_bound == J.lower_bound) new_I_lower_bound_openness |= J.lower_bound_openness();

                auto new_I_upper_bound = I_swallows_J ? J.upper_bound
                    : I_rounds_up ? std::ceil(I.upper_bound)
                    : I_contracts ? J.lower_bound
                    : I.upper_bound;

                auto new_I_upper_bound_openness = I_rounds_up ? Interval::open_upper_bound : I.upper_bound_openness();
                if (new_I_upper_bound == J.lower_bound) {
                    new_I_upper_bound_openness |= J.lower_bound_openness();
                } else if (new_I_upper_bound == J.upper_bound) {
                    new_I_upper_bound_openness |= J.upper_bound_openness();
                }

                auto new_I_space = one_real_interval ? Interval::real
                    : one_integer_complement_interval ? Interval::integer_complement
                    : one_integer_interval ? Interval::integer
                    : I.space();

                new_I = {new_I_lower_bound, new_I_upper_bound, new_I_lower_bound_openness | new_I_upper_bound_openness | new_I_space};
                canonicalise(new_I);

                if (one_interval) {
                    new_J = canonical_null();
                } else {
                    auto new_J_lower_bound = std::max(J.lower_bound, new_I.upper_bound);
                    auto new_J_lower_bound_openness = new_J_lower_bound == new_I.upper_bound && new_I.closed_upper_bound() ? Interval::open_lower_bound : J.lower_bound_openness();
                    auto new_J_upper_bound = J.upper_bound;
                    auto new_J_upper_bound_openness = J.upper_bound_openness;
                    auto new_J_space = I_contracts ? Interval::real : J.space();
                    new_J = {new_J_lower_bound, new_J_upper_bound, new_J_lower_bound_openness | new_J_upper_bound_openness | new_J_space};
                    canonicalise(new_J);
                }

                if (three_intervals) {
                    K = {J.upper_bound, I.upper_bound, Interval::open_lower_bound | I.upper_bound_openness() | Interval::integer_complement};
                    canonicalise(K);
                } else {
                    K = canonical_null();
                }

                I = new_I;
                J = new_J;
                return K;
            }

            Interval<RealType> naive_union(const Interval<RealType>& I, const Interval<RealType>& J) {

            }

            Interval<RealType> naive_intersection(const Interval<RealType>& I, const Interval<RealType>& J) {

            }
    };

}

#endif

