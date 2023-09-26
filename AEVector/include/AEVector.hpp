#pragma once

#ifndef ENGINE_VECTOR_HPP
#define ENGINE_VECTOR_HPP

#include "include/AEMath.hpp"

//TODO: migrate code from old engine
//TODO: add docs
//TODO: clean up code
//TODO: make code faster/easier to read


#define AEVEC_WRAP_DIMENSIONS

template<typename T, const std::size_t dimAmount>
struct AEVector {

	T dims[dimAmount]{};
		

//internal functions 
	//normal value accessor
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
	constexpr const T& operator[](const std::size_t index) const {
		return this->dims[
#ifdef AEVEC_WRAP_DIMENSIONS
			index % dimAmount
#else
			index
#endif
		];
	}

	//equality operator. two must have same amount of dimensions
	template<typename Y>
	constexpr bool operator==(const AEVector<Y, dimAmount>& two) const {

		for (std::size_t i = 0; i < dimAmount; i++) {
			if (this->dims[i] != two.dims[i]) {
				return false;
			}
		}

		return true;
	}

	//inequality operator. two must have same amount of dimensions
	template<typename Y>
	constexpr bool operator!=(const AEVector<Y, dimAmount>& two) const {
		return !this->operator==<Y>(two);
	}


//math functions
	//vector magnitude
	template<typename F = long double>
	constexpr F magnitude(void) const {
		static_assert(std::is_floating_point<F>::value, "Cannot use non-float types in AEVector::magnitude()!");
		F temp = 0;
		for (size_t i = 0; i < dimAmount; i++) {
			temp += this->dims[i] * this->dims[i];
		}

		return ace::math::fsqrt<F>(temp);
	}

	//vector length (same as magnitude)
	template<typename F = long double>
	constexpr F length(void) const {
		return this->magnitude<F>();
	}

	//normalise the dimension
	template<typename F = long double>
	constexpr F normDim(const std::size_t index) const {
		return this->operator[](index)/this->magnitude<F>();
	}

	//normalise the vector
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

	template<typename F = long double, typename Y>
	constexpr F dotProduct(const AEVector<Y, dimAmount>& two) {
		F temp = 0;
		for (std::size_t i = 0; i < dimAmount; i++) {
			temp += this->dims[i] * two.dims[i];
		}
		return temp;
	}

	constexpr static AEVector<T, dimAmount> zero(void) {
		return AEVector<T, dimAmount>{};
	}

	template<typename Y>
	constexpr AEVector<T, dimAmount>& operator+=(const AEVector<Y, dimAmount>& two) {
		for (std::size_t i = 0; i < dimAmount; i++) {
			this->dims[i] += two.dims[i];
		}
		return *this;
	}

	template<typename Y>
	constexpr const AEVector<T, dimAmount> operator+(const AEVector<Y, dimAmount>& two) const {
		AEVector<T, dimAmount> a = *this;
		return a.operator+=(two);
	}
	
	template<typename Y>
	constexpr AEVector<T, dimAmount>& operator*=(const Y two) {
		for (std::size_t i = 0; i < dimAmount; i++) {
			this->dims[i] *= two;
		}
		return *this;
	}

	template<typename Y>
	constexpr const AEVector<T, dimAmount> operator*(const Y two) const {
		AEVector<T, dimAmount> a = *this;
		return a.operator*=(two);
	}



};

typedef AEVector<int, 2> AEVec2Int;
typedef AEVector<int, 3> AEVec3Int;
typedef AEVector<float, 2> AEVec2Float;
typedef AEVector<float, 3> AEVec3Float;


#endif // !ENGINE_VECTOR_HPP

