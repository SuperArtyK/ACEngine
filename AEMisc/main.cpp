
#include <iostream>
#include "AETypedefs.hpp"
#include "AEUtils.hpp"
using std::cout;
using std::endl;



int main() {
	

	std::string testv = ace::utils::getCurrentDate();
	cout << ace::utils::boolToString(false) << "; " << ace::utils::boolToString(true) << endl;
	cout << ace::utils::isInRange(-128,127,64) << "; " << ace::utils::isInRange(-128, 127, 255) << endl;
	cout << ace::utils::toLowerVal("abc123def") << "; " << ace::utils::toUpperVal("abc123def") << endl;
	cout << ace::utils::boolToString(ace::utils::isNum("-123 567 90")) << "; " << ace::utils::boolToString(ace::utils::isNum("-1234567890")) << endl;
	cout << ace::utils::getCurrentDate() << "; " << ace::utils::addrToStr(&testv) << endl;
	cout << endl;

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
