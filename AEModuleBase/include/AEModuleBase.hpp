
#pragma once

#ifndef ENGINE_AEMODULEBASE_HPP
#define ENGINE_AEMODULEBASE_HPP

#include <string>
#include <atomic>
#include "include/AETypedefs.hpp"

//file meaning: base classes for all modules

template <typename T>
inline const char* getTypeName(void) { return "getTypeName"; }

/// adds module name to inherit from __AEModuleBase
#define REGISTER_CLASS(T) template <> constexpr const char* getTypeName<T>() { return #T; }

// Usage to register the class:
// add REGISTER_CLASS(<classname>)
// after the class definition

/// <summary>
/// This is a base class for all basic modules, meaning that the module is not using other modules to work. 
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
	__AEModuleBase(){
		this->m_ullModulesAlive++;
	}

	/// <summary>
	/// Basic copy constructor that increases module count
	/// </summary>
	/// <param name="val">The value to be copied...useless since we have nothing to copy lol (but we need it)</param>
	__AEModuleBase(const __AEModuleBase<T>& val) {
		m_ullModulesAlive++;
	}

	/// <summary>
	/// Basic destructor that decreases module count
	/// </summary>
	virtual ~__AEModuleBase(){
		m_ullModulesAlive--;
	}
	
	/// <summary>
	/// Returns the amount of instances of the module currently existing
	/// </summary>
	/// <returns>Unsigned long long of the module amount</returns>
	static inline ullint getModuleAmount(void) {
		return m_ullModulesAlive.load();
	}
	
	/// <summary>
	/// Returns the name of the registered module
	/// </summary>
	/// <returns></returns>
	static inline std::string getModuleName(void) {
		return m_sModulename;
	}

protected:

	/// <summary>
	/// Name of the module (class), same as in code
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
