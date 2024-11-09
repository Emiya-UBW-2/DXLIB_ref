#include "DXLib_ref_40_DXDraw.hpp"

namespace DXLibRef {
	const DXDraw* SingletonBase<DXDraw>::m_Singleton = nullptr;
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
#if defined(_USE_OPENVR_)
	class DXDraw::VRControl {
		class VRDeviceClass {
		private:
			int							m_ID{ 0 };
			char						m_DeviceNumber{ 0 };
			vr::ETrackedDeviceClass 	m_type{ vr::TrackedDeviceClass_Invalid };
			bool						m_isInitialized{ false };
			bool						m_isActive{ false };
			// 位置やボタン判定
			uint64_t					m_ButtonPressFlag{ 0 };
			uint64_t					m_ButtonTouchFlag{ 0 };
			Vector3DX					m_TouchPadPoint{ Vector3DX::zero() };
			moves						m_move;
		public:
			const auto& GetID(void) const noexcept { return m_ID; }
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
			const auto& GetTouchPadPoint(void) const noexcept { return m_TouchPadPoint; }
			const auto& GetMove(void) const noexcept { return m_move; }
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
		char						m_VR_HMDID{ InvalidID };
		char						m_VR_Hand1ID{ InvalidID };
		char						m_VR_Hand2ID{ InvalidID };
		std::vector<char>			m_VR_TrackerID;
		bool						m_VR_PrevHMDIsActive{ false };
		bool						m_VR_HMD_StartFlag{ true };
		Matrix4x4DX					m_VR_HMD_StartPoint;
		// 
		GraphHandle					m_OutScreen;							// スクリーンバッファ
		GraphHandle					UI_Screen;								// UI
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
			if (OptionParts->GetParamBoolean(EnumSaveParam::usevr) && (id_ != InvalidID) && m_VR_SystemPtr) {
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
			// VRが使えるかチェック
			if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
				m_VR_ErrorHandle = vr::VRInitError_None;
				m_VR_SystemPtr = vr::VR_Init(&m_VR_ErrorHandle, vr::VRApplication_Scene);
				if (m_VR_ErrorHandle != vr::VRInitError_None) {
					m_VR_SystemPtr = nullptr;
					OptionParts->SetParamBoolean(EnumSaveParam::usevr, false);
				}
			}
			// デバイスセット
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
		void SetupBuffer() noexcept {
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
				// 画面セット
				m_OutScreen.Make(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax());	// 左目
				UI_Screen.Make(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), true);	// UI
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
		void SetUpBackUI(std::function<void()> doingUI) noexcept {
			// UIをスクリーンに描画しておく
			UI_Screen.SetDraw_Screen();
			{
				doingUI();
			}
		}
		void SubmitDraw(char eye_type, const GraphHandle& MainDrawScreen, std::function<void()> doingUI2) noexcept {
			Camera3DInfo tmp_cam = GetCamPos(eye_type);
			// 目線用に合成
			m_OutScreen.SetDraw_Screen();
			tmp_cam.FlipCamInfo();
			{
				MainDrawScreen.DrawGraph(0, 0, true);
				{
					// 視差に対応しているUI
					SetCameraNearFar(0.01f, 2.f);
					SetUseZBuffer3D(FALSE);												// zbufuse
					SetWriteZBuffer3D(FALSE);											// zbufwrite
					{
						DrawBillboard3D((tmp_cam.GetCamPos() + (tmp_cam.GetCamVec() - tmp_cam.GetCamPos()).normalized() * 1.0f).get(),
							0.5f, 0.5f, 1.8f, 0.f, UI_Screen.get(), TRUE);
					}
					SetUseZBuffer3D(TRUE);												// zbufuse
					SetWriteZBuffer3D(TRUE);											// zbufwrite
				}
			}
			// 合成したものをBACKに持ってきて
			GraphHandle::SetDraw_Screen(static_cast<int>(DX_SCREEN_BACK));
			{
				m_OutScreen.DrawGraph(0, 0, false);
				doingUI2();
			}
			// それぞれの目にDX_SCREEN_BACKの内容を送信
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
				// vr::VR_Shutdown();
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
		void SetupBuffer() noexcept {}
		void Execute(void) noexcept {}
		void Submit(char) noexcept {}
		void SetUpBackUI(std::function<void()>) noexcept {}
		void SubmitDraw(char, const GraphHandle&, std::function<void()>) noexcept {}
		void WaitSync(void) noexcept {}
		void Dispose(void) noexcept {}
	};
#endif
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	void DXDraw::ShadowDraw::SetupCam(Vector3DX Center, float scale) const noexcept {
		ClearDrawScreen();
		SetupCamera_Ortho(30.f * scale * Scale3DRate);		// カメラのタイプを正射影タイプにセット、描画範囲も指定
		SetCameraNearFar(0.05f * scale * Scale3DRate, 60.f * scale * Scale3DRate);		// 描画する奥行き範囲をセット
		// カメラの位置と注視点はステージ全体が見渡せる位置
		auto Vec = m_ShadowVec;
		if (m_ShadowVec.x == 0.f && m_ShadowVec.z == 0.f) {
			Vec.z = (0.1f);
		}
		SetCameraPositionAndTarget_UpVecY((Center - Vec.normalized() * (30.f * scale * Scale3DRate)).get(), Center.get());
	}
	void DXDraw::ShadowDraw::Update(std::function<void()> Shadowdoing, Vector3DX Center, float Scale) noexcept {
		m_Scale = Scale;
		// 影用の深度記録画像の準備を行う
		DepthBaseScreenHandle.SetRenderTargetToShader(0);
		SetRenderTargetToShader(1, InvalidID);
		DepthScreenHandle.SetRenderTargetToShader(2);
		{
			SetupCam(Center, m_Scale);
			m_CamViewMatrix.at(0) = GetCameraViewMatrix();
			m_CamProjectionMatrix.at(0) = GetCameraProjectionMatrix();
			Shadowdoing();
		}
		SetRenderTargetToShader(0, InvalidID);
		SetRenderTargetToShader(1, InvalidID);
		SetRenderTargetToShader(2, InvalidID);
	}
	void DXDraw::ShadowDraw::UpdateFar(std::function<void()> Shadowdoing, Vector3DX Center, float Scale) noexcept {
		m_ScaleFar = Scale;
		// 影用の深度記録画像の準備を行う
		DepthBaseScreenHandle.SetRenderTargetToShader(0);
		SetRenderTargetToShader(1, InvalidID);
		DepthFarScreenHandle.SetRenderTargetToShader(2);
		{
			SetupCam(Center, m_ScaleFar);
			m_CamViewMatrix.at(1) = GetCameraViewMatrix();
			m_CamProjectionMatrix.at(1) = GetCameraProjectionMatrix();
			Shadowdoing();
		}
		SetRenderTargetToShader(0, InvalidID);
		SetRenderTargetToShader(1, InvalidID);
		SetRenderTargetToShader(2, InvalidID);
	}
	void DXDraw::ShadowDraw::SetDraw(std::function<void()> doing_rigid, std::function<void()> doing, Camera3DInfo tmp_cam) noexcept {
		auto* OptionParts = OPTION::Instance();
		BaseShadowHandle.SetUseTextureToShader(0);				// 影用深度記録画像をテクスチャにセット
		DepthScreenHandle.SetUseTextureToShader(1);
		DepthFarScreenHandle.SetUseTextureToShader(2);
		// 影の結果を出力
		tmp_cam.SetCamInfo(tmp_cam.GetCamFov(), 0.1f * Scale3DRate, 100.f * Scale3DRate);
		BaseShadowHandle.SetDraw_Screen();
		tmp_cam.FlipCamInfo();
		{
			m_Shader.SetPixelParam(3, static_cast<float>(OptionParts->GetParamInt(EnumSaveParam::shadow)), m_Scale * 180.f, 0.f, 0.f);
			m_Shader.SetVertexCameraMatrix(4, m_CamViewMatrix.at(0), m_CamProjectionMatrix.at(0));
			//m_Shader.SetVertexCameraMatrix(5, m_CamViewMatrix.at(1), m_CamProjectionMatrix.at(1));
			m_Shader.Draw_lamda(doing);
			m_ShaderRigid.SetPixelParam(3, static_cast<float>(OptionParts->GetParamInt(EnumSaveParam::shadow)), m_Scale * 180.f, 0.f, 0.f);
			m_ShaderRigid.SetVertexCameraMatrix(4, m_CamViewMatrix.at(0), m_CamProjectionMatrix.at(0));
			//m_ShaderRigid.SetVertexCameraMatrix(5, m_CamViewMatrix.at(1), m_CamProjectionMatrix.at(1));
			m_ShaderRigid.Draw_lamda(doing_rigid);
		}
		SetUseTextureToShader(1, InvalidID);				// 使用テクスチャの設定を解除
		SetUseTextureToShader(2, InvalidID);				// 使用テクスチャの設定を解除
		// 後処理
		BaseShadowHandle.GraphBlend(DepthBaseScreenHandle, 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
			DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_R);
	}
	void DXDraw::ShadowDraw::Draw(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
		BaseShadowHandle.DrawExtendGraph(0, 0, DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		// DepthScreenHandle.DrawExtendGraph(0, 0,1080,1080, true);
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
		auto* DrawParts = DXDraw::Instance();
		auto* OptionParts = OPTION::Instance();
		m_PrevShadow = OptionParts->GetParamInt(EnumSaveParam::shadow) > 0;
		BaseShadowHandle.Make(DrawParts->GetScreenXMax() / EXTEND, DrawParts->GetScreenYMax() / EXTEND, TRUE);
		int size = 2 << 10;
		DepthBaseScreenHandle.Make(size, size, FALSE);			// 深度バッファ用の作成
		DepthScreenHandle.MakeDepth(size, size);					// 深度バッファの作成
		DepthFarScreenHandle.MakeDepth(size, size);				// 深度バッファの作成
		m_Shader.Init("CommonData/shader/VS_SoftShadow.vso", "CommonData/shader/PS_SoftShadow.pso");
		m_ShaderRigid.Init("CommonData/shader/VS_SoftShadow_Rigid.vso", "CommonData/shader/PS_SoftShadow.pso");
	}
	bool DXDraw::ShadowDraw::UpdateActive(void) noexcept {
		auto* OptionParts = OPTION::Instance();
		bool shadow = OptionParts->GetParamInt(EnumSaveParam::shadow) > 0;
		if (m_PrevShadow != shadow) {
			m_PrevShadow = shadow;
			if (shadow) {
				SetActive();
				return true;
			}
			else {
				Dispose();
			}
		}
		return false;
	}
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	DXDraw::DXDraw(void) noexcept {
		auto* OptionParts = OPTION::Instance();
		// VR初期化
		if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
			m_VRControl = new VRControl;
			this->GetVRControl()->Init();// 機器が認識できないと中でusevr=falseに
		}
		if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
			// 解像度指定
			uint32_t t_x = 1080;
			uint32_t t_y = 1200;
			// m_VR_SystemPtr->GetRecommendedRenderTargetSize(&t_x,&t_y);
			int DispXSize = static_cast<int>(t_x) * 2;
			int DispYSize = static_cast<int>(t_y) * 2;

			this->m_DispXSize_Win = DispXSize - 72 * 16 / 9;
			this->m_DispYSize_Win = DispYSize - 72;
			this->m_DispXSize_Border = DispXSize;
			this->m_DispYSize_Border = DispYSize;
		}
		else {
			// DPIを反映するデスクトップサイズ
			int DispXSize = static_cast<int>(GetSystemMetrics(SM_CXSCREEN));
			int DispYSize = static_cast<int>(GetSystemMetrics(SM_CYSCREEN));
			// 解像度指定
			if (DispYSize >= (DispXSize * BaseScreenHeight / BaseScreenWidth)) {// 4:3
				DispYSize = (DispXSize * BaseScreenHeight / BaseScreenWidth);
			}
			else {// 16:9より横長
				DispXSize = (DispYSize * BaseScreenWidth / BaseScreenHeight);
			}
			this->m_DispXSize_Win = DispXSize - 72 * 16 / 9;
			this->m_DispYSize_Win = DispYSize - 72;
			this->m_DispXSize_Border = DispXSize;
			this->m_DispYSize_Border = DispYSize;
		}
		int DispXSize_Max = std::min(GetSizeXMax(), static_cast<int>(static_cast<float>(std::min<int>(BaseScreenWidth, GetSizeXMax())) * std::clamp(OptionParts->GetParamFloat(EnumSaveParam::DrawScale), 0.25f, 10.f)));
		int DispYSize_Max = std::min(GetSizeYMax(), static_cast<int>(static_cast<float>(std::min<int>(BaseScreenHeight, GetSizeYMax())) * std::clamp(OptionParts->GetParamFloat(EnumSaveParam::DrawScale), 0.25f, 10.f)));
		this->m_ScreenXSize = BaseScreenWidth * DispXSize_Max / this->m_DispXSize_Win;
		this->m_ScreenYSize = BaseScreenHeight * DispYSize_Max / this->m_DispYSize_Win;

