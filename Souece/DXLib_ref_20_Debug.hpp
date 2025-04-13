#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
#if defined(DEBUG)
	class DebugClass : public SingletonBase<DebugClass> {
	private:
		friend class SingletonBase<DebugClass>;
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
		DebugClass(void) noexcept;
		DebugClass(const DebugClass&) = delete;
		DebugClass(DebugClass&& o) = delete;
		DebugClass& operator=(const DebugClass&) = delete;
		DebugClass& operator=(DebugClass&& o) = delete;
	public:
		void SetStartPoint(void) noexcept;
		void SetPoint(const char* DebugMes) noexcept;
		void SetEndPoint(void) noexcept;
		void DebugWindow(int xpos, int ypos) noexcept;
	};
#endif // DEBUG
};