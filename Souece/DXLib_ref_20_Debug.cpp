#include "DXLib_ref_20_Debug.hpp"

namespace DXLibRef {
#if defined(DEBUG)
	// --------------------------------------------------------------------------------------------------
	// ÉVÉìÉOÉãÉgÉì
	// --------------------------------------------------------------------------------------------------
	const DebugClass* SingletonBase<DebugClass>::m_Singleton = nullptr;


	DebugClass::DebugClass(void) noexcept {
		m_Point.resize(static_cast<size_t>(PointFrame + 1));
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
			m_PointSel++;
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
		for (int index : std::views::iota(0, PMax)) {
			if (!(static_cast<int>(m_PointSel) > index)) {
				m_Point.at(0)[static_cast<std::size_t>(index)] = m_Point.at(0)[m_PointSel - 1];
			}
			if (index == PointMax) {
				m_Point.at(0)[static_cast<std::size_t>(index)] = 1000.0f * DXLib_refParts->GetDeltaTime();
			}

			for (int j = static_cast<int>(PointFrame - 1); j >= 1; --j) {
				m_Point[static_cast<std::size_t>(j)][static_cast<std::size_t>(index)] = m_Point[static_cast<std::size_t>(j - 1)][static_cast<std::size_t>(index)];
			}

			// ïΩãœ
			m_Point[static_cast<std::size_t>(PointFrame)][static_cast<std::size_t>(index)] = 0.f;
			for (int j : std::views::iota(0, PointFrame)) {
				m_Point[static_cast<std::size_t>(PointFrame)][static_cast<std::size_t>(index)] += m_Point[static_cast<std::size_t>(j)][static_cast<std::size_t>(index)];
			}
			m_Point[static_cast<std::size_t>(PointFrame)][static_cast<std::size_t>(index)] /= PointFrame;
		}
		for (int index = PMax - 1; index >= 1; --index) {
			if (index > 0) {
				m_Point[static_cast<std::size_t>(PointFrame)][static_cast<std::size_t>(index)] = m_Point[static_cast<std::size_t>(PointFrame)][static_cast<std::size_t>(index)] - m_Point[static_cast<std::size_t>(PointFrame)][static_cast<std::size_t>(index) - 1];
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
				for (size_t j = static_cast<size_t>(PointFrame - 1 - 1); j > 0; --j) {
					int xnow = xp + static_cast<int>(static_cast<float>(j) * xs);
					int xnext = xp + static_cast<int>(static_cast<float>(j + 1) * xs);

					for (size_t index = static_cast<size_t>(PMax - 1); index > 0; --index) {
						int ynow = std::max(yp, ye - static_cast<int>(m_Point[j][index] * ys));
						int ynext = std::max(yp, ye - static_cast<int>(m_Point[j + 1][index] * ys));
						DrawCtrls->SetDrawQuadrangle(
							WindowSystem::DrawLayer::Normal,
							xnow, ynow, xnext, ynext,
							xnext, ye, xnow, ye,
							Colors[index],
							TRUE);
					}
					/*
					for (int index : std::views::iota(0, PMax)) {
						int ynow = std::max(yp, ye - static_cast<int>(m_Point[static_cast<size_t>(j)][static_cast<size_t>(index)] * ys));
						int ynext = std::max(yp, ye - static_cast<int>(m_Point[static_cast<size_t>(j + 1)][static_cast<size_t>(index)] * ys));
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
			int i = 0;
			// ì‡óe
			DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
				FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xpos, ypos + (i * LineHeight),
				White, Black, "AsyncCount :%d", GetASyncLoadNum());
			++i;
			DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
				FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xpos, ypos + (i * LineHeight),
				White, Black, "Drawcall  :%d", GetDrawCallCount());
			++i;
			DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
				FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xpos, ypos + (i * LineHeight),
				White, Black, "FPS    :%5.2f fps", GetFPS());
			++i;
			for (size_t index : std::views::iota(1, static_cast<int>(m_PointSel + 1))) {
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xpos, ypos + (i * LineHeight),
					Colors[index], DarkGreen, "%02d(%5.2fms)[%s]", index, m_Point[static_cast<size_t>(PointFrame)][index], m_Str[index - 1].c_str());
				++i;
			}
			{
				size_t index = static_cast<size_t>(PointMax);
				DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
					FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xpos, ypos + (i * LineHeight),
					Colors[index], DarkGreen, "%02d(%5.2fms)[%s]", index, m_Point[static_cast<size_t>(PointFrame)][index], m_Str[index - 1].c_str());
				++i;
			}
		}
	}
#endif // DEBUG
};
