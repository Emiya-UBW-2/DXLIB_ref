#include "DXLib_ref.h"
// #include "DXLib_ref_07_EffekseerEffectHandle.hpp"

namespace DXLibRef {
	// --------------------------------------------------------------------------------------------------
	// シングルトン
	// --------------------------------------------------------------------------------------------------
#if defined(_USE_EFFEKSEER_)
	const EffectResource* SingletonBase<EffectResource>::m_Singleton = nullptr;

	EffectResource::EffectResource(void) noexcept {
		std::vector<WIN32_FIND_DATA> data_t;
		GetFileNamesInDirectory("data/effect/*", &data_t);
		for (const auto& d : data_t) {
			std::string p = d.cFileName;
			if (p.find(".efk") != std::string::npos && p.find(".efkproj") == std::string::npos) {
				effsorce.emplace_back();
				effsorce.back().load("data/effect/" + p);
			}
		}
		effsorce.emplace_back();
		effsorce.back().load("data/effect/gndsmk.efk");								// 戦車用エフェクト
	}
#endif
};
