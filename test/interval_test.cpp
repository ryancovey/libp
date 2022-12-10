#include <limits>
#include <sstream>
#include <string>
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
    std::stringstream ss; ss << C;
    libp::IntervalUnion<double> D; ss >> D;
    BOOST_TEST(C == D);
}
