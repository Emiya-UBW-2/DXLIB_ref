#include "DXLib_ref_12_Draw2DSystem.hpp"
//#include "DXLib_ref.h"

namespace DXLibRef {
	const SideLog* SingletonBase<SideLog>::m_Singleton = nullptr;
	const PopUp* SingletonBase<PopUp>::m_Singleton = nullptr;
	const WindowSystem::DrawControl* SingletonBase<WindowSystem::DrawControl>::m_Singleton = nullptr;
	// 
	namespace WindowSystem {
		void DrawData::Output() const noexcept {
			auto* WindowSizeParts = WindowSizeControl::Instance();
			Rect2D Widow; Widow.Set(0, 0, WindowSizeParts->GetUIXMax(), WindowSizeParts->GetUIYMax());

			switch (m_type) {
			case DrawType::Alpha:
				if (this->m_intParam.at(0) < 255) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, this->m_intParam.at(0));
				}
				else {
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, this->m_intParam.at(0));
				}
				break;
			case DrawType::Add:
				SetDrawBlendMode(DX_BLENDMODE_ADD, this->m_intParam.at(0));
				break;
			case DrawType::Bright:
				SetDrawBright(this->m_intParam.at(0), this->m_intParam.at(1), this->m_intParam.at(2));
				break;
			case DrawType::Box:
			{
				Rect2D One; One.Set(std::min(this->m_intParam.at(0), this->m_intParam.at(2)), std::min(this->m_intParam.at(1), this->m_intParam.at(3)),
					std::abs(this->m_intParam.at(0) - this->m_intParam.at(2)), std::abs(this->m_intParam.at(1) - this->m_intParam.at(3)));
				if (Widow.IsHit(One)) {
					DxLib::DrawBox(m_intParam.at(0), this->m_intParam.at(1), this->m_intParam.at(2), this->m_intParam.at(3), this->m_UintParam.at(0), (m_boolParam.at(0)) ? TRUE : FALSE);
				}
			}
			break;
			case DrawType::Quadrangle:
			{
				DxLib::DrawQuadrangle(
					this->m_intParam.at(0), this->m_intParam.at(1),
					this->m_intParam.at(2), this->m_intParam.at(3),
					this->m_intParam.at(4), this->m_intParam.at(5),
					this->m_intParam.at(6), this->m_intParam.at(7),
					this->m_UintParam.at(0), (m_boolParam.at(0)) ? TRUE : FALSE);
			}
			break;
			case DrawType::Circle:
			{
				Rect2D One; One.Set(this->m_intParam.at(0) - this->m_intParam.at(2) / 2, this->m_intParam.at(1) - this->m_intParam.at(2) / 2,
					this->m_intParam.at(2) * 2, this->m_intParam.at(2) * 2);
				if (Widow.IsHit(One)) {
					DxLib::DrawCircle(m_intParam.at(0), this->m_intParam.at(1), this->m_intParam.at(2), this->m_UintParam.at(0), (m_boolParam.at(0)) ? TRUE : FALSE, this->m_intParam.at(3));
				}
			}
			break;
			case DrawType::Line:
			{
				Rect2D One; One.Set(std::min(this->m_intParam.at(0), this->m_intParam.at(2)), std::min(this->m_intParam.at(1), this->m_intParam.at(3)),
					std::abs(this->m_intParam.at(0) - this->m_intParam.at(2)), std::abs(this->m_intParam.at(1) - this->m_intParam.at(3)));
				if (Widow.IsHit(One)) {
					DxLib::DrawLine(m_intParam.at(0), this->m_intParam.at(1), this->m_intParam.at(2), this->m_intParam.at(3), this->m_UintParam.at(0), this->m_intParam.at(4));
				}
			}
			break;
			case DrawType::String:
				FontPool::Instance()->Get((FontPool::FontType)m_intParam.at(0), this->m_intParam.at(1), 3)->DrawString(
					InvalidID,
					(FontHandle::FontXCenter)m_intParam.at(2), (FontHandle::FontYCenter)m_intParam.at(3),
					m_intParam.at(4), this->m_intParam.at(5),
					m_UintParam.at(0),
					m_UintParam.at(1),
					m_string.c_str()
				);
				break;
			case DrawType::StringAutoFit:
				FontPool::Instance()->Get((FontPool::FontType)m_intParam.at(0), this->m_intParam.at(1), 3)->DrawStringAutoFit(
					m_intParam.at(2), m_intParam.at(3),
					m_intParam.at(4), this->m_intParam.at(5),
					m_UintParam.at(0),
					m_UintParam.at(1),
					m_string.c_str()
				);
				break;
			case DrawType::RotaGraph:
				if (m_GraphHandleParam.at(0)) {
					if (m_floatParam.at(0) < 0.9f && 1.1f < this->m_floatParam.at(0)) {
						auto prev = GetDrawMode();
						SetDrawMode(DX_DRAWMODE_BILINEAR);
						m_GraphHandleParam.at(0)->DrawRotaGraph(this->m_intParam.at(0), this->m_intParam.at(1), this->m_floatParam.at(0), this->m_floatParam.at(1), this->m_boolParam.at(0));
						SetDrawMode(prev);
					}
					else {
						m_GraphHandleParam.at(0)->DrawRotaGraph(this->m_intParam.at(0), this->m_intParam.at(1), this->m_floatParam.at(0), this->m_floatParam.at(1), this->m_boolParam.at(0));
					}
				}
				break;
			case DrawType::ExtendGraph:
				if (m_GraphHandleParam.at(0)) {
					auto prev = GetDrawMode();
					SetDrawMode(DX_DRAWMODE_BILINEAR);
					m_GraphHandleParam.at(0)->DrawExtendGraph(this->m_intParam.at(0), this->m_intParam.at(1), this->m_intParam.at(2), this->m_intParam.at(3), this->m_boolParam.at(0));
					SetDrawMode(prev);
				}
				break;
			case DrawType::CircleGauge:
				if (m_GraphHandleParam.at(0)) {
					DrawCircleGauge(this->m_intParam.at(0), this->m_intParam.at(1),
						(double)this->m_floatParam.at(0),
						m_GraphHandleParam.at(0)->get(),
						(double)this->m_floatParam.at(1),
						(double)this->m_floatParam.at(2));
				}
				break;
			default:
				break;
			}
		}
		DrawControl::DrawControl(void) noexcept {
			this->m_DrawDatas.resize(static_cast<size_t>(DrawLayer::Max));
			this->m_PrevDrawDatas.resize(static_cast<size_t>(DrawLayer::Max));

			auto* WindowSizeParts = WindowSizeControl::Instance();
			m_BufferScreen.Make(WindowSizeParts->GetSizeXMax(), WindowSizeParts->GetSizeYMax(), true);
		}
		bool DrawControl::IsDrawOnWindow(int x1, int y1, int x2, int y2) noexcept {
			auto* WindowSizeParts = WindowSizeControl::Instance();
			return HitRectangleToRectangle(0, 0, WindowSizeParts->GetUIXMax(), WindowSizeParts->GetUIYMax(), std::min(x1, x2), std::min(y1, y2), std::max(x1, x2), std::max(y1, y2));
		}
		// 箱
		void SetBox(int xp1, int yp1, int xp2, int yp2, unsigned int colorSet) noexcept {
			DrawControl::Instance()->SetDrawBox(DrawLayer::Normal, xp1, yp1, xp2, yp2, colorSet, true);
		}
		// マウスでクリックできるボタン
		bool SetClickBox(int xp1, int yp1, int xp2, int yp2, unsigned int colorSet, bool IsRepeat, bool IsActive) noexcept {
			auto* Pad = PadControl::Instance();
			bool MouseOver = IsActive && IntoMouse(xp1, yp1, xp2, yp2);
			SetBox(xp1, yp1, xp2, yp2, MouseOver ? (Pad->GetMouseClick().press() ? Gray25 : White) : colorSet);
			return (MouseOver && (IsRepeat ? Pad->GetMouseClick().repeat() : Pad->GetMouseClick().trigger()));
		};
		// 文字
		bool GetMsgPosOn(int* xp1, int* yp1, int ySize, int xSize, FontHandle::FontXCenter FontX) noexcept {
			auto* WindowSizeParts = WindowSizeControl::Instance();
			switch (FontX) {
			case FontHandle::FontXCenter::LEFT:
				*xp1 = *xp1 + WindowSizeParts->GetUIY(6);
				return HitRectangleToRectangle(
					(*xp1), (*yp1 - ySize / 2), (*xp1 + xSize), (*yp1 + ySize / 2),
					0, 0, WindowSizeParts->GetUIXMax(), WindowSizeParts->GetUIYMax());
			case FontHandle::FontXCenter::MIDDLE:
				return HitRectangleToRectangle(
					(*xp1 - xSize / 2), (*yp1 - ySize / 2), (*xp1 + xSize / 2), (*yp1 + ySize),
					0, 0, WindowSizeParts->GetUIXMax(), WindowSizeParts->GetUIYMax());
			case FontHandle::FontXCenter::RIGHT:
				*xp1 = *xp1 - WindowSizeParts->GetUIY(6);
				return HitRectangleToRectangle(
					(*xp1 - xSize), (*yp1 - ySize / 2), (*xp1), (*yp1 + ySize / 2),
					0, 0, WindowSizeParts->GetUIXMax(), WindowSizeParts->GetUIYMax());
			default:
				return false;
			}
		};
		// オンオフできるボタン
		bool CheckBox(int xp1, int yp1, bool switchturn) noexcept {
			auto* WindowSizeParts = WindowSizeControl::Instance();
			int xp3 = xp1 + EdgeSize;
			int yp3 = yp1 + EdgeSize;
			int xp4 = xp1 + LineHeight * 2 - EdgeSize;
			int yp4 = yp1 + LineHeight - EdgeSize;

			auto* Pad = PadControl::Instance();
			bool MouseOver = IntoMouse(xp3, yp3, xp4, yp4);
			if (MouseOver && Pad->GetMouseClick().trigger()) {
				switchturn ^= 1;
				auto* SE = SoundPool::Instance();
				SE->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}
			unsigned int color = Gray25;
			int Edge = WindowSizeParts->GetUIY(5);
			SetBox(xp3 + Edge, yp3 + Edge, xp4 - Edge, yp4 - Edge, Black);
			xp4 = xp1 + LineHeight * (switchturn ? 1 : 0) - EdgeSize;
			SetBox(xp3 + Edge, yp3 + Edge, xp4 + Edge, yp4 - Edge, Gray50);
			xp3 = xp1 + LineHeight * (switchturn ? 1 : 0) + EdgeSize;
			xp4 = xp1 + LineHeight * (switchturn ? 2 : 1) - EdgeSize;
			SetBox(xp3, yp3, xp4, yp4, color);
			return switchturn;
		}
		// スライダー
		int UpDownBar(int xmin, int xmax, int yp, int value, int valueMin, int valueMax) noexcept {
			int xpmin = xmin + 1;
			int xpmax = xmax - 1;

			auto* Pad = PadControl::Instance();
			bool MouseOver = IntoMouse(xpmin - 5, yp, xpmin + (xpmax - xpmin) + 5, yp + LineHeight);
			if (MouseOver && Pad->GetMouseClick().trigger()) {
				auto* SE = SoundPool::Instance();
				SE->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}
			if (MouseOver && Pad->GetMouseClick().press()) {
				value = std::clamp(((valueMax - valueMin) * (Pad->GetMS_X() - xpmin) / (xpmax - xpmin)) + valueMin, valueMin, valueMax);
			}

			SetBox(xpmin, yp, xpmin + (xpmax - xpmin), yp + LineHeight, DarkGreen);
			SetBox(xpmin, yp, xpmin + (xpmax - xpmin) * std::clamp(value - valueMin, 0, valueMax - valueMin) / (valueMax - valueMin), yp + LineHeight,
				MouseOver ? (Pad->GetMouseClick().press() ? Gray25 : White) : Green);
			int xp = (xmin + (xmax - xmin) / 2);
			SetMsg(xp, yp + LineHeight / 2, LineHeight, FontHandle::FontXCenter::MIDDLE, White, Black, "%03d", value);
			return value;
		}
		// 0~valueMaxの選択制ボタン集
		int UpDownBox(int xmin, int xmax, int yp, int value, int valueMax) noexcept {
			int width = LineHeight;
			int r = LineHeight / 3;
			int xps = (xmax + xmin) / 2;
			int yps = yp + LineHeight / 2;
			for (int loop : std::views::iota(0, valueMax)) {
				int xp1 = xps + loop * width - width * (valueMax - 1) / 2;
				if (SetClickBox(xp1 - r, yps - r, xp1 + r, yps + r, (value == loop) ? Green : DarkGreen, false, true)) {
					auto* SE = SoundPool::Instance();
					SE->Get(static_cast<int>(SoundEnumCommon::UI_Select)).Play(0, DX_PLAYTYPE_BACK, TRUE);

					value = loop;
				}
			}
			return value;
		}
	};
	// 
	void SideLog::SideLogData::UpdateActive(void) noexcept {
		auto* DXLib_refParts = DXLib_ref::Instance();
		if (m_Time > 0.f) {
			m_Time -= DXLib_refParts->GetDeltaTime();
		}
		else {
			m_Time = -1.f;
		}
		Easing(&m_Flip_Y, m_Flip, 0.9f, EasingType::OutExpo);
	}
	void SideLog::Draw(void) noexcept {
		auto* WindowSizeParts = WindowSizeControl::Instance();
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		int xp1, yp1;
		xp1 = WindowSizeParts->GetUIY(64);
		yp1 = WindowSizeParts->GetUIY(256);

		{
			int yp = yp1 - WindowSizeParts->GetUIY(static_cast<int>(24.f * 0.f));
			if (SelYadd > 0.f) {
				float per = std::clamp(SelYadd / 5.f, 0.f, 1.f);
				float per2 = 1.f - std::clamp(SelYadd / 10.f, 0.f, 1.f);
				DrawCtrls->SetAdd(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * per), 0, 255));
				DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal,
					&this->m_SelectBackImage,
					xp1 + WindowSizeParts->GetUIY(200 / 2) - (int)((float)(WindowSizeParts->GetUIY(150)) * per2), yp + LineHeight / 2 - (int)((float)(LineHeight * 4) * per2),
					xp1 + WindowSizeParts->GetUIY(200 / 2) + (int)((float)(WindowSizeParts->GetUIY(150)) * per2), yp + LineHeight / 2 + (int)((float)(LineHeight * 4) * per2),
					true);
			}
		}

		for (auto& d : data) {
			if (d.ActivePer() > 0.f) {
				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * d.ActivePer()), 0, 255));
				int yp = yp1 - WindowSizeParts->GetUIY(static_cast<int>(24.f * d.GetFlip()));
				WindowSystem::SetBox(
					xp1 - WindowSizeParts->GetUIY(6), yp + LineHeight,
					xp1 - WindowSizeParts->GetUIY(6) + static_cast<int>(static_cast<float>(std::max(WindowSystem::GetMsgLen(LineHeight, d.GetMsg()), WindowSizeParts->GetUIY(200))) * d.ActivePer()), yp + LineHeight + WindowSizeParts->GetUIY(5),
					Black);
				WindowSystem::SetMsg(static_cast<int>(xp1), static_cast<int>(yp) + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, d.GetMsgColor(), Black, d.GetMsg());
			}
		}
		DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
	}
	// 
	void PopUp::PopUpDrawClass::Start(void) noexcept {
		auto* SE = SoundPool::Instance();
		auto* Pad = PadControl::Instance();

		Pad->SetGuideUpdate();
		Pad->ChangeGuide(
			[this]() {
				auto* Pad = PadControl::Instance();
				auto* LocalizeParts = LocalizePool::Instance();
				Pad->AddGuide(PADS::RELOAD, LocalizeParts->Get(9991));
				if (m_GuideDoing) {
					m_GuideDoing();
				}
			}
		);
		SE->Get(static_cast<int>(SoundEnumCommon::UI_OK)).Play(0, DX_PLAYTYPE_BACK, TRUE);

		m_Active = true;
		m_ActiveSwitch = true;
	}
	void PopUp::PopUpDrawClass::End(void) noexcept {
		auto* SE = SoundPool::Instance();
		auto* Pad = PadControl::Instance();

		SE->Get(static_cast<int>(SoundEnumCommon::UI_CANCEL)).Play(0, DX_PLAYTYPE_BACK, TRUE);
		m_Active = false;
		m_ActiveSwitch = true;
		Pad->SetGuideUpdate();
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
			if (Pad->GetKey(PADS::RELOAD).trigger()) {
				End();
			}
		}
	}
	void PopUp::PopUpDrawClass::Draw(int xcenter, int ycenter) noexcept {
		if (m_ActivePer < (1.f / 255.f)) { return; }

		auto* WindowSizeParts = WindowSizeControl::Instance();
		auto* LocalizeParts = LocalizePool::Instance();
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();

		int xm1 = xcenter - WindowSizeParts->GetUIY(WinSizeX) / 2;
		int ym1 = ycenter - WindowSizeParts->GetUIY(WinSizeY) / 2;
		int xm2 = xcenter + WindowSizeParts->GetUIY(WinSizeX) / 2;
		int ym2 = ycenter + WindowSizeParts->GetUIY(WinSizeY) / 2;

		// 背景
		auto per = std::clamp(m_ActivePer * 0.5f, 0.f, 1.f);
		DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * per), 0, 255));
		WindowSystem::SetBox(xm1, ym1, xm2, ym2, Gray50);
		DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);

		// タイトル
		WindowSystem::SetMsg(
			xm1 + WindowSizeParts->GetUIY(32), ym1 + LineHeight / 4 + LineHeight,
			LineHeight * 2, FontHandle::FontXCenter::LEFT, White, Black, m_WindwoName);
		// 
		if (m_Active) {
			int xp1 = xm2 - WindowSizeParts->GetUIY(140);
			int yp1 = ym1 + LineHeight / 4 + LineHeight / 2;
			if (WindowSystem::SetMsgClickBox(xp1, yp1 + WindowSizeParts->GetUIY(5), xp1 + WindowSizeParts->GetUIY(108), yp1 + LineHeight * 2 - WindowSizeParts->GetUIY(5), LineHeight, Red, false, true, LocalizeParts->Get(20))) {
				End();
			}
		}
		// 背景
		{
			int xp1 = xm1 + WindowSizeParts->GetUIY(24);
			int yp1 = ym1 + LineHeight * 3;
			int xp2 = xm2 - WindowSizeParts->GetUIY(24);
			int yp2 = ym2 - LineHeight;
			DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * 0.5f), 0, 255));
			WindowSystem::SetBox(xp1, yp1, xp2, yp2, Gray50);
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
};
