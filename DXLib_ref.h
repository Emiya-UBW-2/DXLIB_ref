#pragma once

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*ビルド設定																																	*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
//#define DEBUG
//#define _USE_OPENVR_

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*pragma																																	*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/

#ifdef _USE_OPENVR_
#pragma comment(lib,"..\\..\\..\\openvr\\openvr_api.lib")
#endif // _USE_OPENVR_

#ifdef _DEBUG
#pragma comment(lib,"..\\..\\..\\Box2D\\Debug\\Box2D.lib")
#else
#pragma comment(lib,"..\\..\\..\\Box2D\\Release\\Box2D.lib")
#endif // _DEBUG
//エラー,警告取り
#define NOMINMAX
#pragma warning(disable:4505)

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*include																																	*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
//VR
#ifdef _USE_OPENVR_
#include <openvr.h>
#define BUTTON_TRIGGER vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Trigger)
#define BUTTON_SIDE vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_Grip)
#define BUTTON_TOUCHPAD vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Touchpad)
#define BUTTON_TOPBUTTON vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_ApplicationMenu)
#define BUTTON_TOPBUTTON_B vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_IndexController_B)
#endif // _USE_OPENVR_
//共通
#include <stdint.h>
#include <array>
#include <list>
#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include <memory>
#include <functional>
#include <algorithm>
#include <D3D11.h>
#include <optional>
#include <cassert>
#include <filesystem>
#include <VersionHelpers.h>
#include <iostream>
#include <math.h>
//追加物
#include "Box2D/Box2D.h"
#include "json.hpp"

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*const																																		*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
constexpr float M_GR{ -122.5f };				/*重力加速度*/
//constexpr float M_GR{ -9.8f };				/*重力加速度*/

//DPIを反映するデスクトップサイズ
//const int32_t deskx{ (int32_t)(950) / 1 };
//const int32_t desky{ (int32_t)(950 * 9 / 16) / 1 };
const int32_t deskx{ (int32_t)(GetSystemMetrics(SM_CXSCREEN)) / 1 };
const int32_t desky{ (int32_t)(GetSystemMetrics(SM_CYSCREEN)) / 1 };

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*include DXLIB																																*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
//DXLIB
#include "DxLib.h"
#include "EffekseerForDXLib.h"
//追加
#include "DXLib_ref_00_Util.hpp"
#include "DXLib_ref_01_Vector.hpp"
#include "DXLib_ref_02_SoundHandle.hpp"
#include "DXLib_ref_03_GraphHandle.hpp"
#include "DXLib_ref_04_LightHandle.hpp"
#include "DXLib_ref_05_FontHandle.hpp"
#include "DXLib_ref_06_MV1Handle.hpp"
#include "DXLib_ref_07_EffekseerEffectHandle.hpp"
#include "DXLib_ref_08_Etc.hpp"
#include "DXLib_ref_09_KeyControl.hpp"
#include "DXLib_ref_10_Option.hpp"
#include "DXLib_ref_20_Debug.hpp"
#include "DXLib_ref_30_Box2D.hpp"
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*必須オブジェクト																															*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
namespace DXLib_ref {
	class ShadowControl {
		int				m_ShadowHandle{ -1 };
		VECTOR_ref		m_ShadowVec;
	public:
		const auto&		GetHandle(void) const noexcept { return m_ShadowHandle; }
		void			SetDir(const VECTOR_ref& Vec) noexcept { m_ShadowVec = Vec; }
	public:
		void			Init() noexcept {
			auto size = int(pow(2, 13));
			//近
			if (m_ShadowHandle == -1) {
				m_ShadowHandle = MakeShadowMap(size, size);
			}
			SetShadowMapAdjustDepth(m_ShadowHandle, 0.0005f);
		}
		void			Update(std::function<void()> doing, const VECTOR_ref& MaxPos, const VECTOR_ref& MinPos) noexcept {
			if (m_ShadowHandle != -1) {
				SetShadowMapLightDirection(m_ShadowHandle, m_ShadowVec.get());
				SetShadowMapDrawArea(m_ShadowHandle, MinPos.get(), MaxPos.get());
				ShadowMap_DrawSetup(m_ShadowHandle);
				doing();
				ShadowMap_DrawEnd();
			}
		}
		void			Dispose() noexcept {
			if (m_ShadowHandle != -1) {
				DeleteShadowMap(m_ShadowHandle);
				m_ShadowHandle = -1;
			}
		}
	};
	//main
	class DXDraw : public SingletonBase<DXDraw> {
	private:
		friend class SingletonBase<DXDraw>;
	public:
		int				m_DispXSize{ deskx };
		int				m_DispYSize{ desky };
	private:
		switchs			m_PauseActive;
		//LONGLONG		m_StartTime{ 0 };
		std::array<ShadowControl, 3>		m_Shadow;
		VECTOR_ref		m_LightVec;
		COLOR_F			m_LightColorF{ GetColorF(0, 0, 0, 0) };
		//
		Camera3DInfo	m_MainCamera;					//カメラ
		//カメラシェイク
		bool						m_SendCamShake{ false };
		float						m_SendCamShakeTime{ 1.f };
		float						m_SendCamShakePower{ 1.f };
		float						m_CamShake{ 0.f };
		VECTOR_ref					m_CamShake1;
		VECTOR_ref					m_CamShake2;
	private://コンストラクタ
		DXDraw(void) noexcept;
		~DXDraw(void) noexcept;
	public:
		const auto		IsPause() const noexcept { return m_PauseActive.on(); }
		const auto		IsPauseSwitch() const noexcept { return m_PauseActive.trigger(); }
		void			PauseIn() noexcept;
		void			PauseExit() noexcept;

