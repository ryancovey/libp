#ifndef LIBP_SETS_INTERVAL_HPP_GUARD
#define LIBP_SETS_INTERVAL_HPP_GUARD

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <limits>
#include <utility>
#include <vector>
#include <libp/internal/constants.hpp>
#include <libp/internal/max.hpp>
#include <libp/internal/where.hpp>
// #include <libp/measures/counting.hpp>
// #include <libp/sets/measurable_set.hpp>

namespace libp {

    template<class RealType>
    class IntervalUnion;

    template<class RealType>
    class Interval {
        friend class IntervalUnion<RealType>;

        public:
            Interval(): 
                left_value_m(0),
                right_value_m(0),
                left_bracket_m('('),
                right_bracket_m(')')
            { }

            Interval(char left_bracket_in, RealType left_value_in, RealType right_value_in, char right_bracket_in):
                left_value_m(left_value_in),
                right_value_m(right_value_in),
                left_bracket_m(left_bracket_in),
                right_bracket_m(right_bracket_in)
            {
                if (
                    (left_bracket_m != '(' && left_bracket_m != '[') ||
                    (right_bracket_m != ')' && right_bracket_m != ']') ||
                    std::isnan(left_value_m) || std::isnan(right_value_m)
                ) {
                    set_to_nan();
                } else {
                    canonicalise();
                }
            }

            auto left_value(void) const { return left_value_m; }
            auto left_bracket(void) const { return left_bracket_m; }
            auto inv_left_bracket(void) const { return left_bracket_m == '(' ? '[' : '('; }

            auto right_value(void) const { return right_value_m; }
            auto right_bracket(void) const { return right_bracket_m; }
            auto inv_right_bracket(void) const { return right_bracket_m == ')' ? ']' : ')'; }

            auto open(void) const { return left_bracket() == '(' && right_bracket() == ')'; }
            auto closed(void) const { return left_bracket() == '[' && right_bracket() == ']'; }

            auto empty(void) const {
                // Does not assume that we're in a canonical representation, but does assume that the
                // left bracket is '(' or '[', likewise for the right bracket. Returns false if NaN.
                return ((left_value_m == right_value_m && !closed()) || left_value_m > right_value_m);
            }

            auto isnan(void) const { return std::isnan(left_value_m); }

            template<class T>
            bool operator==(const Interval<T>& rhs) const {
                return left_bracket_m == rhs.left_bracket_m &&
                    left_value_m == rhs.left_value_m &&
                    right_value_m == rhs.right_value_m &&
                    right_bracket_m == rhs.right_bracket_m;
            }

            template<class T>
            bool operator!=(const Interval<T>& rhs) const {
                if (isnan() || rhs.isnan()) {
                    return false;
                } else {
                    return !operator==(rhs);
                }
            }

            static Interval<RealType> nan(void) {
                Interval<RealType> ret;
                ret.set_to_nan();
                return ret;
            }

        private:
            RealType left_value_m;
            RealType right_value_m;
            char left_bracket_m;
            char right_bracket_m;

            void canonicalise(void) {
                if (empty()) {
                    left_bracket_m = '(';
                    left_value_m = 0;
                    right_value_m = 0;
                    right_bracket_m = ')';
                }
            }

            void set_to_nan(void) {
                left_bracket_m = '(';
                left_value_m = std::numeric_limits<RealType>::quiet_NaN();
                right_value_m = std::numeric_limits<RealType>::quiet_NaN();
                right_bracket_m = ']';
            };
    };

    template<class RealType>
    std::ostream& operator<<(std::ostream& os, const libp::Interval<RealType>& I) {
        os << I.left_bracket() << I.left_value() << ',' << I.right_value() << I.right_bracket();
        return os;
    }

    template<class RealType>
    class IntervalUnion {
        public:
            IntervalUnion() = default;

            IntervalUnion(Interval<RealType> interval) {
                if (!interval.empty()) { intervals.emplace_back(std::move(interval)); }
            }

            IntervalUnion(std::initializer_list<Interval<RealType>> l) {
                for (const auto& interval : l) {
                    if (interval.isnan()) {
                        intervals.clear();
                        intervals.emplace_back(interval);
                        return;
                    } else if (!interval.empty()) {
                        intervals.emplace_back(interval);
                    }
                }
                canonicalise_unempty_intervals();
            }

            auto cbegin(void) const { return intervals.cbegin(); }
            auto cend(void) const { return intervals.cend(); }

