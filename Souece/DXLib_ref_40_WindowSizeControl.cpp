#include "DXLib_ref_40_WindowSizeControl.hpp"

namespace DXLibRef {
	const WindowSizeControl* SingletonBase<WindowSizeControl>::m_Singleton = nullptr;
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	void			WindowSizeControl::SetWindowOrBorderless(void) noexcept {
		auto* OptionParts = OPTION::Instance();
		if (!OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
			switch (static_cast<WindowType>(OptionParts->GetParamInt(EnumSaveParam::WindowMode))) {
			case WindowType::Window:
				this->m_DispXSize = this->m_DispXSize_Win;
				this->m_DispYSize = this->m_DispYSize_Win;
				SetWindowStyleMode(0);
				SetWindowPosition(0, 0);
				UpdateWindowSize();
				ChangeWindowMode(TRUE);
				break;
			case WindowType::Borderless:
				this->m_DispXSize = this->m_DispXSize_Border;
				this->m_DispYSize = this->m_DispYSize_Border;
				SetWindowStyleMode(2);
				SetWindowPosition(0, 0);
				UpdateWindowSize();
				ChangeWindowMode(TRUE);
				break;
			case WindowType::FullScreen:
				this->m_DispXSize = this->m_DispXSize_Border;
				this->m_DispYSize = this->m_DispYSize_Border;
				SetWindowStyleMode(2);
				SetWindowPosition(0, 0);
				UpdateWindowSize();
				SetFullScreenResolutionMode(DX_FSRESOLUTIONMODE_NATIVE);
				SetFullScreenScalingMode(DX_FSSCALINGMODE_NEAREST);
				ChangeWindowMode(FALSE);
				break;
			case WindowType::Max:
			default:
				break;
			}
		}
	}
	// 
	void			WindowSizeControl::SetAmbientLight(const Vector3DX& AmbientLightVec, const COLOR_F& LightColor) noexcept {
		m_LightVec = AmbientLightVec;
		m_LightColorF = LightColor;
		SetGlobalAmbientLight(LightColor);
		SetLightDirection(AmbientLightVec.get());
		auto* PostPassParts = PostPassEffect::Instance();
		PostPassParts->SetAmbientLight(AmbientLightVec);
	}

	void			WindowSizeControl::SetupWindowSize(void) noexcept {
		auto* OptionParts = OPTION::Instance();
		if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
			// 解像度指定
			uint32_t t_x = 1080;
			uint32_t t_y = 1200;
			// m_VR_SystemPtr->GetRecommendedRenderTargetSize(&t_x,&t_y);
			int DispXSize = static_cast<int>(t_x) * 2;
			int DispYSize = static_cast<int>(t_y) * 2;

			this->m_DispXSize_Win = DispXSize - 72 * 16 / 9;
			this->m_DispYSize_Win = DispYSize - 72;
			this->m_DispXSize_Border = DispXSize;
			this->m_DispYSize_Border = DispYSize;
		}
		else {
			// DPIを反映するデスクトップサイズ
			int DispXSize = static_cast<int>(GetSystemMetrics(SM_CXSCREEN));
			int DispYSize = static_cast<int>(GetSystemMetrics(SM_CYSCREEN));
			// 解像度指定
			if (DispYSize >= (DispXSize * BaseScreenHeight / BaseScreenWidth)) {// 4:3
				DispYSize = (DispXSize * BaseScreenHeight / BaseScreenWidth);
			}
			else {// 16:9より横長
				DispXSize = (DispYSize * BaseScreenWidth / BaseScreenHeight);
			}
			this->m_DispXSize_Win = DispXSize - 72 * 16 / 9;
			this->m_DispYSize_Win = DispYSize - 72;
			this->m_DispXSize_Border = DispXSize;
			this->m_DispYSize_Border = DispYSize;
		}
		int DispXSize_Max = std::min(GetSizeXMax(), static_cast<int>(static_cast<float>(std::min<int>(BaseScreenWidth, GetSizeXMax())) * std::clamp(OptionParts->GetParamFloat(EnumSaveParam::DrawScale), 0.25f, 10.f)));
		int DispYSize_Max = std::min(GetSizeYMax(), static_cast<int>(static_cast<float>(std::min<int>(BaseScreenHeight, GetSizeYMax())) * std::clamp(OptionParts->GetParamFloat(EnumSaveParam::DrawScale), 0.25f, 10.f)));
		this->m_ScreenXSize = BaseScreenWidth * DispXSize_Max / this->m_DispXSize_Win;
		this->m_ScreenYSize = BaseScreenHeight * DispYSize_Max / this->m_DispYSize_Win;
	}
	// 
};
