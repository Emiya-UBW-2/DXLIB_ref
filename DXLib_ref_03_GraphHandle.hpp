/*=============================================================================
 Copyright (C) 2020 yumetodo <yume-wikijp@live.jp>
 Distributed under the Boost Software License, Version 1.0.
 (See https://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	class Camera3DInfo {
		Vector3DX	m_pos;
		Vector3DX	m_vec;
		Vector3DX	m_up{ Vector3DX::up() };	//カメラ
		float		m_fov{ DX_PI_F / 2 };		//カメラ
		float		m_near{ 0.1f };
		float		m_far{ 10.f };	//ニアファー
	public:
		const auto&			GetCamPos(void)const noexcept { return m_pos; }
		const auto&			GetCamVec(void)const noexcept { return m_vec; }
		const auto&			GetCamUp(void)const noexcept { return m_up; }
		const auto&			GetCamFov(void)const noexcept { return m_fov; }
		const auto&			GetCamNear(void)const noexcept { return m_near; }
		const auto&			GetCamFar(void)const noexcept { return m_far; }
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

		const MATRIX GetViewMatrix(void) const noexcept {
			MATRIX mat_view;					// ビュー行列
			VECTOR vec_from = m_pos.get();		// カメラの位置
			VECTOR vec_lookat = m_vec.get();  // カメラの注視点
			VECTOR vec_up = m_up.get();    // カメラの上方向
			CreateLookAtMatrix(&mat_view, &vec_from, &vec_lookat, &vec_up);
			return mat_view;
		}

		const MATRIX GetProjectionMatrix(void) const noexcept {
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
	class GraphHandle {
	private:
		int handle_;
		constexpr GraphHandle(int h) noexcept : handle_(h) {}
		static constexpr int invalid_handle = InvalidID;

	public:
		constexpr GraphHandle(void) noexcept : handle_(invalid_handle) {}
		GraphHandle(const GraphHandle&) = delete;
		GraphHandle(GraphHandle&& o) noexcept : handle_(o.handle_) { o.handle_ = invalid_handle; }
		GraphHandle& operator=(const GraphHandle&) = delete;
		GraphHandle& operator=(GraphHandle&& o) noexcept {
			this->handle_ = o.handle_;
			o.handle_ = invalid_handle;
			return *this;
		}
		void operator=(int& hand) noexcept { this->handle_ = hand; }

		~GraphHandle(void) noexcept {
			Dispose();
		}
		bool IsActive(void) const noexcept { return this->handle_ != invalid_handle; }
		void Dispose(void) noexcept {
			if (IsActive()) {
				DeleteGraph(this->handle_);
				this->handle_ = invalid_handle;
			}
		}
		int get(void) const noexcept { return handle_; }

		static GraphHandle Load_Tex(std::basic_string_view<TCHAR> FileName) noexcept {
			return { DxLib::MV1LoadTextureWithStrLen(FileName.data(), FileName.length()) };
		}
		static GraphHandle Load(std::basic_string_view<TCHAR> FileName, bool NotUse3DFlag = false) noexcept {
			return { DxLib::LoadGraphWithStrLen(FileName.data(), FileName.length(), NotUse3DFlag) };
		}
		static GraphHandle LoadDiv(std::basic_string_view<TCHAR> FileName, int AllNum, int XNum, int YNum, int XSize, int YSize, int* HandleArray, bool NotUse3DFlag = false) noexcept {
			return { DxLib::LoadDivGraphWithStrLen(FileName.data(), FileName.length(), AllNum, XNum, YNum,  XSize, YSize, HandleArray, NotUse3DFlag) };
		}
		static GraphHandle DerivationGraph(int x, int y, int xsize, int ysize, const GraphHandle& baseImage) noexcept {
			return { DxLib::DerivationGraph(x,y,xsize,ysize,baseImage.get()) };
		}

		static GraphHandle Make(int SizeX, int SizeY, bool trns = false) noexcept {
			return { DxLib::MakeScreen(SizeX, SizeY, (trns ? TRUE : FALSE)) };
		}

		static GraphHandle MakeDepth(int SizeX, int SizeY) noexcept {
			// 深度を描画するテクスチャの作成( 2チャンネル浮動小数点32ビットテクスチャ )
			auto prevMip = GetCreateDrawValidGraphChannelNum();
			auto prevFloatType = GetDrawValidFloatTypeGraphCreateFlag();
			auto prevBit = GetCreateGraphChannelBitDepth();
			SetCreateDrawValidGraphChannelNum(2);
			SetDrawValidFloatTypeGraphCreateFlag(TRUE);
			SetCreateGraphChannelBitDepth(32);
			auto ret = DxLib::MakeScreen(SizeX, SizeY, FALSE);
			SetCreateDrawValidGraphChannelNum(prevMip);
			SetDrawValidFloatTypeGraphCreateFlag(prevFloatType);
			SetCreateGraphChannelBitDepth(prevBit);
			return ret;
		}

		void DrawGraph(int posx, int posy, bool trns) const noexcept {
			if (IsActive()) {
				DxLib::DrawGraph(posx, posy, this->handle_, (trns ? TRUE : FALSE));
			}
		}

		void DrawRotaGraph(int posx, int posy, float Exrate, float rad, bool trns) const noexcept {
			if (IsActive()) {
				DxLib::DrawRotaGraph(posx, posy, static_cast<double>(Exrate), static_cast<double>(rad), this->handle_, (trns ? TRUE : FALSE));
			}
		}

		void DrawRotaGraphF(float posx, float posy, float Exrate, float rad, bool trns) const noexcept {
			if (IsActive()) {
				DxLib::DrawRotaGraphF(posx, posy, static_cast<double>(Exrate), static_cast<double>(rad), this->handle_, (trns ? TRUE : FALSE));
			}
		}

		void DrawRotaGraph3(int posx, int posy,
			int cx, int cy,
			float ExtRateX, float ExtRateY, float rad, bool trns) const noexcept {
			if (IsActive()) {
				DxLib::DrawRotaGraph3(posx, posy, cx, cy, static_cast<double>(ExtRateX), static_cast<double>(ExtRateY), static_cast<double>(rad), this->handle_, (trns ? TRUE : FALSE));
			}
		}

		void DrawExtendGraph(int posx1, int posy1, int posx2, int posy2, bool trns) const noexcept {
			if (IsActive()) {
				DxLib::DrawExtendGraph(posx1, posy1, posx2, posy2, this->handle_, (trns ? TRUE : FALSE));
			}
		}
		//GetGraphSize
		void GetSize(int* xsize, int* ysize) const noexcept {
			if (IsActive()) {
				GetGraphSize(this->handle_, xsize, ysize);
			}
		}
		//
		void SetDraw_Screen(const bool& Clear = true) const noexcept {
			SetDrawScreen(this->handle_);
			if (Clear) {
				ClearDrawScreen();
			}
		}
		//
		static void SetDraw_Screen(int handle, const bool& Clear = true) noexcept {
			SetDrawScreen(handle);
			if (Clear) {
				ClearDrawScreen();
			}
		}

		auto getp(void) noexcept { return &handle_; }
		static void LoadDiv(std::basic_string_view<TCHAR> FileName, int AllNum, int XNum, int YNum, int  XSize, int  YSize, std::vector<GraphHandle>* Handles, bool NotUse3DFlag = false) noexcept {
			int* HandleArray = new int[static_cast<size_t>(AllNum)];
			DxLib::LoadDivGraphWithStrLen(FileName.data(), FileName.length(), AllNum, XNum, YNum, XSize, YSize, HandleArray, NotUse3DFlag);

			Handles->clear();
			for (int i : std::views::iota(0, AllNum)) {
				Handles->resize(Handles->size() + 1);
				Handles->back() = HandleArray[static_cast<size_t>(i)];
			}
			delete[] HandleArray;

			return;
		}
	};
};
