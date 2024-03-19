
/*! @file AEMisc/include/AEMath.hpp
 *
 *  @brief This file contains the engine's **math and math-related functions**.
 *  
 *	These would be quite fast, I tried optimising them for the types that are needed.
 *	
 *	Especially with the addition of if-consteval in c++23 (and std::is_constant_evaluated() in c++20) :)
 *	
 *  Since it would be possible to use the constexpr implementations in compile time, 
 *  and fast (and proper) runtime versions of them at...runtime.
 *  
 *  @date 2023/09/22
 *
 *	@author	Artemii Kozhemiak (SuperArtyK)
 *
 *	@copyright MIT License
 *	
 *  *Should not cause everything to break :)*
 */

#pragma once

#ifndef ENGINE_MATH_HPP
#define ENGINE_MATH_HPP

#include "AEMathConst.hpp"
#include "AETypedefs.hpp"
#include <limits>
#include <cmath>
#include <type_traits>


/// This namespace contains the engine's **math and math-related functions**.
/// 
/// Mathematical functions and other things usefull in math. <br>
/// Almost all functions have a template parameter for the type of the operation. <br>
/// That's made so you won't need to make the operation of a wider type, if you need it in the narrower type. <br>
/// Example: calculate a cosecant using (and for) a float instead of a default long double. <br>
namespace ace::math {
	
	//math stuff
	
	/// <summary>
	/// Convert given **degrees to radians**
	/// </summary>
	/// <typeparam name="T">The type of the degrees value and the resulting operation</typeparam>
	/// <param name="deg">Value of degrees to convert</param>
	/// <returns>
	///		Radians from given degrees as type **T**
	/// </returns>
	template<typename T>
	[[nodiscard]] constexpr T toRad(const T deg) noexcept {
		return (deg * ace::math::cst::pi<T>() / T(180) );
	}

	/// <summary>
	/// Convert given **radians to degrees**
	/// </summary>
	/// <typeparam name="T">The type of the radians value and the resulting operation</typeparam>
	/// <param name="rad">Value of radians to convert</param>
	/// <returns>
	///		Degrees from given radians as type **T**
	/// </returns>
	template<typename T>
	[[nodiscard]] constexpr T toDeg(const T rad) noexcept {
		return (rad * T(180) / ace::math::cst::pi<T>());
	}

	/// <summary>
	/// Checks **if the float is "infinite"** (inf)
	/// </summary>
	/// <typeparam name="T">The type of a floating point number</typeparam>
	/// <param name="num">The number to check</param>
	/// <returns>
	///		* True if the given float number is infinite
	///		* False otherwise
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr bool isInf(const T num) noexcept {
		if constexpr (!std::is_floating_point<T>::value) { return false; } // it's always false
		return (num == std::numeric_limits<T>::infinity());
	}

	/// <summary>
	/// Checks **if the float is "NaN"**
	/// </summary>
	/// <typeparam name="T">The type of a floating point number</typeparam>
	/// <param name="num">The number to check</param>
	/// <returns>
	///		* True if the float number is NaN
	///		* False otherwise
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr bool isNan(const T num) noexcept {
		if constexpr (!std::is_floating_point<T>::value) { return false; } // it's always false
		return (num != num);
	}

	/// <summary>
	/// Checks **if the float is finite** (not infinity or NaN)
	/// </summary>
	/// <typeparam name="T">The type of a floating point number</typeparam>
	/// <param name="num">The number to check</param>
	/// <returns>
	///		* True if the float number is finite, not infinite or NaN
	///		* False otherwise
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr bool isFinite(const T num) noexcept {
		if constexpr (!std::is_floating_point<T>::value) { return true; } // it's always finite
		return !(ace::math::isInf<T>(num) || ace::math::isNan<T>(num));
	}

	

