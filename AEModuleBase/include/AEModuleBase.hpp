
/*! @file AEModuleBase/include/AEModuleBase.hpp
 *
 *	@brief This file contains the engine's **basic module template**.
 *
 *	All modules in the engine base on this class (or on other module template classes that base on this class). 
 *	
 *	It gives the default behaviour for the classes, such as allowing for 
 *	naming the module (giving it a module name) and module amount 
 *	(counting the alive instances).
 *	
 *	Also in the future, I'll add instance names and the instance number.
 *	
 *	With the latter being the number of the instance (increases with every creation), 
 *	and the first being the combination of a module name and the instance number.
 *
 *	@date 2023/09/21
 *
 *	@author	Artemii Kozhemiak (SuperArtyK)
 *
 *	@copyright MIT License
 *
 *  *Should not cause everything to break :)*
 */

#pragma once

#ifndef ENGINE_AEMODULEBASE_HPP
#define ENGINE_AEMODULEBASE_HPP

#include "include/AETypedefs.hpp"
#include "include/AEFlags.hpp"
#include "include/AEUtilsMacros.hpp"
#include <atomic>
#include <string>
#include <string_view>

/// A macro to **complete the module declaration with __AEModuleBase**.
/// Under the hood, it finishes specialising __AEModuleBase::getModuleName() for the inheriting class.
/// <typeparam name="T">Inheriting module's class name</typeparam>
/// @note Usage: Always put it as the **last** entry to the class, before the closing brace **}**
/// @warning If you won't do it and use the __AEModuleBase<T>::getModuleName(), the project won't compile with the "unresolved external" error
/// @see __AEModuleBase
#define REGISTER_MODULE(T) public: \
	[[nodiscard]] static constexpr std::string_view getModuleName(void) noexcept { \
	static_assert(sizeof(STR(T)) <= AELE_MODULENAME_SIZE, "The module name is TOO BIG! (check AELE_MODULENAME_SIZE in AEMisc/include/AEFlags.hpp"); \
	return #T; }

// A process to correctly copy-construct the module (that inherited from __AEModuleBase)
// (under the hood: calls the copy-constructor of the __AEModuleBase)
// Usage: put it as the first item of the initializer list
// Example: TestClass(const TestClass& val) : COPYCONSTRUCT_MODULE(T, val), [other member initialisation] {...}
// @warning If you won't do it, the module count (done by __AEModuleBase) won't be incremented
// <typeparam name="T">Inheriting module's class name</typeparam>
// <typeparam name="val">The name of the argument to be copy-constructed from</typeparam>
//#define COPYCONSTRUCT_MODULE(T, val) __AEModuleBase<T>(val)

/// Macro for the **default no error flag** for all modules.
#define ENGINE_MODULE_ERR_NOERROR 0

// Usage to register the class:
// add REGISTER_MODULE(<classname>)
// after the class definition

/// <summary>
/// This is a **base class for all the modules in the engine**; all basic modules inherit from it.
/// 
/// This allows tracking the name of the module, the amount of alive instances and soon: name and number of the instance.
/// 
/// @remark Intended to be inherited from in class declaration, not direct usage.
/// @note Usage: <br>
///		class **[classname]** : public __AEModuleBase<**[classname]**>
/// @warning If you use the __AEModuleBase<T>::getModuleName() and do not include #REGISTER_MODULE(T) after the whole class definition (before closing '}'), the program won't compile
/// @see #REGISTER_MODULE(T)
/// </summary>
/// <typeparam name="T">The class name of the module to inherit from this class</typeparam>
template<typename T>
class __AEModuleBase
{
public:

	/// <summary>
	/// **Basic constructor** that increases module count
	/// </summary>
	__AEModuleBase(void) noexcept {
		this->m_ullModulesAlive++;
	}

	/// <summary>
	/// **Basic copy constructor** that increases module count
	/// @remark It doesn't get called, unless expicitly called
	/// @note Doesn't do anything other than increase module count
	/// </summary>
	/// <param name="val">The object passed to the copy constructor (we don't do anything with it)</param>
	__AEModuleBase(const __AEModuleBase<T>& val) noexcept {
		this->m_ullModulesAlive++;
	}

	/// <summary>
	/// **Basic destructor** that decreases module count
	/// </summary>
	virtual ~__AEModuleBase(void) noexcept {
		this->m_ullModulesAlive--;
	}

	/// <summary>
	/// **Basic assignment operator -- it does nothing, so it's deleted** (make your own)
	/// </summary>
	virtual __AEModuleBase<T>& operator=(const __AEModuleBase<T>& val) = delete;

	/// <summary>
	/// Get the **amount of the currently-alive instances of module T**
	/// @see __AEModuleBase<T>::m_ullModulesAlive
	/// </summary>
	/// <returns>Unsigned long long of the alive module amount</returns>
	[[nodiscard]] static inline ullint getModuleAmount(void) noexcept {
		return m_ullModulesAlive.load();
	}
	
	/// <summary>
	/// Get the **name of the module**
	/// @attention You *need* to add #REGISTER_MODULE() to the end of the class declarations if you want to use this thing
	/// @see #REGISTER_MODULE()
	/// </summary>
	/// <returns>The name of the module as a const std::strinv_view type</returns>
	[[nodiscard]] static constexpr const std::string_view getModuleName(void) noexcept;

protected:

	/// <summary>
	/// Amount of the currently-alive (not destroyed) instances of the module.
	/// </summary>
	static inline std::atomic<std::size_t> m_ullModulesAlive = 0;

	//REGISTER_MODULE(__AEModuleBase)
};



#endif // !ENGINE_AEMODULEBASE_HPP
