#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
#if defined(_USE_OPENVR_)
	enum class VR_PAD {
		TRIGGER,
		SIDEBUTTON,
		TOUCHPAD,
		TOPBUTTON1,
		TOPBUTTON2,
	};
	class VRControl : public SingletonBase<VRControl> {
	private:
		friend class SingletonBase<VRControl>;
	private:
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
			//
			Vector3DX					m_Pos;
			Matrix3x3DX					m_Mat;
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
			const auto& GetPos(void) const noexcept { return m_Pos; }
			const auto& GetMat(void) const noexcept { return m_Mat; }
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
					this->m_Pos = Vector3DX::vget(tmp.mDeviceToAbsoluteTracking.m[0][3], tmp.mDeviceToAbsoluteTracking.m[1][3], -tmp.mDeviceToAbsoluteTracking.m[2][3]);
					this->m_Mat = Matrix3x3DX::Axis1(
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
					this->m_Pos = Vector3DX::vget(tmp.mDeviceToAbsoluteTracking.m[0][3], tmp.mDeviceToAbsoluteTracking.m[1][3], -tmp.mDeviceToAbsoluteTracking.m[2][3]);
					this->m_Mat = Matrix3x3DX::Axis1(
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
				this->m_Pos = Vector3DX::zero();
				this->m_Mat = Matrix3x3DX::identity();
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
	private:// コンストラクタ
		VRControl(void) noexcept{}
		VRControl(const VRControl&) = delete;
		VRControl(VRControl&& o) = delete;
		VRControl& operator=(const VRControl&) = delete;
		VRControl& operator=(VRControl&& o) = delete;
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
				*mat = Matrix4x4DX::Axis1(HMDPtr->GetMat().yvec(), HMDPtr->GetMat().zvec() * -1.f);
				if (!HMDPtr->IsActive()) {
					m_VR_HMD_StartFlag = true;
				}
				if (m_VR_HMD_StartFlag && HMDPtr->IsActive() != m_VR_PrevHMDIsActive) {
					m_VR_HMD_StartFlag = false;
					// 
					Vector3DX pos = HMDPtr->GetPos(); pos.y = 0.f;
					Vector3DX tmp = mat->zvec(); tmp.y = 0.f; tmp = tmp.normalized();
					float rad = DX_PI_F + std::atan2f(tmp.x, -tmp.z);
					m_VR_HMD_StartPoint = Matrix4x4DX::RotAxis(Vector3DX::up(), rad) * Matrix4x4DX::Mtrans(pos);
				}
				m_VR_PrevHMDIsActive = HMDPtr->IsActive();
				*pos_ = HMDPtr->GetPos() - m_VR_HMD_StartPoint.pos();
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
					return Matrix3x3DX::Vtrans(Vector3DX::vget(tmpmat.m[0][3], tmpmat.m[1][3], tmpmat.m[2][3]), HMDPtr->GetMat());
				}
			}
			return Vector3DX::zero();
		}
		const GraphHandle* GetOutBuffer(void) const noexcept { return &m_OutScreen; }
	public:
		void Init(void) noexcept;
		void SetupBuffer() noexcept;
		void Execute(void) noexcept;
		void Submit(char eye_type) noexcept;
		void SetUpBackUI(std::function<void()> doingUI) noexcept;
		void SubmitDraw(char eye_type, const GraphHandle& MainDrawScreen, std::function<void()> doingUI2) noexcept;
		void WaitSync(void) noexcept;
		void Dispose(void) noexcept;
	};
#endif
};
