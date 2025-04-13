#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
#if defined(DEBUG)
	class DebugDraw : public SingletonBase<DebugDraw> {
	private:
		friend class SingletonBase<DebugDraw>;
	private:
		static const int	PointMax{ 12 };
		static const size_t	PointFrame{ 120 };
	private:
		LONGLONG													m_StartTime{ 0 };
		std::array<std::string, PointMax + 1>						m_Str{};
		std::array<std::array<float, PointMax + 1>, PointFrame + 1>	m_Point{};
		std::array<std::array<float, PointMax + 1>, PointFrame + 1>	m_PointP{};
		size_t														m_PointSel{ 0 };
		switchs														m_Switch{};
		bool														m_IsActive{ true };
	private:
		DebugDraw(void) noexcept;
		DebugDraw(const DebugDraw&) = delete;
		DebugDraw(DebugDraw&& o) = delete;
		DebugDraw& operator=(const DebugDraw&) = delete;
		DebugDraw& operator=(DebugDraw&& o) = delete;
	public:
		void SetStartPoint(void) noexcept;
		void SetPoint(const char* DebugMes) noexcept;
		void SetEndPoint(void) noexcept;
		void DebugWindow(int xpos, int ypos) noexcept;
	};
#endif // DEBUG
};