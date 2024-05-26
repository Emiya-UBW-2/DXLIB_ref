#include "DXLib_ref_09_KeyControl.hpp"

namespace DXLib_ref {
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	const PadControl* SingletonBase<PadControl>::m_Singleton = nullptr;

	void PadControl::KeyGuideGraphs::AddGuideXBox(int ID, std::string_view GuideStr) noexcept {
		if (0 <= ID && ID < XBoxNum) {
			std::string Path = "data/key/key_glay/";
			Path += XBoxGuidePath[ID];
			Path += ".png";
			GuideImg = GraphHandle::Load(Path);
			GuideImg.GetSize(&xsize, &ysize);
			xsize = xsize * y_r(24) / ysize;
			ysize = ysize * y_r(24) / ysize;
		}
		else {
			xsize = 0;
			ysize = 0;
		}
		GuideString = GuideStr;
	}
	void PadControl::KeyGuideGraphs::AddGuideDS4(int ID, std::string_view GuideStr) noexcept {
		if (0 <= ID && ID < DS4Num) {
			std::string Path = "data/key/key_glay/";
			Path += DS4GuidePath[ID];
			Path += ".png";
			GuideImg = GraphHandle::Load(Path);
			GuideImg.GetSize(&xsize, &ysize);
			xsize = xsize * y_r(24) / ysize;
			ysize = ysize * y_r(24) / ysize;
		}
		else {
			xsize = 0;
			ysize = 0;
		}
		GuideString = GuideStr;
	}
	void PadControl::KeyGuideGraphs::AddGuidePC(int ID, std::string_view GuideStr) noexcept {
		if (0 <= ID && ID < KeyNum) {
			std::string Path = "data/key/key_glay/";
			Path += KeyGuidePath[ID];
			if ((KeyID[ID] & 0xF00) != 0) {
				Path += ".png";
			}
			else {
				Path += ".jpg";
			}
			GuideImg = GraphHandle::Load(Path);
			GuideImg.GetSize(&xsize, &ysize);
			xsize = xsize * y_r(21) / ysize;
			ysize = ysize * y_r(21) / ysize;
		}
		else {
			xsize = 0;
			ysize = 0;
		}
		GuideString = GuideStr;
	}
	int PadControl::KeyGuideGraphs::GetDrawSize(float scale) const noexcept {
		auto* Fonts = FontPool::Instance();

		int ofs = 0;
		if (xsize > 0) {
			ofs += (int)(xsize * scale) + y_r(3.f* scale);
		}
		if (GuideString != "") {
			ofs += Fonts->Get(FontPool::FontType::Nomal_Edge).GetStringWidth(y_r(18.f*scale), GuideString) + y_r(12.f*scale);
		}
		return ofs;
	}

