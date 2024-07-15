#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	enum class LightType {
		POINT = DX_LIGHTTYPE_POINT,
		SPOT = DX_LIGHTTYPE_SPOT,
		DIRECTIONAL = DX_LIGHTTYPE_DIRECTIONAL,
	};
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

		static LightHandle CreateSpot(const Vector3DX& Position, const Vector3DX& Direction, float OutAngle, float InAngle, float Range, float Atten0, float Atten1, float Atten2) {
			return { DxLib::CreateSpotLightHandle(Position.get(), Direction.get(), OutAngle, InAngle, Range, Atten0, Atten1, Atten2) };
		}

		static LightHandle CreatePoint(const Vector3DX& Position, float Range, float Atten0, float Atten1, float Atten2) {
			return { DxLib::CreatePointLightHandle(Position.get(), Range, Atten0, Atten1, Atten2) };
		}

		static LightHandle CreateDir(const Vector3DX& Directional) {
			return {DxLib::CreateDirLightHandle(Directional.get())};
		}
	};
	//ライトプール
	class LightPool : public SingletonBase<LightPool> {
	private:
		friend class SingletonBase<LightPool>;
	private:
		std::array<LightHandle, 5> handles;
		int now = 0;
	private:
		LightPool(void) noexcept {}
		LightPool(const LightPool&) = delete;
		LightPool(LightPool&& o) = delete;
		LightPool& operator=(const LightPool&) = delete;
		LightPool& operator=(LightPool&& o) = delete;

		~LightPool(void) noexcept {}
	public:
		const LightHandle&			Put(LightType Lighttype, const Vector3DX& pos) noexcept {
			auto prev = now;
			if (handles[static_cast<size_t>(now)].get() != -1) {
				handles[static_cast<size_t>(now)].Dispose();
			}
			//handles[static_cast<size_t>(now)].time = GetNowHiPerformanceCount();
			switch (Lighttype) {
				case LightType::POINT:
					handles[static_cast<size_t>(now)] = LightHandle::CreatePoint(pos, 2.5f, 0.5f, 1.5f, 0.5f);
					break;
				case LightType::SPOT:
					handles[static_cast<size_t>(now)] = LightHandle::CreateSpot(pos, Vector3DX::down(), DX_PI_F / 2, DX_PI_F / 4, 2.5f, 0.5f, 1.5f, 0.5f);
					break;
				case LightType::DIRECTIONAL:
					handles[static_cast<size_t>(now)] = LightHandle::CreateDir(pos);
					break;
				default:
					break;
			}
			++now %= handles.size();
			return handles[static_cast<size_t>(prev)];
		}
		void			Update(void) noexcept {
			/*
			for (auto& h : handles) {
				if (h.get() != -1) {
					if ((GetNowHiPerformanceCount() - h.time) >= 1000000 / 30) {
						h.Dispose();
					}
				}
			}
			//*/
		}
	};
};
