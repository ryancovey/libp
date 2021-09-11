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

    auto print_and = [&](const auto& lhs, const auto& rhs) {
        cout << lhs << cap << rhs << " == " << (lhs && rhs) << endl;
    };

    print_and(O,O);
    print_and(O,N);
    print_and(O,Z);
    print_and(O,R);
    print_and(O,U);
    print_and(N,N);
    print_and(N,Z);
    print_and(N,R);
    print_and(N,U);
    print_and(Z,Z);
    print_and(Z,R);
    print_and(Z,U);
    print_and(R,R);
    print_and(R,U);
    print_and(U,U);

    auto print_or = [&](const auto& lhs, const auto& rhs) {
        cout << lhs << cup << rhs << " == " << (lhs || rhs) << endl;
    };

    print_or(O,O);
    print_or(O,N);
    print_or(O,Z);
    print_or(O,R);
    print_or(O,U);
    print_or(N,N);
    print_or(N,Z);
    print_or(N,R);
    print_or(N,U);
    print_or(Z,Z);
    print_or(Z,R);
    print_or(Z,U);
    print_or(R,R);
    print_or(R,U);
    print_or(U,U);

    auto A = finite_set({1,2,3});
    auto B = finite_set({3,4,5,6});
    cout << "A == " << A << endl;
    cout << "B == " << B << endl;
    cout << endl;
}

