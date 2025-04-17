#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	class WindowSizeControl : public SingletonBase<WindowSizeControl> {
	private:
		friend class SingletonBase<WindowSizeControl>;
	private:
		int							m_DispXSize{ BaseScreenWidth };
		int							m_DispYSize{ BaseScreenHeight };

		int							m_DispXSize_Win{ BaseScreenWidth };
		int							m_DispYSize_Win{ BaseScreenHeight };

		int							m_DispXSize_Border{ BaseScreenWidth };
		int							m_DispYSize_Border{ BaseScreenHeight };

		int							m_ScreenXSize{ BaseScreenWidth };
		int							m_ScreenYSize{ BaseScreenHeight };
	public:// �Q�b�^�[
		// UI�ȊO�̃X�N���[�����
		const auto		GetScreenX(int value) const noexcept { return value * this->m_ScreenXSize / BaseScreenWidth; }
		const auto		GetScreenY(int value) const noexcept { return value * this->m_ScreenYSize / BaseScreenHeight; }
		// UI�ȊO�̃X�N���[�����(1920,1080�̏ꍇ)
		const auto&		GetScreenXMax(void) const noexcept { return this->m_ScreenXSize; }
		const auto&		GetScreenYMax(void) const noexcept { return this->m_ScreenYSize; }
		// UI�p
		const auto		GetUIX(int value) const noexcept { return value * this->m_DispXSize / BaseScreenWidth; }
		const auto		GetUIY(int value) const noexcept { return value * this->m_DispYSize / BaseScreenHeight; }
		// UI�p(1920,1080�̏ꍇ)
		const auto&		GetUIXMax(void) const noexcept { return this->m_DispXSize; }
		const auto&		GetUIYMax(void) const noexcept { return this->m_DispYSize; }
		// UI�p(1920,1080�̏ꍇ)
		const auto&		GetSizeXMax(void) const noexcept { return this->m_DispXSize_Border; }
		const auto&		GetSizeYMax(void) const noexcept { return this->m_DispYSize_Border; }
	public:
		void			SetWindowOrBorderless(void) noexcept;
		void			UpdateWindowSize(void) const noexcept {
			SetWindowSize(this->m_DispXSize * GetDPI() / BaseDPI, this->m_DispYSize * GetDPI() / BaseDPI);
		}
	private:// �R���X�g���N�^
		WindowSizeControl(void) noexcept{}
		WindowSizeControl(const WindowSizeControl&) = delete;
		WindowSizeControl(WindowSizeControl&& o) = delete;
		WindowSizeControl& operator=(const WindowSizeControl&) = delete;
		WindowSizeControl& operator=(WindowSizeControl&& o) = delete;
	public:
		void			SetupWindowSize(void) noexcept;
	};
};
