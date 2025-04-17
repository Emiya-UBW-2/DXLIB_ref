#include "DXLib_ref.h"

namespace DXLibRef {
	const DXLib_ref* SingletonBase<DXLib_ref>::m_Singleton = nullptr;

	DXLib_ref::DXLib_ref(void) noexcept {
		OptionManager::Create();
		LocalizePool::Create();
		SaveData::Create();
		auto* SaveDataParts = SaveData::Instance();
		auto* OptionParts = OptionManager::Instance();
		// ロード
		this->m_IsFirstBoot = !SaveDataParts->Load();
		if (this->m_IsFirstBoot) {
			// 初回データ作成
			SaveDataParts->Save();
			OptionParts->SetParamBoolean(EnumSaveParam::usevr, false);
		}
#if defined(_USE_OPENVR_)
		// VR初期化
		if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
			VRControl::Create();
			VRControl::Instance()->Init();// 機器が認識できないと中でusevr=falseに
		}
#endif
		WindowSizeControl::Create();						// 汎用
		auto* WindowSizeParts = WindowSizeControl::Instance();
		WindowSizeParts->SetupWindowSize();
		SetWaitVSync();												// 垂直同期
		SetOutApplicationLogValidFlag(TRUE);						// log
		SetMainWindowText("Loading...");							// タイトル
		ChangeWindowMode(TRUE);										// 窓表示
		SetUseDirect3DVersion(DirectXVerID[OptionParts->GetParamInt(EnumSaveParam::DirectXVer)]);								// directX ver
		SetUseDirectInputFlag(TRUE);								// 
		SetDirectInputMouseMode(TRUE);								// 
		SetGraphMode(WindowSizeParts->GetSizeXMax() * GetDPI() / BaseDPI, WindowSizeParts->GetSizeYMax() * GetDPI() / BaseDPI, 32);		// 解像度
		SetWindowSizeChangeEnableFlag(FALSE, FALSE);				// ウインドウサイズを手動不可、ウインドウサイズに合わせて拡大もしないようにする
		Set3DSoundOneMetre(1.0f);									// 
		SetZBufferBitDepth(32);										// デフォのZバッファ精度を32bitに
		DxLib_Init();												// 初期化
		SetChangeScreenModeGraphicsSystemResetFlag(FALSE);			// 画面モード変更時( とウインドウモード変更時 )にリセットを走らせない
		SetUsePixelLighting(TRUE);									// ピクセルライティングの使用
		if (GetUseDirect3DVersion() != DirectXVerID[OptionParts->GetParamInt(EnumSaveParam::DirectXVer)]) {
			auto* LocalizeParts = LocalizePool::Instance();
			MessageBox(NULL, LocalizeParts->Get(10), "", MB_OK);
		}
		SetSysCommandOffFlag(TRUE);									// 
#if defined(_USE_EFFEKSEER_)
		Effekseer_Init(8000);										// Effekseer
		Effekseer_SetGraphicsDeviceLostCallbackFunctions();			// Effekseer
#endif
		SetAlwaysRunFlag(TRUE);										// background
		SetUseZBuffer3D(TRUE);										// zbufuse
		SetWriteZBuffer3D(TRUE);									// zbufwrite
		// MV1SetLoadModelPhysicsWorldGravity(GravityRate);			// 重力
		WindowSizeParts->SetWindowOrBorderless();

		// 
		SoundPool::Create();							// サウンド
		FontSystem::FontPool::Create();
		KeyGuide::Create();
		PadControl::Create();							// キー
		OptionPopup::Create();
		PopUp::Create();