	/// <summary>
	/// Calculates the **sine of degrees**
	/// </summary>
	/// <typeparam name="T">The type to do the operation with</typeparam>
	/// <param name="degrees">Degree number to calculate sine from</param>
	/// <returns>
	///		The values of sine of given degrees, as type **T**
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] inline T sinDeg(const T degrees) noexcept {
		return std::sin(ace::math::toRad(degrees));
	}

	/// <summary>
	/// Calculates the **cosine of degrees**
	/// </summary>
	/// <typeparam name="T">The type to do the operation with</typeparam>
	/// <param name="degrees">Degree number to calculate cosine from</param>
	/// <returns>
	///		The values of cosine of given degrees, as type **T**
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] inline T cosDeg(const T degrees) noexcept {
		return std::cos(ace::math::toRad(degrees));
	}

	/// <summary>
	/// Calculates the **tangent of degrees**
	/// </summary>
	/// <typeparam name="T">The type to do the operation with</typeparam>
	/// <param name="degrees">Degree number to calculate tangent from</param>
	/// <returns>
	///		The values of tangent of given degrees, as type **T**
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] inline T tanDeg(const T degrees) noexcept {
		return std::tan(ace::math::toRad(degrees));
	}

	/// <summary>
	/// Calculates the **cotangent of degrees**
	/// </summary>
	/// <typeparam name="T">The type to do the operation with</typeparam>
	/// <param name="degrees">Degree number to calculate cotangent from</param>
	/// <returns>
	///		The values of cotangent of given degrees, as type **T**
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] inline T cotDeg(const T degrees) noexcept {
		return 1 / ace::math::tanDeg(degrees);
	}

	/// <summary>
	/// Calculates the **cosecant of degrees**
	/// </summary>
	/// <typeparam name="T">The type to do the operation with</typeparam>
	/// <param name="degrees">Degree number to calculate cosecant from</param>
	/// <returns>
	///		The values of cosecant of given degrees, as type **T**
	///	</returns>
	template<typename T = long double>
	[[nodiscard]] inline T cscDeg(const T degrees) noexcept {
		return 1 / ace::math::sinDeg(degrees);
	}

	/// <summary>
	/// Calculates the **secant of degrees**
	/// </summary>
	/// <typeparam name="T">The type to do the operation with</typeparam>
	/// <param name="degrees">Degree number to calculate secant from</param>
	/// <returns>
	///		The values of secant of given degrees, as type **T**
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] inline T secDeg(const T degrees) noexcept {
		return 1 / ace::math::cosDeg(degrees);
	}
	
	/// <summary>
	/// Calculates the **absolute value** of a given number.
	/// </summary>
	/// <typeparam name="T">The type of the passed number</typeparam>
	/// <param name="num">The value to calculate absolute value of</param>
	/// <returns>
	///		**Absolute value** of a given number of type **T**
	/// </returns>
	template<typename T>
	[[nodiscard]] constexpr inline T absval(const T num) noexcept {
		return (num == 0) ? T(0) : // work with signed 0's...if they exist
			(num < 0) ? -num : num;
	}

	/// <summary>
	/// **Compares two floating** point values **for equality**, within a given epsilon
	/// 
	/// @remark Requires the type **T** to be a floating-point type
	/// </summary>
	/// <typeparam name="T">The type of the floating point value</typeparam>
	/// <param name="num">The first float to compare</param>
	/// <param name="num2">The second float to compare</param>
	/// <param name="_epsilon">The epsilon to compare against</param>
	/// <returns>
	///		If both numbers are equal (within epsilon):
	///		* **True** 
	/// 
	///		Otherwise:
	///		* **False** 
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr bool fequals(const T num, const T num2, const T _epsilon) noexcept requires(std::is_floating_point<T>::value == true) {
		static_assert(std::is_floating_point<T>::value, "Cannot use non-float types in ace::math::fequals()!");
		const T dif = ace::math::absval<T>(num - num2);
		return (dif) <= _epsilon;
	}

	/// <summary>
	/// **Compares two floating** point values **for equality**, within a default (scaled) epsilon.
	/// 
	/// The epsilon is std::numeric_limits<T>::epsilon() * the smallest number of the 2.
	/// 
	/// @remark This is a helper/shortcut function of ace::math::fequals(const T num, const T num2, const T _epsilon)
	/// @remark Requires the type **T** to be a floating-point type
	/// </summary>
	/// <typeparam name="T">The type of the floats</typeparam>
	/// <param name="num">The first float to compare</param>
	/// <param name="num2">The second float to compare</param>
	/// <returns>
	///		If both numbers are equal (within epsilon):
	///		* **True** 
	/// 
	///		Otherwise:
	///		* **False** 
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr bool fequals(const T num, const T num2) noexcept requires(std::is_floating_point<T>::value == true) {
		const T _epsilon = std::numeric_limits<T>::epsilon() * std::min<T>(num, num2);
		return ace::math::fequals<T>(num, num2, _epsilon);
	}

	/// <summary>
	/// Checks **if given 2 numbers are equal**, a generic function for all types.
	/// 
	/// @note If the T type is a float, returns the result of ace::math::fequals() with the default epsilon values
	/// @see ace::math::fequals(const T num, const T num2)
	/// </summary>
	/// <typeparam name="T">The type of the first number passed</typeparam>
	/// <typeparam name="Y">The type of the second number passed (defaults to T)</typeparam>
	/// <param name="num">The first number to compare</param>
	/// <param name="num2">The second number to compare</param>
	/// <returns>
	///		If both numbers are equal:
	///		* **True** 
	/// 
	///		Otherwise:
	///		* **False** 
	/// </returns>
	template<typename T, typename Y = T>
	[[nodiscard]] constexpr bool equals(const T& num, const Y& num2) noexcept {
		if constexpr ((std::is_floating_point<T>::value && !std::is_floating_point<Y>::value) || // 1st type is float, 2nd one isn't
			(std::numeric_limits<T>::max_digits10 > std::numeric_limits<Y>::max_digits10) ) // or...both are float, but 1'st one is larger
			{ 
			return ace::math::fequals<T>(num, T(num2));
		}
		if constexpr ((!std::is_floating_point<T>::value && std::is_floating_point<Y>::value) || // 1st type isn't float, 2nd one is
			(std::numeric_limits<T>::max_digits10 > std::numeric_limits<Y>::max_digits10) ) { // or...both are float, but 2nd one is larger
			return ace::math::fequals<Y>(Y(num), num2);
		}
		else {
			return (num == num2);
		}
	}

	/// <summary>
	/// Calculate the **integer exponent** of a given number
	/// 
	/// @attention If the type T cannot hold the result, it will overflow!
	/// </summary>
	/// <typeparam name="T">The type of the variable to calculate it with</typeparam>
	/// <param name="num">The value to raise to power</param>
	/// <param name="power">Integer power to raise the number to</param>
	/// <returns>
	///		If the passed **num** is valid and finite:
	///		* Number **num** raised to the exponent **power**
	///		
	///		Otherwise, if **num** isn't finite, or it's 0 *and* **power** is negative:
	///		* **std::numeric_limits<T>::max()** (largest value of type **T**)
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T intPow(const T num, llint power) noexcept {
		if (ace::math::isFinite<T>(num) || // check if number is finite
			(ace::math::equals(num, T(0)) && power < 0)) { // or 0 raised to negative exponent

			return std::numeric_limits<T>::max();
		}

		switch (power) {

			case -1:
				return T(1) / num;
				break;

			case 0: // power is 0, anything to power 0 is 1
				return T(1);
				break;

			case 1: // power is 0, anything to power 1 is itself
				return num;
				break;

			default:
				if (ace::math::equals(num, T(1))) { return T(1); } // 1 to power of anything is still 1
				break;
		}

		T res;
		if (power > 1) {
			res = num;
			for (int i = 1; i < power; i++) {
				res *= num;
			}
		}
		else {
			res = 1;
			power = -power;
			for (int i = 1; i < power; i++) {
				res /= num;
			}
		}
		return res;
	}

	/// <summary>
	/// Newton's method **sqrt implementation, compatible with constexpr** evaluation.
	/// 
	/// @remark Exists here for the only reason that std::sqrt isn't constexpr untill c++26 (https://en.cppreference.com/w/cpp/numeric/math/sqrt)
	/// @todo When C++23 support appears, add if-consteval to use the newton's method in constexpr context, and use classic std::sqrt otherwise
	/// </summary>
	/// <typeparam name="T">The type of the passed value and type to calculate it with</typeparam>
	/// <param name="num">The value to calculate the square root from</param>
	/// <returns>
	///		If the **num** is positive and finite:
	///		* The square root of **num** as type **T**
	///		
	///		If the **num** is negative or isn't finite:
	///		* **std::numeric_limits<T>::max()** (largest value of type **T**)
	///	</returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T sqrt(const T num) noexcept {
		if (num < 0 || ace::math::isFinite<T>(num)) {
			return std::numeric_limits<T>::max();
		}

		if (std::is_constant_evaluated()) {
			

			T val[2] = { num,0 };
			while (!ace::math::equals(val[0], val[1])) {
				val[1] = val[0];
				val[0] = (val[0] + num / val[0]) / T(2);
				if constexpr (std::is_integral<T>::value) {
					if (val[1] < val[0]) {
						break;
					}
				}
			}
			return val[1];
		}
		else {
			return std::sqrt(num);
		}
	}

	/// <summary>
	/// Newton's method **cbrt implementation, compatible with constexpr** evaluation.
	/// 
	/// @remark Exists here for the only reason that std::cbrt isn't constexpr untill c++26 (https://en.cppreference.com/w/cpp/numeric/math/cbrt)
	/// @todo When C++23 support appears, add if-consteval to use the newton's method in constexpr context, and use classic std::cbrt otherwise
	/// </summary>
	/// <typeparam name="T">The type of the passed value and type to calculate it with</typeparam>
	/// <param name="num">The value to calculate the cube root from</param>
	/// <returns>
	///		If the **num** is finite:
	///		* The cube root of **num** as type **T**
	///		
	///		If the **num** isn't finite:
	///		* **std::numeric_limits<T>::max()** (largest value of type **T**)
	///	</returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T cbrt(const T num) noexcept {

		if (ace::math::isFinite<T>(num)) {
			return std::numeric_limits<T>::max();
		}

		if (std::is_constant_evaluated()) {
			T val[2] = { num,0 };
			while (!ace::math::equals(val[0], val[1])) {
				val[1] = val[0];
				val[0] = (2 * val[0] + (num / (val[0] * val[0]))) / 3;
				if constexpr (std::is_integral<T>::value) {
					if (val[1] < val[0]) {
						break;
					}
				}
			}
			return val[1];
		}
		else {
			return std::cbrt(num);
		}
	}

	/// <summary>
	/// Calculate the **n'th root** of a number.
	/// 
	/// Thanks, Newton!
	/// 
	/// @remark If **rtNum** is 2, calls ace::math::sqrt() instead
	/// </summary>
	/// <typeparam name="T">The type of the variable to calculate it with</typeparam>
	/// <param name="num">The number to take a root of</param>
	/// <param name="rtNum">The root degree to calculate</param>
	/// <returns>
	///		If the **num** value is finite and valid for the given root degree:
	///		* The n'th root of **num** as type **T**
	///		
	///		Otherwise, if **num** isn't finite, or the operation results in an imaginary root:
	///		* std::numeric_limits<T>::max() (largest value of type **T**)
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T root(const T num, const uint rtNum) noexcept {

		switch (rtNum) {
			case 2:
				return ace::math::sqrt(num);
				break;

			case 3:
				return ace::math::cbrt(num);
				break;
		}

		if (ace::math::isFinite<T>(num) || (num < 0 && !(rtNum % 2))) {
			return std::numeric_limits<T>::max();
		}

		if (std::is_constant_evaluated()) {
			const uint rtMinusOne = rtNum - 1;
			T val[2] = { num,0 };
			while (!ace::math::equals(val[0], val[1])) {
				val[1] = val[0];
				val[0] = (rtMinusOne * val[0] + (num / ace::math::intPow<long double>(val[0], rtMinusOne))) / T(rtNum);
				if constexpr (std::is_integral<T>::value) {
					if (val[1] < val[0]) {
						break;
					}
				}
			}
			return val[1];
		}
		else {
			return std::pow(num, 1.0L / rtNum);
		}
	}

	/// <summary>
	/// Returns the **largest value** between the given two values.
	/// </summary>
	/// <typeparam name="T">Type of the values</typeparam>
	/// <param name="a">The first value to compare</param>
	/// <param name="b">The second value to compare</param>
	/// <returns>
	///		If **a** is more than **b**:
	///		* A copy of value **a** (since returned by value)
	///		
	///		Otherwise:
	///		* A copy of value **b** (since returned by value)
	/// </returns>
	template<typename T>
	[[nodiscard]] constexpr T max(const T& a, const T& b) noexcept {
		return (a > b) ? a : b;
	}

	/// <summary>
	/// Gets the **smallest value** between given two values.
	/// </summary>
	/// <typeparam name="T">Type of the values</typeparam>
	/// <param name="a">The first value to compare</param>
	/// <param name="b">The second value to compare</param>
	/// <returns>
	///		If **a** is less than **b**:
	///		* A copy of value **a** (since returned by value)
	///		
	///		Otherwise:
	///		* A copy of value **b** (since returned by value)
	/// </returns>
	template<typename T>
	[[nodiscard]] constexpr T min(const T& a, const T& b) noexcept {
		return (a < b) ? a : b;
	}

	/// <summary>
	/// Calculate the **length of the given integer** number.
	/// 
	/// @remark Requires the type **T** to be an integral type
	/// </summary>
	/// <typeparam name="T">The type of the integer number</typeparam>
	/// <param name="num">The number to calculate the length of</param>
	/// <returns>
	///		The length of the integer **num** as ::uint
	/// </returns>
	template<typename T>
	[[nodiscard]] constexpr uint lengthOfInt(T num) noexcept requires(std::is_integral<T>::value == true) {
		static_assert(std::is_integral<T>::value, "Cannot use non-integral types in the ace::math::lengthOfInt()");
		unsigned int dig = 1;
		while (num /= 10)
			dig++;
		return dig;
	}

	/// <summary>
	/// Calculate the **length of the given float** number's non-fractional part.
	/// 
	/// Essentially ace::math::lengthOfInt() for larger numbers.
	/// 
	/// @remark Requires the type **T** to be a floating point type
	/// </summary>
	/// <typeparam name="T">The type of the float number</typeparam>
	/// <param name="num">The number to calculate the length of</param>
	/// <returns>
	///		The length of the **num**'s non fractional part as ::uint
	/// </returns>
	template<typename T>
	[[nodiscard]] constexpr uint lengthOfFloat(const T num) noexcept requires(std::is_floating_point<T>::value == true) {
		static_assert(std::is_floating_point<T>::value, "Cannot use non-float types in the ace::math::lengthOfFloat()!");
		return ((num == 0) ? 1 : (unsigned int)std::log10(std::abs(num))) + 1;
	}

	/// <summary>
	/// Computes the **linear inter/extrapolation** with the given interval boundaries a and b, and the interval percentage **c**.
	/// 
	/// The value of **c** is the percentage of the distance between values **a** and **c**:
	/// * 0% is **a**
	/// * 100% is **b**
	/// * 50% is in between them
	/// * 150% is **b**+half of distance **a** and **b**. 
	/// Example: lower boundary **a** is 0, higher boundary **b** is 10, interval coefficient **c** is 0.75; The result is 7.5. <br>
	/// Example: lower boundary **a** is 0, higher boundary **b** is 10, interval coefficient **c** is 1.5; The result is 15. <br>
	/// 
	/// @note if **c** is outside of 0 and 1, computes linear extrapolation.
	/// @note If **c** is between 0 (closest to boundary **a**) and 1 (closest to boundary **b**), computes linear interpolation.
	/// 
	/// https://en.wikipedia.org/wiki/Linear_interpolation
	/// </summary>
	/// <typeparam name="T">The type to calculate it with</typeparam>
	/// <param name="a">The value of a (lower known boundary)</param>
	/// <param name="b">The value of b (higher known boundary)</param>
	/// <param name="c">The value for inter/extrapolation (interval percentage)</param>
	/// <returns>
	///		The inter/extrapolated value of type **T**
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T lerp(const T a, const T b, const T c) noexcept {
		return  a + c * (b - a);
	}

	/// <summary>
	/// Calculates the **hypotenuse length** from given values a and b.
	/// </summary>
	/// <typeparam name="T">The type of the variable to calculate it with</typeparam>
	/// <param name="a">The length of side a in the right triangle</param>
	/// <param name="b">The length of side b in the right triangle</param>
	/// <returns>
	///		The length of hypotenuse of type **T** with given lengths of **a** and **b** sides
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T hypot(const T a, const T b) noexcept {
		return ace::math::sqrt<T>(a * a + b * b);
	}

	/// <summary>
	/// **Rounds the given number** of type Y **to integer** of type T.
	/// 
	/// @remark If value overflows -- return value depends on the overflow behaviour of your platform/compiler
	/// @attention **num** should be finite (check with ace::math::isFinite())
	/// @remark Requires the type **T** to be an integral type
	/// @remark Requires the type **Y** to be an arithmetic type
	/// </summary>
	/// <typeparam name="T">The type of the integer to round to</typeparam>
	/// <typeparam name="Y">The type of the float to round</typeparam>
	/// <param name="num">The floating point number to round</param>
	/// <returns>
	///		If the type **Y** is an integral type:
	///		* Simply **num** casted to type **T**;
	///		
	///		If the type **Y** is a floating point type:
	///		* The rounded integer of type **T**, from the given **num** value		
	/// </returns>
	template<typename T = llint, typename Y = long double>
	[[nodiscard]] constexpr T roundToInt(const Y num) noexcept requires(std::is_arithmetic<Y>::value == true && std::is_integral<T>::value == true) {
		if constexpr (std::is_integral<Y>::value) { return num; } // it's an int anyway
		else {
			//static_assert(std::is_floating_point<Y>::value, "Cannot use non-float types as the float type in ace::math::roundtoint()!");
			return (num < 0) ? T(num + Y(-0.5f)) : T(num + Y(0.5f));
		}
	}

	/// <summary>
	/// **Rounds the passed number** of type Y and converts it to the type T.
	/// 
	/// @note During compile-time calculation, it calls ace::math::roundToInt(). Mind the overflows
	/// @todo When c++23 support appears, change it to a call to std::round() instead
	/// @see ace::math::roundToInt()
	/// @remark Types **T** and **Y** have to be arithmetic types
	/// </summary>
	/// <typeparam name="T">The type to convert the result to</typeparam>
	/// <typeparam name="Y">The type of the passed number. Defaults to **T**</typeparam>
	/// <param name="num">The number to round</param>
	/// <returns>
	///		During runtime:
	///		* Result of the call to **std::round()**
	///		
	///		During compile-time:
	///		* Result of the call to ace::math::roundToInt() 
	/// </returns>
	template<typename T = long double, typename Y = T>
	[[nodiscard]] constexpr T round(const Y num) noexcept requires(std::is_arithmetic<Y>::value == true && std::is_arithmetic<T>::value == true){
		if (std::is_constant_evaluated()) {
			ace::math::roundToInt(num);
		}
		else {
			return std::round(num);
		}
	}



	/// <summary>	
	/// **Floor's the given float** of type Y and converts **to integer** of type T.
	/// 
	/// @remark If value overflows -- return value depends on the overflow behaviour of your platform/compiler
	/// @attention **num** should be finite (check with ace::math::isFinite())
	/// @note If num is positive, it truncates the decimal digits (towards zero); if negative, it ceil's the decimals (away from zero)
	/// @remark Requires the type **T** to be an integral type
	/// @remark Requires the type **Y** to be an arithmetic type
	/// </summary>
	/// <typeparam name="T">The type of the integer to floor to</typeparam>
	/// <typeparam name="Y">The type of the float to floor</typeparam>
	/// <param name="num">The floating point number to floor</param>
	/// <returns>
	///		If the type **Y** is an integral type:
	///		* Simply **num** casted to type **T**;
	///		
	///		If the type **Y** is a floating point type:
	///		* The floored integer of type **T**, from the given **num** value	
	/// </returns>
	template<typename T = llint, typename Y = long double>
	[[nodiscard]] constexpr T floorToInt(const Y num) noexcept requires(std::is_arithmetic<Y>::value == true && std::is_integral<T>::value == true) {
		if constexpr (std::is_integral<Y>::value) { return num; } // it's an int anyway
		else {
			return (num < 0 && !ace::math::fequals<Y>(T(num), num)) ? T(num) - 1 : T(num);
		}
	}

	/// <summary>
	/// **Floor's the passed number** of type Y and converts it to the type T.
	/// 
	/// @note During compile-time calculation, it calls ace::math::floorToInt(). Mind the overflows
	/// @todo When c++23 support appears, change it to a call to std::floor() instead
	/// @see ace::math::floorToInt()
	/// @remark Types **T** and **Y** have to be arithmetic types
	/// </summary>
	/// <typeparam name="T">The type to convert the result to</typeparam>
	/// <typeparam name="Y">The type of the passed number. Defaults to **T**</typeparam>
	/// <param name="num">The number to floor</param>
	/// <returns>
	///		During runtime:
	///		* Result of the call to **std::floor()**
	///		
	///		During compile-time:
	///		* Result of the call to ace::math::floorToInt() 
	/// </returns>
	template<typename T = long double, typename Y = T>
	[[nodiscard]] constexpr T floor(const Y num) noexcept requires(std::is_arithmetic<Y>::value == true && std::is_arithmetic<T>::value == true) {
		if (std::is_constant_evaluated()) {
			ace::math::floorToInt(num);
		}
		else {
			return std::floor(num);
		}
	}


	/// <summary>
	/// **Ceil's the given float** of type Y and converts **to integer** of type T.
	/// 
	/// @remark If value overflows -- return value depends on the overflow behaviour of your platform/compiler
	/// @attention **num** should be finite (check with ace::math::isFinite())
	/// @remark Requires the type **T** to be an integral type
	/// @remark Requires the type **Y** to be an arithmetic type
	/// </summary>
	/// <typeparam name="T">The type of the integer to ceil to</typeparam>
	/// <typeparam name="Y">The type of the float to ceil</typeparam>
	/// <param name="num">The floating point number to ceil</param>
	/// <returns>
	///		If the type **Y** is an integral type:
	///		* Simply **num** casted to type **T**;
	///		
	///		If the type **Y** is a floating point type:
	///		* The ceiled integer of type **T**, from the given **num** value	
	/// </returns>
	template<typename T = llint, typename Y = long double>
	[[nodiscard]] constexpr T ceilToInt(const Y num) noexcept requires(std::is_arithmetic<Y>::value == true && std::is_integral<T>::value == true) {
		if constexpr (std::is_integral<Y>::value) { return num; } // it's an int anyway
		else {
			return (ace::math::fequals<Y>(num, T(num))) ? T(num) : T(num) + 1;
		}
	}
	
	/// <summary>
	/// **Ceil's the passed number** of type Y and converts it to the type T.
	/// 
	/// @note During compile-time calculation, it calls ace::math::ceilToInt(). Mind the overflows
	/// @todo When c++23 support appears, change it to a call to std::ceil() instead
	/// @see ace::math::ceilToInt()
	/// @remark Types **T** and **Y** have to be arithmetic types
	/// </summary>
	/// <typeparam name="T">The type to convert the result to</typeparam>
	/// <typeparam name="Y">The type of the passed number. Defaults to **T**</typeparam>
	/// <param name="num">The number to ceil</param>
	/// <returns>
	///		During runtime:
	///		* Result of the call to **std::ceil()**
	///		
	///		During compile-time:
	///		* Result of the call to ace::math::ceilToInt() 
	/// </returns>
	template<typename T = long double, typename Y = T>
	[[nodiscard]] constexpr T ceil(const Y num) noexcept requires(std::is_arithmetic<Y>::value == true && std::is_arithmetic<T>::value == true) {
		if (std::is_constant_evaluated()) {
			ace::math::ceilToInt(num);
		}
		else {
			return std::ceil(num);
		}
	}


	/// <summary>
	/// **Truncates the given float** of type Y and converts **to integer** of type T.
	/// 
	/// @remark If value overflows -- return value depends on the overflow behaviour of your platform/compiler
	/// @attention **num** should be finite (check with ace::math::isFinite())
	/// @remark Requires the type **T** to be an integral type
	/// @remark Requires the type **Y** to be an arithmetic type
	/// </summary>
	/// <typeparam name="T">The type of the integer to truncate to</typeparam>
	/// <typeparam name="Y">The type of the float to truncate</typeparam>
	/// <param name="num">The floating point number to truncate</param>
	/// <returns>
	///		If the type **Y** is an integral type:
	///		* Simply **num** casted to type **T**;
	///		
	///		If the type **Y** is a floating point type:
	///		* The truncated integer of type **T**, from the given **num** value	
	/// </returns>
	template<typename T = llint, typename Y = long double>
	[[nodiscard]] constexpr T truncToInt(const Y num) noexcept requires(std::is_arithmetic<Y>::value == true && std::is_integral<T>::value == true) {
		if constexpr (std::is_integral<Y>::value) { return num; } // it's an int anyway
		else {
			return T(num);
		}

	}

	/// <summary>
	/// **Truncates the passed number** of type Y and converts it to the type T.
	/// 
	/// @note During compile-time calculation, it calls ace::math::truncToInt(). Mind the overflows
	/// @todo When c++23 support appears, change it to a call to std::trunc() instead
	/// @see ace::math::truncToInt()
	/// @remark Types **T** and **Y** have to be arithmetic types
	/// </summary>
	/// <typeparam name="T">The type to convert the result to</typeparam>
	/// <typeparam name="Y">The type of the passed number. Defaults to **T**</typeparam>
	/// <param name="num">The number to trunc</param>
	/// <returns>
	///		During runtime:
	///		* Result of the call to **std::trunc()**
	///		
	///		During compile-time:
	///		* Result of the call to ace::math::truncToInt() 
	/// </returns>
	template<typename T = long double, typename Y = T>
	[[nodiscard]] constexpr T trunc(const Y num) noexcept requires(std::is_arithmetic<Y>::value == true && std::is_arithmetic<T>::value == true) {
		if (std::is_constant_evaluated()) {
			ace::math::truncToInt(num);
		}
		else {
			return std::trunc(num);
		}
	}

}

#endif // ENGINE_MATH_HPP
