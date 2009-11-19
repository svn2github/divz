#ifndef PropertyUtils_H
#define PropertyUtils_H

#include <QDebug>
#ifndef DEBUG_MARK
	#define DEBUG_MARK __FILE__":"#__LINE__
#endif 


/** @brief PROP_SET is designed to be used in the .cpp file to generate a simple setter method for the given property. */

#define PROP_SET(className,setterName,typeName,memberName) \
	void className::set##setterName(typeName newValue) { \
		m_##memberName = newValue; \
	}

/** @brief PROP_SET_CHANGED gives a simple setter that also calls a setChanged(const char* propertyName,QVariant oldValue,QVariant newValue) method to notify of a change. */
#define PROP_SET_CHANGED(className,setterName,typeName,memberName) \
	void className::set##setterName(typeName newValue) { \
		typeName oldValue = m_##memberName; \
		m_##memberName = newValue; \
		setChanged(#memberName,newValue,oldValue); \
	}

/** @brief PROP_DEF is designed to be used in the class header. It defines (A) the Q_PROPERTY macro for the property (as private), (B) the setter/getter function defs (public), and (C) the member itself (protected). */
#define PROP_DEF(typeName,memberName,setterName) \
	private: \
		Q_PROPERTY(typeName memberName READ memberName WRITE set##setterName); \
	protected: \
		typeName m_##memberName; \
	public: \
		    void set##setterName(typeName value); \
		typeName memberName() const { return m_##memberName; }
		
/** @brief PROP_DEF_FULL is the same as PROP_DEF, except it defines the full settor right along with the rest of the header definitions. */  
#define PROP_DEF_FULL(typeName,memberName,setterName) \
	private: \
		Q_PROPERTY(typeName memberName READ memberName WRITE set##setterName); \
	protected: \
		typeName m_##memberName; \
	public: \
		void set##setterName(typeName value) { m_##memberName = value; }\
		typeName memberName() const { return m_##memberName; };

/** @brief PROP_VDEF is the same as PROP_DEF, except it defines the getter/setter as virtual */
#define PROP_VDEF(typeName,memberName,setterName) \
	private: \
		Q_PROPERTY(typeName memberName READ memberName WRITE set##setterName); \
	protected: \
		typeName m_##memberName; \
	public: \
		virtual     void set##setterName(typeName value); \
		virtual typeName memberName() const { return m_##memberName; }

/** @brief PROP_PV_DEF is the same as PROP_DEF, except it defines the setter as pure virtual */
#define PROP_PV_DEF(typeName,memberName,setterName) \
	private: \
		Q_PROPERTY(typeName memberName READ memberName WRITE set##setterName); \
	protected: \
		typeName m_##memberName; \
	public: \
		virtual     void set##setterName(typeName value) = 0; \
		virtual typeName memberName() const { return m_##memberName; } 


#endif // PropertyUtils_H

