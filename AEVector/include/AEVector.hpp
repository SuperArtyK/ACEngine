
/** @file AEVector/include/AEVector.hpp
 *  This file contains the engine's math vector code.
 *  So you can make more math calculations with it...and things in graphics and games
 *  that need (math) vectors to accomplish
 *
 *  Should not cause everything to break :)
 */

#pragma once

#ifndef ENGINE_VECTOR_HPP
#define ENGINE_VECTOR_HPP

#include "include/AEMath.hpp"
#include "include/AEFlags.hpp"
#include <ostream>
#include <string>



/// <summary>
/// The ArtyK's Engine's mathematical vector -- for mathematically-correct vector operations.
/// Yeah, the mathematical vectors, that are in CS..just labeled arrays of numbers for ease of use.
/// </summary>
/// <typeparam name="T">The numberical type to use in the vector</typeparam>
/// <typeparam name="dimAmount">The amount of dimensions in the vector</typeparam>
template<typename T, const std::size_t dimAmount>
struct AEVector {

public:
	/// The array of the vector's dimension values of the type T and size dimAmount
	T dims[dimAmount]{};
	
	/// <summary>
	/// Returns the zeroed vector (of given dimensions and type)
	/// </summary>
	/// <returns>std::string of values of the vector in brackets</returns>
	static constexpr AEVector<T, dimAmount> zero(void) {
		return AEVector<T, dimAmount>{};
	}

//internal functions 
	//normal value accessor
	/// <summary>
	/// The [] operator for ease of use and wrap functionality
	/// @note If the AEVEC_WRAP_DIMENSIONS is defined, then the index is wrapped around the dimension amount. Example: index 12 in 5-dimensional vector will be wrapped to get the item at index 2
	/// </summary>
	/// <param name="index">The index of the dimension</param>
	/// <returns>Reference to the dimension pointed by index</returns>
	constexpr T& operator[](const std::size_t index) {
		return this->dims[
#ifdef AEVEC_WRAP_DIMENSIONS
			index % dimAmount
#else
			index
#endif
		];
	}

	//const value accessor. For peeking without modifying
	/// <summary>
	/// The [] operator for ease of use and wrap functionality
	/// @note If the AEVEC_WRAP_DIMENSIONS is defined, then the index is wrapped around the dimension amount. Example: index 12 in 5-dimensional vector will be wrapped to get the item at index 2
	/// </summary>
	/// <param name="index">The index of the dimension</param>
	/// <returns>Constant reference to the dimension pointed by index</returns>
	constexpr const T& operator[](const std::size_t index) const {
		return this->dims[
#ifdef AEVEC_WRAP_DIMENSIONS
			index % dimAmount
#else
			index
#endif
		];
	}

	/// <summary>
	/// The equality comparison operator -- compare "this" and another vector
	/// @note The second vector must have the same amount of dimensions (to even compile)
	/// </summary>
	/// <typeparam name="Y">The dimension type in the second vector, normally same as T</typeparam>
	/// <param name="two">The second vector</param>
	/// <returns>True if the vectors are equal, false otherwise</returns>
	template<typename Y = T>
	constexpr bool operator==(const AEVector<Y, dimAmount>& two) const {

		for (std::size_t i = 0; i < dimAmount; i++) {
			if (this->dims[i] != two.dims[i]) {
				return false;
			}
		}

		return true;
	}
	
	/// <summary>
	/// The inequality comparison operator -- compare "this" and another vector
	/// @note The second vector must have the same amount of dimensions (to even compile)
	/// </summary>
	/// <typeparam name="Y">The dimension type in the second vector</typeparam>
	/// <param name="two">The second vector</param>
	/// <returns>True if the vectors are not equal, false otherwise</returns>
	template<typename Y = T>
	constexpr bool operator!=(const AEVector<Y, dimAmount>& two) const {
		return !this->operator==<Y>(two);
	}

	/// <summary>
	/// The addition-assignment operator -- add another vector to "this" 
	/// @note The second vector must have the same amount of dimensions (to even compile)
	/// </summary>
	/// <typeparam name="Y">The dimension type in the second vector</typeparam>
	/// <param name="two">The second vector</param>
	/// <returns>The reference to the resulting vector after the operation</returns>
	template<typename Y = T>
	constexpr AEVector<T, dimAmount>& operator+=(const AEVector<Y, dimAmount>& two) {
		for (std::size_t i = 0; i < dimAmount; i++) {
			this->dims[i] += two.dims[i];
		}
		return *this;
	}

	/// <summary>
	/// The addition operator -- "add" this and another vector
	/// @note The second vector must have the same amount of dimensions (to even compile)
	/// </summary>
	/// <typeparam name="Y">The dimension type in the second vector</typeparam>
	/// <param name="two">The second vector</param>
	/// <returns>The value of the resulting vector after the operation</returns>
	template<typename Y = T>
	constexpr const AEVector<T, dimAmount> operator+(const AEVector<Y, dimAmount>& two) const {
		AEVector<T, dimAmount> a = *this;
		return a.operator+=(two);
	}

