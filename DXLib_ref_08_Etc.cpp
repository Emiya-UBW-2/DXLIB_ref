#include "DXLib_ref.h"
// #include "DXLib_ref_08_Etc.hpp"

namespace DXLibRef {
	const SaveDataClass* SingletonBase<SaveDataClass>::m_Singleton = nullptr;
	const SideLog* SingletonBase<SideLog>::m_Singleton = nullptr;
	const PopUp* SingletonBase<PopUp>::m_Singleton = nullptr;
	const UniversalUI::UISystem* SingletonBase<UniversalUI::UISystem>::m_Singleton = nullptr;
	const CameraShake* SingletonBase<CameraShake>::m_Singleton = nullptr;
	const WindowSystem::DrawControl* SingletonBase<WindowSystem::DrawControl>::m_Singleton = nullptr;

	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	float GetEasingRatio(EasingType EasingType, float ratio) noexcept {
		auto* DrawParts = DXDraw::Instance();
		switch (EasingType) {
		case EasingType::OutExpo:
			return (1.f - std::powf(ratio, FrameRate * DrawParts->GetDeltaTime()));
		default:
			return 1.f;
		}
	}
	bool IntoMouse(int x1, int y1, int x2, int y2) noexcept {
		auto* Pad = PadControl::Instance();
		int mx = Pad->GetMS_X();
		int my = Pad->GetMS_Y();
		return HitPointToRectangle(mx, my, x1, y1, x2, y2);
	}
	static Vector3DX GetScreenPos(const Vector3DX& campos, const Vector3DX& camvec, const Vector3DX& camup, float fov, float near_t, float far_t, const Vector3DX& worldpos) noexcept {
		auto* DrawParts = DXDraw::Instance();
		// ビュー行列と射影行列の取得
		Camera3DInfo tmpcam;
		tmpcam.SetCamPos(campos, camvec, camup);
		tmpcam.SetCamInfo(fov, near_t, far_t);
		// ビューポート行列（スクリーン行列）の作成
		float w = static_cast<float>(DrawParts->GetScreenXMax()) / 2.0f;
		float h = static_cast<float>(DrawParts->GetScreenYMax()) / 2.0f;
		MATRIX viewport = {
			w , 0 , 0 , 0 ,
			0 ,-h , 0 , 0 ,
			0 , 0 , 1 , 0 ,
			w , h , 0 , 1
		};
		// ビュー変換とプロジェクション変換
		Vector3DX tmp = Matrix4x4DX::Vtrans(Matrix4x4DX::Vtrans(worldpos, tmpcam.GetViewMatrix()), tmpcam.GetProjectionMatrix());
		// zで割って-1~1の範囲に収める
		tmp /= tmp.z;
		// スクリーン変換
		Vector3DX screenPos = Matrix4x4DX::Vtrans(tmp, viewport);
		// 深度チェックを簡素に追加
		if (Vector3DX::Dot(camvec - campos, worldpos - campos) > 0.f) {
			screenPos.z = 0.5f;
		}
		else {
			screenPos.z = -1.f;
		}
		return screenPos;
	}
	// 
	namespace WindowSystem {
		void DrawData::Output() const noexcept {
			auto* DrawParts = DXDraw::Instance();
			Rect2D Widow; Widow.Set(0, 0, DrawParts->GetUIXMax(), DrawParts->GetUIYMax());

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

			auto* DrawParts = DXDraw::Instance();
			m_BufferScreen.Make(DrawParts->GetUIXMax(), DrawParts->GetUIYMax(), true);
		}
		bool DrawControl::IsDrawOnWindow(int x1, int y1, int x2, int y2) noexcept {
			auto* DrawParts = DXDraw::Instance();
			return HitRectangleToRectangle(0, 0, DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), std::min(x1, x2), std::min(y1, y2), std::max(x1, x2), std::max(y1, y2));
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
			auto* DrawParts = DXDraw::Instance();
			switch (FontX) {
			case FontHandle::FontXCenter::LEFT:
				*xp1 = *xp1 + DrawParts->GetUIY(6);
				return HitRectangleToRectangle(
					(*xp1), (*yp1 - ySize / 2), (*xp1 + xSize), (*yp1 + ySize / 2),
					0, 0, DrawParts->GetUIXMax(), DrawParts->GetUIYMax());
			case FontHandle::FontXCenter::MIDDLE:
				return HitRectangleToRectangle(
					(*xp1 - xSize / 2), (*yp1 - ySize / 2), (*xp1 + xSize / 2), (*yp1 + ySize),
					0, 0, DrawParts->GetUIXMax(), DrawParts->GetUIYMax());
			case FontHandle::FontXCenter::RIGHT:
				*xp1 = *xp1 - DrawParts->GetUIY(6);
				return HitRectangleToRectangle(
					(*xp1 - xSize), (*yp1 - ySize / 2), (*xp1), (*yp1 + ySize / 2),
					0, 0, DrawParts->GetUIXMax(), DrawParts->GetUIYMax());
			default:
				return false;
			}
		};
		// オンオフできるボタン
		bool CheckBox(int xp1, int yp1, bool switchturn) noexcept {
			auto* DrawParts = DXDraw::Instance();
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
			int Edge = DrawParts->GetUIY(5);
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
	void			switchs::Execute(bool key) noexcept {
		auto* DrawParts = DXDraw::Instance();
		m_press = key;
		if (m_press) {
			m_presscount = std::clamp<int8_t>(m_presscount + 1, 0, 2);

			m_repeat = trigger();
			m_repeatcount -= DrawParts->GetDeltaTime();
			if (m_repeatcount <= 0.f) {
				m_repeatcount += 2.f / 60.f;
				m_repeat = true;
			}
		}
		else {
			m_presscount = std::clamp<int8_t>(m_presscount - 1, 0, 2);

			m_repeat = false;
			m_repeatcount = 0.5f;
		}
		if (trigger()) {
			m_on ^= 1;
		}
	}
	// 
	void			Pendulum2D::Update(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		m_vel += (-9.8f / this->m_PendulumLength * std::sin(m_rad) - this->m_drag_coeff / this->m_PendulumMass * this->m_vel) * DrawParts->GetDeltaTime();
		m_rad += this->m_vel * DrawParts->GetDeltaTime();
	}
	// 
	void SideLog::SideLogData::UpdateActive(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		if (m_Time > 0.f) {
			m_Time -= DrawParts->GetDeltaTime();
		}
		else {
			m_Time = -1.f;
		}
		Easing(&m_Flip_Y, m_Flip, 0.9f, EasingType::OutExpo);
	}
	void SideLog::Draw(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		int xp1, yp1;
		xp1 = DrawParts->GetUIY(64);
		yp1 = DrawParts->GetUIY(256);

		for (auto& d : data) {
			if (d.ActivePer() > 0.f) {
				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * d.ActivePer()), 0, 255));
				int yp = yp1 - DrawParts->GetUIY(static_cast<int>(24.f * d.GetFlip()));
				WindowSystem::SetBox(
					xp1 - DrawParts->GetUIY(6), yp + LineHeight,
					xp1 - DrawParts->GetUIY(6) + static_cast<int>(static_cast<float>(std::max(WindowSystem::GetMsgLen(LineHeight, d.GetMsg()), DrawParts->GetUIY(200))) * d.ActivePer()), yp + LineHeight + DrawParts->GetUIY(5),
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
		if (m_ActivePer < 1.f / 255.f) { return; }

		auto* DrawParts = DXDraw::Instance();
		auto* LocalizeParts = LocalizePool::Instance();
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();

		int xm1 = xcenter - DrawParts->GetUIY(WinSizeX) / 2;
		int ym1 = ycenter - DrawParts->GetUIY(WinSizeY) / 2;
		int xm2 = xcenter + DrawParts->GetUIY(WinSizeX) / 2;
		int ym2 = ycenter + DrawParts->GetUIY(WinSizeY) / 2;

		// 背景
		auto per = std::clamp(m_ActivePer * 0.5f, 0.f, 1.f);
		DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * per), 0, 255));
		WindowSystem::SetBox(xm1, ym1, xm2, ym2, Gray50);
		DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);

		// タイトル
		WindowSystem::SetMsg(
			xm1 + DrawParts->GetUIY(32), ym1 + LineHeight / 4 + LineHeight,
			LineHeight * 2, FontHandle::FontXCenter::LEFT, White, Black, m_WindwoName);
		// 
		if (m_Active) {
			int xp1 = xm2 - DrawParts->GetUIY(140);
			int yp1 = ym1 + LineHeight / 4 + LineHeight / 2;
			if (WindowSystem::SetMsgClickBox(xp1, yp1 + DrawParts->GetUIY(5), xp1 + DrawParts->GetUIY(108), yp1 + LineHeight * 2 - DrawParts->GetUIY(5), LineHeight, Red, false, true, LocalizeParts->Get(20))) {
				End();
			}
		}
		// 背景
		{
			int xp1 = xm1 + DrawParts->GetUIY(24);
			int yp1 = ym1 + LineHeight * 3;
			int xp2 = xm2 - DrawParts->GetUIY(24);
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
			auto* DrawParts = DXDraw::Instance();
			PrevPause = DrawParts->IsPause();
			DrawParts->SetPause(true);
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
					auto* DrawParts = DXDraw::Instance();
					DrawParts->SetPause(false);
				}
			}
		}
	}
	// 
	namespace UniversalUI {
		void UISystem::UI_CommonParts::SetParts(const nlohmann::json& pJson) noexcept {
			this->m_Name = pJson["Name"];
			// 
			std::string Type = pJson["Type"];
			for (size_t i : std::views::iota(0, static_cast<int>(EnumUIPartsType::Max))) {
				if (Type == g_UIPartsString[i]) {
					this->m_EnumUIPartsType = (EnumUIPartsType)i;
					break;
				}
			}
			// 
			this->m_Layer = pJson["Layer"];
			// 
			this->m_IsMouseClickActive = pJson["MouseClickActive"];
			// 
			this->m_XPos = pJson["XPos"];
			this->m_YPos = pJson["YPos"];
			this->m_XSize = pJson["XSize"];
			this->m_YSize = pJson["YSize"];
			this->m_ZRotate = pJson["ZRotate"];
			// 
			std::string XCenter = pJson["XCenter"];
			for (size_t i : std::views::iota(0, 3)) {
				if (XCenter == g_UIXCenterString[i]) {
					this->m_UIXCenter = (UIXCenter)i;
					break;
				}
			}
			std::string YCenter = pJson["YCenter"];
			for (size_t i : std::views::iota(0, 3)) {
				if (YCenter == g_UIYCenterString[i]) {
					this->m_UIYCenter = (UIYCenter)i;
					break;
				}
			}
			// 色関係
			auto GetColorByPallet = [this](const std::string& ColorStr) {
				for (size_t i : std::views::iota(0, static_cast<int>(g_UIColorPalletNum))) {
					if (ColorStr == g_UIColorPalletString[i]) {
						return g_UIColorPallet[i];
						break;
					}
				}
				return Black;
				};
			this->m_PressColor = GetColorByPallet(pJson["PressColor"]);
			this->m_IntoColor = GetColorByPallet(pJson["IntoColor"]);
			this->m_BaseColor = GetColorByPallet(pJson["BaseColor"]);
			this->m_EdgeColor = GetColorByPallet(pJson["EdgeColor"]);
			// テキスト
			this->m_TextID = pJson["TextID"];
			// 描画位置を決定
			switch (this->m_UIXCenter) {
			case UIXCenter::LEFT:
				this->m_DrawXCenter = this->m_XPos + this->m_XSize / 2;
				break;
			case UIXCenter::MIDDLE:
				this->m_DrawXCenter = this->m_XPos;
				break;
			case UIXCenter::RIGHT:
				this->m_DrawXCenter = this->m_XPos - this->m_XSize / 2;
				break;
			default:
				break;
			}
			switch (this->m_UIYCenter) {
			case UIYCenter::TOP:
				this->m_DrawYCenter = this->m_YPos + this->m_YSize / 2;
				break;
			case UIYCenter::MIDDLE:
				this->m_DrawYCenter = this->m_YPos;
				break;
			case UIYCenter::BOTTOM:
				this->m_DrawYCenter = this->m_YPos - this->m_YSize / 2;
				break;
			default:
				break;
			}
		}
		void UISystem::UI_CommonParts::Update(void) noexcept {
			if (this->m_IsMouseClickActive) {
				auto* Pad = PadControl::Instance();
				int mx = Pad->GetMS_X();
				int my = Pad->GetMS_Y();

				float rad = this->m_ZRotate + this->m_FrameInfo.m_ZRotOfs;
				switch (this->m_EnumUIPartsType) {
				case EnumUIPartsType::Box:
				{
					int xp = this->m_DrawXCenter + static_cast<int>(this->m_FrameInfo.m_XOfs);
					int yp = this->m_DrawYCenter + static_cast<int>(this->m_FrameInfo.m_YOfs);
					float xs = static_cast<float>(this->m_XSize) * this->m_FrameInfo.m_XScale / 2.f;
					float ys = static_cast<float>(this->m_YSize) * this->m_FrameInfo.m_YScale / 2.f;

					int  x1 = static_cast<int>(-xs * cos(rad) - -ys * sin(rad));
					int  y1 = static_cast<int>(-ys * cos(rad) + -xs * sin(rad));
					int  x2 = static_cast<int>(xs * cos(rad) - -ys * sin(rad));
					int  y2 = static_cast<int>(-ys * cos(rad) + xs * sin(rad));
					int  x3 = static_cast<int>(xs * cos(rad) - ys * sin(rad));
					int  y3 = static_cast<int>(ys * cos(rad) + xs * sin(rad));
					int  x4 = static_cast<int>(-xs * cos(rad) - ys * sin(rad));
					int  y4 = static_cast<int>(ys * cos(rad) + -xs * sin(rad));
					m_MouseOver = HitPointToSquare(
						mx, my,
						xp + x1, yp + y1,
						xp + x2, yp + y2,
						xp + x3, yp + y3,
						xp + x4, yp + y4
					);
					m_MousePress = m_MouseOver && Pad->GetMouseClick().press();
					printfDx("[]\n");
				}
				break;
				case EnumUIPartsType::Msg:
				case EnumUIPartsType::Max:
				default:
					m_MouseOver = false;
					break;
				}
			}
			else {
				m_MouseOver = false;
			}
		}
		void UISystem::UI_CommonParts::Draw(void) noexcept {
			if (this->m_FrameInfo.m_Alpha == 0.f) {
				return;
			}
			auto* DrawCtrls = WindowSystem::DrawControl::Instance();
			if (1.f > this->m_FrameInfo.m_Alpha) {
				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * this->m_FrameInfo.m_Alpha), 0, 255));
			}
			auto* LocalizeParts = LocalizePool::Instance();

			float rad = this->m_ZRotate + this->m_FrameInfo.m_ZRotOfs;

			switch (this->m_EnumUIPartsType) {
			case EnumUIPartsType::Box:
			{
				int xp = this->m_DrawXCenter + static_cast<int>(this->m_FrameInfo.m_XOfs);
				int yp = this->m_DrawYCenter + static_cast<int>(this->m_FrameInfo.m_YOfs);
				float xs = static_cast<float>(this->m_XSize) * this->m_FrameInfo.m_XScale / 2.f;
				float ys = static_cast<float>(this->m_YSize) * this->m_FrameInfo.m_YScale / 2.f;

				int  x1 = static_cast<int>(-xs * cos(rad) - -ys * sin(rad));
				int  y1 = static_cast<int>(-ys * cos(rad) + -xs * sin(rad));
				int  x2 = static_cast<int>(xs * cos(rad) - -ys * sin(rad));
				int  y2 = static_cast<int>(-ys * cos(rad) + xs * sin(rad));
				int  x3 = static_cast<int>(xs * cos(rad) - ys * sin(rad));
				int  y3 = static_cast<int>(ys * cos(rad) + xs * sin(rad));
				int  x4 = static_cast<int>(-xs * cos(rad) - ys * sin(rad));
				int  y4 = static_cast<int>(ys * cos(rad) + -xs * sin(rad));
				DrawQuadrangle(
					xp + x1, yp + y1,
					xp + x2, yp + y2,
					xp + x3, yp + y3,
					xp + x4, yp + y4,
					m_MouseOver ? (m_MousePress ? this->m_PressColor : this->m_IntoColor) : this->m_BaseColor, TRUE);
			}
			break;
			case EnumUIPartsType::Msg:
			{
				int Width = GetDrawFormatStringWidth(
					LocalizeParts->Get(this->m_TextID),
					m_TextEX0.at(0).c_str(),
					m_TextEX0.at(1).c_str(),
					m_TextEX0.at(2).c_str()
				);
				int Size = GetFontSize();
				double XCenter = 0.5;
				double YCenter = 0.5;
				switch (this->m_UIXCenter) {
				case UIXCenter::LEFT:
					this->m_DrawXCenter = this->m_XPos;
					XCenter = 0.0;
					break;
				case UIXCenter::MIDDLE:
					this->m_DrawXCenter = this->m_XPos + Width / 2;
					XCenter = 0.5;
					break;
				case UIXCenter::RIGHT:
					this->m_DrawXCenter = this->m_XPos + Width;
					XCenter = 1.0;
					break;
				default:
					break;
				}
				switch (this->m_UIYCenter) {
				case UIYCenter::TOP:
					this->m_DrawYCenter = this->m_YPos;
					YCenter = 0.0;
					break;
				case UIYCenter::MIDDLE:
					this->m_DrawYCenter = this->m_YPos + Size / 2;
					YCenter = 0.5;
					break;
				case UIYCenter::BOTTOM:
					this->m_DrawYCenter = this->m_YPos + Size;
					YCenter = 1.0;
					break;
				default:
					break;
				}
				int xp = this->m_DrawXCenter + static_cast<int>(this->m_FrameInfo.m_XOfs);
				int yp = this->m_DrawYCenter + static_cast<int>(this->m_FrameInfo.m_YOfs);

				DrawRotaFormatString(
					xp, yp,
					1.0, 1.0,
					XCenter, YCenter,
					static_cast<double>(rad),
					m_MouseOver ? (m_MousePress ? this->m_PressColor : this->m_IntoColor) : this->m_BaseColor, this->m_EdgeColor,
					FALSE,
					LocalizeParts->Get(this->m_TextID),
					m_TextEX0.at(0).c_str(),
					m_TextEX0.at(1).c_str(),
					m_TextEX0.at(2).c_str()
				);
				/*
				GetDrawFormatStringWidthToHandle(
					int FontHandle,
					LocalizeParts->Get(this->m_TextID),
					m_TextEX0.at(0).c_str(),
					m_TextEX0.at(1).c_str(),
					m_TextEX0.at(2).c_str()
				);
				GetFontSizeToHandle(int FontHandle);
				DrawRotaFormatStringToHandle(
					xp, yp,
					1.0, 1.0,
					XCenter, YCenter,
					static_cast<double>(rad),
					m_MouseOver ? (m_MousePress ? this->m_PressColor : this->m_IntoColor) : this->m_BaseColor, this->m_EdgeColor,
					int FontHandle,
					this->m_EdgeColor,
					FALSE,
					LocalizeParts->Get(this->m_TextID),
					m_TextEX0.at(0).c_str(),
					m_TextEX0.at(1).c_str(),
					m_TextEX0.at(2).c_str()
					);
					//*/
			}
			break;
			case EnumUIPartsType::Max:
			default:
				break;
			}
			if (1.f > this->m_FrameInfo.m_Alpha) {
				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
			}
		}
		// 
		void UISystem::UI_CommonAnimes::SetParts(const nlohmann::json& pJson, const std::vector<std::unique_ptr<UI_CommonParts>>& Parts) noexcept {
			// 
			m_TargetID.clear();
			for (auto& n : pJson["Target"]) {
				std::string S = n;
				for (auto& p : Parts) {
					if (p->GetName() == S) {
						m_TargetID.emplace_back(p->GetUniqueID());
						break;
					}
				}
			}
			// 
			m_AnimeFrame.clear();
			for (auto& n : pJson["Anime"]) {
				FrameInfo tmp;
				tmp.m_framepoint = n["Frame"];
				tmp.m_Alpha = n["Alpha"];
				tmp.m_XScale = n["XScale"];
				tmp.m_YScale = n["YScale"];
				tmp.m_XOfs = n["XOffset"];
				tmp.m_YOfs = n["YOffset"];
				tmp.m_ZRotOfs = n["ZRotOfs"];
				std::string Lerptype = n["LerpType"];
				for (size_t i : std::views::iota(0, static_cast<int>(LerpType::Max))) {
					if (Lerptype == g_LerpTypeStr[i]) {
						tmp.m_LerpType = (LerpType)i;
						break;
					}
				}
				m_AnimeFrame.emplace_back(tmp);
			}
			// 
		}
		void UISystem::UI_CommonAnimes::Update(std::vector<std::unique_ptr<UI_CommonParts>>* Parts) noexcept {
			if (m_AnimeFrame.size() == 0) {
				return;
			}
			if (m_Frame < 0) {
				return;
			}
			float FramePer = 0.f;
			// 現在のアニメ番号を取得
			m_NowAnim = InvalidID;
			int tmpFrame = m_Frame;
			for (size_t index = 0; auto & a : m_AnimeFrame) {
				if (tmpFrame < a.m_framepoint) {
					m_NowAnim = static_cast<int>(index);
					FramePer = static_cast<float>(tmpFrame) / static_cast<float>(a.m_framepoint);
					switch (a.m_LerpType) {
					case LerpType::linear:
						FramePer = FramePer;
						break;
					case LerpType::pow2:
						FramePer = FramePer * FramePer;
						break;
					case LerpType::Max:
						break;
					default:
						break;
					}
					break;
				}
				tmpFrame -= a.m_framepoint;
				index++;
			}
			if (m_NowAnim == InvalidID) {
				m_NowAnim = static_cast<int>(m_AnimeFrame.size()) - 1;
				FramePer = 1.f;
			}
			// 反映
			for (auto& t : m_TargetID) {
				for (auto& p : *Parts) {
					if (p->GetUniqueID() == t) {
						auto& Prev = m_AnimeFrame.at(static_cast<size_t>(std::max(m_NowAnim - 1, 0)));
						auto& Now = m_AnimeFrame.at(static_cast<size_t>(m_NowAnim));
						FrameInfo tmp;
						tmp.m_Alpha = Lerp(Prev.m_Alpha, Now.m_Alpha, FramePer);
						tmp.m_XScale = Lerp(Prev.m_XScale, Now.m_XScale, FramePer);
						tmp.m_YScale = Lerp(Prev.m_YScale, Now.m_YScale, FramePer);
						tmp.m_XOfs = Lerp(Prev.m_XOfs, Now.m_XOfs, FramePer);
						tmp.m_YOfs = Lerp(Prev.m_YOfs, Now.m_YOfs, FramePer);
						tmp.m_ZRotOfs = Lerp(Prev.m_ZRotOfs, Now.m_ZRotOfs, FramePer);
						p->SetFrameInfo(tmp);
					}
				}
			}
		}
		// 1レイヤー分
		void UISystem::UI_OneLayer::Load(const char* path) noexcept {
			m_IsEnd = false;

			UniqueIDNum = 0;

			int mdata = FileRead_open(path);
			size_t size = static_cast<size_t>(FileRead_size_handle(mdata));
			std::string JsonStr; JsonStr.resize(size, '\0');
			FileRead_read((void*)JsonStr.c_str(), static_cast<int>(size), mdata);
			FileRead_close(mdata);

			nlohmann::json pJson = nlohmann::json::parse(JsonStr);
			for (auto& j : pJson["Parts"]) {
				m_CommonParts.emplace_back(std::make_unique<UI_CommonParts>());
				m_CommonParts.back()->SetUniqueID(UniqueIDNum);
				m_CommonParts.back()->SetParts(j);
				++UniqueIDNum;
			}
			for (auto& j : pJson["TotalAnim"]) {
				m_CommonAnimes.emplace_back(std::make_unique<UI_CommonAnimes>());
				m_CommonAnimes.back()->SetParts(j, m_CommonParts);
			}
			std::sort(m_CommonParts.begin(), m_CommonParts.end(), [this](const std::unique_ptr<UI_CommonParts>& A, const std::unique_ptr<UI_CommonParts>& B) {
				return (A->GetLayer() != B->GetLayer()) ? (A->GetLayer() < B->GetLayer()) : (A->GetUniqueID() < B->GetUniqueID());
				});
			if (UniqueIDNum == 0) {
				UniqueIDNum = InvalidID;
			}
		}
		void UISystem::UI_OneLayer::Update(void) noexcept {
			for (auto& p : m_CommonAnimes) {
				p->Update(&m_CommonParts);
			}
			for (auto& p : m_CommonParts) {
				p->Update();
			}
		}
		void UISystem::UI_OneLayer::Draw(void) noexcept {
			for (auto& p : m_CommonParts) {
				p->Draw();
			}
		}
		void UISystem::UI_OneLayer::Dispose(void) noexcept {
			for (auto& p : m_CommonAnimes) {
				p.reset();
			}
			m_CommonAnimes.clear();
			for (auto& p : m_CommonParts) {
				p.reset();
			}
			m_CommonParts.clear();
			UniqueIDNum = InvalidID;
		}
		// 全体
		int UISystem::AddUI(const char* path) noexcept {
			for (size_t index = 0; auto & l : m_Layer) {
				if (!l.IsActive()) {
					l.Load(path);
					return static_cast<int>(index);
				}
				index++;
			}
			return InvalidID;
		}
		void UISystem::DelUI(int layer) noexcept {
			auto& l = m_Layer.at(static_cast<size_t>(layer));
			if (l.IsActive()) {
				l.Dispose();
			}
		}
		void UISystem::Update(void) noexcept {
			for (auto& l : m_Layer) {
				if (l.IsActive()) {
					l.Update();
				}
			}
		}
		void UISystem::Draw(void) noexcept {
			for (auto& l : m_Layer) {
				if (l.IsActive()) {
					l.Draw();
				}
			}
		}
		void UISystem::DisposeAll(void) noexcept {
			for (auto& l : m_Layer) {
				if (l.IsActive()) {
					l.Dispose();
				}
			}
		}
	}
	// 
	void CameraShake::Update(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		if (m_SendCamShakeTime > 0.f) {
			if (m_SendCamShake) {
				m_SendCamShake = false;
				this->m_CamShake = m_SendCamShakeTime;
			}
			auto RandRange = this->m_CamShake / m_SendCamShakeTime * m_SendCamShakePower;
			Easing(&this->m_CamShake1, Vector3DX::vget(GetRandf(RandRange), GetRandf(RandRange), GetRandf(RandRange)), 0.8f, EasingType::OutExpo);
			Easing(&this->m_CamShake2, this->m_CamShake1, 0.8f, EasingType::OutExpo);
			this->m_CamShake = std::max(this->m_CamShake - DrawParts->GetDeltaTime(), 0.f);
		}
	}
	// 
};
