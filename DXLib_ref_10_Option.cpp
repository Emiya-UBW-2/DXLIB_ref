#include "DXLib_ref_09_KeyControl.hpp"
#include "DXLib_ref_10_Option.hpp"
#include "DXLib_ref_08_Etc.hpp"


namespace DXLib_ref {
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	const OPTION* SingletonBase<OPTION>::m_Singleton = nullptr;

	void			OPTION::Load(void) noexcept {
		m_SaveParams.at((int)EnumSaveParam::LightMode).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at((int)EnumSaveParam::DirectXVer).SetEnumParamType(EnumParamType::Else);//DirectXVer
		m_SaveParams.at((int)EnumSaveParam::usevr).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at((int)EnumSaveParam::grass_level).SetEnumParamType(EnumParamType::Int);
		m_SaveParams.at((int)EnumSaveParam::DoF).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at((int)EnumSaveParam::bloom).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at((int)EnumSaveParam::shadow).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at((int)EnumSaveParam::SSAO).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at((int)EnumSaveParam::fov).SetEnumParamType(EnumParamType::Int);
		m_SaveParams.at((int)EnumSaveParam::vsync).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at((int)EnumSaveParam::FpsLimit).SetEnumParamType(EnumParamType::Int);
		m_SaveParams.at((int)EnumSaveParam::SE).SetEnumParamType(EnumParamType::Float);
		m_SaveParams.at((int)EnumSaveParam::VOICE).SetEnumParamType(EnumParamType::Float);
		m_SaveParams.at((int)EnumSaveParam::BGM).SetEnumParamType(EnumParamType::Float);
		m_SaveParams.at((int)EnumSaveParam::AllWaysFront).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at((int)EnumSaveParam::aberration).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at((int)EnumSaveParam::SSR).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at((int)EnumSaveParam::MotionBlur).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at((int)EnumSaveParam::Xsensing).SetEnumParamType(EnumParamType::Float);
		m_SaveParams.at((int)EnumSaveParam::Ysensing).SetEnumParamType(EnumParamType::Float);
		m_SaveParams.at((int)EnumSaveParam::HeadBobbing).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at((int)EnumSaveParam::EnableCheck).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at((int)EnumSaveParam::ControlType).SetEnumParamType(EnumParamType::Else);//ControlType
		m_SaveParams.at((int)EnumSaveParam::Language).SetEnumParamType(EnumParamType::Else);//Language
		m_SaveParams.at((int)EnumSaveParam::EX_UI).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at((int)EnumSaveParam::EX_UI2).SetEnumParamType(EnumParamType::Boolean);
		m_SaveParams.at((int)EnumSaveParam::AA).SetEnumParamType(EnumParamType::Boolean);

		//SetOutApplicationLogValidFlag(FALSE);
		int mdata = -1;
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
				SetParamBoolean(EnumSaveParam::LightMode, false);
				SetParamInt(EnumSaveParam::DirectXVer, 1);
				SetParamBoolean(EnumSaveParam::usevr, false);
				SetParamInt(EnumSaveParam::grass_level, 1);
				SetParamBoolean(EnumSaveParam::DoF, true);
				SetParamBoolean(EnumSaveParam::bloom, true);
				SetParamBoolean(EnumSaveParam::shadow, true);
				SetParamBoolean(EnumSaveParam::SSAO, true);
				SetParamInt(EnumSaveParam::fov, 90);
				SetParamBoolean(EnumSaveParam::vsync, true);
				SetParamInt(EnumSaveParam::FpsLimit, 60);
				SetParamFloat(EnumSaveParam::SE, 1.f);
				SetParamFloat(EnumSaveParam::VOICE, 1.f);
				SetParamFloat(EnumSaveParam::BGM, 1.f);
				SetParamBoolean(EnumSaveParam::AllWaysFront, false);
				SetParamBoolean(EnumSaveParam::aberration, true);
				SetParamBoolean(EnumSaveParam::SSR, true);
				SetParamBoolean(EnumSaveParam::MotionBlur, false);
				SetParamFloat(EnumSaveParam::Xsensing, 0.5f);
				SetParamFloat(EnumSaveParam::Ysensing, 0.5f);
				SetParamBoolean(EnumSaveParam::HeadBobbing, true);
				SetParamBoolean(EnumSaveParam::EnableCheck, true);
				SetParamInt(EnumSaveParam::ControlType, 2);
				SetParamBoolean(EnumSaveParam::EX_UI, true);
				SetParamBoolean(EnumSaveParam::EX_UI2, true);
				SetParamBoolean(EnumSaveParam::AA, true);
				return;
			}
		}
		while (true) {
			if (FileRead_eof(mdata) != 0) { break; }
			auto ALL = getparams::Getstr(mdata);
			if (ALL == "") { continue; }
			auto LEFT = getparams::getleft(ALL);
			auto RIGHT = getparams::getright(ALL);
			for (int loop = 0;loop < (int)EnumSaveParam::Max;loop++) {
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
						if (loop == (int)EnumSaveParam::DirectXVer) {
							for (int i = 0;i < 2;i++) {
								if (RIGHT == DirectXVerStr[i]) {
									SetParamInt((EnumSaveParam)loop, i);
									break;
								}
							}
						}
						else if (loop == (int)EnumSaveParam::ControlType) {
							for (int i = 1;i < 3;i++) {
								if (RIGHT == ControlTypeStr[i]) {
									SetParamInt((EnumSaveParam)loop, i);
									break;
								}
							}
						}
						else if (loop == (int)EnumSaveParam::Language && !NewData) {
							for (int i = 0;i < 2;i++) {
								if (RIGHT == LanguageStr[i]) {
									SetParamInt((EnumSaveParam)loop, i);
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

		for (int loop = 0;loop < (int)EnumSaveParam::Max;loop++) {
			switch (m_SaveParams.at(loop).GetEnumParamType()) {
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
					if (loop == (int)EnumSaveParam::DirectXVer) {
						outputfile << std::string(OptionStr[loop]) + "=" + std::string(DirectXVerStr[GetParamInt((EnumSaveParam)loop)]) + "\n";
					}
					else if (loop == (int)EnumSaveParam::ControlType) {
						outputfile << std::string(OptionStr[loop]) + "=" + std::string(ControlTypeStr[GetParamInt((EnumSaveParam)loop)]) + "\n";
					}
					else if (loop == (int)EnumSaveParam::Language) {
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
		ypos += y_r((int)selanim);
		WindowSystem::SetMsgWW(xpos, ypos, xpos, ypos + LineHeight,
							   LineHeight, FontHandle::FontXCenter::LEFT, isMine ? White : Gray50, Black, m_Name);
		m_Draw(xpos + y_r(720 - 324), ypos, isMine);
	}
	//
	void OptionWindowClass::OptionTabsInfo::Execute(int *select, bool CanPress) noexcept {
		if ((*select) < 0) { return; }
		auto* SE = SoundPool::Instance();
		auto* Pad = PadControl::Instance();
		m_Elements.at((*select)).GetAnyDoing();
		if (CanPress) {
			if (Pad->GetKey(PADS::MOVE_W).trigger()) {
				--(*select);
				if ((*select) < 0) { (*select) = (int)m_Elements.size() - 1; }
				m_Elements.at((*select)).selanim = 3.f;
				SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}
			if (Pad->GetKey(PADS::MOVE_S).trigger()) {
				++(*select);
				if ((*select) > (int)m_Elements.size() - 1) { (*select) = 0; }
				m_Elements.at((*select)).selanim = -3.f;
				SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}
			if (Pad->GetKey(PADS::MOVE_A).repeat()) {
				m_Elements.at((*select)).GetLeftPush();
			}
			if (Pad->GetKey(PADS::MOVE_D).repeat()) {
				m_Elements.at((*select)).GetRightPush();
			}
			if (Pad->GetKey(PADS::INTERACT).trigger() || Pad->GetMouseClick().trigger()) {
				m_Elements.at((*select)).GetOKPush();
			}
		}
		for (auto& e : m_Elements) {
			Easing(&e.selanim, 0.f, 0.95f, EasingType::OutExpo);
		}
	}
	void OptionWindowClass::OptionTabsInfo::Draw(int xpos, int ypos, bool isActive, int* TabSel, int *select) noexcept {
		int xp1, yp1;
		//タブ
		{
			xp1 = xpos + (y_r(140) + y_r(12)) * m_id;
			yp1 = ypos;
			if (WindowSystem::SetMsgClickBox(xp1, yp1 + y_r(5), xp1 + y_r(140), yp1 + LineHeight * 2 - y_r(5), (isActive ? Gray25 : Gray75), m_name)) {
				*TabSel = GetID();
				*select = 0;
				auto* SE = SoundPool::Instance();
				SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}
		}
		//内容
		if (isActive) {
			xp1 = xpos;
			yp1 = ypos + LineHeight * 2;
			auto* Pad = PadControl::Instance();
			for (int i = 0; i < (int)m_Elements.size(); i++) {
				yp1 += (LineHeight + y_r(6));
				if (in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xp1, yp1, xp1 + y_r(500), yp1 + (LineHeight + y_r(6)))) {
					*select = i;
				}
				m_Elements.at(i).Draw(xp1, yp1, (*select == i));
			}
		}
	}
	void OptionWindowClass::OptionTabsInfo::DrawInfo(int xpos, int ypos, int select) noexcept {
		WindowSystem::SetMsgWW(xpos, ypos, xpos, ypos + LineHeight, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, m_Elements.at(select).GetInfo());
	}
	//
	void OptionWindowClass::SoundTabsInfo::Init_Sub() noexcept {
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("BGM", LocalizePool::Instance()->Get(1110),
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamFloat(EnumSaveParam::BGM, std::clamp(OptionParts->GetParamFloat(EnumSaveParam::BGM) - 0.1f, 0.f, 1.f));
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 BGMPool::Instance()->SetVol(OptionParts->GetParamFloat(EnumSaveParam::BGM));
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamFloat(EnumSaveParam::BGM, std::clamp(OptionParts->GetParamFloat(EnumSaveParam::BGM) + 0.1f, 0.f, 1.f));
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 BGMPool::Instance()->SetVol(OptionParts->GetParamFloat(EnumSaveParam::BGM));
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 int value = WindowSystem::UpDownBar(xpos, xpos + y_r(200), ypos, (int)(OptionParts->GetParamFloat(EnumSaveParam::BGM)*100.f + 0.5f), 0, 100);
										 OptionParts->SetParamFloat(EnumSaveParam::BGM, (float)value / 100.f);
										 BGMPool::Instance()->SetVol(OptionParts->GetParamFloat(EnumSaveParam::BGM));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("SE", LocalizePool::Instance()->Get(1111),
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamFloat(EnumSaveParam::SE, std::clamp(OptionParts->GetParamFloat(EnumSaveParam::SE) - 0.1f, 0.f, 1.f));
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 SE->SetVol(OptionParts->GetParamFloat(EnumSaveParam::SE));
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamFloat(EnumSaveParam::SE, std::clamp(OptionParts->GetParamFloat(EnumSaveParam::SE) + 0.1f, 0.f, 1.f));
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 SE->SetVol(OptionParts->GetParamFloat(EnumSaveParam::SE));
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 int value = WindowSystem::UpDownBar(xpos, xpos + y_r(200), ypos, (int)(OptionParts->GetParamFloat(EnumSaveParam::SE)*100.f + 0.5f), 0, 100);
										 OptionParts->SetParamFloat(EnumSaveParam::SE, (float)value / 100.f);
										 SE->SetVol(OptionParts->GetParamFloat(EnumSaveParam::SE));
									 }
									 );
	}
	void OptionWindowClass::GraphicTabsInfo::Init_Sub() noexcept {
		HDC hdc;
		hdc = GetDC(GetMainWindowHandle());	// デバイスコンテキストの取得
		RefreshRate = GetDeviceCaps(hdc, VREFRESH);	// リフレッシュレートの取得
		ReleaseDC(GetMainWindowHandle(), hdc);	// デバイスコンテキストの解放

		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Light Mode", LocalizePool::Instance()->Get(1120),
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::LightMode);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 OptionWindowClass::Instance()->SetRestart();
									 },
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::LightMode);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 OptionWindowClass::Instance()->SetRestart();
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 auto prev = OptionParts->GetParamBoolean(EnumSaveParam::LightMode);
										 OptionParts->SetParamBoolean(EnumSaveParam::LightMode, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::LightMode)));
										 if (prev != OptionParts->GetParamBoolean(EnumSaveParam::LightMode)) {
											 OptionWindowClass::Instance()->SetRestart();
										 }
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Window Mode", LocalizePool::Instance()->Get(1121),
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::AllWaysFront);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 OptionWindowClass::Instance()->SetRestart();
									 },
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::AllWaysFront);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 OptionWindowClass::Instance()->SetRestart();
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 auto prev = OptionParts->GetParamBoolean(EnumSaveParam::AllWaysFront);

										 OptionParts->SetParamBoolean(EnumSaveParam::AllWaysFront, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::AllWaysFront)));
										 if (prev != OptionParts->GetParamBoolean(EnumSaveParam::AllWaysFront)) {
											 OptionWindowClass::Instance()->SetRestart();
										 }

										 WindowSystem::SetMsgWW(xpos + y_r(100), ypos, xpos + y_r(100), ypos + LineHeight,
																LineHeight, FontHandle::FontXCenter::MIDDLE, White, Black, OptionParts->GetParamBoolean(EnumSaveParam::AllWaysFront) ? LocalizePool::Instance()->Get(1135) : LocalizePool::Instance()->Get(1136));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Fov", LocalizePool::Instance()->Get(1122),
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::fov, std::clamp(OptionParts->GetParamInt(EnumSaveParam::fov) - 5, 45, 110));
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::fov, std::clamp(OptionParts->GetParamInt(EnumSaveParam::fov) + 5, 45, 110));
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::fov, WindowSystem::UpDownBar(xpos, xpos + y_r(200), ypos, OptionParts->GetParamInt(EnumSaveParam::fov), 45, 110));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("V Sync", LocalizePool::Instance()->Get(1123),
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->ChangeParamBoolean(EnumSaveParam::vsync);
										 if (OptionParts->GetParamBoolean(EnumSaveParam::vsync)) {
											 OptionParts->SetParamInt(EnumSaveParam::FpsLimit, RefreshRate);
										 }
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 OptionWindowClass::Instance()->SetRestart();
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->ChangeParamBoolean(EnumSaveParam::vsync);
										 if (OptionParts->GetParamBoolean(EnumSaveParam::vsync)) {
											 OptionParts->SetParamInt(EnumSaveParam::FpsLimit, RefreshRate);
										 }
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 OptionWindowClass::Instance()->SetRestart();
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
											 OptionWindowClass::Instance()->SetRestart();
										 }
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("FPS Limit", LocalizePool::Instance()->Get(1124),
									 [&]() {
										 auto* OptionParts = OPTION::Instance();

										 int value = OptionParts->GetParamInt(EnumSaveParam::FpsLimit);
										 bool isHit = false;
										 for (int i = 0;i < FrameLimitsNum;i++) {
											 if (FrameLimits[i] == value) {
												 i--;
												 if (i < 0) { i = FrameLimitsNum - 1; }
												 value = FrameLimits[i];
												 isHit = true;
												 break;
											 }
										 }
										 if (!isHit) {
											 value = FrameLimits[1];
										 }

										 OptionParts->SetParamInt(EnumSaveParam::FpsLimit, value);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 int value = OptionParts->GetParamInt(EnumSaveParam::FpsLimit);
										 bool isHit = false;
										 for (int i = 0;i < FrameLimitsNum;i++) {
											 if (FrameLimits[i] == value) {
												 i++;
												 if (i > FrameLimitsNum - 1) { i = 0; }
												 value = FrameLimits[i];
												 isHit = true;
												 break;
											 }
										 }
										 if (!isHit) {
											 value = FrameLimits[1];
										 }
										 OptionParts->SetParamInt(EnumSaveParam::FpsLimit, value);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 int value = WindowSystem::UpDownBar(xpos, xpos + y_r(200), ypos, OptionParts->GetParamInt(EnumSaveParam::FpsLimit), FrameLimits[0], FrameLimits[FrameLimitsNum - 1]);
										 //結果から一番近いやつに指定
										 int diff = 10000;
										 int valuetmp = value;
										 for (int i = 0;i < FrameLimitsNum;i++) {
											 int tmp = std::abs(FrameLimits[i] - value);
											 if (diff > tmp) {
												 diff = tmp;
												 valuetmp = FrameLimits[i];
											 }
										 }
										 value = valuetmp;

										 OptionParts->SetParamInt(EnumSaveParam::FpsLimit, value);
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("DirectX Version", LocalizePool::Instance()->Get(1125),
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::DirectXVer, 1 - OptionParts->GetParamInt(EnumSaveParam::DirectXVer));
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 OptionWindowClass::Instance()->SetRestart();
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::DirectXVer, 1 - OptionParts->GetParamInt(EnumSaveParam::DirectXVer));
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 OptionWindowClass::Instance()->SetRestart();
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 auto prev = OptionParts->GetParamInt(EnumSaveParam::DirectXVer);
										 OptionParts->SetParamInt(EnumSaveParam::DirectXVer, WindowSystem::CheckBox(xpos, ypos, (OptionParts->GetParamInt(EnumSaveParam::DirectXVer) == 1)) ? 1 : 0);
										 if (prev != OptionParts->GetParamInt(EnumSaveParam::DirectXVer)) {
											 OptionWindowClass::Instance()->SetRestart();
										 }
										 WindowSystem::SetMsgWW(xpos + y_r(100), ypos, xpos + y_r(100), ypos + LineHeight,
																LineHeight, FontHandle::FontXCenter::MIDDLE, White, Black, DirectXVerStr[OptionParts->GetParamInt(EnumSaveParam::DirectXVer)]);
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("AntiAlias", LocalizePool::Instance()->Get(1126),
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::AA);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::AA);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamBoolean(EnumSaveParam::AA, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::AA)));
									 }
									 );

		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("SSAO", LocalizePool::Instance()->Get(1127),
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::SSAO);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::SSAO);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamBoolean(EnumSaveParam::SSAO, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::SSAO)));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("SSR", LocalizePool::Instance()->Get(1128),
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::SSR);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::SSR);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamBoolean(EnumSaveParam::SSR, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::SSR)));
									 }
									 );

		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Shadow", LocalizePool::Instance()->Get(1129),
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::shadow);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::shadow);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamBoolean(EnumSaveParam::shadow, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::shadow)));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Object", LocalizePool::Instance()->Get(1130),
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::grass_level, 1 - OptionParts->GetParamInt(EnumSaveParam::grass_level));
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::grass_level, 1 - OptionParts->GetParamInt(EnumSaveParam::grass_level));
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamInt(EnumSaveParam::grass_level, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamInt(EnumSaveParam::grass_level) > 0) ? 1 : 0);
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Bloom Effect", LocalizePool::Instance()->Get(1131),
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::bloom);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::bloom);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamBoolean(EnumSaveParam::bloom, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::bloom)));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Color Aberration", LocalizePool::Instance()->Get(1132),
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::aberration);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::aberration);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamBoolean(EnumSaveParam::aberration, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::aberration)));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("DoF", LocalizePool::Instance()->Get(1133),
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::DoF);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::DoF);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamBoolean(EnumSaveParam::DoF, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::DoF)));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("MotionBlur", LocalizePool::Instance()->Get(1134),
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::MotionBlur);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::MotionBlur);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamBoolean(EnumSaveParam::MotionBlur, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::MotionBlur)));
									 }
									 );
	}
	void OptionWindowClass::ElseTabsInfo::Init_Sub() noexcept {
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Language", LocalizePool::Instance()->Get(1145),
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 switch ((LanguageType)OptionParts->GetParamInt(EnumSaveParam::Language)) {
											 case  LanguageType::Eng:
												 OptionParts->SetParamInt(EnumSaveParam::Language, (int)LanguageType::Jpn);
												 break;
											 case  LanguageType::Jpn:
												 OptionParts->SetParamInt(EnumSaveParam::Language, (int)LanguageType::Eng);
												 break;
											 default:
												 break;
										 }
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 OptionWindowClass::Instance()->SetRestart();
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 switch ((LanguageType)OptionParts->GetParamInt(EnumSaveParam::Language)) {
											 case  LanguageType::Eng:
												 OptionParts->SetParamInt(EnumSaveParam::Language, (int)LanguageType::Jpn);
												 break;
											 case  LanguageType::Jpn:
												 OptionParts->SetParamInt(EnumSaveParam::Language, (int)LanguageType::Eng);
												 break;
											 default:
												 break;
										 }
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 OptionWindowClass::Instance()->SetRestart();
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 auto prev = OptionParts->GetParamInt(EnumSaveParam::Language);
										 if (WindowSystem::CheckBox(xpos, ypos, (OptionParts->GetParamInt(EnumSaveParam::Language) == (int)LanguageType::Eng))) {
											 OptionParts->SetParamInt(EnumSaveParam::Language, (int)LanguageType::Eng);
										 }
										 else {
											 OptionParts->SetParamInt(EnumSaveParam::Language, (int)LanguageType::Jpn);
										 }
										 if (prev != OptionParts->GetParamInt(EnumSaveParam::Language)) {
											 OptionWindowClass::Instance()->SetRestart();
										 }

										 WindowSystem::SetMsgWW(xpos + y_r(125), ypos, xpos + y_r(125), ypos + LineHeight,
																LineHeight, FontHandle::FontXCenter::MIDDLE, White, Black, LanguageStr[OptionParts->GetParamInt(EnumSaveParam::Language)]);
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("X sensing", LocalizePool::Instance()->Get(1140),
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamFloat(EnumSaveParam::Xsensing, std::clamp(OptionParts->GetParamFloat(EnumSaveParam::Xsensing) - 0.01f, 0.01f, 1.f));
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamFloat(EnumSaveParam::Xsensing, std::clamp(OptionParts->GetParamFloat(EnumSaveParam::Xsensing) + 0.01f, 0.01f, 1.f));
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 int value = WindowSystem::UpDownBar(xpos, xpos + y_r(200), ypos, (int)(OptionParts->GetParamFloat(EnumSaveParam::Xsensing)*100.f + 0.5f), 10, 100);
										 OptionParts->SetParamFloat(EnumSaveParam::Xsensing, (float)value / 100.f);
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Y sensing", LocalizePool::Instance()->Get(1141),
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamFloat(EnumSaveParam::Ysensing, std::clamp(OptionParts->GetParamFloat(EnumSaveParam::Ysensing) - 0.01f, 0.f, 1.f));
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamFloat(EnumSaveParam::Ysensing, std::clamp(OptionParts->GetParamFloat(EnumSaveParam::Ysensing) + 0.01f, 0.f, 1.f));
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 int value = WindowSystem::UpDownBar(xpos, xpos + y_r(200), ypos, (int)(OptionParts->GetParamFloat(EnumSaveParam::Ysensing)*100.f + 0.5f), 10, 100);
										 OptionParts->SetParamFloat(EnumSaveParam::Ysensing, (float)value / 100.f);
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("HeadBobbing", LocalizePool::Instance()->Get(1142),
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::HeadBobbing);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::HeadBobbing);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamBoolean(EnumSaveParam::HeadBobbing, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::HeadBobbing)));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("CheckMagazine", LocalizePool::Instance()->Get(1143),
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::EnableCheck);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::EnableCheck);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamBoolean(EnumSaveParam::EnableCheck, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::EnableCheck)));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("PadType", LocalizePool::Instance()->Get(1144),
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 switch ((ControlType)OptionParts->GetParamInt(EnumSaveParam::ControlType)) {
											 case  ControlType::PS4:
												 OptionParts->SetParamInt(EnumSaveParam::ControlType, (int)ControlType::XBox);
												 break;
											 case  ControlType::XBox:
												 OptionParts->SetParamInt(EnumSaveParam::ControlType, (int)ControlType::PS4);
												 break;
											 default:
												 break;
										 }
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* OptionParts = OPTION::Instance();
										 switch ((ControlType)OptionParts->GetParamInt(EnumSaveParam::ControlType)) {
											 case  ControlType::PS4:
												 OptionParts->SetParamInt(EnumSaveParam::ControlType, (int)ControlType::XBox);
												 break;
											 case  ControlType::XBox:
												 OptionParts->SetParamInt(EnumSaveParam::ControlType, (int)ControlType::PS4);
												 break;
											 default:
												 break;
										 }
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 if (WindowSystem::CheckBox(xpos, ypos, (OptionParts->GetParamInt(EnumSaveParam::ControlType) == (int)ControlType::PS4))) {
											 OptionParts->SetParamInt(EnumSaveParam::ControlType, (int)ControlType::PS4);
										 }
										 else {
											 OptionParts->SetParamInt(EnumSaveParam::ControlType, (int)ControlType::XBox);
										 }
										 ypos -= LineHeight * 1 / 6;
										 if (OptionParts->GetParamInt(EnumSaveParam::ControlType) == (int)ControlType::XBox) {
											 WindowSystem::SetMsgWW(xpos + y_r(125), ypos, xpos + y_r(125), ypos + LineHeight * 2 / 3,
																	LineHeight * 2 / 3, FontHandle::FontXCenter::MIDDLE, White, Black, "XInput");
										 }
										 else {
											 WindowSystem::SetMsgWW(xpos + y_r(125), ypos, xpos + y_r(125), ypos + LineHeight * 2 / 3,
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
													 WindowSystem::SetMsgWW(xpos + y_r(125), ypos + LineHeight * 2 / 3, xpos + y_r(125), ypos + LineHeight * 4 / 3,
																			LineHeight * 2 / 3, FontHandle::FontXCenter::MIDDLE, White, Black, "推奨:DirectInput");
													 break;
												 case DX_PADTYPE_XBOX_360:
												 case DX_PADTYPE_XBOX_ONE:
													 WindowSystem::SetMsgWW(xpos + y_r(125), ypos + LineHeight * 2 / 3, xpos + y_r(125), ypos + LineHeight * 4 / 3,
																			LineHeight * 2 / 3, FontHandle::FontXCenter::MIDDLE, White, Black, "推奨:XInput");
													 break;
												 default:
													 break;
											 }
										 }
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("EX UI", LocalizePool::Instance()->Get(1146),
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::EX_UI);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::EX_UI);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamBoolean(EnumSaveParam::EX_UI, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::EX_UI)));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("EX UI2", LocalizePool::Instance()->Get(1147),
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::EX_UI2);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 OPTION::Instance()->ChangeParamBoolean(EnumSaveParam::EX_UI2);
										 SoundPool::Instance()->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->SetParamBoolean(EnumSaveParam::EX_UI2, WindowSystem::CheckBox(xpos, ypos, OptionParts->GetParamBoolean(EnumSaveParam::EX_UI2)));
									 }
									 );
	}
	void OptionWindowClass::ControlTabsInfo::KeyDraw(int xpos, int ypos, bool isMine, int Sel) noexcept {
		auto* Pad = PadControl::Instance();
		WindowSystem::SetMsgWW(xpos, ypos, xpos + y_r(200), ypos + LineHeight,
							   LineHeight, FontHandle::FontXCenter::MIDDLE, (Pad->GetKeyReserve((PADS)Sel) < 0) ? Red : (isMine ? White : Gray25), Black,
							   "[%s]->[%s]", Pad->GetIDtoStr(Pad->GetKeyassign((PADS)Sel)).c_str(), Pad->GetIDtoStr(Pad->GetKeyReserve((PADS)Sel)).c_str());

	}


	void OptionWindowClass::ControlTabsInfo::Init_Sub() noexcept {
		const char* KeyInfo = LocalizePool::Instance()->Get(1151);

		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Reset", LocalizePool::Instance()->Get(1150),
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&](int xpos, int ypos, bool isMine) {
										 if (isMine && PadControl::Instance()->GetMouseClick().trigger()) {
											 PadControl::Instance()->ResetAssign();
										 }
										 WindowSystem::SetMsgWW(xpos, ypos, xpos + y_r(200), ypos + LineHeight, LineHeight, FontHandle::FontXCenter::MIDDLE, isMine ? White : Gray25, Black, "LMB Click");
									 }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizePool::Instance()->Get(1153), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)0); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 0); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizePool::Instance()->Get(1154), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)2); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 2); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizePool::Instance()->Get(1155), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)1); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 1); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizePool::Instance()->Get(1156), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)3); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 3); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizePool::Instance()->Get(1157), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)10); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 10); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizePool::Instance()->Get(1158), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)11); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 11); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizePool::Instance()->Get(1159), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)12); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 12); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizePool::Instance()->Get(1160), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)13); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 13); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizePool::Instance()->Get(1161), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)14); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 14); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizePool::Instance()->Get(1162), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)15); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 15); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizePool::Instance()->Get(1163), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)16); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 16); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizePool::Instance()->Get(1164), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)18); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 18); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizePool::Instance()->Get(1165), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)19); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 19); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizePool::Instance()->Get(1166), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)20); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 20); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizePool::Instance()->Get(1167), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)21); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 21); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizePool::Instance()->Get(1168), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)22); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 22); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizePool::Instance()->Get(1169), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)23); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 23); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizePool::Instance()->Get(1170), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)24); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 24); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init(LocalizePool::Instance()->Get(1171), KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)25); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 25); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Save", LocalizePool::Instance()->Get(1152),
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&](int xpos, int ypos, bool isMine) {
										 auto* Pad = PadControl::Instance();
										 if (isMine && Pad->GetMouseClick().trigger()) {
											 Pad->FlipAssign();
											 Pad->Save();
										 }
										 WindowSystem::SetMsgWW(xpos, ypos, xpos + y_r(200), ypos + LineHeight, LineHeight, FontHandle::FontXCenter::MIDDLE, isMine ? White : Gray25, Black, "LMB Click");
									 }
		);
	}
	//
	void OptionWindowClass::Init() noexcept {
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
		m_PopUpDrawClass.Set("Option", y_r(720), y_r(720), [&](int WinSizeX, int WinSizeY, bool EndSwitch) {
			auto* Pad = PadControl::Instance();
			auto* SE = SoundPool::Instance();
			int xp1, yp1;


			xp1 = y_r(960) - WinSizeX / 2 + y_r(48);
			yp1 = y_r(540) - WinSizeY / 2 + LineHeight * 3;
			for (auto& t : m_Tabs) {
				t->Draw(xp1, yp1, m_tabsel == t->GetID(), &m_tabsel, &m_select);
			}
			//ガイド
			xp1 = y_r(960) - WinSizeX / 2 + y_r(48);
			yp1 = y_r(540) + WinSizeY / 2 - LineHeight * 5 / 2;
			m_Tabs.at(m_tabsel)->DrawInfo(xp1, yp1, m_select);

			//
			if (Pad->GetKey(PADS::LEAN_L).trigger() && (m_tabsel != 3)) {
				--m_tabsel;
				if (m_tabsel < 0) { m_tabsel = (int)m_Tabs.size() - 1; }
				m_select = 0;
				SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}
			if (Pad->GetKey(PADS::LEAN_R).trigger() && (m_tabsel != 3)) {
				++m_tabsel;
				if (m_tabsel > (int)m_Tabs.size() - 1) { m_tabsel = 0; }
				m_select = 0;
				SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}

			m_Tabs.at(m_tabsel)->Execute(&m_select, (m_tabsel != 3));
			//
			if (EndSwitch) {
				Pad->SetGuideUpdate();
				OPTION::Instance()->Save();
				Pad->Save();
				m_tabsel = 0;
				m_select = 0;
			}
							 });
		//
	}
	void OptionWindowClass::Execute(void) noexcept {
		m_PopUpDrawClass.Update(m_ActiveSwitch);
		if (m_PopUpDrawClass.IsActive()) {
			auto* Pad = PadControl::Instance();
			Pad->ChangeGuide(
				[&]() {
					auto* KeyGuide = PadControl::Instance();

					KeyGuide->AddGuide(PADS::INTERACT, LocalizePool::Instance()->Get(9992));
					KeyGuide->AddGuide(PADS::RELOAD, LocalizePool::Instance()->Get(9991));

					KeyGuide->AddGuide(PADS::LEAN_L, "");
					KeyGuide->AddGuide(PADS::LEAN_R, LocalizePool::Instance()->Get(9994));
					KeyGuide->AddGuide(PADS::MOVE_W, "");
					KeyGuide->AddGuide(PADS::MOVE_A, "");
					KeyGuide->AddGuide(PADS::MOVE_S, "");
					KeyGuide->AddGuide(PADS::MOVE_D, "");
					KeyGuide->AddGuide(PADS::MOVE_STICK, LocalizePool::Instance()->Get(9993));
				}
			);
		}
		m_ActiveSwitch = false;
	}
	void OptionWindowClass::Draw() noexcept {
		m_PopUpDrawClass.Draw();
	}
};
