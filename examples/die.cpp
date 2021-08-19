#include <iostream>
#include <libp/libp.hpp>

using namespace libp;

int main(void) {
    auto P_die = UniformDistribution<FiniteSet<int>>({1,2,3,4,5,6});
    std::cout << "P_die({1,2,3}) == " << P_die({1,2,3}) << std::endl;
    // std::cout << "P_die({1,2,3,4}|{3,4,5,6}) == " << P_die({1,2,3,4}|{3,4,5,6}) << std::endl;
    return 0;
}

