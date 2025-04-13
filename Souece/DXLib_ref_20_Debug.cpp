#include "DXLib_ref_20_Debug.hpp"

namespace DXLibRef {
#if defined(DEBUG)
	// --------------------------------------------------------------------------------------------------
	// ÉVÉìÉOÉãÉgÉì
	// --------------------------------------------------------------------------------------------------
	const DebugClass* SingletonBase<DebugClass>::m_Singleton = nullptr;


	DebugClass::DebugClass(void) noexcept {
		m_IsActive = true;
	}

	void DebugClass::SetStartPoint(void) noexcept {
		if (!m_IsActive) {
			return;
		}
		m_StartTime = GetNowHiPerformanceCount();
		m_PointSel = 0;
		SetPoint("-----Start-----");
	}
	void DebugClass::SetPoint(const char* DebugMes) noexcept {
		if (!m_IsActive) {
			return;
		}
		if (m_PointSel < PointMax) {
			m_Point.at(0)[m_PointSel] = static_cast<float>(GetNowHiPerformanceCount() - m_StartTime) / 1000.0f;
			m_Str[m_PointSel] = DebugMes;
			++m_PointSel;
			return;
		}
	}
	void DebugClass::SetEndPoint(void) noexcept {
		auto* DXLib_refParts = DXLib_ref::Instance();
		m_Switch.Update(CheckHitKey(KEY_INPUT_F1) != 0);
		if (m_Switch.trigger()) {
			m_IsActive ^= 1;
		}
		if (!m_IsActive) {
			return;
		}
		auto PMax = PointMax + 1;
		// ç≈å„ÇÃââéZ
		SetPoint("-----End-----");
		for (size_t loop : std::views::iota(0, PMax)) {
			if (m_PointSel <= loop) {
				m_Point[0][loop] = m_Point[0][m_PointSel - 1];
			}
			if (loop == PointMax) {
				m_Point[0][loop] = 1000.0f * DXLib_refParts->GetDeltaTime();
			}

			for (int loop2 = static_cast<int>(PointFrame - 1); loop2 >= 1; --loop2) {
				m_Point[static_cast<size_t>(loop2)][loop] = m_Point[static_cast<size_t>(loop2 - 1)][loop];
			}

			// ïΩãœ
			m_Point[PointFrame][loop] = 0.f;
			for (size_t loop2 : std::views::iota(static_cast<size_t>(0), PointFrame)) {
				m_Point[PointFrame][loop] += m_Point[loop2][loop];
			}
			m_Point[PointFrame][loop] /= PointFrame;
		}
		for (int loop = PMax - 1; loop >= 1; --loop) {
			if (loop > 0) {
				m_Point[PointFrame][static_cast<size_t>(loop)] = m_Point[PointFrame][static_cast<size_t>(loop)] - m_Point[PointFrame][static_cast<size_t>(loop - 1)];
			}
		}

		for (size_t loop2 : std::views::iota(0, PMax)) {
			m_PointP[0][loop2] = 0;
			for (size_t loop = 0; loop <= loop2; ++loop) {
				m_PointP[0][loop2] += m_Point[PointFrame][loop];
			}
			for (int loop3 = static_cast<int>(PointFrame - 1); loop3 >= 1; --loop3) {
				m_PointP[static_cast<size_t>(loop3)][loop2] = m_PointP[static_cast<size_t>(loop3 - 1)][loop2];
			}

		}
	}

	void DebugClass::DebugWindow(int xpos, int ypos) noexcept {
		auto* OptionParts = OptionManager::Instance();
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		if (!m_IsActive) {
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
			// îwåi
			DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xpos, ypos, xpos + wide, ypos + height, White, true);
			DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xpos + 1, ypos + 1, xpos + wide - 1, ypos + height - 1, Black, true);

			{
				const int xp = xpos;
				const int yp = ypos;
				// ì‡óe
				int value = OptionParts->GetParamInt(EnumSaveParam::FpsLimit);
				const float xs = static_cast<float>(wide) / PointFrame;
				const float ys = static_cast<float>(border) / (1000.0f / static_cast<float>(value));

				const int ye = yp + height;
				for (size_t loop2 = PointFrame - 1 - 1; loop2 > 0; --loop2) {
					int xnow = xp + static_cast<int>(static_cast<float>(loop2) * xs);
					int xnext = xp + static_cast<int>(static_cast<float>(loop2 + 1) * xs);

					for (size_t loop = static_cast<size_t>(PMax - 1); loop > 0; --loop) {
						int ynow = std::max(yp, ye - static_cast<int>(m_PointP[loop2][loop] * ys));
						int ynext = std::max(yp, ye - static_cast<int>(m_PointP[loop2 + 1][loop] * ys));
						DrawCtrls->SetDrawQuadrangle(
							WindowSystem::DrawLayer::Normal,
							xnow, ynow, xnext, ynext,
							xnext, ye, xnow, ye,
							Colors[loop],
							TRUE);
					}
					/*
					for (int loop : std::views::iota(0, PMax)) {
						int ynow = std::max(yp, ye - static_cast<int>(m_PointP[static_cast<size_t>(loop2)][static_cast<size_t>(loop)] * ys));
						int ynext = std::max(yp, ye - static_cast<int>(m_PointP[static_cast<size_t>(loop2 + 1)][static_cast<size_t>(loop)] * ys));
						DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xnow, ynow, xnext, ynext, Gray75);
					}
					//*/
				}
				DrawCtrls->SetDrawLine(WindowSystem::DrawLayer::Normal, xp, ye - border, xp + wide, ye - border, White);// äÓèÄê¸
			}
			ypos += height;
		}
		{
			const int wide = (350);
			const int height = static_cast<int>(m_PointSel + 3 + 1) * LineHeight + (10);
			// îwåi
			DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xpos, ypos, xpos + wide, ypos + height, White, true);
			DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, xpos + 1, ypos + 1, xpos + wide - 1, ypos + height - 1, Black, true);

			xpos += (2);
			ypos += (2);
			int num = 0;
			// ì‡óe
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
			for (size_t loop : std::views::iota(1, static_cast<int>(m_PointSel + 1))) {
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xpos, ypos + (num * LineHeight),
					Colors[loop], DarkGreen, "%02d(%5.2fms)[%s]", loop, m_Point[PointFrame][loop], m_Str[loop - 1].c_str());
				++num;
			}
			{
				size_t loop = static_cast<size_t>(PointMax);
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xpos, ypos + (num * LineHeight),
					Colors[loop], DarkGreen, "%02d(%5.2fms)[%s]", loop, m_Point[PointFrame][loop], m_Str[loop - 1].c_str());
				++num;
			}
		}
	}
#endif // DEBUG
};
