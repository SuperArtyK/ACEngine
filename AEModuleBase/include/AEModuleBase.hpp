
/** @file AEModuleBase/include/AEModuleBase.hpp
 *  This file contains the code for the module that all (utility) modules 
 *  base on in the ArtyK's Console Engine class.
 *
 *  Should not cause everything to break :)
 */

#pragma once

#ifndef ENGINE_AEMODULEBASE_HPP
#define ENGINE_AEMODULEBASE_HPP

#include "include/AETypedefs.hpp"
#include <string>
#include <atomic>

//file meaning: base classes for all modules


template <typename T>
constexpr const char* const getTypeName(void) noexcept { return "getTypeName"; }

/// adds module name to inherit from __AEModuleBase
#define REGISTER_CLASS(T) template <> constexpr const char* const getTypeName<T>() noexcept { return #T; }

// Usage to register the class:
// add REGISTER_CLASS(<classname>)
// after the class definition

/// <summary>
/// @brief This is a base class for all basic modules, meaning that the module is not using other modules to work. 
/// Intended to be inherited from in class declaration, not direct usage.
/// Usage: class [classname] : public __AEModuleBase<[classname]>
/// </summary>
/// <typeparam name="T">Module's class name</typeparam>
template<typename T>
class __AEModuleBase
{
public:

	/// <summary>
	/// Basic constructor that increases module count
	/// </summary>
	__AEModuleBase() noexcept {
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
	virtual ~__AEModuleBase() noexcept {
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
	static inline const char* const getModuleName(void) noexcept {
		return m_sModulename;
	}

protected:

	/// <summary>
	/// Name of the module (class), same as in code
	/// @todo Find a way to make it constexpr and compile-time evaluated 
	/// </summary>
	static const char* const m_sModulename;
	/// <summary>
	/// amount of alive module instances
	/// </summary>
	static inline std::atomic<ullint> m_ullModulesAlive = 0;
};

/// sets up the static variable values for the base class (and inherited classes)
template<typename T>
const char* const __AEModuleBase<T>::m_sModulename = getTypeName<T>();

//REGISTER_CLASS(__AEModuleBase)

#endif // !ENGINE_AEMODULEBASE_HPP
