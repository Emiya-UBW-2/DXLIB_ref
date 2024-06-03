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
		const auto&		GetCamPos(void)const noexcept { return m_pos; }
		const auto&		GetCamVec(void)const noexcept { return m_vec; }
		const auto&		GetCamUp(void)const noexcept { return m_up; }
		const auto&		GetCamFov(void)const noexcept { return m_fov; }
		const auto&		GetCamNear(void)const noexcept { return m_near; }
		const auto&		GetCamFar(void)const noexcept { return m_far; }
	public:
		void			SetCamPos(const Vector3DX& cam_pos, const Vector3DX& cam_vec, const Vector3DX& cam_up) {
			m_pos = cam_pos;
			m_vec = cam_vec;
			m_up = cam_up;
		}
		void			SetNowCamPos() {
			m_pos = DxLib::GetCameraPosition();
			m_vec = DxLib::GetCameraTarget();
			m_up = DxLib::GetCameraUpVector();
		}
		void			SetCamInfo(float cam_fov_, float cam_near_, float cam_far_) {
			m_fov = cam_fov_;
			m_near = cam_near_;
			m_far = cam_far_;
		}
	};
	class GraphHandle {
	private:
		int handle_;
		constexpr GraphHandle(int h) noexcept : handle_(h) {}
		static constexpr int invalid_handle = -1;

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
		static GraphHandle LoadDiv(std::basic_string_view<TCHAR> FileName, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray, bool NotUse3DFlag = false) noexcept {
			return { DxLib::LoadDivGraphWithStrLen(FileName.data(), FileName.length(), AllNum, XNum, YNum,   XSize, YSize, HandleArray, NotUse3DFlag) };
		}

		static GraphHandle Make(int SizeX, int SizeY, bool trns = false) noexcept {
			return { DxLib::MakeScreen(SizeX, SizeY, (trns ? TRUE : FALSE)) };
		}

		void DrawGraph(int posx, int posy, bool trns) const noexcept {
			if (IsActive()) {
				DxLib::DrawGraph(posx, posy, this->handle_, (trns ? TRUE : FALSE));
			}
		}

		void DrawRotaGraph(int posx, int posy, float Exrate, float rad, bool trns) const noexcept {
			if (IsActive()) {
				DxLib::DrawRotaGraph(posx, posy, double(Exrate), double(rad), this->handle_, (trns ? TRUE : FALSE));
			}
		}

		void DrawRotaGraph3(int posx, int posy,
			int cx, int cy,
			float ExtRateX, float ExtRateY, float rad, bool trns) const noexcept {
			if (IsActive()) {
				DxLib::DrawRotaGraph3(posx, posy, cx, cy, double(ExtRateX), double(ExtRateY), double(rad), this->handle_, (trns ? TRUE : FALSE));
			}
		}

		void DrawExtendGraph(int posx1, int posy1, int posx2, int posy2, bool trns) const noexcept {
			if (IsActive()) {
				DxLib::DrawExtendGraph(posx1, posy1, posx2, posy2, this->handle_, (trns ? TRUE : FALSE));
			}
		}
		//GetGraphSize
		void GetSize(int*xsize, int*ysize) const noexcept {
			if (IsActive()) {
				GetGraphSize(this->handle_, xsize, ysize);
			}
		}
		//
		void SetDraw_Screen(const bool& Clear = true) {
			SetDrawScreen(this->handle_);
			if (Clear) {
				ClearDrawScreen();
			}
		}
		void SetDraw_Screen(const Camera3DInfo& camInfo, const bool& Clear = true) {
			SetDraw_Screen(Clear);
			SetCameraNearFar(camInfo.GetCamNear(), camInfo.GetCamFar());
			SetupCamera_Perspective(camInfo.GetCamFov());
			SetCameraPositionAndTargetAndUpVec(camInfo.GetCamPos().get(), camInfo.GetCamVec().get(), camInfo.GetCamUp().get());
		}
		void SetDraw_Screen(const Vector3DX& campos, const Vector3DX& camvec, const Vector3DX& camup, float fov, float near_, float far_, const bool& Clear = true) {
			SetDraw_Screen(Clear);
			SetCameraNearFar(near_, far_);
			SetupCamera_Perspective(fov);
			SetCameraPositionAndTargetAndUpVec(campos.get(), camvec.get(), camup.get());
		}
		//
		static void SetDraw_Screen(int handle, const bool& Clear = true) {
			SetDrawScreen(handle);
			if (Clear) {
				ClearDrawScreen();
			}
		}

		static void SetDraw_Screen(int handle, const Camera3DInfo& camInfo, const bool& Clear = true) {
			SetDraw_Screen(handle, Clear);
			SetCameraNearFar(camInfo.GetCamNear(), camInfo.GetCamFar());
			SetupCamera_Perspective(camInfo.GetCamFov());
			SetCameraPositionAndTargetAndUpVec(camInfo.GetCamPos().get(), camInfo.GetCamVec().get(), camInfo.GetCamUp().get());
		}

		static void SetDraw_Screen(int handle, const Vector3DX& campos, const Vector3DX& camvec, const Vector3DX& camup, float fov, float near_, float far_, const bool& Clear = true) {
			SetDraw_Screen(handle, Clear);
			SetCameraNearFar(near_, far_);
			SetupCamera_Perspective(fov);
			SetCameraPositionAndTargetAndUpVec(campos.get(), camvec.get(), camup.get());
		}

		auto getp(void) noexcept { return &handle_; }
		static void LoadDiv(std::basic_string_view<TCHAR> FileName, int AllNum, int XNum, int YNum, int   XSize, int   YSize, std::vector<GraphHandle> *Handles, bool NotUse3DFlag = false) noexcept {
			int* HandleArray = new int[AllNum];
			DxLib::LoadDivGraphWithStrLen(FileName.data(), FileName.length(), AllNum, XNum, YNum, XSize, YSize, HandleArray, NotUse3DFlag);

			Handles->clear();
			for (int i = 0; i < AllNum; i++) {
				Handles->resize(Handles->size() + 1);
				Handles->back() = HandleArray[i];
			}
			delete[] HandleArray;

			return;
		}
	};
};
