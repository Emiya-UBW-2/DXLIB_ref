#pragma once
#include "DXLib_ref.h"

namespace DXLib_ref {
	class OPTION : public SingletonBase<OPTION> {
	private:
		friend class SingletonBase<OPTION>;
	private:
		OPTION() {
			Load();
		}
		~OPTION() {}
	private:
		int		grass_level = 4;
		bool	DoF{ false };
		bool	Bloom{ false };
		bool	Shadow{ false };
		bool	useVR{ true };
		bool	SSAO{ true };
		float	Fov = 45.f;
		bool	Vsync{ false };
		float	SE{ 1.f };
		float	VOICE{ 1.f };
		float	BGM{ 1.f };
		bool	AllWaysFront{ false };
		bool	aberration{ true };
		int		DirectXVer{ DX_DIRECT3D_11 };
	public:
		//VRでは使えない機能は　 && !useVR
		const auto		Get_grass_level()const noexcept { return grass_level; }
		const auto		Get_DoF()const noexcept { return DoF && !useVR; }
		const auto		Get_Bloom()const noexcept { return Bloom && !useVR; }
		const auto		Get_Shadow()const noexcept { return Shadow; }
		const auto		Get_useVR()const noexcept { return useVR; }
		const auto		Get_SSAO()const noexcept { return SSAO && !useVR; }
		const auto		Get_Fov()const noexcept { return Fov; }
		const auto		Get_Vsync()const noexcept { return Vsync; }
		const auto		Get_SE()const noexcept { return SE; }
		const auto		Get_VOICE()const noexcept { return VOICE; }
		const auto		Get_BGM()const noexcept { return BGM; }
		const auto		Get_AllWaysFront()const noexcept { return AllWaysFront; }
		const auto		Get_aberration()const noexcept { return aberration; }
		const auto		Get_DirectXVer()const noexcept { return DirectXVer; }
	public:
		void			Set_grass_level(int value) noexcept { grass_level = value; }
		void			Set_Bloom(bool value) noexcept { Bloom = value; }
		void			Set_Shadow(bool value) noexcept { Shadow = value; }
		void			Set_useVR(bool use) { useVR = use; }
		void			Set_Vsync(bool value) noexcept { Vsync = value; }
		void			Set_SE(float per) noexcept { SE = per; }
		void			Set_VOICE(float per)noexcept { VOICE = per; }
		void			Set_BGM(float per) noexcept { BGM = per; }
		void			Set_aberration(bool value) noexcept { aberration = value; }
		void			Set_DirectXVer(int value) noexcept { DirectXVer = value; }
	public:
		void			Load(void) noexcept;
		void			Save(void) noexcept;
	};
}
