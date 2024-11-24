﻿#include "DXLib_ref_14_Popup.hpp"

namespace DXLibRef {
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	const PopUp* SingletonBase<PopUp>::m_Singleton = nullptr;
	const OptionPopup* SingletonBase<OptionPopup>::m_Singleton = nullptr;
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	void PopUp::PopUpDrawClass::Start(void) noexcept {
		auto* SE = SoundPool::Instance();
		auto* KeyGuideParts = KeyGuide::Instance();

		KeyGuideParts->SetGuideFlip();
		KeyGuideParts->ChangeGuide(
			[this]() {
				auto* KeyGuideParts = KeyGuide::Instance();
				auto* LocalizeParts = LocalizePool::Instance();
				KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::RELOAD), LocalizeParts->Get(9991));
				if (m_GuideDoing) {
					m_GuideDoing();
				}
			}
		);
		SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_OK))->Play(DX_PLAYTYPE_BACK, TRUE);

		m_Active = true;
		m_ActiveSwitch = true;
	}
	void PopUp::PopUpDrawClass::End(void) noexcept {
		auto* SE = SoundPool::Instance();
		auto* KeyGuideParts = KeyGuide::Instance();

		SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_CANCEL))->Play(DX_PLAYTYPE_BACK, TRUE);
		m_Active = false;
		m_ActiveSwitch = true;
		KeyGuideParts->SetGuideFlip();
		if (m_ExitDoing) {
			m_ExitDoing();
		}
	}
	void PopUp::PopUpDrawClass::Update(void) noexcept {
		auto* Pad = PadControl::Instance();
		m_ActiveSwitch = false;
		Easing(&m_ActivePer, m_Active ? 1.f : 0.f, m_Active ? 0.7f : 0.3f, EasingType::OutExpo);

		if (m_Active) {
			Pad->SetMouseMoveEnable(false);
			if (Pad->GetPadsInfo(Controls::PADS::RELOAD).GetKey().trigger()) {
				End();
			}
		}
	}
	void PopUp::PopUpDrawClass::Draw(int xcenter, int ycenter) noexcept {
		if (m_ActivePer < (1.f / 255.f)) { return; }

		auto* LocalizeParts = LocalizePool::Instance();
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();

		int xm1 = xcenter - (WinSizeX) / 2;
		int ym1 = ycenter - (WinSizeY) / 2;
		int xm2 = xcenter + (WinSizeX) / 2;
		int ym2 = ycenter + (WinSizeY) / 2;

		// 背景
		auto per = std::clamp(m_ActivePer * 0.5f, 0.f, 1.f);
		DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * per), 0, 255));
		DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xm1, ym1, xm2, ym2, Gray50, true);
		DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);

		// タイトル
		DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight * 2,
			FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xm1 + (32), ym1 + LineHeight / 4, White, Black, m_WindwoName);
		// 
		if (m_Active) {
			int xp1 = xm2 - (140);
			int yp1 = ym1 + LineHeight / 4 + LineHeight / 2;
			if (WindowSystem::SetMsgClickBox(xp1, yp1 + (5), xp1 + (108), yp1 + LineHeight * 2 - (5), LineHeight, Red, false, true, LocalizeParts->Get(20))) {
				End();
			}
		}
		// 背景
		{
			int xp1 = xm1 + (24);
			int yp1 = ym1 + LineHeight * 3;
			int xp2 = xm2 - (24);
			int yp2 = ym2 - LineHeight;
			DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * 0.5f), 0, 255));
			DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xp1, yp1, xp2, yp2, Gray50, true);
			DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			if (m_Doing) {
				m_Doing(xp1, yp1, xp2, yp2, m_ActiveSwitch);
			}
		}
	}
	void PopUp::Add(const char* WindowName, int sizex, int sizey,
		std::function<void(int xmin, int ymin, int xmax, int ymax, bool EndSwitch)> doing,
		std::function<void()> ExitDoing,
		std::function<void()> GuideDoing,
		bool IsInsert) noexcept {
		auto& Last = que.at(LastSel);
		Last.Set(WindowName, sizex, sizey, doing, ExitDoing, GuideDoing);
		if (!IsActivePop()) {
			Last.Start();
			auto* SceneParts = SceneControl::Instance();
			PrevPause = SceneParts->IsPause();
			SceneParts->ChangePause(true);
		}
		else if (IsInsert) {
			que.at(NowSel).End();
			NowSel = LastSel;
			Last.Start();
		}
		++LastSel %= que.size();
	}
	void PopUp::EndAll(void) noexcept {
		if (!IsActivePop()) {
			return;
		}
		que.at(NowSel).End();
		NowSel = LastSel;
	}
	void PopUp::Update(void) noexcept {
		if (!IsActivePop()) {
			return;
		}
		que.at(NowSel).Update();
		if (que.at(NowSel).IsEnd()) {
			++NowSel %= que.size();
			if (IsActivePop()) {
				que.at(NowSel).Start();
			}
			else {
				if (!PrevPause) {
					auto* SceneParts = SceneControl::Instance();
					SceneParts->ChangePause(false);
				}
			}
		}
	}
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	// オンオフできるボタン
	static bool CheckBox(int xp1, int yp1, bool switchturn) noexcept {
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		int xp3 = xp1 + EdgeSize;
		int yp3 = yp1 + EdgeSize;
		int xp4 = xp1 + LineHeight * 2 - EdgeSize;
		int yp4 = yp1 + LineHeight - EdgeSize;

		auto* Pad = PadControl::Instance();
		bool MouseOver = IntoMouse(xp3, yp3, xp4, yp4);
		if (MouseOver && Pad->GetMouseClick().trigger()) {
			switchturn ^= 1;
			auto* SE = SoundPool::Instance();
			SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
		}
		unsigned int color = Gray25;
		int Edge = (5);
		DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xp3 + Edge, yp3 + Edge, xp4 - Edge, yp4 - Edge, Black, true);
		xp4 = xp1 + LineHeight * (switchturn ? 1 : 0) - EdgeSize;
		DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xp3 + Edge, yp3 + Edge, xp4 + Edge, yp4 - Edge, Gray50, true);
		xp3 = xp1 + LineHeight * (switchturn ? 1 : 0) + EdgeSize;
		xp4 = xp1 + LineHeight * (switchturn ? 2 : 1) - EdgeSize;
		DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xp3, yp3, xp4, yp4, color, true);
		return switchturn;
	}
	// スライダー
	static int UpDownBar(int xmin, int xmax, int yp, int value, int valueMin, int valueMax) noexcept {
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		int xpmin = xmin + 1;
		int xpmax = xmax - 1;

		auto* Pad = PadControl::Instance();
		bool MouseOver = IntoMouse(xpmin - 5, yp, xpmin + (xpmax - xpmin) + 5, yp + LineHeight);
		if (MouseOver && Pad->GetMouseClick().trigger()) {
			auto* SE = SoundPool::Instance();
			SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
		}
		if (MouseOver && Pad->GetMouseClick().press()) {
			value = std::clamp(((valueMax - valueMin) * (Pad->GetMS_X() - xpmin) / (xpmax - xpmin)) + valueMin, valueMin, valueMax);
		}

		DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xpmin, yp, xpmin + (xpmax - xpmin), yp + LineHeight, DarkGreen, true);
		DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xpmin, yp, xpmin + (xpmax - xpmin) * std::clamp(value - valueMin, 0, valueMax - valueMin) / (valueMax - valueMin), yp + LineHeight,
			MouseOver ? (Pad->GetMouseClick().press() ? Gray25 : White) : Green, true);
		int xp = (xmin + (xmax - xmin) / 2);
		DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
			FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xp, yp,
			White, Black, "%03d", value);
		return value;
	}
	// 0~valueMaxの選択制ボタン集
	static int UpDownBox(int xmin, int xmax, int yp, int value, int valueMax) noexcept {
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		auto* Pad = PadControl::Instance();
		auto* SE = SoundPool::Instance();
		int width = LineHeight;
		int r = LineHeight / 3;
		int xps = (xmax + xmin) / 2;
		int yps = yp + LineHeight / 2;
		for (int loop : std::views::iota(0, valueMax)) {
			int xp1 = xps + loop * width - width * (valueMax - 1) / 2;

			bool MouseOver = IntoMouse(xp1 - r, yps - r, xp1 + r, yps + r);
			DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal,
				xp1 - r, yps - r, xp1 + r, yps + r,
				MouseOver ? (Pad->GetMouseClick().press() ? Gray25 : White) : ((value == loop) ? Green : DarkGreen), true);
			if (MouseOver && Pad->GetMouseClick().trigger()) {
				SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);

				value = loop;
			}
		}
		return value;
	}
	//
	static void BoolChange(EnumSaveParam SaveParam) noexcept {
		auto* OptionParts = OptionManager::Instance();
		auto* SE = SoundPool::Instance();
		OptionParts->ChangeParamBoolean(SaveParam);
		SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
		auto* PostPassParts = PostPassEffect::Instance();
		PostPassParts->UpdateActive();
	}
	static bool BoolDraw(int xpos, int ypos, EnumSaveParam SaveParam) noexcept {
		auto* OptionParts = OptionManager::Instance();
		auto prev = OptionParts->GetParamBoolean(SaveParam);
		OptionParts->SetParamBoolean(SaveParam, CheckBox(xpos, ypos, OptionParts->GetParamBoolean(SaveParam)));
		if (prev != OptionParts->GetParamBoolean(SaveParam)) {
			auto* PostPassParts = PostPassEffect::Instance();
			PostPassParts->UpdateActive();
			return true;
		}
		return false;
	}
	static void IntChange(EnumSaveParam SaveParam, int Pow, int Min, int Max) noexcept {
		auto* OptionParts = OptionManager::Instance();
		auto* SE = SoundPool::Instance();
		OptionParts->SetParamInt(SaveParam, std::clamp(OptionParts->GetParamInt(SaveParam) + Pow, Min, Max));
		SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
		auto* PostPassParts = PostPassEffect::Instance();
		PostPassParts->UpdateActive();
	}
	static bool IntUpDownBoxDraw(int xpos, int ypos, EnumSaveParam SaveParam, int Max) noexcept {
		auto* OptionParts = OptionManager::Instance();
		auto prev = OptionParts->GetParamInt(SaveParam);
		OptionParts->SetParamInt(SaveParam, UpDownBox(xpos, xpos + (200), ypos, OptionParts->GetParamInt(SaveParam), Max));
		if (prev != OptionParts->GetParamInt(SaveParam)) {
			auto* PostPassParts = PostPassEffect::Instance();
			PostPassParts->UpdateActive();
			return true;
		}
		return false;
	}
	static void FloatChange(EnumSaveParam SaveParam, float Pow, float Min, float Max) noexcept {
		auto* OptionParts = OptionManager::Instance();
		auto* SE = SoundPool::Instance();
		OptionParts->SetParamFloat(SaveParam, std::clamp(OptionParts->GetParamFloat(SaveParam) + Pow, Min, Max));
		SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
	}
	static bool FloatUpDownBarDraw(int xpos, int ypos, EnumSaveParam SaveParam, float Min, float Max, int Scale) noexcept {
		auto* OptionParts = OptionManager::Instance();
		int prev = static_cast<int>(OptionParts->GetParamFloat(SaveParam) * Scale + 0.5f);
		int value = UpDownBar(xpos, xpos + (200), ypos, static_cast<int>(OptionParts->GetParamFloat(SaveParam) * Scale + 0.5f), (int)(Min * Scale + 0.5f), (int)(Max * Scale + 0.5f));
		OptionParts->SetParamFloat(SaveParam, static_cast<float>(value) / Scale);
		if (prev != value) {
			return true;
		}
		return false;
	}
	// --------------------------------------------------------------------------------------------------
	void OptionPopup::OptionElementsInfo::Draw(int xpos, int ypos, bool isMine) const noexcept {
		ypos += (static_cast<int>(selanim));
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
			FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xpos, ypos,
			isMine ? White : Gray50, Black, m_Name);
		m_Draw(xpos + (720 - 324), ypos, isMine);
	}
	// 
	void OptionPopup::OptionTabsInfo::Execute(int* select, bool CanPress) noexcept {
		if ((*select) < 0) {
			return;
		}
		auto* SE = SoundPool::Instance();
		auto* Pad = PadControl::Instance();
		m_Elements.at(static_cast<size_t>(*select)).GetAnyDoing();
		if (CanPress) {
			if (Pad->GetPadsInfo(Controls::PADS::MOVE_W).GetKey().trigger()) {
				--(*select);
				if ((*select) < 0) {
					(*select) = static_cast<int>(m_Elements.size()) - 1;
				}
				m_Elements.at(static_cast<size_t>(*select)).selanim = 3.f;
				SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
			}
			if (Pad->GetPadsInfo(Controls::PADS::MOVE_S).GetKey().trigger()) {
				++(*select);
				if ((*select) > static_cast<int>(m_Elements.size()) - 1) {
					(*select) = 0;
				}
				m_Elements.at(static_cast<size_t>(*select)).selanim = -3.f;
				SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
			}
			if (Pad->GetPadsInfo(Controls::PADS::MOVE_A).GetKey().repeat()) {
				m_Elements.at(static_cast<size_t>(*select)).GetLeftPush();
			}
			if (Pad->GetPadsInfo(Controls::PADS::MOVE_D).GetKey().repeat()) {
				m_Elements.at(static_cast<size_t>(*select)).GetRightPush();
			}
			if (Pad->GetPadsInfo(Controls::PADS::INTERACT).GetKey().trigger() || Pad->GetMouseClick().trigger()) {
				m_Elements.at(static_cast<size_t>(*select)).GetOKPush();
			}
		}
		for (auto& e : m_Elements) {
			Easing(&e.selanim, 0.f, 0.95f, EasingType::OutExpo);
		}
	}
	void OptionPopup::OptionTabsInfo::Draw(int xpos, int ypos, int xsize, bool isActive, int* TabSel, int* select) noexcept {
		// タブ
		{
			int xp1 = xpos + ((140) + (12)) * m_id;
			if (WindowSystem::SetMsgClickBox(xp1, ypos + (5), xp1 + (140), ypos + LineHeight * 2 - (5), LineHeight, (isActive ? Gray25 : Gray75), false, true, m_name)) {
				*TabSel = GetID();
				*select = 0;
				auto* SE = SoundPool::Instance();
				SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
			}
		}
		// 内容
		if (isActive) {
			int yp1 = ypos + LineHeight * 2;
			for (int i : std::views::iota(0, static_cast<int>(m_Elements.size()))) {
				yp1 += (LineHeight + (6));
				if (IntoMouse(xpos, yp1, xpos + xsize, yp1 + (LineHeight + (6)))) {
					*select = i;
				}
				m_Elements.at(static_cast<size_t>(i)).Draw(xpos, yp1, (*select == i));
			}
		}
	}
	void OptionPopup::OptionTabsInfo::DrawInfo(int xpos, int ypos, int select) noexcept {
		auto* LocalizeParts = LocalizePool::Instance();
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
			FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xpos, ypos,
			White, Black, LocalizeParts->Get(m_Elements.at(static_cast<size_t>(select)).GetInfoTextID()));
	}
	// 
	void OptionPopup::SoundTabsInfo::Init_Sub(void) noexcept {
		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("BGM", 1110,
			[this]() {
				FloatChange(EnumSaveParam::BGM, -0.1f, 0.f, 1.f);
				auto* SE = SoundPool::Instance();
				SE->FlipVolume();
			},
			[this]() {
				FloatChange(EnumSaveParam::BGM, 0.1f, 0.f, 1.f);
				auto* SE = SoundPool::Instance();
				SE->FlipVolume();
			},
			[]() {},
			[]() {},
			[this](int xpos, int ypos, bool) {
				if (FloatUpDownBarDraw(xpos, ypos, EnumSaveParam::BGM, 0.f, 1.f, 100)) {
					auto* SE = SoundPool::Instance();
					SE->FlipVolume();
				}
			}
		);
		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("SE", 1111,
			[this]() {
				FloatChange(EnumSaveParam::SE, -0.1f, 0.f, 1.f);
				auto* SE = SoundPool::Instance();
				SE->FlipVolume();
			},
			[this]() {
				FloatChange(EnumSaveParam::SE, 0.1f, 0.f, 1.f);
				auto* SE = SoundPool::Instance();
				SE->FlipVolume();
			},
			[]() {},
			[]() {},
			[this](int xpos, int ypos, bool) {
				if (FloatUpDownBarDraw(xpos, ypos, EnumSaveParam::SE, 0.f, 1.f, 100)) {
					auto* SE = SoundPool::Instance();
					SE->FlipVolume();
				}
			}
		);
	}
	void OptionPopup::GraphicTabsInfo::Init_Sub(void) noexcept {
		HDC hdc;
		hdc = GetDC(GetMainWindowHandle());	// デバイスコンテキストの取得
		RefreshRate = GetDeviceCaps(hdc, VREFRESH);	// リフレッシュレートの取得
		ReleaseDC(GetMainWindowHandle(), hdc);	// デバイスコンテキストの解放

		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("Graphics Preset", 1120,
			[this]() {
				IntChange(EnumSaveParam::GraphicsPreset, -1, 0, 4);
				auto* OptionParts = OptionManager::Instance();
				switch (OptionParts->GetParamInt(EnumSaveParam::GraphicsPreset)) {
				case 0:
					OptionParts->SetParamBoolean(EnumSaveParam::AA, false);
					OptionParts->SetParamBoolean(EnumSaveParam::SSAO, false);
					OptionParts->SetParamInt(EnumSaveParam::Reflection, 0);
					OptionParts->SetParamInt(EnumSaveParam::shadow, 0);
					OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 0);
					OptionParts->SetParamBoolean(EnumSaveParam::bloom, false);
					OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);

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
					OptionParts->SetParamBoolean(EnumSaveParam::SSAO, true);
					OptionParts->SetParamInt(EnumSaveParam::Reflection, 1);
					OptionParts->SetParamInt(EnumSaveParam::shadow, 2);
					OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 2);
					OptionParts->SetParamBoolean(EnumSaveParam::bloom, true);
					OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);

					break;
				case 3:
					OptionParts->SetParamBoolean(EnumSaveParam::AA, true);
					OptionParts->SetParamBoolean(EnumSaveParam::SSAO, true);
					OptionParts->SetParamInt(EnumSaveParam::Reflection, 2);
					OptionParts->SetParamInt(EnumSaveParam::shadow, 3);
					OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 3);
					OptionParts->SetParamBoolean(EnumSaveParam::bloom, true);
					OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);

					break;
				case 4:
					OptionParts->SetParamBoolean(EnumSaveParam::AA, true);
					OptionParts->SetParamBoolean(EnumSaveParam::SSAO, true);
					OptionParts->SetParamInt(EnumSaveParam::Reflection, 2);
					OptionParts->SetParamInt(EnumSaveParam::shadow, 4);
					OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 4);
					OptionParts->SetParamBoolean(EnumSaveParam::bloom, true);
					OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);

					break;
				}
				auto* PostPassParts = PostPassEffect::Instance();
				PostPassParts->UpdateActive();
			},
			[this]() {
				IntChange(EnumSaveParam::GraphicsPreset, 1, 0, 4);
				auto* OptionParts = OptionManager::Instance();
				switch (OptionParts->GetParamInt(EnumSaveParam::GraphicsPreset)) {
				case 0:
					OptionParts->SetParamBoolean(EnumSaveParam::AA, false);
					OptionParts->SetParamBoolean(EnumSaveParam::SSAO, false);
					OptionParts->SetParamInt(EnumSaveParam::Reflection, 0);
					OptionParts->SetParamInt(EnumSaveParam::shadow, 0);
					OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 0);
					OptionParts->SetParamBoolean(EnumSaveParam::bloom, false);
					OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);

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
					OptionParts->SetParamBoolean(EnumSaveParam::SSAO, true);
					OptionParts->SetParamInt(EnumSaveParam::Reflection, 1);
					OptionParts->SetParamInt(EnumSaveParam::shadow, 2);
					OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 2);
					OptionParts->SetParamBoolean(EnumSaveParam::bloom, true);
					OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);

					break;
				case 3:
					OptionParts->SetParamBoolean(EnumSaveParam::AA, true);
					OptionParts->SetParamBoolean(EnumSaveParam::SSAO, true);
					OptionParts->SetParamInt(EnumSaveParam::Reflection, 2);
					OptionParts->SetParamInt(EnumSaveParam::shadow, 3);
					OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 3);
					OptionParts->SetParamBoolean(EnumSaveParam::bloom, true);
					OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);

					break;
				case 4:
					OptionParts->SetParamBoolean(EnumSaveParam::AA, true);
					OptionParts->SetParamBoolean(EnumSaveParam::SSAO, true);
					OptionParts->SetParamInt(EnumSaveParam::Reflection, 2);
					OptionParts->SetParamInt(EnumSaveParam::shadow, 4);
					OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 4);
					OptionParts->SetParamBoolean(EnumSaveParam::bloom, true);
					OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);

					break;
				}
				auto* PostPassParts = PostPassEffect::Instance();
				PostPassParts->UpdateActive();
			},
			[]() {},
			[]() {},
			[this](int xpos, int ypos, bool) {
				if (IntUpDownBoxDraw(xpos, ypos, EnumSaveParam::GraphicsPreset, 4)) {
					auto* OptionParts = OptionManager::Instance();
					switch (OptionParts->GetParamInt(EnumSaveParam::GraphicsPreset)) {
					case 0:
						OptionParts->SetParamBoolean(EnumSaveParam::AA, false);
						OptionParts->SetParamBoolean(EnumSaveParam::SSAO, false);
						OptionParts->SetParamInt(EnumSaveParam::Reflection, 0);
						OptionParts->SetParamInt(EnumSaveParam::shadow, 0);
						OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 0);
						OptionParts->SetParamBoolean(EnumSaveParam::bloom, false);
						OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);

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
						OptionParts->SetParamBoolean(EnumSaveParam::SSAO, true);
						OptionParts->SetParamInt(EnumSaveParam::Reflection, 1);
						OptionParts->SetParamInt(EnumSaveParam::shadow, 2);
						OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 2);
						OptionParts->SetParamBoolean(EnumSaveParam::bloom, true);
						OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);

						break;
					case 3:
						OptionParts->SetParamBoolean(EnumSaveParam::AA, true);
						OptionParts->SetParamBoolean(EnumSaveParam::SSAO, true);
						OptionParts->SetParamInt(EnumSaveParam::Reflection, 2);
						OptionParts->SetParamInt(EnumSaveParam::shadow, 3);
						OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 3);
						OptionParts->SetParamBoolean(EnumSaveParam::bloom, true);
						OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);

						break;
					case 4:
						OptionParts->SetParamBoolean(EnumSaveParam::AA, true);
						OptionParts->SetParamBoolean(EnumSaveParam::SSAO, true);
						OptionParts->SetParamInt(EnumSaveParam::Reflection, 2);
						OptionParts->SetParamInt(EnumSaveParam::shadow, 4);
						OptionParts->SetParamInt(EnumSaveParam::ObjLevel, 4);
						OptionParts->SetParamBoolean(EnumSaveParam::bloom, true);
						OptionParts->SetParamBoolean(EnumSaveParam::ScreenEffect, true);

						break;
					}
					auto* PostPassParts = PostPassEffect::Instance();
					PostPassParts->UpdateActive();
				}
			}
		);
		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("Window Mode", 1121,
			[this]() {
				IntChange(EnumSaveParam::WindowMode, -1, static_cast<int>(WindowType::None), static_cast<int>(WindowType::Max));
				auto* WindowSizeParts = WindowSizeControl::Instance();
				WindowSizeParts->SetWindowOrBorderless();
			},
			[this]() {
				IntChange(EnumSaveParam::WindowMode, 1, static_cast<int>(WindowType::None), static_cast<int>(WindowType::Max));
				auto* WindowSizeParts = WindowSizeControl::Instance();
				WindowSizeParts->SetWindowOrBorderless();
			},
			[]() {},
			[]() {},
			[this](int xpos, int ypos, bool) {
				if (IntUpDownBoxDraw(xpos, ypos, EnumSaveParam::WindowMode, static_cast<int>(WindowType::Max))) {
					auto* WindowSizeParts = WindowSizeControl::Instance();
					WindowSizeParts->SetWindowOrBorderless();
				}
				auto* LocalizeParts = LocalizePool::Instance();
				auto* OptionParts = OptionManager::Instance();
				auto* DrawCtrls = WindowSystem::DrawControl::Instance();
				std::string Type;
				switch (static_cast<WindowType>(OptionParts->GetParamInt(EnumSaveParam::WindowMode))) {
				case WindowType::Window:
					Type = LocalizeParts->Get(1135);
					break;
				case WindowType::Borderless:
					Type = LocalizeParts->Get(1136);
					break;
				case WindowType::FullScreen:
					Type = LocalizeParts->Get(1137);
					break;
				case WindowType::Max:
				default:
					break;
				}
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight * 2 / 3,
					FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xpos + (250), ypos,
					White, Black, Type.c_str());
			}
		);
		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("V Sync", 1123,
			[this]() {
				BoolChange(EnumSaveParam::vsync);
				auto* OptionParts = OptionManager::Instance();
				auto* DXLib_refParts = DXLib_ref::Instance();
				if (OptionParts->GetParamBoolean(EnumSaveParam::vsync)) {
					OptionParts->SetParamInt(EnumSaveParam::FpsLimit, RefreshRate);
				}
				DXLib_refParts->SetWaitVSync();// 垂直同期
			},
			[this]() {
				BoolChange(EnumSaveParam::vsync);
				auto* OptionParts = OptionManager::Instance();
				auto* DXLib_refParts = DXLib_ref::Instance();
				if (OptionParts->GetParamBoolean(EnumSaveParam::vsync)) {
					OptionParts->SetParamInt(EnumSaveParam::FpsLimit, RefreshRate);
				}
				DXLib_refParts->SetWaitVSync();// 垂直同期
			},
			[]() {},
			[]() {},
			[this](int xpos, int ypos, bool) {
				if (BoolDraw(xpos, ypos, EnumSaveParam::vsync)) {
					auto* OptionParts = OptionManager::Instance();
					auto* DXLib_refParts = DXLib_ref::Instance();
					if (OptionParts->GetParamBoolean(EnumSaveParam::vsync)) {
						OptionParts->SetParamInt(EnumSaveParam::FpsLimit, RefreshRate);
					}
					DXLib_refParts->SetWaitVSync();// 垂直同期
				}
			}
		);
		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("FPS Limit", 1124,
			[this]() {
				auto* OptionParts = OptionManager::Instance();
				auto* SE = SoundPool::Instance();

				int value = OptionParts->GetParamInt(EnumSaveParam::FpsLimit);
				bool isHit = false;
				for (int i = 0; i < FrameLimitsNum; ++i) {
					if (FrameLimits[static_cast<size_t>(i)] == value) {
						i--;
						if (i < 0) {
							i = FrameLimitsNum - 1;
						}
						value = FrameLimits[static_cast<size_t>(i)];
						isHit = true;
						break;
					}
				}
				if (!isHit) {
					value = FrameLimits[1];
				}

				OptionParts->SetParamInt(EnumSaveParam::FpsLimit, value);
				SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
			},
			[this]() {
				auto* OptionParts = OptionManager::Instance();
				auto* SE = SoundPool::Instance();
				int value = OptionParts->GetParamInt(EnumSaveParam::FpsLimit);
				bool isHit = false;
				for (int i = 0; i < FrameLimitsNum; ++i) {
					if (FrameLimits[static_cast<size_t>(i)] == value) {
						++i;
						if (i > FrameLimitsNum - 1) {
							i = 0;
						}
						value = FrameLimits[static_cast<size_t>(i)];
						isHit = true;
						break;
					}
				}
				if (!isHit) {
					value = FrameLimits[1];
				}
				OptionParts->SetParamInt(EnumSaveParam::FpsLimit, value);
				SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
			},
			[]() {},
			[]() {},
			[this](int xpos, int ypos, bool) {
				auto* OptionParts = OptionManager::Instance();
				int ret = 0;
				// 結果から一番近いやつに指定
				int diff = 10000;
				for (int i : std::views::iota(0, FrameLimitsNum)) {
					int tmp = std::abs(FrameLimits[static_cast<size_t>(i)] - OptionParts->GetParamInt(EnumSaveParam::FpsLimit));
					if (diff > tmp) {
						diff = tmp;
						ret = i;
					}
				}
				int value = UpDownBox(xpos, xpos + (200), ypos, ret, FrameLimitsNum);
				OptionParts->SetParamInt(EnumSaveParam::FpsLimit, FrameLimits[value]);
				auto* DrawCtrls = WindowSystem::DrawControl::Instance();
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
					FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xpos + (250), ypos,
					White, Black, "%d", OptionParts->GetParamInt(EnumSaveParam::FpsLimit));
			}
		);
		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("DirectX Version", 1125,
			[]() {
				auto* OptionParts = OptionManager::Instance();
				auto* OptionWindowParts = OptionPopup::Instance();
				auto* SE = SoundPool::Instance();
				OptionParts->SetParamInt(EnumSaveParam::DirectXVer, 1 - OptionParts->GetParamInt(EnumSaveParam::DirectXVer));
				SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
				OptionWindowParts->SetRestart();
				auto* PostPassParts = PostPassEffect::Instance();
				PostPassParts->UpdateActive();
			},
			[]() {
				auto* OptionParts = OptionManager::Instance();
				auto* OptionWindowParts = OptionPopup::Instance();
				auto* SE = SoundPool::Instance();
				OptionParts->SetParamInt(EnumSaveParam::DirectXVer, 1 - OptionParts->GetParamInt(EnumSaveParam::DirectXVer));
				SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
				OptionWindowParts->SetRestart();
				auto* PostPassParts = PostPassEffect::Instance();
				PostPassParts->UpdateActive();
			},
			[]() {},
			[]() {},
			[](int xpos, int ypos, bool) {
				auto* OptionWindowParts = OptionPopup::Instance();
				auto* OptionParts = OptionManager::Instance();
				auto prev = OptionParts->GetParamInt(EnumSaveParam::DirectXVer);
				OptionParts->SetParamInt(EnumSaveParam::DirectXVer, CheckBox(xpos, ypos, (OptionParts->GetParamInt(EnumSaveParam::DirectXVer) == 1)) ? 1 : 0);
				if (prev != OptionParts->GetParamInt(EnumSaveParam::DirectXVer)) {
					OptionWindowParts->SetRestart();
					auto* PostPassParts = PostPassEffect::Instance();
					PostPassParts->UpdateActive();
				}
				auto* DrawCtrls = WindowSystem::DrawControl::Instance();
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight * 2 / 3,
					FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xpos + (100), ypos,
					White, Black, DirectXVerStr[OptionParts->GetParamInt(EnumSaveParam::DirectXVer)]);
			}
		);
		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("AntiAlias", 1126,
			[this]() { BoolChange(EnumSaveParam::AA); },
			[this]() { BoolChange(EnumSaveParam::AA); },
			[]() {},
			[]() {},
			[this](int xpos, int ypos, bool) { BoolDraw(xpos, ypos, EnumSaveParam::AA); }
		);

		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("SSAO", 1127,
			[this]() { BoolChange(EnumSaveParam::SSAO); },
			[this]() { BoolChange(EnumSaveParam::SSAO); },
			[]() {},
			[]() {},
			[this](int xpos, int ypos, bool) { BoolDraw(xpos, ypos, EnumSaveParam::SSAO); }
		);
		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("Reflection", 1128,
			[this]() { IntChange(EnumSaveParam::Reflection, -1, 0, 3); },
			[this]() { IntChange(EnumSaveParam::Reflection, 1, 0, 3); },
			[]() {},
			[]() {},
			[this](int xpos, int ypos, bool) { IntUpDownBoxDraw(xpos, ypos, EnumSaveParam::Reflection, 3); }
		);

		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("Shadow", 1129,
			[this]() { IntChange(EnumSaveParam::shadow, -1, 0, 4); },
			[this]() { IntChange(EnumSaveParam::shadow, 1, 0, 4); },
			[]() {},
			[]() {},
			[this](int xpos, int ypos, bool) { IntUpDownBoxDraw(xpos, ypos, EnumSaveParam::shadow, 4); }
		);
		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("Object Level", 1130,
			[this]() { IntChange(EnumSaveParam::ObjLevel, -1, 0, 4); },
			[this]() { IntChange(EnumSaveParam::ObjLevel, 1, 0, 4); },
			[]() {},
			[]() {},
			[this](int xpos, int ypos, bool) { IntUpDownBoxDraw(xpos, ypos, EnumSaveParam::ObjLevel, 4); }
		);
		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("Bloom Effect", 1131,
			[this]() { BoolChange(EnumSaveParam::bloom); },
			[this]() { BoolChange(EnumSaveParam::bloom); },
			[]() {},
			[]() {},
			[this](int xpos, int ypos, bool) { BoolDraw(xpos, ypos, EnumSaveParam::bloom); }
		);
		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("Screen Effect", 1132,
			[this]() { BoolChange(EnumSaveParam::ScreenEffect); },
			[this]() { BoolChange(EnumSaveParam::ScreenEffect); },
			[]() {},
			[]() {},
			[this](int xpos, int ypos, bool) { BoolDraw(xpos, ypos, EnumSaveParam::ScreenEffect); }
		);
		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("DoF", 1133,
			[this]() { BoolChange(EnumSaveParam::DoF); },
			[this]() { BoolChange(EnumSaveParam::DoF); },
			[]() {},
			[]() {},
			[this](int xpos, int ypos, bool) { BoolDraw(xpos, ypos, EnumSaveParam::DoF); }
		);
		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("MotionBlur", 1134,
			[this]() { BoolChange(EnumSaveParam::MotionBlur); },
			[this]() { BoolChange(EnumSaveParam::MotionBlur); },
			[]() {},
			[]() {},
			[this](int xpos, int ypos, bool) { BoolDraw(xpos, ypos, EnumSaveParam::MotionBlur); }
		);
		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("Fov", 1122,
			[this]() { IntChange(EnumSaveParam::fov, -5, 45, 110); },
			[this]() { IntChange(EnumSaveParam::fov, 5, 45, 110); },
			[]() {},
			[]() {},
			[](int xpos, int ypos, bool) {
				auto* OptionParts = OptionManager::Instance();
				OptionParts->SetParamInt(EnumSaveParam::fov, UpDownBar(xpos, xpos + (200), ypos, OptionParts->GetParamInt(EnumSaveParam::fov), 45, 110));
			}
		);
		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("Render Scale", 1138,
			[this]() {
				FloatChange(EnumSaveParam::DrawScale, -0.1f, 0.25f, 1.f);
				auto* OptionWindowParts = OptionPopup::Instance();
				OptionWindowParts->SetRestart();
			},
			[this]() {
				FloatChange(EnumSaveParam::DrawScale, 0.1f, 0.25f, 1.f);
				auto* OptionWindowParts = OptionPopup::Instance();
				OptionWindowParts->SetRestart();
			},
			[]() {},
			[]() {},
			[this](int xpos, int ypos, bool) {
				if (FloatUpDownBarDraw(xpos, ypos, EnumSaveParam::DrawScale, 0.25f, 1.f, 100)) {
					auto* OptionWindowParts = OptionPopup::Instance();
					OptionWindowParts->SetRestart();
				}
			}
		);
		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("LoadMovie", 1139,
			[this]() { BoolChange(EnumSaveParam::LoadMovie); },
			[this]() { BoolChange(EnumSaveParam::LoadMovie); },
			[]() {},
			[]() {},
			[this](int xpos, int ypos, bool) { BoolDraw(xpos, ypos, EnumSaveParam::LoadMovie); }
		);
	}
	void OptionPopup::ElseTabsInfo::Init_Sub(void) noexcept {
		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("Language", 1145,
			[]() {
				auto* OptionParts = OptionManager::Instance();
				auto* SE = SoundPool::Instance();
				auto* LocalizeParts = LocalizePool::Instance();
				auto* KeyGuideParts = KeyGuide::Instance();
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
				SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);

				LocalizeParts->Load();
				KeyGuideParts->SetGuideFlip();
			},
			[]() {
				auto* OptionParts = OptionManager::Instance();
				auto* SE = SoundPool::Instance();
				auto* LocalizeParts = LocalizePool::Instance();
				auto* KeyGuideParts = KeyGuide::Instance();
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
				SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);

				LocalizeParts->Load();
				KeyGuideParts->SetGuideFlip();
			},
			[]() {},
			[]() {},
			[](int xpos, int ypos, bool) {
				auto* OptionParts = OptionManager::Instance();
				auto* LocalizeParts = LocalizePool::Instance();
				auto* KeyGuideParts = KeyGuide::Instance();
				auto* DrawCtrls = WindowSystem::DrawControl::Instance();
				auto prev = OptionParts->GetParamInt(EnumSaveParam::Language);
				if (CheckBox(xpos, ypos, (OptionParts->GetParamInt(EnumSaveParam::Language) == static_cast<int>(LanguageType::Eng)))) {
					OptionParts->SetParamInt(EnumSaveParam::Language, static_cast<int>(LanguageType::Eng));
				}
				else {
					OptionParts->SetParamInt(EnumSaveParam::Language, static_cast<int>(LanguageType::Jpn));
				}
				if (prev != OptionParts->GetParamInt(EnumSaveParam::Language)) {
					LocalizeParts->Load();
					KeyGuideParts->SetGuideFlip();
				}

				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
					FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xpos + (125), ypos,
					White, Black, LanguageStr[OptionParts->GetParamInt(EnumSaveParam::Language)]);
			}
		);
		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("X sensing", 1140,
			[this]() { FloatChange(EnumSaveParam::Xsensing, -0.01f, 0.01f, 1.f); },
			[this]() { FloatChange(EnumSaveParam::Xsensing, 0.01f, 0.01f, 1.f); },
			[]() {},
			[]() {},
			[this](int xpos, int ypos, bool) { FloatUpDownBarDraw(xpos, ypos, EnumSaveParam::Xsensing, 0.01f, 1.f, 100); }
		);
		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("Y sensing", 1141,
			[this]() { FloatChange(EnumSaveParam::Ysensing, -0.01f, 0.01f, 1.f); },
			[this]() { FloatChange(EnumSaveParam::Ysensing, 0.01f, 0.01f, 1.f); },
			[]() {},
			[]() {},
			[this](int xpos, int ypos, bool) { FloatUpDownBarDraw(xpos, ypos, EnumSaveParam::Ysensing, 0.01f, 1.f, 100); }
		);
		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("HeadBobbing", 1142,
			[this]() { BoolChange(EnumSaveParam::HeadBobbing); },
			[this]() { BoolChange(EnumSaveParam::HeadBobbing); },
			[]() {},
			[]() {},
			[this](int xpos, int ypos, bool) { BoolDraw(xpos, ypos, EnumSaveParam::HeadBobbing); }
		);
		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("GamePadType", 1144,
			[]() {
				auto* OptionParts = OptionManager::Instance();
				auto* SE = SoundPool::Instance();
				switch ((Controls::ControlType)OptionParts->GetParamInt(EnumSaveParam::ControlType)) {
				case Controls::ControlType::PS4:
					OptionParts->SetParamInt(EnumSaveParam::ControlType, static_cast<int>(Controls::ControlType::XBox));
					break;
				case Controls::ControlType::XBox:
					OptionParts->SetParamInt(EnumSaveParam::ControlType, static_cast<int>(Controls::ControlType::PS4));
					break;
				case Controls::ControlType::PC:
				default:
					break;
				}
				SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
			},
			[]() {
				auto* OptionParts = OptionManager::Instance();
				auto* SE = SoundPool::Instance();
				switch ((Controls::ControlType)OptionParts->GetParamInt(EnumSaveParam::ControlType)) {
				case Controls::ControlType::PS4:
					OptionParts->SetParamInt(EnumSaveParam::ControlType, static_cast<int>(Controls::ControlType::XBox));
					break;
				case Controls::ControlType::XBox:
					OptionParts->SetParamInt(EnumSaveParam::ControlType, static_cast<int>(Controls::ControlType::PS4));
					break;
				case Controls::ControlType::PC:
				default:
					break;
				}
				SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
			},
			[]() {},
			[]() {},
			[](int xpos, int ypos, bool) {
				auto* OptionParts = OptionManager::Instance();
				auto* DrawCtrls = WindowSystem::DrawControl::Instance();
				if (CheckBox(xpos, ypos, (OptionParts->GetParamInt(EnumSaveParam::ControlType) == static_cast<int>(Controls::ControlType::PS4)))) {
					OptionParts->SetParamInt(EnumSaveParam::ControlType, static_cast<int>(Controls::ControlType::PS4));
				}
				else {
					OptionParts->SetParamInt(EnumSaveParam::ControlType, static_cast<int>(Controls::ControlType::XBox));
				}
				ypos -= LineHeight * 1 / 6;
				if (OptionParts->GetParamInt(EnumSaveParam::ControlType) == static_cast<int>(Controls::ControlType::XBox)) {
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight * 2 / 3,
						FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE, xpos + (125), ypos + LineHeight / 3,
						White, Black, "XInput");
				}
				else {
					DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight * 2 / 3,
						FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE, xpos + (125), ypos + LineHeight / 3,
						White, Black, "DirectInput");
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
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight * 2 / 3,
							FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE, xpos + (125), ypos + LineHeight * 3 / 3,
							White, Black, "推奨:DirectInput");
						break;
					case DX_PADTYPE_XBOX_360:
					case DX_PADTYPE_XBOX_ONE:
						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight * 2 / 3,
							FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE, xpos + (125), ypos + LineHeight * 3 / 3,
							White, Black, "推奨:XInput");
						break;
					default:
						break;
					}
				}
			}
		);
	}
	void OptionPopup::ControlTabsInfo::KeyDraw(int xpos, int ypos, bool isMine, Controls::PADS Sel) noexcept {
		auto* Pad = PadControl::Instance();
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		auto* KeyGuideParts = KeyGuide::Instance();
		if (KeyGuide::GetPADStoOffset(Sel) != InvalidID) {
			int Size = KeyGuideParts->GetDrawSize(KeyGuide::GetPADStoOffset(Sel));
			KeyGuideParts->DrawButton(xpos + 50 - Size, ypos + LineHeight / 2 - 24 / 2, KeyGuide::GetPADStoOffset(Sel));
		}
		else {
			DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic,
				LineHeight, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP, xpos + 50, ypos, isMine ? Red : Red25, Black, "None");
		}
		DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic,
			LineHeight, FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xpos + 75, ypos, isMine ? White : Gray25, Black, "->");
		if (Pad->GetPadsInfo(Sel).IsEnableSelectReserve()) {
			KeyGuideParts->DrawButton(xpos + 100, ypos + LineHeight / 2 - 24 / 2, Controls::GetIDtoOffset(Pad->GetPadsInfo(Sel).GetReserve(), Pad->GetControlType()));
		}
		else {
			DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic,
				LineHeight, FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xpos + 100, ypos, isMine ? Red : Red25, Black, "None");
		}
	}
	void OptionPopup::ControlTabsInfo::Init_Sub(void) noexcept {
		auto* Pad = PadControl::Instance();
		auto* LocalizeParts = LocalizePool::Instance();
		const int KeyInfo = 1151;

		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("Reset", 1150,
			[]() {},
			[]() {},
			[]() {},
			[]() {},
			[](int xpos, int ypos, bool isMine) {
				auto* Pad = PadControl::Instance();
				if (isMine && Pad->GetMouseClick().trigger()) {
					Pad->ResetAssign();
				}
				auto* DrawCtrls = WindowSystem::DrawControl::Instance();
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
					FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xpos + (100), ypos,
					isMine ? White : Gray25, Black, "LMB Click");
			}
		);
		if (Pad->GetPadsInfo(Controls::PADS::MOVE_W).IsUse()) {
			this->m_Elements.emplace_back();
			this->m_Elements.back().Init(LocalizeParts->Get(1153), KeyInfo,
				[]() {},
				[]() {},
				[]() {},
				[]() {
					auto* Pad = PadControl::Instance();
					if (Pad->ChangeConfig(Controls::PADS::MOVE_W)) {
						auto* SE = SoundPool::Instance();
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				},
				[this](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, Controls::PADS::MOVE_W); }
			);
		}
		if (Pad->GetPadsInfo(Controls::PADS::MOVE_S).IsUse()) {
			this->m_Elements.emplace_back();
			this->m_Elements.back().Init(LocalizeParts->Get(1154), KeyInfo,
				[]() {},
				[]() {},
				[]() {},
				[]() {
					auto* Pad = PadControl::Instance();
					if (Pad->ChangeConfig(Controls::PADS::MOVE_S)) {
						auto* SE = SoundPool::Instance();
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				},
				[this](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, Controls::PADS::MOVE_S); }
			);
		}
		if (Pad->GetPadsInfo(Controls::PADS::MOVE_A).IsUse()) {
			this->m_Elements.emplace_back();
			this->m_Elements.back().Init(LocalizeParts->Get(1155), KeyInfo,
				[]() {},
				[]() {},
				[]() {},
				[]() {
					auto* Pad = PadControl::Instance();
					if (Pad->ChangeConfig(Controls::PADS::MOVE_A)) {
						auto* SE = SoundPool::Instance();
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				},
				[this](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, Controls::PADS::MOVE_A); }
			);
		}
		if (Pad->GetPadsInfo(Controls::PADS::MOVE_D).IsUse()) {
			this->m_Elements.emplace_back();
			this->m_Elements.back().Init(LocalizeParts->Get(1156), KeyInfo,
				[]() {},
				[]() {},
				[]() {},
				[]() {
					auto* Pad = PadControl::Instance();
					if (Pad->ChangeConfig(Controls::PADS::MOVE_D)) {
						auto* SE = SoundPool::Instance();
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				},
				[this](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, Controls::PADS::MOVE_D); }
			);
		}

		if (Pad->GetPadsInfo(Controls::PADS::LEAN_L).IsUse()) {
			this->m_Elements.emplace_back();
			this->m_Elements.back().Init(LocalizeParts->Get(1157), KeyInfo,
				[]() {},
				[]() {},
				[]() {},
				[]() {
					auto* Pad = PadControl::Instance();
					if (Pad->ChangeConfig(Controls::PADS::LEAN_L)) {
						auto* SE = SoundPool::Instance();
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				},
				[this](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, Controls::PADS::LEAN_L); }
			);
		}
		if (Pad->GetPadsInfo(Controls::PADS::LEAN_R).IsUse()) {
			this->m_Elements.emplace_back();
			this->m_Elements.back().Init(LocalizeParts->Get(1158), KeyInfo,
				[]() {},
				[]() {},
				[]() {},
				[]() {
					auto* Pad = PadControl::Instance();
					if (Pad->ChangeConfig(Controls::PADS::LEAN_R)) {
						auto* SE = SoundPool::Instance();
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				},
				[this](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, Controls::PADS::LEAN_R); }
			);
		}
		if (Pad->GetPadsInfo(Controls::PADS::RELOAD).IsUse()) {
			this->m_Elements.emplace_back();
			this->m_Elements.back().Init(LocalizeParts->Get(1159), KeyInfo,
				[]() {},
				[]() {},
				[]() {},
				[]() {
					auto* Pad = PadControl::Instance();
					if (Pad->ChangeConfig(Controls::PADS::RELOAD)) {
						auto* SE = SoundPool::Instance();
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				},
				[this](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, Controls::PADS::RELOAD); }
			);
		}
		if (Pad->GetPadsInfo(Controls::PADS::INTERACT).IsUse()) {
			this->m_Elements.emplace_back();
			this->m_Elements.back().Init(LocalizeParts->Get(1160), KeyInfo,
				[]() {},
				[]() {},
				[]() {},
				[]() {
					auto* Pad = PadControl::Instance();
					if (Pad->ChangeConfig(Controls::PADS::INTERACT)) {
						auto* SE = SoundPool::Instance();
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				},
				[this](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, Controls::PADS::INTERACT); }
			);
		}
		if (Pad->GetPadsInfo(Controls::PADS::THROW).IsUse()) {
			this->m_Elements.emplace_back();
			this->m_Elements.back().Init(LocalizeParts->Get(1161), KeyInfo,
				[]() {},
				[]() {},
				[]() {},
				[]() {
					auto* Pad = PadControl::Instance();
					if (Pad->ChangeConfig(Controls::PADS::THROW)) {
						auto* SE = SoundPool::Instance();
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				},
				[this](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, Controls::PADS::THROW); }
			);
		}
		if (Pad->GetPadsInfo(Controls::PADS::MELEE).IsUse()) {
			this->m_Elements.emplace_back();
			this->m_Elements.back().Init(LocalizeParts->Get(1162), KeyInfo,
				[]() {},
				[]() {},
				[]() {},
				[]() {
					auto* Pad = PadControl::Instance();
					if (Pad->ChangeConfig(Controls::PADS::MELEE)) {
						auto* SE = SoundPool::Instance();
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				},
				[this](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, Controls::PADS::MELEE); }
			);
		}
		if (Pad->GetPadsInfo(Controls::PADS::JUMP).IsUse()) {
			this->m_Elements.emplace_back();
			this->m_Elements.back().Init(LocalizeParts->Get(1163), KeyInfo,
				[]() {},
				[]() {},
				[]() {},
				[]() {
					auto* Pad = PadControl::Instance();
					if (Pad->ChangeConfig(Controls::PADS::JUMP)) {
						auto* SE = SoundPool::Instance();
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				},
				[this](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, Controls::PADS::JUMP); }
			);
		}
		if (Pad->GetPadsInfo(Controls::PADS::RUN).IsUse()) {
			this->m_Elements.emplace_back();
			this->m_Elements.back().Init(LocalizeParts->Get(1164), KeyInfo,
				[]() {},
				[]() {},
				[]() {},
				[]() {
					auto* Pad = PadControl::Instance();
					if (Pad->ChangeConfig(Controls::PADS::RUN)) {
						auto* SE = SoundPool::Instance();
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				},
				[this](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, Controls::PADS::RUN); }
			);
		}
		if (Pad->GetPadsInfo(Controls::PADS::WALK).IsUse()) {
			this->m_Elements.emplace_back();
			this->m_Elements.back().Init(LocalizeParts->Get(1165), KeyInfo,
				[]() {},
				[]() {},
				[]() {},
				[]() {
					auto* Pad = PadControl::Instance();
					if (Pad->ChangeConfig(Controls::PADS::WALK)) {
						auto* SE = SoundPool::Instance();
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				},
				[this](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, Controls::PADS::WALK); }
			);
		}
		if (Pad->GetPadsInfo(Controls::PADS::SHOT).IsUse()) {
			this->m_Elements.emplace_back();
			this->m_Elements.back().Init(LocalizeParts->Get(1166), KeyInfo,
				[]() {},
				[]() {},
				[]() {},
				[]() {
					auto* Pad = PadControl::Instance();
					if (Pad->ChangeConfig(Controls::PADS::SHOT)) {
						auto* SE = SoundPool::Instance();
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				},
				[this](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, Controls::PADS::SHOT); }
			);
		}
		if (Pad->GetPadsInfo(Controls::PADS::AIM).IsUse()) {
			this->m_Elements.emplace_back();
			this->m_Elements.back().Init(LocalizeParts->Get(1167), KeyInfo,
				[]() {},
				[]() {},
				[]() {},
				[]() {
					auto* Pad = PadControl::Instance();
					if (Pad->ChangeConfig(Controls::PADS::AIM)) {
						auto* SE = SoundPool::Instance();
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				},
				[this](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, Controls::PADS::AIM); }
			);
		}
		if (Pad->GetPadsInfo(Controls::PADS::ULT).IsUse()) {
			this->m_Elements.emplace_back();
			this->m_Elements.back().Init(LocalizeParts->Get(1168), KeyInfo,
				[]() {},
				[]() {},
				[]() {},
				[]() {
					auto* Pad = PadControl::Instance();
					if (Pad->ChangeConfig(Controls::PADS::ULT)) {
						auto* SE = SoundPool::Instance();
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				},
				[this](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, Controls::PADS::ULT); }
			);
		}
		if (Pad->GetPadsInfo(Controls::PADS::SQUAT).IsUse()) {
			this->m_Elements.emplace_back();
			this->m_Elements.back().Init(LocalizeParts->Get(1169), KeyInfo,
				[]() {},
				[]() {},
				[]() {},
				[]() {
					auto* Pad = PadControl::Instance();
					if (Pad->ChangeConfig(Controls::PADS::SQUAT)) {
						auto* SE = SoundPool::Instance();
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				},
				[this](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, Controls::PADS::SQUAT); }
			);
		}
		if (Pad->GetPadsInfo(Controls::PADS::PRONE).IsUse()) {
			this->m_Elements.emplace_back();
			this->m_Elements.back().Init(LocalizeParts->Get(1170), KeyInfo,
				[]() {},
				[]() {},
				[]() {},
				[]() {
					auto* Pad = PadControl::Instance();
					if (Pad->ChangeConfig(Controls::PADS::PRONE)) {
						auto* SE = SoundPool::Instance();
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				},
				[this](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, Controls::PADS::PRONE); }
			);
		}
		if (Pad->GetPadsInfo(Controls::PADS::CHECK).IsUse()) {
			this->m_Elements.emplace_back();
			this->m_Elements.back().Init(LocalizeParts->Get(1171), KeyInfo,
				[]() {},
				[]() {},
				[]() {},
				[]() {
					auto* Pad = PadControl::Instance();
					if (Pad->ChangeConfig(Controls::PADS::CHECK)) {
						auto* SE = SoundPool::Instance();
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				},
				[this](int xpos, int ypos, bool isMine) { KeyDraw(xpos, ypos, isMine, Controls::PADS::CHECK); }
			);
		}

		this->m_Elements.emplace_back();
		this->m_Elements.back().Init("Save", 1152,
			[]() {},
			[]() {},
			[]() {},
			[]() {},
			[](int xpos, int ypos, bool isMine) {
				auto* Pad = PadControl::Instance();
				if (isMine && Pad->GetMouseClick().trigger()) {
					Pad->FlipAssign();
					Pad->Save();
				}
				auto* DrawCtrls = WindowSystem::DrawControl::Instance();
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
					FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::TOP, xpos + (100), ypos,
					isMine ? White : Gray25, Black, "LMB Click");
			}
		);
	}
	// 
	void OptionPopup::Init(void) noexcept {
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
	void OptionPopup::Update(void) noexcept {
		if (m_ActiveSwitch) {
			m_ActiveSwitch = false;
			m_Active = true;
			auto* PopUpParts = PopUp::Instance();
			PopUpParts->Add("Option", 720, 720,
				[this](int xmin, int ymin, int xmax, int ymax, bool EndSwitch) {
					auto* OptionParts = OptionManager::Instance();
					auto* Pad = PadControl::Instance();
					auto* KeyGuideParts = KeyGuide::Instance();
					auto* SE = SoundPool::Instance();
					//
					for (auto& t : m_Tabs) {
						t->Draw(xmin + 24, ymin, (xmax - 24) - (xmin + 24), m_tabsel == t->GetID(), &m_tabsel, &m_select);
					}
					// ガイド
					m_Tabs.at(static_cast<size_t>(m_tabsel))->DrawInfo(xmin + 24, ymax - LineHeight * 3 / 2, m_select);
					// 
					if (Pad->GetPadsInfo(Controls::PADS::LEAN_L).GetKey().trigger() && (m_tabsel != 3)) {
						--m_tabsel;
						if (m_tabsel < 0) {
							m_tabsel = static_cast<int>(m_Tabs.size()) - 1;
						}
						m_select = 0;
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
					}
					if (Pad->GetPadsInfo(Controls::PADS::LEAN_R).GetKey().trigger() && (m_tabsel != 3)) {
						++m_tabsel;
						if (m_tabsel > static_cast<int>(m_Tabs.size()) - 1) {
							m_tabsel = 0;
						}
						m_select = 0;
						SE->Get(SoundType::SE, static_cast<int>(SoundSelectCommon::UI_Select))->Play(DX_PLAYTYPE_BACK, TRUE);
					}

					m_Tabs.at(static_cast<size_t>(m_tabsel))->Execute(&m_select, (m_tabsel != 3));
					// 
					if (EndSwitch) {
						KeyGuideParts->SetGuideFlip();
						OptionParts->Save();
						Pad->Save();
						m_tabsel = 0;
						m_select = 0;
					}
				},
				[this]() {m_Active = false; },
				[]() {
					auto* KeyGuideParts = KeyGuide::Instance();
					auto* LocalizeParts = LocalizePool::Instance();

					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::INTERACT), LocalizeParts->Get(9992));

					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::LEAN_L), "");
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::LEAN_R), LocalizeParts->Get(9994));
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_W), "");
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_A), "");
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_S), "");
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_D), "");
					KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(Controls::PADS::MOVE_STICK), LocalizeParts->Get(9993));
				},
				true
			);
		}
	}
}
