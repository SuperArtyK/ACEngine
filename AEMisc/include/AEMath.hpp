
/** @file AEMisc/include/AEMath.hpp
 *  This file contains the engine math functions, (probably)optimised for engine stuff.
 *  may be faster or equivalent to what can you find/do yourself.
 *  And yeah...easier to use.
 *  
 *  Should not cause everything to break :)
 */

#pragma once

#ifndef ENGINE_MATH_HPP
#define ENGINE_MATH_HPP

#include "AETypedefs.hpp"
#include <limits>
#include <type_traits>


/// Macro to convert degrees value x to radians.
/// <param name="deg">Value of degrees to convert</param>
/// <param name="type">The type of the resulting operation</param>
#define torad(deg, type) (deg * ace::math::pi<type>() / type(180))

/// Macro to convert radians value x to degrees.
/// <param name="rad">Value of radians to convert</param>
/// <param name="type">The type of the resulting operation</param>
#define todeg(rad, type) (rad*type(180)/ace::math::pi<type>())


///Mathematical functions and other things usefull in math
namespace ace::math {

	//math stuff
	
	/// <summary>
	/// Engine's pi value.
	/// </summary>
	/// <typeparam name="T">Type to convert pi to</typeparam>
	/// <returns>Pi converted/rounded to a given type</returns>
	template<typename T = long double>
	constexpr T pi(void) noexcept {
		return T(3.14159265358979323846L);
	}

	/// <summary>
	/// Engine's tau (pi*2) value.
	/// </summary>
	/// <typeparam name="T">Type to convert tau to</typeparam>
	/// <returns>Tau converted/rounded to a given type</returns>
	template<typename T = long double>
	constexpr T tau(void) noexcept {
		return T(6.28318530717958647692L);
	}

	/// <summary>
	/// Engine's pi*2 variable (tau). Another name for ace::math::tau()
	/// </summary>
	/// <typeparam name="T">Type to convert tau to</typeparam>
	/// <returns>Tau converted/rounded to a given type</returns>
	template<typename T = long double>
	constexpr T piDouble(void) noexcept {
		return ace::math::tau<T>();
	}

	/// <summary>
	/// Engine's pi/2 (half) value.
	/// </summary>
	/// <typeparam name="T">Type to convert pi/2 to</typeparam>
	/// <returns>Pi/2 converted/rounded to a given type</returns>
	template<typename T = long double>
	constexpr T piHalf(void) noexcept {
		return T(1.57079632679489661923L);
	}

	/// <summary>
	/// Engine's pi/4 (quarter) value.
	/// </summary>
	/// <typeparam name="T">Type to convert pi/4 to</typeparam>
	/// <returns>Pi/4 converted/rounded to a given type</returns>
	template<typename T = long double>
	constexpr T piQuarter(void) noexcept {
		return T(0.785398163397448309616L);
	}


	/// <summary>
	/// Engine's euler number value.
	/// </summary>
	/// <typeparam name="T">Type to convert e to</typeparam>
	/// <returns>e converted/rounded to a given type</returns>
	template<typename T = long double>
	constexpr T e(void) noexcept {
		return T(2.71828182845904523536L);
	}

	/// <summary>
	/// Engine's sqrt(2) value.
	/// </summary>
	/// <typeparam name="T">Type to convert sqrt(2) to</typeparam>
	/// <returns>sqrt(2) converted/rounded to a given type</returns>
	template<typename T = long double>
	constexpr T sqrtOf2(void) noexcept {
		return T(1.41421356237309504880L);
	}

	/// <summary>
	/// Calculates the sine of the given degrees.
	/// </summary>
	/// <param name="degrees">Degree number to calculate sine from</param>
	/// <returns>Float of sine results</returns>
	template<typename T = long double>
	inline T sinDeg(const T degrees) noexcept {
		return std::sin(torad(degrees, T));
	}

	/// <summary>
	/// Calculates the cosine of the given degrees.
	/// </summary>
	/// <param name="degrees">Degree number to calculate cosine from</param>
	/// <returns>Float of cosine results</returns>
	template<typename T = long double>
	inline T cosDeg(const T degrees) noexcept {
		return std::cos(torad(degrees, T));
	}

	/// <summary>
	/// Calculates the tangent of the given degrees.
	/// </summary>
	/// <param name="degrees">Degree number to calculate tangent from</param>
	/// <returns>Float of tangent results</returns>
	template<typename T = long double>
	inline T tanDeg(const T degrees) noexcept {
		return std::tan(torad(degrees, T));
	}

