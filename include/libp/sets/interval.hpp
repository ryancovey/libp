#ifndef LIBP_SETS_INTERVAL_HPP_GUARD
#define LIBP_SETS_INTERVAL_HPP_GUARD

#include <algorithm>
#include <cctype>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <ostream>
#include <iterator>
#include <limits>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace libp {

    template<class Boundary>
    concept BoundaryConcept = requires(Boundary x, Boundary y) {
        requires std::default_initializable<Boundary>;
        requires std::totally_ordered<Boundary>;
        std::numeric_limits<Boundary>::infinity();
        std::numeric_limits<Boundary>::quiet_NaN();
        std::min(x,y);
        std::max(x,y);
    };

    template<BoundaryConcept Boundary>
    class IntervalUnion;

    template<BoundaryConcept Boundary>
    class Interval {
        template<BoundaryConcept B>
        friend class Interval;

        template<BoundaryConcept B>
        friend class IntervalUnion;

        public:
            using boundary_type = Boundary;

            Interval(): 
                left_value_m(0),
                right_value_m(0),
                left_bracket_m('('),
                right_bracket_m(')')
            { }

            template<BoundaryConcept S, BoundaryConcept T>
            Interval(char left_bracket_in, S left_value_in, T right_value_in, char right_bracket_in):
                left_value_m(std::move(left_value_in)),
                right_value_m(std::move(right_value_in)),
                left_bracket_m(left_bracket_in),
                right_bracket_m(right_bracket_in)
            {
                if (
                    (left_bracket_m != '(' && left_bracket_m != '[') ||
                    (right_bracket_m != ')' && right_bracket_m != ']') ||
                    std::isnan(left_value_m) || std::isnan(right_value_m)
                ) {
                    set_to_nan();
                } else if (isempty()) {
                    set_to_empty();
                }
            }

            template<BoundaryConcept RhsBoundary>
            Interval(const Interval<RhsBoundary>& rhs):
                left_value_m(rhs.left_value_m),
                right_value_m(rhs.right_value_m),
                left_bracket_m(rhs.left_bracket_m),
                right_bracket_m(rhs.right_bracket_m)
            { }

            auto left_value(void) const { return left_value_m; }
            auto left_bracket(void) const { return left_bracket_m; }

            auto right_value(void) const { return right_value_m; }
            auto right_bracket(void) const { return right_bracket_m; }

            auto open(void) const { return left_bracket() == '(' && right_bracket() == ')'; }
            auto closed(void) const { return left_bracket() == '[' && right_bracket() == ']'; }

            auto isempty(void) const {
                // Does not assume that we're in a canonical representation, but does assume that the
                // left bracket is '(' or '[', likewise for the right bracket. Returns false if NaN.
                return ((left_value_m == right_value_m && !closed()) || left_value_m > right_value_m);
            }

            auto isnan(void) const { return std::isnan(left_value_m); }

            template<BoundaryConcept BoundaryX>
            auto operator()(const BoundaryX& x) const {
                return (left_value() < x && x < right_value()) ||
                    (x == left_value() && left_bracket() == '[') ||
                    (x == right_value() && right_bracket() == ']');
            }

            template<BoundaryConcept B>
            bool operator==(const Interval<B>& rhs) const {
                return left_bracket_m == rhs.left_bracket_m &&
                    left_value_m == rhs.left_value_m &&
                    right_value_m == rhs.right_value_m &&
                    right_bracket_m == rhs.right_bracket_m;
            }

            template<BoundaryConcept B>
            bool operator!=(const Interval<B>& rhs) const {
                if (isnan() || rhs.isnan()) {
                    return false;
                } else {
                    return !operator==(rhs);
                }
            }

            static Interval<Boundary> empty(void) {
                return {};
            }

            static Interval<Boundary> universal(bool extended_real_line = false) {
                return Interval<Boundary>().inv(extended_real_line);
            }

            static Interval<Boundary> nan(void) {
                Interval<Boundary> ret;
                ret.set_to_nan();
                return ret;
            } 

        private:
            Boundary left_value_m;
            Boundary right_value_m;
            char left_bracket_m;
            char right_bracket_m;

            void set_to_empty(void) {
                left_bracket_m = '(';
                left_value_m = 0;
                right_value_m = 0;
                right_bracket_m = ')';
            }

            void set_to_nan(void) {
                left_bracket_m = '(';
                left_value_m = std::numeric_limits<Boundary>::quiet_NaN();
                right_value_m = std::numeric_limits<Boundary>::quiet_NaN();
                right_bracket_m = ']';
            };
    };

    Interval() -> Interval<double>;

    template<BoundaryConcept S, BoundaryConcept T>
    Interval(char, S, T, char) -> Interval<std::common_type_t<S,T>>;

    template<BoundaryConcept RhsBoundary>
    Interval(const Interval<RhsBoundary>& rhs) -> Interval<RhsBoundary>;

    template<BoundaryConcept Boundary>
    std::ostream& operator<<(std::ostream& os, const libp::Interval<Boundary>& I) {
        os << I.left_bracket() << I.left_value() << ',' << I.right_value() << I.right_bracket();
        return os;
    }

    template<BoundaryConcept Boundary>
    std::istream& operator>>(std::istream& is, libp::Interval<Boundary>& I) {
        auto backtrack_istream_impl = [](std::istream& isn, const std::string& buffer) -> std::istream& {
            isn.clear();
            for (auto i = buffer.size(); i != 0; --i) {
                if (!isn.putback(buffer[i-1])) { return isn; }
            }
            return isn;
        };

        auto backtrack_istream = [&backtrack_istream_impl](std::istream& isn, const std::ostringstream& buffer) -> std::istream& {
            return backtrack_istream_impl(isn, buffer.str());
        };

        auto backtrackable_get_from = [](std::istream& isn, std::ostringstream& buffer, char& c) -> std::istream& {
            if (isn >> c) { buffer << c; }
            return isn;
        };

        auto match = [&backtrack_istream, &backtrackable_get_from](std::istream& isn, const std::string& str) -> std::istream& {
            std::ostringstream buffer;
            char stream_char;
            for (decltype(str.size()) i = 0; i != str.size(); ++i) {
                if (!backtrackable_get_from(isn, buffer, stream_char) || stream_char != str[i]) {
                    backtrack_istream(isn, buffer);
                    isn.setstate(std::ios_base::failbit);
                    return isn;
                }
            }
            return isn;
        };

        auto read_boundary = [&match](std::istream& isn, Boundary& b) -> std::istream& {
            // A minus character will be extracted by isn >> b, which can turn -inf into inf.
            // We capture the next character so that we can put it back later if it turns out
            // to be a minus.
            char maybe_minus;
            if (!(isn >> maybe_minus)) {
                return isn;
            }
            isn.putback(maybe_minus);

            // Try finite boundary.
            if (isn >> b) {
                return isn;
            }

            // Try negatively infinite boundary.
            isn.clear();
            if (maybe_minus == '-') { isn.putback('-'); }
            auto inf = std::numeric_limits<Boundary>::infinity();
            if (std::ostringstream ss; (ss << -inf) && match(isn, ss.str())) {
                b = -inf;
                return isn;
            }

            // Try positively infinite boundary.
            isn.clear();
            if (std::ostringstream ss; (ss << inf) && match(isn, ss.str())) {
                b = inf;
                return isn;
            }

            // Try nan boundary.
            isn.clear();
            auto nan = std::numeric_limits<Boundary>::quiet_NaN();
            if (std::ostringstream ss; (ss << nan) && match(isn, ss.str())) {
                b = nan;
                return isn;
            }

            // Reading failed.
            isn.setstate(std::ios_base::failbit);
            return isn;
        };

        std::ostringstream buffer;

        char left_bracket; if (!(is >> left_bracket)) { return is; }
        if (left_bracket != '(' && left_bracket != '[') {
            is.putback(left_bracket);
            is.setstate(std::ios_base::failbit);
            return is;
        }
        buffer << left_bracket;

        Boundary left_value;
        if (!read_boundary(is, left_value)) {
            backtrack_istream(is, buffer);
            is.setstate(std::ios_base::failbit);
            return is;
        }
        buffer << left_value;

        char comma;
        if (!(is >> comma)) {
            backtrack_istream(is, buffer);
            is.setstate(std::ios_base::failbit);
            return is;
        }
        buffer << comma;
        if (comma != ',') {
            backtrack_istream(is, buffer);
            is.setstate(std::ios_base::failbit);
            return is;
        }

        Boundary right_value;
        if (!read_boundary(is, right_value)) {
            backtrack_istream(is, buffer);
            is.setstate(std::ios_base::failbit);
            return is;
        }
        buffer << right_value;

        char right_bracket;
        if (!(is >> right_bracket)) {
            backtrack_istream(is, buffer);
            is.setstate(std::ios_base::failbit);
            return is;
        }
        buffer << right_bracket;
        if (right_bracket != ')' && right_bracket != ']') {
            backtrack_istream(is, buffer);
            is.setstate(std::ios_base::failbit);
            return is;
        }

        libp::Interval<Boundary> J(left_bracket, left_value, right_value, right_bracket);
        std::swap(I,J);
        
        return is;
    }

    template<BoundaryConcept Boundary>
    class IntervalUnion {
        template<BoundaryConcept B>
        friend class IntervalUnion;

        public:
            using boundary_type = Boundary;

            IntervalUnion() = default;

            template<BoundaryConcept IntBoundary>
            IntervalUnion(Interval<IntBoundary> interval) {
                if (!interval.isempty()) { intervals.emplace_back(std::move(interval)); }
            }

            template<BoundaryConcept S, BoundaryConcept T>
            IntervalUnion(char left_bracket_in, S left_value_in, T right_value_in, char right_bracket_in):
                IntervalUnion(Interval<Boundary>(left_bracket_in, std::move(left_value_in), std::move(right_value_in), right_bracket_in))
            { }

            template<std::forward_iterator Iter>
            IntervalUnion(Iter first, Iter last) {
                intervals.reserve(std::distance(first, last));
                for (auto iter = first; iter != last; ++iter) {
                    const Interval<Boundary>& I = *iter;
                    if (I.isnan()) {
                        intervals.clear();
                        intervals.emplace_back(I);
                        return;
                    } else if (!I.isempty()) {
                        intervals.emplace_back(I);
                    }
                }
                canonicalise_unempty_intervals();
            }

            IntervalUnion(std::initializer_list<Interval<Boundary>> l):
                IntervalUnion(l.begin(), l.end())
            { }

            template<BoundaryConcept RhsBoundary>
            IntervalUnion(const IntervalUnion<RhsBoundary>& rhs):
                IntervalUnion(rhs.cbegin(), rhs.cend())
            { }

            auto cbegin(void) const { return intervals.cbegin(); }
            auto cend(void) const { return intervals.cend(); }

            bool isempty(void) const { return intervals.empty(); }

            bool isnan(void) const { return !isempty() && intervals.front().isnan(); }

            static IntervalUnion<Boundary> empty(void) { return {}; }

            static IntervalUnion<Boundary> universal(bool extended_real_line = false) {
                return IntervalUnion<Boundary>().inv(extended_real_line);
            }

            static IntervalUnion<Boundary> nan(void) { return Interval<Boundary>::nan(); }

            IntervalUnion<Boundary> inv(bool extended_real_line = false) const {
                if (intervals.size() == 0) {
                    return Interval<Boundary>(
                        extended_real_line ? '[' : '(',
                        -std::numeric_limits<Boundary>::infinity(),
                        std::numeric_limits<Boundary>::infinity(),
                        extended_real_line ? ']' : ')'
                    );
                } else {
                    const auto& first_interval = intervals.front();
                    if (first_interval.isnan()) {
                        return *this;
                    } else {
                        auto intervals_size = intervals.size();

                        IntervalUnion<Boundary> complement; complement.intervals.reserve(intervals_size + 1);
                        
                        Interval<Boundary> new_first_interval(
                            extended_real_line ? '[' : '(',
                            -std::numeric_limits<Boundary>::infinity(),
                            first_interval.left_value(),
                            first_interval.left_bracket() == '[' ? ')' : ']'
                        );
                        if (!new_first_interval.isempty()) {
                            complement.intervals.emplace_back(new_first_interval);
                        }

                        for (decltype(intervals_size) i = 0; i != intervals_size-1; ++i) {
                            complement.intervals.emplace_back(
                                intervals[i].right_bracket() == ']' ? '(' : '[',
                                intervals[i].right_value(),
                                intervals[i+1].left_value(),
                                intervals[i+1].left_bracket() == '[' ? ')' : ']'
                            );
                        }

                        const auto& last_interval = intervals.back();
                        Interval<Boundary> new_last_interval(
                            last_interval.right_bracket() == ']' ? '(' : '[',
                            last_interval.right_value(),
                            std::numeric_limits<Boundary>::infinity(),
                            extended_real_line ? ']' : ')'
                        );
                        if (!new_last_interval.isempty()) {
                            complement.intervals.emplace_back(std::move(new_last_interval));
                        }

                        return complement;
                    }
                }
            }

            IntervalUnion<Boundary> operator!() const {
                const auto& front = intervals.front();
                const auto& back = intervals.back();
                return inv(
                    !empty() && (
                        (front.left_value() == -std::numeric_limits<Boundary>::infinity() && front.left_bracket() == '[') ||
                        (back.right_value() == std::numeric_limits<Boundary>::infinity() && back.right_bracket() == ']')
                    )
                );
            }

            template<BoundaryConcept RhsBoundary>
            auto operator&&(const IntervalUnion<RhsBoundary>& rhs) const {
                using CommonBoundary = std::common_type_t<Boundary, RhsBoundary>;
                using CommonInterval = Interval<CommonBoundary>;
                using CommonIntervalUnion = IntervalUnion<CommonBoundary>;
                if (isnan() || rhs.isnan()) { return CommonIntervalUnion::nan(); }
                CommonIntervalUnion intersection;
                if (intervals.size() != 0 && rhs.intervals.size() != 0) {
                    intersection.intervals.reserve(std::max(intervals.size(), rhs.intervals.size()));
                    auto lhs_iter = intervals.cbegin();
                    auto lhs_end = intervals.cend();
                    auto rhs_iter = rhs.intervals.cbegin();
                    auto rhs_end = rhs.intervals.cend();
                    CommonInterval I = *lhs_iter;
                    CommonInterval J = *rhs_iter;
                    while (true) {
                        std::tie(I,J) = interval_intersection_right_remainder(I,J);
                        if (!I.isempty()) { intersection.intervals.emplace_back(std::move(I)); }
                        if (J.isempty()) {
                            if (++lhs_iter == lhs_end || ++rhs_iter == rhs_end) { break; }
                            I = *lhs_iter;
                            J = *rhs_iter;
                        } else if (J.right_value() == rhs_iter->right_value() && J.right_bracket() == rhs_iter->right_bracket()) {
                            if (++lhs_iter == lhs_end) { break; }
                            I = *lhs_iter;
                        } else {
                            if (++rhs_iter == rhs_end) { break; }
                            I = *rhs_iter;
                        }
                    }
                }
                return intersection;
            }

            template<BoundaryConcept RhsBoundary>
            auto operator||(const IntervalUnion<RhsBoundary>& rhs) const {
                using CommonIntervalUnion = IntervalUnion<std::common_type_t<Boundary, RhsBoundary>>;
                if (isnan() || rhs.isnan()) { return CommonIntervalUnion::nan(); }
                CommonIntervalUnion set_union;
                set_union.intervals.reserve(std::max(intervals.size(), rhs.intervals.size()));
                auto lhs_iter = intervals.cbegin();
                auto rhs_iter = rhs.intervals.cbegin();
                while (lhs_iter != intervals.cend() && rhs_iter != rhs.intervals.cend()) {
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

            template<BoundaryConcept RhsBoundary>
            bool operator==(const IntervalUnion<RhsBoundary>& rhs) const {
                if (isnan() || rhs.isnan() || intervals.size() != rhs.intervals.size()) {
                    return false;
                } else {
                    for (decltype(intervals.size()) i = 0; i != intervals.size(); ++i) {
                        if (intervals[i] != rhs.intervals[i]) return false;
                    }
                }
                return true;
            }

            template<BoundaryConcept RhsBoundary>
            bool operator!=(const IntervalUnion<RhsBoundary>& rhs) const {
                if (isnan() || rhs.isnan()) {
                    return false;
                } else {
                    return !operator==(rhs);
                }
            }

            template<BoundaryConcept BoundaryX>
            Boundary operator()(const BoundaryX& x) const {
                auto iter = std::lower_bound(
                    intervals.cbegin(),
                    intervals.cend(),
                    x,
                    [](const Interval<Boundary>& I, const BoundaryX& y) {
                        return I.right_value() < y;
                    }
                );
                return iter == intervals.cend() ? 0 : (*iter)(x);
            }

        private:
            std::vector<Interval<Boundary>> intervals;

            template<BoundaryConcept B>
            static auto interval_intersection_right_remainder(const Interval<B>& I, const Interval<B>& J) {
                auto interval_intersection = Interval<B>(
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
                                                             J.right_bracket()
                    )
                );

                auto right_remainder = [&]() {
                    if (interval_intersection.isempty()) {
                        return I.right_value() <= J.left_value() ? J : I;
                    }
                    return Interval<B>(
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

            template<BoundaryConcept BoundaryI, BoundaryConcept BoundaryJ>
            static bool canonicalise_interval_union(Interval<BoundaryI>& I, const Interval<BoundaryJ>& J) {
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
                if (writing_iter != intervals.end()) {
                    for (auto reading_iter = intervals.cbegin() + 1; reading_iter != intervals.cend(); ++reading_iter) {
                        if (canonicalise_interval_union(*writing_iter, *reading_iter)) {
                            *(++writing_iter) = *reading_iter;
                        }
                    }
                    intervals.erase(++writing_iter, intervals.end());
                }
            }

            void canonicalise_unempty_intervals(void) {
                std::sort(
                    intervals.begin(),
                    intervals.end(),
                    [](const Interval<Boundary>& I, const Interval<Boundary>& J) {
                        return (
                            I.left_value() < J.left_value() ||
                            I.left_value() == J.left_value() && I.left_bracket() == '[' && J.left_bracket() == '('
                        );
                    }
                );
                canonicalise_sorted_unempty_intervals();
            }
    };

    IntervalUnion() -> IntervalUnion<double>;

    template<BoundaryConcept IntBoundary>
    IntervalUnion(Interval<IntBoundary>) -> IntervalUnion<IntBoundary>;

    template<BoundaryConcept S, BoundaryConcept T>
    IntervalUnion(char, S, T, char) -> IntervalUnion<std::common_type_t<S,T>>;

    template<std::forward_iterator Iter>
    IntervalUnion(Iter, Iter) -> IntervalUnion<typename Iter::value_type::boundary_type>;

    template<BoundaryConcept RhsBoundary>
    IntervalUnion(const IntervalUnion<RhsBoundary>&) -> IntervalUnion<RhsBoundary>;

    template<BoundaryConcept Boundary>
    std::ostream& operator<<(std::ostream& os, const libp::IntervalUnion<Boundary>& A) {
        if (A.isempty()) {
            os << libp::Interval<Boundary>('(',0,0,')');
        } else {
            for (auto iter = A.cbegin(); iter != A.cend(); ++iter) {
                os << *iter;
            }
        }
        os << ';';
        return os;
    }

    template<BoundaryConcept Boundary>
    std::istream& operator>>(std::istream& is, libp::IntervalUnion<Boundary>& A) {
        std::vector<libp::Interval<Boundary>> intervals;
        bool isnan = false;
        for (libp::Interval<Boundary> I; is >> I; ) {
            if (!isnan) {
                if (I.isnan()) {
                    isnan = true;
                    intervals.clear();
                }
                if (!I.isempty()) {
                    intervals.emplace_back(std::move(I));
                }
            }
        }

        auto populate_A = [&]() { libp::IntervalUnion<Boundary> B(intervals.cbegin(), intervals.cend()); std::swap(A,B); };

        if (is.eof()) {
            populate_A();
        } else if (is.fail()) {
            is.clear();
            char c;
            if (is >> c && c == ';') {
                populate_A();
            } else {
                is.setstate(std::ios_base::failbit);
            }
        }

        return is;
    }

}

#endif
