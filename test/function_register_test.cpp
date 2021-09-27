#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>
#include <libp/internal/function_register.hpp>

namespace bdata = boost::unit_test::data;

uint64_t cantor_pairing_function(uint64_t k1, uint64_t k2) {
    auto sum = k1 + k2;
    return (sum*(sum + 1))/2 + k2;
}

template<class Iter>
uint64_t cantor_tuple_function(uint64_t so_far, Iter remaining_args_begin, Iter remaining_args_end) {
    for (auto iter = remaining_args_begin; iter != remaining_args_end; ++iter) {
        so_far = cantor_pairing_function(so_far, *iter);
    }
    return so_far;
}

template<class Iter>
uint64_t cantor_tuple_function(Iter args_begin, Iter args_end) {
    return args_begin < args_end ? cantor_tuple_function(*args_begin, args_begin + 1, args_end) : std::numeric_limits<uint64_t>::max();
}

void inv_cantor_pairing_function(uint64_t& x, uint64_t& y, uint64_t z) {
    uint64_t w = std::llround(std::floor(0.5*std::sqrt(8*z + 1) - 0.5));
    y = z - (w*(w+1))/2;
    x = w - y;
}

template<class Iter>
void inv_cantor_tuple_function(uint64_t z, Iter args_begin, Iter args_end) {
    auto args_rbegin = std::make_reverse_iterator(args_end);
    auto args_rend = std::make_reverse_iterator(args_begin);
    for (auto iter = args_rbegin; iter + 1 < args_rend; ++iter) {
        inv_cantor_pairing_function(z, *iter, z);
    }
    if (args_begin != args_end) { *args_begin = z; }
}

struct Constant {
    uint64_t c;
    Constant(): c(std::numeric_limits<uint64_t>::max()) { }
    Constant(uint64_t c_in): c(c_in) { }
    uint64_t operator()(void) { return c; }
    bool operator==(const Constant& rhs) { return c == rhs.c; }
    bool operator!=(const Constant& rhs) { return !(*this == rhs); }
};       

BOOST_DATA_TEST_CASE(
    cantor_tuple_function_test,
    bdata::make({100}) * bdata::make({1, 2, 3}),
    max_arg, dimension
) {
    std::vector<uint64_t> args(dimension, max_arg);

    uint64_t max_z = 1;
    for (uint64_t i = 0; i != dimension; ++i) {
        max_z *= max_arg;
    }

    for (uint64_t z = 0; z <= max_z; ++z) {
        inv_cantor_tuple_function(z, args.begin(), args.end());
        auto z_check = cantor_tuple_function(args.cbegin(), args.cend());
        BOOST_TEST(z == z_check); if (z != z_check) break;
   }
}

template<class T, std::size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<T, N>& arr) {
    auto iter = arr.begin();
    auto end = arr.end();
    os << "{";
    if (iter != end) {
        os << *iter;
        for (++iter; iter != end; ++iter) {
            os << ", " << *iter;
        }
    }
    os << "}";
    return os;
}

template<uint64_t Dimension>
void function_register_test_nested(uint64_t max_arg) {
    std::array<uint64_t, Dimension> args;
    
    uint64_t max_z = 1;
    for (uint64_t i = 0; i != Dimension; ++i) {
        max_z *= max_arg;
    }

    for (auto& s : args) { s = max_arg; }
    libp::internal::FunctionRegister<void(*)(void), Dimension> function_register_null(args);
    for (uint64_t z = 0; z <= max_z; ++z) {
        inv_cantor_tuple_function(z, args.begin(), args.end());
        auto fn = function_register_null.get_function(args);
        BOOST_TEST(fn == nullptr);
        if (fn != nullptr) break;
    }

    libp::internal::FunctionRegister<Constant, Dimension> function_register;

    std::vector<uint64_t> z = [max_z]() {
        using namespace std::chrono;

        std::vector<uint64_t> z_nested; z_nested.reserve(max_z + 1);
        for (uint64_t zi = 0; zi <= max_z; ++zi) {
            z_nested.push_back(zi);
        }
        
        auto ms_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        auto time_based_seed = static_cast<unsigned>(ms_since_epoch & 0xffffffffull);
        std::random_device rd;
        std::mt19937_64 g(rd() ^ time_based_seed);
        std::shuffle(z_nested.begin(), z_nested.end(), g);

        return z_nested;
    }();

    for (auto iter = z.cbegin(); iter != z.cend(); ++iter) {
        uint64_t zi = *iter;
        inv_cantor_tuple_function(zi, args.begin(), args.end());
        function_register.register_function(args, Constant(zi));
    }

    for (uint64_t z = 0; z <= max_z; ++z) {
        inv_cantor_tuple_function(z, args.begin(), args.end());
        uint64_t execute_function_args = function_register.execute_function(args);
        BOOST_TEST(z == execute_function_args);
        if (z != execute_function_args) {
            if (execute_function_args == std::numeric_limits<uint64_t>::max()) {
                std::cerr << "Expected " << z << " at " << args << ", which was empty." << std::endl;
            } else {
                std::array<uint64_t, Dimension> orig_args;
                inv_cantor_tuple_function(execute_function_args, orig_args.begin(), orig_args.end());
                std::cerr << execute_function_args << " at " << orig_args << " moved to " << args << ", where we expected " << z << '.' << std::endl;
            }
            break;
        }
    }
}


BOOST_DATA_TEST_CASE(
    function_register_test,
    bdata::make({100}) * bdata::make({1, 2, 3}),
    max_arg, dimension
) {
    switch(dimension) {
        case 1: function_register_test_nested<1>(max_arg); break;
        case 2: function_register_test_nested<2>(max_arg); break;
        case 3: function_register_test_nested<3>(max_arg); break;
        case 4: function_register_test_nested<4>(max_arg); break;
        case 5: function_register_test_nested<5>(max_arg); break;
        default: throw std::logic_error("Dimension not covered in case statement.");
    }
}

