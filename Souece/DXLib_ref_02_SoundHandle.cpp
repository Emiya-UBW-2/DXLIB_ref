#include "DXLib_ref_02_SoundHandle.hpp"

namespace DXLibRef {
	// シングルトンの実態定義
	const SoundPool* SingletonBase<SoundPool>::m_Singleton = nullptr;
	// 保持している音声すべての音量を設定
	void Soundhave::FlipVolume() noexcept {
		//BGMかSEかに合わせて取得するボリュームを変更
		auto* OptionParts = OPTION::Instance();
		float Vol = 1.0f;
		switch (this->m_SoundType) {
		case SoundType::SE:			Vol = OptionParts->GetParamFloat(EnumSaveParam::SE);			break;
		case SoundType::BGM:		Vol = OptionParts->GetParamFloat(EnumSaveParam::BGM);			break;
		default:
			break;
		}
		this->m_Handles->SetVolAll(static_cast<int>(Vol * static_cast<float>(this->m_LocalVolume)));
	}
};
