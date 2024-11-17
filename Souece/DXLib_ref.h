#pragma once

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*ビルド設定																																	*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
//#define DEBUG
//#define _USE_OPENVR_
//#define _USE_BOX2D_
#define _USE_EFFEKSEER_

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*pragma																																	*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/

#if defined(_USE_OPENVR_)

#pragma comment(lib,"..\\..\\..\\openvr\\openvr_api.lib")

#endif // _USE_OPENVR_

#if defined(_USE_BOX2D_)

#if defined(DEBUG)
#pragma comment(lib,"..\\..\\..\\Box2D\\Debug\\Box2D.lib")
#else
#pragma comment(lib,"..\\..\\..\\Box2D\\Release\\Box2D.lib")
#endif // _DEBUG

#endif // _USE_BOX2D_

// エラー,警告取り
#define NOMINMAX
/*
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5045)
//*/
#pragma warning(disable:4505)
/*
#pragma warning(disable:4514)
#pragma warning(disable:4820)
#pragma warning(disable:5259)
//*/

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*include																																	*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
// VR
#if defined(_USE_OPENVR_)
#include <openvr.h>
#endif // _USE_OPENVR_
// Box2D
#if defined(_USE_BOX2D_)
#include "Box2D/Box2D.h"
#endif // _USE_BOX2D_

// 共通
// #pragma warning( push, 3 )

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
#include <thread>
#pragma comment(lib, "shlwapi.lib")
// 追加物
#include "json.hpp"

// #pragma warning( pop )
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*const																																		*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
constexpr float		FrameRate{ 60.f };								/*処理の基準になるフレームレート*/
constexpr float		Scale3DRate{ 12.5f };							/*1mに相当する3D空間上の長さ*/
constexpr float		GravityRate{ -9.8f * Scale3DRate };				/*重力加速度*/
constexpr int		InvalidID{ -1 };								/*共通の無効値*/
constexpr int		BaseScreenWidth{ 1920 };						/*UI描画などの基準となる解像度*/
constexpr int		BaseScreenHeight{ 1080 };						/*UI描画などの基準となる解像度*/

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*include DXLIB																																*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
// DXLIB
#include "DxLib.h"
#if defined(_USE_EFFEKSEER_)
#include "EffekseerForDXLib.h"
#endif
// 追加
#include "DXLib_ref_00_Util.hpp"
#include "DXLib_ref_01_Algorithm.hpp"
#include "DXLib_ref_02_SoundHandle.hpp"
#include "DXLib_ref_03_GraphHandle.hpp"
#include "DXLib_ref_04_LightHandle.hpp"
#include "DXLib_ref_05_FontHandle.hpp"
#include "DXLib_ref_06_MV1Handle.hpp"
#include "DXLib_ref_07_EffekseerEffectHandle.hpp"
#include "DXLib_ref_08_Etc.hpp"
#include "DXLib_ref_09_KeyControl.hpp"
#include "DXLib_ref_10_Option.hpp"
#include "DXLib_ref_11_FileControl.hpp"
#include "DXLib_ref_12_Draw2DSystem.hpp"
#include "DXLib_ref_20_Debug.hpp"
#include "DXLib_ref_30_Box2D.hpp"
#include "DXLib_ref_31_VR.hpp"
#include "DXLib_ref_40_WindowSizeControl.hpp"
#include "DXLib_ref_100_PostPass.hpp"
#include "DXLib_ref_101_sequence.hpp"
#include "DXLib_ref_102_ObjectManager.hpp"
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*必須オブジェクト																															*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
namespace DXLibRef {
	class DXLib_ref : public SingletonBase<DXLib_ref> {
	private:
		friend class SingletonBase<DXLib_ref>;
	private:
		bool						m_IsFirstBoot{ false };
		LONGLONG					Update_effect_was = 0;					// エフェクトのアップデートタイミングタイマー

		float						m_DeltaTime{ 0.f };		//1フレームにかかった時間 マイクロ秒
		LONGLONG					m_StartTime{ 0 };		// 1フレームが始まってからの経過時間 マイクロ秒

		CheckPCSpec					m_CheckPCSpec;
	private:// コンストラクタ
		DXLib_ref(void) noexcept;
		DXLib_ref(const DXLib_ref&) = delete;
		DXLib_ref(DXLib_ref&& o) = delete;
		DXLib_ref& operator=(const DXLib_ref&) = delete;
		DXLib_ref& operator=(DXLib_ref&& o) = delete;

		~DXLib_ref(void) noexcept {}
	private:
		void		StartCount(void) noexcept;			// ループの最初に通す
		bool		WaitCount(void) const noexcept;		// 表画面に反映し、垂直同期または一定のFPSまで待機する
	public:
		const auto& GetDeltaTime(void) const noexcept { return m_DeltaTime; }		// 1フレームにかかった時間を取得
		const auto	GetFps(void) const noexcept { return 1.f / m_DeltaTime; }		// FPS値の取得
	public:
		void		SetWaitVSync(void) noexcept;		//垂直同期のフラグ設定
	public:
		bool		FirstBootSetting(void) noexcept;
		void		StartLogic(void) noexcept;
		void		MainLogic(void) noexcept;
	};
};
