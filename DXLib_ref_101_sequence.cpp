#include "DXLib_ref_101_sequence.hpp"

namespace DXLibRef {
	const SceneControl* SingletonBase<SceneControl>::m_Singleton = nullptr;
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------

	void TEMPSCENE::BG_Draw_Sub(void) noexcept {
		FillGraph(GetDrawScreen(), 192, 192, 192);
	}

	void SceneControl::DrawFrontCommon(void) noexcept {
		auto* OptionParts = OPTION::Instance();
		auto* DrawParts = DXDraw::Instance();
#ifdef DEBUG
		auto* DebugParts = DebugClass::Instance();		//デバッグ
#endif // DEBUG

		//追加の描画物
		if (DrawParts->IsPause()) {
			//
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * 0.5f), 0, 255));
			DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, UIWidth, UIHeight, Black, TRUE);
			DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			//
			m_PauseFlashCount += 1.f / DrawParts->GetFps();
			if (m_PauseFlashCount > 1.f) {
				m_PauseFlashCount -= 1.f;
			}
			//
			if (m_PauseFlashCount > 0.5f) {
				WindowSystem::SetMsg(DrawParts->GetUIY(16), DrawParts->GetUIY(16) + DrawParts->GetUIY(36) / 2, DrawParts->GetUIY(36), FontHandle::FontXCenter::LEFT, Green, Black, "Pause");
			}
		}
		UISystem::Instance()->Draw();
		PadControl::Instance()->Draw();
		SideLog::Instance()->Draw();
		PopUp::Instance()->Draw(UIWidth / 2, UIHeight / 2);
		//FPS
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

			WindowSystem::SetMsg(UIWidth - DrawParts->GetUIY(8), DrawParts->GetUIY(8) + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, White, Black, "%5.2f FPS", Avg);
			WindowSystem::SetMsg(UIWidth - DrawParts->GetUIY(8), DrawParts->GetUIY(8 + 20) + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, White, Black, "%d Drawcall", GetDrawCallCount());
		}
#ifdef DEBUG
		DebugParts->DebugWindow(UIWidth - DrawParts->GetUIY(350), DrawParts->GetUIY(150));
#endif // DEBUG
	}


	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	void SceneControl::StartScene(void) noexcept {
		SetUseMaskScreenFlag(FALSE);//←一部画面でエフェクトが出なくなるため入れる
		auto* OptionParts = OPTION::Instance();
		auto* DrawParts = DXDraw::Instance();
		//カメラの初期設定
		DrawParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamBoolean(EnumSaveParam::usevr) ? 120 : OptionParts->GetParamInt(EnumSaveParam::fov)), 0.05f, 200.f);
		//環境光と影の初期化
		DrawParts->SetAmbientLight(Vector3DX::vget(0.25f, -1.f, 0.25f), GetColorF(1.f, 1.f, 1.f, 0.0f));
		GetNowScene()->Set();
		PadControl::Instance()->SetGuideUpdate();
		//FPS表示
		for (auto& f : FPSAvgs) {
			f = Frame_Rate;
		}
		m_FPSAvg = 0;
	}
	bool SceneControl::FirstExecute(void) noexcept {
#ifdef DEBUG
		clsDx();
#endif // DEBUG
		WindowSystem::DrawControl::Instance()->ClearList();
		return DXDraw::Instance()->FirstExecute();
	}

	bool SceneControl::Execute(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
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
		PadControl::Instance()->Execute();
		UISystem::Instance()->Update();
		auto SelEnd = !GetNowScene()->Update();		//更新
		OptionWindowClass::Instance()->Execute();
		DrawParts->Execute();
		SideLog::Instance()->Update();
		PopUp::Instance()->Update();
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
				[&]() {
					GetNowScene()->DrawUI_Base();
					DrawFrontCommon();
				},
				[&]() {
					GetNowScene()->DrawUI_In();
					WindowSystem::DrawControl::Instance()->Draw();
				}
			);
		}
		else {
			//画面に反映
			PostPassEffect::Instance()->Set_DoFNearFar(0.1f, 5.f, 0.05f, 6.f);
			DrawParts->Draw2D(
				[&]() { GetNowScene()->MainDraw(); },
				[&]() {
					GetNowScene()->DrawUI_Base();
					DrawFrontCommon();
					GetNowScene()->DrawUI_In();
					WindowSystem::DrawControl::Instance()->Draw();
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
		GetNowScene()->Dispose();							//解放
		if (this->m_NowScenesPtr != GetNowScene()->Get_Next()) {
			GetNowScene()->Dispose_Load();
		}
		this->m_NowScenesPtr = GetNowScene()->Get_Next();		//遷移
		PadControl::Instance()->Dispose();
		GetNowScene()->Load();
	}
};
