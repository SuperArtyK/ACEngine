#include <iostream>
#include "AEVector.hpp"
#include "include/AEUtils.hpp"
using std::cout;

template <typename T1, typename T2, std::size_t dimAmount>
bool isEqual(const AEVector<T1, dimAmount>& vec1, const AEVector<T2, dimAmount>& vec2) {
	// Check if the number of dimensions is the same
	if (dimAmount != dimAmount)
		return false;

	// Compare the values in the dims arrays
	for (std::size_t i = 0; i < dimAmount; ++i) {
		if (vec1.dims[i] != vec2.dims[i])
			return false;
	}

	// If we haven't returned false by now, the vectors are equal
	return true;
}

template<typename T, const std::size_t dimAmount>
std::ostream& operator<<(std::ostream& out, const AEVector<T, dimAmount>& two) {

	out << '[';
	for (std::size_t i = 0; i < dimAmount-1; i++) {
		out << two.dims[i] << ", ";
	}
	out << two.dims[dimAmount - 1] << ']';
	return out;
}

int main() {


	AEVec2Int a{ 3,4 };
	AEVec3Int b{ 3,4,5 };
	AEVec2Float c{ 3.1f,4.5f };

	
	cout << a.magnitude() << "; " << b.magnitude() << "; " << c.magnitude()<<NLC;
	cout << a<<"; "<<b<<"; "<<c<<NLC;
	cout << a.normalise() << "; " << b.normalise() << "; " << c.normalise() << NLC;
	cout << (a != c) << NLC;
	cout << a.dotProduct<float>(c)<<NLC;
	cout << (a + c)<<NLC;
	cout << (a * 3) << NLC;

    return 0;
}
