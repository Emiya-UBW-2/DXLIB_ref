#include "DXLib_ref_10_Option.hpp"

#define EdgeSize	y_r(2)
#define LineHeight	y_r(18)

namespace DXLib_ref {
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	const OPTION* SingletonBase<OPTION>::m_Singleton = nullptr;

	void			OPTION::Load(void) noexcept {
		SetOutApplicationLogValidFlag(FALSE);
		int mdata = FileRead_open("data/Setting.txt", FALSE);
		grass_level = std::clamp<int>(getparams::_int(mdata), 0, 4);
		DoF = getparams::_bool(mdata);
		Bloom = getparams::_bool(mdata);
		Shadow = getparams::_bool(mdata);
		useVR = getparams::_bool(mdata);
		SSAO = getparams::_bool(mdata);
		Fov = getparams::_float(mdata);
		Vsync = getparams::_bool(mdata);
		SE = getparams::_float(mdata);
		VOICE = getparams::_float(mdata);
		BGM = getparams::_float(mdata);
		AllWaysFront = getparams::_bool(mdata);
		aberration = getparams::_bool(mdata);
		{
			std::string DctXVer = getparams::_str(mdata);
			if (DctXVer == "9c") {
				DirectXVer = DX_DIRECT3D_9EX;
			}
			else if (DctXVer == "11") {
				DirectXVer = DX_DIRECT3D_11;
			}
		}
		SSR = getparams::_bool(mdata);
		MotionBlur = getparams::_bool(mdata);
		Xsensing = getparams::_float(mdata);
		Ysensing = getparams::_float(mdata);
		FileRead_close(mdata);
		SetOutApplicationLogValidFlag(TRUE);
	}
	void			OPTION::Save(void) noexcept {
		std::ofstream outputfile("data/setting.txt");
		outputfile << "grass_level=" + std::to_string(grass_level) + "\n";
		outputfile << "DoF=" + std::string((DoF) ? "true" : "false") + "\n";
		outputfile << "bloom=" + std::string((Bloom) ? "true" : "false") + "\n";
		outputfile << "shadow=" + std::string((Shadow) ? "true" : "false") + "\n";
		outputfile << "usevr=" + std::string((useVR) ? "true" : "false") + "\n";
		outputfile << "SSAO=" + std::string((SSAO) ? "true" : "false") + "\n";
		outputfile << "fov=" + std::to_string(Fov) + "\n";
		outputfile << "vsync=" + std::string((Vsync) ? "true" : "false") + "\n";
		outputfile << "SE=" + std::to_string(SE) + "\n";
		outputfile << "VOICE=" + std::to_string(VOICE) + "\n";
		outputfile << "BGM=" + std::to_string(BGM) + "\n";
		outputfile << "AllWaysFront=" + std::string((AllWaysFront) ? "true" : "false") + "\n";
		outputfile << "aberration=" + std::string((aberration) ? "true" : "false") + "\n";
		switch (DirectXVer) {
			case  DX_DIRECT3D_9EX:
				outputfile << "DirectXVer=" + std::string("9c") + "\n";
				break;
			case  DX_DIRECT3D_11:
				outputfile << "DirectXVer=" + std::string("11") + "\n";
				break;
			default:
				break;
		}
		outputfile << "SSR=" + std::string((SSR) ? "true" : "false") + "\n";
		outputfile << "MotionBlur=" + std::string((MotionBlur) ? "true" : "false") + "\n";
		outputfile << "Xsensing=" + std::to_string(Xsensing) + "\n";
		outputfile << "Ysensing=" + std::to_string(Ysensing) + "\n";
		outputfile.close();
	}

	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	const OptionWindowClass* SingletonBase<OptionWindowClass>::m_Singleton = nullptr;

	//カラー指定
	static const unsigned int Red{GetColor(255, 0, 0)};
	static const unsigned int DarkGreen{GetColor(21, 128, 45)};
	static const unsigned int Green{GetColor(43, 255, 91)};
	static const unsigned int White{GetColor(255, 255, 255)};
	static const unsigned int Gray25{GetColor(192, 192, 192)};
	static const unsigned int Gray50{GetColor(128, 128, 128)};
	static const unsigned int Gray75{GetColor(64, 64, 64)};
	static const unsigned int Black{GetColor(0, 0, 0)};

