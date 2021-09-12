#include <boost/test/unit_test.hpp>
#include <libp/libp.hpp>

BOOST_AUTO_TEST_CASE(real_sets_relationships_test) {
    auto O = libp::none();
    auto N = libp::all<unsigned>();
    auto Z = libp::all<int>();
    auto R = libp::all<double>();
    auto U = libp::all();

    BOOST_TEST((O&&O)==O);
    BOOST_TEST((O&&N)==O);
    BOOST_TEST((O&&Z)==O);
    BOOST_TEST((O&&R)==O);
    BOOST_TEST((O&&U)==O);
    BOOST_TEST((N&&N)==N);
    BOOST_TEST((N&&Z)==N);
    BOOST_TEST((N&&R)==N);
    BOOST_TEST((N&&U)==N);
    BOOST_TEST((Z&&Z)==Z);
    BOOST_TEST((Z&&R)==Z);
    BOOST_TEST((Z&&U)==Z);
    BOOST_TEST((R&&R)==R);
    BOOST_TEST((R&&U)==R);
    BOOST_TEST((U&&U)==U);

    BOOST_TEST((O||O)==O);
    BOOST_TEST((O||N)==N);
    BOOST_TEST((O||Z)==Z);
    BOOST_TEST((O||R)==R);
    BOOST_TEST((O||U)==U);
    BOOST_TEST((N||N)==N);
    BOOST_TEST((N||Z)==Z);
    BOOST_TEST((N||R)==R);
    BOOST_TEST((N||U)==U);
    BOOST_TEST((Z||Z)==Z);
    BOOST_TEST((Z||R)==R);
    BOOST_TEST((Z||U)==U);
    BOOST_TEST((R||R)==R);
    BOOST_TEST((R||U)==U);
    BOOST_TEST((U||U)==U);
}

