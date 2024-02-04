#include <iostream>
#include "AEVector.hpp"
#include "include/AEUtils.hpp"
using std::cout;

template <typename T1, typename T2, std::size_t dimAmount>
bool isEqual(const AEVector<T1, dimAmount>& vec1, const AEVector<T2, dimAmount>& vec2) {
	// Check if the number of dimensions is the same
	if (vec1.dimAmount != vec2.dimAmount)
		return false;

	// Compare the values in the dims arrays
	for (std::size_t i = 0; i < dimAmount; ++i) {
		if (vec1.dims[i] != vec2.dims[i])
			return false;
	}

	// If we haven't returned false by now, the vectors are equal
	return true;
}



int main() {


	cout << ace::math::sqrt(12345)<< NLC;

	AEVec2Int a{ 3,4 };
	AEVec3Int b{ 3,4,5 };
	AEVec2Float c{ 3.1f,4.5f };


	cout << a.magnitude<int>() << "; " << b.magnitude() << "; " << c.magnitude()<<NLC;
	cout << a<<"; "<<b<<"; "<<c<<NLC;
	cout << a.normalise() << "; " << b.normalise() << "; " << c.normalise() << NLC;
	cout << (a != c) << NLC;
	cout << a.dotProduct<float>(c)<<NLC;
	cout << (a + c)<<NLC;
	cout << (a * 3) << NLC;
	cout << (a / 3) << NLC;
	cout << (a + 5) << NLC;
	cout << (a * c) << NLC;

	cout << "\n\n\n";
	constexpr AEVec2Int f{ 3,4 }, g{ 4,3 };
	//constexpr AEVec3Int testval = AEVec3Int{1,2,3}.crossProduct(AEVec3Int{ 1,2,3 });


	long double cos1 = f.cosAngleBetween(g);

	AEVec3Int test1{ -1,2,-3 }, test2{ 0,-4,1 }, N = test1.crossProduct(test2), test3{ -test1 };


	cout << cos1<<NLC;
	cout << f.angleBetweenRad(g) << NLC;
	cout << f.angleBetweenDeg(g) << NLC;
	cout << (f-g) << NLC;


	AEVector<int, 10> vec{ 0,1,2,3,4,5,6,7,8,9 };
	cout << vec<<NLC;


	cout << "\n\n\n";
	cout << N<<NLC;
	cout << test2.crossProduct(test1)<<NLC;


	cout << N.isOrthogonal(test1) << NLC;
	cout << N.isOrthogonal(test2) << NLC;
	cout << N.isCollinear(N) << NLC;
	cout << N.isCollinear(test1) << NLC;
	cout << test3 << NLC;


	cout << "\n\n\n";
	cout << c << NLC;
	cout << c.perpendicularToItself() << NLC;
	cout << (c.perpendicularToItself()).isOrthogonal(c) << NLC;
    return 0;
}