	namespace WindowSystem {
		//箱
		static void SetBox(int xp1, int yp1, int xp2, int yp2, unsigned int colorSet) {
			DrawBox(xp1, yp1, xp2, yp2, colorSet, true);
		};
		//文字
		template <typename... Args>
		static const auto GetMsgLen(int size, std::string_view String, Args&&... args) {
			auto* Fonts = FontPool::Instance();
			return Fonts->Get(FontPool::FontType::Nomal_EdgeL, size).GetStringWidth(-1, ((std::string)String).c_str(), args...) + y_r(6) + 2;//エッジ分:
		}
		template <typename... Args>
		static const auto SetMsg(int xp1, int yp1, int xp2, int yp2, int size, FontHandle::FontXCenter FontX, unsigned int Color, unsigned int EdleColor, std::string_view String, Args&&... args) {
			if (String == "") { return 0; }
			auto* DrawParts = DXDraw::Instance();
			auto* Fonts = FontPool::Instance();
			int xSize = GetMsgLen(size, String, args...);
			int xpos = 0;
			int ypos = yp1 + (yp2 - yp1) / 2;
			if ((ypos - size / 2) > DrawParts->m_DispYSize || (ypos + size / 2) < 0) { return 0; }				//画面外は表示しない
			switch (FontX) {
				case FontHandle::FontXCenter::LEFT:
					xpos = xp1 + y_r(6);
					if ((xpos) > DrawParts->m_DispXSize || (xpos + xSize) < 0) { return 0; }						//画面外は表示しない
					break;
				case FontHandle::FontXCenter::MIDDLE:
					xpos = xp1 + (xp2 - xp1) / 2;
					if ((xpos - xSize / 2) > DrawParts->m_DispXSize || (xpos + xSize / 2) < 0) { return 0; }		//画面外は表示しない
					break;
				case FontHandle::FontXCenter::RIGHT:
					xpos = xp2 - y_r(6);
					if ((xpos - xSize) > DrawParts->m_DispXSize || (xpos) < 0) { return 0; }						//画面外は表示しない
					break;
				default:
					break;
			}
			Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(size, FontX, FontHandle::FontYCenter::MIDDLE, xpos, ypos, Color, EdleColor, ((std::string)String).c_str(), args...);
			return xSize;//エッジ分
		};
		//
		template <typename... Args>
		static void SetMsgBox(int xp1, int yp1, int xp2, int yp2, unsigned int defaultcolor, std::string_view String, Args&&... args) {
			SetBox(xp1, yp1, xp2, yp2, defaultcolor);
			SetMsg(xp1, yp1, xp2, yp2, std::min(LineHeight, yp2 - yp1), FontHandle::FontXCenter::MIDDLE, White, Black, String, args...);
		};

		static bool CheckBox(int xp1, int yp1, bool switchturn) {
			int xp3 = xp1 + EdgeSize;
			int yp3 = yp1 + EdgeSize;
			int xp4 = xp1 + LineHeight * 2 - EdgeSize;
			int yp4 = yp1 + LineHeight - EdgeSize;

			auto* Pad = PadControl::Instance();
			bool MouseOver = in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xp3, yp3, xp4, yp4);
			if (MouseOver && Pad->GetMouseClick().trigger()) {
				switchturn ^= 1;
				auto* SE = SoundPool::Instance();
				SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}
			unsigned int color = Gray25;
			SetBox(xp3 + y_r(5), yp3 + y_r(5), xp4 - y_r(5), yp4 - y_r(5), Black);
			xp4 = xp1 + LineHeight * (switchturn ? 1 : 0) - EdgeSize;
			SetBox(xp3 + y_r(5), yp3 + y_r(5), xp4 + y_r(5), yp4 - y_r(5), Gray50);
			xp3 = xp1 + LineHeight * (switchturn ? 1 : 0) + EdgeSize;
			xp4 = xp1 + LineHeight * (switchturn ? 2 : 1) - EdgeSize;
			SetBox(xp3, yp3, xp4, yp4, color);
			return switchturn;
		}

