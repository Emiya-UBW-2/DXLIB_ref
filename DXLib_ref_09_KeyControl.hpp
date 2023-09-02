#pragma once
#include "DXLib_ref.h"

namespace DXLib_ref {
	class PadControl : public SingletonBase<PadControl> {
	private:
		friend class SingletonBase<PadControl>;
	private:
		switchs UpKey;
		switchs DownKey;
		switchs LeftKey;
		switchs RightKey;
		switchs OKKey;
		switchs NGKey;

		switchs m_ShotKey;
		switchs m_MouseWheel;
		switchs m_LookKey;

		switchs m_AccelKey;
		switchs m_BrakeKey;

		switchs m_QKey;
		switchs m_EKey;

		float LS_X{ 0.f };
		float LS_Y{ 0.f };
	private:
		bool m_IsPad{ false };
		bool m_IsUpdate{ true };
	private:
		PadControl() {
			m_IsUpdate = true;
		}
		~PadControl() {}
	public:
		void SetGuideUpdate() noexcept { m_IsUpdate = true; }
	public:
		const auto& GetUpKey() const noexcept { return UpKey; }
		const auto& GetDownKey() const noexcept { return DownKey; }
		const auto& GetLeftKey() const noexcept { return LeftKey; }
		const auto& GetRightKey() const noexcept { return RightKey; }
		const auto& GetOKKey() const noexcept { return OKKey; }
		const auto& GetNGKey() const noexcept { return NGKey; }
		const auto& GetShotKey() const noexcept { return m_ShotKey; }
		const auto& GetMouseWheel() const noexcept { return m_MouseWheel; }
		const auto& GetLookKey() const noexcept { return m_LookKey; }

		const auto& GetAccelKey() const noexcept { return m_AccelKey; }
		const auto& GetBrakeKey() const noexcept { return m_BrakeKey; }

		const auto& GetQKey() const noexcept { return m_QKey; }
		const auto& GetEKey() const noexcept { return m_EKey; }

		const auto& GetLS_X() const noexcept { return LS_X; }
		const auto& GetLS_Y() const noexcept { return LS_Y; }
	public:
		void Execute(std::function<void()>Guide_Pad_PS4, std::function<void()>Guide_Key, switchs* MouseActive = nullptr) noexcept;
	};
};