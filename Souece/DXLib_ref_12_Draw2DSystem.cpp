#include "DXLib_ref_12_Draw2DSystem.hpp"
//#include "DXLib_ref.h"

namespace DXLibRef {
	const SideLog* SingletonBase<SideLog>::m_Singleton = nullptr;
	const PopUp* SingletonBase<PopUp>::m_Singleton = nullptr;
	const KeyGuide* SingletonBase<KeyGuide>::m_Singleton = nullptr;
	const WindowSystem::DrawControl* SingletonBase<WindowSystem::DrawControl>::m_Singleton = nullptr;
	// 
	namespace WindowSystem {
		//
		void DrawData::Output(void) const noexcept {
			auto* WindowSizeParts = WindowSizeControl::Instance();
			switch (m_type) {
			case DrawType::Alpha:
				if (this->m_intParam.at(0) < 255) {
					DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, this->m_intParam.at(0));
				}
				else {
					DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, this->m_intParam.at(0));
				}
				break;
			case DrawType::Add:
				DxLib::SetDrawBlendMode(DX_BLENDMODE_ADD, this->m_intParam.at(0));
				break;
			case DrawType::Bright:
				DxLib::SetDrawBright(this->m_intParam.at(0), this->m_intParam.at(1), this->m_intParam.at(2));
				break;
			case DrawType::Box:
				if (this->m_intParam.at(4) == 1 || (m_boolParam.at(0))) {
					DxLib::DrawBox(
						WindowSizeParts->GetUIY(this->m_intParam.at(0)), WindowSizeParts->GetUIY(this->m_intParam.at(1)),
						WindowSizeParts->GetUIY(this->m_intParam.at(2)), WindowSizeParts->GetUIY(this->m_intParam.at(3)),
						this->m_UintParam.at(0), (m_boolParam.at(0)) ? TRUE : FALSE);
				}
				else {
					int p1x = WindowSizeParts->GetUIY(this->m_intParam.at(0));
					int p1y = WindowSizeParts->GetUIY(this->m_intParam.at(1));
					int p2x = WindowSizeParts->GetUIY(this->m_intParam.at(2));
					int p2y = WindowSizeParts->GetUIY(this->m_intParam.at(3));
					DxLib::DrawLine(p1x, p1y, p1x, p2y, this->m_UintParam.at(0), this->m_intParam.at(4));
					DxLib::DrawLine(p1x, p1y, p2x, p1y, this->m_UintParam.at(0), this->m_intParam.at(4));
					DxLib::DrawLine(p1x, p2y, p2x, p2y, this->m_UintParam.at(0), this->m_intParam.at(4));
					DxLib::DrawLine(p2x, p1y, p2x, p2y, this->m_UintParam.at(0), this->m_intParam.at(4));
				}
				break;
			case DrawType::Quadrangle:
				DxLib::DrawQuadrangle(
					WindowSizeParts->GetUIY(this->m_intParam.at(0)), WindowSizeParts->GetUIY(this->m_intParam.at(1)),
					WindowSizeParts->GetUIY(this->m_intParam.at(2)), WindowSizeParts->GetUIY(this->m_intParam.at(3)),
					WindowSizeParts->GetUIY(this->m_intParam.at(4)), WindowSizeParts->GetUIY(this->m_intParam.at(5)),
					WindowSizeParts->GetUIY(this->m_intParam.at(6)), WindowSizeParts->GetUIY(this->m_intParam.at(7)),
					this->m_UintParam.at(0), (m_boolParam.at(0)) ? TRUE : FALSE);
				break;
			case DrawType::Circle:
				DxLib::DrawCircle(
					WindowSizeParts->GetUIY(m_intParam.at(0)), WindowSizeParts->GetUIY(this->m_intParam.at(1)), WindowSizeParts->GetUIY(this->m_intParam.at(2)),
					this->m_UintParam.at(0), (m_boolParam.at(0)) ? TRUE : FALSE,
					(this->m_intParam.at(3) >= 2) ? WindowSizeParts->GetUIY(this->m_intParam.at(3)) : this->m_intParam.at(3));
				break;
			case DrawType::Line:
				DxLib::DrawLine(WindowSizeParts->GetUIY(m_intParam.at(0)), WindowSizeParts->GetUIY(this->m_intParam.at(1)), WindowSizeParts->GetUIY(this->m_intParam.at(2)), WindowSizeParts->GetUIY(this->m_intParam.at(3)), this->m_UintParam.at(0),
					(this->m_intParam.at(4) >= 2) ? WindowSizeParts->GetUIY(this->m_intParam.at(4)) : this->m_intParam.at(4));
				break;
			case DrawType::String:
				FontSystem::FontPool::Instance()->Get((FontSystem::FontType)m_intParam.at(0), WindowSizeParts->GetUIY(this->m_intParam.at(1)), 3)->DrawString(
						(FontSystem::FontXCenter)m_intParam.at(2), (FontSystem::FontYCenter)m_intParam.at(3),
						WindowSizeParts->GetUIY(m_intParam.at(4)), WindowSizeParts->GetUIY(this->m_intParam.at(5)),
						m_UintParam.at(0),
						m_UintParam.at(1),
						m_string.c_str()
					);
				break;
			case DrawType::StringAutoFit:
				FontSystem::FontPool::Instance()->Get((FontSystem::FontType)m_intParam.at(0), WindowSizeParts->GetUIY(this->m_intParam.at(1)), 3)->DrawStringAutoFit(
					WindowSizeParts->GetUIY(m_intParam.at(2)), WindowSizeParts->GetUIY(m_intParam.at(3)),
						WindowSizeParts->GetUIY(m_intParam.at(4)), WindowSizeParts->GetUIY(this->m_intParam.at(5)),
						m_UintParam.at(0),
						m_UintParam.at(1),
						m_string.c_str()
					);
				break;
			case DrawType::RotaGraph:
				if (m_GraphHandleParam.at(0)) {
					float Scale = (float)(WindowSizeParts->GetUIY((int)(this->m_floatParam.at(0) * 10000))) / 10000.f;

					if (Scale < 0.95f && 1.05f < Scale) {
						auto prev = GetDrawMode();
						DxLib::SetDrawMode(DX_DRAWMODE_BILINEAR);
						m_GraphHandleParam.at(0)->DrawRotaGraph(
							WindowSizeParts->GetUIY(this->m_intParam.at(0)), WindowSizeParts->GetUIY(this->m_intParam.at(1)),
							Scale, this->m_floatParam.at(1), this->m_boolParam.at(0));
						DxLib::SetDrawMode(prev);
					}
					else {
						m_GraphHandleParam.at(0)->DrawRotaGraph(
							WindowSizeParts->GetUIY(this->m_intParam.at(0)), WindowSizeParts->GetUIY(this->m_intParam.at(1)),
							Scale, this->m_floatParam.at(1), this->m_boolParam.at(0));
					}
				}
				break;
			case DrawType::ExtendGraph:
				if (m_GraphHandleParam.at(0)) {
					auto prev = GetDrawMode();
					DxLib::SetDrawMode(DX_DRAWMODE_BILINEAR);
					m_GraphHandleParam.at(0)->DrawExtendGraph(WindowSizeParts->GetUIY(this->m_intParam.at(0)), WindowSizeParts->GetUIY(this->m_intParam.at(1)), WindowSizeParts->GetUIY(this->m_intParam.at(2)), WindowSizeParts->GetUIY(this->m_intParam.at(3)), this->m_boolParam.at(0));
					DxLib::SetDrawMode(prev);
				}
				break;
			case DrawType::CircleGauge:
				if (m_GraphHandleParam.at(0)) {
					DxLib::DrawCircleGauge(WindowSizeParts->GetUIY(this->m_intParam.at(0)), WindowSizeParts->GetUIY(this->m_intParam.at(1)),
						(double)this->m_floatParam.at(0),
						m_GraphHandleParam.at(0)->get(),
						(double)this->m_floatParam.at(1),
						(double)(this->m_floatParam.at(2) * (float)(WindowSizeParts->GetUIY(100)) / 100.f));
				}
				break;
			case DrawType::NineSliceGraph:
				if (m_GraphHandleParam.at(0)) {
					Draw9SliceGraph(
						WindowSizeParts->GetUIY(this->m_intParam.at(0)), WindowSizeParts->GetUIY(this->m_intParam.at(1)),
						WindowSizeParts->GetUIY(this->m_intParam.at(2)), WindowSizeParts->GetUIY(this->m_intParam.at(3)),
						WindowSizeParts->GetUIY(this->m_intParam.at(4)), WindowSizeParts->GetUIY(this->m_intParam.at(5)),
						WindowSizeParts->GetUIY(this->m_intParam.at(6)), WindowSizeParts->GetUIY(this->m_intParam.at(7)),
						this->m_floatParam.at(0), this->m_floatParam.at(1), this->m_floatParam.at(2),
						m_GraphHandleParam.at(0)->get(),this->m_boolParam.at(0), this->m_boolParam.at(1)
					);
				}
				break;
			default:
				break;
			}
		}
		//
		DrawControl::DrawControl(void) noexcept {
			this->m_DrawDatas.resize(static_cast<size_t>(DrawLayer::Max));
			this->m_PrevDrawDatas.resize(static_cast<size_t>(DrawLayer::Max));

			auto* WindowSizeParts = WindowSizeControl::Instance();
			m_BufferScreen.Make(WindowSizeParts->GetSizeXMax(), WindowSizeParts->GetSizeYMax(), true);
		}
		bool DrawControl::IsDrawOnWindow(int x1, int y1, int x2, int y2) noexcept {
			return HitRectangleToRectangle(0, 0, BaseScreenWidth, BaseScreenHeight, std::min(x1, x2), std::min(y1, y2), std::max(x1, x2), std::max(y1, y2));
		}
		void DrawControl::ClearList(void) noexcept {
			for (size_t index = 0; auto & d : this->m_DrawDatas) {
				auto& pd = this->m_PrevDrawDatas.at(index);
				pd.clear();
				for (auto& d2 : d) {
					pd.emplace_back(d2);
				}
				index++;
			}
			for (auto& d : this->m_DrawDatas) {
				d.clear();
			}
		}
		void DrawControl::Draw(void) noexcept {
			bool IsHit = false;
			// 同じかどうかチェック
			for (size_t index = 0; auto & d : this->m_DrawDatas) {
				auto& pd = this->m_PrevDrawDatas.at(index);
				if (pd.size() == d.size()) {
					for (size_t index2 = 0; auto & d2 : d) {
						auto& pd2 = pd.at(index2);
						if (!(pd2 == d2)) {
							IsHit = true;
							break;
						}
						index2++;
					}
				}
				else {
					IsHit = true;
					break;
				}
				index++;
			}
			// 
			if (IsHit) {
				auto NowScreen = GetDrawScreen();
				m_BufferScreen.SetDraw_Screen(true);
				{
					for (auto& d : this->m_DrawDatas) {
						for (auto& d2 : d) {
							d2.Output();
						}
						DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
						DxLib::SetDrawBright(255, 255, 255);
					}
				}
				GraphHandle::SetDraw_Screen(NowScreen, false);
			}
			// 前に描画したものをそのまま出す
			m_BufferScreen.DrawGraph(0, 0, true);
		}
		void DrawControl::Dispose(void) noexcept {
			for (auto& d : this->m_DrawDatas) {
				d.clear();
			}
			this->m_DrawDatas.clear();

			for (auto& d : this->m_PrevDrawDatas) {
				d.clear();
			}
			this->m_PrevDrawDatas.clear();
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
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		int xp1, yp1;
		xp1 = (64);
		yp1 = (256);

		{
			int yp = yp1 - (static_cast<int>(24.f * 0.f));
			if (SelYadd > 0.f) {
				float per = std::clamp(SelYadd / 5.f, 0.f, 1.f);
				float per2 = 1.f - std::clamp(SelYadd / 10.f, 0.f, 1.f);
				DrawCtrls->SetAdd(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * per), 0, 255));
				DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal,
					&this->m_SelectBackImage,
					xp1 + (200 / 2) - (int)((float)((150)) * per2), yp + LineHeight / 2 - (int)((float)(LineHeight * 4) * per2),
					xp1 + (200 / 2) + (int)((float)((150)) * per2), yp + LineHeight / 2 + (int)((float)(LineHeight * 4) * per2),
					true);
			}
		}

		for (auto& d : data) {
			if (d.ActivePer() > 0.f) {
				DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, std::clamp(static_cast<int>(255.f * d.ActivePer()), 0, 255));
				int yp = yp1 - (static_cast<int>(24.f * d.GetFlip()));
				int FontLen = FontSystem::FontPool::Instance()->Get(FontSystem::FontType::MS_Gothic, LineHeight, 3)->GetStringWidth(d.GetMsg());
				DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal,
					xp1 - 6, yp + LineHeight,
					xp1 - 6 + static_cast<int>(static_cast<float>(std::max(FontLen, 200)) * d.ActivePer()), yp + LineHeight + 5,
					Black, true);
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp1, yp, d.GetMsgColor(), Black, d.GetMsg());
			}
		}
		DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
	}
	// 
	void PopUp::PopUpDrawClass::Start(void) noexcept {
		auto* SE = SoundPool::Instance();
		auto* KeyGuideParts = KeyGuide::Instance();

		KeyGuideParts->SetGuideFlip();
		KeyGuideParts->ChangeGuide(
			[this]() {
				auto* KeyGuideParts = KeyGuide::Instance();
				auto* LocalizeParts = LocalizePool::Instance();
				KeyGuideParts->AddGuide(KeyGuide::GetPADStoOffset(PADS::RELOAD), LocalizeParts->Get(9991));
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
			if (Pad->GetPadsInfo(PADS::RELOAD).GetKey().trigger()) {
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
	//
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
	int KeyGuide::KeyGuideGraph::GetDrawSize(void) const noexcept { return (xsize * 24 / ysize) + 3; }
	void KeyGuide::KeyGuideGraph::Draw(int x, int y) const noexcept {
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		DrawCtrls->SetDrawExtendGraph(WindowSystem::DrawLayer::Normal, &GuideImg, x, y, x + (xsize * 24 / ysize), y + 24, true);
	}
	// 
	int KeyGuide::KeyGuideOnce::GetDrawSize(void) const noexcept {
		int ofs = (m_GuideGraph) ? m_GuideGraph->GetDrawSize() : 0;
		if (GuideString != "") {
			ofs += FontSystem::FontPool::Instance()->Get(FontSystem::FontType::MS_Gothic, LineHeight, 3)->GetStringWidth(GuideString) + 12;
		}
		return ofs;
	}
	void KeyGuide::KeyGuideOnce::Draw(int x, int y) const noexcept {
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		int ofs = 0;
		if (m_GuideGraph) {
			m_GuideGraph->Draw(x, y);
			ofs = m_GuideGraph->GetDrawSize();
		}
		if (GuideString != "") {
			DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic,
				LineHeight, FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::MIDDLE,
				x + ofs, y + 24 / 2,
				White, Black, GuideString);
		}
	}
	// 
	KeyGuide::KeyGuide(void) noexcept {
		SetGuideFlip();
		m_GuideBaseImage.Load("CommonData/key/OutputFont.png");
		{
			int count = 0;
			FileStreamDX FileStream("CommonData/key/OutputFont.psf");
			while (true) {
				if (FileStream.ComeEof()) { break; }
				auto ALL = FileStream.SeekLineAndGetStr();
				if (ALL == "") { continue; }
				//=の右側の文字をカンマ区切りとして識別する
				auto RIGHT = FileStreamDX::getright(ALL);
				std::vector<int> Args;
				while (true) {
					auto div = RIGHT.find(",");
					if (div != std::string::npos) {
						Args.emplace_back(std::stoi(RIGHT.substr(0, div)));
						RIGHT = RIGHT.substr(div + 1);
					}
					else {
						Args.emplace_back(std::stoi(RIGHT));
						break;
					}
				}
				//得た情報をもとに分割した画像を得る
				m_DerivationGuideImage.emplace_back(std::make_shared<KeyGuideGraph>());
				m_DerivationGuideImage.back()->AddGuide(Args.at(0), Args.at(1), Args.at(2), Args.at(3), m_GuideBaseImage);
				++count;
			}
		}
	}
	void KeyGuide::ChangeGuide(std::function<void()>Guide_Pad) noexcept {
		if (m_IsFlipGuide) {
			m_IsFlipGuide = false;
			Dispose();
			// 絶対出すガイド
			auto* Pad = PadControl::Instance();
			auto* LocalizeParts = LocalizePool::Instance();
			AddGuide(GetIDtoOffset(Pad->GetPadsInfo(PADS::Escape).GetAssign(), ControlType::PC), LocalizeParts->Get(9990));
			AddGuide(GetPADStoOffset(PADS::INVENTORY), LocalizeParts->Get(9995));
			// 追加のガイド
			Guide_Pad();
		}
	}
	void KeyGuide::Draw(void) const noexcept {
		int x = 32;
		int y = BaseScreenHeight - (21 + 16);
		for (const auto& k : m_Key) {
			k->Draw(x, y);
			x += k->GetDrawSize();
			//次の行へ移行
			if (x > BaseScreenWidth / 2) {
				x = 32;
				y -= 28;
			}
		}
	}
};
