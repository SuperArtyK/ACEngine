
#pragma once

#ifndef ENGINE_AEMODULEBASE_HPP
#define ENGINE_AEMODULEBASE_HPP

#include <string>
#include <atomic>
#include "typedefs.hpp"

//file meaning: base classes for all modules

template <typename T>
inline const char* getTypeName() { return "getTypeName"; }

/// adds module name to inherit from __AEModuleBase
#define ADD_MODULE_TO_ENGINE(T) template <> inline const char* getTypeName<T>() { return #T; }

/// <summary>
/// This is a base class for all basic modules, meaning that the module
/// is not using other modules to work. Intended to be inherited from in class declaration,
/// not direct usage.
/// </summary>
/// <typeparam name="T">Module's class name</typeparam>
template<typename T>
class __AEBasicModule
{
public:
	__AEBasicModule(){
		m_ullModulesAlive++;
	}

	virtual ~__AEBasicModule(){
		m_ullModulesAlive--;
	}

	static inline biguint getAliveModuleAmt() {
		return m_ullModulesAlive.load();
	}

	static inline std::string getModuleName() {
		return m_sModulename;
	}

protected:
	/// name of the module it is assigned
	/// same as named in code
	static const std::string m_sModulename;
	/// amount of alive modules
	static inline std::atomic<biguint> m_ullModulesAlive = 0;
};
template<typename T>
const std::string __AEBasicModule<T>::m_sModulename = getTypeName<T>();







//ADD_MODULE_TO_ENGINE(__AEModuleBase)

#endif // !ENGINE_AEMODULEBASE_HPP
