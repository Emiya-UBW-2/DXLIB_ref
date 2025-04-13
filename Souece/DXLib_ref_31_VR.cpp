#include "DXLib_ref_40_WindowSizeControl.hpp"

namespace DXLibRef {
#if defined(_USE_OPENVR_)
	const VRControl* SingletonBase<VRControl>::m_Singleton = nullptr;
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	void VRControl::Init(void) noexcept {
		auto* OptionParts = OptionManager::Instance();
		// VRが使えるかチェック
		if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
			m_VR_ErrorHandle = vr::VRInitError_None;
			m_VR_SystemPtr = vr::VR_Init(&m_VR_ErrorHandle, vr::VRApplication_Scene);
			if (m_VR_ErrorHandle != vr::VRInitError_None) {
				m_VR_SystemPtr = nullptr;
				OptionParts->SetParamBoolean(EnumSaveParam::usevr, false);
			}
		}
		// デバイスセット
		if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
			m_VR_TrackerID.clear();
			char DeviceCount = 0;
			bool IsFirstHand = true;
			for (char loop = 0; loop < 8; ++loop) {
				auto DeviceID = DeviceCount;
				auto DeviceType = m_VR_SystemPtr->GetTrackedDeviceClass(loop);
				switch (DeviceType) {
				case vr::TrackedDeviceClass_HMD:
					m_VR_HMDID = DeviceID;
					++DeviceCount;
					break;
				case vr::TrackedDeviceClass_Controller:
					if (IsFirstHand) {
						IsFirstHand = false;
						m_VR_Hand1ID = DeviceID;
					}
					else {
						m_VR_Hand2ID = DeviceID;
					}
					++DeviceCount;
					break;
				case vr::TrackedDeviceClass_GenericTracker:
					m_VR_TrackerID.emplace_back(DeviceID);
					++DeviceCount;
					break;
				case vr::TrackedDeviceClass_TrackingReference:
					++DeviceCount;
					break;
				default:
					continue;
				}
				m_VR_DeviceInfo.resize(DeviceCount);
				m_VR_DeviceInfo.back().Init(DeviceID, loop, DeviceType);
			}
		}
	}
	void VRControl::SetupBuffer(void) noexcept {
		auto* WindowSizeParts = WindowSizeControl::Instance();
		auto* OptionParts = OptionManager::Instance();
		if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
			// 画面セット
			m_OutScreen.Make(WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax());	// 左目
			UI_Screen.Make(WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), true);	// UI
		}
	}
	void VRControl::Execute(void) noexcept {
		auto* OptionParts = OptionManager::Instance();
		if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
			for (auto& c : m_VR_DeviceInfo) {
				c.Update(m_VR_SystemPtr);
			}
		}
		else {
			for (auto& c : m_VR_DeviceInfo) {
				c.Reset();
			}
		}
	}
	void VRControl::Submit(char eye_type) noexcept {
		vr::Texture_t tex = { (void*)GetUseDirect3D11BackBufferTexture2D(), vr::ETextureType::TextureType_DirectX,vr::EColorSpace::ColorSpace_Auto };
		vr::VRCompositor()->Submit((vr::EVREye)eye_type, &tex, NULL, vr::Submit_Default);
	}
	void VRControl::SetUpBackUI(std::function<void()> doingUI) noexcept {
		// UIをスクリーンに描画しておく
		UI_Screen.SetDraw_Screen();
		{
			doingUI();
		}
	}
	void VRControl::SubmitDraw(char eye_type, const GraphHandle& MainDrawScreen, std::function<void()> doingUI2) noexcept {
		auto* WindowSizeParts = WindowSizeControl::Instance();
		Camera3DInfo tmp_cam = WindowSizeParts->GetMainCamera();
		tmp_cam.SetCamPos(
			tmp_cam.GetCamPos() + this->GetEyePosition(eye_type),
			tmp_cam.GetCamVec() + this->GetEyePosition(eye_type),
			tmp_cam.GetCamUp()
		);
		// 目線用に合成
		m_OutScreen.SetDraw_Screen();
		tmp_cam.FlipCamInfo();
		{
			MainDrawScreen.DrawGraph(0, 0, true);
			{
				// 視差に対応しているUI
				SetCameraNearFar(0.01f, 2.f);
				SetUseZBuffer3D(FALSE);												// zbufuse
				SetWriteZBuffer3D(FALSE);											// zbufwrite
				{
					DrawBillboard3D((tmp_cam.GetCamPos() + (tmp_cam.GetCamVec() - tmp_cam.GetCamPos()).normalized() * 1.0f).get(),
						0.5f, 0.5f, 1.8f, 0.f, UI_Screen.get(), TRUE);
				}
				SetUseZBuffer3D(TRUE);												// zbufuse
				SetWriteZBuffer3D(TRUE);											// zbufwrite
			}
		}
		// 合成したものをBACKに持ってきて
		GraphHandle::SetDraw_Screen(static_cast<int>(DX_SCREEN_BACK));
		{
			m_OutScreen.DrawGraph(0, 0, false);
			doingUI2();
		}
		// それぞれの目にDX_SCREEN_BACKの内容を送信
		this->Submit(eye_type);
	}
	void VRControl::WaitSync(void) noexcept {
		auto* OptionParts = OptionManager::Instance();
		if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
			vr::TrackedDevicePose_t tmp;
			vr::VRCompositor()->WaitGetPoses(&tmp, 1, NULL, 1);
		}
	}
	void VRControl::Dispose(void) noexcept {
		auto* OptionParts = OptionManager::Instance();
		if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
			// vr::VR_Shutdown();
			m_VR_SystemPtr = nullptr;
		}
	}
#endif
}