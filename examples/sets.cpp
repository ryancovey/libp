#include <iostream>
#include <libp/libp.hpp>

using namespace libp;
using std::cout;
using std::endl;

int main(void) {
    const char * cap = reinterpret_cast<const char *>(u8" \u2229 ");
    const char * cup = reinterpret_cast<const char *>(u8" \u222A ");

    std::vector<MeasurableSet> sets = {none(), natural_numbers(), integers(), real_numbers(), all()};

    cout << "Intersections of Fundamental Sets" << endl;
    for (const auto& lhs : sets) {
        for (const auto& rhs : sets) {
            cout << lhs << cap << rhs << " == " << (lhs && rhs) << endl;
        }
    }

    cout << endl << "Unions of Fundamental Sets" << endl;
    for (const auto& lhs : sets) {
        for (const auto& rhs : sets) {
            cout << lhs << cup << rhs << " == " << (lhs || rhs) << endl;
        }
    }

    cout << endl << "Finite Sets" << endl;
    auto A = finite_set({1,2,3});
    auto B = finite_set({3,4,5,6});
    cout << "A == " << A << endl;
    cout << "B == " << B << endl;
    cout << "A" << cap << "B == " << (A && B) << endl;
    cout << "A" << cup << "B == " << (A || B) << endl;

    return 0;
}

