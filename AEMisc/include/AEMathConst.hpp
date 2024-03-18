
/*! @file AEMisc/include/AEMathConst.hpp
 *
 *  @brief This file contains the engine's **math constants**.
 *
 *	The brief description says it all -- this file has the math constants in the engine.
 *	
 *	All those pi's, tau's, sqrt of 2's, etc, which you can call, and cast to the necessary type.
 *
 *  @date 2024/03/14
 *
 *	@author	Artemii Kozhemiak (SuperArtyK)
 *
 *	@copyright MIT License
 *
 *  *Should not cause everything to break :)*
 */

#pragma once

#ifndef ENGINE_MATH_CONST_HPP
#define ENGINE_MATH_CONST_HPP

/// This namespace contains the engine's **math constants**.
/// The brief description says it all-- this file has the math constants in the engine. <br>
/// All those pi's, tau's, sqrt of 2's, etc, which you can call, and cast to the necessary type.
namespace ace::math::cst {

	/// <summary>
	/// Engine's **pi** value.
	/// </summary>
	/// <typeparam name="T">Type to convert the constant to</typeparam>
	/// <returns>
	///		The constant converted/rounded to a given type
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T pi(void) noexcept {
		return T(3.14159265358979323846L);
	}

	/// <summary>
	/// Engine's **tau (pi*2)** value.
	/// </summary>
	/// <typeparam name="T">Type to convert the constant to</typeparam>
	/// <returns>
	///		The constant converted/rounded to a given type
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T tau(void) noexcept {
		return T(6.28318530717958647693L);
	}

	/// <summary>
	/// Engine's **pi*2** (tau) value.
	/// 
	/// Just another name for ace::math::tau()
	/// </summary>
	/// <typeparam name="T">Type to convert the constant to</typeparam>
	/// <returns>
	///		The constant converted/rounded to a given type
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T piDouble(void) noexcept {
		return ace::math::cst::tau<T>();
	}

	/// <summary>
	/// Engine's **pi/2** (half) value.
	/// </summary>
	/// <typeparam name="T">Type to convert the constant to</typeparam>
	/// <returns>
	///		The constant converted/rounded to a given type
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T piHalf(void) noexcept {
		return T(1.57079632679489661923L);
	}

	/// <summary>
	/// Engine's **pi/4** (quarter) value.
	/// </summary>
	/// <typeparam name="T">Type to convert the constant to</typeparam>
	/// <returns>
	///		The constant converted/rounded to a given type
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T piQuarter(void) noexcept {
		return T(0.78539816339744830962L);
	}

	/// <summary>
	/// Engine's **pi^2** (squared) value.
	/// </summary>
	/// <typeparam name="T">Type to convert the constant to</typeparam>
	/// <returns>
	///		The constant converted/rounded to a given type
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T piSquared(void) noexcept {
		return T(9.86960440108935861883L);
	}

	/// <summary>
	/// Engine's **1/pi** (inverse) value.
	/// </summary>
	/// <typeparam name="T">Type to convert the constant to</typeparam>
	/// <returns>
	///		The constant converted/rounded to a given type
	///	</returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T piInv(void) noexcept {
		return T(0.31830988618379067154L);
	}

	/// <summary>
	/// Engine's **log2(e)** value.
	/// </summary>
	/// <typeparam name="T">Type to convert the constant to</typeparam>
	/// <returns>
	///		The constant converted/rounded to a given type
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T log2e(void) noexcept {
		return T(1.44269504088896340736L);
	}

	/// <summary>
	/// Engine's **log10(e)** value.
	/// </summary>
	/// <typeparam name="T">Type to convert the constant to</typeparam>
	/// <returns>
	///		The constant converted/rounded to a given type
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T log10e(void) noexcept {
		return T(0.43429448190325182765L);
	}

	/// <summary>
	/// Engine's **ln(2)** value.
	/// </summary>
	/// <typeparam name="T">Type to convert the constant to</typeparam>
	/// <returns>
	///		The constant converted/rounded to a given type
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T ln2(void) noexcept {
		return T(0.69314718055994530942L);
	}

	/// <summary>
	/// Engine's **ln(10)** value.
	/// </summary>
	/// <typeparam name="T">Type to convert the constant to</typeparam>
	/// <returns>
	///		The constant converted/rounded to a given type
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T ln10(void) noexcept {
		return T(2.30258509299404568402L);
	}

	/// <summary>
	/// Engine's **sqrt(2)** value.
	/// </summary>
	/// <typeparam name="T">Type to convert the constant to</typeparam>
	/// <returns>
	///		The constant converted/rounded to a given type
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T sqrtOf2(void) noexcept {
		return T(1.41421356237309504880L);
	}

	/// <summary>
	/// Engine's **sqrt(3)** value.
	/// </summary>
	/// <typeparam name="T">Type to convert the constant to</typeparam>
	/// <returns>
	///		The constant converted/rounded to a given type
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T sqrtOf3(void) noexcept {
		return T(1.73205080756887729353L);
	}

	/// <summary>
	/// Engine's **1/sqrt(2)** value.
	/// </summary>
	/// <typeparam name="T">Type to convert the constant to</typeparam>
	/// <returns>
	///		The constant converted/rounded to a given type
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T sqrtOf2Inv(void) noexcept {
		return T(0.70710678118654752440L);
	}

	/// <summary>
	/// Engine's **1/sqrt(3)** value.
	/// </summary>
	/// <typeparam name="T">Type to convert the constant to</typeparam>
	/// <returns>
	///		The constant converted/rounded to a given type
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T sqrtOf3Inv(void) noexcept {
		return T(0.57735026918962576451L);
	}

	/// <summary>
	/// Engine's **sqrt(pi)** value.
	/// </summary>
	/// <typeparam name="T">Type to convert the constant to</typeparam>
	/// <returns>
	///		The constant converted/rounded to a given type
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T sqrtOfPi(void) noexcept {
		return T(1.77245385090551602730L);
	}

	/// <summary>
	/// Engine's **1/sqrt(pi)** value.
	/// </summary>
	/// <typeparam name="T">Type to convert the constant to</typeparam>
	/// <returns>
	///		The constant converted/rounded to a given type
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T sqrtOfPiInv(void) noexcept {
		return T(0.56418958354775628695L);
	}

	/// <summary>
	/// Engine's **euler's number** value.
	/// </summary>
	/// <typeparam name="T">Type to convert the constant to</typeparam>
	/// <returns>
	///		The constant converted/rounded to a given type
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T e(void) noexcept {
		return T(2.71828182845904523536L);
	}

	/// <summary>
	/// Engine's **euler's gamma function** (euler's constant) value.
	/// 
	/// @note Don't confuse with euler's number ace::math::e()
	/// </summary>
	/// <typeparam name="T">Type to convert the constant to</typeparam>
	/// <returns>
	///		The constant converted/rounded to a given type
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T eGamma(void) noexcept {
		return T(0.57721566490153286061L);
	}

	/// <summary>
	/// Engine's **golden ratio** (phi) value.
	/// </summary>
	/// <typeparam name="T">Type to convert the constant to</typeparam>
	/// <returns>
	///		The constant converted/rounded to a given type
	/// </returns>
	template<typename T = long double>
	[[nodiscard]] constexpr T phi(void) noexcept {
		return T(1.61803398874989484821L);
	}
}



#endif // !ENGINE_MATH_CONST_HPP
