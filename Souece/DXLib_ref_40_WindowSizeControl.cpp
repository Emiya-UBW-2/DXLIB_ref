#include "DXLib_ref_40_WindowSizeControl.hpp"

namespace DXLibRef {
	const WindowSizeControl* SingletonBase<WindowSizeControl>::m_Singleton = nullptr;
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	void			WindowSizeControl::SetWindowOrBorderless(void) noexcept {
		auto* OptionParts = OptionManager::Instance();
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
	void			WindowSizeControl::SetupWindowSize(void) noexcept {
		auto* OptionParts = OptionManager::Instance();
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
			// DPI設定
			int DPI = 96;
			GetMonitorDpi(NULL, &DPI);
			if (DPI == 0) {
				DPI = 96;
			}
			// DPIを反映するデスクトップサイズ
			int DispXSize = static_cast<int>(GetSystemMetrics(SM_CXSCREEN)) * 96 / DPI;
			int DispYSize = static_cast<int>(GetSystemMetrics(SM_CYSCREEN)) * 96 / DPI;
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
		this->m_ScreenXSize = static_cast<int>(static_cast<float>(std::min(BaseScreenWidth, GetSizeXMax())) * OptionParts->GetParamFloat(EnumSaveParam::DrawScale))
			* BaseScreenWidth / this->m_DispXSize_Win;
		this->m_ScreenYSize = static_cast<int>(static_cast<float>(std::min(BaseScreenHeight, GetSizeYMax())) * OptionParts->GetParamFloat(EnumSaveParam::DrawScale))
			* BaseScreenHeight / this->m_DispYSize_Win;
	}
	// 
};
