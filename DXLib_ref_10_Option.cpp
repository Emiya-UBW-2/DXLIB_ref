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
			if (DctXVer == "9") {
				DirectXVer = DX_DIRECT3D_9;
			}
			else if (DctXVer == "9c") {
				DirectXVer = DX_DIRECT3D_9EX;
			}
			else if (DctXVer == "11") {
				DirectXVer = DX_DIRECT3D_11;
			}
		}
		SSR = getparams::_bool(mdata);

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
		case  DX_DIRECT3D_9:
			outputfile << "DirectXVer=" + std::string("9") + "\n";
			break;
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
		outputfile.close();
	}

	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	const OptionWindowClass* SingletonBase<OptionWindowClass>::m_Singleton = nullptr;

	//カラー指定
	static const unsigned int Red{ GetColor(255, 0, 0) };
	static const unsigned int DarkGreen{ GetColor(21, 128, 45) };
	static const unsigned int Green{ GetColor(43, 255, 91) };
	static const unsigned int White{ GetColor(255, 255, 255) };
	static const unsigned int Gray25{ GetColor(192, 192, 192) };
	static const unsigned int Gray50{ GetColor(128, 128, 128) };
	static const unsigned int Gray75{ GetColor(64, 64, 64) };
	static const unsigned int Black{ GetColor(0, 0, 0) };

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
		static bool ClickCheckBox(int xp1, int yp1, int xp2, int yp2, bool IsActive, unsigned int defaultcolor, std::string_view String, Args&&... args) {
			auto* Pad = PadControl::Instance();
			unsigned int color = defaultcolor;
			bool isHit = false;
			if (IsActive) {
				color = White;
				if (Pad->GetOKKey().press()) {
					if (Pad->GetOKKey().trigger()) {
						isHit = true;
					}
					color = Black;
				}
				HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
				SetCursor(hCursor);
			}
			SetBox(xp1, yp1, xp2, yp2, color);
			SetMsg(xp1, yp1, xp2, yp2, std::min(LineHeight, yp2 - yp1), FontHandle::FontXCenter::MIDDLE, White, Black, String, args...);
			return isHit;
		};

		static void CheckBox(int xp1, int yp1, bool switchturn) {
			int xp3 = xp1 + EdgeSize;
			int yp3 = yp1 + EdgeSize;
			int xp4 = xp1 + LineHeight * 2 - EdgeSize;
			int yp4 = yp1 + LineHeight - EdgeSize;
			unsigned int color = Gray25;
			SetBox(xp3 + y_r(5), yp3 + y_r(5), xp4 - y_r(5), yp4 - y_r(5), Black);
			xp4 = xp1 + LineHeight * (switchturn ? 1 : 0) - EdgeSize;
			SetBox(xp3 + y_r(5), yp3 + y_r(5), xp4 + y_r(5), yp4 - y_r(5), Gray50);
			xp3 = xp1 + LineHeight * (switchturn ? 1 : 0) + EdgeSize;
			xp4 = xp1 + LineHeight * (switchturn ? 2 : 1) - EdgeSize;
			SetBox(xp3, yp3, xp4, yp4, color);
		}

		static void	UpDownBar(int xmin, int xmax, int yp, int value, int valueMin, int valueMax) {
			int xp = 0;
			{
				int xpmin = xmin + 1;
				int xpmax = xmax - 1;
				SetBox(xpmin, yp, xpmin + (xpmax - xpmin), yp + LineHeight, DarkGreen);
				SetBox(xpmin, yp, xpmin + (xpmax - xpmin)*std::clamp(value - valueMin, 0, valueMax - valueMin) / (valueMax - valueMin), yp + LineHeight, Green);
			}
			xp = (xmin + (xmax - xmin) / 2);
			SetMsg(xp, yp, xp, yp + LineHeight, LineHeight, FontHandle::FontXCenter::MIDDLE, White, Black, "%03d", value);
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
						if (Pad->GetOKKey().trigger()) {
							m_IsChangeScrollY = true;
						}

						if (!m_IsChangeScrollY) {
							HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
							SetCursor(hCursor);
						}
					}
					if (m_IsChangeScrollY) {
						if (Pad->GetOKKey().press()) {
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
		m_Draw(xpos + y_r(720 - 324), ypos);
	}
	//
	void OptionWindowClass::OptionTabsInfo::Execute(int *select) noexcept {
		if ((*select) < 0) { return; }
		auto* SE = SoundPool::Instance();
		auto* Pad = PadControl::Instance();
		if (Pad->GetUpKey().trigger()) {
			--(*select);
			if ((*select) < 0) { (*select) = (int)m_Elements.size() - 1; }
			m_Elements.at((*select)).selanim = 3.f;
			SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
		}
		if (Pad->GetDownKey().trigger()) {
			++(*select);
			if ((*select) > (int)m_Elements.size() - 1) { (*select) = 0; }
			m_Elements.at((*select)).selanim = -3.f;
			SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
		}
		if (Pad->GetLeftKey().trigger()) {
			m_Elements.at((*select)).GetLeftPush();
		}
		if (Pad->GetRightKey().trigger()) {
			m_Elements.at((*select)).GetRightPush();
		}
		if (Pad->GetOKKey().trigger()) {
			m_Elements.at((*select)).GetOKPush();
		}
		for (auto& e : m_Elements) {
			Easing(&e.selanim, 0.f, 0.95f, EasingType::OutExpo);
		}
	}
	void OptionWindowClass::OptionTabsInfo::Draw(int xpos, int ypos, bool isActive, int select) noexcept {
		int xp1, yp1;
		//タブ
		{
			xp1 = xpos + (y_r(200) + y_r(12)) * m_id;
			yp1 = ypos;
			WindowSystem::ClickCheckBox(xp1, yp1 + y_r(5), xp1 + y_r(200), yp1 + LineHeight * 2 - y_r(5), true, isActive ? Gray25 : Gray75, m_name);
		}
		//内容
		if (isActive) {
			xp1 = xpos;
			yp1 = ypos + LineHeight * 3;
			for (int i = 0; i < (int)m_Elements.size(); i++) {
				yp1 += (LineHeight + y_r(6));
				m_Elements.at(i).Draw(xp1, yp1, (select == i));
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
			[&](int xpos, int ypos) {
			auto* OptionParts = OPTION::Instance();
			WindowSystem::UpDownBar(xpos, xpos + y_r(200), ypos, (int)(OptionParts->Get_BGM()*100.f + 0.5f), 0, 100);
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
			[&](int xpos, int ypos) {
			auto* OptionParts = OPTION::Instance();
			WindowSystem::UpDownBar(xpos, xpos + y_r(200), ypos, (int)(OptionParts->Get_SE()*100.f + 0.5f), 0, 100);
		}
		);
	}
	void OptionWindowClass::GraphicTabsInfo::Init_Sub() noexcept {
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
			[&](int xpos, int ypos) {
			auto* OptionParts = OPTION::Instance();
			WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_Vsync());
		}
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("DirectX Version", "DirectXのバージョンを変更します(反映は再起動後にされます)",
			[&]() {
			auto* SE = SoundPool::Instance();
			auto* OptionParts = OPTION::Instance();
			switch (OptionParts->Get_DirectXVer()) {
			case  DX_DIRECT3D_9:
				OptionParts->Set_DirectXVer(DX_DIRECT3D_11);
				break;
			case  DX_DIRECT3D_9EX:
				OptionParts->Set_DirectXVer(DX_DIRECT3D_9);
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
			case  DX_DIRECT3D_9:
				OptionParts->Set_DirectXVer(DX_DIRECT3D_9EX);
				break;
			case  DX_DIRECT3D_9EX:
				OptionParts->Set_DirectXVer(DX_DIRECT3D_11);
				break;
			case  DX_DIRECT3D_11:
				OptionParts->Set_DirectXVer(DX_DIRECT3D_9);
				break;
			default:
				break;
			}
			SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
		},
			[&]() {},
			[&](int xpos, int ypos) {
			auto* OptionParts = OPTION::Instance();
			WindowSystem::SetMsg(xpos, ypos, xpos, ypos + LineHeight,
				LineHeight, FontHandle::FontXCenter::LEFT, (OptionParts->Get_DirectXVer() == DX_DIRECT3D_9) ? White : Gray50, Black, "9.0");
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
			OptionParts->Set_Vsync(OptionParts->Get_Vsync() ^ 1);
			SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
		},
			[&]() {
			auto* SE = SoundPool::Instance();
			auto* OptionParts = OPTION::Instance();
			OptionParts->Set_SSAO(OptionParts->Get_SSAO() ^ 1);
			SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
		},
			[&]() {},
			[&](int xpos, int ypos) {
			auto* OptionParts = OPTION::Instance();
			WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_SSAO());
		}
		);
		this->m_Elements.resize(this->m_Elements.size() + 1);
		this->m_Elements.back().Init("SSR", "画面ベースの鏡面反射の有効無効を指定します DirectX11でのみ有効です",
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
			[&](int xpos, int ypos) {
			auto* OptionParts = OPTION::Instance();
			WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_SSR());
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
			[&](int xpos, int ypos) {
			auto* OptionParts = OPTION::Instance();
			WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_Shadow());
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
			[&](int xpos, int ypos) {
			auto* OptionParts = OPTION::Instance();
			WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_grass_level() > 0);
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
			[&](int xpos, int ypos) {
			auto* OptionParts = OPTION::Instance();
			WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_Bloom());
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
			[&](int xpos, int ypos) {
			auto* OptionParts = OPTION::Instance();
			WindowSystem::CheckBox(xpos, ypos, OptionParts->Get_aberration());
		}
		);
	}
	//
	void OptionWindowClass::Init() noexcept {
		m_tabsel = 0;
		m_select = 0;
		m_isActive = false;
		m_ActivePer = 0.f;
		//
		m_Tabs.at(0) = std::make_unique<SoundTabsInfo>();
		m_Tabs.at(0)->Init(0, "Sound");
		m_Tabs.at(1) = std::make_unique<GraphicTabsInfo>();
		m_Tabs.at(1)->Init(1, "Graphic");
		//
	}
	void OptionWindowClass::Execute(void) noexcept {
		if (m_isActive) {
			auto* Pad = PadControl::Instance();
			auto* SE = SoundPool::Instance();
			/*
			//呼び出しもとで呼んでなければこれを渡す
			Pad->Execute(
				[&]() {},
				[&]() {});
			//*/
			//
			if (Pad->GetQKey().trigger()) {
				--m_tabsel;
				if (m_tabsel < 0) { m_tabsel = (int)m_Tabs.size() - 1; }
				m_select = 0;
				SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}
			if (Pad->GetEKey().trigger()) {
				++m_tabsel;
				if (m_tabsel > (int)m_Tabs.size() - 1) { m_tabsel = 0; }
				m_select = 0;
				SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}

			m_Tabs.at(m_tabsel)->Execute(&m_select);
			//
			if (Pad->GetNGKey().trigger()) {
				SE->Get((int)SoundEnumCommon::UI_NG).Play(0, DX_PLAYTYPE_BACK, TRUE);
				m_isActive = false;
			}
			if (!m_isActive) {
				OPTION::Instance()->Save();
			}
		}
		else {
			m_tabsel = 0;
			m_select = 0;
		}
		Easing(&m_ActivePer, m_isActive ? 1.f : 0.f, 0.5f, EasingType::OutExpo);
	}
	void OptionWindowClass::Draw() const noexcept {
		int WinSizeX = y_r(720);
		int WinSizeY = y_r(480);
		//背景
		if (m_ActivePer > 0.f) {
			auto per = std::clamp(m_ActivePer * 0.3f, 0.f, 1.f);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
			WindowSystem::SetBox(y_r(960) - WinSizeX / 2, y_r(540) - WinSizeY / 2, y_r(960) + WinSizeX / 2, y_r(540) + WinSizeY / 2, Gray50);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		if (m_ActivePer > 0.9f) {
			int xp1, yp1;
			//タイトル
			{
				xp1 = y_r(960) - WinSizeX / 2 + y_r(32);
				yp1 = y_r(540) - WinSizeY / 2 + LineHeight/4;
				WindowSystem::SetMsg(xp1, yp1, xp1, yp1 + LineHeight * 2, LineHeight * 2, FontHandle::FontXCenter::LEFT, White, Black, "Option");
			}
			//タブ
			xp1 = y_r(960) - WinSizeX / 2;
			yp1 = y_r(540) - WinSizeY / 2 + LineHeight * 3;

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*0.3), 0, 255));
			WindowSystem::SetBox(xp1 + y_r(24), yp1, xp1 + WinSizeX - y_r(24), yp1 + LineHeight * 2, Gray50);
			WindowSystem::SetBox(xp1 + y_r(24), yp1 + LineHeight * 3, xp1 + WinSizeX - y_r(24), y_r(540) + WinSizeY / 2 - LineHeight, Gray50);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

			xp1 = y_r(960) - WinSizeX / 2 + y_r(48);
			yp1 = y_r(540) - WinSizeY / 2 + LineHeight * 3;
			for (auto& t : m_Tabs) {
				t->Draw(xp1, yp1, m_tabsel == t->GetID(), m_select);
			}
			//ガイド
			xp1 = y_r(960) - WinSizeX / 2 + y_r(48);
			yp1 = y_r(540) + WinSizeY / 2 - LineHeight * 5/2;
			m_Tabs.at(m_tabsel)->DrawInfo(xp1, yp1, m_select);
			//
		}
	}
};
