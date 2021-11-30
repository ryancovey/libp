#ifndef LIBP_SETS_INTERVAL_HPP_GUARD
#define LIBP_SETS_INTERVAL_HPP_GUARD

#include <cmath>
#include <cstdint>
#include <iterator>
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
                auto zero = zero<RealType>();
                auto inf = infinity<RealType>();
                auto falsey = falsey<BooleanType>();
                if (intervals.size() == 0) {
                    Interval<RealType, BooleanType> real_line;
                    real_line.lower_bound = -inf;
                    real_line.upper_bound = inf;
                    real_line.lower_bound_closed = falsey;
                    real_line.upper_bound_closed = falsey;
                    complement.intervals.emplace_back(std::move(real_line));
                } else {
                    const auto& first_interval = intervals.front();
                    Interval<RealType, BooleanType> new_first_interval;
                    auto first_interval_unbounded_below = first_interval.lower_bound == -inf;
                    new_first_interval.lower_bound = where(first_interval_unbounded_below, zero, -inf);
                    new_first_interval.upper_bound = where(first_interval_unbounded_below, zero, first_interval.lower_bound);
                    new_first_interval.lower_bound_closed = falsey;
                    new_first_interval.upper_bound_closed = where(first_interval_unbounded_below, falsey, !first_interval.lower_bound_closed);
                    if (!all_of(counting_measure<RealType>(new_first_interval) == zero)) {
                        complement.intervals.emplace_back(std::move(new_first_interval));
                    }

                    auto intervals_size = intervals.size();
                    for (decltype(intervals_size) i = 0; i != intervals_size-1; ++i) {
                        Interval<RealType, BooleanType> new_interval;
                        new_interval.lower_bound = intervals[i].upper_bound;
                        new_interval.upper_bound = intervals[i+1].lower_bound;
                        new_interval.lower_bound_closed = !intervals[i].upper_bound_closed;
                        new_interval.upper_bound_closed = !intervals[i+1].lower_bound_closed;
                        complement.intervals.emplace_back(std::move(new_interval));
                    }

                    const auto& last_interval = intervals.last();
                    Interval<RealType, BooleanType> new_last_interval;
                    auto last_interval_unbounded_above = last_interval.upper_bound == inf;
                    new_last_interval.lower_bound = where(last_interval_unbounded_above, zero, inf);
                    new_last_interval.upper_bound = where(last_interval_unbounded_above, zero, -inf);
                    new_last_interval.lower_bound_closed = where(last_interval_unbounded_above, falsey, !last_interval.upper_bound_closed);
                    new_last_interval.upper_bound_closed = falsey;
                    if (!all_of(counting_measure<RealType>(new_last_interval) == zero)) {
                        complement.intervals.emplace_back(std::move(new_last_interval));
                    }
                }
                return complement;
            }

            IntervalUnion operator&&(const IntervalUnion& rhs) {
                // Right now this function works only with intervals defined by scalars
                // (RealType == double, BooleanType == bool) and not by tensors
                // (RealType == torch::Tensor, BooleanType == torch::Tensor). We might
                // need to specialise this function for tensors, to account for the fact
                // that tensor intervals with a mix of null and non-null intervals across
                // tensor-indices need to have their nulls filled in by the non-null
                // interval with the same tensor-index but the next intervals-index
                // (by intervals-index we mean the index of the private member variable
                // "intervals").

                IntervalUnion<RealType, BooleanType> intersection;
                if (intervals.size() != 0 && rhs.intervals.size() != 0) {
                    auto lhs_iter = intervals.cbegin();
                    auto lhs_end = intervals.cend();
                    auto rhs_iter = rhs.intervals.cbegin();
                    auto rhs_end = rhs.intervals.cend();
                    auto J = *rhs_iter;
                    while (true) {
                        bool lhsi_lteq_J = lhs_iter->lower_bound <= J->lower_bound;
                        auto I = where(lhsi_lteq_J, *lhs_iter, J);
                        J = where(lhsi_lteq_J, J, *lhs_iter);
                        canonicalise_interval_intersection(I,J); // This call sets I to I and J, and sets J to J\I.
                        if (counting_measure<std::size_t>(I) != 0) { intersection.intervals.emplace_back(std::move(I)); }
                        if (++lhs_iter == lhs_end) { return intersection; }
                        if (counting_measure<std::size_t>(J) == 0) {
                            if (++rhs_iter == rhs_end) { return intersection; }
                            J = *rhs_iter;
                        }
                    }
                }
                return intersection;
            }

            IntervalUnion operator||(const IntervalUnion& rhs) {
                // See message at top of IntervalUnion function above.
                // Right now we are implementing this using demorgan's laws,
                // but we should eventually replace this with a more
                // (memory and time) efficient solution once we have a
                // version of operator&& that can handle vectors.

                auto& lhs = *this;
                return !(!lhs && !rhs);
            }

        private:
            std::vector<Interval<RealType, BooleanType>> intervals;

            void canonicalise_interval(Interval<RealType, BooleanType>& I) {
                auto cardinality = counting_measure<RealType>(I);
                auto zero = zero<RealType>();
                auto inf = infinity<RealType>();
                auto falsey = falsey<BooleanType>();
                I.lower_bound = where(cardinality == zero, zero, I.lower_bound);
                I.upper_bound = where(cardinality == zero, zero, I.upper_bound);
                I.lower_bound_closed = where(cardinality == zero_var || I.lower_bound == -inf, falsey, I.lower_bound_closed);
                I.upper_bound_closed = where(cardinality == zero_var || I.upper_bound == inf, falsey, I.upper_bound_closed);         
            }

            void canonicalise_interval_intersection(
                Interval<RealType, BooleanType>& I,
                Interval<RealType, BooleanType>& J
            ) {
                // Assumes that I and J are nonempty and in their canonical forms.
                // Assumes that I.lower_bound <= J.lower_bound.
                // Ensures that I and J on input equals I on output.
                // Ensures that J on output equals J\I on input.
                
                auto zero = zero<RealType>();
                auto falsey = falsey<BooleanType>();

                auto null_I_out = J.lower_bound > I.upper_bound || (J.lower_bound == I.upper_bound && I.upper_bound_open() && J.lower_bound_open());
                auto null_J_out = J.upper_bound < I.upper_bound || (J.upper_bound == I.upper_bound && (I.upper_bound_closed() || J.upper_bound_open()));

                Interval<RealType, BooleanType> I_out;
                I_out.lower_bound = where(null_I_out, zero, J.lower_bound);
                I_out.upper_bound = where(null_I_out, zero, min(I.upper_bound, J.upper_bound));
                I_out.lower_bound_closed = where(
                    null_interval,
                    falsey,
                    where(
                        I.lower_bound == J.lower_bound && I.lower_bound_open(),
                        falsey,
                        J.lower_bound_closed
                    )
                );
                I_out.upper_bound_closed = where(
                    null_interval,
                    falsey,
                    where(
                        I.upper_bound < J.upper_bound,
                        I.upper_bound_closed,
                        J.upper_bound_closed
                    )
                );

                Interval<RealType, BooleanType> J_out;
                J_out.lower_bound = where(null_J_out, zero, max(J.lower_bound, I.upper_bound));
                J_out.upper_bound = where(null_J_out, zero, J.upper_bound);
                J_out.lower_bound_closed = where(
                    null_J_out,
                    falsey,
                    where(
                        I.upper_bound == J.lower_bound && I.upper_bound_closed(),
                        falsey,
                        J.lower_bound_closed
                    )
                );
                J_out.upper_bound_closed = where(null_J_out, zero, J.upper_bound_closed);

                I = I_out;
                J = J_out;
            }

            void canonicalise_interval_union(
                Interval<RealType, BooleanType>& I,
                Interval<RealType, BooleanType>& J
            ) {
                // Assumes that I and J are nonempty and in their canonical forms.
                // Assumes that I.lower_bound <= J.lower_bound.
                // Ensures that a < b for all a in I and b in J on output.
                // Also ensures I U J on input equals I U J on output.

                auto zero = zero<RealType>();
                auto truthy = truthy<BooleanType>();
                auto falsey = falsey<BooleanType>();

                auto one_interval = J.lower_bound < I.upper_bound || (J.lower_bound == I.upper_bound && (I.upper_bound_closed() || J.lower_bound_closed()));
                
                Interval<RealType, BooleanType> I_out;
                I_out.upper_bound = where(one_interval, J.upper_bound, I.upper_bound);
                I_out.lower_bound_closed = where(I.lower_bound == J.lower_bound && J.lower_bound_closed(), truthy, I.lower_bound_closed());
                I_out.upper_bound_closed = where(
                    one_interval,
                    where(
                        I.upper_bound == J.upper_bound && I.upper_bound_closed(),
                        truthy,
                        J.upper_bound_closed
                    ),
                    I.upper_bound_closed
                );
                
                I = I_out;
                J.lower_bound = where(one_interval, zero, J.lower_bound);
                J.upper_bound = where(one_interval, zero, J.upper_bound);
                J.lower_bound_closed = where(one_interval, falsey, J.lower_bound_closed);
                J.upper_bound_closed = where(one_interval, falsey, J.upper_bound_closed);
            }
    };

}


#endif

