#include "DXLib_ref.h"
//#include "DXLib_ref_07_EffekseerEffectHandle.hpp"

namespace DXLibRef {
	//--------------------------------------------------------------------------------------------------
	//シングルトン
	//--------------------------------------------------------------------------------------------------
#ifdef _USE_EFFEKSEER_
	const EffectResource* SingletonBase<EffectResource>::m_Singleton = nullptr;
#endif
};
