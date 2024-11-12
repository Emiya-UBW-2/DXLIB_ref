#include "DXLib_ref.h"
// #include "DXLib_ref_05_FontHandle.hpp"

namespace DXLibRef {
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	const FontPool* SingletonBase<FontPool>::m_Singleton = nullptr;
	const LocalizePool* SingletonBase<LocalizePool>::m_Singleton = nullptr;

	void FontPool::Fonthave::Set(FontType type, int fontSize, int edgeSize) noexcept {
		this->m_Type = type;
		this->m_EdgeSize = edgeSize;
		this->m_CustomSize = fontSize;
		if (fontSize <= 16) {
			this->m_commonsize = 16;
		}
		else if (fontSize <= 24) {
			this->m_commonsize = 24;
		}
		else {
			this->m_commonsize = 32;
		}

		std::string Str;

		switch (this->m_Type) {
		case FontType::MS_Gothic:
			Str = "CommonData/Font/MSG_";
			break;
		case FontType::DIZ_UD_Gothic:
			Str = "CommonData/Font/BIZUDG_";
			break;
		default:
			break;
		}
		Str += std::to_string(this->m_commonsize);
		Str += ".dft";
		this->m_scaleType = DX_DRAWMODE_BILINEAR;
		this->m_Handle.Load(Str, this->m_EdgeSize);
	}

	LocalizePool::LocalizePool(void) noexcept {
		auto* OptionParts = OPTION::Instance();
		Load(LanguageStr[OptionParts->GetParamInt(EnumSaveParam::Language)]);
	}
	void LocalizePool::Load(const char* Lang) noexcept {
		std::string Path = "data/Localize/";
		Path += Lang;
		Path += ".txt";
		this->havehandle.reserve(256);
		FileStreamDX FileStream(Path.c_str());
		while (true) {
			if (FileStream.ComeEof()) { break; }
			auto ALL = FileStream.SeekLineAndGetStr();
			if (ALL == "") { continue; }
			//=の右側の文字をカンマ区切りとして識別する
			auto LEFT = FileStreamDX::getleft(ALL);
			auto RIGHT = FileStreamDX::getright(ALL);
			LocalizeID ID = (LocalizeID)(std::stoi(LEFT));
			bool IsHit = false;
			for (auto& h : this->havehandle) {
				if (h.m_ID == ID) {
					std::string Before = h.m_Str;
					sprintfDx(h.m_Str, "%s\n%s", Before.c_str(), RIGHT.c_str());// 改行して同じテキストとする
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
	}
};
