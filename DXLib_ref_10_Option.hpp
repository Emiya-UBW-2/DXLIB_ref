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
		bool	SSR{ true };
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
		const auto		Get_SSR()const noexcept { return SSR && !useVR; }
	public:
		void			Set_grass_level(int value) noexcept { grass_level = value; }
		void			Set_DoF(bool use) { DoF = use; }
		void			Set_Bloom(bool value) noexcept { Bloom = value; }
		void			Set_Shadow(bool value) noexcept { Shadow = value; }
		void			Set_useVR(bool use) { useVR = use; }
		void			Set_SSAO(bool use) { SSAO = use; }

		void			Set_Vsync(bool value) noexcept { Vsync = value; }
		void			Set_SE(float per) noexcept { SE = per; }
		void			Set_VOICE(float per)noexcept { VOICE = per; }
		void			Set_BGM(float per) noexcept { BGM = per; }
		void			Set_aberration(bool value) noexcept { aberration = value; }
		void			Set_DirectXVer(int value) noexcept { DirectXVer = value; }
		void			Set_SSR(bool use) { SSR = use; }
	public:
		void			Load(void) noexcept;
		void			Save(void) noexcept;
	};

	class OptionWindowClass : public SingletonBase<OptionWindowClass> {
	private:
		friend class SingletonBase<OptionWindowClass>;
	private:
		class OptionElementsInfo {
		private:
			std::string m_Name;
			std::string m_Info;
			std::function<void()> m_LeftPush;
			std::function<void()> m_RightPush;
			std::function<void()> m_OKPush;
			std::function<void(int xpos, int ypos)> m_Draw;
		public:
			float selanim{ 0.f };
		public:
			const auto& GetName() const noexcept { return m_Name; }
			const auto& GetInfo() const noexcept { return m_Info; }

			void GetLeftPush() const noexcept { m_LeftPush(); }
			void GetRightPush() const noexcept { m_RightPush(); }
			void GetOKPush() const noexcept { m_OKPush(); }
		public:
			void Init(const char* name, const char* info, std::function<void()> LeftPush, std::function<void()> RightPush, std::function<void()> OKPush, std::function<void(int xpos, int ypos)> draw) {
				selanim = 0;

				m_Name = name;
				m_Info = info;
				m_LeftPush = LeftPush;
				m_RightPush = RightPush;
				m_OKPush = OKPush;
				m_Draw = draw;
			}
			void Draw(int xpos, int ypos, bool isMine) const noexcept;
		};
		class OptionTabsInfo {
		private:
		protected:
			int m_id{ 0 };
			std::string m_name;
			std::vector<OptionElementsInfo> m_Elements;
		protected:
			virtual void Init_Sub() noexcept {}
		public:
			const auto& GetID() const noexcept { return m_id; }
		public:
			void Init(int ID, const char* name) noexcept {
				m_id = ID;
				m_name = name;
				Init_Sub();
			}
			void Execute(int *select) noexcept;
			void Draw(int xpos, int ypos, bool isActive, int select) noexcept;

			void DrawInfo(int xpos, int ypos, int select) noexcept;
		};

		class SoundTabsInfo :public OptionTabsInfo {
		protected:
			void Init_Sub() noexcept override;
		};
		class GraphicTabsInfo :public OptionTabsInfo {
		protected:
			void Init_Sub() noexcept override;
		};
	private:
		int m_tabsel{ 0 };
		int m_select{ 0 };
		std::array<std::unique_ptr<OptionTabsInfo>, 2> m_Tabs;
		bool m_isActive{ false };
		float m_ActivePer{ 0.f };
	private:
	public:
		void SetActive() noexcept { m_isActive = true; }
		const auto& IsActive() const noexcept { return m_isActive; }
	public:
		void Init() noexcept;
		void Execute(void)noexcept;
		void Draw() const noexcept;
	};
}
