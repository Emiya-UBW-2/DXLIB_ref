#include "DXLib_ref.h"
//#include "DXLib_ref_07_EffekseerEffectHandle.hpp"

namespace DXLib_ref {
	//--------------------------------------------------------------------------------------------------
	//ƒVƒ“ƒOƒ‹ƒgƒ“
	//--------------------------------------------------------------------------------------------------
	const EffectResource* SingletonBase<EffectResource>::m_Singleton = nullptr;

	/*
	template <class T>
	void SingletonBase<T>::Create(void) {
		m_Singleton = new T();
	}
	template <class T>
	T* SingletonBase<T>::Instance(void) {
		if (m_Singleton == nullptr) { m_Singleton = new T(); }
		return (T*)m_Singleton;
	}
	//*/
};