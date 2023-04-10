#define BOOST_TEST_DYN_LINK

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include <boost/test/unit_test.hpp>
#include <libp/sets/interval.hpp>

BOOST_AUTO_TEST_CASE(simple_interval_test) {
    BOOST_TEST(libp::Interval('(',1.0,-1.0,')') == libp::Interval('(',0.0,0.0,')'));
    
    auto nan_interval = libp::Interval<double>::nan();
    BOOST_TEST(!(nan_interval == nan_interval));
    BOOST_TEST(!(nan_interval != nan_interval));

    libp::IntervalUnion<double> A = {{'(',-1.0,0.0,']'}, {'(',0.0,1.0,']'}};
    libp::IntervalUnion<double> B = {{'(',-1.0,1.0,']'}};
    BOOST_TEST(A == B);

    libp::IntervalUnion<double> C = {{'(',-2.0,-1.0,']'}, {'(',1.0,2.0,']'}};
    std::stringstream ss1; ss1 << C;
    libp::IntervalUnion<double> D; ss1 >> D;
    BOOST_TEST(C == D);
}

template<libp::BoundaryConcept B>
bool complex_interval_test_impl(int n);

template<libp::BoundaryConcept BA, libp::BoundaryConcept BB>
bool complex_interval_test_impl(int n);

template<libp::BoundaryConcept BA, libp::BoundaryConcept BB, libp::BoundaryConcept BC>
bool complex_interval_test_impl(int n);

template<libp::BoundaryConcept BA, libp::BoundaryConcept BB, libp::BoundaryConcept BC, libp::BoundaryConcept... Tail>
bool complex_interval_test_impl(int n);

BOOST_AUTO_TEST_CASE(complex_interval_test) {
    constexpr auto n = 100; // The number of randomly generated set triplets to test for each possible set type triple
                            // given the list in the template below. The triplets being, for example, (float, float, float),
                            // (float, float, double), (float, double, float), etc. It might be worth increasing this
                            // value when testing new functionality.
    complex_interval_test_impl<float, double>(n);
}

template<libp::BoundaryConcept BoundaryA, libp::BoundaryConcept BoundaryB, libp::BoundaryConcept BoundaryC>
struct SetPairDist {
    static_assert(sizeof(BoundaryA) <= sizeof(uint64_t));
    static_assert(sizeof(BoundaryB) <= sizeof(uint64_t));
    static_assert(sizeof(BoundaryC) <= sizeof(uint64_t));

    std::default_random_engine eng{
        static_cast<std::random_device::result_type>(
            std::random_device{}() ^ std::chrono::high_resolution_clock::now().time_since_epoch().count()
        )
    };

    std::poisson_distribution<> interval_count_dist;

    static constexpr int finite_boundary = 0;
    static constexpr int inf_boundary = 1;
    static constexpr int nan_boundary = 2;
    static constexpr int repeat_boundary = 3;
    std::discrete_distribution<> boundary_finiteness_dist = {
        0.75, // finite
        0.04, // (+/-)inf
        0.01, // nan
        0.2, // repeated boundary
    };

    std::bernoulli_distribution pos_inf_dist{0.5};

    std::discrete_distribution<> repeat_dist{1.0/3, 1.0/3, 1.0/3};

    std::uniform_int_distribution<uint64_t> finite_boundaries_dist;

    std::bernoulli_distribution closed_bracket_dist{0.5};

    template<std::floating_point T>
    auto isfinite(T t) { return std::isfinite(t); }

