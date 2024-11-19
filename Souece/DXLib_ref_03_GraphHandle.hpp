/*=============================================================================
 Copyright (C) 2020 yumetodo <yume-wikijp@live.jp>
 Distributed under the Boost Software License, Version 1.0.
 (See https:// www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	class Camera3DInfo {
		Vector3DX	m_pos;
		Vector3DX	m_vec;
		Vector3DX	m_up{ Vector3DX::up() };	// カメラ
		float		m_fov{ DX_PI_F / 2 };		// カメラ
		float		m_near{ 0.1f };
		float		m_far{ 10.f };	// ニアファー
	public:
		const auto& GetCamPos(void)const noexcept { return m_pos; }
		const auto& GetCamVec(void)const noexcept { return m_vec; }
		const auto& GetCamUp(void)const noexcept { return m_up; }
		const auto& GetCamFov(void)const noexcept { return m_fov; }
		const auto& GetCamNear(void)const noexcept { return m_near; }
		const auto& GetCamFar(void)const noexcept { return m_far; }
	public:
		void			SetCamPos(const Vector3DX& cam_pos, const Vector3DX& cam_vec, const Vector3DX& cam_up) noexcept {
			m_pos = cam_pos;
			m_vec = cam_vec;
			m_up = cam_up;
		}
		void			SetNowCamPos(void) noexcept {
			m_pos = DxLib::GetCameraPosition();
			m_vec = DxLib::GetCameraTarget();
			m_up = DxLib::GetCameraUpVector();
		}
		void			SetCamInfo(float cam_fov_, float cam_near_, float cam_far_) noexcept {
			m_fov = cam_fov_;
			m_near = cam_near_;
			m_far = cam_far_;
		}
		void FlipCamInfo(void) const noexcept {
			SetUpCamInfo(this->m_pos, this->m_vec, this->m_up, this->m_fov, this->m_near, this->m_far);
		}

		const Matrix4x4DX GetViewMatrix(void) const noexcept {
			MATRIX mat_view;					// ビュー行列
			VECTOR vec_from = m_pos.get();		// カメラの位置
			VECTOR vec_lookat = m_vec.get();  // カメラの注視点
			VECTOR vec_up = m_up.get();    // カメラの上方向
			CreateLookAtMatrix(&mat_view, &vec_from, &vec_lookat, &vec_up);
			return mat_view;
		}

		const Matrix4x4DX GetProjectionMatrix(void) const noexcept {
			MATRIX mat_view;					// プロジェクション行列
			CreatePerspectiveFovMatrix(&mat_view, m_fov, m_near, m_far);
			return mat_view;
		}

		static void SetUpCamInfo(const Vector3DX& campos, const Vector3DX& camvec, const Vector3DX& camup, float fov, float near_, float far_) noexcept {
			SetCameraNearFar(near_, far_);
			SetupCamera_Perspective(fov);
			SetCameraPositionAndTargetAndUpVec(campos.get(), camvec.get(), camup.get());
		}
	};
	class GraphHandle : public DXHandle {
	protected:
		void	Dispose_Sub(void) noexcept override {
			DeleteGraph(DXHandle::get());
		}
	public:
		void Load_Tex(std::basic_string_view<TCHAR> FileName) noexcept {
			DXHandle::SetHandleDirect(DxLib::MV1LoadTextureWithStrLen(FileName.data(), FileName.length()));
		}
		void Load(std::basic_string_view<TCHAR> FileName, bool NotUse3DFlag = false) noexcept {
			DXHandle::SetHandleDirect(DxLib::LoadGraphWithStrLen(FileName.data(), FileName.length(), NotUse3DFlag));
		}
		void LoadDiv(std::basic_string_view<TCHAR> FileName, int AllNum, int XNum, int YNum, int XSize, int YSize, int* HandleArray, bool NotUse3DFlag = false) noexcept {
			DXHandle::SetHandleDirect(DxLib::LoadDivGraphWithStrLen(FileName.data(), FileName.length(), AllNum, XNum, YNum, XSize, YSize, HandleArray, NotUse3DFlag));
		}
		void DerivationGraph(int x, int y, int xsize, int ysize, const GraphHandle& baseImage) noexcept {
			DXHandle::SetHandleDirect(DxLib::DerivationGraph(x, y, xsize, ysize, baseImage.get()));
		}
		void Make(int SizeX, int SizeY, bool trns = false) noexcept {
			DXHandle::SetHandleDirect(DxLib::MakeScreen(SizeX, SizeY, (trns ? TRUE : FALSE)));
		}
		void MakeDepth(int SizeX, int SizeY) noexcept {
			// 深度を描画するテクスチャの作成( 2チャンネル浮動小数点32ビットテクスチャ )
			auto prevMip = GetCreateDrawValidGraphChannelNum();
			auto prevFloatType = GetDrawValidFloatTypeGraphCreateFlag();
			auto prevBit = GetCreateGraphChannelBitDepth();
			SetCreateDrawValidGraphChannelNum(2);
			SetDrawValidFloatTypeGraphCreateFlag(TRUE);
			SetCreateGraphChannelBitDepth(32);
			DXHandle::SetHandleDirect(DxLib::MakeScreen(SizeX, SizeY, FALSE));
			SetCreateDrawValidGraphChannelNum(prevMip);
			SetDrawValidFloatTypeGraphCreateFlag(prevFloatType);
			SetCreateGraphChannelBitDepth(prevBit);
		}
		void CreateGraphFromBmp(const BITMAPINFO* RGBBmpInfo, const void* RGBBmpImage, const BITMAPINFO* AlphaBmpInfo = nullptr, const void* AlphaBmpImage = nullptr, bool TextureFlag = true, bool ReverseFlag = false) noexcept {
			DXHandle::SetHandleDirect(DxLib::CreateGraphFromBmp(RGBBmpInfo, RGBBmpImage, AlphaBmpInfo, AlphaBmpImage, TextureFlag ? TRUE : FALSE, ReverseFlag ? TRUE : FALSE));
		}
	public:
		void DrawGraph(int posx, int posy, bool trns) const noexcept {
			if (!DXHandle::IsActive()) { return; }
			DxLib::DrawGraph(posx, posy, DXHandle::get(), (trns ? TRUE : FALSE));
		}

		void DrawRotaGraph(int posx, int posy, float Exrate, float rad, bool trns) const noexcept {
			if (!DXHandle::IsActive()) { return; }
			DxLib::DrawRotaGraph(posx, posy, static_cast<double>(Exrate), static_cast<double>(rad), DXHandle::get(), (trns ? TRUE : FALSE));
		}

		void DrawRotaGraphF(float posx, float posy, float Exrate, float rad, bool trns) const noexcept {
			if (!DXHandle::IsActive()) { return; }
			DxLib::DrawRotaGraphF(posx, posy, static_cast<double>(Exrate), static_cast<double>(rad), DXHandle::get(), (trns ? TRUE : FALSE));
		}

		void DrawRotaGraph3(int posx, int posy,
			int cx, int cy,
			float ExtRateX, float ExtRateY, float rad, bool trns) const noexcept {
			if (!DXHandle::IsActive()) { return; }
			DxLib::DrawRotaGraph3(posx, posy, cx, cy, static_cast<double>(ExtRateX), static_cast<double>(ExtRateY), static_cast<double>(rad), DXHandle::get(), (trns ? TRUE : FALSE));
		}

		void DrawExtendGraph(int posx1, int posy1, int posx2, int posy2, bool trns) const noexcept {
			if (!DXHandle::IsActive()) { return; }
			DxLib::DrawExtendGraph(posx1, posy1, posx2, posy2, DXHandle::get(), (trns ? TRUE : FALSE));
		}
		void FillGraph(int r, int g, int b) const noexcept {
			if (!DXHandle::IsActive()) { return; }
			DxLib::FillGraph(DXHandle::get(), r, g, b);
		}

		template <typename... Args>
		void GraphFilter(int FilterType /* DX_GRAPH_FILTER_GAUSS 等 */, Args&&... args) noexcept {
			DxLib::GraphFilter(DXHandle::get(), FilterType, args...);
		}

		template <typename... Args>
		void GraphFilterBlt(const GraphHandle& targetImage, int FilterType /* DX_GRAPH_FILTER_GAUSS 等 */, Args&&... args) noexcept {
			DxLib::GraphFilterBlt(targetImage.get(), DXHandle::get(), FilterType, args...);
		}

		template <typename... Args>
		void GraphBlend(const GraphHandle& BlendImage, int BlendRatio /* ブレンド効果の影響度( 0:０％  255:１００％ ) */, int BlendType /* DX_GRAPH_BLEND_ADD 等 */, Args&&... args) noexcept {
			DxLib::GraphBlend(DXHandle::get(), BlendImage.get(), BlendRatio, BlendType, args...);
		}

		template <typename... Args>
		void GraphBlendBlt(const GraphHandle& BaseImage, const GraphHandle& BlendImage, int BlendRatio /* ブレンド効果の影響度( 0:０％  255:１００％ ) */, int BlendType /* DX_GRAPH_BLEND_ADD 等 */, Args&&... args) noexcept {
			DxLib::GraphBlendBlt(BaseImage.get(), BlendImage.get(), DXHandle::get(), BlendRatio, BlendType, args...);
		}


		// GetGraphSize
		void GetSize(int* xsize, int* ysize) const noexcept {
			if (!DXHandle::IsActive()) { return; }
			GetGraphSize(DXHandle::get(), xsize, ysize);
		}
		// 
		void SetDraw_Screen(const bool& Clear = true) const noexcept {
			SetDrawScreen(DXHandle::get());
			if (Clear) {
				ClearDrawScreen();
			}
		}
		// 
		void SetUseTextureToShader(int ID) const noexcept {
			DxLib::SetUseTextureToShader(ID, DXHandle::get());
		}
		void SetRenderTargetToShader(int ID, int SurfaceIndex = 0, int MipLevel = 0) const noexcept {
			DxLib::SetRenderTargetToShader(ID, DXHandle::get(), SurfaceIndex, MipLevel);
		}
	public:
		// 
		static void SetDraw_Screen(int handle, const bool& Clear = true) noexcept {
			SetDrawScreen(handle);
			if (Clear) {
				ClearDrawScreen();
			}
		}
		// 
		/*
		static void SetUseTextureToShader(int ID, int handle) noexcept {
			DxLib::SetUseTextureToShader(ID, handle);
		}
		//
		static void SetRenderTargetToShader(int ID, int handle, int SurfaceIndex = 0, int MipLevel = 0) noexcept {
			DxLib::SetRenderTargetToShader(ID, handle, SurfaceIndex, MipLevel);
		}
		//*/

		static void LoadDiv(std::basic_string_view<TCHAR> FileName, int AllNum, int XNum, int YNum, int  XSize, int  YSize, std::vector<GraphHandle>* Handles, bool NotUse3DFlag = false) noexcept {
			int* HandleArray = new int[static_cast<size_t>(AllNum)];
			DxLib::LoadDivGraphWithStrLen(FileName.data(), FileName.length(), AllNum, XNum, YNum, XSize, YSize, HandleArray, NotUse3DFlag);

			Handles->clear();
			for (int i : std::views::iota(0, AllNum)) {
				Handles->emplace_back();
				Handles->back().SetHandleDirect(HandleArray[static_cast<size_t>(i)]);
			}
			delete[] HandleArray;

			return;
		}
	};
};