            bool empty(void) const { return intervals.empty(); }

            bool isnan(void) const { return !empty() && intervals.front().isnan(); }
            static IntervalUnion<RealType> nan(void) { return Interval<RealType>::nan(); }

            IntervalUnion<RealType> inv(bool extended_real_line = false) const {
                if (intervals.size() == 0) {
                    return Interval<RealType>(
                        extended_real_line ? '[' : '(',
                        -std::numeric_limits<RealType>::infinity(),
                        std::numeric_limits<RealType>::infinity(),
                        extended_real_line ? ']' : ')'
                    );
                } else {
                    const auto& first_interval = intervals.front();
                    if (first_interval.isnan()) {
                        return *this;
                    } else {
                        auto intervals_size = intervals.size();

                        IntervalUnion<RealType> complement; complement.intervals.reserve(intervals_size + 1);
                        
                        Interval<RealType> new_first_interval(
                            extended_real_line ? '[' : '(',
                            -std::numeric_limits<RealType>::infinity(),
                            first_interval.left_value(),
                            first_interval.inv_left_bracket()
                        );
                        if (!new_first_interval.empty()) {
                            complement.intervals.emplace_back(new_first_interval);
                        }

                        for (decltype(intervals_size) i = 0; i != intervals_size-1; ++i) {
                            complement.intervals.emplace_back(
                                intervals[i].inv_right_bracket(),
                                intervals[i].right_value(),
                                intervals[i+1].left_value(),
                                intervals[i+1].inv_left_bracket()
                            );
                        }

                        const auto& last_interval = intervals.last();
                        Interval<RealType> new_last_interval(
                            last_interval.inv_right_bracket(),
                            last_interval.right_value(),
                            std::numeric_limits<RealType>::infinity(),
                            extended_real_line ? ']' : ')'
                        );
                        if (!new_last_interval.empty()) {
                            complement.intervals.emplace_back(std::move(new_last_interval));
                        }

                        return complement;
                    }
                }
            }

            IntervalUnion<RealType> operator!() const {
                const auto& front = intervals.front();
                const auto& back = intervals.back();
                return inv(
                    !empty() && (
                        front.left_value() == -std::numeric_limits<RealType>::infinity() && front.left_bracket() == '[' ||
                        back.right_value() == std::numeric_limits<RealType>::infinity() && back.right_bracket() == ']'
                    )
                );
            }

            IntervalUnion<RealType> operator&&(const IntervalUnion<RealType>& rhs) const {
                if (isnan() || rhs.isnan()) { return nan(); }
                IntervalUnion<RealType> intersection; intersection.intervals.reserve(std::max(intervals.size(), rhs.intervals.size()));
                if (intervals.size() != 0 && rhs.intervals.size() != 0) {
                    auto lhs_iter = intervals.cbegin();
                    auto lhs_end = intervals.cend();
                    auto rhs_iter = rhs.intervals.cbegin();
                    auto rhs_end = rhs.intervals.cend();
                    auto I = *lhs_iter;
                    auto J = *rhs_iter;
                    while (lhs_iter != lhs_end && rhs_iter != rhs_end) {
                        std::tie(I,J) = interval_intersection_right_remainder(I,J);
                        if (!I.empty()) { intersection.intervals.emplace_back(std::move(I)); }
                        if (J.right_value() == rhs_iter->right_value() && J.right_bracket() == rhs_iter->right_bracket()) {
                            ++lhs_iter;
                        } else {
                            ++rhs_iter;
                        }
                    }
                }
                return intersection;
            }

            IntervalUnion<RealType> operator||(const IntervalUnion<RealType>& rhs) const {
                if (isnan() || rhs.isnan()) { return nan(); }
                IntervalUnion<RealType> set_union; set_union.intervals.reserve(std::max(intervals.size(), rhs.intervals.size()));
                auto lhs_iter = intervals.cbegin();
                auto rhs_iter = rhs.intervals.cbegin();
                while (lhs_iter != intervals.cend() && rhs_iter != intervals.cend()) {
                    if (
                        lhs_iter->left_value() < rhs_iter->left_value() ||
                        lhs_iter->left_value() == rhs_iter->left_value() && lhs_iter->left_bracket() == '[' && rhs_iter->left_bracket() == '('
                    ) {
                        set_union.intervals.emplace_back(*lhs_iter++);
                    } else {
                        set_union.intervals.emplace_back(*rhs_iter++);
                    }
                }
                set_union.intervals.insert(set_union.intervals.end(), lhs_iter, intervals.cend());
                set_union.intervals.insert(set_union.intervals.end(), rhs_iter, rhs.intervals.cend());
                set_union.canonicalise_sorted_unempty_intervals();
                return set_union;
            }

