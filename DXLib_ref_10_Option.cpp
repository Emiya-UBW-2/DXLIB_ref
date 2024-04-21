#include "DXLib_ref_09_KeyControl.hpp"
#include "DXLib_ref_10_Option.hpp"
#include "DXLib_ref_08_Etc.hpp"

namespace DXLib_ref {
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	const OPTION* SingletonBase<OPTION>::m_Singleton = nullptr;

	void			OPTION::Load(void) noexcept {
		SetOutApplicationLogValidFlag(FALSE);
		int mdata = FileRead_open("data/Setting.txt", FALSE);
		while (true) {
			if (FileRead_eof(mdata) != 0) { break; }
			auto ALL = getparams::Getstr(mdata);
			if (ALL == "") { continue; }
			auto LEFT = getparams::getleft(ALL);
			auto RIGHT = getparams::getright(ALL);
			if (LEFT == OptionStr[0]) {
				grass_level = std::clamp<int>(std::stoi(RIGHT), 0, 4);
			}
			else if (LEFT == OptionStr[1]) {
				DoF = (RIGHT.find("true") != std::string::npos);
			}
			else if (LEFT == OptionStr[2]) {
				Bloom = (RIGHT.find("true") != std::string::npos);
			}
			else if (LEFT == OptionStr[3]) {
				Shadow = (RIGHT.find("true") != std::string::npos);
			}
			else if (LEFT == OptionStr[4]) {
				useVR = (RIGHT.find("true") != std::string::npos);
			}
			else if (LEFT == OptionStr[5]) {
				SSAO = (RIGHT.find("true") != std::string::npos);
			}
			else if (LEFT == OptionStr[6]) {
				Fov = std::stof(RIGHT);
			}
			else if (LEFT == OptionStr[7]) {
				Vsync = (RIGHT.find("true") != std::string::npos);
			}
			else if (LEFT == OptionStr[8]) {
				FrameLimit = std::stoi(RIGHT);
			}
			else if (LEFT == OptionStr[9]) {
				SE = std::stof(RIGHT);
			}
			else if (LEFT == OptionStr[10]) {
				VOICE = std::stof(RIGHT);
			}
			else if (LEFT == OptionStr[11]) {
				BGM = std::stof(RIGHT);
			}
			else if (LEFT == OptionStr[12]) {
				AllWaysFront = (RIGHT.find("true") != std::string::npos);
			}
			else if (LEFT == OptionStr[13]) {
				aberration = (RIGHT.find("true") != std::string::npos);
			}
			else if (LEFT == OptionStr[14]) {
				std::string DctXVer = RIGHT;
				for (int i = 1;i < 2;i++) {
					if (DctXVer == DirectXVerStr[i]) {
						DirectXVer = i;
					}
				}
			}
			else if (LEFT == OptionStr[15]) {
				SSR = (RIGHT.find("true") != std::string::npos);
			}
			else if (LEFT == OptionStr[16]) {
				MotionBlur = (RIGHT.find("true") != std::string::npos);
			}
			else if (LEFT == OptionStr[17]) {
				Xsensing = std::stof(RIGHT);
			}
			else if (LEFT == OptionStr[18]) {
				Ysensing = std::stof(RIGHT);
			}
			else if (LEFT == OptionStr[19]) {
				HeadBobbing = (RIGHT.find("true") != std::string::npos);
			}
			else if (LEFT == OptionStr[20]) {
				EnableCheck = (RIGHT.find("true") != std::string::npos);
			}
			else if (LEFT == OptionStr[21]) {
				LightMode = (RIGHT.find("true") != std::string::npos);
			}
			else if (LEFT == OptionStr[22]) {
				std::string DctXVer = RIGHT;
				for (int i = 1;i < 3;i++) {
					if (DctXVer == ControlTypeStr[i]) {
						PadType = i;
					}
				}
			}
		}
		FileRead_close(mdata);
		SetOutApplicationLogValidFlag(TRUE);
	}
	template<class T>
	std::string GetString(int id, T Param) {
		switch (ParamTypes[id]) {
			case EnumParamType::Boolean:
				return std::string((Param) ? "true" : "false");
			case EnumParamType::Int:
				return std::to_string(Param);
			case EnumParamType::Float:
				return std::to_string(Param);
			case EnumParamType::Else:
				break;
			default:
				break;
		}
		return "";
	}