		auto&			SetMainCamera(void) noexcept { return m_MainCamera; }
		const auto&		GetMainCamera(void) const noexcept { return m_MainCamera; }

		const auto&		GetCamShake(void) const noexcept { return m_CamShake2; }
	public:
		void			SetUseShadow(void) noexcept {
			for (auto& s : m_Shadow) {
				SetUseShadowMap((int)(&s - &m_Shadow.front()), s.GetHandle());
			}
		}
		void			ResetUseShadow(void) noexcept {
			for (auto& s : m_Shadow) {
				SetUseShadowMap((int)(&s - &m_Shadow.front()), -1);
			}
		}
		void			Update_Shadow(std::function<void()> doing, const VECTOR_ref& MaxPos, const VECTOR_ref& MinPos, int shadowSelect) noexcept;
		void			SetShadowDir(const VECTOR_ref& Vec, int shadowSelect) noexcept { m_Shadow[shadowSelect].SetDir(Vec); }
		void			SetAmbientLight(const VECTOR_ref& AmbientLightVec, const COLOR_F& LightColor) noexcept;

		void			SetCamShake(float time, float power) noexcept {
			this->m_SendCamShake = true;
			this->m_SendCamShakeTime = time;
			this->m_SendCamShakePower = power;
		}
	public:

		void			Execute(void) noexcept;
		void			Draw(
			std::function<void(const Camera3DInfo&)> doing,
			std::function<void()> doingUI,
			std::function<void()> doingUI2
		) noexcept;
		bool			Screen_Flip(void) noexcept;

