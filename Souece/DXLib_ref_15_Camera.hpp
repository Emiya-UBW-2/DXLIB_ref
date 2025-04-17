#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// カメラシェイク
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class Camera3D : public SingletonBase<Camera3D> {
	private:
		friend class SingletonBase<Camera3D>;
	private:
		bool						m_SendCamShake{ false };
		float						m_SendCamShakeTime{ 1.f };
		float						m_SendCamShakePower{ 1.f };
		float						m_CamShake{ 0.f };
		Vector3DX					m_CamShake1;
		Vector3DX					m_CamShake2;

		Camera3DInfo				m_MainCamera;				// カメラ
	public:
		const auto& GetMainCamera(void) const noexcept { return this->m_MainCamera; }
		auto& SetMainCamera(void) noexcept { return this->m_MainCamera; }
	public:
		const auto& GetCamShake(void) const noexcept { return this->m_CamShake2; }
	public:
		void			SetCamShake(float time, float power) noexcept {
			this->m_SendCamShake = true;
			this->m_SendCamShakeTime = time;
			this->m_SendCamShakePower = power;
		}
	private:
		Camera3D(void) noexcept {}
		Camera3D(const Camera3D&) = delete;
		Camera3D(Camera3D&& o) = delete;
		Camera3D& operator=(const Camera3D&) = delete;
		Camera3D& operator=(Camera3D&& o) = delete;

		virtual ~Camera3D(void) noexcept {}
	public:
		void Update(void) noexcept;
	};
}
