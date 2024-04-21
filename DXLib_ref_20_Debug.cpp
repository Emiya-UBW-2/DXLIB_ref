#include "DXLib_ref_20_Debug.hpp"

namespace DXLib_ref {
#ifdef DEBUG
	//--------------------------------------------------------------------------------------------------
	//シングルトン
	//--------------------------------------------------------------------------------------------------
	const DebugClass* SingletonBase<DebugClass>::m_Singleton = nullptr;

	DebugClass::DebugClass() {
		m_Point.resize((int)m_FrameRate);
	}
	DebugClass::~DebugClass() {}

	void DebugClass::SetStartPoint(void) noexcept {
		m_StartTime = GetNowHiPerformanceCount();
		m_PointSel = 0;
	}
	void DebugClass::SetPoint(const char* DebugMes) noexcept {
		if (m_PointSel < PointMax) {
			m_Point[0][m_PointSel].first = (float)(GetNowHiPerformanceCount() - m_StartTime) / 1000.0f;
			m_Point[0][m_PointSel].second = DebugMes;
			/*
			//60FPSを切ったかチェック
			if (DxLib::CheckHitKey(KEY_INPUT_RSHIFT) != 0) {
				if (
					((m_PointSel == 0) && (m_Point[0][m_PointSel] > (1000.0f / m_FrameRate)))
					|| ((m_PointSel > 0) & (m_Point[0][m_PointSel] - m_Point[0][m_PointSel - 1] > (1000.0f / m_FrameRate)))
					) {
					//int a = 0;//60fpsを切ったポイント
				}
			}
			//*/
			m_PointSel++;
			return;
		}
	}
	void DebugClass::DebugWindow(int xpos, int ypos) noexcept {
		int DrawCall = GetDrawCallCount();
		auto PMax = PointMax + 1;
		SetPoint("End");
		auto* Fonts = FontPool::Instance();
		auto black = GetColor(1, 1, 1);
		
		int FontSize = y_r(12);

		auto wide = y_r(180);
		auto height = y_r(100);
		auto half = height / 2;

		for (int index = 0; index < PMax; ++index) {
			if (m_PointSel - 1 <= index) {
				m_Point[0][index] = m_Point[0][m_PointSel - 1];
			}
			if (index == PointMax) {
				m_Point[0][index].first = 1000.0f / GetFPS();
			}
			for (int j = (int)(m_Point.size() - 1); j >= 1; --j) {
				m_Point[j][index] = m_Point[j - 1][index];
			}
		}

		DrawBox_2D(xpos - 1, ypos - 1, xpos + wide + 1, ypos + height + 1, black, TRUE);
		DrawBox_2D(xpos, ypos, xpos + wide, ypos + height, White, FALSE);
		DrawLine_2D(xpos, ypos + height - half, xpos + wide, ypos + height - half, White);//基準線
		auto xs = (float)wide / m_FrameRate;
		auto ys = (float)half / (1000.0f / m_FrameRate);
		for (int j = (int)(m_Point.size() - 1 - 1); j >= 0; --j) {
			for (int index = 0; index < PMax; ++index) {
				DrawLine_2D(
					xpos + (int)((float)j * xs), ypos + height - (int)(m_Point[j][index].first * ys),
					xpos + (int)((float)(j + 1) * xs), ypos + height - (int)(m_Point[j + 1][index].first * ys),
					(index < PointMax) ? GetColor(50, 128 + 127 * (int)(index) / PointMax, 50) : GetColor(255, 255, 0));
			}
		}

		auto xp_f = xpos;
		auto yp_f = ypos + height;
		auto xs_f = y_r(250);
		auto ys_f = (int)(m_PointSel + 3 + 1) * FontSize + 3;
		DrawBox_2D(xp_f - 1, yp_f - 1, xp_f + xs_f + 1, yp_f + ys_f + 1, black, TRUE);
		DrawBox_2D(xp_f, yp_f, xp_f + xs_f, yp_f + ys_f, White, FALSE);

		Fonts->Get(FontPool::FontType::Nomal_Edge, FontSize).DrawString(-1, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp_f + 2, yp_f, White, black,
			"AsyncCount :%d", GetASyncLoadNum()); yp_f += FontSize;
		Fonts->Get(FontPool::FontType::Nomal_Edge, FontSize).DrawString(-1, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp_f + 2, yp_f, White, black,
			"Drawcall   :%d", DrawCall); yp_f += FontSize;
		Fonts->Get(FontPool::FontType::Nomal_Edge, FontSize).DrawString(-1, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp_f + 2, yp_f, White, black,
			"FPS        :%5.2f fps", GetFPS()); yp_f += FontSize;
		for (int index = 0; index < PMax; ++index) {
			if (m_PointSel - 1 >= index || index == PointMax) {
				if (index == 0) {
					Fonts->Get(FontPool::FontType::Nomal_Edge, FontSize).DrawString(-1, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp_f + 2, yp_f, White, black,
						"%02d(%5.2fms)[%s]", index, m_Point[0][index].first, "Start");
				}
				else {
					Fonts->Get(FontPool::FontType::Nomal_Edge, FontSize).DrawString(-1, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp_f + 2, yp_f, White, black,
						"%02d(%5.2fms)[%s]", index, m_Point[0][index].first - m_Point[0][index - 1].first, m_Point[0][index - 1].second.c_str());
				}
				yp_f += FontSize;
			}
		}
	}
#endif // DEBUG
};
