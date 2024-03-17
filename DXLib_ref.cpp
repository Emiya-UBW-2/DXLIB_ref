#include "DXLib_ref.h"

#define LineHeight	y_r(18)

namespace DXLib_ref {
	const DXDraw* SingletonBase<DXDraw>::m_Singleton = nullptr;
	//
	DXDraw::DXDraw(void) noexcept {
		OPTION::Create();
		auto* OptionParts = OPTION::Instance();
		//解像度設定
		this->m_DispXSize = deskx;
		this->m_DispYSize = desky;
#ifdef _USE_OPENVR_
		VR_Init();
#endif // _USE_OPENVR_
		SetOutApplicationLogValidFlag(TRUE);						//log
		SetMainWindowText("Loading...");							//タイトル
		ChangeWindowMode(TRUE);										//窓表示
		SetUseDirect3DVersion(OptionParts->Get_DirectXVer());		//directX ver
		SetUseDirectInputFlag(TRUE);								//
		SetDirectInputMouseMode(TRUE);								//
		SetGraphMode(this->m_DispXSize, this->m_DispYSize, 32);		//解像度
		SetWindowSizeChangeEnableFlag(FALSE, FALSE);				//ウインドウサイズを手動不可、ウインドウサイズに合わせて拡大もしないようにする
		//SetFullSceneAntiAliasingMode(4, 3);						//アンチエイリアス
		//SetEnableXAudioFlag(TRUE);								//Xaudio(ロードが長いとロストするので必要に応じて)
		Set3DSoundOneMetre(1.0f);									//
		SetWaitVSyncFlag(OptionParts->Get_Vsync() ? TRUE : FALSE);	//垂直同期
		SetZBufferBitDepth(32);										//
		DxLib_Init();												//
		SetUsePixelLighting(TRUE);									//ピクセルライティングの使用
		//SetCreateDrawValidGraphMultiSample(4, 3);					//アンチエイリアス
		if (GetUseDirect3DVersion() != OptionParts->Get_DirectXVer()) {
			MessageBox(NULL, "DirectXのバージョンが適用していません。古いバージョンで動作しています", "", MB_OK);
		}
		if (OptionParts->Get_AllWaysFront()) {
			SetWindowPos(GetMainWindowHandle(), HWND_TOPMOST, (deskx - this->m_DispXSize) / 2, (desky - this->m_DispYSize) / 2, 0, 0, SWP_FRAMECHANGED);
		}
		else {
			SetWindowStyleMode(2);
			SetWindowPos(GetMainWindowHandle(), HWND_TOP, (deskx - this->m_DispXSize) / 2, (desky - this->m_DispYSize) / 2, 0, 0, SWP_FRAMECHANGED);
		}
		Effekseer_Init(8000);										//Effekseer
		SetSysCommandOffFlag(TRUE);									//
		SetChangeScreenModeGraphicsSystemResetFlag(FALSE);			//Effekseer
		Effekseer_SetGraphicsDeviceLostCallbackFunctions();			//Effekseer
		SetAlwaysRunFlag(TRUE);										//background
		SetUseZBuffer3D(TRUE);										//zbufuse
		SetWriteZBuffer3D(TRUE);									//zbufwrite
		//MV1SetLoadModelPhysicsWorldGravity(M_GR);					//重力
		int DPI = 96;
		if (IsWindows10OrGreater()) {
			//DPI = GetDpiForWindow(GetMainWindowHandle());
			if (DPI == 0) {
				//強制終了
			}
		}
		if (!OptionParts->Get_useVR()) {
			SetWindowSize(deskx * DPI / 96, desky * DPI / 96);
			if (OptionParts->Get_AllWaysFront()) {
				DxLib::GetWindowSize(&this->m_DispXSize, &this->m_DispYSize);
			}
		}
		//m_StartTime = GetNowHiPerformanceCount();
#ifdef DEBUG
		DebugClass::Create();
#endif // DEBUG
		EffectResource::Create();						//エフェクト
		SoundPool::Create();							//サウンド
		FontPool::Create();
		PadControl::Create();							//キー
		OptionWindowClass::Create();
		PadControl::Create();
		SaveDataClass::Create();

		auto* SE = SoundPool::Instance();
		SE->Add((int)SoundEnumCommon::UI_Select, 2, "data/Sound/UI/cursor.wav", false);
		SE->Add((int)SoundEnumCommon::UI_OK, 1, "data/Sound/UI/hit.wav", false);
		SE->Add((int)SoundEnumCommon::UI_NG, 1, "data/Sound/UI/cancel.wav", false);
		SE->SetVol(OptionParts->Get_SE());
		//影生成
		for (auto& s : m_Shadow) {
			s.Init();
		}
		//Init
		m_PauseActive.Set(false);
	}
	DXDraw::~DXDraw(void) noexcept {
		//影削除
		for (auto& s : m_Shadow) {
			s.Dispose();
		}
		//
#ifdef _USE_OPENVR_
		VR_Dispose();
#endif // _USE_OPENVR_
		Effkseer_End();
		DxLib_End();
	}
	//
	void			DXDraw::PauseIn() noexcept {
		if (!IsPause()) {
			m_PauseActive.Execute(true);
			PadControl::Instance()->SetGuideUpdate();
		}
	}
	//
	void			DXDraw::PauseExit() noexcept {
		if (IsPause()) {
			m_PauseActive.Execute(true);
			PadControl::Instance()->SetGuideUpdate();
		}
	}
	//
	void			DXDraw::Update_Shadow(std::function<void()> doing, const VECTOR_ref& MaxPos, const VECTOR_ref& MinPos, int shadowSelect) noexcept {
		if (OPTION::Instance()->Get_Shadow()) {
			m_Shadow[shadowSelect].Update(doing, MaxPos, MinPos);
		}
	}
	//
	void			DXDraw::SetAmbientLight(const VECTOR_ref& AmbientLightVec, const COLOR_F& LightColor) noexcept {
		m_LightVec = AmbientLightVec;
		m_LightColorF = LightColor;
		SetGlobalAmbientLight(LightColor);
		SetLightDirection(AmbientLightVec.get());
	}
	//
	void			DXDraw::Init(void) noexcept {
		OptionWindowClass::Instance()->Init();
		//
		m_PopUpDrawClass.Set("Exit", y_r(480), y_r(240), [&](int WinSizeX, int WinSizeY, bool) {
			auto* Pad = PadControl::Instance();
			auto* Fonts = FontPool::Instance();

			auto White = GetColor(255, 255, 255);
			auto Gray25 = GetColor(216, 216, 216);
			auto Black = GetColor(0, 0, 0);
			int xp1, yp1;
			//タイトル
			{
				xp1 = y_r(960) - WinSizeX / 2 + y_r(48);
				yp1 = y_r(540) - WinSizeY / 2 + LineHeight * 3 + LineHeight;
				int Height = LineHeight;

				int xpos = xp1 + y_r(6);
				int ypos = yp1 + (yp1 + Height - yp1) / 2;
				Fonts->Get(FontPool::FontType::Gothic_Edge).DrawString(Height, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE,
																	   xpos, ypos, White, Black, "ゲームを終了しますか？");
			}
			//
			{
				xp1 = y_r(960) - y_r(54);
				yp1 = y_r(540) + WinSizeY / 2 - LineHeight * 4;

				bool MouseOver = in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xp1, yp1, xp1 + y_r(108), yp1 + LineHeight * 2);
				int xpos = xp1 + (xp1 + y_r(108) - xp1) / 2;
				int ypos = yp1 + (yp1 + LineHeight * 2 - yp1) / 2;

				DrawBox(xp1, yp1, xp1 + y_r(108), yp1 + LineHeight * 2, MouseOver ? White : Gray25, true);
				Fonts->Get(FontPool::FontType::Gothic_Edge).DrawString(std::min(LineHeight, yp1 + LineHeight * 2 - yp1), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE,
																	   xpos, ypos, White, Black, "終了");

				if (Pad->GetKey(PADS::INTERACT).trigger() || (MouseOver && Pad->GetMouseClick().trigger())) {
					m_IsEnd = true;
				}
			}
							 });
	}
	void			DXDraw::Execute(void) noexcept {
#ifdef _USE_OPENVR_
		VR_Execute();		//VR空間に適用
#endif
		if (EffectResource::Instance()->Update_effect_f) {
			UpdateEffekseer3D();
		}
		m_PauseActive.Execute(PadControl::Instance()->GetKey(PADS::INVENTORY).press());
		if (m_SendCamShakeTime > 0.f) {
			if (m_SendCamShake) {
				m_SendCamShake = false;
				this->m_CamShake = m_SendCamShakeTime;
			}
			auto RandRange = this->m_CamShake / m_SendCamShakeTime * m_SendCamShakePower;
			Easing(&this->m_CamShake1, VECTOR_ref::vget(GetRandf(RandRange), GetRandf(RandRange), GetRandf(RandRange)), 0.8f, EasingType::OutExpo);
			Easing(&this->m_CamShake2, this->m_CamShake1, 0.8f, EasingType::OutExpo);
			this->m_CamShake = std::max(this->m_CamShake - 1.f / GetFPS(), 0.f);
		}
	}
	void			DXDraw::Draw(
		std::function<void(const Camera3DInfo&)> doing,
		std::function<void()> doingUI,
		std::function<void()> doingUI2
		) noexcept {
		auto* OptionParts = OPTION::Instance();
		auto* PostPassParts = PostPassEffect::Instance();
		//画面に映す
		if (OptionParts->Get_useVR()) {
			//VRに移す
#ifdef _USE_OPENVR_
				VR_Draw([&] {
					auto tmp = GetDrawScreen();
					Camera3DInfo tmp_cam = GetMainCamera();
					tmp_cam.SetNowCamPos();
					doing(tmp_cam);
					UI_Screen.SetDraw_Screen();	//UIをスクリーンに描画しておく
					{
						doingUI();
					}
					GraphHandle::SetDraw_Screen(tmp, tmp_cam, true);//描画対象を戻す
					{
						//結果を描画
						PostPassParts->Get_MAIN_Screen().DrawGraph(0, 0, true);	//デフォ描画
						//視差に対応しているUI
						{
							SetCameraNearFar(0.01f, 2.f);
							SetUseZBuffer3D(FALSE);												//zbufuse
							SetWriteZBuffer3D(FALSE);											//zbufwrite
							{
								DrawBillboard3D((tmp_cam.GetCamPos() + (tmp_cam.GetCamVec() - tmp_cam.GetCamPos()).Norm()*1.0f).get(), 0.5f, 0.5f, 1.8f, 0.f, UI_Screen.get(), TRUE);
							}
							SetUseZBuffer3D(TRUE);												//zbufuse
							SetWriteZBuffer3D(TRUE);											//zbufwrite
						}
						doingUI2();										//UI2
					}
				});
#endif
		}
		else {
			doing(GetMainCamera());
			//ディスプレイ描画
			GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK, true);
			{
				PostPassParts->Get_MAIN_Screen().DrawGraph(0, 0, true);	//デフォ描画
				doingUI();										//UI1
				doingUI2();										//UI2
			}
		}

	}
	bool			DXDraw::Screen_Flip(void) noexcept {
		ScreenFlip();
		/*
		auto* OptionParts = OPTION::Instance();
		if (!OptionParts->Get_Vsync()) {
			//4msだけスリープ
			while ((GetNowHiPerformanceCount() - m_StartTime) < 1000 * 1000 / 60 - 1000 * 4) {
				if (ProcessMessage() != 0) { return false; }
				Sleep(1);	// 1msecスリープする
			}
			while ((GetNowHiPerformanceCount() - m_StartTime) < 1000 * 1000 / 60) {}
			m_StartTime = GetNowHiPerformanceCount();
		}
		//*/
#ifdef _USE_OPENVR_
		VR_WaitSync();
#endif // _USE_OPENVR_
		return true;
	}

	bool			DXDraw::UpdateEscWindow(void) noexcept {
		m_PopUpDrawClass.Update(PadControl::Instance()->GetEsc().trigger());
		if (m_PopUpDrawClass.IsActive()) {
			auto* Pad = PadControl::Instance();
			Pad->ChangeGuide(
				[&]() {
					auto* KeyGuide = PadControl::Instance();
					KeyGuide->AddGuide(PADS::RELOAD, "戻る");
				}
			);
		}
		return m_IsEnd;
	}
	void			DXDraw::DrawEscWindow(void) noexcept {
		m_PopUpDrawClass.Draw();
	}
	//VR
