
/** @file AEModuleBase/include/AEModuleBase.hpp
 *  This file contains the code for the module that all (utility) modules base on in the ArtyK's Console Engine class.
 *
 *  Should not cause everything to break :)
 */

#pragma once

#ifndef ENGINE_AEMODULEBASE_HPP
#define ENGINE_AEMODULEBASE_HPP

#include "include/AETypedefs.hpp"
#include <string>
#include <atomic>
#include <string_view>

//file meaning: base classes for all modules

/// <summary>
/// The support function for __AEModuleBase's module name registration
/// @note Please don't use it anywhere else...and where *can* you use it other than here?
/// </summary>
/// <typeparam name="T">"Type", name of the struct/class</typeparam>
/// <returns>Without registration and overloads, default "__getTypeName". With them, in correct setting, the name of the regitered module</returns>
template <typename T>
constexpr std::string_view __getTypeName(void) noexcept { return "__getTypeName"; }

/// Adds module name to inherit from __AEModuleBase
#define REGISTER_MODULE(T) template <> constexpr std::string_view __getTypeName<T>() noexcept { return #T; }

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
	static inline std::string_view getModuleName(void) noexcept {
		return m_sModulename;
	}

protected:

	/// <summary>
	/// Name of the module (class), same as in code
	/// @todo Find a way to make it constexpr and compile-time evaluated 
	/// </summary>
	static const std::string_view m_sModulename;
	/// <summary>
	/// Amount of alive module instances
	/// </summary>
	static inline std::atomic<ullint> m_ullModulesAlive = 0;
};

/// Sets up the static variable values for the base class (and inherited classes)
template<typename T>
const std::string_view __AEModuleBase<T>::m_sModulename = __getTypeName<T>();

//REGISTER_MODULE(__AEModuleBase)

#endif // !ENGINE_AEMODULEBASE_HPP