    template<libp::BoundaryConcept BoundaryThird, libp::BoundaryConcept BoundarySecond, libp::BoundaryConcept BoundaryFirst>
    auto draw_third_boundaries(
        std::vector<uint64_t>& third_boundaries,
        const std::vector<uint64_t>& second_boundaries,
        const std::vector<uint64_t>& first_boundaries
    ) {
        static constexpr auto neg_inf = -std::numeric_limits<BoundaryThird>::infinity();
        static constexpr auto pos_inf = std::numeric_limits<BoundaryThird>::infinity();
        static constexpr auto nan = std::numeric_limits<BoundaryThird>::quiet_NaN();
        third_boundaries.clear();
        auto interval_count = interval_count_dist(eng);
        auto boundary_count = std::max(2*interval_count, 2*(interval_count/2));
        third_boundaries.reserve(boundary_count);
        for (decltype(boundary_count) i = 0; i != boundary_count; ++i) {
            uint64_t boundary_third_uint64 = 0;
            switch (boundary_finiteness_dist(eng)) {
                case finite_boundary:
                    BoundaryThird boundary_third;
                    do {
                        boundary_third_uint64 = finite_boundaries_dist(eng);
                        std::memcpy(&boundary_third, &boundary_third_uint64, sizeof(BoundaryThird));
                    } while (!isfinite(boundary_third));
                    third_boundaries.push_back(boundary_third_uint64);
                    break;
                case inf_boundary:
                    std::memcpy(&boundary_third_uint64, pos_inf_dist(eng) ? &pos_inf : &neg_inf, sizeof(BoundaryThird));
                    third_boundaries.push_back(boundary_third_uint64);
                    break;
                case nan_boundary:
                    std::memcpy(&boundary_third_uint64, &nan, sizeof(BoundaryThird));
                    third_boundaries.push_back(boundary_third_uint64);
                    break;
                case repeat_boundary:
                    auto to_repeat = repeat_dist(eng) + 1;
                    if (to_repeat == 1) {
                        if (first_boundaries.empty()) {
                            to_repeat = 2;
                        } else {
                            BoundaryFirst boundary_first;
                            std::memcpy(&boundary_first, &first_boundaries.at(finite_boundaries_dist(eng) % first_boundaries.size()), sizeof(BoundaryFirst));
                            boundary_third = boundary_first;
                            std::memcpy(&boundary_third_uint64, &boundary_third, sizeof(BoundaryThird));
                            third_boundaries.push_back(boundary_third_uint64);
                        }
                    }
                    if (to_repeat == 2) {
                        if (second_boundaries.empty()) {
                            to_repeat = 3;
                        } else {
                            BoundarySecond boundary_second;
                            std::memcpy(&boundary_second, &second_boundaries.at(finite_boundaries_dist(eng) % second_boundaries.size()), sizeof(BoundarySecond));
                            boundary_third = boundary_second;
                            std::memcpy(&boundary_third_uint64, &boundary_third, sizeof(BoundaryThird));
                            third_boundaries.push_back(boundary_third_uint64);
                        }
                    }
                    if (to_repeat == 3) {
                        if (third_boundaries.empty()) {
                            --i;
                            continue;
                        } else {
                            third_boundaries.push_back(third_boundaries.back());
                        }
                    }
                    break;
            }
        }
    }

    template<libp::BoundaryConcept B>
    auto draw_set_from_boundaries(std::vector<uint64_t>& boundaries_uint64) {
        std::sort(boundaries_uint64.begin(), boundaries_uint64.end());
        
        std::vector<libp::Interval<B>> intervals; intervals.reserve(boundaries_uint64.size()/2);
        for (decltype(boundaries_uint64.size()) i = 0; i+1 < boundaries_uint64.size(); i += 2) {
            B left_value; std::memcpy(&left_value, &boundaries_uint64.at(i), sizeof(B));
            B right_value; std::memcpy(&right_value, &boundaries_uint64.at(i+1), sizeof(B));
            intervals.emplace_back(
                closed_bracket_dist(eng) ? '[' : '(',
                left_value,
                right_value,
                closed_bracket_dist(eng) ? ']' : ')'
            );
        }

        return libp::IntervalUnion<B>(intervals.begin(), intervals.end());
    }