		static int UpDownBar(int xmin, int xmax, int yp, int value, int valueMin, int valueMax) {
			int xp = 0;
			{
				int xpmin = xmin + 1;
				int xpmax = xmax - 1;

				auto* Pad = PadControl::Instance();
				bool MouseOver = in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xpmin, yp, xpmin + (xpmax - xpmin), yp + LineHeight);
				if (MouseOver && Pad->GetMouseClick().trigger()) {
					auto* SE = SoundPool::Instance();
					SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
				if (MouseOver && Pad->GetMouseClick().press()) {
					value = ((valueMax - valueMin) * (Pad->GetMS_X() - xpmin) / (xpmax - xpmin)) + valueMin;
				}

				SetBox(xpmin, yp, xpmin + (xpmax - xpmin), yp + LineHeight, DarkGreen);
				SetBox(xpmin, yp, xpmin + (xpmax - xpmin)*std::clamp(value - valueMin, 0, valueMax - valueMin) / (valueMax - valueMin), yp + LineHeight,
					   MouseOver ? (Pad->GetMouseClick().press() ? Gray25 : White) : Green);
			}
			xp = (xmin + (xmax - xmin) / 2);
			SetMsg(xp, yp, xp, yp + LineHeight, LineHeight, FontHandle::FontXCenter::MIDDLE, White, Black, "%03d", value);

			return value;
		}
		//
		/*
		class ScrollBoxClass {
			bool		m_IsChangeScrollY{ false };
			int			m_BaseScrollY{ 0 };
			float		m_NowScrollYPer{ 0.f };
		public:
			const auto&		GetNowScrollYPer(void) const noexcept { return this->m_NowScrollYPer; }
			void			ScrollBox(int xp1, int yp1, int xp2, int yp2, float TotalPer, bool IsActive) {
				auto* Pad = PadControl::Instance();
				unsigned int color = Gray25;

				int length = (int)((float)(yp2 - yp1) / TotalPer);
				float Total = (float)(yp2 - yp1 - length);
				int Yp_t = (int)(Total * this->m_NowScrollYPer);
				int Yp_s = std::max(yp1, yp1 + Yp_t);
				int Yp_e = std::min(yp2, Yp_s + length);

				if (IsActive) {
					if (in2_(Pad->GetMouseX(), Pad->GetMouseY(), xp1, yp1, xp2, yp2)) {
						if (Pad->GetWheelAdd() != 0.f) {
							m_NowScrollYPer = std::clamp(m_NowScrollYPer + (float)(-Pad->GetWheelAdd() * 3) / Total, 0.f, 1.f);
						}
					}
					if (in2_(Pad->GetMouseX(), Pad->GetMouseY(), xp2 - y_r(24), yp1, xp2, yp2)) {
						if (Pad->GetINTERACTKey().trigger()) {
							m_IsChangeScrollY = true;
						}

						if (!m_IsChangeScrollY) {
							HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
							SetCursor(hCursor);
						}
					}
					if (m_IsChangeScrollY) {
						if (Pad->GetINTERACTKey().press()) {
							color = White;
							m_NowScrollYPer = std::clamp((float)(Pad->GetMouseY() - this->m_BaseScrollY) / Total, 0.f, 1.f);

							HCURSOR hCursor = LoadCursor(NULL, IDC_SIZENS);
							SetCursor(hCursor);
						}
						else {
							m_IsChangeScrollY = false;
						}
					}
					else {
						m_BaseScrollY = Pad->GetMouseY() - Yp_t;
						if (Pad->GetMouseY() < Yp_s) {
							m_BaseScrollY += Yp_s - Pad->GetMouseY();
						}

						if (Pad->GetMouseY() > Yp_e) {
							m_BaseScrollY += Yp_e - Pad->GetMouseY();
						}
					}
				}
				SetBox(xp2 - y_r(24), yp1, xp2, yp2, Gray50);
				SetBox(xp2 - y_r(24) + y_r(1), Yp_s, xp2 - y_r(1), Yp_e, color);
			};
		};
		//*/
	};

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
			if (Pad->GetKey(PADS::INTERACT).trigger()) {
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
			auto* Pad = PadControl::Instance();
			bool MouseOver = in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xp1, yp1 - y_r(5), xp1 + y_r(140), yp1 + LineHeight * 2 + y_r(5));
			if (MouseOver && Pad->GetMouseClick().trigger()) {
				*TabSel = GetID();
				auto* SE = SoundPool::Instance();
				SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}
			WindowSystem::SetMsgBox(xp1, yp1 + y_r(5), xp1 + y_r(140), yp1 + LineHeight * 2 - y_r(5),
									MouseOver ? (Pad->GetMouseClick().press() ? Gray25 : White) : (isActive ? Gray25 : Gray75),
									m_name);
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
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_BGM(std::clamp(OptionParts->Get_BGM() - 0.1f, 0.f, 1.f));
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_BGM(std::clamp(OptionParts->Get_BGM() + 0.1f, 0.f, 1.f));
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 int value = WindowSystem::UpDownBar(xpos, xpos + y_r(200), ypos, (int)(OptionParts->Get_BGM()*100.f + 0.5f), 0, 100);
										 OptionParts->Set_BGM((float)value / 100.f);
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
										 auto* OptionParts = OPTION::Instance();
										 int value = WindowSystem::UpDownBar(xpos, xpos + y_r(200), ypos, (int)(OptionParts->Get_SE()*100.f + 0.5f), 0, 100);
										 OptionParts->Set_SE((float)value / 100.f);
									 }
									 );
	}
	void OptionWindowClass::GraphicTabsInfo::Init_Sub() noexcept {
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
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_Vsync(OptionParts->Get_Vsync() ^ 1);
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
										 [&]() {},
										 [&]() {},
										 [&](int xpos, int ypos, bool) {
										 auto* OptionParts = OPTION::Instance();
										 OptionParts->Set_Vsync(WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_Vsync()));
									 }
									 );
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("DirectX Version", "DirectXのバージョンを変更します(反映は再起動後にされます)",
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 switch (OptionParts->Get_DirectXVer()) {
											 case  DX_DIRECT3D_9EX:
												 OptionParts->Set_DirectXVer(DX_DIRECT3D_11);
												 break;
											 case  DX_DIRECT3D_11:
												 OptionParts->Set_DirectXVer(DX_DIRECT3D_9EX);
												 break;
											 default:
												 break;
										 }
										 SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
									 },
									 [&]() {
										 auto* SE = SoundPool::Instance();
										 auto* OptionParts = OPTION::Instance();
										 switch (OptionParts->Get_DirectXVer()) {
											 case  DX_DIRECT3D_9EX:
												 OptionParts->Set_DirectXVer(DX_DIRECT3D_11);
												 break;
											 case  DX_DIRECT3D_11:
												 OptionParts->Set_DirectXVer(DX_DIRECT3D_9EX);
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
										 if (WindowSystem::CheckBox(xpos, ypos, (OptionParts->Get_DirectXVer() == DX_DIRECT3D_11))) {
											 OptionParts->Set_DirectXVer(DX_DIRECT3D_11);
										 }
										 else {
											 OptionParts->Set_DirectXVer(DX_DIRECT3D_9EX);
										 }

										 WindowSystem::SetMsg(xpos + y_r(100), ypos, xpos + y_r(100), ypos + LineHeight,
															  LineHeight, FontHandle::FontXCenter::MIDDLE, (OptionParts->Get_DirectXVer() == DX_DIRECT3D_9EX) ? White : Gray50, Black, "9.0C");
										 WindowSystem::SetMsg(xpos + y_r(200), ypos, xpos + y_r(200), ypos + LineHeight,
															  LineHeight, FontHandle::FontXCenter::RIGHT, (OptionParts->Get_DirectXVer() == DX_DIRECT3D_11) ? White : Gray50, Black, "11.0");
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
		this->m_Elements.back().Init("Grass", "木の表示をするかを指定します",
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
			if (Pad->GetKey(PADS::LEAN_L).trigger() && (m_select != 3)) {
				--m_tabsel;
				if (m_tabsel < 0) { m_tabsel = (int)m_Tabs.size() - 1; }
				m_select = 0;
				SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}
			if (Pad->GetKey(PADS::LEAN_R).trigger() && (m_select != 3)) {
				++m_tabsel;
				if (m_tabsel > (int)m_Tabs.size() - 1) { m_tabsel = 0; }
				m_select = 0;
				SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}

			m_Tabs.at(m_tabsel)->Execute(&m_select, (m_select != 3));
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
