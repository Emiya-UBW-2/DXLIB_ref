#include "DXLib_ref.h"
//#include "DXLib_ref_05_FontHandle.hpp"

namespace DXLibRef {
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	const FontPool* SingletonBase<FontPool>::m_Singleton = nullptr;
	const LocalizePool* SingletonBase<LocalizePool>::m_Singleton = nullptr;

	LocalizePool::LocalizePool(void) noexcept {
		auto* OptionParts = OPTION::Instance();
		Load(LanguageStr[OptionParts->GetParamInt(EnumSaveParam::Language)]);
	}

	void LocalizePool::Load(const char* Lang) {
		std::string Path = "data/Localize/";
		Path += Lang;
		Path += ".txt";
		this->havehandle.reserve(256);
		int mdata = FileRead_open(Path.c_str(), FALSE);
		while (true) {
			if (FileRead_eof(mdata) != 0) { break; }
			auto ALL = getparams::Getstr(mdata);
			if (ALL == "") { continue; }
			auto LEFT = getparams::getleft(ALL);
			auto RIGHT = getparams::getright(ALL);
			LocalizeID ID = (LocalizeID)(std::stoi(LEFT));
			bool IsHit = false;
			for (auto& h : this->havehandle) {
				if (h.m_ID == ID) {
					std::string Before = h.m_Str;
					sprintfDx(h.m_Str, "%s\n%s", Before.c_str(), RIGHT.c_str());//改行して同じテキストとする
					IsHit = true;
					break;
				}
			}
			if (!IsHit) {
				this->havehandle.resize(this->havehandle.size() + 1);
				this->havehandle.back().m_ID = (LocalizeID)(std::stoi(LEFT));
				sprintfDx(this->havehandle.back().m_Str, RIGHT.c_str());
			}
		}
		FileRead_close(mdata);
	}
	void FontPool::Fonthave::Set(FontType type, int fontSize) noexcept {
		auto* DrawParts = DXDraw::Instance();
		auto* LocalizeParts = LocalizePool::Instance();
		this->m_fontsize = fontSize;
		this->m_Type = type;
		switch (this->m_Type) {
		case FontType::Nomal_Edge:
			this->m_size = 32;
			this->m_scaleType = DX_DRAWMODE_BILINEAR;
			if (this->m_fontsize != INVALID_ID) {
				this->m_Handle = FontHandle::Create(LocalizeParts->Get(0), this->m_fontsize, DX_FONTTYPE_EDGE, INVALID_ID, DrawParts->GetUIX(1));
			}
			else {
				this->m_Handle = FontHandle::Create(LocalizeParts->Get(0), this->m_size, DX_FONTTYPE_ANTIALIASING_EDGE, INVALID_ID, DrawParts->GetUIX(1));
			}
			break;
		case FontType::Nomal_EdgeL:
			this->m_size = 32;
			this->m_scaleType = DX_DRAWMODE_BILINEAR;
			if (this->m_fontsize != INVALID_ID) {
				this->m_Handle = FontHandle::Create(LocalizeParts->Get(0), this->m_fontsize, DX_FONTTYPE_EDGE, INVALID_ID, DrawParts->GetUIX(3));
			}
			else {
				this->m_Handle = FontHandle::Create(LocalizeParts->Get(0), this->m_size, DX_FONTTYPE_ANTIALIASING_EDGE, INVALID_ID, DrawParts->GetUIX(3));
			}
			break;
		case FontType::Nomal_AA:
			this->m_size = 92;
			this->m_scaleType = DX_DRAWMODE_NEAREST;
			if (this->m_fontsize != INVALID_ID) {
				this->m_Handle = FontHandle::Create(LocalizeParts->Get(0), this->m_fontsize, DX_FONTTYPE_NORMAL, INVALID_ID, INVALID_ID);
			}
			else {
				this->m_Handle = FontHandle::Create(LocalizeParts->Get(0), this->m_size, DX_FONTTYPE_ANTIALIASING, INVALID_ID, INVALID_ID);
			}
			break;
		case FontType::Nomal_ItalicAA:
			this->m_size = 92;
			this->m_scaleType = DX_DRAWMODE_NEAREST;
			this->m_Handle = FontHandle::Create(LocalizeParts->Get(0), (this->m_fontsize != INVALID_ID) ? this->m_fontsize : this->m_size, DX_FONTTYPE_NORMAL, INVALID_ID, INVALID_ID, true);
			break;
		case FontType::Gothic_Edge:
			this->m_size = 32;
			this->m_scaleType = DX_DRAWMODE_BILINEAR;
			if (this->m_fontsize != INVALID_ID) {
				this->m_Handle = FontHandle::Create(LocalizeParts->Get(1), this->m_fontsize, DX_FONTTYPE_EDGE, INVALID_ID, DrawParts->GetUIX(1));
			}
			else {
				this->m_Handle = FontHandle::Create(LocalizeParts->Get(1), this->m_size, DX_FONTTYPE_ANTIALIASING_EDGE, INVALID_ID, DrawParts->GetUIX(1));
			}
			break;
		case FontType::Gothic_AA:
			this->m_size = 32;
			this->m_scaleType = DX_DRAWMODE_BILINEAR;
			if (this->m_fontsize != INVALID_ID) {
				this->m_Handle = FontHandle::Create(LocalizeParts->Get(1), this->m_fontsize, DX_FONTTYPE_NORMAL, INVALID_ID, INVALID_ID);
			}
			else {
				this->m_Handle = FontHandle::Create(LocalizeParts->Get(1), this->m_size, DX_FONTTYPE_ANTIALIASING, INVALID_ID, INVALID_ID);
			}
			break;
		case FontType::WW_Gothic:
			this->m_size = 32;
			this->m_scaleType = DX_DRAWMODE_BILINEAR;
			if (this->m_fontsize != INVALID_ID) {
				this->m_Handle = FontHandle::Create("MS UI Gothic", this->m_fontsize, DX_FONTTYPE_EDGE, INVALID_ID, DrawParts->GetUIX(3));
			}
			else {
				this->m_Handle = FontHandle::Create("MS UI Gothic", this->m_size, DX_FONTTYPE_ANTIALIASING_EDGE, INVALID_ID, DrawParts->GetUIX(3));
			}
			break;
		default:
			break;
		}
	}
};