	void			OPTION::Save(void) noexcept {
		std::ofstream outputfile("data/Setting.txt");
		outputfile << std::string(OptionStr[0]) + "=" + GetString(0, Get_grass_level()) + "\n";
		outputfile << std::string(OptionStr[1]) + "=" + GetString(1, Get_DoF()) + "\n";
		outputfile << std::string(OptionStr[2]) + "=" + GetString(2, Get_Bloom()) + "\n";
		outputfile << std::string(OptionStr[3]) + "=" + GetString(3, Get_Shadow()) + "\n";
		outputfile << std::string(OptionStr[4]) + "=" + GetString(4, Get_useVR()) + "\n";
		outputfile << std::string(OptionStr[5]) + "=" + GetString(5, Get_SSAO()) + "\n";
		outputfile << std::string(OptionStr[6]) + "=" + GetString(6, Get_Fov()) + "\n";
		outputfile << std::string(OptionStr[7]) + "=" + GetString(7, Get_Vsync()) + "\n";
		outputfile << std::string(OptionStr[8]) + "=" + GetString(8, Get_FrameLimit()) + "\n";
		outputfile << std::string(OptionStr[9]) + "=" + GetString(9, Get_SE()) + "\n";
		outputfile << std::string(OptionStr[10]) + "=" + GetString(10, Get_VOICE()) + "\n";
		outputfile << std::string(OptionStr[11]) + "=" + GetString(11, Get_BGM()) + "\n";
		outputfile << std::string(OptionStr[12]) + "=" + GetString(12, Get_AllWaysFront()) + "\n";
		outputfile << std::string(OptionStr[13]) + "=" + GetString(13, Get_aberration()) + "\n";
		outputfile << std::string(OptionStr[14]) + "=" + std::string(DirectXVerStr[DirectXVer]) + "\n";
		outputfile << std::string(OptionStr[15]) + "=" + GetString(15, Get_SSR()) + "\n";
		outputfile << std::string(OptionStr[16]) + "=" + GetString(16, Get_MotionBlur()) + "\n";
		outputfile << std::string(OptionStr[17]) + "=" + GetString(17, Get_Xsensing()) + "\n";
		outputfile << std::string(OptionStr[18]) + "=" + GetString(18, Get_Ysensing()) + "\n";
		outputfile << std::string(OptionStr[19]) + "=" + GetString(19, Get_HeadBobbing()) + "\n";
		outputfile << std::string(OptionStr[20]) + "=" + GetString(20, Get_EnableCheck()) + "\n";
		outputfile << std::string(OptionStr[21]) + "=" + GetString(21, Get_LightMode()) + "\n";
		outputfile << std::string(OptionStr[22]) + "=" + std::string(ControlTypeStr[Get_PadType()]) + "\n";
		outputfile.close();
	}

	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	const OptionWindowClass* SingletonBase<OptionWindowClass>::m_Singleton = nullptr;
	//
	void OptionWindowClass::OptionElementsInfo::Draw(int xpos, int ypos, bool isMine) const noexcept {
		ypos += y_r((int)selanim);
		WindowSystem::SetMsg(xpos, ypos, xpos, ypos + LineHeight,
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
		WindowSystem::SetMsg(xpos, ypos, xpos, ypos + LineHeight, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, m_Elements.at(select).GetInfo());
	}
	//
	void OptionWindowClass::SoundTabsInfo::Init_Sub() noexcept {
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("BGM", "BGMボリュームを変更します",
									 [&]() {
										 auto* BGM = BGMPool::Instance();
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_BGM(std::clamp(OptionParts->Get_BGM() - 0.1f, 0.f, 1.f));
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);

										 BGM->SetVol(OptionParts->Get_BGM());
									 },
									 [&]() {
										 auto* BGM = BGMPool::Instance();
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_BGM(std::clamp(OptionParts->Get_BGM() + 0.1f, 0.f, 1.f));
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);

										 BGM->SetVol(OptionParts->Get_BGM());
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* BGM = BGMPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 int value = WindowSystem::UpDownBar(xpos, xpos + y_r(200), ypos, (int)(OptionParts->Get_BGM()*100.f + 0.5f), 0, 100);
										 OptionParts->Set_BGM((float)value / 100.f);

										 BGM->SetVol(OptionParts->Get_BGM());
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("SE", "SEボリュームを変更します",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_SE(std::clamp(OptionParts->Get_SE() - 0.1f, 0.f, 1.f));
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 SE->SetVol(OptionParts->Get_SE());
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_SE(std::clamp(OptionParts->Get_SE() + 0.1f, 0.f, 1.f));
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
										 SE->SetVol(OptionParts->Get_SE());
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 int value = WindowSystem::UpDownBar(xpos, xpos + y_r(200), ypos, (int)(OptionParts->Get_SE()*100.f + 0.5f), 0, 100);
										 OptionParts->Set_SE((float)value / 100.f);
										 SE->SetVol(OptionParts->Get_SE());
									 }
									 );
	}
	void OptionWindowClass::GraphicTabsInfo::Init_Sub() noexcept {
		HDC hdc;
		hdc = GetDC(GetMainWindowHandle());	// デバイスコンテキストの取得
		RefreshRate = GetDeviceCaps(hdc, VREFRESH);	// リフレッシュレートの取得
		ReleaseDC(GetMainWindowHandle(), hdc);	// デバイスコンテキストの解放

		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Light Mode", "軽量モードを指定します(反映は再起動後にされます)",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_LightMode(OptionParts->Get_LightMode() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_LightMode(OptionParts->Get_LightMode() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_LightMode(WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_LightMode()));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Window Mode", "ウィンドウ、ボーダーレスモードを選択します(反映は再起動後にされます)",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_AllWaysFront(OptionParts->Get_AllWaysFront() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_AllWaysFront(OptionParts->Get_AllWaysFront() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_AllWaysFront(WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_AllWaysFront()));

										 WindowSystem::SetMsg(xpos + y_r(100), ypos, xpos + y_r(100), ypos + LineHeight,
															  LineHeight, FontHandle::FontXCenter::MIDDLE, White, Black, OptionParts->Get_AllWaysFront() ? "ウィンドウ" : "ボーダーレス");
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Fov", "垂直視野角を変更します",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_Fov(std::clamp(OptionParts->Get_Fov() - 5.f, 45.f, 110.f));
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_Fov(std::clamp(OptionParts->Get_Fov() + 5.f, 45.f, 110.f));
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 int value = WindowSystem::UpDownBar(xpos, xpos + y_r(200), ypos, (int)(OptionParts->Get_Fov() + 0.5f), 45, 110);
										 OptionParts->Set_Fov((float)value);
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("V Sync", "垂直同期の有効無効を指定します(反映は再起動後にされます)",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_Vsync(OptionParts->Get_Vsync() ^ 1);
										 if (OptionParts->Get_Vsync()) {
											 OptionParts->Set_FrameLimit(RefreshRate);
										 }
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_Vsync(OptionParts->Get_Vsync() ^ 1);
										 if (OptionParts->Get_Vsync()) {
											 OptionParts->Set_FrameLimit(RefreshRate);
										 }
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_Vsync(WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_Vsync()));
										 if (OptionParts->Get_Vsync()) {
											 OptionParts->Set_FrameLimit(RefreshRate);
										 }
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("FPS Limit", "垂直同期を切った際のFPS制限を指定します",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();

										 int value = OptionParts->Get_FrameLimit();
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

										 OptionParts->Set_FrameLimit(value);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 int value = OptionParts->Get_FrameLimit();
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
										 OptionParts->Set_FrameLimit(value);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 int value = WindowSystem::UpDownBar(xpos, xpos + y_r(200), ypos, OptionParts->Get_FrameLimit(), FrameLimits[0], FrameLimits[FrameLimitsNum - 1]);
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

										 OptionParts->Set_FrameLimit(value);
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("DirectX Version", "DirectXのバージョンを変更します(反映は再起動後にされます)",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 for (int i = 1;i < 2;i++) {
											 if (OptionParts->Get_DirectXVer() == i) {
												 OptionParts->Set_DirectXVer((i + 1) % 2);
												 break;
											 }
										 }
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 for (int i = 1;i < 2;i++) {
											 if (OptionParts->Get_DirectXVer() == i) {
												 OptionParts->Set_DirectXVer((i + 1) % 2);
												 break;
											 }
										 }
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 if (WindowSystem::CheckBox(xpos, ypos, (OptionParts->Get_DirectXVer() == 1))) {
											 OptionParts->Set_DirectXVer(1);
										 }
										 else {
											 OptionParts->Set_DirectXVer(0);
										 }

										 WindowSystem::SetMsg(xpos + y_r(100), ypos, xpos + y_r(100), ypos + LineHeight,
															  LineHeight, FontHandle::FontXCenter::MIDDLE, (OptionParts->Get_DirectXVer() == 0) ? White : Gray50, Black, "9.0C");
										 WindowSystem::SetMsg(xpos + y_r(200), ypos, xpos + y_r(200), ypos + LineHeight,
															  LineHeight, FontHandle::FontXCenter::RIGHT, (OptionParts->Get_DirectXVer() == 1) ? White : Gray50, Black, "11.0");
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("AntiAlias", "FXAAの有効無効を指定します",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_FXAA(OptionParts->Get_FXAA() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_FXAA(OptionParts->Get_FXAA() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_FXAA(WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_FXAA()));
									 }
									 );

		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("SSAO", "画面ベースの環境遮蔽の有効無効を指定します",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_SSAO(OptionParts->Get_SSAO() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_SSAO(OptionParts->Get_SSAO() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_SSAO(WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_SSAO()));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("SSR", "画面ベースの鏡面反射の有効無効を指定します DirectX11のみ有効",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_SSR(OptionParts->Get_SSR() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_SSR(OptionParts->Get_SSR() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_SSR(WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_SSR()));
									 }
									 );

		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Shadow", "影の有効無効を指定します",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_Shadow(OptionParts->Get_Shadow() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_Shadow(OptionParts->Get_Shadow() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_Shadow(WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_Shadow()));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Object", "オブジェクトの表示をするかを指定します",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_grass_level(1 - OptionParts->Get_grass_level());
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_grass_level(1 - OptionParts->Get_grass_level());
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_grass_level(WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_grass_level() > 0) ? 1 : 0);
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Bloom Effect", "ブルームエフェクトの有効無効を指定します",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_Bloom(OptionParts->Get_Bloom() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_Bloom(OptionParts->Get_Bloom() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_Bloom(WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_Bloom()));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Color Aberration", "画面エフェクトの有効無効を指定します",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_aberration(OptionParts->Get_aberration() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_aberration(OptionParts->Get_aberration() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_aberration(WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_aberration()));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("DoF", "遠近のピンボケの有効無効を指定します",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_DoF(OptionParts->Get_DoF() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_DoF(OptionParts->Get_DoF() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_DoF(WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_DoF()));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("MotionBlur", "モーションブラーの有効無効を指定します",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_MotionBlur(OptionParts->Get_MotionBlur() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_MotionBlur(OptionParts->Get_MotionBlur() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_MotionBlur(WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_MotionBlur()));
									 }
									 );
	}
	void OptionWindowClass::ElseTabsInfo::Init_Sub() noexcept {
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("X sensing", "横方向の感度を変更します",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_Xsensing(std::clamp(OptionParts->Get_Xsensing() - 0.01f, 0.01f, 1.f));
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_Xsensing(std::clamp(OptionParts->Get_Xsensing() + 0.01f, 0.01f, 1.f));
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 int value = WindowSystem::UpDownBar(xpos, xpos + y_r(200), ypos, (int)(OptionParts->Get_Xsensing()*100.f + 0.5f), 10, 100);
										 OptionParts->Set_Xsensing((float)value / 100.f);
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("Y sensing", "縦方向の感度を変更します",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_Ysensing(std::clamp(OptionParts->Get_Ysensing() - 0.01f, 0.f, 1.f));
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_Ysensing(std::clamp(OptionParts->Get_Ysensing() + 0.01f, 0.f, 1.f));
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 int value = WindowSystem::UpDownBar(xpos, xpos + y_r(200), ypos, (int)(OptionParts->Get_Ysensing()*100.f + 0.5f), 10, 100);
										 OptionParts->Set_Ysensing((float)value / 100.f);
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("HeadBobbing", "視点の揺れの有効無効を指定します",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_HeadBobbing(OptionParts->Get_HeadBobbing() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_HeadBobbing(OptionParts->Get_HeadBobbing() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_HeadBobbing(WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_HeadBobbing()));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("CheckMagazine", "追加で眺め/マガジン確認をするかを指定します",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_EnableCheck(OptionParts->Get_EnableCheck() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_EnableCheck(OptionParts->Get_EnableCheck() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_EnableCheck(WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_EnableCheck()));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("PadType", "パッドの入力方式を変更します",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 switch ((ControlType)OptionParts->Get_PadType()) {
											 case  ControlType::PS4:
												 OptionParts->Set_PadType((int)ControlType::XBox);
												 break;
											 case  ControlType::XBox:
												 OptionParts->Set_PadType((int)ControlType::PS4);
												 break;
											 default:
												 break;
										 }
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 switch ((ControlType)OptionParts->Get_PadType()) {
											 case  ControlType::PS4:
												 OptionParts->Set_PadType((int)ControlType::XBox);
												 break;
											 case  ControlType::XBox:
												 OptionParts->Set_PadType((int)ControlType::PS4);
												 break;
											 default:
												 break;
										 }
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 if (WindowSystem::CheckBox(xpos, ypos, (OptionParts->Get_PadType() == (int)ControlType::PS4))) {
											 OptionParts->Set_PadType((int)ControlType::PS4);
										 }
										 else {
											 OptionParts->Set_PadType((int)ControlType::XBox);
										 }
										 ypos -= LineHeight * 1 / 6;
										 if (OptionParts->Get_PadType() == (int)ControlType::XBox) {
											 WindowSystem::SetMsg(xpos + y_r(125), ypos, xpos + y_r(125), ypos + LineHeight * 2 / 3,
																  LineHeight * 2 / 3, FontHandle::FontXCenter::MIDDLE, White, Black, "XInput");
										 }
										 else {
											 WindowSystem::SetMsg(xpos + y_r(125), ypos, xpos + y_r(125), ypos + LineHeight * 2 / 3,
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
													 WindowSystem::SetMsg(xpos + y_r(125), ypos + LineHeight * 2 / 3, xpos + y_r(125), ypos + LineHeight * 4 / 3,
																		  LineHeight * 2 / 3, FontHandle::FontXCenter::MIDDLE, White, Black, "推奨:DirectInput");
													 break;
												 case DX_PADTYPE_XBOX_360:
												 case DX_PADTYPE_XBOX_ONE:
													 WindowSystem::SetMsg(xpos + y_r(125), ypos + LineHeight * 2 / 3, xpos + y_r(125), ypos + LineHeight * 4 / 3,
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
		WindowSystem::SetMsg(xpos, ypos, xpos + y_r(200), ypos + LineHeight,
							 LineHeight, FontHandle::FontXCenter::MIDDLE, (Pad->GetKeyReserve((PADS)Sel) < 0) ? Red : (isMine ? White : Gray25), Black,
							 "[%s]->[%s]", Pad->GetIDtoStr(Pad->GetKeyassign((PADS)Sel)).c_str(), Pad->GetIDtoStr(Pad->GetKeyReserve((PADS)Sel)).c_str());

	}


	void OptionWindowClass::ControlTabsInfo::Init_Sub() noexcept {
		const char* KeyInfo = "[マウス必須]カーソルをかざし設定するボタンを押してください";

		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("変更を破棄", "[マウス必須]リセットします",
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&](int xpos, int ypos, bool isMine) {
										 if (isMine && PadControl::Instance()->GetMouseClick().trigger()) {
											 PadControl::Instance()->ResetAssign();
										 }
										 WindowSystem::SetMsg(xpos, ypos, xpos + y_r(200), ypos + LineHeight, LineHeight, FontHandle::FontXCenter::MIDDLE, isMine ? White : Gray25, Black, "LMB Click");
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("前進/上を選択", KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)0); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 0); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("後退/下を選択", KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)2); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 2); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("左移動/左を選択", KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)1); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 1); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("右移動/右を選択", KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)3); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 3); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("左リーン/左タブを選択", KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)10); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 10); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("右リーン/右タブを選択", KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)11); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 11); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("リロード/戻る", KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)12); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 12); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("インタラクト/選択", KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)13); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 13); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("投げる", KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)14); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 14); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("近接攻撃", KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)15); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 15); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("スタンス切替", KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)16); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 16); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("走る", KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)18); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 18); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("歩く", KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)19); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 19); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("射撃", KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)20); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 20); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("エイム", KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)21); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 21); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("銃器切替", KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)22); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 22); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("しゃがみ", KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)23); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 23); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("伏せ", KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)24); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 24); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("装備確認", KeyInfo,
									 [&]() {},
									 [&]() {},
									 [&]() {},
									 [&]() { PadControl::Instance()->GetPushAnySwitch((PADS)25); },
									 [&](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, 25); }
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("セーブ", "[マウス必須]キーコンフィグをセーブします",
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
										 WindowSystem::SetMsg(xpos, ypos, xpos + y_r(200), ypos + LineHeight, LineHeight, FontHandle::FontXCenter::MIDDLE, isMine ? White : Gray25, Black, "LMB Click");
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

					KeyGuide->AddGuide(PADS::INTERACT, "決定");
					KeyGuide->AddGuide(PADS::RELOAD, "戻る");

					KeyGuide->AddGuide(PADS::LEAN_L, "");
					KeyGuide->AddGuide(PADS::LEAN_R, "タブ切替");
					KeyGuide->AddGuide(PADS::MOVE_W, "");
					KeyGuide->AddGuide(PADS::MOVE_A, "");
					KeyGuide->AddGuide(PADS::MOVE_STICK, "上選択");
					KeyGuide->AddGuide(PADS::MOVE_S, "");
					KeyGuide->AddGuide(PADS::MOVE_D, "");
					KeyGuide->AddGuide(PADS::MOVE_STICK, "下選択");
				}
			);
		}
		m_ActiveSwitch = false;
	}
	void OptionWindowClass::Draw() noexcept {
		m_PopUpDrawClass.Draw();
	}
};