	/// <summary>
	/// Calculates the cotangent of the given degrees.
	/// </summary>
	/// <param name="degrees">Degree number to calculate cotangent from</param>
	/// <returns>Float of cotangent results</returns>
	template<typename T = long double>
	inline T cotDeg(const T degrees) noexcept {
		return 1 / std::tan(torad(degrees, T));
	}

	/// <summary>
	/// Calculates the cosecant of the given degrees.
	/// </summary>
	/// <param name="degrees">Degree number to calculate cosecant from</param>
	/// <returns>Float of cosecant results</returns>
	template<typename T = long double>
	inline T cscDeg(const T degrees) noexcept {
		return 1 / std::sin(torad(degrees, T));
	}

	/// <summary>
	/// Calculates the secant of the given degrees.
	/// </summary>
	/// <param name="degrees">Degree number to calculate secant from</param>
	/// <returns>Float of secant results</returns>
	template<typename T = long double>
	inline T secDeg(const T degrees) noexcept {
		return 1 / std::cos(torad(degrees, T));
	}
	
	/// <summary>
	/// Calculates the absolute value of a given number.
	/// </summary>
	/// <typeparam name="T">The type of the number</typeparam>
	/// <param name="num">The value to calculate absolute value of</param>
	/// <returns>absolute value of the float as the type T</returns>
	template<typename T>
	constexpr inline T absval(const T num) noexcept {
		return (num == 0) ? 0 : // work with signed 0's...if they exist
			(num < 0) ? -num : num;
	}

	/// <summary>
	/// Checks if given 2 floats of type T are equal, using given epsilon(must not go further than float epsilon).
	/// </summary>
	/// <typeparam name="T">The type of the float</typeparam>
	/// <param name="num">The first float to compare</param>
	/// <param name="num2">The second float to compare</param>
	/// <param name="_epsilon">The epsilon to compare against. Defaults to the std::numeric_limits<T>::epsilon()</param>
	/// <returns>True if both numbers are equal, false otherwise</returns>
	template<typename T = long double>
	constexpr bool fequals(const T num, const T num2, const T _epsilon = std::numeric_limits<T>::epsilon()) noexcept {
		static_assert(std::is_floating_point<T>::value, "Cannot use non-float types in ace::math::fequals()!");
		const T dif = ace::math::absval<T>(num - num2);
		return (dif) < _epsilon;
	}


	/// <summary>
	/// Checks if given 2 numbers are equal, a generic function for all types.
	/// @note If the T type is a float, returns the result of ace::math::fequals() with the default epsilon values
	/// </summary>
	/// <typeparam name="T">The type of the numbers passed</typeparam>
	/// <param name="num">The first number to compare</param>
	/// <param name="num2">The second number to compare</param>
	/// <returns>True if the two numbers are equal, false otherwise</returns>
	template<typename T>
	constexpr bool equals(const T num, const T num2) noexcept {
		if constexpr (std::is_floating_point<T>::value) {
			return ace::math::fequals(num, num2);
		}
		else {
			return (num == num2);
		}
	}