    auto operator()(void) {
        std::vector<uint64_t> boundaries_A, boundaries_B, boundaries_C;

        draw_third_boundaries<BoundaryA, BoundaryC, BoundaryB>(boundaries_A, boundaries_C, boundaries_B);
        auto A = draw_set_from_boundaries<BoundaryA>(boundaries_A);

        draw_third_boundaries<BoundaryB, BoundaryA, BoundaryC>(boundaries_B, boundaries_A, boundaries_C);
        auto B = draw_set_from_boundaries<BoundaryB>(boundaries_B);

        draw_third_boundaries<BoundaryC, BoundaryB, BoundaryA>(boundaries_C, boundaries_B, boundaries_A);
        auto C = draw_set_from_boundaries<BoundaryC>(boundaries_C);

        return std::tuple{A,B,C};
    }
};

template<libp::BoundaryConcept BoundaryA, libp::BoundaryConcept BoundaryB, libp::BoundaryConcept BoundaryC>
bool complex_interval_test_impl_fixed_boundary_types(int n) {
    // The input n is the number of randomly generated set triplets.

    auto test_set = [&](const auto& A) {
        constexpr auto inf = std::numeric_limits<typename std::decay_t<decltype(A)>::boundary_type>::infinity();

        auto O = libp::IntervalUnion<typename std::decay_t<decltype(A)>::boundary_type>::empty();
        auto C = A.inv(true);
        libp::IntervalUnion U('[',-inf,inf,']');

        std::stringstream ss;
        ss.precision(std::numeric_limits<typename std::decay_t<decltype(A)>::boundary_type>::max_digits10);
        ss << A;
        std::decay_t<decltype(A)> A_streamed; ss >> A_streamed;

        bool pass;
        bool pass_all = true;
        if (A.isnan()) {
            BOOST_TEST(pass = A_streamed.isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = !(A == A)); pass_all = pass_all && pass;
            BOOST_TEST(pass = !(A != A)); pass_all = pass_all && pass;
            BOOST_TEST(pass = !(A == C)); pass_all = pass_all && pass;
            BOOST_TEST(pass = !(A != C)); pass_all = pass_all && pass;
            BOOST_TEST(pass = !(C == A)); pass_all = pass_all && pass;
            BOOST_TEST(pass = !(C != A)); pass_all = pass_all && pass;
            BOOST_TEST(pass = !(C == C)); pass_all = pass_all && pass;
            BOOST_TEST(pass = !(C != C)); pass_all = pass_all && pass;
            BOOST_TEST(pass = C.isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = A.inv(false).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = C.inv(true).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = C.inv(false).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (O||A).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (O&&A).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A||O).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A&&O).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A||A).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A&&A).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A||C).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A&&C).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A||U).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A&&U).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (C||O).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (C&&O).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (C||A).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (C&&A).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (C||C).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (C&&C).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (C||U).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (C&&U).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (U||A).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (U&&A).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (U||C).isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = (U&&C).isnan()); pass_all = pass_all && pass;
        } else {
            BOOST_TEST(pass = A_streamed == A); pass_all = pass_all && pass;
            BOOST_TEST(pass = A == C.inv(true)); pass_all = pass_all && pass;
            BOOST_TEST(pass = C.inv(true) == A); pass_all = pass_all && pass;
            BOOST_TEST(pass = (O||A) == A); pass_all = pass_all && pass;
            BOOST_TEST(pass = (O&&A) == O); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A||O) == A); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A&&O) == O); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A||A) == A); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A&&A) == A); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A||C) == U); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A&&C) == O); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A||U) == U); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A&&U) == A); pass_all = pass_all && pass;
            BOOST_TEST(pass = (C||O) == C); pass_all = pass_all && pass;
            BOOST_TEST(pass = (C&&O) == O); pass_all = pass_all && pass;
            BOOST_TEST(pass = (C||A) == U); pass_all = pass_all && pass;
            BOOST_TEST(pass = (C&&A) == O); pass_all = pass_all && pass;
            BOOST_TEST(pass = (C||C) == C); pass_all = pass_all && pass;
            BOOST_TEST(pass = (C&&C) == C); pass_all = pass_all && pass;
            BOOST_TEST(pass = (C||U) == U); pass_all = pass_all && pass;
            BOOST_TEST(pass = (C&&U) == C); pass_all = pass_all && pass;
            BOOST_TEST(pass = (U||A) == U); pass_all = pass_all && pass;
            BOOST_TEST(pass = (U&&A) == A); pass_all = pass_all && pass;
            BOOST_TEST(pass = (U||C) == U); pass_all = pass_all && pass;
            BOOST_TEST(pass = (U&&C) == C); pass_all = pass_all && pass;
            if (!(A(-inf) || A(inf))) {
                C = A.inv(false);
                U = libp::IntervalUnion('(',-inf,inf,')');
                BOOST_TEST(pass = A == C.inv(false)); pass_all = pass_all && pass;
                BOOST_TEST(pass = C.inv(false) == A); pass_all = pass_all && pass;
                BOOST_TEST(pass = (A||C) == U); pass_all = pass_all && pass;
                BOOST_TEST(pass = (A&&C) == O); pass_all = pass_all && pass;
                BOOST_TEST(pass = (A||U) == U); pass_all = pass_all && pass;
                BOOST_TEST(pass = (A&&U) == A); pass_all = pass_all && pass;
                BOOST_TEST(pass = (C||O) == C); pass_all = pass_all && pass;
                BOOST_TEST(pass = (C&&O) == O); pass_all = pass_all && pass;
                BOOST_TEST(pass = (C||A) == U); pass_all = pass_all && pass;
                BOOST_TEST(pass = (C&&A) == O); pass_all = pass_all && pass;
                BOOST_TEST(pass = (C||C) == C); pass_all = pass_all && pass;
                BOOST_TEST(pass = (C&&C) == C); pass_all = pass_all && pass;
                BOOST_TEST(pass = (C||U) == U); pass_all = pass_all && pass;
                BOOST_TEST(pass = (C&&U) == C); pass_all = pass_all && pass;
                BOOST_TEST(pass = (U||A) == U); pass_all = pass_all && pass;
                BOOST_TEST(pass = (U&&A) == A); pass_all = pass_all && pass;
                BOOST_TEST(pass = (U||C) == U); pass_all = pass_all && pass;
                BOOST_TEST(pass = (U&&C) == C); pass_all = pass_all && pass;
            }
        }

        return pass_all;
    };

    auto test_set_pair = [&](const auto& A, const auto& B) {
        constexpr auto infA = std::numeric_limits<typename std::decay_t<decltype(A)>::boundary_type>::infinity();
        constexpr auto infB = std::numeric_limits<typename std::decay_t<decltype(B)>::boundary_type>::infinity();
        auto AorB = A || B;
        auto AandB = A && B;
        bool pass;
        bool pass_all = true;
        if (A.isnan() || B.isnan()) {
            BOOST_TEST(pass = AorB.isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = AandB.isnan()); pass_all = pass_all && pass;
        } else {
            BOOST_TEST(pass = AorB == (B||A)); pass_all = pass_all && pass;
            BOOST_TEST(pass = AandB == (B&&A)); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A||AandB) == A); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A&&AorB) == A); pass_all = pass_all && pass;
            BOOST_TEST(pass = AorB.inv(true) == (A.inv(true) && B.inv(true))); pass_all = pass_all && pass;
            BOOST_TEST(pass = AandB.inv(true) == (A.inv(true) || B.inv(true))); pass_all = pass_all && pass;
            if (!(A(-infA) || A(infA) || B(-infB) || B(-infB))) {
                BOOST_TEST(pass = AorB.inv(false) == (A.inv(false) && B.inv(false))); pass_all = pass_all && pass;
                BOOST_TEST(pass = AandB.inv(false) == (A.inv(false) || B.inv(false))); pass_all = pass_all && pass;
            }
            BOOST_TEST(pass = ((AandB == A) == (AorB == B))); pass_all = pass_all && pass;
        }
        return pass_all;
    };

    auto test_set_triple = [&](const auto& A, const auto& B, const auto& C) {
        auto AorB = A || B;
        auto AandB = A && B;
        auto AorC = A || C;
        auto AandC = A && C;
        auto BorC = B || C;
        auto BandC = B && C;
        bool pass;
        bool pass_all = true;
        if (A.isnan() || B.isnan()) {
            BOOST_TEST(pass = AorB.isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = AandB.isnan()); pass_all = pass_all && pass;           
        }
        if (A.isnan() || C.isnan()) {
            BOOST_TEST(pass = AorC.isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = AandC.isnan()); pass_all = pass_all && pass;
        }
        if (B.isnan() || C.isnan()) {
            BOOST_TEST(pass = BorC.isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = BandC.isnan()); pass_all = pass_all && pass;
        }
        if (!(A.isnan() || B.isnan() || C.isnan())) {
            BOOST_TEST(pass = (AorB||C) == (A||BorC)); pass_all = pass_all && pass;
            BOOST_TEST(pass = (AandB&&C) == (A&&BandC)); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A||BandC) == (AorB&&AorC)); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A&&BorC) == (AandB||AandC)); pass_all = pass_all && pass;
        }
        return pass_all;
    };

    auto test_sets_pair_triple = [&](const auto& A, const auto& B, const auto& C) {
        bool pass_all = true;

        pass_all = test_set(A) && pass_all;
        pass_all = test_set(B) && pass_all;
        pass_all = test_set(C) && pass_all;

        pass_all = test_set_pair(A,A) && pass_all;
        pass_all = test_set_pair(A,B) && pass_all;
        pass_all = test_set_pair(A,C) && pass_all;
        pass_all = test_set_pair(B,A) && pass_all;
        pass_all = test_set_pair(B,B) && pass_all;
        pass_all = test_set_pair(B,C) && pass_all;
        pass_all = test_set_pair(C,A) && pass_all;
        pass_all = test_set_pair(C,B) && pass_all;
        pass_all = test_set_pair(C,C) && pass_all;

        pass_all = test_set_triple(A,A,A) && pass_all;
        pass_all = test_set_triple(A,A,B) && pass_all;
        pass_all = test_set_triple(A,A,C) && pass_all;
        pass_all = test_set_triple(A,B,A) && pass_all;
        pass_all = test_set_triple(A,B,B) && pass_all;
        pass_all = test_set_triple(A,B,C) && pass_all;
        pass_all = test_set_triple(A,C,A) && pass_all;
        pass_all = test_set_triple(A,C,B) && pass_all;
        pass_all = test_set_triple(A,C,C) && pass_all;
        pass_all = test_set_triple(B,A,A) && pass_all;
        pass_all = test_set_triple(B,A,B) && pass_all;
        pass_all = test_set_triple(B,A,C) && pass_all;
        pass_all = test_set_triple(B,B,A) && pass_all;
        pass_all = test_set_triple(B,B,B) && pass_all;
        pass_all = test_set_triple(B,B,C) && pass_all;
        pass_all = test_set_triple(B,C,A) && pass_all;
        pass_all = test_set_triple(B,C,B) && pass_all;
        pass_all = test_set_triple(B,C,C) && pass_all;
        pass_all = test_set_triple(C,A,A) && pass_all;
        pass_all = test_set_triple(C,A,B) && pass_all;
        pass_all = test_set_triple(C,A,C) && pass_all;
        pass_all = test_set_triple(C,B,A) && pass_all;
        pass_all = test_set_triple(C,B,B) && pass_all;
        pass_all = test_set_triple(C,B,C) && pass_all;
        pass_all = test_set_triple(C,C,A) && pass_all;
        pass_all = test_set_triple(C,C,B) && pass_all;
        pass_all = test_set_triple(C,C,C) && pass_all;

        return pass_all;
    };

    libp::IntervalUnion<BoundaryA> A;
    libp::IntervalUnion<BoundaryB> B;
    libp::IntervalUnion<BoundaryC> C;
    libp::IntervalUnion<double> D;

    bool pass = true;

    std::filesystem::path test_cases_path = "interval_test_cases.txt";
    {
        std::ifstream test_cases{test_cases_path};
        while (true) {
            test_cases >> D; A = D;
            if (test_cases.eof()) { break; }
            BOOST_TEST(test_cases.good());
            if (!test_cases.good()) { pass = false; break; }

            test_cases >> D; B = D;
            BOOST_TEST(test_cases.good());
            if (!test_cases.good()) { pass = false; break; }
            
            test_cases >> D; C = D;
            BOOST_TEST(test_cases.good());
            if (!test_cases.good()) { pass = false; break; }

            pass = test_sets_pair_triple(A,B,C);
        }
    }

    if (pass) {
        SetPairDist<BoundaryA, BoundaryB, BoundaryC> set_triplet_dist;
        while (n-- && pass) {
            auto [A,B,C] = set_triplet_dist();
            pass = test_sets_pair_triple(A,B,C);
            if (!pass) {
                std::ofstream test_cases{test_cases_path, std::ios_base::out | std::ios_base::app};
                test_cases.precision(std::numeric_limits<double>::max_digits10);
                D = A; test_cases << D;
                D = B; test_cases << D;
                D = C; test_cases << D;
                test_cases << std::endl;
            }
        }
    }

    return pass;
}

