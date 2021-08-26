#include <iostream>
#include <libp/libp.hpp>

using namespace libp;
using std::cout;
using std::endl;

int main(void) {
    auto N = all<unsigned>();
    auto Z = all<int>();
    auto R = all<double>();
    auto U = all();
    cout << "N == " << N << endl;
    cout << "Z == " << Z << endl;
    cout << "R == " << R << endl;
    cout << "U == " << U << endl;

    auto A = finite_set({1,2,3});
    auto B = finite_set({3,4,5,6});
    cout << "A == " << A << endl;
    cout << "B == " << B << endl;
    cout << endl;
}

