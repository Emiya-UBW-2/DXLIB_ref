#pragma once
#include "DXLib_ref.h"

namespace DXLib_ref {
	class LightHandle {
	private:
		int handle_;
		constexpr LightHandle(int h) : handle_(h) {}
		static constexpr int invalid_handle = -1;

	public:
		constexpr LightHandle(void) : handle_(invalid_handle) {}
		LightHandle(const LightHandle&) = delete;
		LightHandle(LightHandle&& o) noexcept : handle_(o.handle_) {
			o.handle_ = invalid_handle;
		}
		LightHandle& operator=(const LightHandle&) = delete;
		LightHandle& operator=(LightHandle&& o) noexcept {
			this->handle_ = o.handle_;
			o.handle_ = invalid_handle;
			return *this;
		}
		LightHandle& operator=(int handle) {
			this->handle_ = handle;
			return *this;
		}

		~LightHandle(void) noexcept {
			Dispose();
		}
		void Dispose(void) noexcept {
			if (this->handle_ != invalid_handle) {
				DeleteLightHandle(this->handle_);
				this->handle_ = invalid_handle;
			}
		}
		int get(void) const noexcept { return handle_; }

		LightHandle Duplicate(void) const noexcept { return this->handle_; }
		void SetPos(const Vector3DX& Position) {
			SetLightPositionHandle(this->handle_, Position.get());
		}
		void SetPos(const Vector3DX& Position, const Vector3DX& Direction) {
			SetLightPositionHandle(this->handle_, Position.get());
			SetLightDirectionHandle(this->handle_, Direction.get());
		}

		static LightHandle Create(const Vector3DX& Position, const Vector3DX& Direction, float OutAngle, float InAngle, float Range, float Atten0, float Atten1, float Atten2) {
			return { DxLib::CreateSpotLightHandle(Position.get(), Direction.get(), OutAngle, InAngle, Range, Atten0, Atten1, Atten2) };
		}

		static LightHandle Create(const Vector3DX& Position, float Range, float Atten0, float Atten1, float Atten2) {
			return { DxLib::CreatePointLightHandle(Position.get(), Range, Atten0, Atten1, Atten2) };
		}
	};
	//ライトプール
	class LightPool : public SingletonBase<LightPool> {
	private:
		friend class SingletonBase<LightPool>;
	private:
		class Lights {
		public:
			LightHandle handle;
			LONGLONG time{ 0 };
		};
		std::array<Lights, 2> handles;
		int now = 0;
		Vector3DX campos;
	public:
		void			Put(const Vector3DX& pos) noexcept {
			if ((pos - campos).magnitude() >= 10.f) { return; }
			if (handles[now].handle.get() != -1) {
				handles[now].handle.Dispose();
			}
			handles[now].time = GetNowHiPerformanceCount();
			handles[now].handle = LightHandle::Create(pos, 2.5f, 0.5f, 1.5f, 0.5f);
			SetLightDifColorHandle(handles[now].handle.get(), GetColorF(1.f, 1.f, 0.f, 1.f));
			++now %= handles.size();
		}
		void			Update(const Vector3DX& campos_t) noexcept {
			campos = campos_t;
			/*
			for (auto& h : handles) {
				if (h.handle.get() != -1) {
					if ((GetNowHiPerformanceCount() - h.time) >= 1000000 / 30) {
						h.handle.Dispose();
					}
				}
			}
			//*/
		}
	};
};
