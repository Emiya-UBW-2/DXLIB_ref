#include "DXLib_ref_101_sequence.hpp"

namespace DXLibRef {
	const SceneControl* SingletonBase<SceneControl>::m_Singleton = nullptr;
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------

	void TEMPSCENE::BG_Draw_Sub(void) noexcept { DrawBox_2D(0, 0, y_r(1920), y_r(1080), Gray25, TRUE); }

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
		GetNowScene()->Set();
		Pad->SetGuideUpdate();
		//FPS表示
		for (auto& f : FPSAvgs) {
			f = 60.f;
		}
		m_FPSAvg = 0;
	}
	bool SceneControl::FirstExecute(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
#ifdef DEBUG
		clsDx();
#endif // DEBUG
		return DrawParts->FirstExecute();
	}

	bool SceneControl::Execute(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		auto* ItemLogParts = SideLog::Instance();
		auto* PopUpParts = PopUp::Instance();
		auto* Pad = PadControl::Instance();
#ifdef DEBUG
		auto* DebugParts = DebugClass::Instance();		//デバッグ
#endif // DEBUG

#ifdef DEBUG
		DebugParts->SetStartPoint();
#endif // DEBUG
		if (DrawParts->UpdateShadowActive() || GetNowScene()->GetIsFirstLoop()) {
			DrawParts->Update_Shadow([&]() { GetNowScene()->ShadowDraw_Far(); }, Vector3DX::zero(), true);
		}
		DrawParts->Update_CubeMap([&]() { GetNowScene()->CubeMapDraw(); }, DrawParts->SetMainCamera().GetCamPos());
		Pad->Execute();
		auto SelEnd = !GetNowScene()->Update();		//更新
		OptionWindowClass::Instance()->Execute();
		DrawParts->Execute();
		ItemLogParts->Update();
		PopUpParts->Update();
		//描画

#ifdef DEBUG
		DebugParts->SetPoint("-----DrawStart-----");
#endif // DEBUG
		//影をセット
		DrawParts->Update_Shadow([&] { GetNowScene()->ShadowDraw(); }, DrawParts->SetMainCamera().GetCamPos(), false);
		//画面に反映
		DrawParts->Draw(
			[&]() { GetNowScene()->BG_Draw(); },
			[&]() { GetNowScene()->MainDraw(); },
			[&]() { GetNowScene()->MainDrawFront(); },
			[&](float scale) { GetNowScene()->DrawUI_Base(scale); },
			[&](float scale) {
				GetNowScene()->DrawUI_In(scale);
				//追加の描画物
				auto* ItemLogParts = SideLog::Instance();
				auto* PopUpParts = PopUp::Instance();
				auto* Fonts = FontPool::Instance();
				auto* OptionParts = OPTION::Instance();
				ItemLogParts->Draw(scale);
				PopUpParts->Draw(y_r(960), y_r(540), scale);
				{
					FPSAvgs.at(m_FPSAvg) = GetFPS();
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

					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18.f*scale), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
						y_r((1920 - 8)*scale), y_r(8.f*scale), White, Black, "%5.2f FPS", Avg);
				}
#ifdef DEBUG
				DebugParts->DebugWindow(y_r(1920 - 350), y_r(150), scale);
#endif // DEBUG
			}
		);

		//デバッグ
#ifdef DEBUG
		DebugParts->SetEndPoint();
#endif // DEBUG
		DrawParts->Screen_Flip();//画面の反映
		return SelEnd;
	}
	void SceneControl::NextScene(void) noexcept {
		auto* Pad = PadControl::Instance();
		if (this->m_NowScenesPtr != GetNowScene()->Get_Next()) {
			GetNowScene()->Dispose_Load();
		}
		GetNowScene()->Dispose();							//解放
		this->m_NowScenesPtr = GetNowScene()->Get_Next();		//遷移
		Pad->Dispose();
		GetNowScene()->Load();
	}
};