		SetOutApplicationLogValidFlag(TRUE);						// log
		SetMainWindowText("Loading...");							// タイトル
		ChangeWindowMode(TRUE);										// 窓表示
		SetUseDirect3DVersion(DirectXVerID[OptionParts->GetParamInt(EnumSaveParam::DirectXVer)]);								// directX ver
		SetUseDirectInputFlag(TRUE);								// 
		SetDirectInputMouseMode(TRUE);								// 
		{
			// DPI設定
			int DPI = 96;
			GetMonitorDpi(NULL, &DPI);
			SetGraphMode(GetSizeXMax() * DPI / 96, GetSizeYMax() * DPI / 96, 32);		// 解像度
		}
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
		SetWindowOrBorderless();
		m_ShadowDraw = std::make_unique<ShadowDraw>();
	}
	DXDraw::~DXDraw(void) noexcept {
#if defined(_USE_EFFEKSEER_)
		Effkseer_End();
#endif
		if (m_VRControl) {
			this->GetVRControl()->Dispose();
			delete m_VRControl;
		}
		m_PBR_Shader.Dispose();
		if (m_IsCubeMap) {
			m_RealTimeCubeMap.Dispose();
		}
		m_ShadowDraw->Dispose();
		m_ShadowDraw.reset();
		DxLib_End();
	}
	// 
	bool			DXDraw::UpdateShadowActive(void) noexcept { return m_ShadowDraw->UpdateActive(); }
	void			DXDraw::Update_Shadow(std::function<void()> doing, const Vector3DX& CenterPos, float Scale, bool IsFar) noexcept {
		auto* OptionParts = OPTION::Instance();
		if (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0) {
			// 影用の深度記録画像の準備を行う
			if (!IsFar) {
				m_ShadowDraw->Update(doing, CenterPos, Scale);
			}
			else {
				//m_ShadowDraw->UpdateFar(doing, CenterPos, Scale);
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
				UpdateWindowSize();
				ChangeWindowMode(TRUE);
				break;
			case WindowType::Borderless:
				this->m_DispXSize = this->m_DispXSize_Border;
				this->m_DispYSize = this->m_DispYSize_Border;
				SetWindowStyleMode(2);
				SetWindowPosition(0, 0);
				UpdateWindowSize();
				ChangeWindowMode(TRUE);
				break;
			case WindowType::FullScreen:
				this->m_DispXSize = this->m_DispXSize_Border;
				this->m_DispYSize = this->m_DispYSize_Border;
				SetWindowStyleMode(2);
				SetWindowPosition(0, 0);
				UpdateWindowSize();
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
	void			DXDraw::GetMousePosition(int* MouseX, int* MouseY) const noexcept { GetMousePoint(MouseX, MouseY); }
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

	void			DXDraw::FirstBootSetting(void) noexcept {
		SetMainWindowText("FirstBoot Option");						// タイトル
		auto* OptionWindowParts = OptionWindowClass::Instance();
		auto* Pad = PadControl::Instance();
		auto* PopUpParts = PopUp::Instance();
		auto* LocalizeParts = LocalizePool::Instance();
		auto* OptionParts = OPTION::Instance();
		auto* DXLib_refParts = DXLib_ref::Instance();

		OptionWindowParts->Init();

		m_CheckPCSpec.Set();

		int xBase = GetUIY(1366);
		int yBase = GetUIY(768);
		SetWindowPosition((GetSizeXMax() - xBase) / 2, (m_DispYSize_Border - yBase) / 2);
		SetWindowSize(xBase, yBase);

		// 初期設定画面
		OptionWindowParts->SetActive();
		while (ProcessMessage() == 0) {
			DXLib_refParts->StartCount();
			WindowSystem::DrawControl::Instance()->ClearList();

			Pad->Update();
			OptionWindowParts->Update();
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
					int MouseOverID = InvalidID;
					// CPU
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
					// Mem
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
					// GPU
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
					WindowSystem::SetMsg(xp, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, DarkGreen, LocalizeParts->Get(2035));
					WindowSystem::SetMsg(xBase - Edge, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::RIGHT, White, DarkGreen, "DirectX%s", DirectXVerStr[NowSet]); yp += LineHeight;
					if (MouseOverID != InvalidID) {
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
			ScreenFlip();
			DXLib_refParts->WaitCount();
		}
		OptionParts->Save();
	}
	void			DXDraw::DrawByPBR(std::function<void()> doing) noexcept {
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
	}
	// 
	void			DXDraw::Init(void) noexcept {
		auto* OptionParts = OPTION::Instance();
		auto* OptionWindowParts = OptionWindowClass::Instance();
		OptionWindowParts->Init();
		// 
		this->GetVRControl()->SetupBuffer();
		// Init
		m_PauseActive.Set(false);
		// シェーダー
		m_PBR_Shader.Init("CommonData/shader/VS_PBR3D.vso", "CommonData/shader/PS_PBR3D.pso");
		m_PBR_Shader.AddGeometryShader("CommonData/shader/GS_PBR3D.pso");
		// 
		LightPool::Create();
		// シェーダー
		PostPassEffect::Create();
		auto* PostPassParts = PostPassEffect::Instance();
		PostPassParts->Init();
		// 影生成
		m_ShadowDraw->SetActive();
		// キューブマップ
		m_IsCubeMap = (OptionParts->GetParamInt(EnumSaveParam::Reflection) > 0) && OptionParts->GetParamBoolean(EnumProjectSettingParam::CubeMap);
		if (m_IsCubeMap) {
			m_RealTimeCubeMap.Init();
		}
		Update_effect_was = GetNowHiPerformanceCount();
	}
	void			DXDraw::Update(void) noexcept {
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
		Set3DSoundListenerPosAndFrontPosAndUpVec(SetMainCamera().GetCamPos().get(), SetMainCamera().GetCamVec().get(), SetMainCamera().GetCamUp().get());		// 音位置指定
		this->GetVRControl()->Execute();		// VR空間に適用
		// 
		m_PauseActive.Execute(Pad->GetKey(PADS::INVENTORY).press() && !OptionWindowParts->IsActive());
		if (m_PauseActive.trigger()) {
			Pad->SetGuideUpdate();
		}
		// 
#if defined(_USE_EFFEKSEER_)
		if (!IsPause() && ((GetNowHiPerformanceCount() - Update_effect_was) >= 1000000 / 60)) {
			Update_effect_was = GetNowHiPerformanceCount();
			UpdateEffekseer3D();
		}
#endif
		auto* PostPassParts = PostPassEffect::Instance();
		PostPassParts->Update();
	}

	void			DXDraw::Draw3DMain(
		std::function<void()> sky_doing,
		std::function<void()> setshadowdoing_rigid,
		std::function<void()> setshadowdoing,
		std::function<void()> doing,
		std::function<void()> doingFront,
		const Camera3DInfo& camInfo
	) noexcept {
		auto* OptionParts = OPTION::Instance();
		auto* PostPassParts = PostPassEffect::Instance();

		PostPassParts->SetCamMat(camInfo);
		// 全ての画面を初期化
		PostPassParts->ResetBuffer();
		// 空
		PostPassParts->DrawGBuffer(1000.0f, 50000.0f, sky_doing);
		// 遠距離
		PostPassParts->DrawGBuffer(camInfo.GetCamFar() - 10.f, 1000000.f, [&]() {
			DrawByPBR(doing);
			doingFront();
			});
		// 中間
		PostPassParts->DrawGBuffer(camInfo.GetCamNear(), camInfo.GetCamFar(), [&]() {
#if defined(_USE_EFFEKSEER_)
			Effekseer_Sync3DSetting();
#endif
			DrawByPBR(doing);
#if defined(_USE_EFFEKSEER_)
			DrawEffekseer3D();
#endif
			doingFront();
			});
		// 至近
		PostPassParts->DrawGBuffer(0.1f, 0.1f + camInfo.GetCamNear(), [&]() {
#if defined(_USE_EFFEKSEER_)
			Effekseer_Sync3DSetting();
#endif
			DrawByPBR(doing);
#if defined(_USE_EFFEKSEER_)
			DrawEffekseer3D();
#endif
			doingFront();
			});
		// 影
		if (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0) {
			// 影画像の用意
			m_ShadowDraw->SetDraw(setshadowdoing_rigid, setshadowdoing, camInfo);
			// ソフトシャドウ重ね
			PostPassParts->GetBufferScreen().SetDraw_Screen(false);
			{
				m_ShadowDraw->Draw();
			}
		}
		// ポストプロセス
		PostPassParts->DrawPostProcess();
	}
	void			DXDraw::Draw2DMain(std::function<void()> doing) noexcept {
		auto* PostPassParts = PostPassEffect::Instance();
		// Dofを無効化
		PostPassParts->Set_DoFNearFar(0.1f, 5.f, 0.05f, 6.f);
		// 全ての画面を初期化
		PostPassParts->ResetBuffer();
		// 2D描画
		PostPassParts->GetBufferScreen().SetDraw_Screen();
		{
			doing();
		}
		// ポストプロセス
		PostPassParts->DrawPostProcess();
	}
	void			DXDraw::Draw3DVR(
		std::function<void()> sky_doing,
		std::function<void()> setshadowdoing_rigid,
		std::function<void()> setshadowdoing,
		std::function<void()> doing,
		std::function<void()> doingFront,
		std::function<void()> doingUI,
		std::function<void()> doingUI2
	) noexcept {
		auto* DrawParts = DXDraw::Instance();
		auto* PostPassParts = PostPassEffect::Instance();
		// UIをスクリーンに描画しておく
		this->GetVRControl()->SetUpBackUI(doingUI);
		// VRに移す
		for (char i = 0; i < 2; ++i) {
			Draw3DMain(
				sky_doing,
				setshadowdoing_rigid,
				setshadowdoing,
				doing,
				doingFront,
				this->GetVRControl()->GetCamPos(i));
			// それぞれの目に内容を送信
			this->GetVRControl()->SubmitDraw(i, PostPassParts->GetBufferScreen(), doingUI2);
		}
		// ディスプレイ描画
		GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
		{
			FillGraph(GetDrawScreen(), 0, 0, 0);
			if (this->GetVRControl()->GetOutBuffer()) {
				this->GetVRControl()->GetOutBuffer()->DrawRotaGraph(DrawParts->GetScreenXMax() / 2, DrawParts->GetScreenYMax() / 2, 0.5f, 0, false);
			}
		}
	}
	void			DXDraw::DrawFlipDisplay(std::function<void()> doingUI) const noexcept {
		auto* PostPassParts = PostPassEffect::Instance();
		GraphHandle::SetDraw_Screen(static_cast<int>(DX_SCREEN_BACK), true);
		{
			int Prev = GetDrawMode();
			// SetDrawMode(DX_DRAWMODE_NEAREST);
			SetDrawMode(DX_DRAWMODE_BILINEAR);
			PostPassParts->GetBufferScreen().DrawExtendGraph(0, 0, this->m_DispXSize, this->m_DispYSize, false);
			SetDrawMode(Prev);
			doingUI();
		}
	}
	// VR
	void			DXDraw::Get_VR_HMDPositionVR(Vector3DX* pos_, Matrix4x4DX* mat) noexcept { this->GetVRControl()->GetHMDPosition(pos_, mat); }
	void			DXDraw::Reset_VR_HMD(void) noexcept { this->GetVRControl()->ResetHMD(); }
	bool			DXDraw::Get_VR_Hand1PadPress(VR_PAD ID) const noexcept { return this->GetVRControl()->Get_VR_Hand1Device() ? this->GetVRControl()->Get_VR_Hand1Device()->PadPress(ID) : false; }
	bool			DXDraw::Get_VR_Hand1TouchPress(VR_PAD ID) const noexcept { return this->GetVRControl()->Get_VR_Hand1Device() ? this->GetVRControl()->Get_VR_Hand1Device()->PadTouch(ID) : false; }
	Vector3DX		DXDraw::Get_VR_Hand1TouchPadPoint(void) const noexcept { return this->GetVRControl()->Get_VR_Hand1Device() ? this->GetVRControl()->Get_VR_Hand1Device()->GetTouchPadPoint() : Vector3DX::zero(); }
	bool			DXDraw::Get_VR_Hand2PadPress(VR_PAD ID) const noexcept { return this->GetVRControl()->Get_VR_Hand2Device() ? this->GetVRControl()->Get_VR_Hand2Device()->PadPress(ID) : false; }
	bool			DXDraw::Get_VR_Hand2TouchPress(VR_PAD ID) const noexcept { return this->GetVRControl()->Get_VR_Hand2Device() ? this->GetVRControl()->Get_VR_Hand2Device()->PadTouch(ID) : false; }
	Vector3DX		DXDraw::Get_VR_Hand2TouchPadPoint(void) const noexcept { return this->GetVRControl()->Get_VR_Hand2Device() ? this->GetVRControl()->Get_VR_Hand2Device()->GetTouchPadPoint() : Vector3DX::zero(); }
	void			DXDraw::VR_Haptic(char id_, unsigned short times) noexcept { this->GetVRControl()->Haptic(id_, times); }
	void			DXDraw::VR_WaitSync(void) noexcept { this->GetVRControl()->WaitSync(); }
};
