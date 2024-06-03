#include "DXLib_ref.h"
//#include "DXLib_ref_02_SoundHandle.hpp"

namespace DXLibRef {
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	const SoundPool* SingletonBase<SoundPool>::m_Singleton = nullptr;
	const BGMPool* SingletonBase<BGMPool>::m_Singleton = nullptr;
};
