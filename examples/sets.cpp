#include <iostream>
#include <libp/libp.hpp>

using namespace libp;
using std::cout;
using std::endl;

int main(void) {
    auto O = all<void>();
    auto N = all<unsigned>();
    auto Z = all<int>();
    auto R = all<double>();
    auto U = all();
    cout << "O == " << O << endl;
    cout << "N == " << N << endl;
    cout << "Z == " << Z << endl;
    cout << "R == " << R << endl;
    cout << "U == " << U << endl;
    cout << endl;

    const auto cap = u8" \u2229 ";
    const auto cup = u8" \u222A ";

    cout << O << cap << O << " == " << (O && O) << endl;
    cout << O << cap << N << " == " << (O && N) << endl;
    cout << O << cap << Z << " == " << (O && Z) << endl;
    cout << O << cap << R << " == " << (O && R) << endl;
    cout << O << cap << U << " == " << (O && U) << endl;
    cout << N << cap << N << " == " << (N && N) << endl;
    cout << N << cap << Z << " == " << (N && Z) << endl;
    cout << N << cap << R << " == " << (N && R) << endl;
    cout << N << cap << U << " == " << (N && U) << endl;
    cout << Z << cap << Z << " == " << (Z && Z) << endl;
    cout << Z << cap << R << " == " << (Z && R) << endl;
    cout << Z << cap << U << " == " << (Z && U) << endl;
    cout << R << cap << R << " == " << (R && R) << endl;
    cout << R << cap << U << " == " << (R && U) << endl;
    cout << U << cap << U << " == " << (U && U) << endl;

    auto A = finite_set({1,2,3});
    auto B = finite_set({3,4,5,6});
    cout << "A == " << A << endl;
    cout << "B == " << B << endl;
    cout << endl;
}

