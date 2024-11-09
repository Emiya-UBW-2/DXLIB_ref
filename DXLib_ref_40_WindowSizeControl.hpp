#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	enum class VR_PAD {
		TRIGGER,
		SIDEBUTTON,
		TOUCHPAD,
		TOPBUTTON1,
		TOPBUTTON2,
	};

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

		Vector3DX					m_LightVec;
		COLOR_F						m_LightColorF{ GetColorF(0, 0, 0, 0) };
		Camera3DInfo				m_MainCamera;							// カメラ
	public:// ゲッター
		const auto& GetLightVec(void) const noexcept { return m_LightVec; }
		// UI以外のスクリーン空間
		const auto		GetScreenX(int value) const noexcept { return value * this->m_ScreenXSize / BaseScreenWidth; }
		const auto		GetScreenY(int value) const noexcept { return value * this->m_ScreenYSize / BaseScreenHeight; }
		// UI以外のスクリーン空間(1920,1080の場合)
		const auto& GetScreenXMax(void) const noexcept { return this->m_ScreenXSize; }
		const auto& GetScreenYMax(void) const noexcept { return this->m_ScreenYSize; }
		// UI用
		const auto		GetUIX(int value) const noexcept { return value * this->m_DispXSize / BaseScreenWidth; }
		const auto		GetUIY(int value) const noexcept { return value * this->m_DispYSize / BaseScreenHeight; }
		// UI用(1920,1080の場合)
		const auto& GetUIXMax(void) const noexcept { return this->m_DispXSize; }
		const auto& GetUIYMax(void) const noexcept { return this->m_DispYSize; }
		// UI用(1920,1080の場合)
		const auto& GetSizeXMax(void) const noexcept { return this->m_DispXSize_Border; }
		const auto& GetSizeYMax(void) const noexcept { return this->m_DispYSize_Border; }
		// 
		const auto& GetMainCamera(void) const noexcept { return m_MainCamera; }
		auto& SetMainCamera(void) noexcept { return m_MainCamera; }
	public:
		void			SetAmbientLight(const Vector3DX& AmbientLightVec, const COLOR_F& LightColor) noexcept;
		void			SetWindowOrBorderless(void) noexcept;
		void			UpdateWindowSize(void) const noexcept {
			// DPI設定
			int DPI = 96;
			GetMonitorDpi(NULL, &DPI);
			SetWindowSize(this->m_DispXSize * DPI / 96, this->m_DispYSize * DPI / 96);
		}
	private:// コンストラクタ
		WindowSizeControl(void) noexcept;
		WindowSizeControl(const WindowSizeControl&) = delete;
		WindowSizeControl(WindowSizeControl&& o) = delete;
		WindowSizeControl& operator=(const WindowSizeControl&) = delete;
		WindowSizeControl& operator=(WindowSizeControl&& o) = delete;
	public:
		void			SetupWindowSize(void) noexcept;
		void			Draw3DVR(
			std::function<void()> sky_doing,
			std::function<void()> setshadowdoing_rigid,
			std::function<void()> setshadowdoing,
			std::function<void()> doing,
			std::function<void()> doingFront,
			std::function<void()> doingUI,
			std::function<void()> doingUI2
		) noexcept;
		void			Draw3DMain(
			std::function<void()> sky_doing,
			std::function<void()> setshadowdoing_rigid,
			std::function<void()> setshadowdoing,
			std::function<void()> doing,
			std::function<void()> doingFront,
			const Camera3DInfo& camInfo
		) noexcept;
		// VR
	private:
		class VRControl;
		VRControl* m_VRControl{ nullptr };
		VRControl* GetVRControl(void) noexcept { return m_VRControl; }
		const VRControl* GetVRControl(void) const noexcept { return m_VRControl; }
	public:
		void				VR_Setup(void) noexcept;
		void				VR_Update(void) noexcept;
		void				VR_Dispose(void) noexcept;
		void				Get_VR_HMDPositionVR(Vector3DX* pos_, Matrix4x4DX* mat) noexcept;
		void				Reset_VR_HMD(void) noexcept;
		bool				Get_VR_Hand1PadPress(VR_PAD) const noexcept;
		bool				Get_VR_Hand1TouchPress(VR_PAD) const noexcept;
		Vector3DX			Get_VR_Hand1TouchPadPoint(void) const noexcept;
		bool				Get_VR_Hand2PadPress(VR_PAD) const noexcept;
		bool				Get_VR_Hand2TouchPress(VR_PAD) const noexcept;
		Vector3DX			Get_VR_Hand2TouchPadPoint(void) const noexcept;
		void				VR_Haptic(char id_, unsigned short times) noexcept;	// VRコントローラー振動
		void				VR_WaitSync(void) noexcept;
	};
};
