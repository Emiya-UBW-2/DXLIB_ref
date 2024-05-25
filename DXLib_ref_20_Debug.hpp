#pragma once
#include "DXLib_ref.h"

namespace DXLib_ref {
#ifdef DEBUG
	class DebugClass : public SingletonBase<DebugClass> {
	private:
		friend class SingletonBase<DebugClass>;
	private:
		static const int	PointMax{ 12 };
	private:
		LONGLONG																m_StartTime{ 0 };
		std::vector<std::array<std::pair<float, std::string>, PointMax + 1>>	m_Point;
		size_t																	m_PointSel{ 0 };
	private:
		DebugClass();
		~DebugClass();
	public:
		void SetStartPoint(void) noexcept;
		void SetPoint(const char* DebugMes) noexcept;
		void DebugWindow(int xpos, int ypos) noexcept;
	};
#endif // DEBUG
};