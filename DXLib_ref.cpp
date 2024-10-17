#include "DXLib_ref.h"

namespace DXLibRef {
	const DXLib_ref* SingletonBase<DXLib_ref>::m_Singleton = nullptr;

	DXLib_ref::DXLib_ref(void) noexcept {
		OPTION::Create();
		LocalizePool::Create();
		SaveDataClass::Create();
		auto* SaveDataParts = SaveDataClass::Instance();
		auto* OptionParts = OPTION::Instance();
		//ロード
		m_IsFirstBoot = !SaveDataParts->Load();
		if (m_IsFirstBoot) {
			//初回データ作成
			SaveDataParts->Save();
			OptionParts->SetParamBoolean(EnumSaveParam::usevr, false);
		}
		DXDraw::Create();						//汎用
		//
#ifdef DEBUG
		DebugClass::Create();
#endif // DEBUG
#ifdef _USE_EFFEKSEER_
		EffectResource::Create();						//エフェクト
#endif
		SoundPool::Create();							//サウンド
		BGMPool::Create();
		FontPool::Create();
		PadControl::Create();							//キー
		OptionWindowClass::Create();
		ObjectManager::Create();
		SideLog::Create();
		PopUp::Create();
		UniversalUI::UISystem::Create();
		CameraShake::Create();

		auto* SE = SoundPool::Instance();
		SE->Add(static_cast<int>(SoundEnumCommon::UI_Select), 2, "CommonData/Sound/UI/cursor.wav", false);
		SE->Add(static_cast<int>(SoundEnumCommon::UI_CANCEL), 1, "CommonData/Sound/UI/cancel.wav", false);
		SE->Add(static_cast<int>(SoundEnumCommon::UI_OK), 1, "CommonData/Sound/UI/ok.wav", false);
		SE->Add(static_cast<int>(SoundEnumCommon::UI_NG), 1, "CommonData/Sound/UI/ng.wav", false);
		SE->SetVol(OptionParts->GetParamFloat(EnumSaveParam::SE));
	}
	//
	void DXLib_ref::UpdatePause(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		m_PauseFlashCount += 1.f / DrawParts->GetFps();
		if (m_PauseFlashCount > 1.f) {
			m_PauseFlashCount -= 1.f;
		}
	}
	void DXLib_ref::DrawPause(void) const noexcept {
		auto* DrawParts = DXDraw::Instance();
		if (!DrawParts->IsPause()) {
			return;
		}
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		//
		DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * 0.5f), 0, 255));
		DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, UIWidth, UIHeight, Black, TRUE);
		DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
		//
		if (m_PauseFlashCount > 0.5f) {
			WindowSystem::SetMsg(DrawParts->GetUIY(16), DrawParts->GetUIY(16) + DrawParts->GetUIY(36) / 2, DrawParts->GetUIY(36), FontHandle::FontXCenter::LEFT, Green, Black, "Pause");
		}
	}
	void DXLib_ref::UpdateFPSCounter(void) noexcept {
		FPSAvgs.at(static_cast<size_t>(m_FPSAvgCount)) = GetFPS();

		++m_FPSAvgCount %= static_cast<int>(FPSAvgs.size());
		m_FPSAvg = 0.f;
		for (auto& f : FPSAvgs) {
			m_FPSAvg += f;
		}
		m_FPSAvg = m_FPSAvg / static_cast<float>(FPSAvgs.size());
	}
	void DXLib_ref::DrawFPSCounter(void) const noexcept {
		auto* DrawParts = DXDraw::Instance();
		auto* OptionParts = OPTION::Instance();
		auto color = White;
		//十分！
		if (m_FPSAvg > static_cast<float>(OptionParts->GetParamInt(EnumSaveParam::FpsLimit) - 2)) {
			color = Green;
		}
		else if (m_FPSAvg < 45.f) {
			color = Red;
		}
		else if (m_FPSAvg < 58.f) {
			color = Yellow;
		}
		WindowSystem::SetMsg(UIWidth - DrawParts->GetUIY(8), DrawParts->GetUIY(8) + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, color, Black, "%5.2f FPS", m_FPSAvg);
		WindowSystem::SetMsg(UIWidth - DrawParts->GetUIY(8), DrawParts->GetUIY(8 + 20) + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, White, Black, "%d Drawcall", GetDrawCallCount());
	}
	void DXLib_ref::DrawUICommon(void) const noexcept {
		DrawPause();
		DrawFPSCounter();
		UniversalUI::UISystem::Instance()->Draw();
		PadControl::Instance()->Draw();
		SideLog::Instance()->Draw();
		PopUp::Instance()->Draw(UIWidth / 2, UIHeight / 2);
#ifdef DEBUG
		auto* DrawParts = DXDraw::Instance();
		DebugClass::Instance()->DebugWindow(UIWidth - DrawParts->GetUIY(350), DrawParts->GetUIY(150));
#endif // DEBUG
	}
	//
	bool DXLib_ref::StartLogic(void) const noexcept {
		WindowSystem::DrawControl::Create();
		auto* DrawParts = DXDraw::Instance();
		if (m_IsFirstBoot) {
			DrawParts->FirstBootSetting();
			StartMe();
		}
		else {
			DrawParts->Init();
			SceneControl::Create();
		}
		return true;
	}
	bool DXLib_ref::MainLogic(void) noexcept {
		auto* SceneParts = SceneControl::Instance();
		auto* OptionParts = OPTION::Instance();
		auto* DrawParts = DXDraw::Instance();
		auto* Pad = PadControl::Instance();
		auto* PopUpParts = PopUp::Instance();
		auto* OptionWindowParts = OptionWindowClass::Instance();
		auto* LocalizeParts = LocalizePool::Instance();
#ifdef DEBUG
		auto* DebugParts = DebugClass::Instance();		//デバッグ
#endif // DEBUG
		//最初の読み込み
		auto& NowScene = SceneParts->GetNowScene();
		//繰り返し
		while (true) {
			NowScene->Load();
			{
				SetUseMaskScreenFlag(FALSE);//←一部画面でエフェクトが出なくなるため入れる
				//カメラの初期設定
				DrawParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamBoolean(EnumSaveParam::usevr) ? 120 : OptionParts->GetParamInt(EnumSaveParam::fov)), 0.05f, 200.f);
				//環境光と影の初期化
				DrawParts->SetAmbientLight(Vector3DX::vget(0.25f, -1.f, 0.25f), GetColorF(1.f, 1.f, 1.f, 0.0f));
				NowScene->Set();
				Pad->SetGuideUpdate();
			}
			InitFPSCounter();
			while (true) {
				if (!(ProcessMessage() == 0)) { return false; }
#ifdef DEBUG
				clsDx();
#endif // DEBUG
				WindowSystem::DrawControl::Instance()->ClearList();
				DrawParts->FirstExecute();
				if (Pad->GetEsc().trigger() && !DrawParts->IsExit()) {
					DrawParts->SetExitFlag(true);
					PopUpParts->Add(LocalizeParts->Get(100), 480, 240,
						[this](int xmin, int ymin, int xmax, int ymax, bool) {
							auto* DrawParts = DXDraw::Instance();
							auto* LocalizeParts = LocalizePool::Instance();
							int xp1, yp1;
							//タイトル
							{
								xp1 = xmin + DrawParts->GetUIY(24);
								yp1 = ymin + LineHeight;

								WindowSystem::SetMsg(xp1, yp1 + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, LocalizeParts->Get(101));
							}
							//
							{
								xp1 = (xmax + xmin) / 2 - DrawParts->GetUIY(54);
								yp1 = ymax - LineHeight * 3;

								auto* Pad = PadControl::Instance();
								bool ret = WindowSystem::SetMsgClickBox(xp1, yp1, xp1 + DrawParts->GetUIY(108), yp1 + LineHeight * 2, LineHeight, Gray15, false, true, LocalizeParts->Get(102));
								if (Pad->GetKey(PADS::INTERACT).trigger() || ret) {
									m_IsEnd = true;
								}
							}
						},
						[]() {
							auto* DrawParts = DXDraw::Instance();
							DrawParts->SetExitFlag(false);
						},
						[]() {},
						true
					);
				}
				if (OptionWindowParts->IsRestartSwitch() && !DrawParts->IsRestart()) {
					DrawParts->SetRestartFlag(true);
					PopUpParts->Add(LocalizeParts->Get(100), 480, 240,
						[this](int xmin, int ymin, int xmax, int ymax, bool) {
							auto* DrawParts = DXDraw::Instance();
							auto* LocalizeParts = LocalizePool::Instance();
							int xp1, yp1;
							//タイトル
							{
								xp1 = xmin + DrawParts->GetUIY(24);
								yp1 = ymin + LineHeight;

								WindowSystem::SetMsg(xp1, yp1 + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, LocalizeParts->Get(2101));
							}
							//
							{
								xp1 = (xmax + xmin) / 2 - DrawParts->GetUIY(54);
								yp1 = ymax - LineHeight * 3;

								auto* Pad = PadControl::Instance();
								bool ret = WindowSystem::SetMsgClickBox(xp1, yp1, xp1 + DrawParts->GetUIY(108), yp1 + LineHeight * 2, LineHeight, Gray15, false, true, LocalizeParts->Get(2102));
								if (Pad->GetKey(PADS::INTERACT).trigger() || ret) {
									m_IsEnd = true;
									StartMe();
								}
							}
						},
						[this]() {
							auto* DrawParts = DXDraw::Instance();
							DrawParts->SetRestartFlag(false);
						},
						[this]() {},
						true
					);
				}
				if (m_IsEnd) {
					return false;
				}
#ifdef DEBUG
				DebugParts->SetStartPoint();
#endif // DEBUG
				Pad->Update();
				UniversalUI::UISystem::Instance()->Update();
				auto SelEnd = !NowScene->Update();		//更新
				OptionWindowParts->Update();
				DrawParts->Update();
				CameraShake::Instance()->Update();
				SideLog::Instance()->Update();
				PopUpParts->Update();
				UpdatePause();
				UpdateFPSCounter();
				//キューブマップをセット
				NowScene->CubeMapDraw();
				//影をセット
				NowScene->ShadowDraw_Far();
				NowScene->ShadowDraw();
				//描画
#ifdef DEBUG
				DebugParts->SetPoint("-----DrawStart-----");
#endif // DEBUG
				//画面に反映
				if (NowScene->Get3DActive()) {
					if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
						NowScene->Draw3DVR([this]() {DrawUICommon(); });
					}
					else {
						NowScene->Draw3D([this]() {DrawUICommon(); });
					}
				}
				else {
					NowScene->Draw2D([this]() {DrawUICommon(); });
				}
				//デバッグ
#ifdef DEBUG
				DebugParts->SetEndPoint();
#endif // DEBUG
				//画面の反映
				DrawParts->Screen_Flip();
				if (SelEnd) {
					break;
				}
			}
			//次のシーンへ移行
			SceneParts->NextScene();
			Pad->Dispose();
		}
		return true;
	}
};