		auto* SE = SoundPool::Instance();
		SE->Add(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select), 2, "CommonData/Sound/UI/cursor.wav", false);
		SE->Add(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_CANCEL), 1, "CommonData/Sound/UI/cancel.wav", false);
		SE->Add(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_OK), 1, "CommonData/Sound/UI/ok.wav", false);
		SE->Add(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_NG), 1, "CommonData/Sound/UI/ng.wav", false);

		WindowSystem::DrawControl::Create();
		//
		SceneControl::Create();
	}
	bool			DXLib_ref::FirstBootSetting(void) noexcept {
		auto* WindowSizeParts = WindowSizeControl::Instance();
		if (this->m_IsFirstBoot) {
			SetMainWindowText("FirstBoot Option");						// タイトル
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			auto* OptionWindowParts = OptionPopup::Instance();
			auto* Pad = PadControl::Instance();
			auto* PopUpParts = PopUp::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			auto* OptionParts = OptionManager::Instance();

			OptionWindowParts->Init();
			// シェーダー
			PostPassEffect::Create();
			auto* PostPassParts = PostPassEffect::Instance();
			//
			PostPassParts->Init();

			int xBase = (1366);
			int yBase = (768);

			// 初期設定画面
			OptionWindowParts->SetActive();
			while (ProcessMessage() == 0) {
				StartCount();
				WindowSystem::DrawControl::Instance()->ClearList();

				SetWindowPosition((WindowSizeParts->GetSizeXMax() - xBase) / 2, (WindowSizeParts->GetSizeYMax() - yBase) / 2);
				SetWindowSize(xBase, yBase);

				Pad->Update();
				OptionWindowParts->Update();
				PopUpParts->Update();
				if (!PopUpParts->IsActivePop()) {
					break;
				}
				// 
				GraphHandle::SetDraw_Screen(static_cast<int>(DX_SCREEN_BACK), true);
				{
					int Width = (720);
					int Height = (720);
					int Edge = (16);

					PopUpParts->Draw(Width / 2 + Edge, Height / 2 + Edge);

					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, 12,
						FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::MIDDLE, Edge + Edge, Height + Edge + Edge,
						Green, Black, LocalizeParts->Get(109));

					int xp = Width + Edge + Edge;
					int yp = Edge;
					if (WindowSystem::SetMsgClickBox(xp, yp, xp + (400), yp + LineHeight, LineHeight, Gray50, false, true, LocalizeParts->Get(2000))) {
						this->m_CheckPCSpec.StartSearch();
					}
					yp += (24);
					if (this->m_CheckPCSpec.GetCPUDatas()) {
						int MouseOverID = InvalidID;
						// CPU
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
							FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp, yp,
							White, DarkGreen, LocalizeParts->Get(2001)); yp += LineHeight;
						for (auto& c : *m_CheckPCSpec.GetCPUDatas()) {
							int TextID = 0;
							unsigned int Color = White;
							if (c.m_Score >= 17276) {// 
								Color = Green;
								TextID = 2002;
							}
							else if (c.m_Score >= 6600) {// 
								Color = Yellow;
								TextID = 2003;
							}
							else {// 
								Color = Red;
								TextID = 2004;
							}
							if (IntoMouse(xp + Edge, yp, xBase - Edge, yp + LineHeight * 2)) {
								switch (TextID) {
								case 2002:
									MouseOverID = 2040;
									break;
								case 2003:
									MouseOverID = 2041;
									break;
								case 2004:
									MouseOverID = 2042;
									break;
								default:
									break;
								}
							}
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
								FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp + Edge, yp,
								White, DarkGreen, "[%s]", c.m_Name.c_str());
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight * 2 / 3,
								FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xBase - Edge, yp,
								Color, DarkGreen, "%s", LocalizeParts->Get(TextID)); yp += LineHeight;
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
								FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xBase - Edge, yp, 
								White, DarkGreen, "PassMark Score:%d", c.m_Score); yp += LineHeight;
							yp += LineHeight;
						}
						if (this->m_CheckPCSpec.GetCPUDatas()->size() == 0) {
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
								FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp, yp,
								Red, DarkGreen, LocalizeParts->Get(2005)); yp += LineHeight;
						}
						// Mem
						{
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
								FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp, yp,
								White, DarkGreen, LocalizeParts->Get(2011)); yp += LineHeight;

							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
								FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xBase - Edge, yp,
								White, DarkGreen, "[%4.3lfMB / %4.3lfMB]", this->m_CheckPCSpec.GetFreeMemorySize(), this->m_CheckPCSpec.GetTotalMemorySize());
							int TextID = 0;
							unsigned int Color = White;
							if ((this->m_CheckPCSpec.GetTotalMemorySize() - this->m_CheckPCSpec.GetFreeMemorySize()) >= 2000) {// 
								Color = Green;
								TextID = 2012;
							}
							else {// 
								Color = Yellow;
								TextID = 2013;
							}
							if (IntoMouse(xp + Edge, yp, xBase - Edge, yp + LineHeight * 1)) {
								switch (TextID) {
								case 2012:
									MouseOverID = 2043;
									break;
								case 2013:
									MouseOverID = 2044;
									break;
								default:
									break;
								}
							}
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight * 2 / 3,
								FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xBase - Edge, yp,
								Color, DarkGreen, "%s", LocalizeParts->Get(TextID)); yp += LineHeight;
							yp += LineHeight;
						}
						// GPU
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
							FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp, yp,
							White, DarkGreen, LocalizeParts->Get(2021)); yp += LineHeight;
						for (auto& c : *m_CheckPCSpec.GetGPUDatas()) {
							int TextID = 0;
							unsigned int Color = White;
							if (c.m_Score >= 14649) {// 
								Color = Green;
								TextID = 2022;
							}
							else if (c.m_Score >= 5003) {// 
								Color = Yellow;
								TextID = 2023;
							}
							else {// 
								Color = Red;
								TextID = 2024;
							}
							if (IntoMouse(xp + Edge, yp, xBase - Edge, yp + LineHeight * 2)) {
								switch (TextID) {
								case 2022:
									MouseOverID = 2045;
									break;
								case 2023:
									MouseOverID = 2046;
									break;
								case 2024:
									MouseOverID = 2047;
									break;
								default:
									break;
								}
							}
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight * 3 / 4,
								FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::MIDDLE, xp + Edge, yp + LineHeight / 2,
								White, DarkGreen, "%s", c.m_Name.c_str());
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight * 2 / 3,
								FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xBase - Edge, yp,
								Color, DarkGreen, "%s", LocalizeParts->Get(TextID)); yp += LineHeight;
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
								FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xBase - Edge, yp, 
								White, DarkGreen, "PassMark Score:%d", c.m_Score); yp += LineHeight;
							yp += LineHeight;
						}
						if (this->m_CheckPCSpec.GetGPUDatas()->size() == 0) {
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
								FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp, yp,
								Red, DarkGreen, LocalizeParts->Get(2025)); yp += LineHeight;
						}
						// DirectX
						int NowSet = OptionParts->GetParamInt(EnumSaveParam::DirectXVer);
						for (int loop : std::views::iota(0, 2)) {
							if (GetUseDirect3DVersion() == DirectXVerID[loop]) {
								NowSet = loop;
							}
						}
						if (IntoMouse(xp + Edge, yp, xBase - Edge, yp + LineHeight * 2)) {
							MouseOverID = 2048;
						}
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
							FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp, yp,
							White, DarkGreen, LocalizeParts->Get(2035));
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
							FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xBase - Edge, yp, 
							White, DarkGreen, "DirectX%s", DirectXVerStr[NowSet]); yp += LineHeight;
						if (MouseOverID != InvalidID) {
							xp = Pad->GetMS_X();
							yp = Pad->GetMS_Y();
							DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
								FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::BOTTOM, xp, yp,
								Green, DarkGreen, LocalizeParts->Get(MouseOverID));
						}
					}

					xp = Width + Edge + Edge + Edge;
					yp = Height;
					if (WindowSystem::SetMsgClickBox(xp, yp, xBase - Edge + Edge, yp + Edge + Edge, LineHeight, Green, false, true, "Start Game!")) {
						PopUpParts->EndAll();
					}

					WindowSystem::DrawControl::Instance()->Draw();
				}
				ScreenFlip();
				WaitCount();
			}
			OptionParts->Save();
			WindowSystem::DrawControl::Instance()->Dispose();
