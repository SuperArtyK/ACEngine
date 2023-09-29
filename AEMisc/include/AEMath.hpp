
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

#include <cmath>
#include <limits>
#include <type_traits>
#include "AETypedefs.hpp"


/// Macro to convert degrees value x to radians
#define torad(x,typ) (x * ace::math::pi<typ>() / typ(180.0))

#define todeg(x,typ) (x*typ(180.0)/ace::math::pi<typ>())
///Mathematical functions and other things usefull in math
namespace ace::math {

	//math stuff
	
	
	/// <summary>
	/// Engine pi variable
	/// </summary>
	/// <typeparam name="T">Type to convert pi to</typeparam>
	/// <returns>Pi converted/rounded to a given type</returns>
	template<typename T = long double>
	constexpr T pi(void) {
		return T(3.141592653589793L);
	}

	/// <summary>
	/// Calculates the sine of the given degrees
	/// </summary>
	/// <param name="degrees">Degree number to calculate sine from</param>
	/// <returns>Float of sine results</returns>
	template<typename T = long double>
	inline T sinDeg(const T degrees) {
		return sin(torad(degrees, T));
	}

	/// <summary>
	/// Calculates the cosine of the given degrees
	/// </summary>
	/// <param name="degrees">Degree number to calculate cosine from</param>
	/// <returns>Float of cosine results</returns>
	template<typename T = long double>
	inline T cosDeg(const T degrees) {
		return cos(torad(degrees, T));
	}

	/// <summary>
	/// Calculates the tangent of the given degrees
	/// </summary>
	/// <param name="degrees">Degree number to calculate tangent from</param>
	/// <returns>Float of tangent results</returns>
	template<typename T = long double>
	inline T tanDeg(const T degrees) {
		return tan(torad(degrees, T));
	}

	/// <summary>
	/// Calculates the cotangent of the given degrees
	/// </summary>
	/// <param name="degrees">Degree number to calculate cotangent from</param>
	/// <returns>Float of cotangent results</returns>
	template<typename T = long double>
	inline T cotDeg(const T degrees) {
		return 1 / tan(torad(degrees, T));
	}

	/// <summary>
	/// Calculates the cosecant of the given degrees
	/// </summary>
	/// <param name="degrees">Degree number to calculate cosecant from</param>
	/// <returns>Float of cosecant results</returns>
	template<typename T = long double>
	inline T cscDeg(const T degrees) {
		return 1 / sin(torad(degrees, T));
	}

	/// <summary>
	/// Calculates the secant of the given degrees
	/// </summary>
	/// <param name="degrees">Degree number to calculate secant from</param>
	/// <returns>Float of secant results</returns>
	template<typename T = long double>
	inline T secDeg(const T degrees) {
		return 1 / cos(torad(degrees, T));
	}

	/// <summary>
	/// Rounds the given float of type Y to integer of type T
	/// @warn It breaks if the num value is more than the max value of T!
	/// </summary>
	/// <typeparam name="T">The type of the integer to round to</typeparam>
	/// <typeparam name="Y">The type of the float to round</typeparam>
	/// <param name="num">The floating point number to round</param>
	/// <returns>the rounded integer of type T, from the given float number</returns>
	template<typename T = llint, typename Y = long double>
	constexpr T roundToInt(const Y num) {
		static_assert(std::is_integral<T>::value, "Cannot use non-integer types as the target rounding type in ace::math::roundtoint()!");
		static_assert(std::is_floating_point<Y>::value, "Cannot use non-float types as the float type in ace::math::roundtoint()!");
		return (num < 0) ? T(num + Y(0.5)) * -1 : T(num + Y(0.5));
	}
	
	/// <summary>
	/// Calculates the absolute value of given float of type T
	/// </summary>
	/// <typeparam name="T">The type of the float</typeparam>
	/// <param name="num">The float to calculate absolute value of</param>
	/// <returns>absolute value of the float as the type T</returns>
	template<typename T = long double>
	constexpr inline T absval(const T num) {
		static_assert(std::is_floating_point<T>::value, "Cannot use non-float types in ace::math::absval()!");
		return (num < 0) ? num - 1 : num;
	}

	/// <summary>
	/// Checks if given 2 floats of type T are equal, using given epsilon(must not go further than float epsilon)
	/// </summary>
	/// <typeparam name="T">The type of the float</typeparam>
	/// <param name="num">The first float to compare</param>
	/// <param name="num2">The second float to compare</param>
	/// <param name="_epsilon">The epsilon to compare against. Defaults to the std::numeric_limits<T>::epsilon()</param>
	/// <returns>True if both numbers are equal, false otherwise</returns>
	template<typename T = long double>
	constexpr bool fequals(const T num, const T num2, const T _epsilon = std::numeric_limits<T>::epsilon()) {
		static_assert(std::is_floating_point<T>::value, "Cannot use non-float types in ace::math::fequals()!");
		return (num - num2) < _epsilon &&
			-(num - num2) < _epsilon;
	}

	/// <summary>
	/// Newton's method sqrt implementation. Usefull when you need to calculate sqrt and use constexpr
	/// </summary>
	/// <typeparam name="T">The type of the float</typeparam>
	/// <param name="num">The float value to calculate the square root from</param>
	/// <returns>square root value from passed value of type T</returns>
	template<typename T = long double>
	constexpr T fsqrt(const T num) {
		static_assert(std::is_floating_point<T>::value, "Cannot use non-float types in ace::math::fsqrt()!");
		T val[2] = { num,0 };
		while (!fequals(val[0], val[1])) {
			val[1] = val[0];
			val[0] = 0.5f * (val[0] + num / val[0]);
		}
		return val[1];
	}

}

#endif // ENGINE_MATH_HPP