	/// <summary>
	/// Newton's method sqrt implementation, compatible with constexpr evaluation.
	/// </summary>
	/// <typeparam name="T">The type of the variable to calculate it with</typeparam>
	/// <param name="num">The value to calculate the square root from</param>
	/// <returns>If the num is positive -> square root value from passed value of type T; -1 (as the type T) otherwise</returns>
	template<typename T = long double>
	constexpr T sqrt(const T num) noexcept {
		if (num < 0) {
			return T(-1);
		}

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

	/// <summary>
	/// Calculate the integer power of a given number
	/// </summary>
	/// <typeparam name="T">The type of the variable to calculate it with</typeparam>
	/// <param name="num">The value to raise to power</param>
	/// <param name="power">Integer power to raise the number to</param>
	/// <returns>The [num] of type T raised to power [power]</returns>
	template<typename T = long double>
	constexpr T intPow(const T num, int power) {

		//switch to save up on conditionals and jumps
		//yeah microoptimisation, but it's fine, doesn't hurt
		
		if(num == 0) { return std::numeric_limits<T>::infinity(); } // 1/0 -- infinity to comply with floats 
		if(num == 1) { return T(1); } // 1 to power of anything is still 1

		switch (power) {

			case -1:
				return T(1) / num;

			case 0: // power is 0, anything to power 0 is 1
				return 1;
				break;

			case 1: // power is 0, anything to power 1 is itself
				return num;
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
	/// Calculate the n'th root of a number.
	/// Thanks, Newton!
	/// </summary>
	/// <typeparam name="T">The type of the variable to calculate it with</typeparam>
	/// <param name="num">The number to take a root of</param>
	/// <param name="rtNum">The root degree to calculate</param>
	/// <returns>The rtNum'th root of the number num of type T</returns>
	template<typename T = long double>
	constexpr T root(const T num, const uint rtNum) noexcept {
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

	/// <summary>
	/// Gets the biggest value between given two values.
	/// </summary>
	/// <typeparam name="T">Type of the values</typeparam>
	/// <param name="a">The first value to compare</param>
	/// <param name="b">The second value to compare</param>
	/// <returns>Value of a if it is bigger than b; value of b otherwise</returns>
	template<typename T>
	constexpr T max(const T& a, const T& b) noexcept {
		return (a > b) ? a : b;
	}

	/// <summary>
	/// Gets the smallest value between given two values.
	/// </summary>
	/// <typeparam name="T">Type of the values</typeparam>
	/// <param name="a">The first value to compare</param>
	/// <param name="b">The second value to compare</param>
	/// <returns>Value of a if it is smaller than b; value of b otherwise</returns>
	template<typename T>
	constexpr T min(const T& a, const T& b) noexcept {
		return (a < b) ? a : b;
	}

	/// <summary>
	/// Calculate the length of the given integer number.
	/// </summary>
	/// <typeparam name="T">The type of the integer number</typeparam>
	/// <param name="num">The number to calculate the length of</param>
	/// <returns>Unsigned int of the length of the passed number</returns>
	template<typename T>
	constexpr unsigned int lengthOfInt(const T num) noexcept {
		static_assert(std::is_integral<T>::value, "Cannot use non-integral types in the ace::math::lengthOfInt()");
		unsigned int dig = 1;
		T numtemp = num;
		while (numtemp /= 10)
			dig++;
		return dig;
	}

	/// <summary>
	/// Calculate the length of the given float number.
	/// </summary>
	/// <typeparam name="T">The type of the float number</typeparam>
	/// <param name="num">The number to calculate the length of</param>
	/// <returns>Unsigned int of the length of the passed number</returns>
	template<typename T>
	constexpr unsigned int lengthOfFloat(const T num) noexcept {
		static_assert(std::is_floating_point<T>::value, "Cannot use non-float types in the ace::math::lengthOfFloat()!");
		return ((num == 0) ? 1 : (unsigned int)std::log10(std::abs(num))) + 1;
	}

	/// <summary>
	/// Computes the linear inter/extrapolation with the given values a and b and a value c between or outside of them
	/// @note If c is between 0 and 1, computes linear interpolation
	/// @note if c is outside of 0 and 1, computes linear extrapolation
	/// </summary>
	/// <typeparam name="T">The type to calculate it with</typeparam>
	/// <param name="a">The value of a (lower known boundary)</param>
	/// <param name="b">The value of b (higher known boundary)</param>
	/// <param name="c">The value for inter/extrapolation (distance from value "a")</param>
	/// <returns>The value of type T of the inter/extrapolated value</returns>
	template<typename T = long double>
	constexpr T lerp(const T a, const T b, const T c) {
		return  a + c * (b - a);
	}

	/// <summary>
	/// Calculates the hypotenuse length from given values a and b
	/// </summary>
	/// <typeparam name="T">The type of the variable to calculate it with</typeparam>
	/// <param name="a">The length of side a in the right triangle</param>
	/// <param name="b">The length of side b in the right triangle</param>
	/// <returns>The length of hypotenuse of type T with given lengths of a and b sides</returns>
	template<typename T = long double>
	constexpr T hypot(const T a, const T b) {
		return ace::math::sqrt<T>(a * a + b * b);
	}

	/// <summary>
	/// Checks whether the float is "infinite"
	/// </summary>
	/// <typeparam name="T">The type of a floating point number</typeparam>
	/// <param name="num">The number to check</param>
	/// <returns>True if the number is infinite; otherwise false</returns>
	template<typename T = long double>
	constexpr bool isInf(const T num) noexcept {
		static_assert(std::is_floating_point<T>::value, "Cannot use non-float types in ace::math::isInf()!");
		return (num == std::numeric_limits<T>::infinity());
	}

	/// <summary>
	/// Checks whether the float is "NaN"
	/// </summary>
	/// <typeparam name="T">The type of a floating point number</typeparam>
	/// <param name="num">The number to check</param>
	/// <returns>True if the number is NaN; otherwise false</returns>
	template<typename T = long double>
	constexpr bool isNan(const T num) noexcept {
		static_assert(std::is_floating_point<T>::value, "Cannot use non-float types in ace::math::isNan()!");
		return (num != num);
	}

	/// <summary>
	/// Checks whether the float is finite (not infinity or NaN)
	/// </summary>
	/// <typeparam name="T">The type of a floating point number</typeparam>
	/// <param name="num">The number to check</param>
	/// <returns>True if the number is finite; otherwise false</returns>
	template<typename T = long double>
	constexpr bool isFinite(const T num) noexcept {
		static_assert(std::is_floating_point<T>::value, "Cannot use non-float types in ace::math::isFinite()!");
		return !(ace::math::isInf<T>(num) || ace::math::isNan<T>(num));
	}

	/// <summary>
	/// Rounds the given float of type Y to integer of type T.
	/// @warning It breaks if the num value is more than the max value of T!
	/// @note Float type Y should be a valid value (not NAN or inf)
	/// </summary>
	/// <typeparam name="T">The type of the integer to round to</typeparam>
	/// <typeparam name="Y">The type of the float to round</typeparam>
	/// <param name="num">The floating point number to round</param>
	/// <returns>The rounded integer of type T, from the given float number</returns>
	template<typename T = llint, typename Y = long double>
	constexpr T roundToInt(const Y num) noexcept {
		if constexpr (std::is_integral<Y>::value) { return num; } // it's an int anyway
		else {
			//static_assert(std::is_floating_point<Y>::value, "Cannot use non-float types as the float type in ace::math::roundtoint()!");
			return (num < 0) ? T(num + Y(-0.5f)) : T(num + Y(0.5f));
		}
	}

	/// <summary>
	/// Floor's the given float of type Y and converts to integer of type T.
	/// @warning It breaks if the num value is more than the max value of T!
	/// @note Value of num should be finite (not NAN or inf)
	/// @note If num is positive, it truncates the decimal digits (towards zero); if negative, it ceil's the decimals (away from zero)
	/// @note Exists here while C++23's constexpr floor is still being adopted
	/// @todo Change the body of the function to use std::floor() instead, when it gets constexpr (after c++23)
	/// </summary>
	/// <typeparam name="T">The type of the integer to floor to</typeparam>
	/// <typeparam name="Y">The type of the float to floor</typeparam>
	/// <param name="num">The floating point number to floor</param>
	/// <returns>The floored integer of type T, from the given float number</returns>
	template<typename T = llint, typename Y = long double>
	constexpr T floorToInt(const Y num) noexcept {
		if constexpr (std::is_integral<Y>::value) { return num; } // it's an int anyway
		else {
			return (num < 0 && !ace::math::fequals<Y>(T(num), num)) ? T(num) - 1 : T(num);
		}
	}

	/// <summary>
	/// Ceil's the given float of type Y and converts to integer of type T.
	/// @warning It breaks if the num value is more than the max value of T!
	/// @note Value of num should be finite (not NAN or inf)
	/// @note Exists here while C++23's constexpr ceil is still being adopted
	/// @todo Change the body of the function to use std::ceil() instead, when it gets constexpr (after c++23)
	/// </summary>
	/// <typeparam name="T">The type of the integer to ceil to</typeparam>
	/// <typeparam name="Y">The type of the float to ceil</typeparam>
	/// <param name="num">The floating point number to ceil</param>
	/// <returns>The ceiled integer of type T, from the given float number</returns>
	template<typename T = llint, typename Y = long double>
	constexpr T ceilToInt(const Y num) noexcept {
		if constexpr (std::is_integral<Y>::value) { return num; } // it's an int anyway
		else {
			return (ace::math::fequals<Y>(num, T(num))) ? T(num) : T(num) + 1;
		}
	}
	
	/// <summary>
	/// Truncates the given float of type Y and converts to integer of type T.
	/// @warning It breaks if the num value is more than the max value of T!
	/// @note Value of num should be finite (not NAN or inf)
	/// @note Exists here while C++23's constexpr truncate is still being adopted
	/// @todo Change the body of the function to use std::trunc() instead, when it gets constexpr (after c++23)
	/// </summary>
	/// <typeparam name="T">The type of the integer to truncate to</typeparam>
	/// <typeparam name="Y">The type of the float to truncate</typeparam>
	/// <param name="num">The floating point number to truncate</param>
	/// <returns>The truncated integer of type T, from the given float number</returns>
	template<typename T = llint, typename Y = long double>
	constexpr T truncToInt(const Y num) noexcept {
		if constexpr (std::is_integral<Y>::value) { return num; } // it's an int anyway
		else {
			return T(num);
		}

	}
}

#endif // ENGINE_MATH_HPP
