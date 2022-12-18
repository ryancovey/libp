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
#include <vector>
#include <boost/test/unit_test.hpp>
#include <libp/sets/interval.hpp>

#include <iostream>

BOOST_AUTO_TEST_CASE(simple_interval_test) {
    BOOST_TEST(libp::Interval('(',1.0,-1.0,')') == libp::Interval('(',0.0,0.0,')'));
    
    auto nan_interval = libp::Interval<double>::nan();
    BOOST_TEST(!(nan_interval == nan_interval));
    BOOST_TEST(!(nan_interval != nan_interval));

    libp::IntervalUnion<double> A = {{'(',-1.0,0.0,']'}, {'(',0.0,1.0,']'}};
    libp::IntervalUnion<double> B = {{'(',-1.0,1.0,']'}};
    BOOST_TEST(A == B);

    libp::IntervalUnion<double> C = {{'(',-2.0,-1.0,']'}, {'(',1.0,2.0,']'}};
    std::stringstream ss; ss << C;
    libp::IntervalUnion<double> D; ss >> D;
    BOOST_TEST(C == D);
}

struct SetPairDist {
    static_assert(sizeof(double) == sizeof(uint64_t));

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

    // These constants are copied into int64_t objects via
    // std::memcpy, so we need to be able to take their address,
    // e.g. &neg_inf.
    static constexpr auto neg_inf = std::numeric_limits<double>::infinity();
    static constexpr auto pos_inf = std::numeric_limits<double>::infinity();
    static constexpr auto nan = std::numeric_limits<double>::quiet_NaN();

    auto draw_third_boundaries(
        std::vector<uint64_t>& third_boundaries,
        const std::vector<uint64_t>& second_boundaries,
        const std::vector<uint64_t>& first_boundaries
    ) {
        third_boundaries.clear();
        auto interval_count = interval_count_dist(eng);
        auto boundary_count = std::max(2*interval_count, 2*(interval_count/2));
        third_boundaries.reserve(boundary_count);
        for (decltype(boundary_count) i = 0; i != boundary_count; ++i) {
            uint64_t boundary;
            switch (boundary_finiteness_dist(eng)) {
                case finite_boundary:
                    double boundary_double;
                    do {
                        boundary = finite_boundaries_dist(eng);
                        std::memcpy(&boundary_double, &boundary, sizeof(double));
                    } while (!std::isfinite(boundary_double));
                    third_boundaries.push_back(boundary);
                    break;
                case inf_boundary:
                    std::memcpy(&boundary, pos_inf_dist(eng) ? &pos_inf : &neg_inf, sizeof(double));
                    third_boundaries.push_back(boundary);
                    break;
                case nan_boundary:
                    std::memcpy(&boundary, &nan, sizeof(double));
                    third_boundaries.push_back(boundary);
                    break;
                case repeat_boundary:
                    auto to_repeat = repeat_dist(eng) + 1;
                    if (to_repeat == 1) {
                        if (first_boundaries.empty()) {
                            to_repeat = 2;
                        } else {
                            third_boundaries.push_back(first_boundaries.at(finite_boundaries_dist(eng) % first_boundaries.size()));
                        }
                    }
                    if (to_repeat == 2) {
                        if (second_boundaries.empty()) {
                            to_repeat = 3;
                        } else {
                            third_boundaries.push_back(second_boundaries.at(finite_boundaries_dist(eng) % second_boundaries.size()));
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

    auto draw_set_from_boundaries(std::vector<uint64_t>& boundaries) {
        std::sort(boundaries.begin(), boundaries.end());
        
        std::vector<libp::Interval<double>> intervals; intervals.reserve(boundaries.size()/2);
        for (decltype(boundaries.size()) i = 0; i+1 < boundaries.size(); i += 2) {
            intervals.emplace_back(
                closed_bracket_dist(eng) ? '[' : '(',
                boundaries.at(i),
                boundaries.at(i+1),
                closed_bracket_dist(eng) ? ']' : ')'
            );
        }

        return libp::IntervalUnion<double>(intervals.begin(), intervals.end());
    }

    auto operator()(void) {
        std::array<std::vector<uint64_t>, 3> boundaries;
        std::array<libp::IntervalUnion<double>, 3> sets;
        for (int i = 0; i != 3; ++i) {
            draw_third_boundaries(boundaries.at(i), boundaries.at((i+2)%3), boundaries.at((i+1)%3));
            sets.at(i) = draw_set_from_boundaries(boundaries.at(i));
        }
        return sets;
    }
};

BOOST_AUTO_TEST_CASE(complex_interval_test) {
    auto n = 100; // Random test cases sample size.

    constexpr auto inf = std::numeric_limits<double>::infinity();

    auto test_set = [&](const auto& A) {
        auto O = libp::IntervalUnion<double>::empty();
        auto C = A.inv(true);
        libp::IntervalUnion<double> U('[',-inf,inf,']');

        bool pass;
        bool pass_all = true;
        if (A.isnan()) {
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
                U = libp::IntervalUnion<double>('(',-inf,inf,')');
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
            if (!(A(-inf) || A(inf) || B(-inf) || B(-inf))) {
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
        if (A.isnan() || B.isnan() || C.isnan()) {
            BOOST_TEST(pass = AorB.isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = AandB.isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = AorC.isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = AandC.isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = BorC.isnan()); pass_all = pass_all && pass;
            BOOST_TEST(pass = BandC.isnan()); pass_all = pass_all && pass;
        } else {
            BOOST_TEST(pass = (AorB||C) == (A||BorC)); pass_all = pass_all && pass;
            BOOST_TEST(pass = (AandB&&C) == (A&&BorC)); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A||BandC) == (AorB&&AorC)); pass_all = pass_all && pass;
            BOOST_TEST(pass = (A&&BorC) == (AandB||AandC)); pass_all = pass_all && pass;
        }
        return pass_all;
    };

    auto test_sets_pair_triple = [&](const std::array<libp::IntervalUnion<double>, 3>& sets) {
        bool pass_all = true;
        for (const auto& A : sets) {
            pass_all = test_set(A) && pass_all;
            for (const auto& B : sets) {
                pass_all = test_set_pair(A,B) && pass_all;
                for (const auto& C : sets) {
                    pass_all = test_set_triple(A,B,C) && pass_all;
                }
            }
        }
        return pass_all;
    };

    std::array<libp::IntervalUnion<double>, 3> sets;
    bool pass = true;

    std::filesystem::path test_cases_path = "interval_test_cases.txt";
    {
        std::ifstream test_cases{test_cases_path};
        while (true) {
            test_cases >> sets.at(0);
            if (test_cases.eof()) { break; }
            BOOST_TEST(test_cases.good());
            if (!test_cases.good()) { pass = false; break; }

            test_cases >> sets.at(1);
            BOOST_TEST(test_cases.good());
            if (!test_cases.good()) { pass = false; break; }
            
            test_cases >> sets.at(2);
            BOOST_TEST(test_cases.good());
            if (!test_cases.good()) { pass = false; break; }

            pass = test_sets_pair_triple(sets);
        }
    }

    if (pass) {
        SetPairDist set_pair_dist;
        while (n-- && pass) {
            sets = set_pair_dist();
            pass = test_sets_pair_triple(sets);
            if (!pass) {
                std::ofstream test_cases{test_cases_path};
                for (const auto& A : sets) { test_cases << A; }
                test_cases << std::endl;
            }
        }
    }
}