	/// <summary>
	/// The multiplication-assignment operator -- multiply "this" by a scalar
	/// </summary>
	/// <typeparam name="Y">The dimension type in the second vector</typeparam>
	/// <param name="two">The second vector</param>
	/// <returns>The reference to the resulting vector after the operation</returns>
	template<typename Y>
	constexpr AEVector<T, dimAmount>& operator*=(const Y two) {
		for (std::size_t i = 0; i < dimAmount; i++) {
			this->dims[i] *= two;
		}
		return *this;
	}

	/// <summary>
	/// The multiplication operator -- multiply this and a scalar
	/// </summary>
	/// <typeparam name="Y">The dimension type in the second vector</typeparam>
	/// <param name="two">The second vector</param>
	/// <returns>The value of the resulting vector after the operation</returns>
	template<typename Y>
	constexpr const AEVector<T, dimAmount> operator*(const Y two) const {
		AEVector<T, dimAmount> a = *this;
		return a.operator*=(two);
	}

	/// <summary>
	/// Converts the vector to std::string, of format [a, b, c, ...]
	/// </summary>
	/// <returns>std::string of values of the vector in brackets</returns>
	const std::string toString(void) const {
		std::string str;
		str.reserve(5 + 3 * (dimAmount - 1));

		for (std::size_t i = 0; i < dimAmount - 1; i++) {

			str.append(std::to_string(this->dims[i]));
			str.append(", ");
		}
		str.append(std::to_string(this->dims[dimAmount - 1]));
		str.push_back(']');

		return str;
	}
	
	/// <summary>
	/// The ostream's << operator overload...to output the vector through the ostreams
	/// </summary>
	/// <typeparam name="T">The numberical type to use in the vector</typeparam>
	/// <typeparam name="dimAmount">The amount of dimensions in the vector</typeparam>
	/// <param name="out">The ostream object to output to</param>
	/// <param name="two">The vector to output</param>
	/// <returns>The reference to the resulting ostream object</returns>
	friend inline std::ostream& operator<<(std::ostream& out, const AEVector<T, dimAmount>& two) {
		out << two.toString();
		return out;
	}

//math functions
	/// <summary>
	/// Calculates the magnitude/length of the vector
	/// </summary>
	/// <typeparam name="F">The float type to calculate the magnitude with</typeparam>
	/// <returns>The magnitude of the vector as the float type F</returns>
	template<typename F = long double>
	constexpr F magnitude(void) const {
		return ace::math::sqrt<F>( this->dotProduct<F,T>(*this) );
	}

	/// <summary>
	/// Calculates the normalised value of the given dimension
	/// </summary>
	/// <typeparam name="F">The float type to calculate it all with</typeparam>
	/// <param name="index">The index of the dimension</param>
	/// <returns>The normalised value of the dimension as the float type F</returns>
	template<typename F = long double>
	constexpr F normDim(const std::size_t index) const {
		return this->operator[](index)/this->magnitude<F>();
	}

	/// <summary>
	/// Calculates the normalised vector from the current vector
	/// </summary>
	/// <typeparam name="F">The float type of the resulting normalised vector</typeparam>
	/// <returns>The normalised vector</returns>
	template<typename F = long double>
	constexpr AEVector<F, dimAmount> normalise(void) const {
		//vector magnitude
		const F vecmag = this->magnitude<F>();
		AEVector<F, dimAmount> ret;
		//iterate through all the vector and normalise all the dimensions
		for (std::size_t i = 0; i < dimAmount; i++) {
			ret.dims[i] = dims[i] / vecmag;
		}
		return ret;
	}

	/// <summary>
	/// Calculate the dot product of "this" and another vector
	/// </summary>
	/// <typeparam name="F">The float type of the resulting scalar</typeparam>
	/// <typeparam name="Y">The dimension type in the second vector</typeparam>
	/// <param name="two">The second vector</param>
	/// <returns>The value of the dot product of the type F</returns>
	template<typename F = long double, typename Y = T>
	constexpr F dotProduct(const AEVector<Y, dimAmount>& two) const {
		F temp = 0;
		for (std::size_t i = 0; i < dimAmount; i++) {
			temp += (this->dims[i] * two.dims[i]);
		}
		return temp;
	}
	
};

/// The shortcut typedef for the 2-dimensional integer vector
typedef AEVector<int, 2> AEVec2Int;
/// The shortcut typedef for the 3-dimensional integer vector
typedef AEVector<int, 3> AEVec3Int;
/// The shortcut typedef for the 2-dimensional floating point vector
typedef AEVector<float, 2> AEVec2Float;
/// The shortcut typedef for the 3-dimensional floating point vector
typedef AEVector<float, 3> AEVec3Float;


#endif // !ENGINE_VECTOR_HPP

