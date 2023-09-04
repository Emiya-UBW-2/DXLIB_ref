#include "DXLib_ref.h"

DXLib_ref::DXDraw* DXLib_ref::DXDraw::m_Singleton = nullptr;
namespace DXLib_ref {
	//
	DXDraw::DXDraw(const char* title, int dispx, int dispy) noexcept {
		OPTION::Create();
		auto* OptionParts = OPTION::Instance();

		LPCSTR font_path;
		font_path = "data/x14y24pxHeadUpDaisy.ttf"; // 読み込むフォントファイルのパス
		if (AddFontResourceEx(font_path, FR_PRIVATE, &m_Font1) == 0) {
			MessageBox(NULL, "フォント読込失敗", "", MB_OK);
		}
		font_path = "data/DSFetteGotisch.ttf"; // 読み込むフォントファイルのパス
		if (AddFontResourceEx(font_path, FR_PRIVATE, &m_Font2) == 0) {
			MessageBox(NULL, "フォント読込失敗", "", MB_OK);
		}
		//解像度設定
		this->m_DispXSize = deskx;
		this->m_DispYSize = desky;
		if (dispx != -1) {
			this->m_DispXSize = dispx;
		}
		if (dispy != -1) {
			this->m_DispYSize = dispy;
		}
#ifdef _USE_OPENVR_
		VR_Init();
#endif // _USE_OPENVR_
		SetOutApplicationLogValidFlag(TRUE);						//log
		SetMainWindowText(title);									//タイトル
		ChangeWindowMode(TRUE);										//窓表示
		SetUseDirect3DVersion(OptionParts->Get_DirectXVer());		//directX ver
		SetUseDirectInputFlag(TRUE);								//
		SetDirectInputMouseMode(TRUE);								//
		SetGraphMode(this->m_DispXSize, this->m_DispYSize, 32);		//解像度
		SetWindowSizeChangeEnableFlag(FALSE, FALSE);				//ウインドウサイズを手動不可、ウインドウサイズに合わせて拡大もしないようにする
		//SetFullSceneAntiAliasingMode(4, 3);							//アンチエイリアス
		//SetEnableXAudioFlag(TRUE);									//Xaudio(ロードが長いとロストするので必要に応じて)
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
			SetWindowStyleMode(4);
			SetWindowPos(GetMainWindowHandle(), HWND_TOPMOST, (deskx - this->m_DispXSize) / 2, (desky - this->m_DispYSize) / 2, 0, 0, SWP_FRAMECHANGED);
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
		}
		//m_StartTime = GetNowHiPerformanceCount();
#ifdef DEBUG
		DebugClass::Create();
#endif // DEBUG
		EffectResource::Create();						//エフェクト
		SoundPool::Create();							//サウンド
		PadControl::Create();							//キー
		OptionWindowClass::Create();
		OptionWindowClass::Instance()->Init();

		KeyGuideClass::Create();