            bool operator==(const IntervalUnion<RealType>& rhs) const {
                // Change this to default when migrating to C++20.
                return intervals == rhs.intervals;
            }

            bool operator!=(const IntervalUnion<RealType>& rhs) const {
                // Change this to default when migrating to C++20.
                return intervals != rhs.intervals;
            }

        private:
            std::vector<Interval<RealType>> intervals;

            static auto interval_intersection_right_remainder(const Interval<RealType>& I, const Interval<RealType>& J) {
                auto interval_intersection = Interval<RealType>(
                    (
                        I.left_value() < J.left_value()  ? J.left_bracket() :
                        I.left_value() == J.left_value() ? std::min(I.left_bracket(), J.left_bracket()) :
                                                           I.left_bracket()
                    ),
                    std::max(I.left_value(), J.left_value()),
                    std::min(I.right_value(), J.right_value()),
                    (
                        I.right_value() < J.right_value()  ? I.right_bracket() :
                        I.right_value() == J.right_value() ? std::min(I.right_bracket(), J.right_bracket()) :
                                                             I.right_bracket()
                    )
                );
                auto right_remainder = [&]() {
                    if (interval_intersection.empty()) {
                        return I.left_value() < J.left_value() ? J : I;
                    }
                    return Interval<RealType>(
                        interval_intersection.right_bracket() == ')' ? '[' : '(',
                        interval_intersection.right_value(),
                        std::max(I.right_value(), J.right_value()),
                        (
                            I.right_value() < J.right_value()  ? J.right_bracket() :
                            I.right_value() == J.right_value() ? std::max(I.right_bracket(), J.right_bracket()) :
                                                                 I.right_bracket()
                        )
                    );
                }();
                return std::make_pair(interval_intersection, right_remainder);
            }

            static bool canonicalise_interval_union(Interval<RealType>& I, const Interval<RealType>& J) {
                // If I and J are distinct, return true and leave I unchanged, otherwise return false and set I to IUJ.

                if (
                    (I.left_value() < J.right_value() && J.left_value() < I.right_value()) ||
                    (I.right_value() == J.left_value() && (I.right_bracket() == ']' || J.left_bracket() == '[')) ||
                    (J.right_value() == I.left_value() && (J.right_bracket() == ']' || I.left_bracket() == '['))
                ) {
                    I.left_bracket_m = (
                        I.left_value() < J.left_value()  ? I.left_bracket() :
                        I.left_value() == J.left_value() ? std::max(I.left_bracket(), J.left_bracket()) :
                                                           J.left_bracket()
                    );
                    I.left_value_m = std::min(I.left_value(), J.left_value());
                    
                    I.right_bracket_m = (
                        I.right_value() < J.right_value()  ? J.right_bracket() :
                        I.right_value() == J.right_value() ? std::max(I.right_bracket(), J.right_bracket()) :
                                                             I.right_bracket()
                    );
                    I.right_value_m = std::max(I.right_value(), J.right_value());

                    return false;
                }

                return true;
            }

            void canonicalise_sorted_unempty_intervals(void) {                  
                auto writing_iter = intervals.begin();
                for (auto reading_iter = intervals.cbegin() + 1; reading_iter < intervals.cend(); ++reading_iter) {
                    if (canonicalise_interval_union(*writing_iter, *reading_iter)) {
                        *(++writing_iter) = *reading_iter;
                    }
                }
                intervals.erase(writing_iter, intervals.end());
            }

            void canonicalise_unempty_intervals(void) {
                std::sort(
                    intervals.begin(),
                    intervals.end(),
                    [](const Interval<RealType>& I, const Interval<RealType>& J) {
                        return (
                            I.left_value() < J.left_value() ||
                            I.left_value() == J.left_value() && I.left_bracket() == '[' && J.left_bracket() == '('
                        );
                    }
                );
                canonicalise_sorted_unempty_intervals();
            }
    };

    template<class RealType>
    std::ostream& operator<<(std::ostream& os, const libp::IntervalUnion<RealType>& A) {
        for (auto iter = A.cbegin(); iter != A.cend(); ++iter) {
            os << *iter;
        }
        return os;
    }

}

#endif