	int PadControl::KeyGuideGraphs::Draw(int x, int y, float scale) const noexcept {
		auto* Fonts = FontPool::Instance();

		int ofs = 0;
		if (xsize > 0) {
			GuideImg.DrawExtendGraph(x + ofs, y, x + ofs + (int)(xsize * scale), y + (int)(ysize * scale), true);
			ofs += (int)(xsize * scale) + y_r(3.f* scale);
		}
		Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18 * scale), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE, x + ofs, y + y_r(24 * scale) / 2, White, Black, GuideString);
		return GetDrawSize(scale);
	}

	const bool PadControl::GetButtonPress(int ID) {
		switch (m_ControlType) {
			case DXLib_ref::ControlType::XBox:
				{
					XINPUT_STATE input;
					GetJoypadXInputState(DX_INPUT_PAD1, &input);
					//ボタン
					if (ID >= 0xF100) {
						switch (ID) {
							case 0xF100:
								return input.LeftTrigger > 0;
							case 0xF200:
								return input.RightTrigger > 0;
							default:
								break;
						}
					}
					else if (0 <= ID && ID < 16) {
						return (input.Buttons[ID] != 0);
					}
				}
				break;
			case DXLib_ref::ControlType::PS4:
				{
					DINPUT_JOYSTATE input;
					GetJoypadDirectInputState(DX_INPUT_PAD1, &input);
					//ボタン
					if (ID >= 0xF010) {
						//十字キー
						float deg = (float)(input.POV[0]) / 100.f;
						bool w_key = false;
						bool s_key = false;
						bool a_key = false;
						bool d_key = false;
						if (input.POV[0] != 0xffffffff) {
							w_key = (310.f <= deg || deg <= 50.f);
							s_key = (130.f <= deg && deg <= 230.f);
							a_key = (220.f <= deg && deg <= 320.f);
							d_key = (40.f <= deg && deg <= 140.f);
						}
						switch (ID) {
							case 0xF010:
								return a_key;
							case 0xF020:
								return w_key;
							case 0xF040:
								return d_key;
							case 0xF080:
								return s_key;
							default:
								break;
						}
					}
					else if (0 <= ID && ID < 32) {
						return (input.Buttons[ID] != 0);
					}
				}
				break;
			case DXLib_ref::ControlType::PC:
				//ボタン
				if (0 <= ID) {
					if ((ID & 0xF00) != 0) {
						return ((GetMouseInputWithCheck() & (ID & 0xFF)) != 0);
					}
					else {
						return (CheckHitKeyWithCheck(ID) != 0);
					}
				}
				break;
			default:
				break;
		}
		return false;
	}

	const bool PadControl::GetPushAnySwitchLocal(PADS select, int ID) {
		auto* SE = SoundPool::Instance();
		if (GetButtonPress(ID)) {
			auto& P = m_PadsInfo[(int)select];
			if (P.m_assign != ID) {
			}
			else if (P.m_Key.trigger()) {
				ID = (P.m_reserve == -1) ? P.m_assign : -1;
			}
			else {
				return true;
			}

			//既にアサイン済のものがあった場合そいつを無効化
			for (int p = 0; p < (int)PADS::MAX; p++) {
				auto& P2 = m_PadsInfo[p];
				if ((select != (PADS)p) && (P2.m_reserve == ID || P2.m_assign == ID)) {
					P2.m_reserve = -1;
					break;
				}
			}
			if (P.m_reserve != ID) {
				P.m_reserve = ID;
				SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}
			return true;
		}
		return false;
	}

	void PadControl::GetPushAnySwitch(PADS select) {
		for (int p = 0; p < (int)PADS::MAX; p++) {
			if (p == (int)select) { continue; }
			auto& P = m_PadsInfo[p];
			if (GetPushAnySwitchLocal(select, P.m_reserve)) {
				break;
			}
		}
		for (int p = 0; p < (int)PADS::MAX; p++) {
			if (p == (int)select) { continue; }
			auto& P = m_PadsInfo[p];
			if (GetPushAnySwitchLocal(select, P.m_assign)) {
				break;
			}
		}
		switch (m_ControlType) {
			case DXLib_ref::ControlType::XBox:
				for (int i = 0; i < XBoxNum; i++) {
					if (GetPushAnySwitchLocal(select, XBoxID[i])) {
						break;
					}
				}
				break;
			case DXLib_ref::ControlType::PS4:
				for (int i = 0; i < DS4Num; i++) {
					if (GetPushAnySwitchLocal(select, DS4ID[i])) {
						break;
					}
				}
				break;
			case DXLib_ref::ControlType::PC:
				for (int i = 0; i < KeyNum; i++) {
					if (GetPushAnySwitchLocal(select, KeyID[i])) {
						break;
					}
				}
				break;
			default:
				break;
		}
	}


	void PadControl::Load(void) noexcept {
		int mdata = -1;
		if (std::filesystem::is_regular_file(GetSavePath())) {
			mdata = FileRead_open(GetSavePath(), FALSE);
		}
		else {
			mdata = FileRead_open(GetBasePath(), FALSE);
		}
		while (true) {
			if (FileRead_eof(mdata) != 0) { break; }
			auto ALL = getparams::Getstr(mdata);
			if (ALL == "") { continue; }
			auto LEFT = getparams::getleft(ALL);
			auto RIGHT = getparams::getright(ALL);
			for (int i = 0; i < (int)PADS::MAX; i++) {
				if (LEFT == PADSStr[i]) {
					this->m_PadsInfo[i].m_assign = GetStrtoID(RIGHT.c_str());
					this->m_PadsInfo[i].m_reserve = this->m_PadsInfo[i].m_assign;
					break;
				}
			}
		}
		FileRead_close(mdata);
	}
	void PadControl::Save(void) noexcept {
		std::ofstream outputfile(GetSavePath());
		for (int i = 0; i < (int)PADS::MAX; i++) {
			outputfile << (std::string)PADSStr[i] + "=" + GetIDtoStr(this->m_PadsInfo[i].m_assign) + "\n";
		}
		outputfile.close();
	}

	void PadControl::ResetAssign(void) noexcept {
		for (int i = 0; i < (int)PADS::MAX; i++) {
			this->m_PadsInfo[i].m_reserve = this->m_PadsInfo[i].m_assign;
		}
	}
	void PadControl::FlipAssign(void) noexcept {
		for (int i = 0; i < (int)PADS::MAX; i++) {
			this->m_PadsInfo[i].m_assign = this->m_PadsInfo[i].m_reserve;
		}
	}


	void PadControl::ChangeGuide(std::function<void()>Guide_Pad) noexcept {
		auto* LocalizeParts = LocalizePool::Instance();
		if (m_IsUpdate) {
			m_IsUpdate = false;
			Reset();
			//
			Key.resize(Key.size() + 1);
			//
			Key.back() = std::make_unique<KeyGuideGraphs>();
			for (int i = 0; i < KeyNum; i++) {
				if (strcmpDx(KeyStr[i], "ESCAPE") == 0) {
					Key.back()->AddGuidePC(i, LocalizeParts->Get(9990));
					break;
				}
			}
			AddGuide(PADS::INVENTORY, LocalizeParts->Get(9913));
			//
			Guide_Pad();
		}
	}

	void PadControl::Execute(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		auto* LocalizeParts = LocalizePool::Instance();
		//コントロールタイプ決定
		{
			ControlType NextControlType = ControlType::PC;
			if (GetJoypadNum() > 0) {
				auto* OptionParts = OPTION::Instance();
				NextControlType = (ControlType)OptionParts->GetParamInt(EnumSaveParam::ControlType);
			}
			if (m_ControlType != NextControlType) {
				Save();
				m_ControlType = NextControlType;
				m_IsUpdate = true;
				Load();
				if (GetJoypadNum() > 0) {
					auto* ItemLogParts = SideLog::Instance();
					ItemLogParts->Add(10.f, GetColor(255, 255, 255), LocalizeParts->Get(298));
					ItemLogParts->Add(10.f, GetColor(255, 255, 255), LocalizeParts->Get(299));
				}
			}
		}
		//タイプに合わせた操作
		switch (m_ControlType) {
			case DXLib_ref::ControlType::XBox:
				{
					XINPUT_STATE input;
					GetJoypadXInputState(DX_INPUT_PAD1, &input);
					//左スティック
					{
						int LS_X = input.ThumbLX;
						int LS_Y = -input.ThumbLY;
						if (this->m_PadsInfo[(int)PADS::MOVE_STICK].m_assign == 0xF002) {
							LS_X = input.ThumbRX;
							LS_Y = -input.ThumbRY;
						}
						LS_X = 1000 * LS_X / 32768;
						LS_Y = 1000 * LS_Y / 32768;

						if (abs(LS_X) < 10) { LS_X = 0; }
						if (abs(LS_Y) < 10) { LS_Y = 0; }

						float deg = rad2deg(atan2f((float)LS_X, -(float)LS_Y));
						bool w_key = false;
						bool s_key = false;
						bool a_key = false;
						bool d_key = false;
						if (!(LS_X == 0 && LS_Y == 0)) {
							w_key = (-50.f <= deg && deg <= 50.f);
							a_key = (-140.f <= deg && deg <= -40.f);
							s_key = (130.f <= deg || deg <= -130.f);
							d_key = (40.f <= deg && deg <= 140.f);
						}
						this->m_PadsInfo[(int)PADS::MOVE_W].m_Key.Execute(w_key);
						this->m_PadsInfo[(int)PADS::MOVE_S].m_Key.Execute(s_key);
						this->m_PadsInfo[(int)PADS::MOVE_A].m_Key.Execute(a_key);
						this->m_PadsInfo[(int)PADS::MOVE_D].m_Key.Execute(d_key);
					}
					//右スティック
					{
						int mx = DrawParts->GetDispXSize() / 2, my = DrawParts->GetDispYSize() / 2;
						GetMousePoint(&mx, &my);
						MouseX = (mx * y_r(1920) / DXDraw::Instance()->GetDispXSize());
						MouseY = (my * y_r(1080) / DXDraw::Instance()->GetDispYSize());
						MouseClick.Execute((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0);
						SetMouseDispFlag(TRUE);
						int RS_X = input.ThumbRX;
						int RS_Y = -input.ThumbRY;
						if (this->m_PadsInfo[(int)PADS::DIR_STICK].m_assign == 0xF001) {
							RS_X = input.ThumbLX;
							RS_Y = -input.ThumbLY;
						}
						RS_X = 1000 * RS_X / 32768;
						RS_Y = 1000 * RS_Y / 32768;

						if (!DrawParts->IsPause()) {
							auto* OptionParts = OPTION::Instance();
							Look_XradAdd = std::clamp((float)RS_X / 100.f*4.f*OptionParts->GetParamFloat(EnumSaveParam::Xsensing), -180.f, 180.f);
							Look_YradAdd = std::clamp(-(float)RS_Y / 100.f*4.f*OptionParts->GetParamFloat(EnumSaveParam::Ysensing), -180.f, 180.f);
						}
						else {
							Look_XradAdd = 0.f;
							Look_YradAdd = 0.f;
						}
					}
				}
				break;
			case DXLib_ref::ControlType::PS4:
				{
					DINPUT_JOYSTATE input;
					GetJoypadDirectInputState(DX_INPUT_PAD1, &input);
					//左スティック
					{
						int LS_X = input.X;
						int LS_Y = input.Y;
						if (this->m_PadsInfo[(int)PADS::MOVE_STICK].m_assign == 0xF002) {
							LS_X = input.Z;
							LS_Y = input.Rz;
						}

						float deg = rad2deg(atan2f((float)LS_X, -(float)LS_Y));
						bool w_key = false;
						bool s_key = false;
						bool a_key = false;
						bool d_key = false;
						if (!(input.X == 0 && input.Y == 0)) {
							w_key = (-50.f <= deg && deg <= 50.f);
							a_key = (-140.f <= deg && deg <= -40.f);
							s_key = (130.f <= deg || deg <= -130.f);
							d_key = (40.f <= deg && deg <= 140.f);
						}
						this->m_PadsInfo[(int)PADS::MOVE_W].m_Key.Execute(w_key);
						this->m_PadsInfo[(int)PADS::MOVE_S].m_Key.Execute(s_key);
						this->m_PadsInfo[(int)PADS::MOVE_A].m_Key.Execute(a_key);
						this->m_PadsInfo[(int)PADS::MOVE_D].m_Key.Execute(d_key);
					}
					//右スティック
					{
						int mx = DrawParts->GetDispXSize() / 2, my = DrawParts->GetDispYSize() / 2;
						GetMousePoint(&mx, &my);
						MouseX = (mx * y_r(1920) / DXDraw::Instance()->GetDispXSize());
						MouseY = (my * y_r(1080) / DXDraw::Instance()->GetDispYSize());
						MouseClick.Execute((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0);
						SetMouseDispFlag(TRUE);
						int RS_X = input.Z;
						int RS_Y = input.Rz;
						if (this->m_PadsInfo[(int)PADS::DIR_STICK].m_assign == 0xF001) {
							RS_X = input.X;
							RS_Y = input.Y;
						}

						auto* OptionParts = OPTION::Instance();
						Look_XradAdd = (float)RS_X / 100.f*4.f*OptionParts->GetParamFloat(EnumSaveParam::Xsensing);
						Look_YradAdd = -(float)RS_Y / 100.f*4.f*OptionParts->GetParamFloat(EnumSaveParam::Ysensing);
					}
				}
				break;
			case DXLib_ref::ControlType::PC:
				//右スティック
				{
					int mx = DrawParts->GetDispXSize() / 2, my = DrawParts->GetDispYSize() / 2;
					GetMousePoint(&mx, &my);
					MouseX = (mx * y_r(1920) / DXDraw::Instance()->GetDispXSize());
					MouseY = (my * y_r(1080) / DXDraw::Instance()->GetDispYSize());
					MouseClick.Execute((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0);

					if (m_MouseMoveEnable) {
						if (GetMainWindowHandle() != GetForegroundWindow()) {//次画面が最前ではないよん
							SetMouseDispFlag(TRUE);
						}
						else {
							if (!DrawParts->IsPause()) {
								SetMousePoint(DrawParts->GetDispXSize() / 2, DrawParts->GetDispYSize() / 2);
								SetMouseDispFlag(FALSE);
							}
							else {
								SetMouseDispFlag(TRUE);
							}
						}
					}
					else {
						SetMouseDispFlag(TRUE);
					}
					auto* OptionParts = OPTION::Instance();
					Look_XradAdd = (float)(mx - DrawParts->GetDispXSize() / 2)*2.f*OptionParts->GetParamFloat(EnumSaveParam::Xsensing);
					Look_YradAdd = -(float)(my - DrawParts->GetDispYSize() / 2)*2.f*OptionParts->GetParamFloat(EnumSaveParam::Ysensing);
				}
				break;
			default:
				break;
		}
		//ボタン
		for (int i = 0; i < (int)PADS::MAX; i++) {
			if (m_ControlType != DXLib_ref::ControlType::PC) {
				switch ((PADS)i) {
					case DXLib_ref::PADS::MOVE_W:
					case DXLib_ref::PADS::MOVE_A:
					case DXLib_ref::PADS::MOVE_S:
					case DXLib_ref::PADS::MOVE_D:
						continue;
						break;
					default:
						break;
				}
			}
			this->m_PadsInfo[i].m_Key.Execute(GetButtonPress(this->m_PadsInfo[i].m_assign));
		}
		//
		KeyEsc.Execute(CheckHitKeyWithCheck(KEY_INPUT_ESCAPE) != 0);
	}
	void PadControl::Draw(float scale) const noexcept {
		int xp = 0;
		int y = y_r((1080 - 21 - 16)*scale);
		for (const auto& k : Key) {
			xp += k->Draw(y_r(32 * scale) + xp, y, scale);
			if (xp > y_r(960 * scale)) {
				xp = 0;
				y -= y_r(28 * scale);
			}
		}
	}
	void PadControl::Dispose(void) noexcept {
		Reset();
	}
};
