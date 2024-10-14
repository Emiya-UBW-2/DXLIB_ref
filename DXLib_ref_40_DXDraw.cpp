#include "DXLib_ref_40_DXDraw.hpp"

namespace DXLibRef {
	const DXDraw* SingletonBase<DXDraw>::m_Singleton = nullptr;
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
#ifdef _USE_OPENVR_
	class DXDraw::VRControl {
		class VRDeviceClass {
		private:
			int							m_ID{ 0 };
			char						m_DeviceNumber{ 0 };
			vr::ETrackedDeviceClass 	m_type{ vr::TrackedDeviceClass_Invalid };
			bool						m_isInitialized{ false };
			bool						m_isActive{ false };
			//位置やボタン判定
			uint64_t					m_ButtonPressFlag{ 0 };
			uint64_t					m_ButtonTouchFlag{ 0 };
			Vector3DX					m_TouchPadPoint{ Vector3DX::zero() };
			moves						m_move;
		public:
			const auto&			GetID(void) const noexcept { return m_ID; }
			auto			IsActive(void) const noexcept { return m_isInitialized && m_isActive; }
			bool			PadPress(VR_PAD ID) const noexcept {
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
			bool			PadTouch(VR_PAD ID) const noexcept {
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
			const auto&			GetTouchPadPoint(void) const noexcept { return m_TouchPadPoint; }
			const auto&			GetMove(void) const noexcept { return m_move; }
		public:
			void Init(int ID, char Num, vr::ETrackedDeviceClass Type) noexcept {
				this->m_ID = ID;
				this->m_DeviceNumber = Num;
				this->m_type = Type;
				this->m_isInitialized = true;
				this->m_isActive = false;
				Reset();
			}
			void Update(vr::IVRSystem* SystemPtr) noexcept {
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
						Vector3DX::vget(tmp.mDeviceToAbsoluteTracking.m[0][2], tmp.mDeviceToAbsoluteTracking.m[1][2], -tmp.mDeviceToAbsoluteTracking.m[2][2]) * -1.f
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
						Vector3DX::vget(tmp.mDeviceToAbsoluteTracking.m[0][2], tmp.mDeviceToAbsoluteTracking.m[1][2], -tmp.mDeviceToAbsoluteTracking.m[2][2]) * -1.f
					);
				}
				break;
				default:
					break;
				}
			}
			void Reset(void) noexcept {
				this->m_ButtonPressFlag = 0;
				this->m_ButtonTouchFlag = 0;
				this->m_TouchPadPoint = Vector3DX::zero();
				this->m_move.pos = Vector3DX::zero();
				this->m_move.mat = Matrix4x4DX::identity();
			}
		};
	private:
		vr::IVRSystem* m_VR_SystemPtr{ nullptr };
		vr::EVRInitError			m_VR_ErrorHandle{ vr::VRInitError_None };
		std::vector<VRDeviceClass>	m_VR_DeviceInfo;
		char						m_VR_HMDID{ INVALID_ID };
		char						m_VR_Hand1ID{ INVALID_ID };
		char						m_VR_Hand2ID{ INVALID_ID };
		std::vector<char>			m_VR_TrackerID;
		bool						m_VR_PrevHMDIsActive{ false };
		bool						m_VR_HMD_StartFlag{ true };
		Matrix4x4DX					m_VR_HMD_StartPoint;
		//
		GraphHandle					m_OutScreen;							//スクリーンバッファ
	public:
		VRControl(void) noexcept {}
		~VRControl(void) noexcept {}
	public:
		const VRDeviceClass* Get_VR_Hand1Device(void) const noexcept { return (m_VR_Hand1ID >= 0) ? &m_VR_DeviceInfo.at(m_VR_Hand1ID) : nullptr; }
		const VRDeviceClass* Get_VR_Hand2Device(void) const noexcept { return (m_VR_Hand2ID >= 0) ? &m_VR_DeviceInfo.at(m_VR_Hand2ID) : nullptr; }
		const VRDeviceClass* GetTrackerDevice(int sel) const noexcept { return (0 <= sel && sel < m_VR_TrackerID.size()) ? &m_VR_DeviceInfo.at(m_VR_TrackerID.at(sel)) : nullptr; }
		void			ResetHMD(void) noexcept {
			m_VR_PrevHMDIsActive = false;
			m_VR_HMD_StartFlag = true;
		}
		void			GetHMDPosition(Vector3DX* pos_, Matrix4x4DX* mat) noexcept {
			auto* HMDPtr = (m_VR_HMDID >= 0) ? &m_VR_DeviceInfo.at(m_VR_HMDID) : nullptr;
			if (HMDPtr) {
				*mat = HMDPtr->GetMove().mat;
				*mat = Matrix4x4DX::Axis1((*mat).yvec(), (*mat).zvec() * -1.f);
				if (!HMDPtr->IsActive()) {
					m_VR_HMD_StartFlag = true;
				}
				if (m_VR_HMD_StartFlag && HMDPtr->IsActive() != m_VR_PrevHMDIsActive) {
					m_VR_HMD_StartFlag = false;
					//
					Vector3DX pos = HMDPtr->GetMove().pos; pos.y = 0.f;
					Vector3DX tmp = mat->zvec(); tmp.y = 0.f; tmp = tmp.normalized();
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
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->GetParamBoolean(EnumSaveParam::usevr) && (id_ != INVALID_ID) && m_VR_SystemPtr) {
				m_VR_SystemPtr->TriggerHapticPulse(m_VR_DeviceInfo[id_].GetID(), 2, times);
			}
		}
		auto			GetEyePosition(char eye_type) noexcept {
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
				auto* HMDPtr = (m_VR_HMDID >= 0) ? &m_VR_DeviceInfo.at(m_VR_HMDID) : nullptr;
				if (HMDPtr) {
					const vr::HmdMatrix34_t tmpmat = vr::VRSystem()->GetEyeToHeadTransform((vr::EVREye)eye_type);
					return Matrix4x4DX::Vtrans(Vector3DX::vget(tmpmat.m[0][3], tmpmat.m[1][3], tmpmat.m[2][3]), HMDPtr->GetMove().mat);
				}
			}
			return Vector3DX::zero();
		}
		const GraphHandle* GetOutBuffer(void) const noexcept { return &m_OutScreen; }
		auto			GetCamPos(char eye_type) noexcept {
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
		void Init(void) noexcept {
			auto* OptionParts = OPTION::Instance();
			//VRが使えるかチェック
			if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
				m_VR_ErrorHandle = vr::VRInitError_None;
				m_VR_SystemPtr = vr::VR_Init(&m_VR_ErrorHandle, vr::VRApplication_Scene);
				if (m_VR_ErrorHandle != vr::VRInitError_None) {
					m_VR_SystemPtr = nullptr;
					OptionParts->SetParamBoolean(EnumSaveParam::usevr, false);
				}
			}
			//デバイスセット
			if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
				m_VR_TrackerID.clear();
				char DeviceCount = 0;
				bool IsFirstHand = true;
				for (char i = 0; i < 8; ++i) {
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
		void SetupBuffer(int xsize, int ysize) noexcept {
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
				//画面セット
				m_OutScreen = GraphHandle::Make(xsize, ysize);	//左目
			}
		}
		void Execute(void) noexcept {
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
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
			vr::Texture_t tex = { (void*)GetUseDirect3D11BackBufferTexture2D(), vr::ETextureType::TextureType_DirectX,vr::EColorSpace::ColorSpace_Auto };
			vr::VRCompositor()->Submit((vr::EVREye)eye_type, &tex, NULL, vr::Submit_Default);
		}
		void SubmitDraw(char eye_type, const GraphHandle& MainDrawScreen, const GraphHandle& UIScreen, std::function<void()> doingUI2) noexcept {
			Camera3DInfo tmp_cam = GetCamPos(eye_type);
			//目線用に合成
			m_OutScreen.SetDraw_Screen();
			tmp_cam.FlipCamInfo();
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
			GraphHandle::SetDraw_Screen(static_cast<int>(DX_SCREEN_BACK));
			{
				m_OutScreen.DrawGraph(0, 0, false);
				doingUI2();
			}
			//それぞれの目にDX_SCREEN_BACKの内容を送信
			this->Submit(eye_type);
		}
		void WaitSync(void) noexcept {
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
				vr::TrackedDevicePose_t tmp;
				vr::VRCompositor()->WaitGetPoses(&tmp, 1, NULL, 1);
			}
		}
		void Dispose(void) noexcept {
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
				//vr::VR_Shutdown();
				m_VR_SystemPtr = nullptr;
			}
		}
	};
