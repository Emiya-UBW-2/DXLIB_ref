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

const int32_t basex{1920 / 1};
const int32_t basey{1080 / 1};

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
		class ShadowDraw {
			GraphHandle	BaseShadowHandle;			// モデルハンドル
			GraphHandle DepthBaseScreenHandle;
			GraphHandle DepthScreenHandle;
			GraphHandle DepthFarScreenHandle;
			// 深度記録画像を使ったディレクショナルライト一つ付きの描画用の剛体メッシュ用とスキニングメッシュ用のシェーダー
			ShaderUseClass					m_Shader_Skin4_DepthShadow_Step2;
			ShaderUseClass					m_Shader_Normal_DepthShadow_Step2;
			Vector3DX		m_ShadowVec{Vector3DX::up()};
		public:
			void			SetVec(const Vector3DX& Vec) noexcept { m_ShadowVec = Vec; }

			void Init(int ShadowMapSize, int dispsizex, int dispsizey);
			void Update(std::function<void()> Shadowdoing, Vector3DX Center);
			void UpdateFar(std::function<void()> Shadowdoing, Vector3DX Center);
			void SetDraw(std::function<void()> doing, Camera3DInfo tmp_cam);
			void Draw();
			void Dispose();
		private:
			void SetupCam(Vector3DX Center, float scale);
		};
	private:
		int							m_DispXSize{deskx};
		int							m_DispYSize{desky};

		int							m_DispXSize_Win{deskx };
		int							m_DispYSize_Win{desky };

		int							m_DispXSize_Border{deskx};
		int							m_DispYSize_Border{desky};

		int							m_DispXSize_Max{deskx};
		int							m_DispYSize_Max{desky};

		float						m_FPS{60.f};

		switchs						m_PauseActive;
		LONGLONG					m_StartTime{0};
		bool						m_IsEnd{false};

		bool						m_PrevShadow{false};

		ShadowDraw					m_ShadowDraw;

		Vector3DX					m_LightVec;
		COLOR_F						m_LightColorF{GetColorF(0, 0, 0, 0)};
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
		bool						m_IsExitSelect{ false };
		bool						m_IsRestartSelect{ false };
		float						m_PauseFlashCount{0.f};

		ShaderUseClass::ScreenVertex	m_ScreenVertex;						// 頂点データ
		std::array<ShaderUseClass, 2>	m_Shader2D;
		std::array<shaderparam, 2>		m_ShaderParam;						//シェーダーパラメーター

		LONGLONG					Update_effect_was = 0;					//エフェクトのアップデートタイミングタイマー
		bool						m_IsFirstBoot{false};

		bool						m_IsCubeMap{true};
		RealTimeCubeMap				m_RealTimeCubeMap;

		CheckPCSpec					m_CheckPCSpec;
		ShaderUseClass				m_PBR_Shader;
	public://ゲッター
		const auto&		GetDispXSize(void) const noexcept { return m_DispXSize; }
		const auto&		GetDispYSize(void) const noexcept { return m_DispYSize; }

		const auto&		GetDispXSizeMin(void) const noexcept { return m_DispXSize_Win; }
		const auto&		GetDispYSizeMin(void) const noexcept { return m_DispYSize_Win; }
		const auto&		GetDispXSizeMax(void) const noexcept { return m_DispXSize_Max; }
		const auto&		GetDispYSizeMax(void) const noexcept { return m_DispYSize_Max; }

		const auto&		GetFps(void) const noexcept { return m_FPS; }

		const auto&		is_lens(void) const noexcept { return m_ShaderParam[0].use; }
		const auto&		zoom_lens(void) const noexcept { return m_ShaderParam[0].param[3]; }

		void			Set_is_lens(bool value) noexcept { m_ShaderParam[0].use = value; }
		void			Set_xp_lens(float value) noexcept { m_ShaderParam[0].param[0] = value; }
		void			Set_yp_lens(float value) noexcept { m_ShaderParam[0].param[1] = value; }
		void			Set_size_lens(float value) noexcept { m_ShaderParam[0].param[2] = value; }
		void			Set_zoom_lens(float value) noexcept { m_ShaderParam[0].param[3] = value; }

		void			Set_is_Blackout(bool value) noexcept { m_ShaderParam[1].use = value; }
		void			Set_Per_Blackout(float value) noexcept { m_ShaderParam[1].param[0] = value; }
	private://コンストラクタ
		DXDraw(void) noexcept;
		~DXDraw(void) noexcept;
	public:
		const auto		IsExit() const noexcept { return m_IsExitSelect; }
		const auto		IsRestart() const noexcept { return m_IsRestartSelect; }

		const auto		IsPause() const noexcept { return m_PauseActive.on(); }
		void			SetPause(bool value) noexcept;
	public:
		auto&			SetMainCamera(void) noexcept { return m_MainCamera; }
		const auto&		GetMainCamera(void) const noexcept { return m_MainCamera; }
	public:
		void			SetAberrationPower(float value) noexcept { m_AberrationPower = value; }
		const auto&		GetAberrationPower(void) const noexcept { return m_AberrationPower; }
	public:
		void			SetAmbientLight(const Vector3DX& AmbientLightVec, const COLOR_F& LightColor) noexcept;
		void			Update_Shadow(std::function<void()> doing, const Vector3DX& CenterPos, bool IsFar) noexcept;
		void			Update_CubeMap(std::function<void()> doing, const Vector3DX& CenterPos) noexcept;
	public:
		void			SetCamShake(float time, float power) noexcept {
			this->m_SendCamShake = true;
			this->m_SendCamShakeTime = time;
			this->m_SendCamShakePower = power;
		}
		const auto&		GetCamShake(void) const noexcept { return m_CamShake2; }
		const auto&		IsFirstBoot(void) const noexcept { return m_IsFirstBoot; }
		const auto&		GetCubeMapTex(void) const noexcept { return m_RealTimeCubeMap.GetCubeMapTex(); }

		void			SetWindowOrBorderless() noexcept;
	public:
		void			Init(void) noexcept;
		bool			FirstExecute(void) noexcept;
		bool			UpdateShadowActive() noexcept;
		void			Execute(void) noexcept;
		void			Draw(
			std::function<void()> sky_doing,
			std::function<void()> doing,
			std::function<void()> doingFront,
			std::function<void(float)> doingUI,
			std::function<void(float)> doingUI2
		) noexcept;
		bool			Screen_Flip(void) noexcept;

		//VR
	private:
		class VRControl;
		VRControl*			m_VRControl{nullptr};
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
	private:
	};

};

#include "DXLib_ref_100_PostPass.hpp"
#include "DXLib_ref_101_sequence.hpp"
#include "DXLib_ref_102_ObjectManager.hpp"
