#include "DXLib_ref_20_Debug.hpp"

namespace DXLib_ref {
#ifdef DEBUG
	//--------------------------------------------------------------------------------------------------
	//ƒVƒ“ƒOƒ‹ƒgƒ“
	//--------------------------------------------------------------------------------------------------
	const DebugClass* SingletonBase<DebugClass>::m_Singleton = nullptr;


	DebugClass::DebugClass() {
		m_Point.resize(180);
	}
	DebugClass::~DebugClass() {}

	void DebugClass::SetStartPoint(void) noexcept {
		m_StartTime = GetNowHiPerformanceCount();
		m_PointSel = 0;
		SetPoint("-----Start-----");
	}
	void DebugClass::SetPoint(const char* DebugMes) noexcept {
		if (m_PointSel < PointMax) {
			m_Point[0][m_PointSel].first = (float)(GetNowHiPerformanceCount() - m_StartTime) / 1000.0f;
			m_Point[0][m_PointSel].second = DebugMes;
			m_PointSel++;
			return;
		}
	}
	void DebugClass::DebugWindow(int xpos, int ypos) noexcept {
		auto* DrawParts = DXDraw::Instance();
		int DrawCall = GetDrawCallCount();
		const unsigned int Colors[PointMax + 1] = {
				GetColor(255,  0,  0),
				GetColor(255,255,  0),
				GetColor(0,255,  0),
				GetColor(0,255,255),
				GetColor(0,  0,255),
				GetColor(255,  0,255),
				GetColor(255,  0,  0),
				GetColor(255,255,  0),
				GetColor(0,255,  0),
				GetColor(0,255,255),
				GetColor(0,  0,255),
				GetColor(255,  0,255),
				GetColor(255,  0,  0),
		};
		auto PMax = PointMax + 1;
		//ÅŒã‚Ì‰‰ŽZ
		SetPoint("-----End-----");
		for (int index = 0; index < PMax; ++index) {
			if (m_PointSel - 1 <= index) {
				m_Point[0][index] = m_Point[0][m_PointSel - 1];
			}
			if (index == PointMax) {
				m_Point[0][index].first = 1000.0f / DrawParts->GetFps();
			}
			for (int j = (int)(m_Point.size() - 1); j >= 1; --j) {
				m_Point[j][index] = m_Point[j - 1][index];
			}
		}
		//
		auto* Fonts = FontPool::Instance();
		int FontSize = y_r(12);

		auto xp = xpos;
		auto yp = ypos;
		{
			auto wide = y_r(240);
			auto height = y_r(360);
			auto border = height * 5 / 6;
			//”wŒi
			WindowSystem::SetBox(xp, yp, xp + wide, yp + height, 1.f, Black);
			DrawBox_2D(xp, yp, xp + wide, yp + height, White, FALSE);

			//if (IntoMouse(xp, yp, xp + wide, yp + height, scale))
			{
				//“à—e
				auto* OptionParts = OPTION::Instance();
				int value = OptionParts->GetParamInt(EnumSaveParam::FpsLimit);
				auto xs = (float)wide / 180;
				auto ys = (float)border / (1000.0f / value);
				for (int j = (int)(m_Point.size() - 1 - 1); j >= 0; --j) {
					int xnow = xp + (int)((float)j * xs);
					int xnext = xp + (int)((float)(j + 1) * xs);
					for (int index = PMax - 1; index >= 0; --index) {
						int ynow = yp + height - (int)(m_Point[j][index].first * ys);
						int ynext = yp + height - (int)(m_Point[j + 1][index].first * ys);
						DrawQuadrangle(
							xnow, ynow,
							xnext, ynext,
							xnext, yp + height,
							xnow, yp + height,
							Colors[index],
							TRUE);
					}
					for (int index = 0; index < PMax; ++index) {
						int ynow = yp + height - (int)(m_Point[j][index].first * ys);
						int ynext = yp + height - (int)(m_Point[j + 1][index].first * ys);
						DrawLine_2D(
							xnow, ynow,
							xnext, ynext,
							GetColor(64, 64, 64));
					}
				}
				DrawLine_2D(xp, yp + height - border, xp + wide, yp + height - border, White);//Šî€ü
			}
			yp += height;
		}
		{
			auto wide = y_r(250);
			auto height = (int)(m_PointSel + 3 + 1) * FontSize + 3;
			//”wŒi
			WindowSystem::SetBox(xp, yp, xp + wide, yp + height, 1.f, Black);
			DrawBox_2D(xp, yp, xp + wide, yp + height, White, FALSE);
			//“à—e
			Fonts->Get(FontPool::FontType::Nomal_Edge, FontSize).DrawString(-1, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp + 2, yp, White, Black,
				"AsyncCount :%d", GetASyncLoadNum()); yp += FontSize;
			Fonts->Get(FontPool::FontType::Nomal_Edge, FontSize).DrawString(-1, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp + 2, yp, White, Black,
				"Drawcall   :%d", DrawCall); yp += FontSize;
			Fonts->Get(FontPool::FontType::Nomal_Edge, FontSize).DrawString(-1, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp + 2, yp, White, Black,
				"FPS        :%5.2f fps", DrawParts->GetFps()); yp += FontSize;
			for (int index = 0; index < PMax; ++index) {
				if (m_PointSel - 1 >= index || index == PointMax) {
					if (index == 0) {
						Fonts->Get(FontPool::FontType::Nomal_Edge, FontSize).DrawString(-1, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp + 2, yp, Colors[index], Black,
							"%02d(%5.2fms)[%s]", index, m_Point[0][index].first, "Start");
					}
					else {
						Fonts->Get(FontPool::FontType::Nomal_Edge, FontSize).DrawString(-1, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp + 2, yp, Colors[index], Black,
							"%02d(%5.2fms)[%s]", index, m_Point[0][index].first - m_Point[0][index - 1].first, m_Point[0][index - 1].second.c_str());
					}
					yp += FontSize;
				}
			}
		}
	}
#endif // DEBUG
};
