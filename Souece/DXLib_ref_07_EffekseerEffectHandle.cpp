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
				this->m_Sorce.emplace_back();
				this->m_Sorce.back().load("data/effect/" + p);
			}
		}
		this->m_Size = this->m_Sorce.size();
	}
#endif
};
