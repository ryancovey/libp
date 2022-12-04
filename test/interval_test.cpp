#include <limits>
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
}
