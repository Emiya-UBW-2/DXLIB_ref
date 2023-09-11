#include "DXLib_ref.h"
//#include "DXLib_ref_09_KeyControl.hpp"

namespace DXLib_ref {
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	const PadControl* SingletonBase<PadControl>::m_Singleton = nullptr;

	void PadControl::Execute(void) noexcept {
		if (GetJoypadNum() > 0) {
			DINPUT_JOYSTATE input;
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
				GetJoypadDirectInputState(DX_INPUT_PAD1, &input);
				//左スティック
				{
					float deg = rad2deg(atan2f((float)input.X, -(float)input.Y));
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
					this->Move_UpKey.Execute(w_key);
					this->Move_DownKey.Execute(s_key);
					this->Move_LeftKey.Execute(a_key);
					this->Move_RightKey.Execute(d_key);
				}
				//十字
				{
					//float deg = (float)(input.POV[0]) / 100.f;
					//bool right_key = (40.f <= deg && deg <= 140.f);
					//bool left_key = (220.f <= deg && deg <= 320.f);
					//bool up_key = (310.f <= deg || deg <= 50.f);
					//bool down_key = (130.f <= deg && deg <= 230.f);
				}
				//ボタン
				{
					//(input.Buttons[0] != 0)/*□*/
					//(input.Buttons[1] != 0)/*×*/
					//(input.Buttons[2] != 0)/*〇*/
					//(input.Buttons[3] != 0)/*△*/
					//(input.Buttons[4] != 0)/*L1*/
					//(input.Buttons[5] != 0)/*R1*/
					//(input.Buttons[6] != 0)/*L2*/
					//(input.Buttons[7] != 0)/*R2*/
					//(input.Buttons[8] != 0)/**/
					//(input.Buttons[9] != 0)/**/
					//(input.Buttons[10] != 0)/*L3*/
					//(input.Buttons[11] != 0)/*R3*/
					this->m_OptionKey.Execute((input.Buttons[9] != 0)/**/);

					this->m_Move_Run.Execute((input.Buttons[10] != 0)/*L3*/);
					this->m_Look_On.Execute((input.Buttons[11] != 0)/*R3*/);

					this->m_QKey.Execute((input.Buttons[4] != 0)/*L1*/);
					this->m_EKey.Execute((input.Buttons[5] != 0)/*R1*/);
					this->m_ADSKey.Execute(input.Buttons[6] != 0);/*L2*/	//ADS
					this->m_ShotKey.Execute((input.Buttons[7] != 0)/*R2*/);

					this->OKKey.Execute((input.Buttons[1] != 0)/*×*/);
					this->NGKey.Execute((input.Buttons[2] != 0)/*〇*/);
					this->m_AccelKey.Execute((input.Buttons[0] != 0)/*□*/);
					this->m_BrakeKey.Execute((input.Buttons[3] != 0)/*△*/);
				}
				//右スティック
				{
					Look_XradAdd = std::clamp((float)(input.Z) / 100.f*0.35f, -9.f, 9.f);
					Look_YradAdd = std::clamp(-(float)(input.Rz) / 100.f*0.35f, -9.f, 9.f);
				}
				return;
			case DX_PADTYPE_XBOX_360:
			case DX_PADTYPE_XBOX_ONE:
				break;
			default:
				break;
			}
		}
		//PC
		{
			//左スティック
			{
				this->Move_UpKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_W) != 0 || CheckHitKeyWithCheck(KEY_INPUT_UP) != 0);
				this->Move_DownKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_S) != 0 || CheckHitKeyWithCheck(KEY_INPUT_DOWN) != 0);
				this->Move_LeftKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_A) != 0 || CheckHitKeyWithCheck(KEY_INPUT_LEFT) != 0);
				this->Move_RightKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_D) != 0 || CheckHitKeyWithCheck(KEY_INPUT_RIGHT) != 0);
			}
			//ボタン
			{
				this->m_OptionKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_TAB) != 0);

				this->m_Move_Run.Execute(CheckHitKeyWithCheck(KEY_INPUT_LSHIFT) != 0);
				this->m_Look_On.Execute((GetMouseInputWithCheck() & MOUSE_INPUT_MIDDLE) != 0);

				this->m_QKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_Q) != 0);
				this->m_EKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_E) != 0);
				this->m_ADSKey.Execute((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0);
				this->m_ShotKey.Execute((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0);

				this->OKKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0);
				this->NGKey.Execute((CheckHitKeyWithCheck(KEY_INPUT_X) != 0) || (CheckHitKeyWithCheck(KEY_INPUT_C) != 0));
				this->m_AccelKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_R) != 0);
				this->m_BrakeKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_F) != 0);
			}
			//右スティック
			{
				int mx = DXDraw::Instance()->m_DispXSize / 2, my = DXDraw::Instance()->m_DispYSize / 2;
				if (DXDraw::Instance()->m_PauseActive.on()) {
					if (DXDraw::Instance()->m_PauseActive.trigger()) {
						SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
					}
					GetMousePoint(&mx, &my);
					SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
					SetMouseDispFlag(FALSE);
				}
				else {
					SetMouseDispFlag(TRUE);
				}
				Look_XradAdd = std::clamp((float)(mx - DXDraw::Instance()->m_DispXSize / 2), -180.f, 180.f);
				Look_YradAdd = std::clamp(-(float)(my - DXDraw::Instance()->m_DispYSize / 2), -180.f, 180.f);
			}
			//
		}
		//
	}
	void PadControl::ChangeGuide(std::function<void()>Guide_Pad_PS4, std::function<void()>Guide_Key) noexcept {
		if (DXDraw::Instance()->m_PauseActive.trigger()) {
			SetGuideUpdate();
		}
		//
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
				if (m_IsPad != true) {
					m_IsUpdate = true;
				}
				m_IsPad = true;
				if (m_IsUpdate) {
					m_IsUpdate = false;
					Guide_Pad_PS4();
				}
				return;
			case DX_PADTYPE_XBOX_360:
			case DX_PADTYPE_XBOX_ONE:
				break;
			default:
				break;
			}
		}
		//PC
		{
			if (m_IsPad != false) {
				m_IsUpdate = true;
			}
			m_IsPad = false;
			if (m_IsUpdate) {
				m_IsUpdate = false;
				Guide_Key();
			}
		}
		//
	}

	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	const KeyGuideClass* SingletonBase<KeyGuideClass>::m_Singleton = nullptr;

	void KeyGuideClass::Keys::AddGuide(std::string_view ImgName, std::string_view GuideStr) noexcept {
		if (ImgName != "") {
			std::string Path = "data/key/key_glay/";
			Path += ImgName;
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
	void KeyGuideClass::Keys::Reset() noexcept {
		GuideImg.Dispose();
		GuideString = "";
	}
	int KeyGuideClass::Keys::Draw(int x, int y) const noexcept {
		auto* Fonts = FontPool::Instance();

		int ofs = 0;
		if (xsize > 0) {
			GuideImg.DrawExtendGraph(x + ofs, y, x + ofs + xsize, y + ysize, false);
			ofs += xsize + y_r(6);
		}
		Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(21), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, x + ofs, y, GetColor(255, 255, 255), GetColor(0, 0, 0), GuideString);
		ofs += Fonts->Get(FontPool::FontType::Nomal_Edge).GetStringWidth(y_r(21), GuideString) + y_r(6);
		return ofs;
	}

	void KeyGuideClass::Reset(void) noexcept {
		for (auto& k : Key) {
			k->Reset();
			k.reset();
		}
		Key.clear();
	}
	void KeyGuideClass::AddGuide(std::string_view ImgName, std::string_view GuideStr) noexcept {
		Key.resize(Key.size() + 1);
		Key.back() = std::make_unique<Keys>();
		Key.back()->AddGuide(ImgName, GuideStr);
	}
	void KeyGuideClass::Draw() const noexcept {
		int x = y_r(32);
		int y = y_r(1080 - 21 - 16);
		for (const auto& k : Key) {
			x += k->Draw(x, y);
		}
	}

};
