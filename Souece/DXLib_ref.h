#pragma once

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*�r���h�ݒ�																																	*/
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

// �G���[,�x�����
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

// ����
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
// �ǉ���
#include "json.hpp"

// #pragma warning( pop )
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*const																																		*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
constexpr float		FrameRate{ 60.f };								/*�����̊�ɂȂ�t���[�����[�g*/
constexpr float		Scale3DRate{ 12.5f };							/*1m�ɑ�������3D��ԏ�̒���*/
constexpr float		GravityRate{ -9.8f * Scale3DRate };				/*�d�͉����x*/
constexpr int		InvalidID{ -1 };								/*���ʂ̖����l*/
constexpr int		BaseScreenWidth{ 1920 };						/*UI�`��Ȃǂ̊�ƂȂ�𑜓x*/
constexpr int		BaseScreenHeight{ 1080 };						/*UI�`��Ȃǂ̊�ƂȂ�𑜓x*/

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*include DXLIB																																*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
// DXLIB
#include "DxLib.h"
#if defined(_USE_EFFEKSEER_)
#include "EffekseerForDXLib.h"
#endif
// �ǉ�
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
/*�K�{�I�u�W�F�N�g																															*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/
namespace DXLibRef {
	class DXLib_ref : public SingletonBase<DXLib_ref> {
	private:
		friend class SingletonBase<DXLib_ref>;
	private:
		bool						m_IsFirstBoot{ false };
		LONGLONG					Update_effect_was = 0;					// �G�t�F�N�g�̃A�b�v�f�[�g�^�C�~���O�^�C�}�[

		float						m_DeltaTime{ 0.f };		//1�t���[���ɂ����������� �}�C�N���b
		LONGLONG					m_StartTime{ 0 };		// 1�t���[�����n�܂��Ă���̌o�ߎ��� �}�C�N���b

		CheckPCSpec					m_CheckPCSpec;
	private:// �R���X�g���N�^
		DXLib_ref(void) noexcept;
		DXLib_ref(const DXLib_ref&) = delete;
		DXLib_ref(DXLib_ref&& o) = delete;
		DXLib_ref& operator=(const DXLib_ref&) = delete;
		DXLib_ref& operator=(DXLib_ref&& o) = delete;

		~DXLib_ref(void) noexcept {}
	private:
		void		StartCount(void) noexcept;			// ���[�v�̍ŏ��ɒʂ�
		bool		WaitCount(void) const noexcept;		// �\��ʂɔ��f���A���������܂��͈���FPS�܂őҋ@����
	public:
		const auto& GetDeltaTime(void) const noexcept { return m_DeltaTime; }		// 1�t���[���ɂ����������Ԃ��擾
		const auto	GetFps(void) const noexcept { return 1.f / m_DeltaTime; }		// FPS�l�̎擾
	public:
		void		SetWaitVSync(void) noexcept;		//���������̃t���O�ݒ�
	public:
		bool		FirstBootSetting(void) noexcept;
		void		StartLogic(void) noexcept;
		void		MainLogic(void) noexcept;
	};
};
