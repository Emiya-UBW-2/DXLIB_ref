#include "DXLib_ref_13_Localize.hpp"

namespace DXLibRef {
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	const LocalizePool* SingletonBase<LocalizePool>::m_Singleton = nullptr;

	LocalizePool::LocalizePool(void) noexcept {
		Load();
	}
	void LocalizePool::Load(void) noexcept {
		Dispose();
		auto* OptionParts = OPTION::Instance();
		std::string Path = "data/Localize/";
		Path += LanguageStr[OptionParts->GetParamInt(EnumSaveParam::Language)];
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
				this->havehandle.emplace_back((LocalizeID)(std::stoi(LEFT)), RIGHT.c_str());
			}
		}
	}
};
