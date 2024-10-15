#pragma once

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*ビルド設定																																	*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
#define DEBUG
//#define _USE_OPENVR_
//#define _USE_BOX2D_
#define _USE_EFFEKSEER_

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*pragma																																	*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/

#ifdef _USE_OPENVR_

#pragma comment(lib,"..\\..\\..\\openvr\\openvr_api.lib")

#endif // _USE_OPENVR_

#ifdef _USE_BOX2D_

#ifdef _DEBUG
#pragma comment(lib,"..\\..\\..\\Box2D\\Debug\\Box2D.lib")
#else
#pragma comment(lib,"..\\..\\..\\Box2D\\Release\\Box2D.lib")
#endif // _DEBUG

#endif // _USE_BOX2D_

//エラー,警告取り
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
//VR
#ifdef _USE_OPENVR_
#include <openvr.h>
#endif // _USE_OPENVR_
//Box2D
#ifdef _USE_BOX2D_
#include "Box2D/Box2D.h"
#endif // _USE_BOX2D_

//共通
//#pragma warning( push, 3 )

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
//追加物
#include "json.hpp"

//#pragma warning( pop )
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*const																																		*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
constexpr float		FrameRate{ 60.f };								/*処理の基準になるフレームレート*/
constexpr float		Scale3DRate{ 12.5f };							/*1mに相当する3D空間上の長さ*/
constexpr float		GravityRate{ -9.8f * Scale3DRate };				/*重力加速度*/
constexpr int		InvalidID{ -1 };								/*共通の無効値*/
constexpr int		BaseScreenWidth{ 1920 };						/*UI描画などの基準となる解像度*/
constexpr int		BaseScreenHeight{ 1080 };						/*UI描画などの基準となる解像度*/

//DPIを反映するデスクトップサイズ
const int deskx{ static_cast<int>(GetSystemMetrics(SM_CXSCREEN)) };
const int desky{ static_cast<int>(GetSystemMetrics(SM_CYSCREEN)) };

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*include DXLIB																																*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
//DXLIB
#include "DxLib.h"
#ifdef _USE_EFFEKSEER_
#include "EffekseerForDXLib.h"
#endif
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
#include "DXLib_ref_40_DXDraw.hpp"
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
		std::array<float, 60>		FPSAvgs{};
		int							m_FPSAvgCount = 0;
		float						m_FPSAvg{ 0.f };
		float						m_PauseFlashCount{ 0.f };
		bool						m_IsEnd{ false };
		bool						m_IsFirstBoot{ false };
	private://コンストラクタ
		DXLib_ref(void) noexcept;
		DXLib_ref(const DXLib_ref&) = delete;
		DXLib_ref(DXLib_ref&& o) = delete;
		DXLib_ref& operator=(const DXLib_ref&) = delete;
		DXLib_ref& operator=(DXLib_ref&& o) = delete;

		~DXLib_ref(void) noexcept {}
	private:
		void	UpdatePause(void) noexcept;
		void	DrawPause(void) const noexcept;
		//
		void	InitFPSCounter(void) noexcept {
			//FPS表示
			for (auto& f : FPSAvgs) {
				f = FrameRate;
			}
			m_FPSAvgCount = 0;
		}
		void	UpdateFPSCounter(void) noexcept;
		void	DrawFPSCounter(void) const noexcept;

		void	DrawUICommon(void) const noexcept;
	public:
		bool StartLogic(void) const noexcept;
		bool MainLogic(void) noexcept;
	};
};
