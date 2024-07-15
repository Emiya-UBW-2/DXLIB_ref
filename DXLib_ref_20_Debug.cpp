#include "DXLib_ref_20_Debug.hpp"

namespace DXLibRef {
#ifdef DEBUG
	//--------------------------------------------------------------------------------------------------
	//ÉVÉìÉOÉãÉgÉì
	//--------------------------------------------------------------------------------------------------
	const DebugClass* SingletonBase<DebugClass>::m_Singleton = nullptr;


	DebugClass::DebugClass(void) noexcept {
		m_Point.resize(PointFrame +1);
		m_Switch.Set(true);
	}

	void DebugClass::SetStartPoint(void) noexcept {
		if (!m_Switch.on()) { return; }
		m_StartTime = GetNowHiPerformanceCount();
		m_PointSel = 0;
		SetPoint("-----Start-----");
	}
	void DebugClass::SetPoint(const char* DebugMes) noexcept {
		if (!m_Switch.on()) { return; }
		if (m_PointSel < PointMax) {
			m_Point[0][m_PointSel] = static_cast<float>(GetNowHiPerformanceCount() - m_StartTime) / 1000.0f;
			m_Str[m_PointSel] = DebugMes;
			m_PointSel++;
			return;
		}
	}
	void DebugClass::SetEndPoint(void) noexcept {
		m_Switch.Execute(CheckHitKeyWithCheck(KEY_INPUT_F1) != 0);
		if (!m_Switch.on()) { return; }
		auto PMax = PointMax + 1;
		//ç≈å„ÇÃââéZ
		SetPoint("-----End-----");
		for (int index = 0; index < PMax; ++index) {
			if (!(static_cast<int>(m_PointSel) > index)) {
				m_Point[0][static_cast<std::size_t>(index)] = m_Point[0][m_PointSel - 1];
			}
			if (index == PointMax) {
				m_Point[0][static_cast<std::size_t>(index)] = 1000.0f / GetFPS();
			}

			for (int j = static_cast<int>(PointFrame - 1); j >= 1; --j) {
				m_Point[static_cast<std::size_t>(j)][static_cast<std::size_t>(index)] = m_Point[static_cast<std::size_t>(j - 1)][static_cast<std::size_t>(index)];
			}

			//ïΩãœ
			m_Point[static_cast<std::size_t>(PointFrame)][static_cast<std::size_t>(index)] = 0.f;
			for (int j = 0; j < PointFrame; ++j) {
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
		auto* DrawParts = DXDraw::Instance();
		if (!m_Switch.on()) { return; }
		const unsigned int Colors[PointMax + 1] = {
				GetColor(255,  0,  0),
				GetColor(255,255,  0),
				GetColor(0,255,  0),
				GetColor(0,255,255),
				GetColor(100,100,255),
				GetColor(255,  0,255),
				GetColor(255,  0,  0),
				GetColor(255,255,  0),
				GetColor(0,255,  0),
				GetColor(0,255,255),
				GetColor(100,100,255),
				GetColor(255,  0,255),
				GetColor(255,  0,  0),
		};
		auto PMax = PointMax + 1;
		{
			auto wide = DrawParts->GetUIY(340);
			auto height = DrawParts->GetUIY(360);
			auto border = static_cast<int>((height * 2 / 3));
			//îwåi
			WindowSystem::SetBox(xpos, ypos, xpos + wide, ypos + height, White);
			WindowSystem::SetBox(xpos + 1, ypos + 1, xpos + wide - 1, ypos + height - 1, Black);

			{
				auto xp = static_cast<int>(xpos);
				auto yp = static_cast<int>(ypos);
				//ì‡óe
				auto* OptionParts = OPTION::Instance();
				int value = OptionParts->GetParamInt(EnumSaveParam::FpsLimit);
				auto xs = static_cast<float>(wide) / PointFrame;
				auto ys = (float)border / (1000.0f / value);

				auto ye = yp + static_cast<int>(height);
				for (int j = static_cast<int>(PointFrame - 1 - 1); j >= 0; --j) {
					int xnow = xp + static_cast<int>((float)j * xs);
					int xnext = xp + static_cast<int>(static_cast<float>(j + 1) * xs);

					for (int index = PMax - 1; index >= 0; --index) {
						int ynow = std::max(yp, ye - static_cast<int>(m_Point[static_cast<std::size_t>(j)][static_cast<std::size_t>(index)] * ys));
						int ynext = std::max(yp, ye - static_cast<int>(m_Point[static_cast<std::size_t>(j + 1)][static_cast<std::size_t>(index)] * ys));
						DrawQuadrangle(
							xnow, ynow,
							xnext, ynext,
							xnext, ye,
							xnow, ye,
							Colors[index],
							TRUE);
					}
					for (int index = 0; index < PMax; ++index) {
						int ynow = std::max(yp, ye - static_cast<int>(m_Point[static_cast<std::size_t>(j)][static_cast<std::size_t>(index)] * ys));
						int ynext = std::max(yp, ye - static_cast<int>(m_Point[static_cast<std::size_t>(j + 1)][static_cast<std::size_t>(index)] * ys));
						DrawLine_2D(
							xnow, ynow,
							xnext, ynext,
							GetColor(64, 64, 64));
					}
				}
				DrawLine_2D(xp, ye - border, xp + static_cast<int>(wide), ye - border, White);//äÓèÄê¸
			}
			ypos += height;
		}
		{
			int FontSize = DrawParts->GetUIY(18);
			auto wide = DrawParts->GetUIY(350);
			auto height = static_cast<int>(m_PointSel + 3 + 1) * FontSize + DrawParts->GetUIY(10);
			//îwåi
			WindowSystem::SetBox(xpos, ypos, xpos + wide, ypos + height, White);
			WindowSystem::SetBox(xpos + 1, ypos + 1, xpos + wide - 1, ypos + height - 1, Black);

			xpos += DrawParts->GetUIY(2);
			ypos += DrawParts->GetUIY(2);
			//ì‡óe
			WindowSystem::SetMsg(xpos, ypos + FontSize / 2, FontSize, FontHandle::FontXCenter::LEFT, White, Black, "AsyncCount :%d", GetASyncLoadNum()); ypos += FontSize;
			WindowSystem::SetMsg(xpos, ypos + FontSize / 2, FontSize, FontHandle::FontXCenter::LEFT, White, Black, "Drawcall   :%d", GetDrawCallCount() - 350); ypos += FontSize;
			WindowSystem::SetMsg(xpos, ypos + FontSize / 2, FontSize, FontHandle::FontXCenter::LEFT, White, Black, "FPS        :%5.2f fps", GetFPS()); ypos += FontSize;
			for (int index = 1; index < PMax; ++index) {
				if (static_cast<int>(m_PointSel) >= index || index == PointMax) {
					WindowSystem::SetMsg(xpos, ypos + FontSize / 2, FontSize, FontHandle::FontXCenter::LEFT, Colors[index], DarkGreen, "%02d(%5.2fms)[%s]", index, m_Point[static_cast<std::size_t>(PointFrame)][static_cast<std::size_t>(index)], m_Str[static_cast<std::size_t>(index - 1)].c_str());
					ypos += FontSize;
				}
			}
		}
	}
#endif // DEBUG
};