template<libp::BoundaryConcept B>
bool complex_interval_test_impl(int n) {
    return complex_interval_test_impl_fixed_boundary_types<B, B, B>(n);
}

template<libp::BoundaryConcept BA, libp::BoundaryConcept BB>
bool complex_interval_test_impl(int n) {
    bool pass = true;
    pass = pass && complex_interval_test_impl_fixed_boundary_types<BA, BA, BA>(n);
    pass = pass && complex_interval_test_impl_fixed_boundary_types<BA, BA, BB>(n);
    pass = pass && complex_interval_test_impl_fixed_boundary_types<BA, BB, BB>(n);
    pass = pass && complex_interval_test_impl_fixed_boundary_types<BB, BB, BB>(n);
    return pass;
}

template<libp::BoundaryConcept BA, libp::BoundaryConcept BB, libp::BoundaryConcept BC>
bool complex_interval_test_impl(int n) {
    bool pass = true;
    pass = pass && complex_interval_test_impl_fixed_boundary_types<BA, BA, BA>(n);
    pass = pass && complex_interval_test_impl_fixed_boundary_types<BA, BA, BB>(n);
    pass = pass && complex_interval_test_impl_fixed_boundary_types<BA, BA, BC>(n);
    pass = pass && complex_interval_test_impl_fixed_boundary_types<BA, BB, BB>(n);
    pass = pass && complex_interval_test_impl_fixed_boundary_types<BA, BB, BC>(n);
    pass = pass && complex_interval_test_impl_fixed_boundary_types<BA, BC, BC>(n);
    pass = pass && complex_interval_test_impl_fixed_boundary_types<BB, BB, BB>(n);
    pass = pass && complex_interval_test_impl_fixed_boundary_types<BB, BB, BC>(n);
    pass = pass && complex_interval_test_impl_fixed_boundary_types<BB, BC, BC>(n);
    pass = pass && complex_interval_test_impl_fixed_boundary_types<BC, BC, BC>(n);
    return pass;
}

template<libp::BoundaryConcept BA, libp::BoundaryConcept BB, libp::BoundaryConcept BC, libp::BoundaryConcept... Tail>
bool complex_interval_test_impl(int n) {
    bool pass = true;
    pass = pass && complex_interval_test_impl<BA, BB, BC>(n);
    pass = pass && complex_interval_test_impl<BA, BB, Tail...>(n);
    pass = pass && complex_interval_test_impl<BA, BC, Tail...>(n);
    pass = pass && complex_interval_test_impl<BB, BC, Tail...>(n);
    return pass;
}
