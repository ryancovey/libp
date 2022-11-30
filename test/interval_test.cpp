#include <boost/test/unit_test.hpp>
#include <libp/sets/interval.hpp>

BOOST_AUTO_TEST_CASE(simple_interval_test) {
    BOOST_TEST(libp::Interval<double>(libp::open(1.0), libp::open(-1.0)) == libp::Interval<double>());
}