		//VR
#ifdef _USE_OPENVR_
	public:
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
			VECTOR_ref					m_TouchPadPoint{ VECTOR_ref::zero() };
			VECTOR_ref					m_pos;
			MATRIX_ref					m_mat;
		public:
			const auto&		GetID(void) noexcept { return m_ID; }
			const auto&		GetPos(void) noexcept { return m_pos; }
			const auto&		GetMat(void) noexcept { return m_mat; }
			const auto		IsActive(void) noexcept { return m_isInitialized && m_isActive; }
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
					this->m_TouchPadPoint = VECTOR_ref::zero();
					this->m_isActive = tmp.bPoseIsValid;
					this->m_pos = VECTOR_ref::vget(tmp.mDeviceToAbsoluteTracking.m[0][3], tmp.mDeviceToAbsoluteTracking.m[1][3], -tmp.mDeviceToAbsoluteTracking.m[2][3]);
					this->m_mat = MATRIX_ref::Axis1(
						VECTOR_ref::vget(-tmp.mDeviceToAbsoluteTracking.m[0][0], -tmp.mDeviceToAbsoluteTracking.m[1][0], tmp.mDeviceToAbsoluteTracking.m[2][0])*-1.f,
						VECTOR_ref::vget(tmp.mDeviceToAbsoluteTracking.m[0][1], tmp.mDeviceToAbsoluteTracking.m[1][1], -tmp.mDeviceToAbsoluteTracking.m[2][1]),
						VECTOR_ref::vget(tmp.mDeviceToAbsoluteTracking.m[0][2], tmp.mDeviceToAbsoluteTracking.m[1][2], -tmp.mDeviceToAbsoluteTracking.m[2][2])*-1.f
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
					this->m_TouchPadPoint = VECTOR_ref::vget(night.rAxis[0].x, night.rAxis[0].y, 0);
					this->m_isActive = tmp.bPoseIsValid;
					this->m_pos = VECTOR_ref::vget(tmp.mDeviceToAbsoluteTracking.m[0][3], tmp.mDeviceToAbsoluteTracking.m[1][3], -tmp.mDeviceToAbsoluteTracking.m[2][3]);
					this->m_mat = MATRIX_ref::Axis1(
						VECTOR_ref::vget(-tmp.mDeviceToAbsoluteTracking.m[0][0], -tmp.mDeviceToAbsoluteTracking.m[1][0], tmp.mDeviceToAbsoluteTracking.m[2][0])*-1.f,
						VECTOR_ref::vget(tmp.mDeviceToAbsoluteTracking.m[0][1], tmp.mDeviceToAbsoluteTracking.m[1][1], -tmp.mDeviceToAbsoluteTracking.m[2][1]),
						VECTOR_ref::vget(tmp.mDeviceToAbsoluteTracking.m[0][2], tmp.mDeviceToAbsoluteTracking.m[1][2], -tmp.mDeviceToAbsoluteTracking.m[2][2])*-1.f
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
				this->m_TouchPadPoint = VECTOR_ref::zero();
				this->m_pos.clear();
				this->m_mat.clear();
			}
		};
	private:
		//VR
		vr::IVRSystem*				m_VR_SystemPtr{ nullptr };
		vr::EVRInitError			m_VR_ErrorHandle{ vr::VRInitError_None };
		std::vector<VRDeviceClass>	m_VR_DeviceInfo;
		char						m_VR_HMDID{ -1 };
		char						m_VR_Hand1ID{ -1 };
		char						m_VR_Hand2ID{ -1 };
		std::vector<char>			m_VR_TrackerID;
		bool						m_VR_PrevHMDIsActive{ false };
		bool						m_VR_HMD_StartFlag{ true };
		MATRIX_ref					m_VR_HMD_StartPoint;
		//表示時の画面
		std::array<GraphHandle, 2>	m_OutScreen;			//スクリーンバッファ
		GraphHandle					UI_Screen;				//UI
	public:
		auto*			Get_VR_DeviceList(void) noexcept { return &m_VR_DeviceInfo; }
		const auto&		Get_VR_HMDID(void) noexcept { return m_VR_HMDID; }
		VRDeviceClass*	Get_VR_HMDDevice(void) noexcept {
			if (m_VR_HMDID >= 0) {
				return &m_VR_DeviceInfo.at(m_VR_HMDID);
			}
			return nullptr;
		}
		const auto&		Get_VR_Hand1ID(void) noexcept { return m_VR_Hand1ID; }
		VRDeviceClass*	Get_VR_Hand1Device(void) noexcept {
			if (m_VR_Hand1ID >= 0) {
				return &m_VR_DeviceInfo.at(m_VR_Hand1ID);
			}
			return nullptr;
		}
		const auto&		Get_VR_Hand2ID(void) noexcept { return m_VR_Hand2ID; }
		VRDeviceClass*	Get_VR_Hand2Device(void) noexcept {
			if (m_VR_Hand2ID >= 0) {
				return &m_VR_DeviceInfo.at(m_VR_Hand2ID);
			}
			return nullptr;
		}
		auto*			Get_VR_TrackerIDList(void) noexcept { return &m_VR_TrackerID; }

		void			Get_VR_DevicePositionVR(char handle_, VECTOR_ref* pos_, MATRIX_ref*mat) noexcept;
		void			Get_VR_HMDPositionVR(VECTOR_ref* pos_, MATRIX_ref*mat) noexcept;
		const VECTOR_ref	Set_VR_EyePositionVR(char eye_type) noexcept;
		const VECTOR_ref	Get_VR_EyePosition_minVR(char) noexcept;
		void			Reset_VR_HMD(void) noexcept;							//HMDのリセット
		void			VR_Haptic(char id_, unsigned short times) noexcept;	//VRコントローラー振動
	private:
		void			VR_Init(void) noexcept;
		void			VR_Execute(void) noexcept;							//更新
		void			VR_Draw(std::function<void()> doing) noexcept;
		void			VR_WaitSync(void) noexcept;
		void			VR_Dispose(void) noexcept;
#endif // _USE_OPENVR_
	};

};

#include "DXLib_ref_100_PostPass.hpp"
#include "DXLib_ref_101_sequence.hpp"