#ifdef _USE_OPENVR_
	void			DXDraw::Get_VR_DevicePositionVR(char handle_, VECTOR_ref* pos_, MATRIX_ref*mat) noexcept {
		if (OPTION::Instance()->Get_useVR() && (handle_ != -1)) {
			*pos_ = m_VR_DeviceInfo.at(handle_).GetPos();
			*mat = m_VR_DeviceInfo.at(handle_).GetMat();
		}
		else {
			*pos_ = VECTOR_ref::zero();
			*mat = MATRIX_ref::zero();
		}
	}
	void			DXDraw::Get_VR_HMDPositionVR(VECTOR_ref* pos_, MATRIX_ref*mat) noexcept {
		Get_VR_DevicePositionVR(m_VR_HMDID, pos_, mat);
		*mat = MATRIX_ref::Axis1((*mat).xvec()*-1.f, (*mat).yvec(), (*mat).zvec()*-1.f);
		{
			auto* ptr = Get_VR_HMDDevice();
			if (!ptr->IsActive()) {
				m_VR_HMD_StartFlag = true;
			}
			if (m_VR_HMD_StartFlag && ptr->IsActive() != m_VR_PrevHMDIsActive) {
				m_VR_HMD_StartFlag = false;
				//
				VECTOR_ref pos = *pos_; pos.y(0.f);
				VECTOR_ref tmp = mat->zvec(); tmp.y(0.f); tmp = tmp.Norm();
				float rad = DX_PI_F + std::atan2f(tmp.x(), -tmp.z());
				m_VR_HMD_StartPoint = MATRIX_ref::RotY(rad) * MATRIX_ref::Mtrans(pos);
			}
			m_VR_PrevHMDIsActive = ptr->IsActive();
			*pos_ = *pos_ - m_VR_HMD_StartPoint.pos();
			*mat = *mat * m_VR_HMD_StartPoint.GetRot();
		}
	}
	const VECTOR_ref	DXDraw::Set_VR_EyePositionVR(char eye_type) noexcept {
		if (OPTION::Instance()->Get_useVR()) {
			return Get_VR_HMDDevice()->GetPos() + Get_VR_EyePosition_minVR(eye_type);
		}
		return VECTOR_ref::zero();
	}
	const VECTOR_ref	DXDraw::Get_VR_EyePosition_minVR(char eye_type) noexcept {
		if (OPTION::Instance()->Get_useVR()) {
			const vr::HmdMatrix34_t tmpmat = vr::VRSystem()->GetEyeToHeadTransform((vr::EVREye)eye_type);
			return MATRIX_ref::Vtrans(VECTOR_ref::vget(tmpmat.m[0][3], tmpmat.m[1][3], tmpmat.m[2][3]), Get_VR_HMDDevice()->GetMat());
		}
		return VECTOR_ref::zero();
	}
	void			DXDraw::Reset_VR_HMD(void) noexcept {
		m_VR_PrevHMDIsActive = false;
		m_VR_HMD_StartFlag = true;
	}
	void			DXDraw::VR_Haptic(char id_, unsigned short times) noexcept {
		if (OPTION::Instance()->Get_useVR() && (id_ != -1) && m_VR_SystemPtr) {
			m_VR_SystemPtr->TriggerHapticPulse(m_VR_DeviceInfo[id_].GetID(), 2, times);
		}
	}
	//
	void			DXDraw::VR_Init(void) noexcept {
		auto* OptionParts = OPTION::Instance();
		//システムセット
		if (OptionParts->Get_useVR()) {
			m_VR_ErrorHandle = vr::VRInitError_None;
			m_VR_SystemPtr = vr::VR_Init(&m_VR_ErrorHandle, vr::VRApplication_Scene);
			if (m_VR_ErrorHandle != vr::VRInitError_None) {
				m_VR_SystemPtr = nullptr;
				OptionParts->Set_useVR(false);
			}
		}
		//解像度指定
		if (OptionParts->Get_useVR()) {
			uint32_t t_x = 1080;
			uint32_t t_y = 1200;
			//m_VR_SystemPtr->GetRecommendedRenderTargetSize(&t_x,&t_y);
			this->m_DispXSize = int(t_x) * 2;
			this->m_DispYSize = int(t_y) * 2;
		}
		//デバイスセット
		if (OptionParts->Get_useVR()) {
			m_VR_TrackerID.clear();
			char DeviceCount = 0;
			bool IsFirstHand = true;
			for (char i = 0; i < 8; i++) {
				auto DeviceID = DeviceCount;
				auto DeviceType = m_VR_SystemPtr->GetTrackedDeviceClass(i);
				switch (DeviceType) {
				case vr::TrackedDeviceClass_HMD:
					m_VR_HMDID = DeviceID;
					DeviceCount++;
					break;
				case vr::TrackedDeviceClass_Controller:
					if (IsFirstHand) {
						IsFirstHand = false;
						m_VR_Hand1ID = DeviceID;
					}
					else {
						m_VR_Hand2ID = DeviceID;
					}
					DeviceCount++;
					break;
				case vr::TrackedDeviceClass_GenericTracker:
					m_VR_TrackerID.emplace_back(DeviceID);
					DeviceCount++;
					break;
				case vr::TrackedDeviceClass_TrackingReference:
					DeviceCount++;
					break;
				default:
					continue;
				}
				m_VR_DeviceInfo.resize(DeviceCount);
				m_VR_DeviceInfo.back().Init(DeviceID, i, DeviceType);
			}
		}
		//画面セット
		if (OptionParts->Get_useVR()) {
			m_OutScreen[0] = GraphHandle::Make(this->m_DispXSize, this->m_DispYSize);	//左目
			m_OutScreen[1] = GraphHandle::Make(this->m_DispXSize, this->m_DispYSize);	//右目
			UI_Screen = GraphHandle::Make(this->m_DispXSize, this->m_DispYSize, true);	//UI
		}
	}
	void			DXDraw::VR_Execute(void) noexcept {
		if (OPTION::Instance()->Get_useVR()) {
			for (auto& c : m_VR_DeviceInfo) {
				c.Update(m_VR_SystemPtr);
			}
		}
		else {
			for (auto& c : m_VR_DeviceInfo) {
				c.Reset();
			}
		}
	}
	void			DXDraw::VR_Draw(std::function<void()> doing) noexcept {
		if (OPTION::Instance()->Get_useVR()) {
			for (char i = 0; i < 2; i++) {
				m_OutScreen[i].SetDraw_Screen(GetMainCamera().GetCamPos() + this->Get_VR_EyePosition_minVR(i), GetMainCamera().GetCamVec() + this->Get_VR_EyePosition_minVR(i), GetMainCamera().GetCamUp(), GetMainCamera().GetCamFov(), GetMainCamera().GetCamNear(), GetMainCamera().GetCamFar());
				{
					doing();
				}
				GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
				{
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					m_OutScreen[i].DrawGraph(0, 0, false);
					//それぞれの目に送信
					vr::Texture_t tex = { (void*)GetUseDirect3D11BackBufferTexture2D(), vr::ETextureType::TextureType_DirectX,vr::EColorSpace::ColorSpace_Auto };
					vr::VRCompositor()->Submit((vr::EVREye)i, &tex, NULL, vr::Submit_Default);
				}
			}
			//ディスプレイ描画
			GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
			{
				DrawBox(0, 0, this->m_DispXSize, this->m_DispYSize, GetColor(255, 255, 255), TRUE);
				m_OutScreen[0].DrawRotaGraph(this->m_DispXSize / 2, this->m_DispYSize / 2, 0.5f, 0, false);
			}
		}
	}
	void			DXDraw::VR_WaitSync(void) noexcept {
		if (OPTION::Instance()->Get_useVR()) {
			vr::TrackedDevicePose_t tmp;
			vr::VRCompositor()->WaitGetPoses(&tmp, 1, NULL, 1);
		}
	}
	void			DXDraw::VR_Dispose(void) noexcept {
		if (OPTION::Instance()->Get_useVR()) {
			//vr::VR_Shutdown();
			m_VR_SystemPtr = nullptr;
		}
	}
#endif
};
