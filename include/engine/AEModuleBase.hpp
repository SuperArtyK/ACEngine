#ifndef AEMODULEBASE_HPP
#define AEMODULEBASE_HPP

#include <string>
#include <atomic>
#include "typedefs.hpp"

//file meaning: base class for all modules

template <typename T>
inline const std::string getTypeName();

/// adds module name to inherit from __AEModuleBase
#define ADD_MODULE_TO_ENGINE(T) template <> inline const std::string getTypeName<T>() { return #T; }


template<typename T>
class __AEModuleBase
{
public:
	__AEModuleBase(){
		m_ullModulesAlive++;
	}

	virtual ~__AEModuleBase(){
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
	static inline const std::string m_sModulename = getTypeName<T>();
	/// amount of alive modules
	static inline static inline std::atomic<biguint> m_ullModulesAlive = 0;
};

//ADD_MODULE_TO_ENGINE(__AEModuleBase)

#endif // !AEMODULEBASE_HPP