#include <iostream>
#include <libp/libp.hpp>

using namespace libp;

int main(void) {
    // auto P = UniformDistribution<FiniteSet<int>>({1,2,3,4,5,6});
    auto P = uniform_distribution({1,2,3,4,5,6});

    // We can asign events to variables...
    FiniteSet<int> A = {1,2,3};
    FiniteSet<int> B = {3,4,5,6};
    std::cout << "A = {1,2,3}" << std::endl;
    std::cout << "B = {3,4,5,6}" << std::endl;
    std::cout << "P(A) == " << P(A) << std::endl;
    std::cout << "P(B) == " << P(B) << std::endl;
    std::cout << "P(A && B) == " << P(A && B) << std::endl;
    std::cout << "P(A || B) == " << P(A || B) << std::endl;
    std::cout << "P(A|B) == " << P(A|B) << std::endl;
    std::cout << "P(B|A) == " << P(B|A) << std::endl;

    // ...or define them inline...
    std::cout << "P({1,2,3}) == " << P({1,2,3}) << std::endl;
    std::cout << "P({3,4,5,6}) == " << P({3,4,5,6}) << std::endl;
    // ... but not always! C++ infix operators such as "&&", "||", and "|" cannot be used with braced arguments.
    // std::cout << "P({1,2,3} && {3,4,5,6}) == " << P({1,2,3} && {3,4,5,6}) << std::endl;
    // std::cout << "P({1,2,3} || {3,4,5,6}) == " << P({1,2,3} || {3,4,5,6}) << std::endl;
    // std::cout << "P({1,2,3}|{3,4,5,6}) == " << P({1,2,3}|{3,4,5,6}) << std::endl;
    // std::cout << "P({3,4,5,6}|{1,2,3}) == " << P({3,4,5,6}|{1,2,3}) << std::endl;
    // But we can wrap our inline sets in the finite_set function for the same effect.
    std::cout << "P(finite_set({1,2,3}) && finite_set({3,4,5,6})) == " << P(finite_set({1,2,3}) && finite_set({3,4,5,6})) << std::endl;
    std::cout << "P(finite_set({1,2,3}) || finite_set({3,4,5,6})) == " << P(finite_set({1,2,3}) || finite_set({3,4,5,6})) << std::endl;
    std::cout << "P(finite_set({1,2,3})|finite_set({3,4,5,6})) == " << P(finite_set({1,2,3})|finite_set({3,4,5,6})) << std::endl;
    std::cout << "P(finite_set({3,4,5,6})|finite_set({1,2,3})) == " << P(finite_set({3,4,5,6})|finite_set({1,2,3})) << std::endl;

    return 0;
}

