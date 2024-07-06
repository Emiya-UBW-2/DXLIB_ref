#pragma once

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*ビルド設定																																	*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
#define DEBUG
//#define _USE_OPENVR_
//#define _USE_BOX2D_

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
#pragma warning(disable:4505)

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
	public://ゲッター
	private://コンストラクタ
		DXLib_ref(void) noexcept;
		~DXLib_ref(void) noexcept;
	public:
		bool StartLogic() noexcept;
		bool MainLogic() noexcept;
	};
};
