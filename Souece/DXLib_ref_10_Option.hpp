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
		FlatEarth,
		GodRay,
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
		"FlatEarth",
		"GodRay",
	};

	enum class EnumProjectSettingParam {
		Distortion,
		PBR,
		CubeMap,
		Max,
	};
	static const char* ProjectSettingStr[static_cast<int>(EnumProjectSettingParam::Max)] = {
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

	class SaveParams {
	private:
		EnumParamType	m_EnumParamType{};
		int				m_valueint{ 0 };
	public:
		void SetEnumParamType(EnumParamType value) noexcept { this->m_EnumParamType = value; }
		auto GetEnumParamType(void) const noexcept { return this->m_EnumParamType; }

		void SetBoolean(bool use) noexcept { this->m_valueint = use ? 1 : 0; }
		auto GetBoolean(void) const noexcept { return (this->m_valueint != 0); }
		void ChangeBoolean(void) noexcept { SetBoolean(GetBoolean() ^ 1); }

		void SetInt(int use) noexcept { this->m_valueint = use; }
		auto GetInt(void) const noexcept { return this->m_valueint; }

		void SetFloat(float use) noexcept { this->m_valueint = static_cast<int>(use * 1000.f); }
		auto GetFloat(void) const noexcept { return static_cast<float>(this->m_valueint) / 1000.f; }

	};

	class OptionManager : public SingletonBase<OptionManager> {
	private:
		friend class SingletonBase<OptionManager>;
	private:
		OptionManager(void) noexcept {
			Load();
		}
		OptionManager(const OptionManager&) = delete;
		OptionManager(OptionManager&&) = delete;
		OptionManager& operator=(const OptionManager&) = delete;
		OptionManager& operator=(OptionManager&&) = delete;

		virtual ~OptionManager(void) noexcept {}
	private:
		std::array<SaveParams, static_cast<int>(EnumSaveParam::Max)>	m_SaveParams;
		std::array<SaveParams, static_cast<int>(EnumProjectSettingParam::Max)>	m_ProjectSettingParams;
	public:
		auto		GetParamBoolean(EnumSaveParam id)const noexcept { return this->m_SaveParams[static_cast<size_t>(id)].GetBoolean(); }
		auto		GetParamInt(EnumSaveParam id)const noexcept { return this->m_SaveParams[static_cast<size_t>(id)].GetInt(); }
		auto		GetParamFloat(EnumSaveParam id)const noexcept { return this->m_SaveParams[static_cast<size_t>(id)].GetFloat(); }
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
			case EnumSaveParam::FlatEarth:
			case EnumSaveParam::Max:
			default:
				break;
			}
			this->m_SaveParams.at(static_cast<size_t>(id)).SetBoolean(use);
		}
		void			ChangeParamBoolean(EnumSaveParam id) noexcept { this->m_SaveParams[static_cast<size_t>(id)].ChangeBoolean(); }

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
			case EnumSaveParam::FlatEarth:
			case EnumSaveParam::Max:
			default:
				break;
			}
			this->m_SaveParams.at(static_cast<size_t>(id)).SetInt(use);
		}
		void			SetParamFloat(EnumSaveParam id, float use) noexcept { this->m_SaveParams[static_cast<size_t>(id)].SetFloat(use); }
	public:
		auto		GetParamBoolean(EnumProjectSettingParam id)const noexcept { return this->m_ProjectSettingParams[static_cast<size_t>(id)].GetBoolean(); }
		auto		GetParamInt(EnumProjectSettingParam id)const noexcept { return this->m_ProjectSettingParams[static_cast<size_t>(id)].GetInt(); }
		auto		GetParamFloat(EnumProjectSettingParam id)const noexcept { return this->m_ProjectSettingParams[static_cast<size_t>(id)].GetFloat(); }
	public:
		void			SetParamBoolean(EnumProjectSettingParam id, bool use) noexcept { this->m_ProjectSettingParams[static_cast<size_t>(id)].SetBoolean(use); }
		void			SetParamInt(EnumProjectSettingParam id, int use) noexcept { this->m_ProjectSettingParams[static_cast<size_t>(id)].SetInt(use); }
		void			SetParamFloat(EnumProjectSettingParam id, float use) noexcept { this->m_ProjectSettingParams[static_cast<size_t>(id)].SetFloat(use); }
	public:
		void			Load(void) noexcept;
		void			Save(void) const noexcept;
	};
}
