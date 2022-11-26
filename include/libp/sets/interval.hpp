#ifndef LIBP_SETS_INTERVAL_HPP_GUARD
#define LIBP_SETS_INTERVAL_HPP_GUARD

#include <cmath>
#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>
#include <libp/internal/constants.hpp>
#include <libp/internal/max.hpp>
#include <libp/internal/where.hpp>
#include <libp/measures/counting.hpp>
#include <libp/sets/measurable_set.hpp>

namespace libp {

    enum class EndpointKind {open, closed};

    template<class Numeric>
    struct Endpoint {
        Numeric point;
        EndpointKind kind;
    };

    template<class Numeric>
    Endpoint<Numeric> open(Numeric point) {
        return {std::move(point), EndpointKind::open};
    }

    template<class Numeric>
    Endpoint<Numeric> closed(Numeric point) {
        return {std::move(point), EndpointKind::closed};
    }

    template<class RealType, class BooleanType = bool>
    class IntervalUnion;

    template<class RealType, class BooleanType = bool>
    class Interval : public MeasurableSetCRTP<Interval<RealType, BooleanType>> {
        friend class IntervalUnion<RealType, BooleanType>;
        public:
            Interval(): 
                lower_bound_m(zero<RealType>()),
                upper_bound_m(zero<RealType>()),
                lower_bound_closed_m(falsey<BooleanType>()),
                upper_bound_closed_m(falsey<BooleanType>())
            { }

            Interval(const Endpoint<RealType>& lb, const Endpoint<RealType>& ub):
                lower_bound_m(lb.point),
                upper_bound_m(ub.point),
                lower_bound_closed_m(lb.kind == EndpointKind::closed),
                upper_bound_closed_m(ub.kind == EndpointKind::closed)
            { canonicalise(); }

            auto lower_bound_value(void) { return lower_bound_m; }
            auto lower_bound_closed(void) { return lower_bound_closed_m; }
            auto lower_bound_open(void) { return !lower_bound_closed_m; }

            auto upper_bound_value(void) { return upper_bound_m; }
            auto upper_bound_closed(void) { return upper_bound_closed_m; }
            auto upper_bound_open(void) { return !upper_bound_closed_m; }

            auto closed(void) { return lower_bound_closed() && upper_bound_closed(); }
            auto open(void) { return lower_bound_open() && upper_bound_open(); }

            bool operator==(const Interval<RealType, BooleanType>& rhs) const {
                return lower_bound_m == rhs.lower_bound_m &&
                    upper_bound_m == rhs.upper_bound_m &&
                    lower_bound_closed_m == rhs.lower_bound_closed_m &&
                    upper_bound_closed_m == rhs.upper_bound_closed_m;
            }

            bool operator!=(const Interval<RealType, BooleanType>& rhs) const {
                return !operator==(rhs);
            }

        private:
            RealType lower_bound_m;
            RealType upper_bound_m;
            BooleanType lower_bound_closed_m;
            BooleanType upper_bound_closed_m;

            void canonicalise(void) {
                auto empty = !(lower_bound_m < upper_bound_m || (lower_bound_m == upper_bound_m && closed()));
                if (empty) {
                    auto nill = zero<RealType>();
                    auto no = falsey<BooleanType>();
                    lower_bound_m = nill;
                    lower_bound_closed_m = no;
                    upper_bound_m = nill;
                    upper_bound_closed_m = no;
                }
            }
    };

    template<class RealType, class BooleanType>
    class IntervalUnion : public MeasurableSetCRTP<IntervalUnion<RealType, BooleanType>> {
        public:
            IntervalUnion(Interval<RealType, BooleanType> interval) {
                intervals.emplace_back(std::move(interval));
            }

