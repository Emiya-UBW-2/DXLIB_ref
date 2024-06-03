#include "DXLib_ref_20_Debug.hpp"

namespace DXLibRef {
#ifdef DEBUG
	//--------------------------------------------------------------------------------------------------
	//ƒVƒ“ƒOƒ‹ƒgƒ“
	//--------------------------------------------------------------------------------------------------
	const DebugClass* SingletonBase<DebugClass>::m_Singleton = nullptr;


	DebugClass::DebugClass() {
		m_Point.resize(PointFrame +1);
		m_Switch.Set(true);
	}
	DebugClass::~DebugClass() {}

	void DebugClass::SetStartPoint(void) noexcept {
		if (!m_Switch.on()) { return; }
		m_StartTime = GetNowHiPerformanceCount();
		m_PointSel = 0;
		SetPoint("-----Start-----");
	}
	void DebugClass::SetPoint(const char* DebugMes) noexcept {
		if (!m_Switch.on()) { return; }
		if (m_PointSel < PointMax) {
			m_Point[0][m_PointSel] = (float)(GetNowHiPerformanceCount() - m_StartTime) / 1000.0f;
			m_Str[m_PointSel] = DebugMes;
			m_PointSel++;
			return;
		}
	}
	void DebugClass::SetEndPoint() noexcept {
		m_Switch.Execute(CheckHitKeyWithCheck(KEY_INPUT_F1) != 0);
		if (!m_Switch.on()) { return; }
		auto PMax = PointMax + 1;
		//ÅŒã‚Ì‰‰ŽZ
		SetPoint("-----End-----");
		for (int index = 0; index < PMax; ++index) {
			if (!(m_PointSel > index)) {
				m_Point[0][index] = m_Point[0][m_PointSel - 1];
			}
			if (index == PointMax) {
				m_Point[0][index] = 1000.0f / GetFPS();
			}

			for (int j = (int)(PointFrame - 1); j >= 1; --j) {
				m_Point[j][index] = m_Point[j - 1][index];
			}

			//•½‹Ï
			m_Point[PointFrame][index] = 0.f;
			for (int j = 0; j < PointFrame; ++j) {
				m_Point[PointFrame][index] += m_Point[j][index];
			}
			m_Point[PointFrame][index] /= PointFrame;
		}
		for (int index = PMax - 1; index >= 1; --index) {
			if (index > 0) {
				m_Point[PointFrame][index] = m_Point[PointFrame][index] - m_Point[PointFrame][index - 1];
			}
		}
	}

	void DebugClass::DebugWindow(int xpos, int ypos, float scale) noexcept {
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
			auto wide = y_r(340);
			auto height = y_r(360);
			auto border = (int)((height * 5 / 6) * scale);
			//”wŒi
			WindowSystem::SetBox(xpos, ypos, xpos + wide, ypos + height, scale, White);
			WindowSystem::SetBox(xpos + 1, ypos + 1, xpos + wide - 1, ypos + height - 1, scale, Black);

			{
				auto xp = (int)(xpos*scale);
				auto yp = (int)(ypos*scale);
				//“à—e
				auto* OptionParts = OPTION::Instance();
				int value = OptionParts->GetParamInt(EnumSaveParam::FpsLimit);
				auto xs = (float)(wide*scale) / 180;
				auto ys = (float)border / (1000.0f / value);
				for (int j = (int)(PointFrame - 1 - 1); j >= 0; --j) {
					int xnow = xp + (int)((float)j * xs);
					int xnext = xp + (int)((float)(j + 1) * xs);

					for (int index = PMax - 1; index >= 0; --index) {
						int ynow = yp + (int)(height*scale) - (int)(m_Point[j][index] * ys);
						int ynext = yp + (int)(height*scale) - (int)(m_Point[j + 1][index] * ys);
						DrawQuadrangle(
							xnow, ynow,
							xnext, ynext,
							xnext, yp + (int)(height*scale),
							xnow, yp + (int)(height*scale),
							Colors[index],
							TRUE);
					}
					for (int index = 0; index < PMax; ++index) {
						int ynow = yp + (int)(height*scale) - (int)(m_Point[j][index] * ys);
						int ynext = yp + (int)(height*scale) - (int)(m_Point[j + 1][index] * ys);
						DrawLine_2D(
							xnow, ynow,
							xnext, ynext,
							GetColor(64, 64, 64));
					}
				}
				DrawLine_2D(xp, yp + (int)(height*scale) - border, xp + (int)(wide*scale), yp + (int)(height*scale) - border, White);//Šî€ü
			}
			ypos += height;
		}
		{
			int FontSize = y_r(18);
			auto wide = y_r(350);
			auto height = (int)(m_PointSel + 3 + 1) * FontSize + y_r(10);
			//”wŒi
			WindowSystem::SetBox(xpos, ypos, xpos + wide, ypos + height, scale, White);
			WindowSystem::SetBox(xpos + 1, ypos + 1, xpos + wide - 1, ypos + height - 1, scale, Black);

			xpos += y_r(2);
			ypos += y_r(2);
			//“à—e
			WindowSystem::SetMsg(xpos, ypos, xpos, ypos + FontSize, scale, FontSize, FontHandle::FontXCenter::LEFT, White, Black, "AsyncCount :%d", GetASyncLoadNum()); ypos += FontSize;
			WindowSystem::SetMsg(xpos, ypos, xpos, ypos + FontSize, scale, FontSize, FontHandle::FontXCenter::LEFT, White, Black, "Drawcall   :%d", GetDrawCallCount() - 350); ypos += FontSize;
			WindowSystem::SetMsg(xpos, ypos, xpos, ypos + FontSize, scale, FontSize, FontHandle::FontXCenter::LEFT, White, Black, "FPS        :%5.2f fps", GetFPS()); ypos += FontSize;
			for (int index = 1; index < PMax; ++index) {
				if (m_PointSel >= index || index == PointMax) {
					WindowSystem::SetMsg(xpos, ypos, xpos, ypos + FontSize, scale, FontSize, FontHandle::FontXCenter::LEFT, Colors[index], DarkGreen, "%02d(%5.2fms)[%s]", index, m_Point[PointFrame][index], m_Str[index - 1].c_str());
					ypos += FontSize;
				}
			}
		}
	}
#endif // DEBUG
};
