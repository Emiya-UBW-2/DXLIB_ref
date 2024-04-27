#pragma once
#include "DXLib_ref.h"

namespace DXLib_ref {
	enum class EnumParamType {
		Boolean,
		Int,
		Float,
		Else,
	};
	static EnumParamType ParamTypes[] = {
		EnumParamType::Int,
		EnumParamType::Boolean,
		EnumParamType::Boolean,
		EnumParamType::Boolean,
		EnumParamType::Boolean,
		EnumParamType::Boolean,
		EnumParamType::Float,
		EnumParamType::Boolean,
		EnumParamType::Int,
		EnumParamType::Float,
		EnumParamType::Float,
		EnumParamType::Float,
		EnumParamType::Boolean,
		EnumParamType::Boolean,
		EnumParamType::Else,
		EnumParamType::Boolean,
		EnumParamType::Boolean,
		EnumParamType::Float,
		EnumParamType::Float,
		EnumParamType::Boolean,
		EnumParamType::Boolean,
		EnumParamType::Boolean,
		EnumParamType::Else,
		EnumParamType::Else,
	};
	static const char* OptionStr[] = {
		"grass_level",
		"DoF",
		"bloom",
		"shadow",
		"usevr",
		"SSAO",
		"fov",
		"vsync",
		"FpsLimit",
		"SE",
		"VOICE",
		"BGM",
		"AllWaysFront",
		"aberration",
		"DirectXVer",
		"SSR",
		"MotionBlur",
		"Xsensing",
		"Ysensing",
		"HeadBobbing",
		"EnableCheck",
		"LightMode",
		"ControlType",
		"Language",
	};
	//
	static const char* DirectXVerStr[] = {
	"9c",
	"11",
	};

	enum class LanguageType :int {
		Jpn,
		Eng,
	};
	static const char* LanguageStr[] = {
	"Jpn",
	"Eng",
	};

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
		int		FrameLimit = 60;
		float	SE{ 1.f };
		float	VOICE{ 1.f };
		float	BGM{ 1.f };
		bool	AllWaysFront{ false };
		bool	aberration{ true };
		int		DirectXVer{ 1 };
		bool	SSR{ true };
		bool	MotionBlur{ true };
		float	Xsensing{ 0.5f };
		float	Ysensing{ 0.5f };
		bool	HeadBobbing{true};
		bool	EnableCheck{true};
		bool	FXAA{true};
		bool	LightMode{false};
		int		PadType{1};
		int		Language{0};
	public:
		const auto		Get_DirectXVer()const noexcept { return DirectXVer; }
		const auto		Get_useVR()const noexcept {
#ifndef _USE_OPENVR_
			return false;
#else
			return useVR && (Get_DirectXVer() == 1);
#endif
		}

		const auto		Get_LightMode()const noexcept { return LightMode; }
		//VRでは使えない機能は　 && !Get_useVR()
		const auto		Get_grass_level()const noexcept { return grass_level; }
		const auto		Get_DoF()const noexcept { return DoF && !Get_useVR() && !Get_LightMode(); }
		const auto		Get_Bloom()const noexcept { return Bloom && !Get_useVR() && !Get_LightMode(); }
		const auto		Get_Shadow()const noexcept { return Shadow && !Get_LightMode(); }
		const auto		Get_SSAO()const noexcept { return SSAO && !Get_useVR() && !Get_LightMode() && (Get_DirectXVer() == 1); }
		const auto		Get_Fov()const noexcept { return Fov; }
		const auto		Get_Vsync()const noexcept { return Vsync; }
		const auto		Get_FrameLimit()const noexcept { return FrameLimit; }
		const auto		Get_SE()const noexcept { return SE; }
		const auto		Get_VOICE()const noexcept { return VOICE; }
		const auto		Get_BGM()const noexcept { return BGM; }
		const auto		Get_AllWaysFront()const noexcept { return AllWaysFront; }
		const auto		Get_aberration()const noexcept { return aberration && !Get_LightMode(); }
		const auto		Get_SSR()const noexcept { return SSR && !Get_useVR() && !Get_LightMode() && (Get_DirectXVer() == 1); }
		const auto		Get_MotionBlur()const noexcept { return MotionBlur && !Get_LightMode(); }
		const auto		Get_Xsensing()const noexcept { return Xsensing; }
		const auto		Get_Ysensing()const noexcept { return Ysensing; }
		const auto		Get_HeadBobbing()const noexcept { return HeadBobbing; }
		const auto		Get_EnableCheck()const noexcept { return EnableCheck; }
		const auto		Get_FXAA()const noexcept { return FXAA && !Get_useVR() && !Get_LightMode() && (Get_DirectXVer() == 1); }
		const auto		Get_PadType()const noexcept { return PadType; }
		const auto		Get_Language()const noexcept { return Language; }

