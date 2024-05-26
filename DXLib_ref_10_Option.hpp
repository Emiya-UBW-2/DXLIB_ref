#pragma once
#include "DXLib_ref.h"

namespace DXLib_ref {
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
		AllWaysFront,
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
	static const char* OptionStr[(int)EnumSaveParam::Max] = {
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

	struct SaveParams {
	private:
		EnumParamType m_EnumParamType{};
		int valueint{0};
	public:
		void SetEnumParamType(EnumParamType value) { m_EnumParamType = value; }
		const auto GetEnumParamType() const { return m_EnumParamType; }

		void SetBoolean(bool use) { valueint = use ? 1 : 0; }
		const auto GetBoolean() const { return (valueint != 0); }
		void ChangeBoolean() { SetBoolean(GetBoolean() ^ 1); }

		void SetInt(int use) { valueint = use; }
		const auto GetInt() const { return valueint; }

		void SetFloat(float use) { valueint = (int)(use*1000.f); }
		const auto GetFloat() const { return (float)(valueint) / 1000.f; }

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
		std::array<SaveParams, (int)EnumSaveParam::Max> m_SaveParams;
	public:
		const auto		GetParamBoolean(EnumSaveParam id)const noexcept { return m_SaveParams.at((int)id).GetBoolean(); }
		const auto		GetParamInt(EnumSaveParam id)const noexcept { return m_SaveParams.at((int)id).GetInt(); }
		const auto		GetParamFloat(EnumSaveParam id)const noexcept { return m_SaveParams.at((int)id).GetFloat(); }
	public:
		void			SetParamBoolean(EnumSaveParam id, bool use) {
			switch (id) {
				case EnumSaveParam::SSAO:
				case EnumSaveParam::AA:
				case EnumSaveParam::DoF:
					if (DirectXVerID[GetParamInt(EnumSaveParam::DirectXVer)] != DX_DIRECT3D_11) { use = false; }
					if (GetParamBoolean(EnumSaveParam::usevr)) { use = false; }
					break;
				case EnumSaveParam::usevr:
#ifndef _USE_OPENVR_
					use = false;
#endif
					if (DirectXVerID[GetParamInt(EnumSaveParam::DirectXVer)] != DX_DIRECT3D_11) { use = false; }
					break;
				case EnumSaveParam::bloom:
					if (GetParamBoolean(EnumSaveParam::usevr)) { use = false; }
					break;
				case EnumSaveParam::ScreenEffect:
					if (DirectXVerID[GetParamInt(EnumSaveParam::DirectXVer)] != DX_DIRECT3D_11) { use = false; }
					break;
				default:
					break;
			}
			m_SaveParams.at((int)id).SetBoolean(use);
		}
		void			ChangeParamBoolean(EnumSaveParam id) { m_SaveParams.at((int)id).ChangeBoolean(); }

		void			SetParamInt(EnumSaveParam id, int use) {
			switch (id) {
				case EnumSaveParam::Reflection:
					if (DirectXVerID[GetParamInt(EnumSaveParam::DirectXVer)] != DX_DIRECT3D_11) { use = false; }
					if (GetParamBoolean(EnumSaveParam::usevr)) { use = false; }
					break;
				case EnumSaveParam::shadow:
					if (DirectXVerID[GetParamInt(EnumSaveParam::DirectXVer)] != DX_DIRECT3D_11) { use = 0; }
					break;
				default:
					break;
			}
			m_SaveParams.at((int)id).SetInt(use);
		}
		void			SetParamFloat(EnumSaveParam id, float use) { m_SaveParams.at((int)id).SetFloat(use); }
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
			int m_Info{0};
			std::function<void()> m_LeftPush;
			std::function<void()> m_RightPush;
			std::function<void()> m_OKPush;
			std::function<void()> m_AnyDoing;
			std::function<void(int xpos, int ypos, bool isMine, float scale)> m_Draw;
		public:
			float selanim{0.f};
		public:
			const auto& GetName() const noexcept { return m_Name; }
			const auto& GetInfoTextID() const noexcept { return m_Info; }

			void GetLeftPush() const noexcept { m_LeftPush(); }
			void GetRightPush() const noexcept { m_RightPush(); }
			void GetOKPush() const noexcept { m_OKPush(); }
			void GetAnyDoing() const noexcept { m_AnyDoing(); }
		public:
			void Init(const char* name, int infoTextID, std::function<void()> LeftPush, std::function<void()> RightPush, std::function<void()> OKPush,
					  std::function<void()> AnyDoing,
					  std::function<void(int xpos, int ypos, bool isMine, float scale)> draw) {
				selanim = 0;

				m_Name = name;
				m_Info = infoTextID;
				m_LeftPush = LeftPush;
				m_RightPush = RightPush;
				m_OKPush = OKPush;
				m_AnyDoing = AnyDoing;
				m_Draw = draw;
			}
			void Draw(int xpos, int ypos, bool isMine, float scale) const noexcept;
		};
		class OptionTabsInfo {
		private:
		protected:
			int m_id{0};
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
			void Draw(int xpos, int ypos, bool isActive, int* TabSel, int *select, float scale) noexcept;

			void DrawInfo(int xpos, int ypos, int select, float scale) noexcept;
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
			void KeyDraw(int xpos, int ypos, bool isMine, int Sel, float scale) noexcept;
		};
		class ElseTabsInfo :public OptionTabsInfo {
		protected:
			void Init_Sub() noexcept override;
		};
	private:
		int m_tabsel{0};
		int m_select{0};
		std::array<std::unique_ptr<OptionTabsInfo>, 4> m_Tabs;
		bool						m_Active{ false };
		bool						m_ActiveSwitch{false};
		bool						m_RestartSwitch{false};
	public:
		void SetRestart() noexcept { m_RestartSwitch = true; }
		void SetActive() noexcept { m_ActiveSwitch = true; }
		const auto IsRestartSwitch() noexcept {
			if (!m_Active && m_RestartSwitch) {
				m_RestartSwitch = false;
				return true;
			}
			return false;
		}
		const auto& IsActive() const noexcept { return m_Active; }
	public:
		void Init() noexcept;
		void Execute(void)noexcept;
	};
}
