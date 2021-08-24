#include <iostream>
#include <libp/libp.hpp>

using namespace libp;
using std::cout;
using std::endl;

int main(void) {
    // We can obtain an object representing the distribution of a standard six sided dice as follows.
    auto P = uniform_distribution({1,2,3,4,5,6});
    cout << "P = uniform_distribution({1,2,3,4,5,6})" << endl;
    cout << endl;

    // If we want to specify the time of the elements underlying the distribution, to be unsigned
    // for example, we can write the following.
    // auto P = UniformDistribution(finite_set<unsigned>({1,2,3,4,5,6}));

    // We can assign events to variables...

    auto A = finite_set({1,2,3});
    auto B = finite_set({3,4,5,6});
    cout << "A = {1,2,3}" << endl;
    cout << "B = {3,4,5,6}" << endl;
    cout << endl;

    cout << "P(A) == " << P(A) << endl;
    cout << "P(B) == " << P(B) << endl;
    cout << "P(A && B) == " << P(A && B) << endl;
    cout << "P(B && A) == " << P(B && A) << endl;
    cout << "P(A || B) == " << P(A || B) << endl;
    cout << "P(B || A) == " << P(B || A) << endl;
    cout << "P(A|B) == " << P(A|B) << endl;
    cout << "P(B|A) == " << P(B|A) << endl;
    cout << endl;

    // ...or define them inline...
    
    cout << "P({1,2,3}) == " << P({1,2,3}) << endl;
    cout << "P({3,4,5,6}) == " << P({3,4,5,6}) << endl;
    cout << endl;
    
    // ... but not always! C++ infix operators such as "&&", "||", and "|" cannot be used with braced arguments.
    
    // cout << "P({1,2,3} && {3,4,5,6}) == " << P({1,2,3} && {3,4,5,6}) << endl;
    // cout << "P({1,2,3} || {3,4,5,6}) == " << P({1,2,3} || {3,4,5,6}) << endl;
    // cout << "P({1,2,3}|{3,4,5,6}) == " << P({1,2,3}|{3,4,5,6}) << endl;
    // cout << "P({3,4,5,6}|{1,2,3}) == " << P({3,4,5,6}|{1,2,3}) << endl;
    
    // But we can wrap our inline sets in the finite_set function for the same effect.
    
    cout << "P(finite_set({1,2,3}) && finite_set({3,4,5,6})) == " << P(finite_set({1,2,3}) && finite_set({3,4,5,6})) << endl;
    cout << "P(finite_set({1,2,3}) || finite_set({3,4,5,6})) == " << P(finite_set({1,2,3}) || finite_set({3,4,5,6})) << endl;
    cout << "P(finite_set({1,2,3})|finite_set({3,4,5,6})) == " << P(finite_set({1,2,3})|finite_set({3,4,5,6})) << endl;
    cout << "P(finite_set({3,4,5,6})|finite_set({1,2,3})) == " << P(finite_set({3,4,5,6})|finite_set({1,2,3})) << endl;
    cout << endl;

    // We can also mix and match the types of elements in the sets. In general, two elements of different types are equal
    // if and only if converting from one type to the other and back again does not change the elements.
    
    auto Bd = finite_set({3.0,4.0,5.0,6.0,7.5});
    cout << "P(Bd) == " << P(Bd) << endl;
    cout << "P(A && Bd) == " << P(A && Bd) << endl;
    cout << "P(Bd && A) == " << P(Bd && A) << endl;
    cout << "P(A || Bd) == " << P(A || Bd) << endl;
    cout << "P(Bd || A) == " << P(Bd || A) << endl;
    cout << "P(A|Bd) == " << P(A|Bd) << endl;
    cout << "P(Bd|A) == " << P(Bd|A) << endl;
    cout << endl;
    
    return 0;
}

