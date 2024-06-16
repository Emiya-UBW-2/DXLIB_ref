#include "DXLib_ref.h"

namespace DXLibRef {
	const DXLib_ref* SingletonBase<DXLib_ref>::m_Singleton = nullptr;

	DXLib_ref::DXLib_ref() noexcept {
		DXDraw::Create();						//汎用
		auto* DrawParts = DXDraw::Instance();
		if (!DrawParts->IsFirstBoot()) {
			DrawParts->Init();
		}
	}
	DXLib_ref::~DXLib_ref() noexcept {

	}
	bool DXLib_ref::StartLogic() noexcept {
		auto* DrawParts = DXDraw::Instance();
		if (DrawParts->IsFirstBoot()) {
			DrawParts->FirstBootSetting();
			StartMe();
			return false;
		}
		SceneControl::Create();
		return true;
	}
	bool DXLib_ref::MainLogic() noexcept {
		auto* SceneParts = SceneControl::Instance();
		//最初の読み込み
		SceneParts->GetNowScene()->Load();
		//繰り返し
		while (true) {
			SceneParts->StartScene();
			while (true) {
				if (!SceneParts->FirstExecute()) { return false; }
				if (SceneParts->Execute()) { break; }
			}
			SceneParts->NextScene();							//次のシーンへ移行
		}
		return true;
	}
};