#if defined(_USE_EFFEKSEER_)
			Effkseer_End();
#endif
			DxLib_End();
			StartMe();
			return true;
		}
		return false;
	}
	// 
	void			DXLib_ref::StartLogic(void) noexcept {
		auto* OptionWindowParts = OptionPopup::Instance();
		OptionWindowParts->Init();
#if defined(DEBUG)
		DebugDraw::Create();
#endif // DEBUG
#if defined(_USE_EFFEKSEER_)
		EffectResource::Create();						// エフェクト
#endif
		ObjectManager::Create();
		SideLog::Create();
		Camera3D::Create();
		LightPool::Create();
		// シェーダー
		PostPassEffect::Create();
		auto* PostPassParts = PostPassEffect::Instance();
		//
		PostPassParts->Init();
		//
#if defined(_USE_OPENVR_)
		auto* VRParts = VRControl::Instance();
		VRParts->SetupBuffer();
#endif
		//
		this->m_Update_effect_was = GetNowHiPerformanceCount();
	}
	void			DXLib_ref::MainLogic(void) noexcept {
		auto* SceneParts = SceneControl::Instance();
#if defined(DEBUG)
		auto* DebugParts = DebugDraw::Instance();		// デバッグ
#endif // DEBUG
		//最初のシーンの初期化
		SceneParts->Initialize();
		// 繰り返し
		while (true) {
			if (!(ProcessMessage() == 0)) {
				SceneParts->SetEndGame();
			}
			StartCount();
#if defined(DEBUG)
			clsDx();
			DebugParts->SetStartPoint();
#endif // DEBUG
			SceneParts->Update();
			// 
#if defined(_USE_EFFEKSEER_)
			if (!SceneParts->IsPause() && ((this->m_StartTime - this->m_Update_effect_was) >= 1000000 / 60)) {
				this->m_Update_effect_was = this->m_StartTime;
				UpdateEffekseer3D();
			}
#endif
#if defined(DEBUG)
			DebugParts->SetPoint("-----DrawStart-----");
#endif // DEBUG
			// シーンの描画を行う処理
			SceneParts->DrawMainLoop();
			// デバッグ
#if defined(DEBUG)
			DebugParts->SetEndPoint();
#endif // DEBUG
			ScreenFlip();
			WaitCount();
			// 画面の反映
#if defined(_USE_OPENVR_)
			VRParts->WaitSync();
#endif
			// シーン/ゲームの終了判定が立っているのでループを抜ける
			if (SceneParts->IsEndScene()) {
				// シーンの終わりに通る処理
				SceneParts->NextMainLoop();
			}
			// 終了フラグが立った場合即終了
			if (SceneParts->IsEndGame()) {
				break;
			}
		}
		WindowSystem::DrawControl::Instance()->Dispose();
#if defined(_USE_OPENVR_)
		VRParts->Dispose();
#endif
#if defined(_USE_EFFEKSEER_)
		Effkseer_End();
#endif
		DxLib_End();
	}
	//
	void			DXLib_ref::SetWaitVSync(void) noexcept {
		// 垂直同期
		auto* OptionParts = OptionManager::Instance();
		if (DirectXVerID[OptionParts->GetParamInt(EnumSaveParam::DirectXVer)] == DX_DIRECT3D_11) {
			SetWaitVSyncFlag(OptionParts->GetParamBoolean(EnumSaveParam::vsync));//DirectX11ではあとからの変更が効く
		}
		else {
			SetWaitVSyncFlag(FALSE);//DirectX9では後からの変更が効かない
		}
	}
	// ループの最初に通す
	void			DXLib_ref::StartCount(void) noexcept {
		this->m_DeltaTime = static_cast<float>(GetNowHiPerformanceCount() - this->m_StartTime) / 1000000.f;
		this->m_DeltaTime = std::min(this->m_DeltaTime, 1.f / 30.f);
		this->m_StartTime = GetNowHiPerformanceCount();
	}
	// 表画面に反映し、垂直同期または一定のFPSまで待機する
	bool			DXLib_ref::WaitCount(void) const noexcept {
		auto* OptionParts = OptionManager::Instance();
		if (!OptionParts->GetParamBoolean(EnumSaveParam::vsync)) {
			// 4msだけスリープ
			while ((GetNowHiPerformanceCount() - this->m_StartTime) < static_cast<LONGLONG>(1000 * (1000 / OptionParts->GetParamInt(EnumSaveParam::FpsLimit) - 4))) {
				if (ProcessMessage() != 0) {
					return false;
				}
				SleepThread(1);	// 1msecスリープする
			}
			while ((GetNowHiPerformanceCount() - this->m_StartTime) < static_cast<LONGLONG>(1000 * 1000 / OptionParts->GetParamInt(EnumSaveParam::FpsLimit))) {
			}
		}
		else {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) {
				WaitVSync(1);
			}
		}
		return true;
	}
};
