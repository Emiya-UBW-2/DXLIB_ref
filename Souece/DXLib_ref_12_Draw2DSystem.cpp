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
			switch (this->m_type) {
			case DrawType::Alpha:
				if (this->m_intParam[0] < 255) {
					DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, this->m_intParam[0]);
				}
				else {
					DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, this->m_intParam[0]);
				}
				break;
			case DrawType::Add:
				DxLib::SetDrawBlendMode(DX_BLENDMODE_ADD, this->m_intParam[0]);
				break;
			case DrawType::Bright:
				DxLib::SetDrawBright(this->m_intParam[0], this->m_intParam[1], this->m_intParam[2]);
				break;
			case DrawType::Box:
				if (this->m_intParam[4] == 1 || (this->m_boolParam[0])) {
					DxLib::DrawBox(
						WindowSizeParts->GetUIY(this->m_intParam[0]), WindowSizeParts->GetUIY(this->m_intParam[1]),
						WindowSizeParts->GetUIY(this->m_intParam[2]), WindowSizeParts->GetUIY(this->m_intParam[3]),
						this->m_UintParam[0], (this->m_boolParam[0]) ? TRUE : FALSE);
				}
				else {
					int p1x = WindowSizeParts->GetUIY(this->m_intParam[0]);
					int p1y = WindowSizeParts->GetUIY(this->m_intParam[1]);
					int p2x = WindowSizeParts->GetUIY(this->m_intParam[2]);
					int p2y = WindowSizeParts->GetUIY(this->m_intParam[3]);
					DxLib::DrawLine(p1x, p1y, p1x, p2y, this->m_UintParam[0], this->m_intParam[4]);
					DxLib::DrawLine(p1x, p1y, p2x, p1y, this->m_UintParam[0], this->m_intParam[4]);
					DxLib::DrawLine(p1x, p2y, p2x, p2y, this->m_UintParam[0], this->m_intParam[4]);
					DxLib::DrawLine(p2x, p1y, p2x, p2y, this->m_UintParam[0], this->m_intParam[4]);
				}
				break;
			case DrawType::Quadrangle:
				DxLib::DrawQuadrangle(
					WindowSizeParts->GetUIY(this->m_intParam[0]), WindowSizeParts->GetUIY(this->m_intParam[1]),
					WindowSizeParts->GetUIY(this->m_intParam[2]), WindowSizeParts->GetUIY(this->m_intParam[3]),
					WindowSizeParts->GetUIY(this->m_intParam[4]), WindowSizeParts->GetUIY(this->m_intParam[5]),
					WindowSizeParts->GetUIY(this->m_intParam[6]), WindowSizeParts->GetUIY(this->m_intParam[7]),
					this->m_UintParam[0], (this->m_boolParam[0]) ? TRUE : FALSE);
				break;
			case DrawType::Circle:
				DxLib::DrawCircle(
					WindowSizeParts->GetUIY(this->m_intParam[0]), WindowSizeParts->GetUIY(this->m_intParam[1]), WindowSizeParts->GetUIY(this->m_intParam[2]),
					this->m_UintParam[0], (this->m_boolParam[0]) ? TRUE : FALSE,
					(this->m_intParam[3] >= 2) ? WindowSizeParts->GetUIY(this->m_intParam[3]) : this->m_intParam[3]);
				break;
			case DrawType::Line:
				DxLib::DrawLine(WindowSizeParts->GetUIY(this->m_intParam[0]), WindowSizeParts->GetUIY(this->m_intParam[1]), WindowSizeParts->GetUIY(this->m_intParam[2]), WindowSizeParts->GetUIY(this->m_intParam[3]), this->m_UintParam[0],
					(this->m_intParam[4] >= 2) ? WindowSizeParts->GetUIY(this->m_intParam[4]) : this->m_intParam[4]);
				break;
			case DrawType::String:
				FontSystem::FontPool::Instance()->Get((FontSystem::FontType)m_intParam[0], WindowSizeParts->GetUIY(this->m_intParam[1]), 3)->DrawString(
						(FontSystem::FontXCenter)m_intParam[2], (FontSystem::FontYCenter)m_intParam[3],
						WindowSizeParts->GetUIY(this->m_intParam[4]), WindowSizeParts->GetUIY(this->m_intParam[5]),
						this->m_UintParam[0],
						this->m_UintParam[1],
						this->m_string.c_str()
					);
				break;
			case DrawType::StringAutoFit:
				FontSystem::FontPool::Instance()->Get((FontSystem::FontType)m_intParam[0], WindowSizeParts->GetUIY(this->m_intParam[1]), 3)->DrawStringAutoFit(
					WindowSizeParts->GetUIY(this->m_intParam[2]), WindowSizeParts->GetUIY(this->m_intParam[3]),
						WindowSizeParts->GetUIY(this->m_intParam[4]), WindowSizeParts->GetUIY(this->m_intParam[5]),
						this->m_UintParam[0],
						this->m_UintParam[1],
						this->m_string.c_str()
					);
				break;
			case DrawType::RotaGraph:
				if (this->m_GraphHandleParam[0]) {
					float Scale = (float)(WindowSizeParts->GetUIY((int)(this->m_floatParam[0] * 10000))) / 10000.f;

					if (Scale < 0.95f && 1.05f < Scale) {
						auto prev = GetDrawMode();
						DxLib::SetDrawMode(DX_DRAWMODE_BILINEAR);
						this->m_GraphHandleParam[0]->DrawRotaGraph(
							WindowSizeParts->GetUIY(this->m_intParam[0]), WindowSizeParts->GetUIY(this->m_intParam[1]),
							Scale, this->m_floatParam[1], this->m_boolParam[0]);
						DxLib::SetDrawMode(prev);
					}
					else {
						this->m_GraphHandleParam[0]->DrawRotaGraph(
							WindowSizeParts->GetUIY(this->m_intParam[0]), WindowSizeParts->GetUIY(this->m_intParam[1]),
							Scale, this->m_floatParam[1], this->m_boolParam[0]);
					}
				}
				break;
			case DrawType::ExtendGraph:
				if (this->m_GraphHandleParam[0]) {
					auto prev = GetDrawMode();
					DxLib::SetDrawMode(DX_DRAWMODE_BILINEAR);
					this->m_GraphHandleParam[0]->DrawExtendGraph(WindowSizeParts->GetUIY(this->m_intParam[0]), WindowSizeParts->GetUIY(this->m_intParam[1]), WindowSizeParts->GetUIY(this->m_intParam[2]), WindowSizeParts->GetUIY(this->m_intParam[3]), this->m_boolParam[0]);
					DxLib::SetDrawMode(prev);
				}
				break;
			case DrawType::CircleGauge:
				if (this->m_GraphHandleParam[0]) {
					DxLib::DrawCircleGauge(WindowSizeParts->GetUIY(this->m_intParam[0]), WindowSizeParts->GetUIY(this->m_intParam[1]),
						(double)this->m_floatParam[0],
						this->m_GraphHandleParam[0]->get(),
						(double)this->m_floatParam[1],
						(double)(this->m_floatParam[2] * (float)(WindowSizeParts->GetUIY(100)) / 100.f));
				}
				break;
			case DrawType::NineSliceGraph:
				if (this->m_GraphHandleParam[0]) {
					Draw9SliceGraph(
						WindowSizeParts->GetUIY(this->m_intParam[0]), WindowSizeParts->GetUIY(this->m_intParam[1]),
						WindowSizeParts->GetUIY(this->m_intParam[2]), WindowSizeParts->GetUIY(this->m_intParam[3]),
						WindowSizeParts->GetUIY(this->m_intParam[4]), WindowSizeParts->GetUIY(this->m_intParam[5]),
						WindowSizeParts->GetUIY(this->m_intParam[6]), WindowSizeParts->GetUIY(this->m_intParam[7]),
						this->m_floatParam[0], this->m_floatParam[1], this->m_floatParam[2],
						this->m_GraphHandleParam[0]->get(),this->m_boolParam[0], this->m_boolParam[1]
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
			this->m_BufferScreen.Make(WindowSizeParts->GetSizeXMax(), WindowSizeParts->GetSizeYMax(), true);
			Dispose();
		}
		void DrawControl::Draw(void) noexcept {
			bool IsHit = false;
			// 同じかどうかチェック
			for (auto & d : this->m_DrawDatas[this->m_DrawNow]) {
				int index = static_cast<int>(&d - &this->m_DrawDatas[this->m_DrawNow].front());
				auto& pd = this->m_DrawDatas[static_cast<size_t>(1 - this->m_DrawNow)][index];
				if (pd.second != d.second) {
					IsHit = true;
					break;
				}
				for (int loop = 0; loop < d.second; ++loop) {
					if (pd.first[loop] != pd.first[loop]) {
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
				this->m_BufferScreen.SetDraw_Screen(true);
				{
					for (auto& d : this->m_DrawDatas[this->m_DrawNow]) {
						for (int loop = 0; loop < d.second; ++loop) {
							auto& d2 = d.first[loop];
							d2.Output();
						}
						DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
						DxLib::SetDrawBright(255, 255, 255);
					}
				}
				GraphHandle::SetDraw_Screen(NowScreen, false);
			}
			// バッファーを出力
			this->m_BufferScreen.DrawGraph(0, 0, true);
		}
	};
	// 
	void SideLog::SideLogData::UpdateActive(void) noexcept {
		auto* DXLib_refParts = DXLib_ref::Instance();
		if (this->m_Time > 0.f) {
			this->m_Time -= DXLib_refParts->GetDeltaTime();
		}
		else {
			this->m_Time = -1.f;
		}
		Easing(&this->m_Flip_Y, this->m_Flip, 0.9f, EasingType::OutExpo);
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

		for (auto& d : this->m_List) {
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
		int ofs = (this->m_GuideGraph) ? this->m_GuideGraph->GetDrawSize() : 0;
		if (GuideString != "") {
			ofs += FontSystem::FontPool::Instance()->Get(FontSystem::FontType::MS_Gothic, LineHeight, 3)->GetStringWidth(GuideString) + 12;
		}
		return ofs;
	}
	void KeyGuide::KeyGuideOnce::Draw(int x, int y) const noexcept {
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		int ofs = 0;
		if (this->m_GuideGraph) {
			this->m_GuideGraph->Draw(x, y);
			ofs = this->m_GuideGraph->GetDrawSize();
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
		this->m_GuideBaseImage.Load("CommonData/key/OutputFont.png");
		{
			int count = 0;
			FileStreamDX FileStream("CommonData/key/OutputFont.psf");
			while (true) {
				if (FileStream.ComeEof()) { break; }
				auto ALL = FileStream.SeekLineAndGetStr();
				if (ALL == "") { continue; }
				//=の右側の文字をカンマ区切りとして識別する
				auto RIGHT = FileStreamDX::getright(ALL);
				std::array<int, 4> Args{};
				int now = 0;
				while (true) {
					auto div = RIGHT.find(",");
					if (div != std::string::npos) {
						Args[now] = std::stoi(RIGHT.substr(0, div));
						++now;
						RIGHT = RIGHT.substr(div + 1);
					}
					else {
						Args[now] = std::stoi(RIGHT);
						++now;
						break;
					}
				}
				//得た情報をもとに分割した画像を得る
				this->m_DerivationGuideImage.emplace_back(std::make_shared<KeyGuideGraph>());
				this->m_DerivationGuideImage.back()->AddGuide(Args[0], Args[1], Args[2], Args[3], this->m_GuideBaseImage);
				++count;
			}
		}
	}
	void KeyGuide::ChangeGuide(std::function<void()>Guide_Pad) noexcept {
		if (this->m_IsFlipGuide) {
			this->m_IsFlipGuide = false;
			Dispose();
			// 絶対出すガイド
			auto* SceneParts = SceneControl::Instance();
			if (SceneParts->IsPauseEnable()) {
				auto* LocalizeParts = LocalizePool::Instance();
				AddGuide(GetPADStoOffset(Controls::PADS::Escape), LocalizeParts->Get(9990));
				AddGuide(GetPADStoOffset(Controls::PADS::INVENTORY), LocalizeParts->Get(9995));
			}
			// 追加のガイド
			Guide_Pad();
		}
	}
	void KeyGuide::Draw(void) const noexcept {
		int x = 32;
		int y = BaseScreenHeight - (21 + 16);
		for (const auto& k : this->m_Key) {
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
