#include "DXLib_ref_12_Draw2DSystem.hpp"
//#include "DXLib_ref.h"

namespace DXLibRef {
	const WindowSystem::DrawControl* SingletonBase<WindowSystem::DrawControl>::m_Singleton = nullptr;
	const SideLog* SingletonBase<SideLog>::m_Singleton = nullptr;
	const KeyGuide* SingletonBase<KeyGuide>::m_Singleton = nullptr;
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
			auto* WindowSizeParts = WindowSizeControl::Instance();
			m_BufferScreen.Make(WindowSizeParts->GetSizeXMax(), WindowSizeParts->GetSizeYMax(), true);
			Dispose();
		}
		void DrawControl::Draw(void) noexcept {
			bool IsHit = false;
			// 同じかどうかチェック
			for (auto & d : this->m_DrawDatas.at(this->m_DrawNow)) {
				int index = static_cast<int>(&d - &this->m_DrawDatas.at(this->m_DrawNow).front());
				auto& pd = this->m_DrawDatas.at(static_cast<size_t>(1 - this->m_DrawNow)).at(index);
				if (pd.second != d.second) {
					IsHit = true;
					break;
				}
				for (int loop = 0; loop < d.second; ++loop) {
					if (pd.first.at(loop) != pd.first.at(loop)) {
						IsHit = true;
						break;
					}
				}
				if (IsHit) {
					break;
				}
			}
			// 内容が同じならスルー ちがうならバッファーを更新
			if (IsHit) {
				auto NowScreen = GetDrawScreen();
				m_BufferScreen.SetDraw_Screen(true);
				{
					for (auto& d : this->m_DrawDatas.at(this->m_DrawNow)) {
						for (int loop = 0; loop < d.second; ++loop) {
							auto& d2 = d.first.at(loop);
							d2.Output();
						}
						DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
						DxLib::SetDrawBright(255, 255, 255);
					}
				}
				GraphHandle::SetDraw_Screen(NowScreen, false);
			}
			// バッファーを出力
			m_BufferScreen.DrawGraph(0, 0, true);
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
				std::array<int, 4> Args;
				int now = 0;
				while (true) {
					auto div = RIGHT.find(",");
					if (div != std::string::npos) {
						Args.at(now) = std::stoi(RIGHT.substr(0, div));
						++now;
						RIGHT = RIGHT.substr(div + 1);
					}
					else {
						Args.at(now) = std::stoi(RIGHT);
						++now;
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
			auto* LocalizeParts = LocalizePool::Instance();
			AddGuide(GetPADStoOffset(Controls::PADS::Escape), LocalizeParts->Get(9990));
			AddGuide(GetPADStoOffset(Controls::PADS::INVENTORY), LocalizeParts->Get(9995));
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
