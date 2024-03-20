
#include <iostream>
#include "AETypedefs.hpp"
#include "AEUtils.hpp"
#include "AEFlags.hpp"
#include "AEMath.hpp"
#include "AEMathTrigLookup.hpp"
#include <iomanip>
#include <vector>
using std::cout;
using std::endl;

struct mystruct {

	static void myfunc() {
		dprintf("This is a test debug line");
	}

};



int main() {
	
	constexpr int mya = 1;
	constexpr int myb = 1;


	constexpr int myc = ace::math::max(mya, myb);

	mystruct::myfunc();

	cout << (IS_SAME_NOCV(const int, int)) << " " << (std::is_same < std::remove_cv<const char[]>::type, const char*>::value) << NLC;

	cout << ace::utils::byteToHex(0xFF)<<NLC;

	char myarr[] = "Hello World!";

	cout << ace::utils::readBytesAsHex(myarr, sizeof(myarr)) << NLC;
	cout << ace::utils::readBytesAsHex(myarr, sizeof(myarr), true, "; ") << NLC;
	cout << ace::utils::readBytesAsHex(myarr, sizeof(myarr), false) << NLC;


	cout << (std::is_same< std::remove_cv<std::decay<const char[]>::type>::type, std::remove_cv<std::decay<const char*>::type>::type>::value)<<" "<< IS_SAME_NOCV_DECAY(const char[], const char*) << NLC;

	cout << ace::utils::stringToDate << NLC;

	cout << std::fixed<<std::setprecision(16)<<ace::math::root<float>(236, 2)<<NLC;
	

	for (int i = 0; i < 360; i++) {
		cout << ace::math::lookup::sinDeg_table(i) << "; " << ace::math::lookup::cscDeg_table(i)<<"| ";
		cout << ace::math::lookup::cosDeg_table(i) << "; " << ace::math::lookup::secDeg_table(i) << "| ";
		cout << ace::math::lookup::tanDeg_table(i) << "; " << ace::math::lookup::cotDeg_table(i) << NLC;
	}
	cout << NLC<<NLC;
	cout << "---------------"<<NLC;
	cout << ace::math::lookup::sinDeg_table(1)<<"; "<<ace::math::lookup::cosDeg_table(1)<<NLC;
	cout << sizeof(ace::math::lookup::_tables::sinTable) << NLC;


	cout << std::numeric_limits<long double>::digits10 << endl;
	std::string testv = ace::utils::getCurrentDate();
	cout << ace::utils::boolToString(false) << "; " << ace::utils::boolToString(true) << endl;
	cout << ace::utils::isInRange(-128,127,64) << "; " << ace::utils::isInRange(-128, 127, 255) << endl;
	cout << (ace::utils::toLowerVal("abc123def")) << "; " << (ace::utils::toUpperVal("abc123def")) << endl;
	cout << ace::utils::boolToString(ace::utils::isNum<true>("-123.567")) << "; " << ace::utils::boolToString(ace::utils::isNum<false>("-1234567890")) << endl;
	cout << ace::utils::boolToString(ace::utils::isNum<false>("-123.567")) << "; " << ace::utils::boolToString(ace::utils::isNum<true>("-1234567890")) << endl;
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


	cout << ace::math::lookup::sinDeg_tableInterp(30) << " " << ace::math::lookup::sinDeg_tableInterp(30.25) << " " << ace::math::lookup::sinDeg_tableInterp(30.5) << " " << ace::math::lookup::sinDeg_tableInterp(30.75) << " " << ace::math::lookup::sinDeg_tableInterp(31)<<endl;
	cout << ace::math::sinDeg(30.0) << " " << ace::math::sinDeg(30.25) << " " << ace::math::sinDeg(30.5) << " " << ace::math::sinDeg(30.75) << " " << ace::math::sinDeg(31.0)<<endl;
	

	cout << NLC << NLC;
	cout << ace::math::lerp<long double>(0,10,1.5);



	std::cin.get();
    return 0;
}
