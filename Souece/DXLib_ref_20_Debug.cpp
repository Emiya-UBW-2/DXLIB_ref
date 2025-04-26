#include "DXLib_ref_20_Debug.hpp"

namespace DXLibRef {
#if defined(DEBUG)
	// --------------------------------------------------------------------------------------------------
	// シングルトン
	// --------------------------------------------------------------------------------------------------
	const DebugDraw* SingletonBase<DebugDraw>::m_Singleton = nullptr;


	DebugDraw::DebugDraw(void) noexcept {
		this->m_IsActive = true;
	}

	void DebugDraw::SetStartPoint(void) noexcept {
		if (!this->m_IsActive) {
			return;
		}
		this->m_StartTime = GetNowHiPerformanceCount();
		this->m_PointSel = 0;
		SetPoint("-----Start-----");
	}
	void DebugDraw::SetPoint(const char* DebugMes) noexcept {
		if (!this->m_IsActive) {
			return;
		}
		if (this->m_PointSel < PointMax) {
			this->m_Point[0][m_PointSel] = static_cast<float>(GetNowHiPerformanceCount() - this->m_StartTime) / 1000.0f;
			this->m_Str[m_PointSel] = DebugMes;
			++m_PointSel;
			return;
		}
	}
	void DebugDraw::SetEndPoint(void) noexcept {
		auto* DXLib_refParts = DXLib_ref::Instance();
		this->m_Switch.Update(CheckHitKey(KEY_INPUT_F1) != 0);
		if (this->m_Switch.trigger()) {
			this->m_IsActive ^= 1;
		}
		if (!this->m_IsActive) {
			return;
		}
		auto PMax = PointMax + 1;
		// 最後の演算
		SetPoint("-----End-----");
		for (size_t loop : std::views::iota(static_cast<size_t>(0), PMax)) {
			if (this->m_PointSel <= loop) {
				this->m_Point[0][loop] = this->m_Point[0][m_PointSel - 1];
			}
			if (loop == PointMax) {
				this->m_Point[0][loop] = 1000.0f * DXLib_refParts->GetDeltaTime();
			}

			for (size_t loop2 = PointFrame - 1; loop2 >= 1; --loop2) {
				this->m_Point[loop2][loop] = this->m_Point[loop2 - 1][loop];
			}

			// 平均
			this->m_Point[PointFrame][loop] = 0.f;
			for (size_t loop2 : std::views::iota(static_cast<size_t>(0), PointFrame)) {
				this->m_Point[PointFrame][loop] += this->m_Point[loop2][loop];
			}
			this->m_Point[PointFrame][loop] /= PointFrame;
		}
		for (size_t loop = PMax - 1; loop >= 1; --loop) {
			this->m_Point[PointFrame][loop] = this->m_Point[PointFrame][loop] - this->m_Point[PointFrame][loop - 1];
		}

		for (size_t loop2 : std::views::iota(static_cast<size_t>(0), PMax)) {
			this->m_PointP[0][loop2] = 0;
			for (size_t loop = 0; loop <= loop2; ++loop) {
				this->m_PointP[0][loop2] += this->m_Point[PointFrame][loop];
			}
			for (size_t loop3 = PointFrame - 1; loop3 >= 1; --loop3) {
				this->m_PointP[loop3][loop2] = this->m_PointP[loop3 - 1][loop2];
			}

		}
	}

	void DebugDraw::DebugWindow(int xpos, int ypos) noexcept {
		auto* OptionParts = OptionManager::Instance();
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		if (!this->m_IsActive) {
			return;
		}
		const unsigned int Colors[PointMax + 1] = {
				Red,
				Yellow,
				Green,
				GetColorFix(0,255,255),
				GetColorFix(100,100,255),
				GetColorFix(255, 0,255),
				Red,
				Yellow,
				Green,
				GetColorFix(0,255,255),
				GetColorFix(100,100,255),
				GetColorFix(255, 0,255),
				Red,
		};
		auto PMax = PointMax + 1;
		{
			const int wide = (340);
			const int height = (360);
			const int border = height * 2 / 3;
			// 背景
			DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xpos, ypos, xpos + wide, ypos + height, White, true);
			DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xpos + 1, ypos + 1, xpos + wide - 1, ypos + height - 1, Black, true);

			{
				// 内容
				float FPSLimit = static_cast<float>(OptionParts->GetParamInt(EnumSaveParam::FpsLimit));
				const float xsize = static_cast<float>(wide) / PointFrame;
				const float ysize = static_cast<float>(border) / (1000.0f / FPSLimit);

				const int yend = ypos + height;
				for (size_t loop2 = PointFrame - 1 - 1; loop2 > 0; --loop2) {
					int xnow = xpos + static_cast<int>(static_cast<float>(loop2) * xsize);
					int xnext = xpos + static_cast<int>(static_cast<float>(loop2 + 1) * xsize);

					for (size_t loop = static_cast<size_t>(PMax - 1); loop > 0; --loop) {
						int ynow = std::max(ypos, yend - static_cast<int>(this->m_PointP[loop2][loop] * ysize));
						int ynext = std::max(ypos, yend - static_cast<int>(this->m_PointP[loop2 + 1][loop] * ysize));
						DrawCtrls->SetDrawQuadrangle(
							WindowSystem::DrawLayer::Normal,
							xnow, ynow, xnext, ynext,
							xnext, yend, xnow, yend,
							Colors[loop],
							TRUE);
					}
					/*
					for (int loop : std::views::iota(0, PMax)) {
						int ynow = std::max(ypos, yend - static_cast<int>(this->m_PointP[static_cast<size_t>(loop2)][static_cast<size_t>(loop)] * ysize));
						int ynext = std::max(ypos, yend - static_cast<int>(this->m_PointP[static_cast<size_t>(loop2 + 1)][static_cast<size_t>(loop)] * ysize));
						DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xnow, ynow, xnext, ynext, Gray75);
					}
					//*/
				}
				DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xpos, yend - border, xpos + wide, yend - border, White);// 基準線
			}
			ypos += height;
		}
		{
			const int wide = (350);
			const int height = static_cast<int>(this->m_PointSel + 3 + 1) * LineHeight + (10);
			// 背景
			DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xpos, ypos, xpos + wide, ypos + height, White, true);
			DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xpos + 1, ypos + 1, xpos + wide - 1, ypos + height - 1, Black, true);

			xpos += (2);
			ypos += (2);
			int num = 0;
			// 内容
			DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
				FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xpos, ypos + (num * LineHeight),
				White, Black, "AsyncCount :%d", GetASyncLoadNum());
			++num;
			DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
				FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xpos, ypos + (num * LineHeight),
				White, Black, "Drawcall  :%d", GetDrawCallCount());
			++num;
			DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
				FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xpos, ypos + (num * LineHeight),
				White, Black, "FPS    :%5.2f fps", GetFPS());
			++num;
			for (size_t loop : std::views::iota(1, static_cast<int>(this->m_PointSel + 1))) {
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xpos, ypos + (num * LineHeight),
					Colors[loop], DarkGreen, "%02d(%5.2fms)[%s]", loop, this->m_Point[PointFrame][loop], this->m_Str[loop - 1].c_str());
				++num;
			}
			{
				size_t loop = PointMax;
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xpos, ypos + (num * LineHeight),
					Colors[loop], DarkGreen, "%02d(%5.2fms)[%s]", loop, this->m_Point[PointFrame][loop], this->m_Str[loop - 1].c_str());
				++num;
			}
		}
	}
#endif // DEBUG
};