	public:
		void			Set_useVR(bool use) { useVR = use; }
		void			Set_LightMode(bool use) { LightMode = use; }
		void			Set_grass_level(int value) noexcept { grass_level = value; }
		void			Set_DoF(bool use) { DoF = use; }
		void			Set_Bloom(bool value) noexcept { Bloom = value; }
		void			Set_Shadow(bool value) noexcept { Shadow = value; }
		void			Set_SSAO(bool use) { SSAO = use; }
		void			Set_Fov(float per) noexcept { Fov = per; }
		void			Set_Vsync(bool value) noexcept { Vsync = value; }
		void			Set_FrameLimit(int value) noexcept { FrameLimit = value; }
		void			Set_SE(float per) noexcept { SE = per; }
		void			Set_VOICE(float per)noexcept { VOICE = per; }
		void			Set_BGM(float per) noexcept { BGM = per; }
		void			Set_AllWaysFront(bool value) noexcept { AllWaysFront = value; }
		void			Set_aberration(bool value) noexcept { aberration = value; }
		void			Set_DirectXVer(int value) noexcept { DirectXVer = value; }
		void			Set_SSR(bool use) { SSR = use; }
		void			Set_MotionBlur(bool use) { MotionBlur = use; }
		void			Set_Xsensing(float per) noexcept { Xsensing = per; }
		void			Set_Ysensing(float per) noexcept { Ysensing = per; }
		void			Set_HeadBobbing(bool use) noexcept { HeadBobbing = use; }
		void			Set_EnableCheck(bool use) noexcept { EnableCheck = use; }
		void			Set_FXAA(bool use) noexcept { FXAA = use; }
		void			Set_PadType(int use) noexcept { PadType = use; }
		void			Set_Language(int use) noexcept { Language = use; }
		
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
			std::function<void()> m_AnyDoing;
			std::function<void(int xpos, int ypos, bool isMine)> m_Draw;
		public:
			float selanim{ 0.f };
		public:
			const auto& GetName() const noexcept { return m_Name; }
			const auto& GetInfo() const noexcept { return m_Info; }

			void GetLeftPush() const noexcept { m_LeftPush(); }
			void GetRightPush() const noexcept { m_RightPush(); }
			void GetOKPush() const noexcept { m_OKPush(); }
			void GetAnyDoing() const noexcept { m_AnyDoing(); }
		public:
			void Init(const char* name, const char* info, std::function<void()> LeftPush, std::function<void()> RightPush, std::function<void()> OKPush,
				std::function<void()> AnyDoing,
				std::function<void(int xpos, int ypos, bool isMine)> draw) {
				selanim = 0;

				m_Name = name;
				m_Info = info;
				m_LeftPush = LeftPush;
				m_RightPush = RightPush;
				m_OKPush = OKPush;
				m_AnyDoing = AnyDoing;
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
			void Execute(int *select, bool CanPress) noexcept;
			void Draw(int xpos, int ypos, bool isActive, int* TabSel, int *select) noexcept;

			void DrawInfo(int xpos, int ypos, int select) noexcept;
		};

		class SoundTabsInfo :public OptionTabsInfo {
		protected:
			void Init_Sub() noexcept override;
		};
		class GraphicTabsInfo :public OptionTabsInfo {
			static const int	FrameLimitsNum = 10;
			const int	FrameLimits[FrameLimitsNum] = {
				30,
				60,
				75,
				90,
				120,
				144,
				240,
				300,
				360,
				1000,
			};
		private:
			int RefreshRate{FrameLimits[1]};
		protected:
			void Init_Sub() noexcept override;
		};
		class ControlTabsInfo :public OptionTabsInfo {
		protected:
			void Init_Sub() noexcept override;
		private:
			void KeyDraw(int xpos, int ypos, bool isMine, int Sel) noexcept;
		};
		class ElseTabsInfo :public OptionTabsInfo {
		protected:
			void Init_Sub() noexcept override;
		};
	private:
		int m_tabsel{ 0 };
		int m_select{ 0 };
		std::array<std::unique_ptr<OptionTabsInfo>, 4> m_Tabs;
		PopUpDrawClass				m_PopUpDrawClass;
		bool						m_ActiveSwitch{false};
		bool						m_RestartSwitch{false};
	public:
		void SetRestart() noexcept { m_RestartSwitch = true; }
		void SetActive() noexcept { m_ActiveSwitch = true; }
		void OffSwitch() noexcept {
			if (m_PopUpDrawClass.IsActive()) {
				m_ActiveSwitch = true;
			}
		}
		const auto IsRestartSwitch() noexcept {
			auto Res = !m_PopUpDrawClass.IsActive() && m_RestartSwitch;
			if (Res) {
				m_RestartSwitch = false;
			}
			return Res;
		}
		const auto& IsActive() const noexcept { return m_PopUpDrawClass.IsActive(); }
	public:
		void Init() noexcept;
		void Execute(void)noexcept;
		void Draw() noexcept;
	};
}
