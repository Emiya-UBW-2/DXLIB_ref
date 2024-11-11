#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	enum class EnumSaveParam {
		GraphicsPreset,
		DirectXVer,
		usevr,
		ObjLevel,
		DoF,
		bloom,
		shadow,
		SSAO,
		fov,
		vsync,
		FpsLimit,
		SE,
		VOICE,
		BGM,
		WindowMode,
		ScreenEffect,
		Reflection,
		MotionBlur,
		Xsensing,
		Ysensing,
		HeadBobbing,
		ControlType,
		Language,
		AA,
		DrawScale,
		Max,
	};
	static const char* OptionStr[static_cast<int>(EnumSaveParam::Max)] = {
		"GraphicsPreset",
		"DirectXVer",
		"usevr",
		"ObjLevel",
		"DoF",
		"bloom",
		"shadow",
		"SSAO",
		"fov",
		"vsync",
		"FpsLimit",
		"SE",
		"VOICE",
		"BGM",
		"AllWaysFront",
		"ScreenEffect",
		"Reflection",
		"MotionBlur",
		"Xsensing",
		"Ysensing",
		"HeadBobbing",
		"ControlType",
		"Language",
		"AA",
		"DrawScale",
	};

	enum class EnumProjectSettingParam {
		GodRay,
		Distortion,
		PBR,
		CubeMap,
		Max,
	};
	static const char* ProjectSettingStr[static_cast<int>(EnumProjectSettingParam::Max)] = {
		"GodRay",
		"Distortion",
		"PBR",
		"CubeMap",
	};
	enum class EnumParamType {
		Boolean,
		Int,
		Float,
		Else,
	};
	// 
	static const int DirectXVerID[] = {
		DX_DIRECT3D_9EX,
		DX_DIRECT3D_11,
	};
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

	enum class WindowType :int {
		None = 0,
		Window = None,
		Borderless,
		FullScreen,
		Max,
	};

	struct SaveParams {
	private:
		EnumParamType m_EnumParamType{};
		int valueint{ 0 };
	public:
		void SetEnumParamType(EnumParamType value) noexcept { m_EnumParamType = value; }
		auto GetEnumParamType(void) const noexcept { return m_EnumParamType; }

		void SetBoolean(bool use) noexcept { valueint = use ? 1 : 0; }
		auto GetBoolean(void) const noexcept { return (valueint != 0); }
		void ChangeBoolean(void) noexcept { SetBoolean(GetBoolean() ^ 1); }

		void SetInt(int use) noexcept { valueint = use; }
		auto GetInt(void) const noexcept { return valueint; }

		void SetFloat(float use) noexcept { valueint = static_cast<int>(use * 1000.f); }
		auto GetFloat(void) const noexcept { return static_cast<float>(valueint) / 1000.f; }

	};

	class OPTION : public SingletonBase<OPTION> {
	private:
		friend class SingletonBase<OPTION>;
	private:
		OPTION(void) noexcept {
			Load();
		}
		OPTION(const OPTION&) = delete;
		OPTION(OPTION&& o) = delete;
		OPTION& operator=(const OPTION&) = delete;
		OPTION& operator=(OPTION&& o) = delete;

		~OPTION(void) noexcept {}
	private:
		std::array<SaveParams, static_cast<int>(EnumSaveParam::Max)> m_SaveParams;
		std::array<SaveParams, static_cast<int>(EnumProjectSettingParam::Max)> m_ProjectSettingParams;
	public:
		auto		GetParamBoolean(EnumSaveParam id)const noexcept { return m_SaveParams.at(static_cast<size_t>(id)).GetBoolean(); }
		auto		GetParamInt(EnumSaveParam id)const noexcept { return m_SaveParams.at(static_cast<size_t>(id)).GetInt(); }
		auto		GetParamFloat(EnumSaveParam id)const noexcept { return m_SaveParams.at(static_cast<size_t>(id)).GetFloat(); }
	public:
		void			SetParamBoolean(EnumSaveParam id, bool use) noexcept {
			switch (id) {
			case EnumSaveParam::SSAO:
			case EnumSaveParam::AA:
			case EnumSaveParam::DoF:
				if (DirectXVerID[GetParamInt(EnumSaveParam::DirectXVer)] != DX_DIRECT3D_11) {
					use = false;
				}
				if (GetParamBoolean(EnumSaveParam::usevr)) {
					use = false;
				}
				break;
			case EnumSaveParam::usevr:
#ifndef _USE_OPENVR_
				use = false;
#endif
				if (DirectXVerID[GetParamInt(EnumSaveParam::DirectXVer)] != DX_DIRECT3D_11) {
					use = false;
				}
				break;
			case EnumSaveParam::bloom:
				if (GetParamBoolean(EnumSaveParam::usevr)) {
					use = false;
				}
				break;
			case EnumSaveParam::ScreenEffect:
				if (DirectXVerID[GetParamInt(EnumSaveParam::DirectXVer)] != DX_DIRECT3D_11) {
					use = false;
				}
				break;
			case EnumSaveParam::GraphicsPreset:
			case EnumSaveParam::DirectXVer:
			case EnumSaveParam::ObjLevel:
			case EnumSaveParam::shadow:
			case EnumSaveParam::fov:
			case EnumSaveParam::vsync:
			case EnumSaveParam::FpsLimit:
			case EnumSaveParam::SE:
			case EnumSaveParam::VOICE:
			case EnumSaveParam::BGM:
			case EnumSaveParam::WindowMode:
			case EnumSaveParam::Reflection:
			case EnumSaveParam::MotionBlur:
			case EnumSaveParam::Xsensing:
			case EnumSaveParam::Ysensing:
			case EnumSaveParam::HeadBobbing:
			case EnumSaveParam::ControlType:
			case EnumSaveParam::Language:
			case EnumSaveParam::DrawScale:
			case EnumSaveParam::Max:
			default:
				break;
			}
			m_SaveParams.at(static_cast<size_t>(id)).SetBoolean(use);
		}
		void			ChangeParamBoolean(EnumSaveParam id) noexcept { m_SaveParams.at(static_cast<size_t>(id)).ChangeBoolean(); }

		void			SetParamInt(EnumSaveParam id, int use) noexcept {
			switch (id) {
			case EnumSaveParam::Reflection:
				if (DirectXVerID[GetParamInt(EnumSaveParam::DirectXVer)] != DX_DIRECT3D_11) {
					use = false;
				}
				if (GetParamBoolean(EnumSaveParam::usevr)) {
					use = false;
				}
				break;
			case EnumSaveParam::shadow:
				if (DirectXVerID[GetParamInt(EnumSaveParam::DirectXVer)] != DX_DIRECT3D_11) {
					use = 0;
				}
				break;
			case EnumSaveParam::GraphicsPreset:
			case EnumSaveParam::DirectXVer:
			case EnumSaveParam::usevr:
			case EnumSaveParam::ObjLevel:
			case EnumSaveParam::DoF:
			case EnumSaveParam::bloom:
			case EnumSaveParam::SSAO:
			case EnumSaveParam::fov:
			case EnumSaveParam::vsync:
			case EnumSaveParam::FpsLimit:
			case EnumSaveParam::SE:
			case EnumSaveParam::VOICE:
			case EnumSaveParam::BGM:
			case EnumSaveParam::WindowMode:
			case EnumSaveParam::ScreenEffect:
			case EnumSaveParam::MotionBlur:
			case EnumSaveParam::Xsensing:
			case EnumSaveParam::Ysensing:
			case EnumSaveParam::HeadBobbing:
			case EnumSaveParam::ControlType:
			case EnumSaveParam::Language:
			case EnumSaveParam::AA:
			case EnumSaveParam::DrawScale:
			case EnumSaveParam::Max:
			default:
				break;
			}
			m_SaveParams.at(static_cast<size_t>(id)).SetInt(use);
		}
		void			SetParamFloat(EnumSaveParam id, float use) noexcept { m_SaveParams.at(static_cast<size_t>(id)).SetFloat(use); }
	public:
		auto		GetParamBoolean(EnumProjectSettingParam id)const noexcept { return m_ProjectSettingParams.at(static_cast<size_t>(id)).GetBoolean(); }
		auto		GetParamInt(EnumProjectSettingParam id)const noexcept { return m_ProjectSettingParams.at(static_cast<size_t>(id)).GetInt(); }
		auto		GetParamFloat(EnumProjectSettingParam id)const noexcept { return m_ProjectSettingParams.at(static_cast<size_t>(id)).GetFloat(); }
	public:
		void			SetParamBoolean(EnumProjectSettingParam id, bool use) noexcept { m_ProjectSettingParams.at(static_cast<size_t>(id)).SetBoolean(use); }
		void			SetParamInt(EnumProjectSettingParam id, int use) noexcept { m_ProjectSettingParams.at(static_cast<size_t>(id)).SetInt(use); }
		void			SetParamFloat(EnumProjectSettingParam id, float use) noexcept { m_ProjectSettingParams.at(static_cast<size_t>(id)).SetFloat(use); }
	public:
		void			Load(void) noexcept;
		void			Save(void) const noexcept;
	};

	class OptionWindowClass : public SingletonBase<OptionWindowClass> {
	private:
		friend class SingletonBase<OptionWindowClass>;
	private:
		class OptionElementsInfo {
		private:
			std::string m_Name;
			int m_Info{ 0 };
			std::function<void()> m_LeftPush;
			std::function<void()> m_RightPush;
			std::function<void()> m_OKPush;
			std::function<void()> m_AnyDoing;
			std::function<void(int xpos, int ypos, bool isMine)> m_Draw;
		public:
			float selanim{ 0.f };
		public:
			const auto& GetName(void) const noexcept { return m_Name; }
			const auto& GetInfoTextID(void) const noexcept { return m_Info; }

			void GetLeftPush(void) const noexcept { m_LeftPush(); }
			void GetRightPush(void) const noexcept { m_RightPush(); }
			void GetOKPush(void) const noexcept { m_OKPush(); }
			void GetAnyDoing(void) const noexcept { m_AnyDoing(); }
		public:
			void Init(const char* name, int infoTextID, std::function<void()> LeftPush, std::function<void()> RightPush, std::function<void()> OKPush,
				std::function<void()> AnyDoing,
				std::function<void(int xpos, int ypos, bool isMine)> draw) noexcept {
				selanim = 0;

				m_Name = name;
				m_Info = infoTextID;
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
			virtual void Init_Sub(void) noexcept {}
		public:
			const auto& GetID(void) const noexcept { return m_id; }
		public:
			OptionTabsInfo(void) noexcept {}
			virtual ~OptionTabsInfo(void) noexcept {}
		public:
			void Init(int ID, const char* name) noexcept {
				m_id = ID;
				m_name = name;
				Init_Sub();
			}
			void Execute(int* select, bool CanPress) noexcept;
			void Draw(int xpos, int ypos, int xsize, bool isActive, int* TabSel, int* select) noexcept;

			void DrawInfo(int xpos, int ypos, int select) noexcept;
		};

		class SoundTabsInfo :public OptionTabsInfo {
		public:
			SoundTabsInfo(void) noexcept {}
			virtual ~SoundTabsInfo(void) noexcept {}
		protected:
			void Init_Sub(void) noexcept override;
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
			int RefreshRate{ FrameLimits[1] };
		public:
			GraphicTabsInfo(void) noexcept {}
			GraphicTabsInfo(const GraphicTabsInfo&) = delete;
			GraphicTabsInfo(GraphicTabsInfo&& o) = delete;
			GraphicTabsInfo& operator=(const GraphicTabsInfo&) = delete;
			GraphicTabsInfo& operator=(GraphicTabsInfo&& o) = delete;

			virtual ~GraphicTabsInfo(void) noexcept {}
		protected:
			void Init_Sub(void) noexcept override;
		};
		class ControlTabsInfo :public OptionTabsInfo {
		public:
			ControlTabsInfo(void) noexcept {}
			virtual ~ControlTabsInfo(void) noexcept {}
		protected:
			void Init_Sub(void) noexcept override;
		private:
			void KeyDraw(int xpos, int ypos, bool isMine, PADS Sel) noexcept;
		};
		class ElseTabsInfo :public OptionTabsInfo {
		public:
			ElseTabsInfo(void) noexcept {}
			virtual ~ElseTabsInfo(void) noexcept {}
		protected:
			void Init_Sub(void) noexcept override;
		};
	private:
		int m_tabsel{ 0 };
		int m_select{ 0 };
		std::array<std::unique_ptr<OptionTabsInfo>, 4> m_Tabs;
		bool						m_Active{ false };
		bool						m_ActiveSwitch{ false };
		bool						m_RestartSwitch{ false };
	public:
		void SetRestart(void) noexcept { m_RestartSwitch = true; }
		void SetActive(void) noexcept { m_ActiveSwitch = true; }
		auto IsRestartSwitch(void) noexcept {
			if (!m_Active && m_RestartSwitch) {
				m_RestartSwitch = false;
				return true;
			}
			return false;
		}
		const auto& IsActive(void) const noexcept { return m_Active; }
	private:
		OptionWindowClass(void) noexcept {}
		OptionWindowClass(const OptionWindowClass&) = delete;
		OptionWindowClass(OptionWindowClass&& o) = delete;
		OptionWindowClass& operator=(const OptionWindowClass&) = delete;
		OptionWindowClass& operator=(OptionWindowClass&& o) = delete;

		~OptionWindowClass(void) noexcept {}
	public:
		void Init(void) noexcept;
		void Update(void)noexcept;
	};
}
