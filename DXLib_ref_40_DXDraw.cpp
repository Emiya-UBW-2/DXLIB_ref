#include "DXLib_ref_40_DXDraw.hpp"

namespace DXLibRef {
	const DXDraw* SingletonBase<DXDraw>::m_Singleton = nullptr;
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
#ifdef _USE_OPENVR_
	//AI用
	class DXDraw::VRControl {
		class VRDeviceClass {
		private:
			int							m_ID{0};
			char						m_DeviceNumber{0};
			vr::ETrackedDeviceClass 	m_type{vr::TrackedDeviceClass_Invalid};
			bool						m_isInitialized{false};
			bool						m_isActive{false};
			//位置やボタン判定
			uint64_t					m_ButtonPressFlag{0};
			uint64_t					m_ButtonTouchFlag{0};
			Vector3DX					m_TouchPadPoint{Vector3DX::zero()};
			moves						m_move;
		public:
			const auto&		GetID(void) const noexcept { return m_ID; }
			const auto		IsActive(void) const noexcept { return m_isInitialized && m_isActive; }
			const bool		PadPress(VR_PAD ID) const noexcept {
				switch (ID) {
					case VR_PAD::TRIGGER:
						return(m_ButtonPressFlag & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Trigger));
					case VR_PAD::SIDEBUTTON:
						return(m_ButtonPressFlag & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_Grip));
					case VR_PAD::TOUCHPAD:
						return(m_ButtonPressFlag & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Touchpad));
					case VR_PAD::TOPBUTTON1:
						return(m_ButtonPressFlag & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_ApplicationMenu));
					case VR_PAD::TOPBUTTON2:
						return(m_ButtonPressFlag & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_IndexController_B));
					default:
						return false;
				}
			}
			const bool		PadTouch(VR_PAD ID) const noexcept {
				switch (ID) {
					case VR_PAD::TRIGGER:
						return(m_ButtonTouchFlag & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Trigger));
					case VR_PAD::SIDEBUTTON:
						return(m_ButtonTouchFlag & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_Grip));
					case VR_PAD::TOUCHPAD:
						return(m_ButtonTouchFlag & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Touchpad));
					case VR_PAD::TOPBUTTON1:
						return(m_ButtonTouchFlag & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_ApplicationMenu));
					case VR_PAD::TOPBUTTON2:
						return(m_ButtonTouchFlag & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_IndexController_B));
					default:
						return false;
				}
			}
			const auto&		GetTouchPadPoint(void) const noexcept { return m_TouchPadPoint; }
			const auto&		GetMove(void) const noexcept { return m_move; }
		public:
			void Init(int ID, char Num, vr::ETrackedDeviceClass Type) {
				this->m_ID = ID;
				this->m_DeviceNumber = Num;
				this->m_type = Type;
				this->m_isInitialized = true;
				this->m_isActive = false;
				Reset();
			}
			void Update(vr::IVRSystem* SystemPtr) {
				vr::TrackedDevicePose_t tmp;
				switch (this->m_type) {
					case vr::TrackedDeviceClass_HMD:
						SystemPtr->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0f, &tmp, 1);
						this->m_ButtonPressFlag = 0;
						this->m_ButtonTouchFlag = 0;
						this->m_TouchPadPoint = Vector3DX::zero();
						this->m_isActive = tmp.bPoseIsValid;
						this->m_move.pos = Vector3DX::vget(tmp.mDeviceToAbsoluteTracking.m[0][3], tmp.mDeviceToAbsoluteTracking.m[1][3], -tmp.mDeviceToAbsoluteTracking.m[2][3]);
						this->m_move.mat = Matrix4x4DX::Axis1(
							Vector3DX::vget(tmp.mDeviceToAbsoluteTracking.m[0][1], tmp.mDeviceToAbsoluteTracking.m[1][1], -tmp.mDeviceToAbsoluteTracking.m[2][1]),
							Vector3DX::vget(tmp.mDeviceToAbsoluteTracking.m[0][2], tmp.mDeviceToAbsoluteTracking.m[1][2], -tmp.mDeviceToAbsoluteTracking.m[2][2])*-1.f
						);
						break;
					case vr::TrackedDeviceClass_Controller:
					case vr::TrackedDeviceClass_TrackingReference:
					case vr::TrackedDeviceClass_GenericTracker:
						{
							vr::VRControllerState_t night;
							SystemPtr->GetControllerStateWithPose(vr::TrackingUniverseStanding, this->m_DeviceNumber, &night, sizeof(night), &tmp);
							this->m_ButtonPressFlag = night.ulButtonPressed;
							this->m_ButtonTouchFlag = night.ulButtonTouched;
							this->m_TouchPadPoint = Vector3DX::vget(night.rAxis[0].x, night.rAxis[0].y, 0);
							this->m_isActive = tmp.bPoseIsValid;
							this->m_move.pos = Vector3DX::vget(tmp.mDeviceToAbsoluteTracking.m[0][3], tmp.mDeviceToAbsoluteTracking.m[1][3], -tmp.mDeviceToAbsoluteTracking.m[2][3]);
							this->m_move.mat = Matrix4x4DX::Axis1(
								Vector3DX::vget(tmp.mDeviceToAbsoluteTracking.m[0][1], tmp.mDeviceToAbsoluteTracking.m[1][1], -tmp.mDeviceToAbsoluteTracking.m[2][1]),
								Vector3DX::vget(tmp.mDeviceToAbsoluteTracking.m[0][2], tmp.mDeviceToAbsoluteTracking.m[1][2], -tmp.mDeviceToAbsoluteTracking.m[2][2])*-1.f
							);
						}
						break;
					default:
						break;
				}
			}
			void Reset() {
				this->m_ButtonPressFlag = 0;
				this->m_ButtonTouchFlag = 0;
				this->m_TouchPadPoint = Vector3DX::zero();
				this->m_move.pos.clear();
				this->m_move.mat.clear();
			}
		};
	private:
		vr::IVRSystem*				m_VR_SystemPtr{nullptr};
		vr::EVRInitError			m_VR_ErrorHandle{vr::VRInitError_None};
		std::vector<VRDeviceClass>	m_VR_DeviceInfo;
		char						m_VR_HMDID{-1};
		char						m_VR_Hand1ID{-1};
		char						m_VR_Hand2ID{-1};
		std::vector<char>			m_VR_TrackerID;
		bool						m_VR_PrevHMDIsActive{false};
		bool						m_VR_HMD_StartFlag{true};
		Matrix4x4DX					m_VR_HMD_StartPoint;
		//
		GraphHandle					m_OutScreen;							//スクリーンバッファ
	public:
		VRControl(void) noexcept {}
		~VRControl(void) noexcept {}
	public:
		const VRDeviceClass*	Get_VR_Hand1Device(void) const noexcept { return  (m_VR_Hand1ID >= 0) ? &m_VR_DeviceInfo.at(m_VR_Hand1ID) : nullptr; }
		const VRDeviceClass*	Get_VR_Hand2Device(void) const noexcept { return (m_VR_Hand2ID >= 0) ? &m_VR_DeviceInfo.at(m_VR_Hand2ID) : nullptr; }
		const VRDeviceClass*	GetTrackerDevice(int sel) const noexcept { return (0 <= sel && sel < m_VR_TrackerID.size()) ? &m_VR_DeviceInfo.at(m_VR_TrackerID.at(sel)) : nullptr; }
		void			ResetHMD(void) noexcept {
			m_VR_PrevHMDIsActive = false;
			m_VR_HMD_StartFlag = true;
		}
		void			GetHMDPosition(Vector3DX* pos_, Matrix4x4DX*mat) noexcept {
			auto* HMDPtr = (m_VR_HMDID >= 0) ? &m_VR_DeviceInfo.at(m_VR_HMDID) : nullptr;
			if (HMDPtr) {
				*mat = HMDPtr->GetMove().mat;
				*mat = Matrix4x4DX::Axis1((*mat).yvec(), (*mat).zvec()*-1.f);
				if (!HMDPtr->IsActive()) {
					m_VR_HMD_StartFlag = true;
				}
				if (m_VR_HMD_StartFlag && HMDPtr->IsActive() != m_VR_PrevHMDIsActive) {
					m_VR_HMD_StartFlag = false;
					//
					Vector3DX pos = HMDPtr->GetMove().pos; pos.y(0.f);
					Vector3DX tmp = mat->zvec(); tmp.y(0.f); tmp = tmp.normalized();
					float rad = DX_PI_F + std::atan2f(tmp.x, -tmp.z);
					m_VR_HMD_StartPoint = Matrix4x4DX::RotAxis(Vector3DX::up(), rad) * Matrix4x4DX::Mtrans(pos);
				}
				m_VR_PrevHMDIsActive = HMDPtr->IsActive();
				*pos_ = HMDPtr->GetMove().pos - m_VR_HMD_StartPoint.pos();
				*mat = *mat * m_VR_HMD_StartPoint.rotation();
			}
			else {
				*pos_ = Vector3DX::zero();
				*mat = Matrix4x4DX::identity();
				ResetHMD();
			}
		}
		void			Haptic(char id_, unsigned short times) noexcept {
			if (OPTION::Instance()->GetParamBoolean(EnumSaveParam::usevr) && (id_ != -1) && m_VR_SystemPtr) {
				m_VR_SystemPtr->TriggerHapticPulse(m_VR_DeviceInfo[id_].GetID(), 2, times);
			}
		}
		const auto		GetEyePosition(char eye_type) noexcept {
			if (OPTION::Instance()->GetParamBoolean(EnumSaveParam::usevr)) {
				auto* HMDPtr = (m_VR_HMDID >= 0) ? &m_VR_DeviceInfo.at(m_VR_HMDID) : nullptr;
				const vr::HmdMatrix34_t tmpmat = vr::VRSystem()->GetEyeToHeadTransform((vr::EVREye)eye_type);
				return Matrix4x4DX::Vtrans(Vector3DX::vget(tmpmat.m[0][3], tmpmat.m[1][3], tmpmat.m[2][3]), HMDPtr->GetMove().mat);
			}
			return Vector3DX::zero();
		}
		const GraphHandle* GetOutBuffer() const noexcept { return &m_OutScreen; }
		const auto		GetCamPos(char eye_type) noexcept {
			auto* DrawParts = DXDraw::Instance();
			Camera3DInfo tmp_cam = DrawParts->GetMainCamera();
			tmp_cam.SetCamPos(
				tmp_cam.GetCamPos() + this->GetEyePosition(eye_type),
				tmp_cam.GetCamVec() + this->GetEyePosition(eye_type),
				tmp_cam.GetCamUp()
			);
			return tmp_cam;
		}
	public:
		void Init() noexcept {
			auto* OptionParts = OPTION::Instance();
			//VRが使えるかチェック
			if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
				m_VR_ErrorHandle = vr::VRInitError_None;
				m_VR_SystemPtr = vr::VR_Init(&m_VR_ErrorHandle, vr::VRApplication_Scene);
				if (m_VR_ErrorHandle != vr::VRInitError_None) {
					m_VR_SystemPtr = nullptr;
					OptionParts->Set_useVR(false);
				}
			}
			//デバイスセット
			if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
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
		}
		void SetupBuffer(int xsize,int ysize) noexcept {
			if (OPTION::Instance()->GetParamBoolean(EnumSaveParam::usevr)) {
				auto* DrawParts = DXDraw::Instance();
				//画面セット
				m_OutScreen = GraphHandle::Make(xsize, ysize);	//左目
			}
		}
		void Execute(void) noexcept {
			if (OPTION::Instance()->GetParamBoolean(EnumSaveParam::usevr)) {
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
		void Submit(char eye_type) noexcept {
			vr::Texture_t tex = {(void*)GetUseDirect3D11BackBufferTexture2D(), vr::ETextureType::TextureType_DirectX,vr::EColorSpace::ColorSpace_Auto};
			vr::VRCompositor()->Submit((vr::EVREye)eye_type, &tex, NULL, vr::Submit_Default);
		}
		void SubmitDraw(char eye_type, const GraphHandle& MainDrawScreen, const GraphHandle& UIScreen, std::function<void()> doingUI2) noexcept {
			Camera3DInfo tmp_cam = GetCamPos(eye_type);
			//目線用に合成
			m_OutScreen.SetDraw_Screen(tmp_cam);
			{
				MainDrawScreen.DrawGraph(0, 0, true);
				{
					//視差に対応しているUI
					SetCameraNearFar(0.01f, 2.f);
					SetUseZBuffer3D(FALSE);												//zbufuse
					SetWriteZBuffer3D(FALSE);											//zbufwrite
					{
						DrawBillboard3D((tmp_cam.GetCamPos() + (tmp_cam.GetCamVec() - tmp_cam.GetCamPos()).normalized() * 1.0f).get(),
							0.5f, 0.5f, 1.8f, 0.f, UIScreen.get(), TRUE);
					}
					SetUseZBuffer3D(TRUE);												//zbufuse
					SetWriteZBuffer3D(TRUE);											//zbufwrite
				}
			}
			//合成したものをBACKに持ってきて
			GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
			{
				m_OutScreen.DrawGraph(0, 0, false);
				doingUI2();
			}
			//それぞれの目にDX_SCREEN_BACKの内容を送信
			this->Submit(eye_type);
		}
		void WaitSync(void) noexcept {
			if (OPTION::Instance()->GetParamBoolean(EnumSaveParam::usevr)) {
				vr::TrackedDevicePose_t tmp;
				vr::VRCompositor()->WaitGetPoses(&tmp, 1, NULL, 1);
			}
		}
		void Dispose(void) noexcept {
			if (OPTION::Instance()->GetParamBoolean(EnumSaveParam::usevr)) {
				//vr::VR_Shutdown();
				m_VR_SystemPtr = nullptr;
			}
		}
	};
#else
	//AI用
	class DXDraw::VRControl {
		class VRDeviceClass {
		public:
			const bool		PadPress(VR_PAD) const noexcept { return false; }
			const bool		PadTouch(VR_PAD) const noexcept { return false; }
			const auto&		GetTouchPadPoint(void) const noexcept { return Vector3DX::zero(); }
		};
	public:
		VRControl(void) noexcept {}
		~VRControl(void) noexcept {}
	public:
		const VRDeviceClass*	Get_VR_Hand1Device(void) const noexcept { return  nullptr; }
		const VRDeviceClass*	Get_VR_Hand2Device(void) const noexcept { return nullptr; }
		const VRDeviceClass*	GetTrackerDevice(int) const noexcept { return nullptr; }
		void			ResetHMD(void) noexcept {}
		void			GetHMDPosition(Vector3DX*, Matrix4x4DX*) noexcept {}
		void			Haptic(char, unsigned short) noexcept {}
		const auto		GetEyePosition(char) noexcept { return Vector3DX::zero(); }
		const GraphHandle* GetOutBuffer() const noexcept { return nullptr; }
		const auto		GetCamPos(char eye_type) noexcept {
			auto* DrawParts = DXDraw::Instance();
			Camera3DInfo tmp_cam = DrawParts->GetMainCamera();
			tmp_cam.SetCamPos(
				tmp_cam.GetCamPos() + this->GetEyePosition(eye_type),
				tmp_cam.GetCamVec() + this->GetEyePosition(eye_type),
				tmp_cam.GetCamUp()
			);
			return tmp_cam;
		}
	public:
		void Init() noexcept {}
		void SetupBuffer(int, int) noexcept {}
		void Execute(void) noexcept {}
		void Submit(char) noexcept {}
		void SubmitDraw(char, const GraphHandle&, const GraphHandle&, std::function<void()>) noexcept {}
		void WaitSync(void) noexcept {}
		void Dispose(void) noexcept {}
	};
#endif
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	void DXDraw::ShadowDraw::Init(int ShadowMapSize, int dispsizex, int dispsizey) {
		//
		BaseShadowHandle = GraphHandle::Make(dispsizex, dispsizey, TRUE);
		{
			int size = 2 << ShadowMapSize;
			DepthBaseScreenHandle = GraphHandle::Make(size, size, FALSE);				// 法線バッファの作成
			SetDrawValidFloatTypeGraphCreateFlag(TRUE);
			SetCreateDrawValidGraphChannelNum(2);
			SetCreateGraphChannelBitDepth(32);
			DepthScreenHandle = GraphHandle::Make(size, size, FALSE);				// 法線バッファの作成
			DepthFarScreenHandle = GraphHandle::Make(size, size, FALSE);				// 法線バッファの作成

			// 設定を元に戻す
			SetDrawValidFloatTypeGraphCreateFlag(FALSE);
			SetCreateDrawValidGraphChannelNum(4);
			SetCreateGraphColorBitDepth(32);
		}
		// 深度記録画像を使ったディレクショナルライト一つの描画用頂点シェーダーを読み込む
		m_Shader.Init("shader/VS_SoftShadow.vso", "shader/PS_SoftShadow.pso");
	}
	void DXDraw::ShadowDraw::SetupCam(Vector3DX Center, float scale) const noexcept {
		float Scale_Rate = 12.5f;
		ClearDrawScreen();
		SetupCamera_Ortho(30.f * scale * Scale_Rate);		// カメラのタイプを正射影タイプにセット、描画範囲も指定
		SetCameraNearFar(0.05f * scale * Scale_Rate, 60.f * scale * Scale_Rate);		// 描画する奥行き範囲をセット
		// カメラの位置と注視点はステージ全体が見渡せる位置
		auto Vec = m_ShadowVec;
		if (Vec.x == 0.f && m_ShadowVec.z == 0.f) {
			Vec.z = (0.1f);
		}
		SetCameraPositionAndTarget_UpVecY((Center - Vec.normalized() * (30.f * scale * Scale_Rate)).get(), Center.get());
	}
	void DXDraw::ShadowDraw::Update(std::function<void()> Shadowdoing, Vector3DX Center) {
		// 影用の深度記録画像の準備を行う
		SetRenderTargetToShader(0, DepthBaseScreenHandle.get());
		SetRenderTargetToShader(1, -1);
		SetRenderTargetToShader(2, DepthScreenHandle.get());
		{
			SetupCam(Center, 1.f);
			m_CamViewMatrix[0] = GetCameraViewMatrix();
			m_CamProjectionMatrix[0] = GetCameraProjectionMatrix();
			Shadowdoing();
			// 設定したカメラのビュー行列と射影行列を取得しておく
			m_Shader.SetVertexCameraMatrix(4, m_CamViewMatrix[0], m_CamProjectionMatrix[0]);

			Shadowdoing();
		}
		SetRenderTargetToShader(0, -1);
		SetRenderTargetToShader(1, -1);
		SetRenderTargetToShader(2, -1);
	}
	void DXDraw::ShadowDraw::UpdateFar(std::function<void()> Shadowdoing, Vector3DX Center) {
		// 影用の深度記録画像の準備を行う
		SetRenderTargetToShader(0, DepthBaseScreenHandle.get());
		SetRenderTargetToShader(1, -1);
		SetRenderTargetToShader(2, DepthFarScreenHandle.get());
		{
			SetupCam(Center, 2.f);
			m_CamViewMatrix[1] = GetCameraViewMatrix();
			m_CamProjectionMatrix[1] = GetCameraProjectionMatrix();
			Shadowdoing();
			// 設定したカメラのビュー行列と射影行列を取得しておく
			m_Shader.SetVertexCameraMatrix(5, m_CamViewMatrix[1], m_CamProjectionMatrix[1]);
		}
		SetRenderTargetToShader(0, -1);
		SetRenderTargetToShader(1, -1);
		SetRenderTargetToShader(2, -1);
	}
	void DXDraw::ShadowDraw::SetDraw(std::function<void()> doing, Camera3DInfo tmp_cam) {
		auto* OptionParts = OPTION::Instance();
		SetUseTextureToShader(1, DepthScreenHandle.get());				// 影用深度記録画像をテクスチャ１にセット
		SetUseTextureToShader(2, DepthFarScreenHandle.get());			// 影用深度記録画像をテクスチャ１にセット
		// 影の結果を出力
		float Scale_Rate = 12.5f;
		tmp_cam.SetCamInfo(tmp_cam.GetCamFov(), 0.1f* Scale_Rate, 100.f* Scale_Rate);
		BaseShadowHandle.SetDraw_Screen(tmp_cam);
		{
			m_Shader.SetPixelParam(3, (float)OptionParts->GetParamInt(EnumSaveParam::shadow), 0.f, 0.f, 0.f);
			m_Shader.Draw_lamda(doing);
		}
		SetUseTextureToShader(1, -1);				// 使用テクスチャの設定を解除
		SetUseTextureToShader(2, -1);				// 使用テクスチャの設定を解除
		//後処理
		//*
		//GraphFilter(BaseShadowHandle.get(), DX_GRAPH_FILTER_GAUSS, 16, 1000);
		GraphBlend(BaseShadowHandle.get(), DepthBaseScreenHandle.get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				   DX_RGBA_SELECT_SRC_G,    // 出力結果の赤成分は AlphaHandle の緑成分
				   DX_RGBA_SELECT_SRC_G,    // 出力結果の緑成分は AlphaHandle の赤成分
				   DX_RGBA_SELECT_SRC_G,    // 出力結果の青成分は AlphaHandle の青成分
				   DX_RGBA_SELECT_SRC_R    // 出力結果のアルファ成分は BlendHandle の赤成分
		);
		//*/
	}
	void DXDraw::ShadowDraw::Draw() {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
		BaseShadowHandle.DrawGraph(0, 0, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		//DepthBaseScreenHandle.DrawExtendGraph(0, 0, 960, 960, false);
	}
	void DXDraw::ShadowDraw::Dispose() {
		BaseShadowHandle.Dispose();
		DepthBaseScreenHandle.Dispose();
		DepthScreenHandle.Dispose();
		DepthFarScreenHandle.Dispose();
		m_Shader.Dispose();
	}
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	DXDraw::DXDraw(void) noexcept {
		OPTION::Create();
		LocalizePool::Create();
		SaveDataClass::Create();
		auto* SaveDataParts = SaveDataClass::Instance();
		auto* OptionParts = OPTION::Instance();
		auto* LocalizeParts = LocalizePool::Instance();
		//ロード
		m_IsFirstBoot = false;
		if (!SaveDataParts->Load()) {
			m_IsFirstBoot = true;
			//初回データ作成
			SaveDataParts->Save();
		}

		//VR初期化
		if (!m_IsFirstBoot) {
			m_VRControl = new VRControl;
			this->GetVRControl()->Init();
		}
		//
		if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
			//解像度指定
			uint32_t t_x = 1080;
			uint32_t t_y = 1200;
			//m_VR_SystemPtr->GetRecommendedRenderTargetSize(&t_x,&t_y);
			this->m_DispXSize = int(t_x) * 2;
			this->m_DispYSize = int(t_y) * 2;
		}
		else {
			//解像度指定
			int dx = deskx;
			int dy = desky;
			if (dy < (deskx * basey / basex)) {//16:9より横長
				dx = (desky * basex / basey);
				dy = desky;
			}
			else {//4:3
				dx = deskx;
				dy = (deskx * basey / basex);
			}
			this->m_DispXSize = dx;
			this->m_DispYSize = dy;
			//DPI設定
			int DPI = 96;
			//GetMonitorDpi(NULL,&DPI);
			if (SetProcessDPIAware() != 0) {
				auto hdc = GetDC(nullptr);                 // カレントのスクリーン全体のデバイスコンテキスト取得.
				DPI = GetDeviceCaps(hdc, LOGPIXELSY);
			}
			this->m_DispXSize = this->m_DispXSize * DPI / 96;
			this->m_DispYSize = this->m_DispYSize * DPI / 96;
			//
		}
		this->GetVRControl()->SetupBuffer(this->m_DispXSize, this->m_DispYSize);
		int DXVer = DirectXVerID[OptionParts->GetParamInt(EnumSaveParam::DirectXVer)];
		SetOutApplicationLogValidFlag(TRUE);						//log
		if (m_IsFirstBoot) {
			SetMainWindowText("FirstBoot Option");						//タイトル
		}
		else {
			SetMainWindowText("Loading...");							//タイトル
		}
		ChangeWindowMode(TRUE);										//窓表示
		SetUseDirect3DVersion(DXVer);								//directX ver
		SetUseDirectInputFlag(TRUE);								//
		SetDirectInputMouseMode(TRUE);								//
		SetGraphMode(this->m_DispXSize, this->m_DispYSize, 32);		//解像度
		SetWindowSizeChangeEnableFlag(FALSE, FALSE);				//ウインドウサイズを手動不可、ウインドウサイズに合わせて拡大もしないようにする
		Set3DSoundOneMetre(1.0f);									//
		SetWaitVSyncFlag(FALSE);									//垂直同期
		SetZBufferBitDepth(32);										//
		DxLib_Init();												//
		SetUsePixelLighting(TRUE);									//ピクセルライティングの使用
		if (GetUseDirect3DVersion() != DXVer) {
			MessageBox(NULL, LocalizeParts->Get(10), "", MB_OK);
		}
		if (!m_IsFirstBoot) {
			Effekseer_Init(8000);										//Effekseer
			SetSysCommandOffFlag(TRUE);									//
			SetChangeScreenModeGraphicsSystemResetFlag(FALSE);			//Effekseer
			Effekseer_SetGraphicsDeviceLostCallbackFunctions();			//Effekseer
		}
		SetAlwaysRunFlag(TRUE);										//background
		SetUseZBuffer3D(TRUE);										//zbufuse
		SetWriteZBuffer3D(TRUE);									//zbufwrite
		//MV1SetLoadModelPhysicsWorldGravity(M_GR);					//重力
		if (!OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
			//SetWindowPos(GetMainWindowHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED);

			int x1, y1, x2, y2;
			GetWindowEdgeWidth(&x1, &y1, &x2, &y2);
			SetWindowSize(this->m_DispXSize, this->m_DispYSize);
			this->m_DispXSize_Win = this->m_DispXSize - 72 * 16 / 9;
			this->m_DispYSize_Win = this->m_DispYSize - 72;
			this->m_DispXSize_Border = this->m_DispXSize;
			this->m_DispYSize_Border = this->m_DispYSize;
		}
		m_DispXSize_Max = std::min(m_DispXSize_Border, (int)(std::min(basex, this->m_DispXSize)*std::clamp(OptionParts->GetParamFloat(EnumSaveParam::DrawScale) + 0.1f, 0.25f, 10.f)));
		m_DispYSize_Max = std::min(m_DispYSize_Border, (int)(std::min(basey, this->m_DispYSize)*std::clamp(OptionParts->GetParamFloat(EnumSaveParam::DrawScale) + 0.1f, 0.25f, 10.f)));
		SetWindowOrBorderless();
		//
#ifdef DEBUG
		DebugClass::Create();
#endif // DEBUG
		EffectResource::Create();						//エフェクト
		SoundPool::Create();							//サウンド
		BGMPool::Create();
		FontPool::Create();
		PadControl::Create();							//キー
		OptionWindowClass::Create();
		PadControl::Create();
		ObjectManager::Create();
		SideLog::Create();
		PopUp::Create();
		UISystem::Create();

		auto* SE = SoundPool::Instance();
		SE->Add((int)SoundEnumCommon::UI_Select, 2, "data/Sound/UI/cursor.wav", false);
		SE->Add((int)SoundEnumCommon::UI_CANCEL, 1, "data/Sound/UI/cancel.wav", false);
		SE->Add((int)SoundEnumCommon::UI_OK, 1, "data/Sound/UI/ok.wav", false);
		SE->Add((int)SoundEnumCommon::UI_NG, 1, "data/Sound/UI/ng.wav", false);
		SE->SetVol(OptionParts->GetParamFloat(EnumSaveParam::SE));
		if (!m_IsFirstBoot) {
			//影生成
			m_PrevShadow = false;
			m_ShadowDraw.Init(12, this->GetDispXSize(), this->GetDispYSize());
			//Init
			m_PauseActive.Set(false);
			//
			this->m_ScreenVertex.SetScreenVertex(this->GetDispXSize(), this->GetDispYSize());								// 頂点データの準備
			this->m_Shader2D[0].Init("shader/VS_lens.vso", "shader/PS_lens.pso");																//レンズ
			this->m_Shader2D[1].Init("shader/VS_BlackOut.vso", "shader/PS_BlackOut.pso");						//レンズ

			m_PBR_Shader.Init("shader/VS_PBR3D.vso", "shader/PS_PBR3D.pso");
			LightPool::Create();
		}
	}
	DXDraw::~DXDraw(void) noexcept {
		if (m_VRControl) {
			this->GetVRControl()->Dispose();
			delete m_VRControl;
		}
		if (!m_IsFirstBoot) {
			m_ShadowDraw.Dispose();
			if (m_IsCubeMap) {
				m_RealTimeCubeMap.Dispose();
			}
			Effkseer_End();

			m_PBR_Shader.Dispose();
		}
		DxLib_End();
	}
	//
	void			DXDraw::SetPause(bool value) noexcept {
		auto* Pad = PadControl::Instance();
		if (value != IsPause()) {
			m_PauseActive.Execute(true);
			Pad->SetGuideUpdate();
		}
	}
	//
	void			DXDraw::SetAmbientLight(const Vector3DX& AmbientLightVec, const COLOR_F& LightColor) noexcept {
		m_LightVec = AmbientLightVec;
		m_LightColorF = LightColor;
		SetGlobalAmbientLight(LightColor);
		SetLightDirection(AmbientLightVec.get());
		m_ShadowDraw.SetVec(AmbientLightVec);
	}
	//
	void			DXDraw::Init(void) noexcept {
		auto* OptionWindowParts = OptionWindowClass::Instance();
		auto* Pad = PadControl::Instance();
		auto* PopUpParts = PopUp::Instance();
		auto* Fonts = FontPool::Instance();
		auto* LocalizeParts = LocalizePool::Instance();
		auto* OptionParts = OPTION::Instance();

		OptionWindowParts->Init();
		Update_effect_was = GetNowHiPerformanceCount();

		if (m_IsFirstBoot) {
			//this->GetDispXSize(), this->GetDispYSize()
			m_CheckPCSpec.Set();

			int xsize = y_r(1366);
			int ysize = y_r(768);

			UI_Screen = GraphHandle::Make(xsize, ysize, false);	//UI
			//初期設定画面
			OptionWindowParts->SetActive();
			while (ProcessMessage() == 0) {
				int xopt = xsize * this->GetDispXSize() / y_r(1920);
				int yopt = ysize * this->GetDispYSize() / y_r(1080);
				SetWindowPosition((deskx - xopt) / 2, (desky - yopt) / 2);
				SetWindowSize(xopt, yopt);

				Pad->Execute();
				OptionWindowParts->Execute();
				PopUpParts->Update();
				if (!PopUpParts->IsActivePop()) {
					break;
				}
				//
				UI_Screen.SetDraw_Screen();
				{
					PopUpParts->Draw(y_r(720 / 2 + 16), y_r(720 / 2 + 16));
					Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_r(12), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE, y_r(32), y_r(720 + 16) + y_r(32 / 2), Green, Black, LocalizeParts->Get(109));

					int xp = y_r(720 + 16 + 16);
					int yp = y_r(16);
					if (WindowSystem::SetMsgClickBox(xp, yp, xp + y_r(400), yp + LineHeight, Gray50, LocalizeParts->Get(2000))) {
						m_CheckPCSpec.StartSearch();
					}
					yp += y_r(24);
					if (m_CheckPCSpec.GetCPUDatas()) {
						int MouseOverID = -1;
						//CPU
						WindowSystem::SetMsg(xp, yp, xsize - y_r(16), yp + LineHeight, LineHeight, FontHandle::FontXCenter::LEFT, White, DarkGreen, LocalizeParts->Get(2001));yp += LineHeight;
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
							if (IntoMouse(xp + y_r(16), yp, xsize - y_r(16), yp + LineHeight * 2)) {
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
							WindowSystem::SetMsg(xp + y_r(16), yp, xsize - y_r(16), yp + LineHeight, LineHeight, FontHandle::FontXCenter::LEFT, White, DarkGreen, "[%s]", c.m_Name.c_str());
							WindowSystem::SetMsg(xp + y_r(16), yp, xsize - y_r(16), yp + LineHeight, LineHeight * 2 / 3, FontHandle::FontXCenter::RIGHT, Color, DarkGreen, "%s", LocalizeParts->Get(TextID));yp += LineHeight;
							WindowSystem::SetMsg(xp + y_r(16), yp, xsize - y_r(16), yp + LineHeight, LineHeight, FontHandle::FontXCenter::RIGHT, White, DarkGreen, "PassMark Score:%d", c.m_Score);yp += LineHeight;
							yp += LineHeight;
						}
						if (m_CheckPCSpec.GetCPUDatas()->size() == 0) {
							WindowSystem::SetMsg(xp, yp, xsize - y_r(16), yp + LineHeight, LineHeight, FontHandle::FontXCenter::LEFT, Red, DarkGreen, LocalizeParts->Get(2005));yp += LineHeight;
						}
						//Mem
						{
							WindowSystem::SetMsg(xp, yp, xsize - y_r(16), yp + LineHeight, LineHeight, FontHandle::FontXCenter::LEFT, White, DarkGreen, LocalizeParts->Get(2011));yp += LineHeight;
							WindowSystem::SetMsg(xp + y_r(16), yp, xsize - y_r(16), yp + LineHeight, LineHeight, FontHandle::FontXCenter::LEFT, White, DarkGreen, "[%4.3lfMB / %4.3lfMB]", m_CheckPCSpec.GetFreeMemorySize(), m_CheckPCSpec.GetTotalMemorySize());
							int TextID = 0;
							unsigned int Color = White;
							if ((m_CheckPCSpec.GetTotalMemorySize() - m_CheckPCSpec.GetFreeMemorySize()) >= 2000) {//
								Color = Green;
								TextID = 2012;
							}
							else {//
								Color = Yellow;
								TextID = 2013;
							}
							if (IntoMouse(xp + y_r(16), yp, xsize - y_r(16), yp + LineHeight * 1)) {
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
							WindowSystem::SetMsg(xp + y_r(16), yp, xsize - y_r(16), yp + LineHeight, LineHeight * 2 / 3, FontHandle::FontXCenter::RIGHT, Color, DarkGreen, "%s", LocalizeParts->Get(TextID));yp += LineHeight;
							yp += LineHeight;
						}
						//GPU
						WindowSystem::SetMsg(xp, yp, xsize - y_r(16), yp + LineHeight, LineHeight, FontHandle::FontXCenter::LEFT, White, DarkGreen, LocalizeParts->Get(2021));yp += LineHeight;
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
							if (IntoMouse(xp + y_r(16), yp, xsize - y_r(16), yp + LineHeight * 2)) {
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
							WindowSystem::SetMsg(xp + y_r(16), yp, xsize - y_r(16), yp + LineHeight, LineHeight * 3 / 4, FontHandle::FontXCenter::LEFT, White, DarkGreen, "%s", c.m_Name.c_str());
							WindowSystem::SetMsg(xp + y_r(16), yp, xsize - y_r(16), yp + LineHeight, LineHeight * 2 / 3, FontHandle::FontXCenter::RIGHT, Color, DarkGreen, "%s", LocalizeParts->Get(TextID));yp += LineHeight;
							WindowSystem::SetMsg(xp + y_r(16), yp, xsize - y_r(16), yp + LineHeight, LineHeight, FontHandle::FontXCenter::RIGHT, White, DarkGreen, "PassMark Score:%d", c.m_Score);yp += LineHeight;
							yp += LineHeight;
						}
						if (m_CheckPCSpec.GetGPUDatas()->size() == 0) {
							WindowSystem::SetMsg(xp, yp, xsize - y_r(16), yp + LineHeight, LineHeight, FontHandle::FontXCenter::LEFT, Red, DarkGreen, LocalizeParts->Get(2025));yp += LineHeight;
						}
						//DirectX
						int NowSet = OptionParts->GetParamInt(EnumSaveParam::DirectXVer);
						for (int loop = 0;loop < 2;loop++) {
							if (GetUseDirect3DVersion() == DirectXVerID[loop]) {
								NowSet = loop;
							}
						}
						if (IntoMouse(xp + y_r(16), yp, xsize - y_r(16), yp + LineHeight * 2)) {
							MouseOverID = 2048;
						}
						WindowSystem::SetMsg(xp, yp, xsize - y_r(16), yp + LineHeight, LineHeight, FontHandle::FontXCenter::LEFT, White, DarkGreen, LocalizeParts->Get(2035));
						WindowSystem::SetMsg(xp, yp, xsize - y_r(16), yp + LineHeight, LineHeight, FontHandle::FontXCenter::RIGHT, White, DarkGreen, "DirectX%s", DirectXVerStr[NowSet]);yp += LineHeight;
						if (MouseOverID > 0) {
							xp = Pad->GetMS_X();
							yp = Pad->GetMS_Y();
							WindowSystem::SetMsg(xp, yp - LineHeight, xp, yp, LineHeight, FontHandle::FontXCenter::RIGHT, Green, DarkGreen, LocalizeParts->Get(MouseOverID));
						}
					}

					xp = y_r(720 + 16 + 32);
					yp = y_r(720);
					if (WindowSystem::SetMsgClickBox(xp, yp, xsize-y_r(32), yp + y_r(32), Green, "Start Game!")) {
						PopUpParts->EndAll();
					}
				}
				GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK, true);
				{
					UI_Screen.DrawExtendGraph(0, 0, xopt, yopt, false);
				}
				Screen_Flip();
			}
			OptionParts->Save();
			StartMe();
		}
		else {
			PostPassEffect::Create();						//シェーダー
			if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
				UI_Screen = GraphHandle::Make(y_r(1920), y_r(1080), true);	//UI
			}
		}
		m_IsCubeMap = (OptionParts->GetParamInt(EnumSaveParam::Reflection) > 0);
		if (m_IsCubeMap) {
			m_RealTimeCubeMap.Init();
		}
	}
	bool			DXDraw::FirstExecute(void) noexcept {
		auto* Pad = PadControl::Instance();
		auto* PopUpParts = PopUp::Instance();
		auto* OptionWindowParts = OptionWindowClass::Instance();
		auto* LocalizeParts = LocalizePool::Instance();
		auto* PostPassParts = PostPassEffect::Instance();
		if (!(ProcessMessage() == 0)) { return false; }

		m_FPS = std::max(GetFPS(), 30.f);
		m_StartTime = GetNowHiPerformanceCount();
		if (Pad->GetEsc().trigger() && !m_IsExitSelect) {
			m_IsExitSelect = true;
			PopUpParts->Add(LocalizeParts->Get(100), y_UI(480), y_UI(240),
				[&](int xmin, int ymin, int xmax, int ymax, bool) {
					auto* LocalizeParts = LocalizePool::Instance();
					int xp1, yp1;
					//タイトル
					{
						xp1 = xmin + y_UI(24);
						yp1 = ymin + LineHeight;

						WindowSystem::SetMsgWW(xp1, yp1, xp1, yp1 + LineHeight, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, LocalizeParts->Get(101));
					}
					//
					{
						xp1 = (xmax + xmin) / 2 - y_UI(54);
						yp1 = ymax - LineHeight * 3;

						auto* Pad = PadControl::Instance();
						bool ret = WindowSystem::SetMsgClickBox(xp1, yp1, xp1 + y_UI(108), yp1 + LineHeight * 2, Gray15, LocalizeParts->Get(102));
						if (Pad->GetKey(PADS::INTERACT).trigger() || ret) {
							m_IsEnd = true;
						}
					}
				},
				[&]() {m_IsExitSelect = false; },
				[&]() {},
				true
			);
		}
		if (OptionWindowParts->IsRestartSwitch() && !m_IsRestartSelect) {
			m_IsRestartSelect = true;
			PopUpParts->Add(LocalizeParts->Get(100), y_UI(480), y_UI(240),
				[&](int xmin, int ymin, int xmax, int ymax, bool) {
					auto* LocalizeParts = LocalizePool::Instance();
					int xp1, yp1;
					//タイトル
					{
						xp1 = xmin + y_UI(24);
						yp1 = ymin + LineHeight;

						WindowSystem::SetMsgWW(xp1, yp1, xp1, yp1 + LineHeight, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, LocalizeParts->Get(2101));
					}
					//
					{
						xp1 = (xmax + xmin) / 2 - y_UI(54);
						yp1 = ymax - LineHeight * 3;

						auto* Pad = PadControl::Instance();
						bool ret = WindowSystem::SetMsgClickBox(xp1, yp1, xp1 + y_UI(108), yp1 + LineHeight * 2, Gray15, LocalizeParts->Get(2102));
						if (Pad->GetKey(PADS::INTERACT).trigger() || ret) {
							m_IsEnd = true;
							StartMe();
						}
					}
				},
				[&]() {m_IsRestartSelect = false; },
				[&]() {},
				true
			);
		}
		PostPassParts->Update();
		return !m_IsEnd;
	}
	void			DXDraw::Execute(void) noexcept {
		auto* OptionParts = OPTION::Instance();
		{
			bool Now = (OptionParts->GetParamInt(EnumSaveParam::Reflection) > 0);
			if (Now != m_IsCubeMap) {
				m_IsCubeMap = Now;
				if (m_IsCubeMap) {
					m_RealTimeCubeMap.Init();
				}
				else {
					m_RealTimeCubeMap.Dispose();
				}
			}
		}
		auto* Pad = PadControl::Instance();
		auto* OptionWindowParts = OptionWindowClass::Instance();
		Set3DSoundListenerPosAndFrontPosAndUpVec(SetMainCamera().GetCamPos().get(), SetMainCamera().GetCamVec().get(), SetMainCamera().GetCamUp().get());		//音位置指定
		this->GetVRControl()->Execute();		//VR空間に適用
		//
		m_PauseFlashCount += 1.f / GetFps();
		if (m_PauseFlashCount > 1.f) { m_PauseFlashCount -= 1.f; }
		//
		m_PauseActive.Execute(Pad->GetKey(PADS::INVENTORY).press() && !OptionWindowParts->IsActive());
		if (m_PauseActive.trigger()) {
			Pad->SetGuideUpdate();
		}
		//
		if (!IsPause() && ((m_StartTime - Update_effect_was) >= 1000000 / 60)) {
			UpdateEffekseer3D();
			Update_effect_was = m_StartTime;
		}
		if (m_SendCamShakeTime > 0.f) {
			if (m_SendCamShake) {
				m_SendCamShake = false;
				this->m_CamShake = m_SendCamShakeTime;
			}
			auto RandRange = this->m_CamShake / m_SendCamShakeTime * m_SendCamShakePower;
			Easing(&this->m_CamShake1, Vector3DX::vget(GetRandf(RandRange), GetRandf(RandRange), GetRandf(RandRange)), 0.8f, EasingType::OutExpo);
			Easing(&this->m_CamShake2, this->m_CamShake1, 0.8f, EasingType::OutExpo);
			this->m_CamShake = std::max(this->m_CamShake - 1.f / GetFps(), 0.f);
		}
	}
	void			DXDraw::Draw(
		std::function<void()> sky_doing,
		std::function<void()> doing,
		std::function<void()> doingFront,
		std::function<void()> doingUI,
		std::function<void()> doingUI2
	) noexcept {
		auto* OptionParts = OPTION::Instance();
		auto* PostPassParts = PostPassEffect::Instance();
		auto* Fonts = FontPool::Instance();
		auto* Pad = PadControl::Instance();
		//描画
		auto MainDraw = [&](const Camera3DInfo& camInfo) {
			//影画像の用意
			if (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0) {
				m_ShadowDraw.SetDraw(doing, camInfo);
			}
			UI_Screen.SetDraw_Screen(camInfo, false);
			{
				SetCameraNearFar(camInfo.GetCamNear(), camInfo.GetCamFar());
				SetupCamera_Perspective(camInfo.GetCamFov());
				SetCameraPositionAndTargetAndUpVec(camInfo.GetCamPos().get(), camInfo.GetCamVec().get(), camInfo.GetCamUp().get());

				m_CamViewMatrix = GetCameraViewMatrix();
				m_CamProjectionMatrix = GetCameraProjectionMatrix();
			}
			PostPassParts->DrawDoF(sky_doing, [&]() { m_PBR_Shader.Draw_lamda(doing); }, doingFront, camInfo);
			//ソフトシャドウ重ね
			if (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0) {
				PostPassParts->Plus_Draw([&]() { m_ShadowDraw.Draw(); });
			}
			PostPassParts->Draw();
			//完成した画面に対して後処理の2Dシェーダーを反映
			if (this->m_ShaderParam[0].use) {
				//レンズ
				PostPassParts->Plus_Draw([&]() {
					this->m_Shader2D[0].SetPixelDispSize(y_r(1920), y_r(1080));
					this->m_Shader2D[0].SetPixelParam(3, this->m_ShaderParam[0].param[0], this->m_ShaderParam[0].param[1], this->m_ShaderParam[0].param[2], this->m_ShaderParam[0].param[3]);
					SetUseTextureToShader(0, PostPassParts->Get_MAIN_Screen().get());	//使用するテクスチャをセット
					this->m_Shader2D[0].Draw(this->m_ScreenVertex);
					SetUseTextureToShader(0, -1);
					});
			}
			if (this->m_ShaderParam[1].use) {
				//ブラックアウト
				PostPassParts->Plus_Draw([&]() {
					this->m_Shader2D[1].SetPixelDispSize(y_r(1920), y_r(1080));
					this->m_Shader2D[1].SetPixelParam(3, this->m_ShaderParam[1].param[0], 0, 0, 0);
					SetUseTextureToShader(0, PostPassParts->Get_MAIN_Screen().get());	//使用するテクスチャをセット
					this->m_Shader2D[1].Draw(this->m_ScreenVertex);
					SetUseTextureToShader(0, -1);
					});
			}
			};
		auto PauseDraw = [&]() {
			if (IsPause()) {
				//
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f * 0.5f), 0, 255));
				DrawBox_2D(0, 0, y_UI(1920.f), y_UI(1080.f), Black, TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				//
				if (m_PauseFlashCount > 0.5f) {
					Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_UI(36.f), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, y_UI(16.f), y_UI(16.f), Green, Black, "Pause");
				}
			}
			};
		if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
			//UIをスクリーンに描画しておく
			UI_Screen.SetDraw_Screen();
			{
				doingUI();
				PauseDraw();
				UISystem::Instance()->Draw();
				Pad->Draw();
			}
			//VRに移す
			for (char i = 0; i < 2; i++) {
				MainDraw(this->GetVRControl()->GetCamPos(i));
				//それぞれの目に内容を送信
				this->GetVRControl()->SubmitDraw(i, PostPassParts->Get_MAIN_Screen(), UI_Screen, doingUI2);
			}
			//ディスプレイ描画
			GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
			{
				DrawBox_2D(0, 0, y_r(1920), y_r(1080), White, TRUE);
				if (this->GetVRControl()->GetOutBuffer()) {
					this->GetVRControl()->GetOutBuffer()->DrawRotaGraph(y_r(1920) / 2, y_r(1080) / 2, 0.5f, 0, false);
				}
			}
		}
		else {
			MainDraw(GetMainCamera());
			//ディスプレイ描画
			GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK, true);
			{
				auto Prev = GetDrawMode();
				//SetDrawMode(DX_DRAWMODE_NEAREST);
				SetDrawMode(DX_DRAWMODE_BILINEAR);
				PostPassParts->Get_MAIN_Screen().DrawExtendGraph(0, 0, this->GetDispXSize(), this->GetDispYSize(), false);

				doingUI();
				PauseDraw();
				doingUI2();										//UI2
				UISystem::Instance()->Draw();
				Pad->Draw();
				SetDrawMode(Prev);
			}
		}
	}
	bool					DXDraw::Screen_Flip(void) noexcept {
		auto* OptionParts = OPTION::Instance();
		ScreenFlip();
		if (!OptionParts->GetParamBoolean(EnumSaveParam::vsync)) {
			//4msだけスリープ
			while ((GetNowHiPerformanceCount() - m_StartTime) < 1000 * 1000 / OptionParts->GetParamInt(EnumSaveParam::FpsLimit) - 1000 * 4) {
				if (ProcessMessage() != 0) { return false; }
				SleepThread(1);	// 1msecスリープする
			}
			while ((GetNowHiPerformanceCount() - m_StartTime) < 1000 * 1000 / OptionParts->GetParamInt(EnumSaveParam::FpsLimit)) {}
		}
		else {
			WaitVSync(1);
		}
		this->GetVRControl()->WaitSync();
		return true;
	}
	//VR
	void					DXDraw::Get_VR_HMDPositionVR(Vector3DX* pos_, Matrix4x4DX*mat) noexcept { this->GetVRControl()->GetHMDPosition(pos_, mat); }
	void					DXDraw::Reset_VR_HMD(void) noexcept { this->GetVRControl()->ResetHMD(); }
	bool					DXDraw::Get_VR_Hand1PadPress(VR_PAD ID) const noexcept { return this->GetVRControl()->Get_VR_Hand1Device() ? this->GetVRControl()->Get_VR_Hand1Device()->PadPress(ID) : false; }
	bool					DXDraw::Get_VR_Hand1TouchPress(VR_PAD ID) const noexcept { return this->GetVRControl()->Get_VR_Hand1Device() ? this->GetVRControl()->Get_VR_Hand1Device()->PadTouch(ID) : false; }
	Vector3DX				DXDraw::Get_VR_Hand1TouchPadPoint() const noexcept { return this->GetVRControl()->Get_VR_Hand1Device() ? this->GetVRControl()->Get_VR_Hand1Device()->GetTouchPadPoint() : Vector3DX::zero(); }
	bool					DXDraw::Get_VR_Hand2PadPress(VR_PAD ID) const noexcept { return this->GetVRControl()->Get_VR_Hand2Device() ? this->GetVRControl()->Get_VR_Hand2Device()->PadPress(ID) : false; }
	bool					DXDraw::Get_VR_Hand2TouchPress(VR_PAD ID) const noexcept { return this->GetVRControl()->Get_VR_Hand2Device() ? this->GetVRControl()->Get_VR_Hand2Device()->PadTouch(ID) : false; }
	Vector3DX				DXDraw::Get_VR_Hand2TouchPadPoint() const noexcept { return this->GetVRControl()->Get_VR_Hand2Device() ? this->GetVRControl()->Get_VR_Hand2Device()->GetTouchPadPoint() : Vector3DX::zero(); }

	void					DXDraw::VR_Haptic(char id_, unsigned short times) noexcept { this->GetVRControl()->Haptic(id_, times); }
	//
	bool					DXDraw::UpdateShadowActive() noexcept {
		auto* OptionParts = OPTION::Instance();
		bool shadow = OptionParts->GetParamInt(EnumSaveParam::shadow) > 0;
		if (m_PrevShadow != shadow) {
			m_PrevShadow = shadow;
			if (shadow) {
				m_ShadowDraw.Init(11, y_r(1920), y_r(1080));
				return true;
			}
			else {
				m_ShadowDraw.Dispose();
			}
		}
		return false;
	}

	void			DXDraw::Update_Shadow(std::function<void()> doing, const Vector3DX& CenterPos, bool IsFar) noexcept {
		auto* OptionParts = OPTION::Instance();
		if (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0) {
			// 影用の深度記録画像の準備を行う
			if (!IsFar) {
				m_ShadowDraw.Update(doing, CenterPos);
			}
			else {
				m_ShadowDraw.UpdateFar(doing, CenterPos);
			}
		}
	}

	void			DXDraw::Update_CubeMap(std::function<void()> doing, const Vector3DX& CenterPos) noexcept {
		auto* OptionParts = OPTION::Instance();
		if (OptionParts->GetParamInt(EnumSaveParam::Reflection) > 0) {
			m_RealTimeCubeMap.ReadyDraw(CenterPos, doing);
		}
	}
	void			DXDraw::SetWindowOrBorderless() noexcept {
		auto* OptionParts = OPTION::Instance();
		if (!OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
			if (OptionParts->GetParamBoolean(EnumSaveParam::AllWaysFront)) {
				this->m_DispXSize = this->m_DispXSize_Win;
				this->m_DispYSize = this->m_DispYSize_Win;
				SetWindowStyleMode(0);
				SetWindowPosition(0, 0);
				SetWindowSize(this->m_DispXSize, this->m_DispYSize);
			}
			else {
				this->m_DispXSize = this->m_DispXSize_Border;
				this->m_DispYSize = this->m_DispYSize_Border;
				SetWindowStyleMode(2);
				SetWindowPosition(0, 0);
				SetWindowSize(this->m_DispXSize, this->m_DispYSize);
			}
		}
	}
};
