#pragma once

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*ビルド設定																																	*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
#define DEBUG
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
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")
//追加物
#include "Box2D/Box2D.h"
#include "json.hpp"
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*const																																		*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
constexpr float M_GR{-122.5f};				/*重力加速度*/
//constexpr float M_GR{ -9.8f };				/*重力加速度*/

//DPIを反映するデスクトップサイズ
//const int32_t deskx{ (int32_t)(950) / 1 };
//const int32_t desky{ (int32_t)(950 * 9 / 16) / 1 };
const int32_t deskx{(int32_t)(GetSystemMetrics(SM_CXSCREEN)) / 1};
const int32_t desky{(int32_t)(GetSystemMetrics(SM_CYSCREEN)) / 1};

enum class VR_PAD {
	TRIGGER,
	SIDEBUTTON,
	TOUCHPAD,
	TOPBUTTON1,
	TOPBUTTON2,
};

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
	class DXDraw : public SingletonBase<DXDraw> {
	private:
		friend class SingletonBase<DXDraw>;
	private:
		class ShadowControl {
			int				m_ShadowHandle{-1};
			Vector3DX		m_ShadowVec;
			Vector3DX		m_ShadowMax;
			Vector3DX		m_ShadowMin;
			bool			m_isUpdate{false};
		public:
			const auto&		GetHandle(void) const noexcept { return m_ShadowHandle; }
			void			Set(const Vector3DX& Vec, const Vector3DX& MinSize, const Vector3DX& MaxSize) noexcept {
				m_ShadowVec = Vec;
				m_ShadowMin = MinSize;
				m_ShadowMax = MaxSize;
			}
			void SetisUpdate(bool value) noexcept { m_isUpdate = value; }
		public:
			void			Init() noexcept {
				if (m_ShadowHandle == -1) {
					auto size = int(pow(2, 12));
					m_ShadowHandle = MakeShadowMap(size, size);
					SetShadowMapAdjustDepth(m_ShadowHandle, 0.0005f);
				}
			}
			void			Update(std::function<void()> doing, const Vector3DX& CenterPos) noexcept {
				if (m_ShadowHandle != -1 && m_isUpdate) {
					SetShadowMapLightDirection(m_ShadowHandle, m_ShadowVec.get());
					SetShadowMapDrawArea(m_ShadowHandle, (CenterPos + m_ShadowMin).get(), (CenterPos + m_ShadowMax).get());
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
		class ShadowDraw {
			GraphHandle	BaseShadowHandle;			// モデルハンドル
			GraphHandle DepthBaseScreenHandle;
			GraphHandle DepthScreenHandle;
			// 深度記録画像を使ったディレクショナルライト一つ付きの描画用の剛体メッシュ用とスキニングメッシュ用のシェーダー
			ShaderUseClass					m_Shader_Skin4_DepthShadow_Step2;
			ShaderUseClass					m_Shader_Normal_DepthShadow_Step2;
			Vector3DX		m_ShadowVec{Vector3DX::up()};
			bool			m_isUpdate{false};
		public:
			void			SetVec(const Vector3DX& Vec) noexcept { m_ShadowVec = Vec; }
			void SetisUpdate(bool value) noexcept { m_isUpdate = value; }

			void Init(int ShadowMapSize, int dispsizex, int dispsizey);
			void Update(std::function<void()> Shadowdoing, Vector3DX Center);
			void SetDraw(std::function<void()> doing);

			void Draw() {
				if (!m_isUpdate) { return; }
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
				BaseShadowHandle.DrawGraph(0, 0, true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

				//DepthBaseScreenHandle.DrawExtendGraph(0, 0, 960, 960, false);
			}
		};
	public:
		int							m_DispXSize{deskx};
		int							m_DispYSize{desky};
	private:
		switchs						m_PauseActive;
		LONGLONG					m_StartTime{0};
		bool						m_IsEnd{false};

		std::array<ShadowControl, 3> m_Shadow;
		ShadowDraw					m_ShadowDraw;
		Vector3DX					m_LightVec;
		COLOR_F						m_LightColorF{GetColorF(0, 0, 0, 0)};
		GraphHandle					m_OutScreen;							//スクリーンバッファ
		GraphHandle					UI_Screen;								//UI
		Camera3DInfo				m_MainCamera;							//カメラ
		//カメラシェイク
		bool						m_SendCamShake{false};
		float						m_SendCamShakeTime{1.f};
		float						m_SendCamShakePower{1.f};
		float						m_CamShake{0.f};
		Vector3DX					m_CamShake1;
		Vector3DX					m_CamShake2;
		//
		float						m_AberrationPower{1.f};
		//
		PopUpDrawClass				m_PopUpDrawClass;
		PopUpDrawClass				m_RestartPopUpDrawClass;
		bool						m_PrevPausePopupOpen{false};
		float						m_PauseFlashCount{0.f};

		ShaderUseClass::ScreenVertex	m_ScreenVertex;								// 頂点データ
		std::array<ShaderUseClass, 2>	m_Shader2D;
		std::array<shaderparam, 2> m_ShaderParam;		//シェーダーパラメーター
	public://ゲッター
		const auto&		is_lens(void) const noexcept { return m_ShaderParam[0].use; }
		const auto&		zoom_lens(void) const noexcept { return m_ShaderParam[0].param[3]; }

		void			Set_is_lens(bool value) noexcept { m_ShaderParam[0].use = value; }
		void			Set_xp_lens(float value) noexcept { m_ShaderParam[0].param[0] = value; }
		void			Set_yp_lens(float value) noexcept { m_ShaderParam[0].param[1] = value; }
		void			Set_size_lens(float value) noexcept { m_ShaderParam[0].param[2] = value; }
		void			Set_zoom_lens(float value) noexcept { m_ShaderParam[0].param[3] = value; }

		void			Set_is_Blackout(bool value) noexcept { m_ShaderParam[1].use = value; }
		void			Set_Per_Blackout(float value) noexcept { m_ShaderParam[1].param[0] = value; }

		void			GetShadowAfterDraw() { m_ShadowDraw.Draw(); }
	private://コンストラクタ
		DXDraw(void) noexcept;
		~DXDraw(void) noexcept;
	public:
		const auto		IsExit() const noexcept { return m_PopUpDrawClass.IsActive(); }
		const auto		IsRestart() const noexcept { return m_RestartPopUpDrawClass.IsActive(); }
		const auto		IsPause() const noexcept { return m_PauseActive.on(); }
		void			PauseIn() noexcept;
		void			PauseExit() noexcept;
	public:
		auto&			SetMainCamera(void) noexcept { return m_MainCamera; }
		const auto&		GetMainCamera(void) const noexcept { return m_MainCamera; }
	public:
		void			SetAberrationPower(float value) noexcept { m_AberrationPower = value; }
		const auto&		GetAberrationPower(void) const noexcept { return m_AberrationPower; }
	public:
		void			SetAmbientLight(const Vector3DX& AmbientLightVec, const COLOR_F& LightColor) noexcept;
		void			SetShadow(const Vector3DX& Vec, const Vector3DX& MinSize, const Vector3DX& MaxSize, int shadowSelect) noexcept {
			if (shadowSelect == 0) {
				m_ShadowDraw.SetVec(Vec);
			}
			else {
				m_Shadow[shadowSelect].Set(Vec, MinSize, MaxSize);
			}
		}
		void			SetIsUpdateShadow(int shadowSelect, bool value) noexcept {
			if (shadowSelect == 0) {
				m_ShadowDraw.SetisUpdate(value);
			}
			else {
				m_Shadow[shadowSelect].SetisUpdate(value);
			}
		}

		void			SetUseShadow(void) noexcept {
			SetUseShadowMap(0, m_Shadow.at(1).GetHandle());
			SetUseShadowMap(1, m_Shadow.at(2).GetHandle());
		}
		void			ResetUseShadow(void) noexcept {
			SetUseShadowMap(0, -1);
			SetUseShadowMap(1, -1);
		}
		void			Update_Shadow(std::function<void()> doing, const Vector3DX& CenterPos, int shadowSelect) noexcept;

		void			Update_NearShadow(std::function<void()> doing) noexcept;
	public:
		void			SetCamShake(float time, float power) noexcept {
			this->m_SendCamShake = true;
			this->m_SendCamShakeTime = time;
			this->m_SendCamShakePower = power;
		}
		const auto&		GetCamShake(void) const noexcept { return m_CamShake2; }
	public:
		void			Init(void) noexcept;
		bool			FirstExecute(void) noexcept;
		void			Execute(void) noexcept;
		void			Draw(
			std::function<void(const Camera3DInfo&)> doing,
			std::function<void()> doingUI,
			std::function<void()> doingUI2
		) noexcept;
		bool			Screen_Flip(void) noexcept;

		//VR
	private:
		class VRControl;
		VRControl*			m_VRControl;
		VRControl*			GetVRControl() noexcept { return m_VRControl; }
		const VRControl*	GetVRControl() const noexcept { return m_VRControl; }
	public:
		void				Get_VR_HMDPositionVR(Vector3DX* pos_, Matrix4x4DX*mat) noexcept;
		void				Reset_VR_HMD(void) noexcept;
		bool				Get_VR_Hand1PadPress(VR_PAD) const noexcept;
		bool				Get_VR_Hand1TouchPress(VR_PAD) const noexcept;
		Vector3DX			Get_VR_Hand1TouchPadPoint() const noexcept;
		bool				Get_VR_Hand2PadPress(VR_PAD) const noexcept;
		bool				Get_VR_Hand2TouchPress(VR_PAD) const noexcept;
		Vector3DX			Get_VR_Hand2TouchPadPoint() const noexcept;
		void				VR_Haptic(char id_, unsigned short times) noexcept;	//VRコントローラー振動
	};

};

#include "DXLib_ref_100_PostPass.hpp"
#include "DXLib_ref_101_sequence.hpp"
#include "DXLib_ref_102_ObjectManager.hpp"
