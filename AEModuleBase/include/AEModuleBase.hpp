
/*! @file AEModuleBase/include/AEModuleBase.hpp
 *  This file contains the code for the module that all (utility) modules base on in the ArtyK's Console Engine class.
 *
 *  Should not cause everything to break :)
 */

#pragma once

#ifndef ENGINE_AEMODULEBASE_HPP
#define ENGINE_AEMODULEBASE_HPP

#include "include/AETypedefs.hpp"
#include "include/AEFlags.hpp"
#include <atomic>
#include <string>
#include <string_view>

/// Adds module name to inherit from __AEModuleBase
#define REGISTER_MODULE2(T) template<> const inline std::string_view __AEModuleBase<T>::m_sModulename = #T;

#define REGISTER_MODULE(T) static inline std::string_view getModuleName(void) noexcept { \
	static_assert(sizeof(#T) <= AELE_MODULENAME_SIZE, "The module name is TOO BIG! (check AELE_MODULENAME_SIZE in AEMisc/include/AEFlags.hpp");\
	return #T; }


/// Global "no error" return flag for all engine modules
#define ENGINE_MODULE_ERR_NOERROR 0

// Usage to register the class:
// add REGISTER_MODULE(<classname>)
// after the class definition

/// <summary>
/// @brief This is a base class for all basic modules, meaning that the module is not using other modules to work. 
/// @note Intended to be inherited from in class declaration, not direct usage.
/// Usage: class [classname] : public __AEModuleBase<[classname]>
/// </summary>
/// <typeparam name="T">Inheriting module's class name</typeparam>
template<typename T>
class __AEModuleBase
{
public:

	/// <summary>
	/// Basic constructor that increases module count
	/// </summary>
	__AEModuleBase(void) noexcept {
		this->m_ullModulesAlive++;
	}

	/// <summary>
	/// Basic copy constructor that increases module count
	/// </summary>
	/// <param name="val">The value to be copied...useless since we have nothing to copy lol (but we need it)</param>
	__AEModuleBase(const __AEModuleBase<T>& val) noexcept {
		*this = val;
		m_ullModulesAlive++;
	}

	/// <summary>
	/// Basic destructor that decreases module count
	/// </summary>
	virtual ~__AEModuleBase(void) noexcept {
		m_ullModulesAlive--;
	}
	
	/// <summary>
	/// Returns the amount of instances of the module currently existing
	/// </summary>
	/// <returns>Unsigned long long of the module amount</returns>
	static inline ullint getModuleAmount(void) noexcept {
		return m_ullModulesAlive.load();
	}
	
	/// <summary>
	/// Returns the name of the registered module
	/// </summary>
	/// <returns></returns>
	static constexpr std::string_view getModuleName(void) noexcept;

protected:

	/// <summary>
	/// Amount of alive module instances
	/// </summary>
	static inline std::atomic<ullint> m_ullModulesAlive = 0;
};

constexpr std::string_view getModuleName2(void) noexcept{
	return "__AEModuleBase<>";
}


//REGISTER_MODULE(__AEModuleBase)

#endif // !ENGINE_AEMODULEBASE_HPP
