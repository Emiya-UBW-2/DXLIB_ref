#include "DXLib_ref.h"

namespace DXLibRef {
	const SaveDataClass* SingletonBase<SaveDataClass>::m_Singleton = nullptr;
	const SideLog* SingletonBase<SideLog>::m_Singleton = nullptr;
	const PopUp* SingletonBase<PopUp>::m_Singleton = nullptr;
	const UISystem* SingletonBase<UISystem>::m_Singleton = nullptr;
	
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	float GetEasingRatio(EasingType EasingType, float ratio) {
		auto* DrawParts = DXDraw::Instance();
		switch (EasingType) {
			case EasingType::OutExpo:
				return (1.f - std::powf(ratio, 60.f / DrawParts->GetFps()));
			default:
				return 1.f;
		}
	}
	bool IntoMouse(int x1, int  y1, int  x2, int  y2) {
		auto* Pad = PadControl::Instance();
		int mx =Pad->GetMS_X();
		int my = Pad->GetMS_Y();
		return HitPointToRectangle(mx, my, x1, y1, x2, y2);
	}
	static Vector3DX GetScreenPos(const Vector3DX&campos, const Vector3DX&camvec, const Vector3DX&camup, float fov, float near_t, float far_t, const Vector3DX&worldpos) noexcept {
		int ScrX = y_r(basex);
		int ScrY = y_r(basey);
		// ビュー行列と射影行列の取得
		MATRIX mat_view;					// ビュー行列
		VECTOR vec_from = campos.get();		// カメラの位置
		VECTOR vec_lookat = camvec.get();   // カメラの注視点
		VECTOR vec_up = camup.get();        // カメラの上方向
		CreateLookAtMatrix(&mat_view, &vec_from, &vec_lookat, &vec_up);
		SetCameraNearFar(near_t, far_t);
		SetupCamera_Perspective(fov);
		MATRIX proj = GetCameraProjectionMatrix();
		// ビューポート行列（スクリーン行列）の作成
		float w = (float)ScrX / 2.0f;
		float h = (float)ScrY / 2.0f;
		MATRIX viewport = {
			w , 0 , 0 , 0 ,
			0 ,-h , 0 , 0 ,
			0 , 0 , 1 , 0 ,
			w , h , 0 , 1
		};
		VECTOR screenPos, tmp = worldpos.get();
		// ビュー変換とプロジェクション変換
		tmp = VTransform(tmp, mat_view);
		tmp = VTransform(tmp, proj);
		// zで割って-1~1の範囲に収める
		tmp.x /= tmp.z; tmp.y /= tmp.z; tmp.z /= tmp.z;
		// スクリーン変換
		screenPos = VTransform(tmp, viewport);
		screenPos.z = -1.f;
		if (Vector3DX::Dot(camvec - campos, worldpos - campos) > 0.f) {
			screenPos.z = 0.5f;
		}
		return screenPos;
	}
	//
	namespace WindowSystem {
		//箱
		void SetBox(int xp1, int yp1, int xp2, int yp2, unsigned int colorSet) {
			DrawBox_2D((int)(xp1), (int)(yp1), (int)(xp2), (int)(yp2), colorSet, true);
		}
		bool SetClickBox(int xp1, int yp1, int xp2, int yp2, unsigned int colorSet) {
			auto* Pad = PadControl::Instance();
			bool MouseOver = IntoMouse(xp1, yp1, xp2, yp2);
			SetBox(xp1, yp1, xp2, yp2, MouseOver ? (Pad->GetMouseClick().press() ? Gray25 : White) : colorSet);
			return (MouseOver && Pad->GetMouseClick().trigger());
		};
		//文字
		const bool GetMsgPos(int* xp1, int *yp1, int xp2, int yp2, int size, int xSize, FontHandle::FontXCenter FontX) {
			*yp1 = *yp1 + (yp2 - *yp1) / 2;
			if ((*yp1 - size / 2) > y_UI(1080) || (*yp1 + size / 2) < 0) { return false; }				//画面外は表示しない
			switch (FontX) {
				case FontHandle::FontXCenter::LEFT:
					*xp1 = *xp1 + y_UI(6);
					if ((*xp1) > y_UI(1920) || (*xp1 + xSize) < 0) { return false; }						//画面外は表示しない
					break;
				case FontHandle::FontXCenter::MIDDLE:
					*xp1 = *xp1 + (xp2 - *xp1) / 2;
					if ((*xp1 - xSize / 2) > y_UI(1920) || (*xp1 + xSize / 2) < 0) { return false; }		//画面外は表示しない
					break;
				case FontHandle::FontXCenter::RIGHT:
					*xp1 = xp2 - y_UI(6);
					if ((*xp1 - xSize) > y_UI(1920) || (*xp1) < 0) { return false; }						//画面外は表示しない
					break;
				default:
					break;
			}
			*xp1 = (int)(*xp1);
			*yp1 = (int)(*yp1);
			return true;
		};
		//
		bool CheckBox(int xp1, int yp1, bool switchturn) {
			int xp3 = xp1 + EdgeSize;
			int yp3 = yp1 + EdgeSize;
			int xp4 = xp1 + LineHeight * 2 - EdgeSize;
			int yp4 = yp1 + LineHeight - EdgeSize;

			auto* Pad = PadControl::Instance();
			bool MouseOver = IntoMouse(xp3, yp3, xp4, yp4);
			if (MouseOver && Pad->GetMouseClick().trigger()) {
				switchturn ^= 1;
				auto* SE = SoundPool::Instance();
				SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}
			unsigned int color = Gray25;
			SetBox(xp3 + y_UI(5), yp3 + y_UI(5), xp4 - y_UI(5), yp4 - y_UI(5), Black);
			xp4 = xp1 + LineHeight * (switchturn ? 1 : 0) - EdgeSize;
			SetBox(xp3 + y_UI(5), yp3 + y_UI(5), xp4 + y_UI(5), yp4 - y_UI(5), Gray50);
			xp3 = xp1 + LineHeight * (switchturn ? 1 : 0) + EdgeSize;
			xp4 = xp1 + LineHeight * (switchturn ? 2 : 1) - EdgeSize;
			SetBox(xp3, yp3, xp4, yp4, color);
			return switchturn;
		}
		//
		int UpDownBar(int xmin, int xmax, int yp, int value, int valueMin, int valueMax) {
			int xp = 0;
			{
				int xpmin = xmin + 1;
				int xpmax = xmax - 1;

				auto* Pad = PadControl::Instance();
				bool MouseOver = IntoMouse(xpmin - 5, yp, xpmin + (xpmax - xpmin) + 5, yp + LineHeight);
				if (MouseOver && Pad->GetMouseClick().trigger()) {
					auto* SE = SoundPool::Instance();
					SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
				if (MouseOver && Pad->GetMouseClick().press()) {
					value = std::clamp(((valueMax - valueMin) * (Pad->GetMS_X() - xpmin) / (xpmax - xpmin)) + valueMin, valueMin, valueMax);
				}

				SetBox(xpmin, yp, xpmin + (xpmax - xpmin), yp + LineHeight, DarkGreen);
				SetBox(xpmin, yp, xpmin + (xpmax - xpmin)*std::clamp(value - valueMin, 0, valueMax - valueMin) / (valueMax - valueMin), yp + LineHeight,
					   MouseOver ? (Pad->GetMouseClick().press() ? Gray25 : White) : Green);
			}
			xp = (xmin + (xmax - xmin) / 2);
			SetMsgWW(xp, yp, xp, yp + LineHeight, LineHeight, FontHandle::FontXCenter::MIDDLE, White, Black, "%03d", value);

			return value;
		}

		int UpDownBox(int xmin, int xmax, int yp, int value, int valueMax) {
			{
				int width = LineHeight;
				int r = LineHeight / 3;
				int xps = (xmax + xmin) / 2;
				int yps = yp + LineHeight / 2;
				for (int loop = 0; loop < valueMax; loop++) {
					int xp1 = xps + loop * width - width * (valueMax - 1) / 2;
					if (SetClickBox(xp1 - r, yps - r, xp1 + r, yps + r, (value == loop) ? Green : DarkGreen)) {
						auto* SE = SoundPool::Instance();
						SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);

						value = loop;
					}
				}
			}
			return value;
		}
	};
	//
	void			moves::Update_Physics(float speed_randam, float rate) {
		auto* DrawParts = DXDraw::Instance();
		this->pos += this->vec*((float)((1000 - int(1000.f*speed_randam)) + GetRand(int(1000.f*speed_randam) * 2)) / 1000.f);
		this->vec.y += M_GR / powf((DrawParts->GetFps() / rate), 2.f);

		//this->gun_m.pos += this->gun_m.vec;
		//this->gun_m.vec.yadd(M_GR / std::powf(DrawParts->GetFps(), 2.f));
	}
	void			switchs::Execute(bool key) noexcept {
		auto* DrawParts = DXDraw::Instance();
		m_press = key;
		if (m_press) {
			m_presscount = std::clamp<int8_t>(m_presscount + 1, 0, 2);

			m_repeat = trigger();
			m_repeatcount -= 60.f / DrawParts->GetFps();
			if (m_repeatcount <= 0.f) {
				m_repeatcount += 2.f;
				m_repeat = true;
			}
		}
		else {
			m_presscount = std::clamp<int8_t>(m_presscount - 1, 0, 2);

			m_repeat = false;
			m_repeatcount = 30.f;
		}
		if (trigger()) {
			m_on ^= 1;
		}
	}
	void			Pendulum2D::Update() {
		auto* DrawParts = DXDraw::Instance();
		m_vel += (-9.8f / this->m_PendulumLength * std::sin(m_rad) - this->m_drag_coeff / this->m_PendulumMass * this->m_vel) / DrawParts->GetFps();
		m_rad += this->m_vel / DrawParts->GetFps();
	}
	//
	void SideLog::SideLogData::UpdateActive() noexcept {
		auto* DrawParts = DXDraw::Instance();
		if (m_Time > 0.f) {
			m_Time -= 1.f / DrawParts->GetFps();
		}
		else {
			m_Time = -1.f;
		}
		Easing(&m_Flip_Y, m_Flip, 0.9f, EasingType::OutExpo);
	}
	void SideLog::Draw() noexcept {
		auto* Fonts = FontPool::Instance();

		int xp1, yp1;
		xp1 = y_UI(64);
		yp1 = y_UI(256);

		for (auto& d : data) {
			if (d.ActivePer() > 0.f) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*d.ActivePer()), 0, 255));
				int yp = yp1 - y_UI(24.f * d.GetFlip());
				WindowSystem::SetBox(
					xp1 - y_UI(6), yp + y_UI(18),
					xp1 - y_UI(6) + (int)(std::max(Fonts->Get(FontPool::FontType::Nomal_Edge).GetStringWidth(y_UI(18), d.GetMsg()), y_UI(200))*d.ActivePer()), yp + y_UI(18) + y_UI(5),
					Black);
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_UI(18.f), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
																	  (int)(xp1), (int)(yp), d.GetMsgColor(), Black, d.GetMsg());
			}
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	//
	void PopUp::PopUpDrawClass::Start() noexcept
	{
		auto* SE = SoundPool::Instance();
		auto* Pad = PadControl::Instance();

		Pad->SetGuideUpdate();
		Pad->ChangeGuide(
			[&]() {
				auto* KeyGuide = PadControl::Instance();
				auto* LocalizeParts = LocalizePool::Instance();
				KeyGuide->AddGuide(PADS::RELOAD, LocalizeParts->Get(9991));
				m_GuideDoing();
			}
		);
		SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);

		m_Active = true;
		m_ActiveSwitch = true;
	}
	void PopUp::PopUpDrawClass::End() noexcept
	{
		auto* SE = SoundPool::Instance();
		auto* Pad = PadControl::Instance();

		SE->Get((int)SoundEnumCommon::UI_CANCEL).Play(0, DX_PLAYTYPE_BACK, TRUE);
		m_Active = false;
		m_ActiveSwitch = true;
		Pad->SetGuideUpdate();
		m_ExitDoing();
	}
	void PopUp::PopUpDrawClass::Update() noexcept {
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
		auto* LocalizeParts = LocalizePool::Instance();

		int xm1, ym1;
		int xm2, ym2;

		int xp1, yp1;
		int xp2, yp2;

		if (m_ActivePer > 1.f / 255.f) {
			xm1 = xcenter - WinSizeX / 2;
			ym1 = ycenter - WinSizeY / 2;
			xm2 = xcenter + WinSizeX / 2;
			ym2 = ycenter + WinSizeY / 2;

			//背景
			auto per = std::clamp(m_ActivePer * 0.3f, 0.f, 1.f);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
			WindowSystem::SetBox(xm1, ym1, xm2, ym2, Gray50);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

			//タイトル
			{
				xp1 = xm1 + y_UI(32);
				yp1 = ym1 + LineHeight / 4;
				WindowSystem::SetMsg(xp1,yp1,xp1,yp1+ LineHeight * 2, LineHeight * 2,FontHandle::FontXCenter::LEFT, White, Black, m_WindwoName);
			}
			//
			if (m_Active) {
				xp1 = xm2 - y_UI(140);
				yp1 = ym1 + LineHeight / 4 + LineHeight / 2;
				if (WindowSystem::SetMsgClickBox(xp1, yp1 + y_UI(5), xp1 + y_UI(108), yp1 + LineHeight * 2 - y_UI(5), Red, LocalizeParts->Get(20))) {
					End();
				}
			}
			xp1 = xm1 + y_UI(24);
			yp1 = ym1 + LineHeight * 3;
			xp2 = xm2 - y_UI(24);
			yp2 = ym2 - LineHeight;
			//背景
			{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*0.3), 0, 255));
				WindowSystem::SetBox(xp1, yp1, xp2, yp2, Gray50);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			//
			m_Doing(xp1, yp1, xp2, yp2, m_ActiveSwitch);
			//
		}
	}

	void PopUp::Add(const char* WindowName, int sizex, int sizey,
				std::function<void(int xmin, int ymin, int xmax, int ymax, bool EndSwitch)> doing,
				std::function<void()> ExitDoing,
				std::function<void()> GuideDoing,
				bool IsInsert) noexcept {
		que.at(LastSel).Set(WindowName, sizex, sizey, doing, ExitDoing, GuideDoing);
		if (!IsActivePop()) {
			que.at(LastSel).Start();
			auto* DrawParts = DXDraw::Instance();
			PrevPause = DrawParts->IsPause();
			DrawParts->SetPause(true);
		}
		else if (IsInsert) {
			que.at(NowSel).End();
			NowSel = LastSel;
			que.at(LastSel).Start();
		}
		++LastSel %= ((int)que.size());
	}
	void PopUp::EndAll() noexcept {
		if (!IsActivePop()) { return; }
		que.at(NowSel).End();
		NowSel = LastSel;
	}
	void PopUp::Update() noexcept {
		if (!IsActivePop()) { return; }
		que.at(NowSel).Update();
		if (que.at(NowSel).IsEnd()) {
			++NowSel %= ((int)que.size());
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
	void UISystem::UI_CommonParts::SetParts(const nlohmann::json& pJson) noexcept {
		this->m_Name = pJson["Name"];
		//
		std::string Type = pJson["Type"];
		for (int i = 0; i < (int)EnumUIPartsType::Max; i++) {
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
		for (int i = 0; i < 3; i++) {
			if (XCenter == g_UIXCenterString[i]) {
				this->m_UIXCenter = (UIXCenter)i;
				break;
			}
		}
		std::string YCenter = pJson["YCenter"];
		for (int i = 0; i < 3; i++) {
			if (YCenter == g_UIYCenterString[i]) {
				this->m_UIYCenter = (UIYCenter)i;
				break;
			}
		}
		//色関係
		auto GetColorByPallet = [&](const std::string& ColorStr) {
			for (int i = 0; i < g_UIColorPalletNum; i++) {
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
		//テキスト
		this->m_TextID = pJson["TextID"];
		//描画位置を決定
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
	void UISystem::UI_CommonParts::Update() noexcept {
		if (this->m_IsMouseClickActive) {
			auto* Pad = PadControl::Instance();
			int mx = Pad->GetMS_X();
			int my = Pad->GetMS_Y();

			float rad = this->m_ZRotate + this->m_FrameInfo.m_ZRotOfs;

			switch (this->m_EnumUIPartsType) {
			case EnumUIPartsType::Box:
			{
				int xp = this->m_DrawXCenter + (int)this->m_FrameInfo.m_XOfs;
				int yp = this->m_DrawYCenter + (int)this->m_FrameInfo.m_YOfs;
				float xs = this->m_XSize * this->m_FrameInfo.m_XScale / 2.f;
				float ys = this->m_YSize * this->m_FrameInfo.m_YScale / 2.f;

				int   x1 = (int)(-xs * cos(rad) - -ys * sin(rad));
				int   y1 = (int)(-ys * cos(rad) + -xs * sin(rad));
				int   x2 = (int)(xs * cos(rad) - -ys * sin(rad));
				int   y2 = (int)(-ys * cos(rad) + xs * sin(rad));
				int   x3 = (int)(xs * cos(rad) - ys * sin(rad));
				int   y3 = (int)(ys * cos(rad) + xs * sin(rad));
				int   x4 = (int)(-xs * cos(rad) - ys * sin(rad));
				int   y4 = (int)(ys * cos(rad) + -xs * sin(rad));
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
			default:
				m_MouseOver = false;
				break;
			}
		}
		else {
			m_MouseOver = false;
		}
	}
	void UISystem::UI_CommonParts::Draw() noexcept {
		if (this->m_FrameInfo.m_Alpha == 0.f) { return; }
		if (1.f > this->m_FrameInfo.m_Alpha) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f * this->m_FrameInfo.m_Alpha), 0, 255));
		}
		auto* LocalizeParts = LocalizePool::Instance();

		float rad = this->m_ZRotate + this->m_FrameInfo.m_ZRotOfs;

		switch (this->m_EnumUIPartsType) {
		case EnumUIPartsType::Box:
		{
			int xp = this->m_DrawXCenter + (int)this->m_FrameInfo.m_XOfs;
			int yp = this->m_DrawYCenter + (int)this->m_FrameInfo.m_YOfs;
			float xs = this->m_XSize * this->m_FrameInfo.m_XScale / 2.f;
			float ys = this->m_YSize * this->m_FrameInfo.m_YScale / 2.f;

			int   x1 = (int)(-xs * cos(rad) - -ys * sin(rad));
			int   y1 = (int)(-ys * cos(rad) + -xs * sin(rad));
			int   x2 = (int)(xs * cos(rad) - -ys * sin(rad));
			int   y2 = (int)(-ys * cos(rad) + xs * sin(rad));
			int   x3 = (int)(xs * cos(rad) - ys * sin(rad));
			int   y3 = (int)(ys * cos(rad) + xs * sin(rad));
			int   x4 = (int)(-xs * cos(rad) - ys * sin(rad));
			int   y4 = (int)(ys * cos(rad) + -xs * sin(rad));
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
			int xp = this->m_DrawXCenter + (int)this->m_FrameInfo.m_XOfs;
			int yp = this->m_DrawYCenter + (int)this->m_FrameInfo.m_YOfs;

			DrawRotaFormatString(
				xp, yp,
				1.0, 1.0,
				XCenter, YCenter,
				(double)rad,
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
				(double)rad,
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
		default:
			break;
		}
		if (1.f > this->m_FrameInfo.m_Alpha) {
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}
	//
	void UISystem::UI_CommonAnimes::SetParts(const nlohmann::json& pJson, const std::vector<std::unique_ptr<UI_CommonParts>>& Parts) noexcept {
		//
		m_TargetID.clear();
		for (auto& n : pJson["Target"]) {
			for (auto& p : Parts) {
				if (p->GetName() == n) {
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
			for (int i = 0; i < (int)LerpType::Max; i++) {
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
		if (m_AnimeFrame.size() == 0) { return; }
		if (m_Frame < 0) { return; }
		float FramePer = 0.f;
		//現在のアニメ番号を取得
		m_NowAnim = -1;
		int tmpFrame = m_Frame;
		for (auto& a : m_AnimeFrame) {
			if (tmpFrame < a.m_framepoint) {
				m_NowAnim = (int)(&a - &m_AnimeFrame.front());
				FramePer = (float)tmpFrame / a.m_framepoint;
				switch (a.m_LerpType) {
					case LerpType::linear:
						FramePer = FramePer;
						break;
					case LerpType::pow2:
						FramePer = FramePer * FramePer;
						break;
					default:
						break;
				}
				break;
			}
			tmpFrame -= a.m_framepoint;
		}
		if (m_NowAnim == -1) {
			m_NowAnim = (int)(m_AnimeFrame.size()) - 1;
			FramePer = 1.f;
		}
		//反映
		for (auto& t : m_TargetID) {
			for (auto& p : *Parts) {
				if (p->GetUniqueID() == t) {
					auto& Prev = m_AnimeFrame.at(std::max(m_NowAnim - 1, 0));
					auto& Now = m_AnimeFrame.at(m_NowAnim);
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
	//1レイヤー分
	void UISystem::UI_OneLayer::Load(const char* path) noexcept {
		m_IsEnd = false;

		UniqueIDNum = 0;

		int mdata = FileRead_open(path);
		LONGLONG size = FileRead_size_handle(mdata);
		std::string JsonStr;JsonStr.resize(size, '\0');
		FileRead_read((void*)JsonStr.c_str(), (int)size, mdata);
		FileRead_close(mdata);

		nlohmann::json pJson = nlohmann::json::parse(JsonStr);
		for (auto& j : pJson["Parts"]) {
			m_CommonParts.emplace_back(std::make_unique<UI_CommonParts>());
			m_CommonParts.back()->SetUniqueID(UniqueIDNum);
			m_CommonParts.back()->SetParts(j);
			UniqueIDNum++;
		}
		for (auto& j : pJson["TotalAnim"]) {
			m_CommonAnimes.emplace_back(std::make_unique<UI_CommonAnimes>());
			m_CommonAnimes.back()->SetParts(j, m_CommonParts);
		}
		std::sort(m_CommonParts.begin(), m_CommonParts.end(), [&](const std::unique_ptr<UI_CommonParts>& A, const std::unique_ptr<UI_CommonParts>& B) {
			return (A->GetLayer() != B->GetLayer()) ? (A->GetLayer() < B->GetLayer()) : (A->GetUniqueID() < B->GetUniqueID());
		});
		if (UniqueIDNum == 0) { UniqueIDNum = -1; }
	}
	void UISystem::UI_OneLayer::Update() noexcept {
		for (auto& p : m_CommonAnimes) {
			p->Update(&m_CommonParts);
		}
		for (auto& p : m_CommonParts) {
			p->Update();
		}
	}
	void UISystem::UI_OneLayer::Draw() noexcept {
		for (auto& p : m_CommonParts) {
			p->Draw();
		}
	}
	void UISystem::UI_OneLayer::Dispose() noexcept {
		for (auto& p : m_CommonAnimes) {
			p.reset();
		}
		m_CommonAnimes.clear();
		for (auto& p : m_CommonParts) {
			p.reset();
		}
		m_CommonParts.clear();
		UniqueIDNum = -1;
	}
	//全体
	int UISystem::AddUI(const char* path) noexcept {
		for (auto& l : m_Layer) {
			if (!l.IsActive()) {
				l.Load(path);
				return (int)(&l - &m_Layer.front());
			}
		}
		return -1;
	}
	void UISystem::DelUI(int layer) noexcept {
		auto& l = m_Layer.at(layer);
		if (l.IsActive()) {
			l.Dispose();
		}
	}
	void UISystem::Update() noexcept {
		for (auto& l : m_Layer) {
			if (l.IsActive()) {
				l.Update();
			}
		}
	}
	void UISystem::Draw() noexcept {
		for (auto& l : m_Layer) {
			if (l.IsActive()) {
				l.Draw();
			}
		}
	}
	void UISystem::DisposeAll() noexcept {
		for (auto& l : m_Layer) {
			if (l.IsActive()) {
				l.Dispose();
			}
		}
	}
	//
};
