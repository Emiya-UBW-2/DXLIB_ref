#include "DXLib_ref_40_WindowSizeControl.hpp"

namespace DXLibRef {
	const WindowSizeControl* SingletonBase<WindowSizeControl>::m_Singleton = nullptr;
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
#if defined(_USE_OPENVR_)
	class WindowSizeControl::VRControl {
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
			auto* WindowSizeParts = WindowSizeControl::Instance();
			Camera3DInfo tmp_cam = WindowSizeParts->GetMainCamera();
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
				m_OutScreen.Make(WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax());	// 左目
				UI_Screen.Make(WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), true);	// UI
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
	class WindowSizeControl::VRControl {
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
			auto* WindowSizeParts = WindowSizeControl::Instance();
			Camera3DInfo tmp_cam = WindowSizeParts->GetMainCamera();
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
	WindowSizeControl::WindowSizeControl(void) noexcept {
		auto* OptionParts = OPTION::Instance();
		// VR初期化
		if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
			m_VRControl = new VRControl;
			this->GetVRControl()->Init();// 機器が認識できないと中でusevr=falseに
		}
	}
	// 
	void			WindowSizeControl::SetWindowOrBorderless(void) noexcept {
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
	// 
	void			WindowSizeControl::SetAmbientLight(const Vector3DX& AmbientLightVec, const COLOR_F& LightColor) noexcept {
		m_LightVec = AmbientLightVec;
		m_LightColorF = LightColor;
		SetGlobalAmbientLight(LightColor);
		SetLightDirection(AmbientLightVec.get());
		auto* PostPassParts = PostPassEffect::Instance();
		PostPassParts->SetAmbientLight(AmbientLightVec);
	}

	void			WindowSizeControl::SetupWindowSize(void) noexcept {
		auto* OptionParts = OPTION::Instance();
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
	}
	// 
	void			WindowSizeControl::Draw3DMain(
		std::function<void()> sky_doing,
		std::function<void()> setshadowdoing_rigid,
		std::function<void()> setshadowdoing,
		std::function<void()> doing,
		std::function<void()> doingFront,
		const Camera3DInfo& camInfo
	) noexcept {
		auto* PostPassParts = PostPassEffect::Instance();

		PostPassParts->SetCamMat(camInfo);
		// 全ての画面を初期化
		PostPassParts->ResetBuffer();
		// 空
		PostPassParts->DrawGBuffer(1000.0f, 50000.0f, sky_doing);
		// 遠距離
		PostPassParts->DrawGBuffer(camInfo.GetCamFar() - 10.f, 1000000.f, [&]() {
			auto* PostPassParts = PostPassEffect::Instance();
			PostPassParts->DrawByPBR(doing);
			doingFront();
			});
		// 中間
		PostPassParts->DrawGBuffer(camInfo.GetCamNear(), camInfo.GetCamFar(), [&]() {
#if defined(_USE_EFFEKSEER_)
			Effekseer_Sync3DSetting();
#endif
			auto* PostPassParts = PostPassEffect::Instance();
			PostPassParts->DrawByPBR(doing);
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
			auto* PostPassParts = PostPassEffect::Instance();
			PostPassParts->DrawByPBR(doing);
#if defined(_USE_EFFEKSEER_)
			DrawEffekseer3D();
#endif
			doingFront();
			});
		// ポストプロセス
		PostPassParts->DrawPostProcess(camInfo, setshadowdoing_rigid, setshadowdoing);
	}
	void			WindowSizeControl::Draw3DVR(
		std::function<void()> sky_doing,
		std::function<void()> setshadowdoing_rigid,
		std::function<void()> setshadowdoing,
		std::function<void()> doing,
		std::function<void()> doingFront,
		std::function<void()> doingUI,
		std::function<void()> doingUI2
	) noexcept {
		auto* WindowSizeParts = WindowSizeControl::Instance();
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
				this->GetVRControl()->GetOutBuffer()->DrawRotaGraph(WindowSizeParts->GetScreenXMax() / 2, WindowSizeParts->GetScreenYMax() / 2, 0.5f, 0, false);
			}
		}
	}
	// VR
	void			WindowSizeControl::VR_Setup(void) noexcept { this->GetVRControl()->SetupBuffer(); }
	void			WindowSizeControl::VR_Update(void) noexcept { this->GetVRControl()->Execute(); }
	void			WindowSizeControl::VR_Dispose(void) noexcept {
		if (m_VRControl) {
			this->GetVRControl()->Dispose();
			delete m_VRControl;
		}
	}
	void			WindowSizeControl::Get_VR_HMDPositionVR(Vector3DX* pos_, Matrix4x4DX* mat) noexcept { this->GetVRControl()->GetHMDPosition(pos_, mat); }
	void			WindowSizeControl::Reset_VR_HMD(void) noexcept { this->GetVRControl()->ResetHMD(); }
	bool			WindowSizeControl::Get_VR_Hand1PadPress(VR_PAD ID) const noexcept { return this->GetVRControl()->Get_VR_Hand1Device() ? this->GetVRControl()->Get_VR_Hand1Device()->PadPress(ID) : false; }
	bool			WindowSizeControl::Get_VR_Hand1TouchPress(VR_PAD ID) const noexcept { return this->GetVRControl()->Get_VR_Hand1Device() ? this->GetVRControl()->Get_VR_Hand1Device()->PadTouch(ID) : false; }
	Vector3DX		WindowSizeControl::Get_VR_Hand1TouchPadPoint(void) const noexcept { return this->GetVRControl()->Get_VR_Hand1Device() ? this->GetVRControl()->Get_VR_Hand1Device()->GetTouchPadPoint() : Vector3DX::zero(); }
	bool			WindowSizeControl::Get_VR_Hand2PadPress(VR_PAD ID) const noexcept { return this->GetVRControl()->Get_VR_Hand2Device() ? this->GetVRControl()->Get_VR_Hand2Device()->PadPress(ID) : false; }
	bool			WindowSizeControl::Get_VR_Hand2TouchPress(VR_PAD ID) const noexcept { return this->GetVRControl()->Get_VR_Hand2Device() ? this->GetVRControl()->Get_VR_Hand2Device()->PadTouch(ID) : false; }
	Vector3DX		WindowSizeControl::Get_VR_Hand2TouchPadPoint(void) const noexcept { return this->GetVRControl()->Get_VR_Hand2Device() ? this->GetVRControl()->Get_VR_Hand2Device()->GetTouchPadPoint() : Vector3DX::zero(); }
	void			WindowSizeControl::VR_Haptic(char id_, unsigned short times) noexcept { this->GetVRControl()->Haptic(id_, times); }
	void			WindowSizeControl::VR_WaitSync(void) noexcept { this->GetVRControl()->WaitSync(); }
};
