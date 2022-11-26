#include <boost/test/unit_test.hpp>
#include <libp/libp.hpp>

BOOST_AUTO_TEST_CASE(simple_interval_test) {
    BOOST_TEST(libp::Interval<double>(libp::open(1.0), libp::open(-1.0)) == libp::Interval<double>());
    // BOOST_TEST(libp::Interval<double>(libp::open(1.0), libp::open(-1.0)) != libp::Interval<double>());
    // Running the opposite test (commented out above) leads to a memory access violation. Might be worth
    // investigating (or might not as we move towards IntervalUnion sets only).
}