		auto* SE = SoundPool::Instance();
		SE->Add((int)SoundEnumCommon::UI_Select, 2, "data/Sound/UI/cursor.wav", false);
		SE->Add((int)SoundEnumCommon::UI_OK, 1, "data/Sound/UI/hit.wav", false);
		SE->Add((int)SoundEnumCommon::UI_NG, 1, "data/Sound/UI/cancel.wav", false);
		//影生成
		Create_Shadow();
	}
	DXDraw::~DXDraw(void) noexcept {
		LPCSTR font_path;

		font_path = "data/x14y24pxHeadUpDaisy.ttf"; // 読み込むフォントファイルのパス
		if (RemoveFontResourceEx(font_path, FR_PRIVATE, &m_Font1) == 0) {
			MessageBox(NULL, "フォント読込失敗", "", MB_OK);
		}

		font_path = "data/DSFetteGotisch.ttf"; // 読み込むフォントファイルのパス
		if (RemoveFontResourceEx(font_path, FR_PRIVATE, &m_Font2) == 0) {
			MessageBox(NULL, "フォント読込失敗", "", MB_OK);
		}

		//影削除
		Delete_Shadow();
#ifdef _USE_OPENVR_
		VR_Dispose();
#endif // _USE_OPENVR_
		Effkseer_End();
		DxLib_End();
	}
	//
	void			DXDraw::Create_Shadow(void) noexcept {
		auto size = int(pow(2, 13));
		//近
		if (m_NearShadowHandle == -1) {
			m_NearShadowHandle = MakeShadowMap(size, size);
		}
		SetShadowMapAdjustDepth(m_NearShadowHandle, 0.0005f);
		//中
		if (m_MiddleShadowHandle == -1) {
			m_MiddleShadowHandle = MakeShadowMap(size, size);
		}
		SetShadowMapAdjustDepth(m_MiddleShadowHandle, 0.0005f);
		//遠
		if (m_FarShadowHandle == -1) {
			m_FarShadowHandle = MakeShadowMap(size, size);
		}
		SetShadowMapAdjustDepth(m_FarShadowHandle, 0.0003f);
	}
	void			DXDraw::Delete_Shadow(void) noexcept {
		if (m_NearShadowHandle != -1) {
			DeleteShadowMap(m_NearShadowHandle);
			m_NearShadowHandle = -1;
		}
		if (m_MiddleShadowHandle != -1) {
			DeleteShadowMap(m_MiddleShadowHandle);
			m_MiddleShadowHandle = -1;
		}
		if (m_FarShadowHandle != -1) {
			DeleteShadowMap(m_FarShadowHandle);
			m_FarShadowHandle = -1;
		}
	}
	//
	void			DXDraw::SetAmbientLight(const VECTOR_ref& AmbientLightVec, const COLOR_F& LightColor) noexcept {
		SetGlobalAmbientLight(LightColor);
		SetLightDirection(AmbientLightVec.get());
	}
	void			DXDraw::Update_Shadow(std::function<void()> doing, const VECTOR_ref& CenterPos, const VECTOR_ref& size, int shadowSelect) noexcept {
		if (OPTION::Instance()->Get_Shadow()) {
			int handle = -1;
			switch (shadowSelect) {
			case 0:
				handle = m_NearShadowHandle;
				break;
			case 1:
				handle = m_MiddleShadowHandle;
				break;
			case 2:
				handle = m_FarShadowHandle;
				break;
			default:
				break;
			}
			if (handle != -1) {
				SetShadowMapLightDirection(handle, GetLightDirection());
				SetShadowMapDrawArea(handle, (CenterPos - size).get(), (CenterPos + size).get());
				ShadowMap_DrawSetup(handle);
				doing();
				ShadowMap_DrawEnd();
			}
		}
	}
	//
	void			DXDraw::Execute(void) noexcept {
#ifdef _USE_OPENVR_
		VR_Execute();		//VR空間に適用
#endif
		if (EffectResource::Instance()->Update_effect_f) {
			UpdateEffekseer3D();
		}
	}
	void			DXDraw::Draw(
		const Camera3DInfo&  cams,
		std::function<void()> doingBG3D,
		std::function<void()> doingMain3D,
		std::function<void()> doingUI,
		std::function<void()> doingUI2,
		std::function<void()> doingAfterScreen) noexcept {
		auto* OptionParts = OPTION::Instance();
		auto* PostPassParts = PostPassEffect::Instance();

		auto doing = [&](const Camera3DInfo& tmp_cam) {
			PostPassParts->BUF_Draw(
				doingBG3D,
				[&] {
					//影を追加して描画
					if (OptionParts->Get_Shadow()) {
						SetUseShadowMap(0, m_FarShadowHandle);
						SetUseShadowMap(1, m_MiddleShadowHandle);
						SetUseShadowMap(2, m_NearShadowHandle);
					}
					doingMain3D();
					if (OptionParts->Get_Shadow()) {
						SetUseShadowMap(0, -1);
						SetUseShadowMap(1, -1);
						SetUseShadowMap(2, -1);
					}
				},
				tmp_cam);												//描画
			PostPassParts->SetPostpassEffect();							//ポストパスエフェクトの適用
			doingAfterScreen();											//完成した画面に対して後処理
		};
		//画面に映す
		if (OptionParts->Get_useVR()) {
			//VRに移す
#ifdef _USE_OPENVR_
			VR_Draw([&] {
				auto tmp = GetDrawScreen();
				Camera3DInfo tmp_cam = cams;
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
			}, cams);
#endif
		}
		else {
			doing(cams);
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
	void			DXDraw::VR_Draw(std::function<void()> doing, const Camera3DInfo& cams) noexcept {
		if (OPTION::Instance()->Get_useVR()) {
			for (char i = 0; i < 2; i++) {
				m_OutScreen[i].SetDraw_Screen(cams.GetCamPos() + this->Get_VR_EyePosition_minVR(i), cams.GetCamVec() + this->Get_VR_EyePosition_minVR(i), cams.GetCamUp(), cams.GetCamFov(), cams.GetCamNear(), cams.GetCamFar());
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
