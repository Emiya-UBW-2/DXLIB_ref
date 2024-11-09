#include "DXLib_ref.h"
// #include "DXLib_ref_08_Etc.hpp"

namespace DXLibRef {
	const SaveDataClass* SingletonBase<SaveDataClass>::m_Singleton = nullptr;
	const CameraShake* SingletonBase<CameraShake>::m_Singleton = nullptr;

	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	float GetEasingRatio(EasingType EasingType, float ratio) noexcept {
		auto* DXLib_refParts = DXLib_ref::Instance();
		switch (EasingType) {
		case EasingType::OutExpo:
			return (1.f - std::powf(ratio, FrameRate * DXLib_refParts->GetDeltaTime()));
		default:
			return 1.f;
		}
	}
	bool IntoMouse(int x1, int y1, int x2, int y2) noexcept {
		auto* Pad = PadControl::Instance();
		int mx = Pad->GetMS_X();
		int my = Pad->GetMS_Y();
		return HitPointToRectangle(mx, my, x1, y1, x2, y2);
	}
	static Vector3DX GetScreenPos(const Vector3DX& campos, const Vector3DX& camvec, const Vector3DX& camup, float fov, float near_t, float far_t, const Vector3DX& worldpos) noexcept {
		auto* WindowSizeParts = WindowSizeControl::Instance();
		// ビュー行列と射影行列の取得
		Camera3DInfo tmpcam;
		tmpcam.SetCamPos(campos, camvec, camup);
		tmpcam.SetCamInfo(fov, near_t, far_t);
		// ビューポート行列（スクリーン行列）の作成
		float w = static_cast<float>(WindowSizeParts->GetScreenXMax()) / 2.0f;
		float h = static_cast<float>(WindowSizeParts->GetScreenYMax()) / 2.0f;
		MATRIX viewport = {
			w , 0 , 0 , 0 ,
			0 ,-h , 0 , 0 ,
			0 , 0 , 1 , 0 ,
			w , h , 0 , 1
		};
		// ビュー変換とプロジェクション変換
		Vector3DX tmp = Matrix4x4DX::Vtrans(Matrix4x4DX::Vtrans(worldpos, tmpcam.GetViewMatrix()), tmpcam.GetProjectionMatrix());
		// zで割って-1~1の範囲に収める
		tmp /= tmp.z;
		// スクリーン変換
		Vector3DX screenPos = Matrix4x4DX::Vtrans(tmp, viewport);
		// 深度チェックを簡素に追加
		if (Vector3DX::Dot(camvec - campos, worldpos - campos) > 0.f) {
			screenPos.z = 0.5f;
		}
		else {
			screenPos.z = -1.f;
		}
		return screenPos;
	}
	// 
	void			switchs::Execute(bool key) noexcept {
		auto* DXLib_refParts = DXLib_ref::Instance();
		m_press = key;
		if (m_press) {
			m_presscount = std::clamp<int8_t>(m_presscount + 1, 0, 2);

			m_repeat = trigger();
			m_repeatcount -= DXLib_refParts->GetDeltaTime();
			if (m_repeatcount <= 0.f) {
				m_repeatcount += 2.f / 60.f;
				m_repeat = true;
			}
		}
		else {
			m_presscount = std::clamp<int8_t>(m_presscount - 1, 0, 2);

			m_repeat = false;
			m_repeatcount = 0.5f;
		}
		if (trigger()) {
			m_on ^= 1;
		}
	}
	// 
	void			Pendulum2D::Update(void) noexcept {
		auto* DXLib_refParts = DXLib_ref::Instance();
		m_vel += (-9.8f / this->m_PendulumLength * std::sin(m_rad) - this->m_drag_coeff / this->m_PendulumMass * this->m_vel) * DXLib_refParts->GetDeltaTime();
		m_rad += this->m_vel * DXLib_refParts->GetDeltaTime();
	}
	// 
	void CameraShake::Update(void) noexcept {
		auto* DXLib_refParts = DXLib_ref::Instance();
		if (m_SendCamShakeTime > 0.f) {
			if (m_SendCamShake) {
				m_SendCamShake = false;
				this->m_CamShake = m_SendCamShakeTime;
			}
			auto RandRange = this->m_CamShake / m_SendCamShakeTime * m_SendCamShakePower;
			Easing(&this->m_CamShake1, Vector3DX::vget(GetRandf(RandRange), GetRandf(RandRange), GetRandf(RandRange)), 0.8f, EasingType::OutExpo);
			Easing(&this->m_CamShake2, this->m_CamShake1, 0.8f, EasingType::OutExpo);
			this->m_CamShake = std::max(this->m_CamShake - DXLib_refParts->GetDeltaTime(), 0.f);
		}
	}
	// 
};
