#include "DXLib_ref_09_KeyControl.hpp"
#include "DXLib_ref_10_Option.hpp"
#include "DXLib_ref_08_Etc.hpp"


namespace DXLibRef {
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	const OPTION* SingletonBase<OPTION>::m_Singleton = nullptr;

	void			OPTION::Load(void) noexcept {
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::GraphicsPreset)).SetEnumParamType(EnumParamType::Int);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::DirectXVer)).SetEnumParamType(EnumParamType::Else);//DirectXVer
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::usevr)).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::ObjLevel)).SetEnumParamType(EnumParamType::Int);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::DoF)).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::bloom)).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::shadow)).SetEnumParamType(EnumParamType::Int);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::SSAO)).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::fov)).SetEnumParamType(EnumParamType::Int);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::vsync)).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::FpsLimit)).SetEnumParamType(EnumParamType::Int);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::SE)).SetEnumParamType(EnumParamType::Float);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::VOICE)).SetEnumParamType(EnumParamType::Float);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::BGM)).SetEnumParamType(EnumParamType::Float);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::WindowMode)).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::ScreenEffect)).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::Reflection)).SetEnumParamType(EnumParamType::Int);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::MotionBlur)).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::Xsensing)).SetEnumParamType(EnumParamType::Float);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::Ysensing)).SetEnumParamType(EnumParamType::Float);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::HeadBobbing)).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::ControlType)).SetEnumParamType(EnumParamType::Else);//ControlType
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::Language)).SetEnumParamType(EnumParamType::Else);//Language
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::AA)).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::DrawScale)).SetEnumParamType(EnumParamType::Float);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::GodRay)).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::PBR)).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at(static_cast<size_t>(EnumSaveParam::Distortion)).SetEnumParamType(EnumParamType::Boolean);
		
		//SetOutApplicationLogValidFlag(FALSE);
		int mdata = INVALID_ID;
		bool NewData = true;
		if (std::filesystem::is_regular_file("Save/Setting.txt")) {
			mdata = FileRead_open("Save/Setting.txt", FALSE);
			NewData = false;
		}

		if (NewData) {
			WCHAR localeName[LOCALE_NAME_MAX_LENGTH];
			int retVal = GetUserDefaultLocaleName(localeName, ARRAYSIZE(localeName));
			if (retVal > 0) {
				SetParamInt(EnumSaveParam::Language, (StrCmpW(localeName, L"ja-JP") == 0) ? 0 : 1);
			}
			//共通設定項目
			if (std::filesystem::is_regular_file("data/Setting.txt")) {
				mdata = FileRead_open("data/Setting.txt", FALSE);
			}
			else {
				//デフォ値
				SetParamInt(EnumSaveParam::GraphicsPreset, 3);
				SetParamInt(EnumSaveParam::DirectXVer, 1);
				SetParamBoolean(EnumSaveParam::usevr, false);
				SetParamInt(EnumSaveParam::ObjLevel, 3);
				SetParamBoolean(EnumSaveParam::DoF, true);
				SetParamBoolean(EnumSaveParam::bloom, true);
				SetParamInt(EnumSaveParam::shadow, 3);
				SetParamBoolean(EnumSaveParam::SSAO, true);
				SetParamInt(EnumSaveParam::fov, 90);
				SetParamBoolean(EnumSaveParam::vsync, true);
				SetParamInt(EnumSaveParam::FpsLimit, 60);
				SetParamFloat(EnumSaveParam::SE, 0.5f);
				SetParamFloat(EnumSaveParam::VOICE, 0.5f);
				SetParamFloat(EnumSaveParam::BGM, 0.5f);
				SetParamBoolean(EnumSaveParam::WindowMode, true);
				SetParamBoolean(EnumSaveParam::ScreenEffect, true);
				SetParamInt(EnumSaveParam::Reflection, 2);
				SetParamBoolean(EnumSaveParam::MotionBlur, false);
				SetParamFloat(EnumSaveParam::Xsensing, 0.5f);
				SetParamFloat(EnumSaveParam::Ysensing, 0.5f);
				SetParamBoolean(EnumSaveParam::HeadBobbing, true);
				SetParamInt(EnumSaveParam::ControlType, 2);
				SetParamBoolean(EnumSaveParam::AA, true);
				SetParamFloat(EnumSaveParam::DrawScale, 1.f);
				SetParamBoolean(EnumSaveParam::GodRay, true);
				SetParamBoolean(EnumSaveParam::PBR, true);
				SetParamBoolean(EnumSaveParam::Distortion, true);
				return;
			}
		}
		while (true) {
			if (FileRead_eof(mdata) != 0) { break; }
			auto ALL = getparams::Getstr(mdata);
			if (ALL == "") { continue; }
			auto LEFT = getparams::getleft(ALL);
			auto RIGHT = getparams::getright(ALL);
			for (size_t loop = 0;loop < static_cast<size_t>(EnumSaveParam::Max);loop++) {
				if (LEFT != OptionStr[loop]) { continue; }
				switch (m_SaveParams.at(loop).GetEnumParamType()) {
					case EnumParamType::Boolean:
						SetParamBoolean((EnumSaveParam)loop, (RIGHT.find("true") != std::string::npos));
						break;
					case EnumParamType::Int:
						SetParamInt((EnumSaveParam)loop, std::stoi(RIGHT));
						break;
					case EnumParamType::Float:
						SetParamFloat((EnumSaveParam)loop, std::stof(RIGHT));
						break;
					case EnumParamType::Else:
						if (loop == static_cast<size_t>(EnumSaveParam::DirectXVer)) {
							for (size_t i = 0;i < 2;i++) {
								if (RIGHT == DirectXVerStr[i]) {
									SetParamInt((EnumSaveParam)loop, static_cast<int>(i));
									break;
								}
							}
						}
						else if (loop == static_cast<size_t>(EnumSaveParam::ControlType)) {
							for (size_t i = 1;i < 3;i++) {
								if (RIGHT == ControlTypeStr[i]) {
									SetParamInt((EnumSaveParam)loop, static_cast<int>(i));
									break;
								}
							}
						}
						else if (loop == static_cast<size_t>(EnumSaveParam::Language) && !NewData) {
							for (size_t i = 0;i < 2;i++) {
								if (RIGHT == LanguageStr[i]) {
									SetParamInt((EnumSaveParam)loop, static_cast<int>(i));
									break;
								}
							}
						}
						break;
					default:
						break;
				}
				break;
			}
		}
		FileRead_close(mdata);
		//SetOutApplicationLogValidFlag(TRUE);
	}

	void			OPTION::Save(void) noexcept {
		std::ofstream outputfile("Save/Setting.txt");

		for (int loop = 0;loop < static_cast<int>(EnumSaveParam::Max);loop++) {
			switch (m_SaveParams.at(static_cast<size_t>(loop)).GetEnumParamType()) {
				case EnumParamType::Boolean:
					outputfile << std::string(OptionStr[loop]) + "=" + std::string(GetParamBoolean((EnumSaveParam)loop) ? "true" : "false") + "\n";
					break;
				case EnumParamType::Int:
					outputfile << std::string(OptionStr[loop]) + "=" + std::to_string(GetParamInt((EnumSaveParam)loop)) + "\n";
					break;
				case EnumParamType::Float:
					outputfile << std::string(OptionStr[loop]) + "=" + std::to_string(GetParamFloat((EnumSaveParam)loop)) + "\n";
					break;
				case EnumParamType::Else:
					if (loop == static_cast<int>(EnumSaveParam::DirectXVer)) {
						outputfile << std::string(OptionStr[loop]) + "=" + std::string(DirectXVerStr[GetParamInt((EnumSaveParam)loop)]) + "\n";
					}
					else if (loop == static_cast<int>(EnumSaveParam::ControlType)) {
						outputfile << std::string(OptionStr[loop]) + "=" + std::string(ControlTypeStr[GetParamInt((EnumSaveParam)loop)]) + "\n";
					}
					else if (loop == static_cast<int>(EnumSaveParam::Language)) {
						outputfile << std::string(OptionStr[loop]) + "=" + std::string(LanguageStr[GetParamInt((EnumSaveParam)loop)]) + "\n";
					}
					break;
				default:
					break;
			}
		}

		outputfile.close();
	}

	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	const OptionWindowClass* SingletonBase<OptionWindowClass>::m_Singleton = nullptr;
	//
	void OptionWindowClass::OptionElementsInfo::Draw(int xpos, int ypos, bool isMine) const noexcept {
		auto* DrawParts = DXDraw::Instance();
		ypos += DrawParts->GetUIY(static_cast<int>(selanim));
		WindowSystem::SetMsgWW(xpos, ypos + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, isMine ? White : Gray50, Black, m_Name);
		m_Draw(xpos + DrawParts->GetUIY(720 - 324), ypos, isMine);
	}
	//
	void OptionWindowClass::OptionTabsInfo::Execute(int *select, bool CanPress) noexcept {
		if ((*select) < 0) { return; }
		auto* SE = SoundPool::Instance();
		auto* Pad = PadControl::Instance();
		m_Elements.at(static_cast<size_t>(*select)).GetAnyDoing();
		if (CanPress) {
			if (Pad->GetKey(PADS::MOVE_W).trigger()) {
				--(*select);
				if ((*select) < 0) { (*select) = static_cast<int>(m_Elements.size()) - 1; }
				m_Elements.at(static_cast<size_t>(*select)).selanim = 3.f;
				SE->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}
			if (Pad->GetKey(PADS::MOVE_S).trigger()) {
				++(*select);
				if ((*select) > static_cast<int>(m_Elements.size()) - 1) { (*select) = 0; }
				m_Elements.at(static_cast<size_t>(*select)).selanim = -3.f;
				SE->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}
			if (Pad->GetKey(PADS::MOVE_A).repeat()) {
				m_Elements.at(static_cast<size_t>(*select)).GetLeftPush();
			}
			if (Pad->GetKey(PADS::MOVE_D).repeat()) {
				m_Elements.at(static_cast<size_t>(*select)).GetRightPush();
			}
			if (Pad->GetKey(PADS::INTERACT).trigger() || Pad->GetMouseClick().trigger()) {
				m_Elements.at(static_cast<size_t>(*select)).GetOKPush();
			}
		}
		for (auto& e : m_Elements) {
			Easing(&e.selanim, 0.f, 0.95f, EasingType::OutExpo);
		}
	}
	void OptionWindowClass::OptionTabsInfo::Draw(int xpos, int ypos, bool isActive, int* TabSel, int *select) noexcept {
		auto* DrawParts = DXDraw::Instance();
		int xp1, yp1;
		//タブ
		{
			xp1 = xpos + (DrawParts->GetUIY(140) + DrawParts->GetUIY(12)) * m_id;
			yp1 = ypos;
			if (WindowSystem::SetMsgClickBox(xp1, yp1 + DrawParts->GetUIY(5), xp1 + DrawParts->GetUIY(140), yp1 + LineHeight * 2 - DrawParts->GetUIY(5), LineHeight, (isActive ? Gray25 : Gray75), m_name)) {
				*TabSel = GetID();
				*select = 0;
				auto* SE = SoundPool::Instance();
				SE->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}
		}
		//内容
		if (isActive) {
			xp1 = xpos;
			yp1 = ypos + LineHeight * 2;
			for (int i = 0; i < static_cast<int>(m_Elements.size()); i++) {
				yp1 += (LineHeight + DrawParts->GetUIY(6));
				if (IntoMouse(xp1, yp1, xp1 + DrawParts->GetUIY(500), yp1 + (LineHeight + DrawParts->GetUIY(6)))) {
					*select = i;
				}
				m_Elements.at(static_cast<size_t>(i)).Draw(xp1, yp1, (*select == i));
			}
		}
	}
	void OptionWindowClass::OptionTabsInfo::DrawInfo(int xpos, int ypos, int select) noexcept {
		auto* LocalizeParts = LocalizePool::Instance();
		WindowSystem::SetMsgWW(xpos, ypos + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, LocalizeParts->Get(m_Elements.at(static_cast<size_t>(select)).GetInfoTextID()));
	}
	//
	void OptionWindowClass::SoundTabsInfo::Init_Sub(void) noexcept {
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("BGM", 1110,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamFloat(EnumSaveParam::BGM, std::clamp(OptionParts->GetParamFloat(EnumSaveParam::BGM) - 0.1f, 0.f, 1.f));
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 BGMPool::Instance()->SetVol(OptionParts->GetParamFloat(EnumSaveParam::BGM));
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamFloat(EnumSaveParam::BGM, std::clamp(OptionParts->GetParamFloat(EnumSaveParam::BGM) + 0.1f, 0.f, 1.f));
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 BGMPool::Instance()->SetVol(OptionParts->GetParamFloat(EnumSaveParam::BGM));
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* DrawParts = DXDraw::Instance();
										 auto* OptionParts = OPTION::Instance();
										 int value = WindowSystem::UpDownBar(xpos, xpos + DrawParts->GetUIY(200), ypos, static_cast<int>(OptionParts->GetParamFloat(EnumSaveParam::BGM)*100.f + 0.5f), 0, 100);
										 OptionParts->SetParamFloat(EnumSaveParam::BGM, static_cast<float>(value) / 100.f);
										 BGMPool::Instance()->SetVol(OptionParts->GetParamFloat(EnumSaveParam::BGM));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("SE", 1111,
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamFloat(EnumSaveParam::SE, std::clamp(OptionParts->GetParamFloat(EnumSaveParam::SE) - 0.1f, 0.f, 1.f));
										 SE->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 SE->SetVol(OptionParts->GetParamFloat(EnumSaveParam::SE));
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamFloat(EnumSaveParam::SE, std::clamp(OptionParts->GetParamFloat(EnumSaveParam::SE) + 0.1f, 0.f, 1.f));
										 SE->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 SE->SetVol(OptionParts->GetParamFloat(EnumSaveParam::SE));
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* DrawParts = DXDraw::Instance();
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 int value = WindowSystem::UpDownBar(xpos, xpos + DrawParts->GetUIY(200), ypos, static_cast<int>(OptionParts->GetParamFloat(EnumSaveParam::SE)*100.f + 0.5f), 0, 100);
										 OptionParts->SetParamFloat(EnumSaveParam::SE, static_cast<float>(value) / 100.f);
										 SE->SetVol(OptionParts->GetParamFloat(EnumSaveParam::SE));
									 }
									 );
	}
	void OptionWindowClass::GraphicTabsInfo::Init_Sub(void) noexcept {
		HDC hdc;
		hdc = GetDC(GetMainWindowHandle());	// デバイスコンテキストの取得
		RefreshRate = GetDeviceCaps(hdc, VREFRESH);	// リフレッシュレートの取得
		ReleaseDC(GetMainWindowHandle(), hdc);	// デバイスコンテキストの解放

		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Graphics Preset", 1120,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::GraphicsPreset, std::clamp(OptionParts->GetParamInt(EnumSaveParam::GraphicsPreset) - 1, 0, 4));
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 switch (OptionParts->GetParamInt(EnumSaveParam::GraphicsPreset)) {
											 case 0:
												 OptionParts->SetParamBoolean(EnumSaveParam::AA, false);
												 OptionParts->SetParamBoolean(EnumSaveParam::SSAO, false);
												 OptionParts->SetParamInt(EnumSaveParam::Reflection, 0);
												 OptionParts->SetParamInt(EnumSaveParam::shadow, 0);
												 OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 0);
												 OptionParts->SetParamBoolean(EnumSaveParam::bloom, false);
												 OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, false);
												 
												 break;
											 case 1:
												 OptionParts->SetParamBoolean(EnumSaveParam::AA, false);
												 OptionParts->SetParamBoolean(EnumSaveParam::SSAO, false);
												 OptionParts->SetParamInt(EnumSaveParam::Reflection, 1);
												 OptionParts->SetParamInt(EnumSaveParam::shadow, 1);
												 OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 1);
												 OptionParts->SetParamBoolean(EnumSaveParam::bloom, true);
												 OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);
												 
												 break;
											 case 2:
												 OptionParts->SetParamBoolean(EnumSaveParam::AA, true);
												 OptionParts->SetParamBoolean(EnumSaveParam::SSAO, false);
												 OptionParts->SetParamInt(EnumSaveParam::Reflection, 1);
												 OptionParts->SetParamInt(EnumSaveParam::shadow, 2);
												 OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 2);
												 OptionParts->SetParamBoolean(EnumSaveParam::bloom, true);
												 OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);
												 
												 break;
											 case 3:
												 OptionParts->SetParamBoolean(EnumSaveParam::AA, true);
												 OptionParts->SetParamBoolean(EnumSaveParam::SSAO, false);
												 OptionParts->SetParamInt(EnumSaveParam::Reflection, 2);
												 OptionParts->SetParamInt(EnumSaveParam::shadow, 3);
												 OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 3);
												 OptionParts->SetParamBoolean(EnumSaveParam::bloom, true);
												 OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);
												 
												 break;
											 case 4:
												 OptionParts->SetParamBoolean(EnumSaveParam::AA, true);
												 OptionParts->SetParamBoolean(EnumSaveParam::SSAO, false);
												 OptionParts->SetParamInt(EnumSaveParam::Reflection, 2);
												 OptionParts->SetParamInt(EnumSaveParam::shadow, 4);
												 OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 4);
												 OptionParts->SetParamBoolean(EnumSaveParam::bloom, true);
												 OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);
												 
												 break;
										 }
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::GraphicsPreset, std::clamp(OptionParts->GetParamInt(EnumSaveParam::GraphicsPreset) + 1, 0, 4));
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 switch (OptionParts->GetParamInt(EnumSaveParam::GraphicsPreset)) {
											 case 0:
												 OptionParts->SetParamBoolean(EnumSaveParam::AA, false);
												 OptionParts->SetParamBoolean(EnumSaveParam::SSAO, false);
												 OptionParts->SetParamInt(EnumSaveParam::Reflection, 0);
												 OptionParts->SetParamInt(EnumSaveParam::shadow, 0);
												 OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 0);
												 OptionParts->SetParamBoolean(EnumSaveParam::bloom, false);
												 OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, false);
												 
												 break;
											 case 1:
												 OptionParts->SetParamBoolean(EnumSaveParam::AA, false);
												 OptionParts->SetParamBoolean(EnumSaveParam::SSAO, false);
												 OptionParts->SetParamInt(EnumSaveParam::Reflection, 1);
												 OptionParts->SetParamInt(EnumSaveParam::shadow, 1);
												 OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 1);
												 OptionParts->SetParamBoolean(EnumSaveParam::bloom, true);
												 OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);
												 
												 break;
											 case 2:
												 OptionParts->SetParamBoolean(EnumSaveParam::AA, true);
												 OptionParts->SetParamBoolean(EnumSaveParam::SSAO, false);
												 OptionParts->SetParamInt(EnumSaveParam::Reflection, 1);
												 OptionParts->SetParamInt(EnumSaveParam::shadow, 2);
												 OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 2);
												 OptionParts->SetParamBoolean(EnumSaveParam::bloom, true);
												 OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);
												 
												 break;
											 case 3:
												 OptionParts->SetParamBoolean(EnumSaveParam::AA, true);
												 OptionParts->SetParamBoolean(EnumSaveParam::SSAO, false);
												 OptionParts->SetParamInt(EnumSaveParam::Reflection, 2);
												 OptionParts->SetParamInt(EnumSaveParam::shadow, 3);
												 OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 3);
												 OptionParts->SetParamBoolean(EnumSaveParam::bloom, true);
												 OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);
												 
												 break;
											 case 4:
												 OptionParts->SetParamBoolean(EnumSaveParam::AA, true);
												 OptionParts->SetParamBoolean(EnumSaveParam::SSAO, false);
												 OptionParts->SetParamInt(EnumSaveParam::Reflection, 2);
												 OptionParts->SetParamInt(EnumSaveParam::shadow, 4);
												 OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 4);
												 OptionParts->SetParamBoolean(EnumSaveParam::bloom, true);
												 OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);
												 
												 break;
										 }
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* DrawParts = DXDraw::Instance();
										 auto* OptionParts = OPTION::Instance();
										 auto prev = OptionParts->GetParamInt(EnumSaveParam::GraphicsPreset);
										 OptionParts->SetParamInt(EnumSaveParam::GraphicsPreset, WindowSystem::UpDownBox(xpos, xpos + DrawParts->GetUIY(200), ypos, OptionParts->GetParamInt(EnumSaveParam::GraphicsPreset), 4));
										 if (prev != OptionParts->GetParamInt(EnumSaveParam::GraphicsPreset)) {
											 switch (OptionParts->GetParamInt(EnumSaveParam::GraphicsPreset)) {
												 case 0:
													 OptionParts->SetParamBoolean(EnumSaveParam::AA, false);
													 OptionParts->SetParamBoolean(EnumSaveParam::SSAO, false);
													 OptionParts->SetParamInt(EnumSaveParam::Reflection, 0);
													 OptionParts->SetParamInt(EnumSaveParam::shadow, 0);
													 OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 0);
													 OptionParts->SetParamBoolean(EnumSaveParam::bloom, false);
													 OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, false);
													 
													 break;
												 case 1:
													 OptionParts->SetParamBoolean(EnumSaveParam::AA, false);
													 OptionParts->SetParamBoolean(EnumSaveParam::SSAO, false);
													 OptionParts->SetParamInt(EnumSaveParam::Reflection, 1);
													 OptionParts->SetParamInt(EnumSaveParam::shadow, 1);
													 OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 1);
													 OptionParts->SetParamBoolean(EnumSaveParam::bloom, true);
													 OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);
													 
													 break;
												 case 2:
													 OptionParts->SetParamBoolean(EnumSaveParam::AA, true);
													 OptionParts->SetParamBoolean(EnumSaveParam::SSAO, false);
													 OptionParts->SetParamInt(EnumSaveParam::Reflection, 1);
													 OptionParts->SetParamInt(EnumSaveParam::shadow, 2);
													 OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 2);
													 OptionParts->SetParamBoolean(EnumSaveParam::bloom, true);
													 OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);
													 
													 break;
												 case 3:
													 OptionParts->SetParamBoolean(EnumSaveParam::AA, true);
													 OptionParts->SetParamBoolean(EnumSaveParam::SSAO, false);
													 OptionParts->SetParamInt(EnumSaveParam::Reflection, 2);
													 OptionParts->SetParamInt(EnumSaveParam::shadow, 3);
													 OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 3);
													 OptionParts->SetParamBoolean(EnumSaveParam::bloom, true);
													 OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);
													 
													 break;
												 case 4:
													 OptionParts->SetParamBoolean(EnumSaveParam::AA, true);
													 OptionParts->SetParamBoolean(EnumSaveParam::SSAO, false);
													 OptionParts->SetParamInt(EnumSaveParam::Reflection, 2);
													 OptionParts->SetParamInt(EnumSaveParam::shadow, 4);
													 OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 4);
													 OptionParts->SetParamBoolean(EnumSaveParam::bloom, true);
													 OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);
													 
													 break;
											 }
										 }
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Window Mode", 1121,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->ChangeParamBoolean(EnumSaveParam::WindowMode);
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 auto* DrawParts = DXDraw::Instance();
										 DrawParts->SetWindowOrBorderless();
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->ChangeParamBoolean(EnumSaveParam::WindowMode);
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 auto* DrawParts = DXDraw::Instance();
										 DrawParts->SetWindowOrBorderless();
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* LocalizeParts = LocalizePool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 auto* DrawParts = DXDraw::Instance();
										 auto prev = OptionParts->GetParamBoolean(EnumSaveParam::WindowMode);

										 OptionParts->SetParamBoolean(EnumSaveParam::WindowMode, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::WindowMode)));
										 if (prev != OptionParts->GetParamBoolean(EnumSaveParam::WindowMode)) {
											 DrawParts->SetWindowOrBorderless();
										 }

										 WindowSystem::SetMsgWW(xpos + DrawParts->GetUIY(100), ypos + LineHeight / 2,
																LineHeight, FontHandle::FontXCenter::MIDDLE, White, Black, OptionParts->GetParamBoolean(EnumSaveParam::WindowMode) ? LocalizeParts->Get(1135) : LocalizeParts->Get(1136));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("V Sync", 1123,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->ChangeParamBoolean(EnumSaveParam::vsync);
										 if (OptionParts->GetParamBoolean(EnumSaveParam::vsync)) {
											 OptionParts->SetParamInt(EnumSaveParam::FpsLimit, RefreshRate);
										 }
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->ChangeParamBoolean(EnumSaveParam::vsync);
										 if (OptionParts->GetParamBoolean(EnumSaveParam::vsync)) {
											 OptionParts->SetParamInt(EnumSaveParam::FpsLimit, RefreshRate);
										 }
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 auto prev = OptionParts->GetParamBoolean(EnumSaveParam::vsync);
										 OptionParts->SetParamBoolean(EnumSaveParam::vsync, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::vsync)));
										 if (OptionParts->GetParamBoolean(EnumSaveParam::vsync)) {
											 OptionParts->SetParamInt(EnumSaveParam::FpsLimit, RefreshRate);
										 }
										 if (prev != OptionParts->GetParamBoolean(EnumSaveParam::vsync)) {
											 if (OptionParts->GetParamBoolean(EnumSaveParam::vsync)) {
												 OptionParts->SetParamInt(EnumSaveParam::FpsLimit, RefreshRate);
											 }
										 }
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("FPS Limit", 1124,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();

										 int value = OptionParts->GetParamInt(EnumSaveParam::FpsLimit);
										 bool isHit = false;
										 for (int i = 0;i < FrameLimitsNum;i++) {
											 if (FrameLimits[static_cast<size_t>(i)] == value) {
												 i--;
												 if (i < 0) { i = FrameLimitsNum - 1; }
												 value = FrameLimits[static_cast<size_t>(i)];
												 isHit = true;
												 break;
											 }
										 }
										 if (!isHit) {
											 value = FrameLimits[1];
										 }

										 OptionParts->SetParamInt(EnumSaveParam::FpsLimit, value);
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 int value = OptionParts->GetParamInt(EnumSaveParam::FpsLimit);
										 bool isHit = false;
										 for (int i = 0;i < FrameLimitsNum;i++) {
											 if (FrameLimits[static_cast<size_t>(i)] == value) {
												 i++;
												 if (i > FrameLimitsNum - 1) { i = 0; }
												 value = FrameLimits[static_cast<size_t>(i)];
												 isHit = true;
												 break;
											 }
										 }
										 if (!isHit) {
											 value = FrameLimits[1];
										 }
										 OptionParts->SetParamInt(EnumSaveParam::FpsLimit, value);
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
											[&](int xpos, int ypos, bool) {
												auto* OptionParts = OPTION::Instance();
												auto* DrawParts = DXDraw::Instance();
												int ret = 0;
												//結果から一番近いやつに指定
												int diff = 10000;
												for (int i = 0; i < FrameLimitsNum; i++) {
													int tmp = std::abs(FrameLimits[static_cast<size_t>(i)] - OptionParts->GetParamInt(EnumSaveParam::FpsLimit));
													if (diff > tmp) {
														diff = tmp;
														ret = i;
													}
												}
												int value = WindowSystem::UpDownBox(xpos, xpos + DrawParts->GetUIY(200), ypos, ret, FrameLimitsNum);
												OptionParts->SetParamInt(EnumSaveParam::FpsLimit, FrameLimits[value]);
												WindowSystem::SetMsgWW(xpos + DrawParts->GetUIY(250), ypos + LineHeight/2, 
													LineHeight, FontHandle::FontXCenter::RIGHT, White, Black, "%d", OptionParts->GetParamInt(EnumSaveParam::FpsLimit));
											}
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("DirectX Version", 1125,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::DirectXVer, 1 - OptionParts->GetParamInt(EnumSaveParam::DirectXVer));
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 OptionWindowClass::Instance()->SetRestart();
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::DirectXVer, 1 - OptionParts->GetParamInt(EnumSaveParam::DirectXVer));
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 OptionWindowClass::Instance()->SetRestart();
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* DrawParts = DXDraw::Instance();
										 auto* OptionParts = OPTION::Instance();
										 auto prev = OptionParts->GetParamInt(EnumSaveParam::DirectXVer);
										 OptionParts->SetParamInt(EnumSaveParam::DirectXVer, WindowSystem::CheckBox(xpos, ypos, (OptionParts->GetParamInt(EnumSaveParam::DirectXVer) == 1)) ? 1 : 0);
										 if (prev != OptionParts->GetParamInt(EnumSaveParam::DirectXVer)) {
											 OptionWindowClass::Instance()->SetRestart();
										 }
										 WindowSystem::SetMsgWW(xpos + DrawParts->GetUIY(100), ypos + LineHeight / 2,
																LineHeight, FontHandle::FontXCenter::MIDDLE, White, Black, DirectXVerStr[OptionParts->GetParamInt(EnumSaveParam::DirectXVer)]);
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("AntiAlias", 1126,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->ChangeParamBoolean(EnumSaveParam::AA);
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->ChangeParamBoolean(EnumSaveParam::AA);
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamBoolean(EnumSaveParam::AA, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::AA)));
									 }
									 );

		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("SSAO", 1127,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->ChangeParamBoolean(EnumSaveParam::SSAO);
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->ChangeParamBoolean(EnumSaveParam::SSAO);
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamBoolean(EnumSaveParam::SSAO, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::SSAO)));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Reflection", 1128,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::Reflection, std::clamp(OptionParts->GetParamInt(EnumSaveParam::Reflection) - 1, 0, 3));
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::Reflection, std::clamp(OptionParts->GetParamInt(EnumSaveParam::Reflection) + 1, 0, 3));
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 auto* DrawParts = DXDraw::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::Reflection, WindowSystem::UpDownBox(xpos, xpos + DrawParts->GetUIY(200), ypos, OptionParts->GetParamInt(EnumSaveParam::Reflection), 3));
									 }
									 );

		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Shadow", 1129,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::shadow, std::clamp(OptionParts->GetParamInt(EnumSaveParam::shadow) - 1, 0, 4));
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::shadow, std::clamp(OptionParts->GetParamInt(EnumSaveParam::shadow) + 1, 0, 4));
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* DrawParts = DXDraw::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::shadow, WindowSystem::UpDownBox(xpos, xpos + DrawParts->GetUIY(200), ypos, OptionParts->GetParamInt(EnumSaveParam::shadow), 4));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Object Level", 1130,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::ObjLevel, std::clamp(OptionParts->GetParamInt(EnumSaveParam::ObjLevel) - 1, 0, 4));
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::ObjLevel, std::clamp(OptionParts->GetParamInt(EnumSaveParam::ObjLevel) + 1, 0, 4));
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* DrawParts = DXDraw::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::ObjLevel, WindowSystem::UpDownBox(xpos, xpos + DrawParts->GetUIY(200), ypos, OptionParts->GetParamInt(EnumSaveParam::ObjLevel), 4));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Bloom Effect", 1131,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->ChangeParamBoolean(EnumSaveParam::bloom);
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->ChangeParamBoolean(EnumSaveParam::bloom);
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamBoolean(EnumSaveParam::bloom, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::bloom)));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Screen Effect", 1132,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->ChangeParamBoolean(EnumSaveParam::ScreenEffect);
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->ChangeParamBoolean(EnumSaveParam::ScreenEffect);
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::ScreenEffect)));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("DoF", 1133,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->ChangeParamBoolean(EnumSaveParam::DoF);
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->ChangeParamBoolean(EnumSaveParam::DoF);
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamBoolean(EnumSaveParam::DoF, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::DoF)));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("MotionBlur", 1134,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->ChangeParamBoolean(EnumSaveParam::MotionBlur);
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->ChangeParamBoolean(EnumSaveParam::MotionBlur);
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamBoolean(EnumSaveParam::MotionBlur, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::MotionBlur)));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Fov", 1122,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::fov, std::clamp(OptionParts->GetParamInt(EnumSaveParam::fov) - 5, 45, 110));
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::fov, std::clamp(OptionParts->GetParamInt(EnumSaveParam::fov) + 5, 45, 110));
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* DrawParts = DXDraw::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::fov, WindowSystem::UpDownBar(xpos, xpos + DrawParts->GetUIY(200), ypos, OptionParts->GetParamInt(EnumSaveParam::fov), 45, 110));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Render Scale", 1137,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamFloat(EnumSaveParam::DrawScale, std::clamp(OptionParts->GetParamFloat(EnumSaveParam::DrawScale) - 0.1f, 0.25f, 1.f));
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 OptionWindowClass::Instance()->SetRestart();
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamFloat(EnumSaveParam::DrawScale, std::clamp(OptionParts->GetParamFloat(EnumSaveParam::DrawScale) + 0.1f, 0.25f, 1.f));
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 OptionWindowClass::Instance()->SetRestart();
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* DrawParts = DXDraw::Instance();
										 auto* OptionParts = OPTION::Instance();
										 int prev = static_cast<int>(OptionParts->GetParamFloat(EnumSaveParam::DrawScale)*100.f + 0.5f);
										 int value = WindowSystem::UpDownBar(xpos, xpos + DrawParts->GetUIY(200), ypos, static_cast<int>(OptionParts->GetParamFloat(EnumSaveParam::DrawScale)*100.f + 0.5f), 25, 100);
										 OptionParts->SetParamFloat(EnumSaveParam::DrawScale, static_cast<float>(value) / 100.f);
										 if (prev != value) {
											 OptionWindowClass::Instance()->SetRestart();
										 }
									 }
									 );
	}
	void OptionWindowClass::ElseTabsInfo::Init_Sub(void) noexcept {
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Language", 1145,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 switch ((LanguageType)OptionParts->GetParamInt(EnumSaveParam::Language)) {
											 case LanguageType::Eng:
												 OptionParts->SetParamInt(EnumSaveParam::Language, static_cast<int>(LanguageType::Jpn));
												 break;
											 case LanguageType::Jpn:
												 OptionParts->SetParamInt(EnumSaveParam::Language, static_cast<int>(LanguageType::Eng));
												 break;
											 default:
												 break;
										 }
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 OptionWindowClass::Instance()->SetRestart();

										 auto* LocalizeParts = LocalizePool::Instance();
										 LocalizeParts->Dispose();
										 LocalizeParts->Load(LanguageStr[OptionParts->GetParamInt(EnumSaveParam::Language)]);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 switch ((LanguageType)OptionParts->GetParamInt(EnumSaveParam::Language)) {
											 case LanguageType::Eng:
												 OptionParts->SetParamInt(EnumSaveParam::Language, static_cast<int>(LanguageType::Jpn));
												 break;
											 case LanguageType::Jpn:
												 OptionParts->SetParamInt(EnumSaveParam::Language, static_cast<int>(LanguageType::Eng));
												 break;
											 default:
												 break;
										 }
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 OptionWindowClass::Instance()->SetRestart();

										 auto* LocalizeParts = LocalizePool::Instance();
										 LocalizeParts->Dispose();
										 LocalizeParts->Load(LanguageStr[OptionParts->GetParamInt(EnumSaveParam::Language)]);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* DrawParts = DXDraw::Instance();
										 auto* OptionParts = OPTION::Instance();
										 auto prev = OptionParts->GetParamInt(EnumSaveParam::Language);
										 if (WindowSystem::CheckBox(xpos, ypos, (OptionParts->GetParamInt(EnumSaveParam::Language) == static_cast<int>(LanguageType::Eng)))) {
											 OptionParts->SetParamInt(EnumSaveParam::Language, static_cast<int>(LanguageType::Eng));
										 }
										 else {
											 OptionParts->SetParamInt(EnumSaveParam::Language, static_cast<int>(LanguageType::Jpn));
										 }
										 if (prev != OptionParts->GetParamInt(EnumSaveParam::Language)) {
											 OptionWindowClass::Instance()->SetRestart();
											 auto* LocalizeParts = LocalizePool::Instance();
											 LocalizeParts->Dispose();
											 LocalizeParts->Load(LanguageStr[OptionParts->GetParamInt(EnumSaveParam::Language)]);
										 }

										 WindowSystem::SetMsgWW(xpos + DrawParts->GetUIY(125), ypos + LineHeight/2, 
																LineHeight, FontHandle::FontXCenter::MIDDLE, White, Black, LanguageStr[OptionParts->GetParamInt(EnumSaveParam::Language)]);
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("X sensing", 1140,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamFloat(EnumSaveParam::Xsensing, std::clamp(OptionParts->GetParamFloat(EnumSaveParam::Xsensing) - 0.01f, 0.01f, 1.f));
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamFloat(EnumSaveParam::Xsensing, std::clamp(OptionParts->GetParamFloat(EnumSaveParam::Xsensing) + 0.01f, 0.01f, 1.f));
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* DrawParts = DXDraw::Instance();
										 auto* OptionParts = OPTION::Instance();
										 int value = WindowSystem::UpDownBar(xpos, xpos + DrawParts->GetUIY(200), ypos, static_cast<int>(OptionParts->GetParamFloat(EnumSaveParam::Xsensing)*100.f + 0.5f), 10, 100);
										 OptionParts->SetParamFloat(EnumSaveParam::Xsensing, static_cast<float>(value) / 100.f);
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Y sensing", 1141,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamFloat(EnumSaveParam::Ysensing, std::clamp(OptionParts->GetParamFloat(EnumSaveParam::Ysensing) - 0.01f, 0.f, 1.f));
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamFloat(EnumSaveParam::Ysensing, std::clamp(OptionParts->GetParamFloat(EnumSaveParam::Ysensing) + 0.01f, 0.f, 1.f));
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* DrawParts = DXDraw::Instance();
										 auto* OptionParts = OPTION::Instance();
										 int value = WindowSystem::UpDownBar(xpos, xpos + DrawParts->GetUIY(200), ypos, static_cast<int>(OptionParts->GetParamFloat(EnumSaveParam::Ysensing)*100.f + 0.5f), 10, 100);
										 OptionParts->SetParamFloat(EnumSaveParam::Ysensing, static_cast<float>(value) / 100.f);
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("HeadBobbing", 1142,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->ChangeParamBoolean(EnumSaveParam::HeadBobbing);
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->ChangeParamBoolean(EnumSaveParam::HeadBobbing);
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamBoolean(EnumSaveParam::HeadBobbing, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing)));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("GamePadType", 1144,
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 switch ((ControlType)OptionParts->GetParamInt(EnumSaveParam::ControlType)) {
											 case ControlType::PS4:
												 OptionParts->SetParamInt(EnumSaveParam::ControlType, static_cast<int>(ControlType::XBox));
												 break;
											 case ControlType::XBox:
												 OptionParts->SetParamInt(EnumSaveParam::ControlType, static_cast<int>(ControlType::PS4));
												 break;
											 case ControlType::PC:
											 default:
												 break;
										 }
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 switch ((ControlType)OptionParts->GetParamInt(EnumSaveParam::ControlType)) {
											 case ControlType::PS4:
												 OptionParts->SetParamInt(EnumSaveParam::ControlType, static_cast<int>(ControlType::XBox));
												 break;
											 case ControlType::XBox:
												 OptionParts->SetParamInt(EnumSaveParam::ControlType, static_cast<int>(ControlType::PS4));
												 break;
											 case ControlType::PC:
											 default:
												 break;
										 }
										 SoundPool::Instance()->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* DrawParts = DXDraw::Instance();
										 auto* OptionParts = OPTION::Instance();
										 if (WindowSystem::CheckBox(xpos, ypos, (OptionParts->GetParamInt(EnumSaveParam::ControlType) == static_cast<int>(ControlType::PS4)))) {
											 OptionParts->SetParamInt(EnumSaveParam::ControlType, static_cast<int>(ControlType::PS4));
										 }
										 else {
											 OptionParts->SetParamInt(EnumSaveParam::ControlType, static_cast<int>(ControlType::XBox));
										 }
										 ypos -= LineHeight * 1 / 6;
										 if (OptionParts->GetParamInt(EnumSaveParam::ControlType) == static_cast<int>(ControlType::XBox)) {
											 WindowSystem::SetMsgWW(xpos + DrawParts->GetUIY(125), ypos + LineHeight / 3, 
																	LineHeight * 2 / 3, FontHandle::FontXCenter::MIDDLE, White, Black, "XInput");
										 }
										 else {
											 WindowSystem::SetMsgWW(xpos + DrawParts->GetUIY(125), ypos + LineHeight / 3, 
																	LineHeight * 2 / 3, FontHandle::FontXCenter::MIDDLE, White, Black, "DirectInput");
										 }

										 if (GetJoypadNum() > 0) {
											 switch (GetJoypadType(DX_INPUT_PAD1)) {
												 case DX_PADTYPE_OTHER:
												 case DX_PADTYPE_DUAL_SHOCK_4:
												 case DX_PADTYPE_DUAL_SENSE:
												 case DX_PADTYPE_SWITCH_JOY_CON_L:
												 case DX_PADTYPE_SWITCH_JOY_CON_R:
												 case DX_PADTYPE_SWITCH_PRO_CTRL:
												 case DX_PADTYPE_SWITCH_HORI_PAD:
													 WindowSystem::SetMsgWW(xpos + DrawParts->GetUIY(125), ypos + LineHeight * 3 / 3, 
																			LineHeight * 2 / 3, FontHandle::FontXCenter::MIDDLE, White, Black, "推奨:DirectInput");
													 break;
												 case DX_PADTYPE_XBOX_360:
												 case DX_PADTYPE_XBOX_ONE:
													 WindowSystem::SetMsgWW(xpos + DrawParts->GetUIY(125), ypos + LineHeight * 3 / 3, 
																			LineHeight * 2 / 3, FontHandle::FontXCenter::MIDDLE, White, Black, "推奨:XInput");
													 break;
												 default:
													 break;
											 }
										 }
									 }
									 );
	}
	void OptionWindowClass::ControlTabsInfo::KeyDraw(int xpos, int ypos, bool isMine, int Sel) noexcept {
		auto* Pad = PadControl::Instance();
		auto* DrawParts = DXDraw::Instance();
		WindowSystem::SetMsgWW(xpos + DrawParts->GetUIY(100), ypos + LineHeight/2,
							  LineHeight, FontHandle::FontXCenter::MIDDLE, (Pad->GetKeyReserve((PADS)Sel) < 0) ? Red : (isMine ? White : Gray25), Black,
							  "[%s]->[%s]", Pad->GetIDtoStr(Pad->GetKeyassign((PADS)Sel)).c_str(), Pad->GetIDtoStr(Pad->GetKeyReserve((PADS)Sel)).c_str());

	}


	void OptionWindowClass::ControlTabsInfo::Init_Sub(void) noexcept {
		auto* LocalizeParts = LocalizePool::Instance();
		const int KeyInfo = 1151;

		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Reset", 1150,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&](int xpos, int ypos, bool isMine) {
										 auto* DrawParts = DXDraw::Instance();
										 if (isMine && PadControl::Instance()->GetMouseClick().trigger()) {
											 PadControl::Instance()->ResetAssign();
										 }
										 WindowSystem::SetMsgWW(xpos + DrawParts->GetUIY(100), ypos + LineHeight / 2, LineHeight, FontHandle::FontXCenter::MIDDLE, isMine ? White : Gray25, Black, "LMB Click");
									 }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizeParts->Get(1153), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)0); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 0); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizeParts->Get(1154), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)2); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 2); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizeParts->Get(1155), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)1); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 1); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizeParts->Get(1156), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)3); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 3); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizeParts->Get(1157), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)10); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 10); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizeParts->Get(1158), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)11); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 11); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizeParts->Get(1159), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)12); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 12); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizeParts->Get(1160), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)13); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 13); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizeParts->Get(1161), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)14); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 14); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizeParts->Get(1162), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)15); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 15); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizeParts->Get(1163), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)16); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 16); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizeParts->Get(1164), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)18); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 18); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizeParts->Get(1165), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)19); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 19); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizeParts->Get(1166), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)20); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 20); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizeParts->Get(1167), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)21); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 21); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizeParts->Get(1168), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)22); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 22); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizeParts->Get(1169), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)23); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 23); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizeParts->Get(1170), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)24); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 24); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizeParts->Get(1171), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)25); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 25); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Save", 1152,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&](int xpos, int ypos, bool isMine) {
										 auto* Pad = PadControl::Instance();
										 auto* DrawParts = DXDraw::Instance();
										 if (isMine && Pad->GetMouseClick().trigger()) {
											 Pad->FlipAssign();
											 Pad->Save();
										 }
										 WindowSystem::SetMsgWW(xpos + DrawParts->GetUIY(100), ypos + LineHeight / 2, LineHeight, FontHandle::FontXCenter::MIDDLE, isMine ? White : Gray25, Black, "LMB Click");
									 }
		);
	}
	//
	void OptionWindowClass::Init(void) noexcept {
		//
		m_Tabs.at(0) = std::make_unique<SoundTabsInfo>();
		m_Tabs.at(0)->Init(0, "Sound");
		m_Tabs.at(1) = std::make_unique<GraphicTabsInfo>();
		m_Tabs.at(1)->Init(1, "Graphic");
		m_Tabs.at(2) = std::make_unique<ElseTabsInfo>();
		m_Tabs.at(2)->Init(2, "Else");
		m_Tabs.at(3) = std::make_unique<ControlTabsInfo>();
		m_Tabs.at(3)->Init(3, "Control");
		//
	}
	void OptionWindowClass::Execute(void) noexcept {
		if (m_ActiveSwitch) {
			m_ActiveSwitch = false;
			m_Active = true;
			auto* PopUpParts = PopUp::Instance();
			PopUpParts->Add("Option", 720, 720,
							  [&](int xmin, int ymin, int, int ymax, bool EndSwitch) {
								auto* DrawParts = DXDraw::Instance();
								auto* OptionParts = OPTION::Instance();
									auto* Pad = PadControl::Instance();
								  auto* SE = SoundPool::Instance();
								  int xp1, yp1;


								  xp1 = xmin + DrawParts->GetUIY(24);
								  yp1 = ymin;
								  for (auto& t : m_Tabs) {
									  t->Draw(xp1, yp1, m_tabsel == t->GetID(), &m_tabsel, &m_select);
								  }
								  //ガイド
								  xp1 = xmin + DrawParts->GetUIY(24);
								  yp1 = ymax - LineHeight * 3 / 2;
								  m_Tabs.at(static_cast<size_t>(m_tabsel))->DrawInfo(xp1, yp1, m_select);

								  //
								  if (Pad->GetKey(PADS::LEAN_L).trigger() && (m_tabsel != 3)) {
									  --m_tabsel;
									  if (m_tabsel < 0) { m_tabsel = static_cast<int>(m_Tabs.size()) - 1; }
									  m_select = 0;
									  SE->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
								  }
								  if (Pad->GetKey(PADS::LEAN_R).trigger() && (m_tabsel != 3)) {
									  ++m_tabsel;
									  if (m_tabsel > static_cast<int>(m_Tabs.size()) - 1) { m_tabsel = 0; }
									  m_select = 0;
									  SE->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
								  }

								  m_Tabs.at(static_cast<size_t>(m_tabsel))->Execute(&m_select, (m_tabsel != 3));
								  //
								  if (EndSwitch) {
									  Pad->SetGuideUpdate();
									  OptionParts->Save();
									  Pad->Save();
									  m_tabsel = 0;
									  m_select = 0;
								  }
							  },
							  [&]() {m_Active = false;},
								  [&]() {
								  auto* KeyGuide = PadControl::Instance();
								  auto* LocalizeParts = LocalizePool::Instance();

								  KeyGuide->AddGuide(PADS::INTERACT, LocalizeParts->Get(9992));

								  KeyGuide->AddGuide(PADS::LEAN_L, "");
								  KeyGuide->AddGuide(PADS::LEAN_R, LocalizeParts->Get(9994));
								  KeyGuide->AddGuide(PADS::MOVE_W, "");
								  KeyGuide->AddGuide(PADS::MOVE_A, "");
								  KeyGuide->AddGuide(PADS::MOVE_S, "");
								  KeyGuide->AddGuide(PADS::MOVE_D, "");
								  KeyGuide->AddGuide(PADS::MOVE_STICK, LocalizeParts->Get(9993));
							  },
								  true
								  );
		}
	}
};