            IntervalUnion<RealType, BooleanType> operator!(void) {
                IntervalUnion<RealType, BooleanType> complement; complement.intervals.reserve(intervals.size() + 1);
                auto nill = zero<RealType>();
                auto inf = infinity<RealType>();
                auto no = falsey<BooleanType>();
                if (intervals.size() == 0) {
                    Interval<RealType, BooleanType> real_line;
                    real_line.lower_bound_m = -inf;
                    real_line.upper_bound = inf;
                    real_line.lower_bound_closed_m = no;
                    real_line.upper_bound_closed_m = no;
                    complement.intervals.emplace_back(std::move(real_line));
                } else {
                    const auto& first_interval = intervals.front();
                    Interval<RealType, BooleanType> new_first_interval;
                    auto first_interval_unbounded_below = first_interval.lower_bound_m == -inf;
                    new_first_interval.lower_bound_m = where(first_interval_unbounded_below, nill, -inf);
                    new_first_interval.upper_bound = where(first_interval_unbounded_below, nill, first_interval.lower_bound_m);
                    new_first_interval.lower_bound_closed_m = no;
                    new_first_interval.upper_bound_closed_m = where(first_interval_unbounded_below, no, !first_interval.lower_bound_closed_m);
                    if (!all_of(counting_measure<RealType>(new_first_interval) == nill)) {
                        complement.intervals.emplace_back(std::move(new_first_interval));
                    }

                    auto intervals_size = intervals.size();
                    for (decltype(intervals_size) i = 0; i != intervals_size-1; ++i) {
                        Interval<RealType, BooleanType> new_interval;
                        new_interval.lower_bound_m = intervals[i].upper_bound;
                        new_interval.upper_bound = intervals[i+1].lower_bound_m;
                        new_interval.lower_bound_closed_m = !intervals[i].upper_bound_closed_m;
                        new_interval.upper_bound_closed_m = !intervals[i+1].lower_bound_closed_m;
                        complement.intervals.emplace_back(std::move(new_interval));
                    }

                    const auto& last_interval = intervals.last();
                    Interval<RealType, BooleanType> new_last_interval;
                    auto last_interval_unbounded_above = last_interval.upper_bound == inf;
                    new_last_interval.lower_bound_m = where(last_interval_unbounded_above, nill, inf);
                    new_last_interval.upper_bound = where(last_interval_unbounded_above, nill, -inf);
                    new_last_interval.lower_bound_closed_m = where(last_interval_unbounded_above, no, !last_interval.upper_bound_closed_m);
                    new_last_interval.upper_bound_closed_m = no;
                    if (!all_of(counting_measure<RealType>(new_last_interval) == nill)) {
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
                        bool lhsi_lteq_J = lhs_iter->lower_bound_m <= J->lower_bound_m;
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

            void canonicalise_interval_intersection(
                Interval<RealType, BooleanType>& I,
                Interval<RealType, BooleanType>& J
            ) {
                // Assumes that I and J are nonempty and in their canonical forms.
                // Assumes that I.lower_bound_m <= J.lower_bound_m.
                // Ensures that I and J on input equals I on output.
                // Ensures that J on output equals J\I on input.
                
                auto nill = zero<RealType>();
                auto no = falsey<BooleanType>();

                auto null_I_out = J.lower_bound_m > I.upper_bound || (J.lower_bound_m == I.upper_bound && I.upper_bound_open() && J.lower_bound_open());
                auto null_J_out = J.upper_bound < I.upper_bound || (J.upper_bound == I.upper_bound && (I.upper_bound_closed() || J.upper_bound_open()));

                Interval<RealType, BooleanType> I_out;
                I_out.lower_bound_m = where(null_I_out, nill, J.lower_bound_m);
                I_out.upper_bound = where(null_I_out, nill, min(I.upper_bound, J.upper_bound));
                I_out.lower_bound_closed_m = where(
                    null_I_out,
                    no,
                    where(
                        I.lower_bound_m == J.lower_bound_m && I.lower_bound_open(),
                        no,
                        J.lower_bound_closed_m
                    )
                );
                I_out.upper_bound_closed_m = where(
                    null_I_out,
                    no,
                    where(
                        I.upper_bound < J.upper_bound,
                        I.upper_bound_closed_m,
                        J.upper_bound_closed_m
                    )
                );

                Interval<RealType, BooleanType> J_out;
                J_out.lower_bound_m = where(null_J_out, nill, max(J.lower_bound_m, I.upper_bound));
                J_out.upper_bound = where(null_J_out, nill, J.upper_bound);
                J_out.lower_bound_closed_m = where(
                    null_J_out,
                    no,
                    where(
                        I.upper_bound == J.lower_bound_m && I.upper_bound_closed(),
                        no,
                        J.lower_bound_closed_m
                    )
                );
                J_out.upper_bound_closed_m = where(null_J_out, nill, J.upper_bound_closed_m);

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

                auto nill = zero<RealType>();
                auto yes = truthy<BooleanType>();
                auto no = falsey<BooleanType>();

                auto one_interval = J.lower_bound_m < I.upper_bound || (J.lower_bound_m == I.upper_bound && (I.upper_bound_closed() || J.lower_bound_closed_m()));
                
                Interval<RealType, BooleanType> I_out;
                I_out.upper_bound = where(one_interval, J.upper_bound, I.upper_bound);
                I_out.lower_bound_closed_m = where(I.lower_bound_m == J.lower_bound_m && J.lower_bound_closed(), yes, I.lower_bound_closed());
                I_out.upper_bound_closed_m = where(
                    one_interval,
                    where(
                        I.upper_bound == J.upper_bound && I.upper_bound_closed_m(),
                        yes,
                        J.upper_bound_closed_m
                    ),
                    I.upper_bound_closed_m
                );
                
                I = I_out;
                J.lower_bound_m = where(one_interval, nill, J.lower_bound_m);
                J.upper_bound = where(one_interval, nill, J.upper_bound);
                J.lower_bound_closed_m = where(one_interval, no, J.lower_bound_closed_m);
                J.upper_bound_closed_m = where(one_interval, no, J.upper_bound_closed_m);
            }
    };

}

#endif
