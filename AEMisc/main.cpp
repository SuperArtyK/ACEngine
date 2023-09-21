
#include <iostream>
#include "include/AETypedefs.hpp"
using std::cout;
using std::endl;

int main() {
    
	cout << UCINT_MAX << "; " << UCINT_MIN << endl;
	cout << CINT_MAX << "; " << CINT_MIN << endl;
	cout << USHRT_MAX << "; " << USHRT_MIN << endl;
	cout << SHRT_MAX << "; " << SHRT_MIN << endl;
	cout << UINT_MAX << "; " << UINT_MIN << endl;
	cout << INT_MAX << "; " << INT_MIN << endl;
	cout << ULLINT_MAX << "; " << ULLINT_MIN << endl;
	cout << LLINT_MAX << "; " << LLINT_MIN << endl;

    return 0;
}
