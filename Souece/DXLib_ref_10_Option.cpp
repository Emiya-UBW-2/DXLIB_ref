﻿#include "DXLib_ref_10_Option.hpp"

namespace DXLibRef {
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	const OptionManager* SingletonBase<OptionManager>::m_Singleton = nullptr;
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	void			OptionManager::Load(void) noexcept {
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::GraphicsPreset)].SetEnumParamType(EnumParamType::Int);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::DirectXVer)].SetEnumParamType(EnumParamType::Else);// DirectXVer
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::usevr)].SetEnumParamType(EnumParamType::Boolean);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::ObjLevel)].SetEnumParamType(EnumParamType::Int);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::DoF)].SetEnumParamType(EnumParamType::Boolean);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::bloom)].SetEnumParamType(EnumParamType::Boolean);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::shadow)].SetEnumParamType(EnumParamType::Int);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::SSAO)].SetEnumParamType(EnumParamType::Boolean);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::fov)].SetEnumParamType(EnumParamType::Int);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::vsync)].SetEnumParamType(EnumParamType::Boolean);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::FpsLimit)].SetEnumParamType(EnumParamType::Int);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::SE)].SetEnumParamType(EnumParamType::Float);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::VOICE)].SetEnumParamType(EnumParamType::Float);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::BGM)].SetEnumParamType(EnumParamType::Float);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::WindowMode)].SetEnumParamType(EnumParamType::Int);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::ScreenEffect)].SetEnumParamType(EnumParamType::Boolean);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::Reflection)].SetEnumParamType(EnumParamType::Int);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::MotionBlur)].SetEnumParamType(EnumParamType::Boolean);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::Xsensing)].SetEnumParamType(EnumParamType::Float);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::Ysensing)].SetEnumParamType(EnumParamType::Float);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::HeadBobbing)].SetEnumParamType(EnumParamType::Boolean);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::ControlType)].SetEnumParamType(EnumParamType::Else);// ControlType
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::Language)].SetEnumParamType(EnumParamType::Else);// Language
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::AA)].SetEnumParamType(EnumParamType::Boolean);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::DrawScale)].SetEnumParamType(EnumParamType::Float);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::FlatEarth)].SetEnumParamType(EnumParamType::Boolean);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::GodRay)].SetEnumParamType(EnumParamType::Boolean);
		this->m_SaveParams[static_cast<size_t>(EnumSaveParam::ActiveLockOn)].SetEnumParamType(EnumParamType::Boolean);
		
		this->m_ProjectSettingParams[static_cast<size_t>(EnumProjectSettingParam::PBR)].SetEnumParamType(EnumParamType::Boolean);
		this->m_ProjectSettingParams[static_cast<size_t>(EnumProjectSettingParam::Distortion)].SetEnumParamType(EnumParamType::Boolean);
		this->m_ProjectSettingParams[static_cast<size_t>(EnumProjectSettingParam::CubeMap)].SetEnumParamType(EnumParamType::Boolean);

		// SetOutApplicationLogValidFlag(FALSE);
		{
			FileStreamDX FileStream;
			bool NewData = !IsFileExist("Save/Setting.txt");
			if (NewData) {
				WCHAR localeName[LOCALE_NAME_MAX_LENGTH];
				int retVal = GetUserDefaultLocaleName(localeName, ARRAYSIZE(localeName));
				if (retVal > 0) {
					SetParamInt(EnumSaveParam::Language, (StrCmpW(localeName, L"ja-JP") == 0) ? 0 : 1);
				}
				// 共通設定項目
				if (IsFileExist("data/Setting.txt")) {
					FileStream.Open("data/Setting.txt");
				}
				else {
					// デフォ値
					SetParamInt(EnumSaveParam::GraphicsPreset, 3);
					SetParamInt(EnumSaveParam::DirectXVer, 1);
					SetParamBoolean(EnumSaveParam::usevr, false);
					SetParamInt(EnumSaveParam::ObjLevel, 3);
					SetParamBoolean(EnumSaveParam::DoF, true);
					SetParamBoolean(EnumSaveParam::bloom, true);
					SetParamInt(EnumSaveParam::shadow, 3);
					SetParamBoolean(EnumSaveParam::SSAO, true);
					SetParamInt(EnumSaveParam::fov, 90);
					SetParamBoolean(EnumSaveParam::vsync, true);
					SetParamInt(EnumSaveParam::FpsLimit, 60);
					SetParamFloat(EnumSaveParam::SE, 0.5f);
					SetParamFloat(EnumSaveParam::VOICE, 0.5f);
					SetParamFloat(EnumSaveParam::BGM, 0.5f);
					SetParamBoolean(EnumSaveParam::WindowMode, static_cast<int>(WindowType::Borderless));
					SetParamBoolean(EnumSaveParam::ScreenEffect, true);
					SetParamInt(EnumSaveParam::Reflection, 2);
					SetParamBoolean(EnumSaveParam::MotionBlur, false);
					SetParamFloat(EnumSaveParam::Xsensing, 0.5f);
					SetParamFloat(EnumSaveParam::Ysensing, 0.5f);
					SetParamBoolean(EnumSaveParam::HeadBobbing, true);
					SetParamInt(EnumSaveParam::ControlType, 2);
					SetParamBoolean(EnumSaveParam::AA, true);
					SetParamFloat(EnumSaveParam::DrawScale, 1.f);
					SetParamBoolean(EnumSaveParam::FlatEarth, false);
					SetParamBoolean(EnumSaveParam::GodRay, true);
					SetParamBoolean(EnumSaveParam::ActiveLockOn, true);
					return;
				}
			}
			else {
				FileStream.Open("Save/Setting.txt");
			}

			while (true) {
				if (FileStream.ComeEof()) { break; }
				auto ALL = FileStream.SeekLineAndGetStr();
				if (ALL == "") { continue; }
				//=の右側の文字をカンマ区切りとして識別する
				auto LEFT = FileStreamDX::getleft(ALL);
				auto RIGHT = FileStreamDX::getright(ALL);
				for (size_t loop : std::views::iota(0, static_cast<int>(EnumSaveParam::Max))) {
					if (LEFT != OptionStr[loop]) {
						continue;
					}
					switch (this->m_SaveParams[loop].GetEnumParamType()) {
					case EnumParamType::Boolean:
						SetParamBoolean((EnumSaveParam)loop, (RIGHT.find("true") != std::string::npos));
						break;
					case EnumParamType::Int:
						if (std::all_of(RIGHT.cbegin(), RIGHT.cend(), isdigit)) {
							SetParamInt((EnumSaveParam)loop, std::stoi(RIGHT));
						}
						else {
							SetParamInt((EnumSaveParam)loop, 0);
						}
						break;
					case EnumParamType::Float:
						SetParamFloat((EnumSaveParam)loop, std::stof(RIGHT));// todo 小数か確認
						break;
					case EnumParamType::Else:
						if (loop == static_cast<size_t>(EnumSaveParam::DirectXVer)) {
							for (size_t loop2 : std::views::iota(0, 2)) {
								if (RIGHT == DirectXVerStr[loop2]) {
									SetParamInt((EnumSaveParam)loop, static_cast<int>(loop2));
									break;
								}
							}
						}
						else if (loop == static_cast<size_t>(EnumSaveParam::ControlType)) {
							for (size_t loop2 : std::views::iota(1, 3)) {
								if (RIGHT == Controls::ControlTypeStr[loop2]) {
									SetParamInt((EnumSaveParam)loop, static_cast<int>(loop2));
									break;
								}
							}
						}
						else if (loop == static_cast<size_t>(EnumSaveParam::Language) && !NewData) {
							for (size_t loop2 : std::views::iota(0, 2)) {
								if (RIGHT == LanguageStr[loop2]) {
									SetParamInt((EnumSaveParam)loop, static_cast<int>(loop2));
									break;
								}
							}
						}
						break;
					default:
						break;
					}
					break;
				}
			}
		}
		{
			if (!IsFileExist("CommonData/ProjectSetting.txt")) {
				SetParamBoolean(EnumProjectSettingParam::PBR, true);
				SetParamBoolean(EnumProjectSettingParam::Distortion, true);
				SetParamBoolean(EnumProjectSettingParam::CubeMap, true);
				return;
			}
			FileStreamDX FileStream("CommonData/ProjectSetting.txt");
			while (true) {
				if (FileStream.ComeEof()) { break; }
				auto ALL = FileStream.SeekLineAndGetStr();
				if (ALL == "") { continue; }
				//=の右側の文字をカンマ区切りとして識別する
				auto LEFT = FileStreamDX::getleft(ALL);
				auto RIGHT = FileStreamDX::getright(ALL);
				for (size_t loop : std::views::iota(0, static_cast<int>(EnumProjectSettingParam::Max))) {
					if (LEFT != ProjectSettingStr[loop]) {
						continue;
					}
					switch (this->m_ProjectSettingParams[loop].GetEnumParamType()) {
					case EnumParamType::Boolean:
						SetParamBoolean((EnumProjectSettingParam)loop, (RIGHT.find("true") != std::string::npos));
						break;
					case EnumParamType::Int:
						if (std::all_of(RIGHT.cbegin(), RIGHT.cend(), isdigit)) {
							SetParamInt((EnumProjectSettingParam)loop, std::stoi(RIGHT));
						}
						else {
							SetParamInt((EnumProjectSettingParam)loop, 0);
						}
						break;
					case EnumParamType::Float:
						SetParamFloat((EnumProjectSettingParam)loop, std::stof(RIGHT));// todo 小数か確認
						break;
					case EnumParamType::Else:
						break;
					default:
						break;
					}
					break;
				}
			}
		}
		// SetOutApplicationLogValidFlag(TRUE);
	}
	void			OptionManager::Save(void) const noexcept {
		std::ofstream outputfile("Save/Setting.txt");

		for (size_t loop : std::views::iota(static_cast<size_t>(0), static_cast<size_t>(EnumSaveParam::Max))) {
			switch (this->m_SaveParams[loop].GetEnumParamType()) {
			case EnumParamType::Boolean:
				outputfile << std::string(OptionStr[loop]) + "=" + std::string(GetParamBoolean(static_cast<EnumSaveParam>(loop)) ? "true" : "false") + "\n";
				break;
			case EnumParamType::Int:
				outputfile << std::string(OptionStr[loop]) + "=" + std::to_string(GetParamInt(static_cast<EnumSaveParam>(loop))) + "\n";
				break;
			case EnumParamType::Float:
				outputfile << std::string(OptionStr[loop]) + "=" + std::to_string(GetParamFloat(static_cast<EnumSaveParam>(loop))) + "\n";
				break;
			case EnumParamType::Else:
				if (loop == static_cast<size_t>(EnumSaveParam::DirectXVer)) {
					outputfile << std::string(OptionStr[loop]) + "=" + std::string(DirectXVerStr[GetParamInt(static_cast<EnumSaveParam>(loop))]) + "\n";
				}
				else if (loop == static_cast<size_t>(EnumSaveParam::ControlType)) {
					outputfile << std::string(OptionStr[loop]) + "=" + std::string(Controls::ControlTypeStr[GetParamInt(static_cast<EnumSaveParam>(loop))]) + "\n";
				}
				else if (loop == static_cast<size_t>(EnumSaveParam::Language)) {
					outputfile << std::string(OptionStr[loop]) + "=" + std::string(LanguageStr[GetParamInt(static_cast<EnumSaveParam>(loop))]) + "\n";
				}
				break;
			default:
				break;
			}
		}

		outputfile.close();
	}
};
