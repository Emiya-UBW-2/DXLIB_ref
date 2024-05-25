#include "DXLib_ref_101_sequence.hpp"

namespace DXLib_ref {
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	void SceneControl::StartScene(void) noexcept {
		SetUseMaskScreenFlag(FALSE);//←一部画面でエフェクトが出なくなるため入れる
		auto* OptionParts = OPTION::Instance();
		auto* DrawParts = DXDraw::Instance();
		auto* Pad = PadControl::Instance();
		//カメラの初期設定
		DrawParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamBoolean(EnumSaveParam::usevr) ? 120 : OptionParts->GetParamInt(EnumSaveParam::fov)), 0.05f, 200.f);
		//環境光と影の初期化
		DrawParts->SetAmbientLight(Vector3DX::vget(0.25f, -1.f, 0.25f), GetColorF(1.f, 1.f, 1.f, 0.0f));
		this->m_ScenesPtr->Set();
		Pad->SetGuideUpdate();
		//FPS表示
		for (auto& f : FPSAvgs) {
			f = 60.f;
		}
		m_FPSAvg = 0;
	}
	bool SceneControl::FirstExecute(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		return DrawParts->FirstExecute();
	}

	bool SceneControl::Execute(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		auto* ItemLogParts = SideLog::Instance();
		auto* PopUpParts = PopUp::Instance();
		auto* Pad = PadControl::Instance();
		auto* OptionParts = OPTION::Instance();
#ifdef DEBUG
		auto* DebugParts = DebugClass::Instance();		//デバッグ
#endif // DEBUG

#ifdef DEBUG
		clsDx();
		DebugParts->SetStartPoint();
#endif // DEBUG
		if (DrawParts->UpdateShadowActive() || this->m_ScenesPtr->GetIsFirstLoop()) {
			DrawParts->Update_Shadow([&]() { this->m_ScenesPtr->ShadowDraw_Far(); }, Vector3DX::zero(), true);
		}
		if (OptionParts->GetParamInt(EnumSaveParam::Reflection) > 0) {
			DrawParts->Update_CubeMap([&]() { this->m_ScenesPtr->CubeMapDraw(); }, DrawParts->SetMainCamera().GetCamPos());
		}
		Pad->Execute();
		auto SelEnd = !this->m_ScenesPtr->Update();		//更新
		OptionWindowClass::Instance()->Execute();
		DrawParts->Execute();
		ItemLogParts->Update();
		PopUpParts->Update();
		//描画

#ifdef DEBUG
		DebugParts->SetPoint("-----DrawStart-----");
#endif // DEBUG
		//影をセット
		DrawParts->Update_Shadow([&] { this->m_ScenesPtr->ShadowDraw(); }, DrawParts->SetMainCamera().GetCamPos(), false);
		//画面に反映
		DrawParts->Draw(
			[&]() { this->m_ScenesPtr->BG_Draw(); },
			[&]() { this->m_ScenesPtr->MainDraw(); },
			[&]() { this->m_ScenesPtr->MainDrawFront(); },
			[&]() { this->m_ScenesPtr->DrawUI_Base(); },
			[&]() {
				this->m_ScenesPtr->DrawUI_In();
				//追加の描画物
				auto* ItemLogParts = SideLog::Instance();
				auto* PopUpParts = PopUp::Instance();
				auto* Fonts = FontPool::Instance();
				auto* OptionParts = OPTION::Instance();
				ItemLogParts->Draw();
				PopUpParts->Draw(y_r(960), y_r(540));
				{
					FPSAvgs.at(m_FPSAvg) = DXDraw::Instance()->GetFps();
					auto color = White;
					//危険
					if (FPSAvgs.at(m_FPSAvg) < 45.f) {
						color = Red;
					}
					else if (FPSAvgs.at(m_FPSAvg) < 58.f) {
						color = Yellow;
					}
					//十分！
					if (FPSAvgs.at(m_FPSAvg) > (OptionParts->GetParamInt(EnumSaveParam::FpsLimit) - 2)) {
						color = Green;
					}

					++m_FPSAvg %= ((int)FPSAvgs.size());

					float Avg = 0.f;
					for (auto& f : FPSAvgs) {
						Avg += f;
					}
					Avg = Avg / ((float)FPSAvgs.size());

					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
						y_r(1920 - 8), y_r(8), White, Black, "%5.2f FPS", Avg);
				}
			}
		);

		//デバッグ
#ifdef DEBUG
		DebugParts->DebugWindow(DXDraw::Instance()->GetDispXSize() - y_r(250), y_r(150));
		//TestDrawShadowMap(DrawParts->m_Shadow[0].GetHandle(), 0, 0, 960, 540);
#endif // DEBUG
		DrawParts->Screen_Flip();//画面の反映
		return SelEnd;
	}
	void SceneControl::NextScene(void) noexcept {
		auto* Pad = PadControl::Instance();
		this->m_ScenesPtr->Dispose();							//解放
		this->m_ScenesPtr = this->m_ScenesPtr->Get_Next();		//遷移
		Pad->Dispose();
	}
};
