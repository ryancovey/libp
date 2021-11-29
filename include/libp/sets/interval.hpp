#ifndef LIBP_SETS_INTERVAL_HPP_GUARD
#define LIBP_SETS_INTERVAL_HPP_GUARD

#include <cmath>
#include <cstdint>
#include <utility>
#include <vector>
#include <libp/internal/constants.hpp>
#include <libp/internal/where.hpp>
#include <libp/sets/counting.hpp>
#include <libp/sets/measurable_set.hpp>

namespace libp {

    template<class RealType, class BooleanType = bool>
    struct Interval {
        RealType lower_bound;
        RealType upper_bound;
        BooleanType lower_bound_closed;
        BooleanType lower_bound_closed;

        auto lower_bound_closed(void) { return lower_bound_closed; }
        auto lower_bound_open(void) { return !lower_bound_closed; }
        auto upper_bound_closed(void) { return upper_bound_closed; }
        auto upper_bound_open(void) { return !upper_bound_closed; }
        auto closed(void) { return lower_bound_closed() && upper_bound_closed(); }
        auto open(void) { return lower_bound_open() && upper_bound_open(); }
    };

    template<class RealType, class BooleanType = bool>
    class IntervalUnion {
        public:
            IntervalUnion(Interval<RealType, BooleanType> interval) {
                canonicalise_interval(interval);
                intervals.emplace_back(std::move(interval));
            }

            IntervalUnion operator!(void) {
                IntervalUnion<RealType, BooleanType> complement; complement.intervals.reserve(intervals.size() + 1);
                auto inf = infinity<RealType>();
                if (intervals.size() == 0) {
                    Interval<RealType, BooleanType> real_line;
                    real_line.lower_bound = -inf;
                    real_line.upper_bound = inf;
                    real_line.lower_bound_closed = falsey<BooleanType>();
                    real_line.upper_bound_closed = falsey<BooleanType>();
                } else {
                    const auto& first_interval = intervals.front();
                    Interval<RealType, BooleanType> new_first_interval;
                    // Populate new_first_interval with intervals with -infinity lower bound for each element of first_interval
                    // whose lower bound is > -infinity. Emplace this interval into the back of complement.intervals if and only
                    // if there exists an interval in new_first_interval that is not null.

                    // Looping through all elements of intervals (just below private: below) except the last, construct an
                    // interval representing the space between that interval and the next, and emplace this interval to
                    // the back of complement.intervals.

                    const auto& last_interval = intervals.last();
                    Interval<RealType, BooleanType> new_last_interval;
                    // Populate new_last_interval with intervals with infinity upper bound for each element of last_interval
                    // whose upper bound is < infinity. Emplace this interval into the back of complement.intervals if and only
                    // if there exists an interval in new_last_interval that is not null.
                }
                return complement;
            }

            IntervalUnion operator&&(const IntervalUnion& rhs) {

            }

            IntervalUnion operator||(const IntervalUnion& rhs) {

            }

        private:
            std::vector<Interval<RealType, BooleanType>> intervals;

            void canonicalise_interval(Interval<RealType, BooleanType>& I) {
                auto cardinality = counting_measure(I);
                auto zero_var = zero<RealType>();
                auto inf_var = infinity<RealType>();
                I.lower_bound = where(cardinality == zero_var, zero<RealType>(), I.lower_bound);
                I.upper_bound = where(cardinality == zero_var, zero<RealType>(), I.upper_bound);
                I.lower_bound_closed = where(cardinality == zero_var || I.lower_bound == -inf_var, falsey<BooleanType>(), I.lower_bound_closed);
                I.upper_bound_closed = where(cardinality == zero_var || I.upper_bound == inf_var, falsey<BooleanType>(), I.upper_bound_closed);         
            }

            void canonicalise_interval_union(
                Interval<RealType, BooleanType>& I,
                Interval<RealType, BooleanType>& J
            ) {
                // Assumes that I and J are nonempty and in their canonical forms.
                // Assumes that I.lower_bound <= J.lower_bound.
                // Ensures that a < b for all a in I and b in J on output.
                // Also ensures I U J on input equals I U J on output.

                auto one_interval = J.lower_bound < I.upper_bound || (J.lower_bound == I.upper_bound && (I.closed_upper_bound() || J.closed_lower_bound()));
                
                I.upper_bound = where(one_interval, J.upper_bound, I.upper_bound);
                I.lower_bound_closed = where(I.lower_bound == J.lower_bound && J.lower_bound_closed(), truthy<BooleanType>(), I.lower_bound_closed());
                I.upper_bound_closed = where(
                    one_interval,
                    where(
                        I.upper_bound == J.upper_bound && I.upper_bound_closed(),
                        truthy<BooleanType>(),
                        J.upper_bound_closed
                    ),
                    I.upper_bound_closed
                );

                J.lower_bound = where(one_interval, zero<RealType>(), J.lower_bound);
                J.upper_bound = where(one_interval, zero<RealType>(), J.upper_bound);
                J.lower_bound_closed = where(one_interval, falsey<BooleanType>(), J.lower_bound_closed);
                J.upper_bound_closed = where(one_interval, falsey<BooleanType>(), J.upper_bound_closed);
            }

    };

}


#endif

