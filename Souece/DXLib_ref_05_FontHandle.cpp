#include "DXLib_ref.h"
// #include "DXLib_ref_05_FontHandle.hpp"

namespace DXLibRef {
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	const FontSystem::FontPool* SingletonBase<FontSystem::FontPool>::m_Singleton = nullptr;

	namespace FontSystem {
		Fonthave::Fonthave(FontType type, int fontSize, int edgeSize) noexcept {
			this->m_Type = type;
			this->m_EdgeSize = edgeSize;
			this->m_CustomSize = fontSize;
			if (fontSize <= 16) {
				this->m_CommonSize = 16;
			}
			else if (fontSize <= 24) {
				this->m_CommonSize = 24;
			}
			else {
				this->m_CommonSize = 32;
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
			Str += std::to_string(this->m_CommonSize);
			Str += ".dft";
			this->m_scaleType = DX_DRAWMODE_BILINEAR;
			this->m_Handle.Load(Str, this->m_EdgeSize);
		}
	}
}
