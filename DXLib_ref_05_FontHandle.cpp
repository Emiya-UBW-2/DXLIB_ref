#include "DXLib_ref.h"
//#include "DXLib_ref_05_FontHandle.hpp"

namespace DXLibRef {
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	const FontPool* SingletonBase<FontPool>::m_Singleton = nullptr;
	const LocalizePool* SingletonBase<LocalizePool>::m_Singleton = nullptr;

	LocalizePool::LocalizePool() {
		Load(LanguageStr[OPTION::Instance()->GetParamInt(EnumSaveParam::Language)]);
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
};
