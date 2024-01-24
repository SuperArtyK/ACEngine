
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

/// A process to register the module (that inherited from __AEModuleBase)
/// (under the hood: finish specialising __AEModuleBase::getModuleName() specially for the inheriting class)
/// Usage: Always put it as the **last** entry to the class, before the closing brace }
/// @warning If you won't do it, the project won't compile with the "unresolved external" error
/// <typeparam name="T">Inheriting module's class name</typeparam>
#define REGISTER_MODULE(T) public: static constexpr std::string_view getModuleName(void) noexcept { \
	static_assert(sizeof(#T) <= AELE_MODULENAME_SIZE, "The module name is TOO BIG! (check AELE_MODULENAME_SIZE in AEMisc/include/AEFlags.hpp"); \
	return #T; }

/// A process to correctly copy-construct the module (that inherited from __AEModuleBase)
/// (under the hood: calls the copy-constructor of the __AEModuleBase)
/// Usage: put it as the first item of the initializer list
/// Example: TestClass(const TestClass& val) : COPYCONSTRUCT_MODULE(T, val), [other member initialisation] {...}
/// @warning If you won't do it, the module count (done by __AEModuleBase) won't be incremented
/// <typeparam name="T">Inheriting module's class name</typeparam>
/// <typeparam name="val">The name of the argument to be copy-constructed from</typeparam>
#define COPYCONSTRUCT_MODULE(T, val) __AEModuleBase<T>(val)

/// Global "no error" return flag for all engine modules
#define ENGINE_MODULE_ERR_NOERROR 0

// Usage to register the class:
// add REGISTER_MODULE(<classname>)
// after the class definition

/// <summary>
/// @brief This is a base class for all basic modules, meaning that the module is not using other modules to work. 
/// @note Intended to be inherited from in class declaration, not direct usage.
/// Usage: class [classname] : public __AEModuleBase<[classname]>
/// @warning If you do not include REGISTER_MODULE(T) after the whole class definition (before closing '}'), the program won't compile
/// @warning If you do not include COPYCONSTRUCT_MODULE(T, val) in the copy-constructor, the module increment (every constructing action) won't work!
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
		m_ullModulesAlive++;
	}

	/// <summary>
	/// Basic destructor that decreases module count
	/// </summary>
	virtual ~__AEModuleBase(void) noexcept {
		m_ullModulesAlive--;
	}

	__AEModuleBase& operator=(const __AEModuleBase<T>&) = delete;

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