#else
	class DXDraw::VRControl {
		class VRDeviceClass {
		public:
			bool			PadPress(VR_PAD) const noexcept { return false; }
			bool			PadTouch(VR_PAD) const noexcept { return false; }
			auto			GetTouchPadPoint(void) const noexcept { return Vector3DX::zero(); }
		};
	public:
		VRControl(void) noexcept {}
		~VRControl(void) noexcept {}
	public:
		const VRDeviceClass* Get_VR_Hand1Device(void) const noexcept { return nullptr; }
		const VRDeviceClass* Get_VR_Hand2Device(void) const noexcept { return nullptr; }
		const VRDeviceClass* GetTrackerDevice(int) const noexcept { return nullptr; }
		void			ResetHMD(void) noexcept {}
		void			GetHMDPosition(Vector3DX*, Matrix4x4DX*) noexcept {}
		void			Haptic(char, unsigned short) noexcept {}
		auto			GetEyePosition(char) noexcept { return Vector3DX::zero(); }
		const GraphHandle* GetOutBuffer(void) const noexcept { return nullptr; }
		auto			GetCamPos(char eye_type) noexcept {
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
		void Init(void) noexcept {}
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
	void DXDraw::ShadowDraw::Init(int ShadowMapSize, int dispsizex, int dispsizey) noexcept {
		BaseShadowHandle = GraphHandle::Make(dispsizex, dispsizey, TRUE);
		int size = 2 << ShadowMapSize;
		DepthBaseScreenHandle = GraphHandle::Make(size, size, FALSE);			// 深度バッファ用の作成
		DepthScreenHandle = GraphHandle::MakeDepth(size, size);					// 深度バッファの作成
		DepthFarScreenHandle = GraphHandle::MakeDepth(size, size);				// 深度バッファの作成
		m_Shader.Init("CommonData/shader/VS_SoftShadow.vso", "CommonData/shader/PS_SoftShadow.pso");
		m_ShaderRigid.Init("CommonData/shader/VS_SoftShadow_Rigid.vso", "CommonData/shader/PS_SoftShadow.pso");
	}
	void DXDraw::ShadowDraw::SetupCam(Vector3DX Center, float scale) const noexcept {
		ClearDrawScreen();
		SetupCamera_Ortho(30.f * scale * Scale_Rate);		// カメラのタイプを正射影タイプにセット、描画範囲も指定
		SetCameraNearFar(0.05f * scale * Scale_Rate, 60.f * scale * Scale_Rate);		// 描画する奥行き範囲をセット
		// カメラの位置と注視点はステージ全体が見渡せる位置
		auto Vec = m_ShadowVec;
		if (m_ShadowVec.x == 0.f && m_ShadowVec.z == 0.f) {
			Vec.z = (0.1f);
		}
		SetCameraPositionAndTarget_UpVecY((Center - Vec.normalized() * (30.f * scale * Scale_Rate)).get(), Center.get());
	}
	void DXDraw::ShadowDraw::Update(std::function<void()> Shadowdoing, Vector3DX Center, float Scale) noexcept {
		m_Scale = Scale;
		// 影用の深度記録画像の準備を行う
		SetRenderTargetToShader(0, DepthBaseScreenHandle.get());
		SetRenderTargetToShader(1, INVALID_ID);
		SetRenderTargetToShader(2, DepthScreenHandle.get());
		{
			SetupCam(Center, m_Scale);
			m_CamViewMatrix[0] = GetCameraViewMatrix();
			m_CamProjectionMatrix[0] = GetCameraProjectionMatrix();
			Shadowdoing();
		}
		SetRenderTargetToShader(0, INVALID_ID);
		SetRenderTargetToShader(1, INVALID_ID);
		SetRenderTargetToShader(2, INVALID_ID);
	}
	void DXDraw::ShadowDraw::UpdateFar(std::function<void()> Shadowdoing, Vector3DX Center, float Scale) noexcept {
		m_ScaleFar = Scale;
		// 影用の深度記録画像の準備を行う
		SetRenderTargetToShader(0, DepthBaseScreenHandle.get());
		SetRenderTargetToShader(1, INVALID_ID);
		SetRenderTargetToShader(2, DepthFarScreenHandle.get());
		{
			SetupCam(Center, m_ScaleFar);
			m_CamViewMatrix[1] = GetCameraViewMatrix();
			m_CamProjectionMatrix[1] = GetCameraProjectionMatrix();
			Shadowdoing();
		}
		SetRenderTargetToShader(0, INVALID_ID);
		SetRenderTargetToShader(1, INVALID_ID);
		SetRenderTargetToShader(2, INVALID_ID);
	}
	void DXDraw::ShadowDraw::SetDraw(std::function<void()> doing_rigid, std::function<void()> doing, Camera3DInfo tmp_cam) noexcept {
		auto* OptionParts = OPTION::Instance();
		SetUseTextureToShader(0, BaseShadowHandle.get());				// 影用深度記録画像をテクスチャ１にセット
		SetUseTextureToShader(1, DepthScreenHandle.get());				// 影用深度記録画像をテクスチャ１にセット
		SetUseTextureToShader(2, DepthFarScreenHandle.get());			// 影用深度記録画像をテクスチャ１にセット
		// 影の結果を出力
		tmp_cam.SetCamInfo(tmp_cam.GetCamFov(), 0.1f * Scale_Rate, 100.f * Scale_Rate);
		BaseShadowHandle.SetDraw_Screen();
		tmp_cam.FlipCamInfo();
		{
			m_Shader.SetPixelParam(3, static_cast<float>(OptionParts->GetParamInt(EnumSaveParam::shadow) * 3 / 2), m_Scale * 150.f, m_ScaleFar * 150.f, 0.f);
			m_Shader.SetVertexCameraMatrix(4, m_CamViewMatrix[0], m_CamProjectionMatrix[0]);
			m_Shader.SetVertexCameraMatrix(5, m_CamViewMatrix[1], m_CamProjectionMatrix[1]);
			m_Shader.Draw_lamda(doing);
			m_ShaderRigid.SetPixelParam(3, static_cast<float>(OptionParts->GetParamInt(EnumSaveParam::shadow) * 3 / 2), m_Scale * 150.f, m_ScaleFar * 150.f, 0.f);
			m_ShaderRigid.SetVertexCameraMatrix(4, m_CamViewMatrix[0], m_CamProjectionMatrix[0]);
			m_ShaderRigid.SetVertexCameraMatrix(5, m_CamViewMatrix[1], m_CamProjectionMatrix[1]);
			m_ShaderRigid.Draw_lamda(doing_rigid);
		}
		SetUseTextureToShader(1, INVALID_ID);				// 使用テクスチャの設定を解除
		SetUseTextureToShader(2, INVALID_ID);				// 使用テクスチャの設定を解除
		//後処理
		GraphBlend(BaseShadowHandle.get(), DepthBaseScreenHandle.get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
			DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_R);
	}
	void DXDraw::ShadowDraw::Draw(void) noexcept {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
		BaseShadowHandle.DrawGraph(0, 0, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		//DepthScreenHandle.DrawExtendGraph(0, 0,1080,1080, true);
	}
	void DXDraw::ShadowDraw::Dispose(void) noexcept {
		BaseShadowHandle.Dispose();
		DepthBaseScreenHandle.Dispose();
		DepthScreenHandle.Dispose();
		DepthFarScreenHandle.Dispose();
		m_Shader.Dispose();
		m_ShaderRigid.Dispose();
	}
	void DXDraw::ShadowDraw::SetActive(void) noexcept {
		auto* OptionParts = OPTION::Instance();
		m_PrevShadow = OptionParts->GetParamInt(EnumSaveParam::shadow) > 0;
		Init(11, ScreenWidth, ScreenHeight);
	}
	bool DXDraw::ShadowDraw::UpdateActive(void) noexcept {
		auto* OptionParts = OPTION::Instance();
		bool shadow = OptionParts->GetParamInt(EnumSaveParam::shadow) > 0;
		if (m_PrevShadow != shadow) {
			m_PrevShadow = shadow;
			if (shadow) {
				Init(11, ScreenWidth, ScreenHeight);
				return true;
			}
			else {
				Dispose();
			}
		}
		return false;
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
		m_IsFirstBoot = !SaveDataParts->Load();
		if (m_IsFirstBoot) {
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
				auto hdc = GetDC(nullptr);         // カレントのスクリーン全体のデバイスコンテキスト取得.
				DPI = GetDeviceCaps(hdc, LOGPIXELSY);
			}
			this->m_DispXSize = this->m_DispXSize * DPI / 96;
			this->m_DispYSize = this->m_DispYSize * DPI / 96;
			//
		}
		this->GetVRControl()->SetupBuffer(this->m_DispXSize, this->m_DispYSize);
		int DXVer = DirectXVerID[OptionParts->GetParamInt(EnumSaveParam::DirectXVer)];
		SetOutApplicationLogValidFlag(TRUE);						//log
		SetMainWindowText("Loading...");							//タイトル
		ChangeWindowMode(TRUE);										//窓表示
		SetUseDirect3DVersion(DXVer);								//directX ver
		SetUseDirectInputFlag(TRUE);								//
		SetDirectInputMouseMode(TRUE);								//
		SetGraphMode(this->m_DispXSize, this->m_DispYSize, 32);		//解像度
		SetWindowSizeChangeEnableFlag(FALSE, FALSE);				//ウインドウサイズを手動不可、ウインドウサイズに合わせて拡大もしないようにする
		Set3DSoundOneMetre(1.0f);									//
		//垂直同期
		if (DXVer != DX_DIRECT3D_11) {
			SetWaitVSyncFlag(FALSE);									
		}
		else {
			SetWaitVSyncFlag(OptionParts->GetParamBoolean(EnumSaveParam::vsync));
		}
		SetZBufferBitDepth(32);										//デフォのZバッファ精度を32bitに
		DxLib_Init();												//初期化
		SetChangeScreenModeGraphicsSystemResetFlag(FALSE);			//画面モード変更時( とウインドウモード変更時 )にリセットを走らせない
		SetUsePixelLighting(TRUE);									//ピクセルライティングの使用
		if (GetUseDirect3DVersion() != DXVer) {
			MessageBox(NULL, LocalizeParts->Get(10), "", MB_OK);
		}
		SetSysCommandOffFlag(TRUE);									//
		if (!m_IsFirstBoot) {
#ifdef _USE_EFFEKSEER_
			Effekseer_Init(8000);										//Effekseer
			Effekseer_SetGraphicsDeviceLostCallbackFunctions();			//Effekseer
#endif
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
		m_DispXSize_Max = std::min(m_DispXSize_Border, static_cast<int>(static_cast<float>(std::min<int>(basex, this->m_DispXSize)) * std::clamp(OptionParts->GetParamFloat(EnumSaveParam::DrawScale), 0.25f, 10.f)));
		m_DispYSize_Max = std::min(m_DispYSize_Border, static_cast<int>(static_cast<float>(std::min<int>(basey, this->m_DispYSize)) * std::clamp(OptionParts->GetParamFloat(EnumSaveParam::DrawScale), 0.25f, 10.f)));
		SetWindowOrBorderless();
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
		UISystem::Create();
		CameraShake::Create();

		auto* SE = SoundPool::Instance();
		SE->Add(static_cast<int>(SoundEnumCommon::UI_Select), 2, "CommonData/Sound/UI/cursor.wav", false);
		SE->Add(static_cast<int>(SoundEnumCommon::UI_CANCEL), 1, "CommonData/Sound/UI/cancel.wav", false);
		SE->Add(static_cast<int>(SoundEnumCommon::UI_OK), 1, "CommonData/Sound/UI/ok.wav", false);
		SE->Add(static_cast<int>(SoundEnumCommon::UI_NG), 1, "CommonData/Sound/UI/ng.wav", false);
		SE->SetVol(OptionParts->GetParamFloat(EnumSaveParam::SE));

		m_ShadowDraw = std::make_unique<ShadowDraw>();
	}
	DXDraw::~DXDraw(void) noexcept {
		if (!m_IsFirstBoot) {
			if (m_VRControl) {
				this->GetVRControl()->Dispose();
				delete m_VRControl;
			}
			m_ShadowDraw->Dispose();
			m_ShadowDraw.reset();
			if (m_IsCubeMap) {
				m_RealTimeCubeMap.Dispose();
			}
			m_PBR_Shader.Dispose();
#ifdef _USE_EFFEKSEER_
			Effkseer_End();
#endif
		}
		DxLib_End();
	}
	//
	bool			DXDraw::UpdateShadowActive(void) noexcept {
		return m_ShadowDraw->UpdateActive();
	}
	void			DXDraw::Update_Shadow(std::function<void()> doing, const Vector3DX& CenterPos, float Scale, bool IsFar) noexcept {
		auto* OptionParts = OPTION::Instance();
		if (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0) {
			// 影用の深度記録画像の準備を行う
			if (!IsFar) {
				m_ShadowDraw->Update(doing, CenterPos, Scale);
			}
			else {
				m_ShadowDraw->UpdateFar(doing, CenterPos, Scale);
			}
		}
	}
	void			DXDraw::Update_CubeMap(std::function<void()> doing, const Vector3DX& CenterPos) noexcept {
		auto* OptionParts = OPTION::Instance();
		if ((OptionParts->GetParamInt(EnumSaveParam::Reflection) > 0) && OptionParts->GetParamBoolean(EnumProjectSettingParam::CubeMap)) {
			m_RealTimeCubeMap.ReadyDraw(CenterPos, doing);
		}
	}
	void			DXDraw::SetWindowOrBorderless(void) noexcept {
		auto* OptionParts = OPTION::Instance();
		if (!OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
			switch (static_cast<WindowType>(OptionParts->GetParamInt(EnumSaveParam::WindowMode))) {
			case WindowType::Window:
				this->m_DispXSize = this->m_DispXSize_Win;
				this->m_DispYSize = this->m_DispYSize_Win;
				SetWindowStyleMode(0);
				SetWindowPosition(0, 0);
				SetWindowSize(this->m_DispXSize, this->m_DispYSize);
				ChangeWindowMode(TRUE);
				break;
			case WindowType::Borderless:
				this->m_DispXSize = this->m_DispXSize_Border;
				this->m_DispYSize = this->m_DispYSize_Border;
				SetWindowStyleMode(2);
				SetWindowPosition(0, 0);
				SetWindowSize(this->m_DispXSize, this->m_DispYSize);
				ChangeWindowMode(TRUE);
				break;
			case WindowType::FullScreen:
				this->m_DispXSize = this->m_DispXSize_Border;
				this->m_DispYSize = this->m_DispYSize_Border;
				SetWindowStyleMode(2);
				SetWindowPosition(0, 0);
				SetWindowSize(this->m_DispXSize, this->m_DispYSize);
				SetFullScreenResolutionMode(DX_FSRESOLUTIONMODE_NATIVE);
				SetFullScreenScalingMode(DX_FSSCALINGMODE_NEAREST);
				ChangeWindowMode(FALSE);
				break;
			case WindowType::Max:
			default:
				break;
			}
		}
	}
	void			DXDraw::FirstBootSetting(void) noexcept {
		SetMainWindowText("FirstBoot Option");						//タイトル
		auto* OptionWindowParts = OptionWindowClass::Instance();
		auto* Pad = PadControl::Instance();
		auto* PopUpParts = PopUp::Instance();
		auto* LocalizeParts = LocalizePool::Instance();
		auto* OptionParts = OPTION::Instance();

		OptionWindowParts->Init();

		m_CheckPCSpec.Set();

		//初期設定画面
		OptionWindowParts->SetActive();
		while (ProcessMessage() == 0) {
			WindowSystem::DrawControl::Instance()->ClearList();
			int xBase = GetUIY(1366);
			int yBase = GetUIY(768);
			SetWindowPosition((deskx - xBase) / 2, (desky - yBase) / 2);
			SetWindowSize(xBase, yBase);

			Pad->Execute();
			OptionWindowParts->Execute();
			PopUpParts->Update();
			if (!PopUpParts->IsActivePop()) {
				break;
			}
			//
			GraphHandle::SetDraw_Screen(static_cast<int>(DX_SCREEN_BACK), true);
			{
				int Width = GetUIY(720);
				int Height = GetUIY(720);
				int Edge = GetUIY(16);

				PopUpParts->Draw(Width / 2 + Edge, Height / 2 + Edge);

				WindowSystem::SetMsg(Edge + Edge, Height + Edge + Edge, GetUIY(12), FontHandle::FontXCenter::LEFT, Green, Black, LocalizeParts->Get(109));

				int xp = Width + Edge + Edge;
				int yp = Edge;
				if (WindowSystem::SetMsgClickBox(xp, yp, xp + GetUIY(400), yp + LineHeight, LineHeight, Gray50, false, true, LocalizeParts->Get(2000))) {
					m_CheckPCSpec.StartSearch();
				}
				yp += GetUIY(24);
				if (m_CheckPCSpec.GetCPUDatas()) {
					int MouseOverID = INVALID_ID;
					//CPU
					WindowSystem::SetMsg(xp, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, DarkGreen, LocalizeParts->Get(2001)); yp += LineHeight;
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
						WindowSystem::SetMsg(xp + Edge, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, DarkGreen, "[%s]", c.m_Name.c_str());
						WindowSystem::SetMsg(xBase - Edge, yp + LineHeight / 2, LineHeight * 2 / 3, FontHandle::FontXCenter::RIGHT, Color, DarkGreen, "%s", LocalizeParts->Get(TextID)); yp += LineHeight;
						WindowSystem::SetMsg(xBase - Edge, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, White, DarkGreen, "PassMark Score:%d", c.m_Score); yp += LineHeight;
						yp += LineHeight;
					}
					if (m_CheckPCSpec.GetCPUDatas()->size() == 0) {
						WindowSystem::SetMsg(xp, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, Red, DarkGreen, LocalizeParts->Get(2005)); yp += LineHeight;
					}
					//Mem
					{
						WindowSystem::SetMsg(xp, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, DarkGreen, LocalizeParts->Get(2011)); yp += LineHeight;
						WindowSystem::SetMsg(xBase - Edge, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, DarkGreen, "[%4.3lfMB / %4.3lfMB]", m_CheckPCSpec.GetFreeMemorySize(), m_CheckPCSpec.GetTotalMemorySize());
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
						WindowSystem::SetMsg(xBase - Edge, yp + LineHeight / 2, LineHeight * 2 / 3, FontHandle::FontXCenter::RIGHT, Color, DarkGreen, "%s", LocalizeParts->Get(TextID)); yp += LineHeight;
						yp += LineHeight;
					}
					//GPU
					WindowSystem::SetMsg(xp, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, DarkGreen, LocalizeParts->Get(2021)); yp += LineHeight;
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
						WindowSystem::SetMsg(xp + Edge, yp + LineHeight / 2, LineHeight * 3 / 4, FontHandle::FontXCenter::LEFT, White, DarkGreen, "%s", c.m_Name.c_str());
						WindowSystem::SetMsg(xBase - Edge, yp + LineHeight / 2, LineHeight * 2 / 3, FontHandle::FontXCenter::RIGHT, Color, DarkGreen, "%s", LocalizeParts->Get(TextID)); yp += LineHeight;
						WindowSystem::SetMsg(xBase - Edge, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, White, DarkGreen, "PassMark Score:%d", c.m_Score); yp += LineHeight;
						yp += LineHeight;
					}
					if (m_CheckPCSpec.GetGPUDatas()->size() == 0) {
						WindowSystem::SetMsg(xp, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, Red, DarkGreen, LocalizeParts->Get(2025)); yp += LineHeight;
					}
					//DirectX
					int NowSet = OptionParts->GetParamInt(EnumSaveParam::DirectXVer);
					for (int loop : std::views::iota(0, 2)) {
						if (GetUseDirect3DVersion() == DirectXVerID[loop]) {
							NowSet = loop;
						}
					}
					if (IntoMouse(xp + Edge, yp, xBase - Edge, yp + LineHeight * 2)) {
						MouseOverID = 2048;
					}
					WindowSystem::SetMsg(xp, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, DarkGreen, LocalizeParts->Get(2035));
					WindowSystem::SetMsg(xBase - Edge, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, White, DarkGreen, "DirectX%s", DirectXVerStr[NowSet]); yp += LineHeight;
					if (MouseOverID != INVALID_ID) {
						xp = Pad->GetMS_X();
						yp = Pad->GetMS_Y();
						WindowSystem::SetMsg(xp, yp - LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, Green, DarkGreen, LocalizeParts->Get(MouseOverID));
					}
				}

				xp = Width + Edge + Edge + Edge;
				yp = Height;
				if (WindowSystem::SetMsgClickBox(xp, yp, xBase - Edge + Edge, yp + Edge + Edge, LineHeight, Green, false, true, "Start Game!")) {
					PopUpParts->EndAll();
				}

				WindowSystem::DrawControl::Instance()->Draw();
			}
			Screen_Flip();
		}
		OptionParts->Save();
	}
	void DXDraw::GetMousePosition(int* MouseX, int* MouseY) const noexcept {
		auto y_UIMs = [&](int p1) {
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->GetParamInt(EnumSaveParam::WindowMode) == static_cast<int>(WindowType::Window)) {
				return (int(p1) * m_DispYSize_Border / desky);
			}
			return (int(p1) * m_DispYSize / desky);
			};
		int mx = 0, my = 0;
		GetMousePoint(&mx, &my);
		*MouseX = y_UIMs(mx);
		*MouseY = y_UIMs(my);
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
		m_ShadowDraw->SetVec(AmbientLightVec);
	}
	//
	void			DXDraw::Init(void) noexcept {
		WindowSystem::DrawControl::Create();
		if (m_IsFirstBoot) {
			return;
		}
		auto* OptionParts = OPTION::Instance();
		auto* OptionWindowParts = OptionWindowClass::Instance();
		OptionWindowParts->Init();
		//Init
		m_PauseActive.Set(false);
		//シェーダー
		this->m_ScreenVertex.SetScreenVertex(ScreenWidth, ScreenHeight);// 頂点データの準備
		this->m_Shader2D[0].Init("CommonData/shader/VS_lens.vso", "CommonData/shader/PS_lens.pso");//レンズ
		this->m_Shader2D[1].Init("CommonData/shader/VS_BlackOut.vso", "CommonData/shader/PS_BlackOut.pso");//ブラックアウト
		m_PBR_Shader.Init("CommonData/shader/VS_PBR3D.vso", "CommonData/shader/PS_PBR3D.pso");
		m_PBR_Shader.AddGeometryShader("CommonData/shader/GS_PBR3D.pso");
		//
		LightPool::Create();
		//シェーダー
		PostPassEffect::Create();
		if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
			UI_Screen = GraphHandle::Make(ScreenWidth, ScreenHeight, true);	//UI
		}
		//影生成
		m_ShadowDraw->SetActive();
		//キューブマップ
		m_IsCubeMap = (OptionParts->GetParamInt(EnumSaveParam::Reflection) > 0) && OptionParts->GetParamBoolean(EnumProjectSettingParam::CubeMap);
		if (m_IsCubeMap) {
			m_RealTimeCubeMap.Init();
		}
		Update_effect_was = GetNowHiPerformanceCount();
	}
	bool			DXDraw::FirstExecute(void) noexcept {
		auto* Pad = PadControl::Instance();
		auto* PopUpParts = PopUp::Instance();
		auto* OptionWindowParts = OptionWindowClass::Instance();
		auto* LocalizeParts = LocalizePool::Instance();
		auto* PostPassParts = PostPassEffect::Instance();
		if (!(ProcessMessage() == 0)) {
			return false;
		}

		m_FPS = std::max(GetFPS(), 30.f);
		m_StartTime = GetNowHiPerformanceCount();
		if (Pad->GetEsc().trigger() && !m_IsExitSelect) {
			m_IsExitSelect = true;
			PopUpParts->Add(LocalizeParts->Get(100), 480, 240,
				[&](int xmin, int ymin, int xmax, int ymax, bool) {
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
				[&]() {m_IsExitSelect = false; },
				[&]() {},
				true
			);
		}
		if (OptionWindowParts->IsRestartSwitch() && !m_IsRestartSelect) {
			m_IsRestartSelect = true;
			PopUpParts->Add(LocalizeParts->Get(100), 480, 240,
				[&](int xmin, int ymin, int xmax, int ymax, bool) {
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
			bool Now = (OptionParts->GetParamInt(EnumSaveParam::Reflection) > 0) && OptionParts->GetParamBoolean(EnumProjectSettingParam::CubeMap);
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
		m_PauseActive.Execute(Pad->GetKey(PADS::INVENTORY).press() && !OptionWindowParts->IsActive());
		if (m_PauseActive.trigger()) {
			Pad->SetGuideUpdate();
		}
		//
#ifdef _USE_EFFEKSEER_
		if (!IsPause() && ((m_StartTime - Update_effect_was) >= 1000000 / 60)) {
			UpdateEffekseer3D();
			Update_effect_was = m_StartTime;
		}
#endif
		CameraShake::Instance()->Update();
	}
	void			DXDraw::Draw(
		std::function<void()> sky_doing,
		std::function<void()> setshadowdoing_rigid,
		std::function<void()> setshadowdoing,
		std::function<void()> doing,
		std::function<void()> doingFront,
		std::function<void()> doingUI,
		std::function<void()> doingUI2
	) noexcept {
		auto* OptionParts = OPTION::Instance();
		auto* PostPassParts = PostPassEffect::Instance();
		//描画
		auto MainDraw = [&](const Camera3DInfo& camInfo) {
			m_CamViewMatrix = camInfo.GetViewMatrix();
			m_CamProjectionMatrix = camInfo.GetProjectionMatrix();
			PostPassParts->DrawDoF(sky_doing, [&]() {
				auto* OptionParts = OPTION::Instance();
				if (OptionParts->GetParamBoolean(EnumProjectSettingParam::PBR)) {
					MATRIX view, projection;
					GetTransformToViewMatrix(&view);
					GetTransformToProjectionMatrix(&projection);
					m_PBR_Shader.SetGeometryCONSTBUFFER(1, &view, &projection);
					m_PBR_Shader.Draw_lamda(doing);
				}
				else {
					doing();
				}
				}, doingFront, camInfo);
			if (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0) {
				//影画像の用意
				m_ShadowDraw->SetDraw(setshadowdoing_rigid, setshadowdoing, camInfo);
				//ソフトシャドウ重ね
				PostPassParts->Plus_Draw([&]() { m_ShadowDraw->Draw(); });
			}
			PostPassParts->Draw();
			//完成した画面に対して後処理の2Dシェーダーを反映
			if (this->m_ShaderParam[0].use) {
				//レンズ
				PostPassParts->Plus_Draw([&]() {
					this->m_Shader2D[0].SetPixelDispSize(ScreenWidth, ScreenHeight);
					this->m_Shader2D[0].SetPixelParam(3, this->m_ShaderParam[0].param[0], this->m_ShaderParam[0].param[1], this->m_ShaderParam[0].param[2], this->m_ShaderParam[0].param[3]);
					SetUseTextureToShader(0, PostPassParts->Get_MAINBuffer_Screen().get());	//使用するテクスチャをセット
					this->m_Shader2D[0].Draw(this->m_ScreenVertex);
					SetUseTextureToShader(0, INVALID_ID);
					});
			}
			if (this->m_ShaderParam[1].use) {
				//ブラックアウト
				PostPassParts->Plus_Draw([&]() {
					this->m_Shader2D[1].SetPixelDispSize(ScreenWidth, ScreenHeight);
					this->m_Shader2D[1].SetPixelParam(3, this->m_ShaderParam[1].param[0], 0, 0, 0);
					SetUseTextureToShader(0, PostPassParts->Get_MAINBuffer_Screen().get());	//使用するテクスチャをセット
					this->m_Shader2D[1].Draw(this->m_ScreenVertex);
					SetUseTextureToShader(0, INVALID_ID);
					});
			}
			};
		if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
			//UIをスクリーンに描画しておく
			UI_Screen.SetDraw_Screen();
			{
				doingUI();
			}
			//VRに移す
			for (char i = 0; i < 2; ++i) {
				MainDraw(this->GetVRControl()->GetCamPos(i));
				//それぞれの目に内容を送信
				this->GetVRControl()->SubmitDraw(i, PostPassParts->Get_MAIN_Screen(), UI_Screen, doingUI2);
			}
			//ディスプレイ描画
			GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
			{
				FillGraph(GetDrawScreen(), 0, 0, 0);
				if (this->GetVRControl()->GetOutBuffer()) {
					this->GetVRControl()->GetOutBuffer()->DrawRotaGraph(ScreenWidth / 2, ScreenHeight / 2, 0.5f, 0, false);
				}
			}
		}
		else {
			MainDraw(GetMainCamera());
			//ディスプレイ描画
			GraphHandle::SetDraw_Screen(static_cast<int>(DX_SCREEN_BACK), true);
			{
				int Prev = GetDrawMode();
				//SetDrawMode(DX_DRAWMODE_NEAREST);
				SetDrawMode(DX_DRAWMODE_BILINEAR);
				PostPassParts->Get_MAIN_Screen().DrawExtendGraph(0, 0, this->m_DispXSize, this->m_DispYSize, false);
				SetDrawMode(Prev);
				doingUI();
				doingUI2();										//UI2
			}
		}
	}
	void			DXDraw::Draw2D(
		std::function<void()> doing,
		std::function<void()> doingUI
	) noexcept {
		auto* PostPassParts = PostPassEffect::Instance();
		//描画
		PostPassParts->Draw2D(doing);
		//
		PostPassParts->Draw();
		//ディスプレイ描画
		GraphHandle::SetDraw_Screen(static_cast<int>(DX_SCREEN_BACK), true);
		{
			int Prev = GetDrawMode();
			//SetDrawMode(DX_DRAWMODE_NEAREST);
			SetDrawMode(DX_DRAWMODE_BILINEAR);
			PostPassParts->Get_MAIN_Screen().DrawExtendGraph(0, 0, this->m_DispXSize, this->m_DispYSize, false);
			SetDrawMode(Prev);
			doingUI();
		}
	}
	bool					DXDraw::Screen_Flip(void) noexcept {
		auto* OptionParts = OPTION::Instance();
		ScreenFlip();
		if (!OptionParts->GetParamBoolean(EnumSaveParam::vsync)) {
			//4msだけスリープ
			while ((GetNowHiPerformanceCount() - m_StartTime) < static_cast<LONGLONG>(1000 * (1000 / OptionParts->GetParamInt(EnumSaveParam::FpsLimit) - 4))) {
				if (ProcessMessage() != 0) {
					return false;
				}
				SleepThread(1);	// 1msecスリープする
			}
			while ((GetNowHiPerformanceCount() - m_StartTime) < static_cast<LONGLONG>(1000 * 1000 / OptionParts->GetParamInt(EnumSaveParam::FpsLimit))) {
			}
		}
		else {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) {
				WaitVSync(1);
			}
		}
		this->GetVRControl()->WaitSync();
		return true;
	}
	//VR
	void			DXDraw::Get_VR_HMDPositionVR(Vector3DX* pos_, Matrix4x4DX* mat) noexcept { this->GetVRControl()->GetHMDPosition(pos_, mat); }
	void			DXDraw::Reset_VR_HMD(void) noexcept { this->GetVRControl()->ResetHMD(); }
	bool			DXDraw::Get_VR_Hand1PadPress(VR_PAD ID) const noexcept { return this->GetVRControl()->Get_VR_Hand1Device() ? this->GetVRControl()->Get_VR_Hand1Device()->PadPress(ID) : false; }
	bool			DXDraw::Get_VR_Hand1TouchPress(VR_PAD ID) const noexcept { return this->GetVRControl()->Get_VR_Hand1Device() ? this->GetVRControl()->Get_VR_Hand1Device()->PadTouch(ID) : false; }
	Vector3DX		DXDraw::Get_VR_Hand1TouchPadPoint(void) const noexcept { return this->GetVRControl()->Get_VR_Hand1Device() ? this->GetVRControl()->Get_VR_Hand1Device()->GetTouchPadPoint() : Vector3DX::zero(); }
	bool			DXDraw::Get_VR_Hand2PadPress(VR_PAD ID) const noexcept { return this->GetVRControl()->Get_VR_Hand2Device() ? this->GetVRControl()->Get_VR_Hand2Device()->PadPress(ID) : false; }
	bool			DXDraw::Get_VR_Hand2TouchPress(VR_PAD ID) const noexcept { return this->GetVRControl()->Get_VR_Hand2Device() ? this->GetVRControl()->Get_VR_Hand2Device()->PadTouch(ID) : false; }
	Vector3DX		DXDraw::Get_VR_Hand2TouchPadPoint(void) const noexcept { return this->GetVRControl()->Get_VR_Hand2Device() ? this->GetVRControl()->Get_VR_Hand2Device()->GetTouchPadPoint() : Vector3DX::zero(); }
	void			DXDraw::VR_Haptic(char id_, unsigned short times) noexcept { this->GetVRControl()->Haptic(id_, times); }
};
