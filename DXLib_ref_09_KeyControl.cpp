#include "DXLib_ref_09_KeyControl.hpp"

namespace DXLib_ref {
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	const PadControl* SingletonBase<PadControl>::m_Singleton = nullptr;

	void PadControl::KeyGuideGraphs::AddGuidePC(int ID, std::string_view GuideStr) noexcept {
		if (0 <= ID && ID < KeyNum) {
			std::string Path = "data/key/key_glay/";
			Path += KeyGuidePath[ID];
			Path += ".jpg";
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
	void PadControl::KeyGuideGraphs::AddGuideDS4(int ID, std::string_view GuideStr) noexcept {
		if (0 <= ID && ID < DS4Num) {
			std::string Path = "data/key/key_glay/";
			Path += DS4GuidePath[ID];
			Path += ".png";
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
	int PadControl::KeyGuideGraphs::GetDrawSize() const noexcept {
		auto* Fonts = FontPool::Instance();

		int ofs = 0;
		if (xsize > 0) {
			ofs += xsize + y_r(6);
		}
		ofs += Fonts->Get(FontPool::FontType::Nomal_Edge).GetStringWidth(y_r(21), GuideString) + y_r(6);
		return ofs;
	}

	int PadControl::KeyGuideGraphs::Draw(int x, int y) const noexcept {
		auto* Fonts = FontPool::Instance();

		int ofs = 0;
		if (xsize > 0) {
			GuideImg.DrawExtendGraph(x + ofs, y, x + ofs + xsize, y + ysize, false);
			ofs += xsize + y_r(6);
		}
		Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(21), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, x + ofs, y, GetColor(255, 255, 255), GetColor(0, 0, 0), GuideString);
		return GetDrawSize();
	}

	const bool PadControl::GetButtonPress(int ID) {
		switch (m_ControlType) {
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
			else if (ID < 32) {
				return (input.Buttons[ID] != 0);
			}
		}
		break;
		case DXLib_ref::ControlType::PC:
			//ボタン
			if ((ID & 0xF00) != 0) {
				return ((GetMouseInputWithCheck() & (ID & 0xFF)) != 0);
			}
			else {
				return (CheckHitKeyWithCheck(ID) != 0);
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
		int mdata = FileRead_open(GetSavePath(), FALSE);
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
		if (DXDraw::Instance()->IsPauseSwitch()) {
			m_IsUpdate = true;
		}
		if (m_IsUpdate) {
			m_IsUpdate = false;
			Reset();
			Guide_Pad();
		}
	}

	void PadControl::Execute(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		//コントロールタイプ決定
		{
			ControlType NextControlType = ControlType::PC;
			if (GetJoypadNum() > 0) {
				int padID = DX_INPUT_PAD1;
				GetJoypadInputState(padID);
				switch (GetJoypadType(padID)) {
				case DX_PADTYPE_OTHER:
				case DX_PADTYPE_DUAL_SHOCK_4:
				case DX_PADTYPE_DUAL_SENSE:
				case DX_PADTYPE_SWITCH_JOY_CON_L:
				case DX_PADTYPE_SWITCH_JOY_CON_R:
				case DX_PADTYPE_SWITCH_PRO_CTRL:
				case DX_PADTYPE_SWITCH_HORI_PAD:
					NextControlType = ControlType::PS4;
					break;
				case DX_PADTYPE_XBOX_360:
				case DX_PADTYPE_XBOX_ONE:
					break;
				default:
					break;
				}
			}
			if (m_ControlType != NextControlType) {
				Save();
				m_ControlType = NextControlType;
				m_IsUpdate = true;
				Load();
			}
		}
		//タイプに合わせた操作
		switch (m_ControlType) {
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
				int mx = DrawParts->m_DispXSize / 2, my = DrawParts->m_DispYSize / 2;
				MouseX = mx;
				MouseY = my;
				MouseClick.Execute(false);
				SetMouseDispFlag(TRUE);
				int RS_X = input.Z;
				int RS_Y = input.Rz;
				if (this->m_PadsInfo[(int)PADS::DIR_STICK].m_assign == 0xF001) {
					RS_X = input.X;
					RS_Y = input.Y;
				}

				if (!DrawParts->IsPause()) {
					auto* OptionParts = OPTION::Instance();
					Look_XradAdd = std::clamp((float)RS_X / 100.f*4.f*OptionParts->Get_Xsensing(), -180.f, 180.f);
					Look_YradAdd = std::clamp(-(float)RS_Y / 100.f*4.f*OptionParts->Get_Ysensing(), -180.f, 180.f);
				}
				else {
					Look_XradAdd = 0.f;
					Look_YradAdd = 0.f;
				}
			}
		}
		break;
		case DXLib_ref::ControlType::PC:
			//右スティック
		{
			int mx = DrawParts->m_DispXSize / 2, my = DrawParts->m_DispYSize / 2;
			GetMousePoint(&mx, &my);
			MouseX = mx;
			MouseY = my;
			MouseClick.Execute((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0);
			if (m_MouseMoveEnable) {
				if (GetMainWindowHandle() != GetForegroundWindow()) {//次画面が最前ではないよん
					SetMouseDispFlag(TRUE);
				}
				else {
					if (!DrawParts->IsPause()) {
						if (DrawParts->IsPauseSwitch()) {
							SetMousePoint(DrawParts->m_DispXSize / 2, DrawParts->m_DispYSize / 2);
						}
						SetMousePoint(DrawParts->m_DispXSize / 2, DrawParts->m_DispYSize / 2);
						SetMouseDispFlag(FALSE);
						auto* OptionParts = OPTION::Instance();
						Look_XradAdd = std::clamp((float)(MouseX - DrawParts->m_DispXSize / 2)*2.f*OptionParts->Get_Xsensing(), -180.f, 180.f);
						Look_YradAdd = std::clamp(-(float)(MouseY - DrawParts->m_DispYSize / 2)*2.f*OptionParts->Get_Ysensing(), -180.f, 180.f);
					}
					else {
						SetMouseDispFlag(TRUE);
						Look_XradAdd = 0.f;
						Look_YradAdd = 0.f;
					}
				}
			}
			else {
				SetMouseDispFlag(TRUE);
				Look_XradAdd = 0.f;
				Look_YradAdd = 0.f;
			}
		}
		break;
		default:
			break;
		}
		//ボタン
		for (int i = 0; i < (int)PADS::MAX; i++) {
			if (m_ControlType != DXLib_ref::ControlType::PC) {
				switch ((PADS)i)
				{
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
	void PadControl::Draw() const noexcept {
		int xp = 0;
		int y = y_r(1080 - 21 - 16);
		for (const auto& k : Key) {
			xp += k->Draw(y_r(32) + xp, y);
			if (xp > y_r(960)) {
				xp = 0;
				y -= y_r(24);
			}
		}
	}
	void PadControl::Dispose(void) noexcept {
		Reset();
	}
};
