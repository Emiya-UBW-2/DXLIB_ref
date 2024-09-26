#include "DXLib_ref_101_sequence.hpp"

namespace DXLibRef {
	const SceneControl* SingletonBase<SceneControl>::m_Singleton = nullptr;
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------

	void TEMPSCENE::BG_Draw_Sub(void) noexcept {
		FillGraph(GetDrawScreen(), 192, 192, 192);
	}

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
		WindowSystem::DrawControl::Instance()->ClearList();
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
		if (GetNowScene()->Get3DActive()) {
			if (DrawParts->UpdateShadowActive() || GetNowScene()->GetIsFirstLoop()) {
				DrawParts->Update_Shadow([&]() { GetNowScene()->ShadowDraw_Far(); }, Vector3DX::zero(), m_NowScenesPtr->GetShadowScale() *4.f, true);
			}
		}
		Vector3DX Pos = DrawParts->SetMainCamera().GetCamPos(); Pos.y *= -1.f;
		if (GetNowScene()->Get3DActive()) {
			DrawParts->Update_CubeMap([&]() { GetNowScene()->CubeMapDraw(); }, Pos);
		}
		Pad->Execute();
		UISystem::Instance()->Update();
		auto SelEnd = !GetNowScene()->Update();		//更新
		OptionWindowClass::Instance()->Execute();
		DrawParts->Execute();
		ItemLogParts->Update();
		PopUpParts->Update();
		//描画

#ifdef DEBUG
		DebugParts->SetPoint("-----DrawStart-----");
#endif // DEBUG
		//画面に反映
		if (GetNowScene()->Get3DActive()) {
			//影をセット
			DrawParts->Update_Shadow([&] { GetNowScene()->ShadowDraw(); }, DrawParts->SetMainCamera().GetCamPos(), m_NowScenesPtr->GetShadowScale(), false);
			//画面に反映
			DrawParts->Draw(
				[&]() { GetNowScene()->BG_Draw(); },
				[&]() { GetNowScene()->SetShadowDraw_Rigid(); },
				[&]() { GetNowScene()->SetShadowDraw(); },
				[&]() { GetNowScene()->MainDraw(); },
				[&]() { GetNowScene()->MainDrawFront(); },
				[&]() { GetNowScene()->DrawUI_Base(); },
				[&]() {
					GetNowScene()->DrawUI_In();
					//追加の描画物
					auto* ItemLogParts = SideLog::Instance();
					auto* PopUpParts = PopUp::Instance();
					auto* OptionParts = OPTION::Instance();
					auto* DrawParts = DXDraw::Instance();
					ItemLogParts->Draw();
					PopUpParts->Draw(DrawParts->GetUIY(960), DrawParts->GetUIY(540));
					{
						FPSAvgs.at(static_cast<size_t>(m_FPSAvg)) = GetFPS();
						auto color = White;
						//危険
						if (FPSAvgs.at(static_cast<size_t>(m_FPSAvg)) < 45.f) {
							color = Red;
						}
						else if (FPSAvgs.at(static_cast<size_t>(m_FPSAvg)) < 58.f) {
							color = Yellow;
						}
						//十分！
						if (FPSAvgs.at(static_cast<size_t>(m_FPSAvg)) > static_cast<float>(OptionParts->GetParamInt(EnumSaveParam::FpsLimit) - 2)) {
							color = Green;
						}

						++m_FPSAvg %= static_cast<int>(FPSAvgs.size());

						float Avg = 0.f;
						for (auto& f : FPSAvgs) {
							Avg += f;
						}
						Avg = Avg / static_cast<float>(FPSAvgs.size());

						WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal, 
							FontPool::FontType::MS_Gothic, DrawParts->GetUIY(18), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
							DrawParts->GetUIY((1920 - 8)), DrawParts->GetUIY(8), White, Black, "%5.2f FPS", Avg);

						WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal,
							FontPool::FontType::MS_Gothic, DrawParts->GetUIY(18), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
							DrawParts->GetUIY((1920 - 8)), DrawParts->GetUIY(8+20), White, Black, "%d Drawcall", GetDrawCallCount());
					}
#ifdef DEBUG
					DebugParts->DebugWindow(DrawParts->GetUIY(1920 - 350), DrawParts->GetUIY(150));
#endif // DEBUG
				}
			);
		}
		else {
			//画面に反映
			PostPassEffect::Instance()->Set_DoFNearFar(0.1f, 5.f, 0.05f, 6.f);
			DrawParts->Draw2D(
				[&]() { GetNowScene()->MainDraw(); },
				[&]() { GetNowScene()->DrawUI_Base(); },
				[&]() {
					GetNowScene()->DrawUI_In();
					//追加の描画物
					auto* ItemLogParts = SideLog::Instance();
					auto* PopUpParts = PopUp::Instance();
					auto* OptionParts = OPTION::Instance();
					auto* DrawParts = DXDraw::Instance();
					ItemLogParts->Draw();
					PopUpParts->Draw(DrawParts->GetUIY(960), DrawParts->GetUIY(540));
					{
						FPSAvgs.at(static_cast<size_t>(m_FPSAvg)) = GetFPS();
						auto color = White;
						//危険
						if (FPSAvgs.at(static_cast<size_t>(m_FPSAvg)) < 45.f) {
							color = Red;
						}
						else if (FPSAvgs.at(static_cast<size_t>(m_FPSAvg)) < 58.f) {
							color = Yellow;
						}
						//十分！
						if (FPSAvgs.at(static_cast<size_t>(m_FPSAvg)) > static_cast<float>(OptionParts->GetParamInt(EnumSaveParam::FpsLimit) - 2)) {
							color = Green;
						}

						++m_FPSAvg %= static_cast<int>(FPSAvgs.size());

						float Avg = 0.f;
						for (auto& f : FPSAvgs) {
							Avg += f;
						}
						Avg = Avg / static_cast<float>(FPSAvgs.size());

						WindowSystem::DrawControl::Instance()->SetString(WindowSystem::DrawLayer::Normal,
							FontPool::FontType::MS_Gothic, DrawParts->GetUIY(18), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
							DrawParts->GetUIY((1920 - 8)), DrawParts->GetUIY(8), White, Black, "%5.2f FPS", Avg);
					}
#ifdef DEBUG
					DebugParts->DebugWindow(DrawParts->GetUIY(1920 - 350), DrawParts->GetUIY(150));
#endif // DEBUG
				}
			);
		}

		//デバッグ
#ifdef DEBUG
		DebugParts->SetEndPoint();
#endif // DEBUG
		DrawParts->Screen_Flip();//画面の反映
		return SelEnd;
	}
	void SceneControl::NextScene(void) noexcept {
		auto* Pad = PadControl::Instance();
		GetNowScene()->Dispose();							//解放
		if (this->m_NowScenesPtr != GetNowScene()->Get_Next()) {
			GetNowScene()->Dispose_Load();
		}
		this->m_NowScenesPtr = GetNowScene()->Get_Next();		//遷移
		Pad->Dispose();
		GetNowScene()->Load();
	}
};
