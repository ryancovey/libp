// See "Static library usage variant" here:
// https://www.boost.org/doc/libs/1_69_0/libs/test/doc/html/boost_test/usage_variants.html

#define BOOST_TEST_MODULE libp_measurable_set
#include <boost/test/unit_test.hpp>

#define private public
#include <libp/libp.hpp>
#undef private

using namespace libp;

BOOST_AUTO_TEST_CASE(libp_measurable_set_registration_test) {
    BOOST_TEST(!MeasurableSetCRTP<RealNumbers>::type_registered);
    BOOST_TEST(!MeasurableSetCRTP<Integers>::type_registered);
    BOOST_TEST(!MeasurableSetCRTP<NaturalNumbers>::type_registered);
    for (std::size_t i = 0; i <= 2; ++i) {
        BOOST_TEST(MeasurableSet::default_intersection_register().get_function({i}) == nullptr);
        BOOST_TEST(MeasurableSet::default_union_register().get_function({i}) == nullptr);
        for (std::size_t j = 0; j <= 2; ++j) {
            BOOST_TEST(MeasurableSet::pairwise_intersection_register().get_function({i,j}) == nullptr);
            BOOST_TEST(MeasurableSet::pairwise_union_register().get_function({i,j}) == nullptr);
        }
    }

    auto ridx = MeasurableSetCRTP<RealNumbers>::get_registered_type_index();
    BOOST_TEST(ridx >= 0);
    BOOST_TEST(ridx <= 2);

    BOOST_TEST(MeasurableSetCRTP<RealNumbers>::type_registered);
    BOOST_TEST(!MeasurableSetCRTP<Integers>::type_registered);
    BOOST_TEST(!MeasurableSetCRTP<NaturalNumbers>::type_registered);
    for (std::size_t i = 0; i <= 2; ++i) {
        for (std::size_t j = 0; j <= 2; ++j) {
            BOOST_TEST((i == ridx && j == ridx) == (MeasurableSet::pairwise_intersection_register().get_function({i,j}) != nullptr));
            BOOST_TEST((i == ridx && j == ridx) == (MeasurableSet::pairwise_union_register().get_function({i,j}) != nullptr));
        }
    }

    auto zidx = MeasurableSetCRTP<Integers>::get_registered_type_index();
    BOOST_TEST(zidx >= 0);
    BOOST_TEST(zidx <= 2);
    BOOST_TEST(zidx != ridx);

    auto nidx = 3 - ridx - zidx;

    BOOST_TEST(MeasurableSetCRTP<RealNumbers>::type_registered);
    BOOST_TEST(MeasurableSetCRTP<Integers>::type_registered);
    BOOST_TEST(!MeasurableSetCRTP<NaturalNumbers>::type_registered);
    for (std::size_t i = 0; i <= 2; ++i) {
        for (std::size_t j = 0; j <= 2; ++j) {
            BOOST_TEST((i != nidx && j != nidx) == (MeasurableSet::pairwise_intersection_register().get_function({i,j}) != nullptr));
            BOOST_TEST((i != nidx && j != nidx) == (MeasurableSet::pairwise_intersection_register().get_function({i,j}) != nullptr));
        }
    }

    BOOST_TEST(nidx == MeasurableSetCRTP<NaturalNumbers>::get_registered_type_index());

    BOOST_TEST(MeasurableSetCRTP<RealNumbers>::type_registered);
    BOOST_TEST(MeasurableSetCRTP<Integers>::type_registered);
    BOOST_TEST(MeasurableSetCRTP<NaturalNumbers>::type_registered);
    for (std::size_t i = 0; i <= 2; ++i) {
        for (std::size_t j = 0; j <= 2; ++j) {
            BOOST_TEST(MeasurableSet::pairwise_intersection_register().get_function({i,j}));
            BOOST_TEST(MeasurableSet::pairwise_union_register().get_function({i,j}));
        }
    }

    BOOST_TEST(!MeasurableSetCRTP<NullSet>::type_registered);
    BOOST_TEST(!MeasurableSetCRTP<UniversalSet>::type_registered);
}

