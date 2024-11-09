/*=============================================================================
 Copyright (C) 2020 yumetodo <yume-wikijp@live.jp>
 Distributed under the Boost Software License, Version 1.0.
 (See https:// www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {

	namespace draw_string_center_impl {
		static std::vector<DRAWCHARINFO> get_draw_string_char_info(const std::basic_string<TCHAR>& string, int font_handle) {
			std::vector<DRAWCHARINFO> info;
			info.resize(string.size());
			auto char_info_num = GetDrawStringCharInfoToHandle(info.data(), info.size(), string.c_str(), static_cast<int>(string.length() * sizeof(TCHAR)), font_handle, false);
			if (char_info_num < 0) throw std::runtime_error("fail in function DxLib::GetDrawStringCharInfoToHandle");
			if (info.size() < static_cast<std::size_t>(char_info_num)) {
				info.resize(static_cast<std::size_t>(char_info_num) + 1);
				// 再取得
				char_info_num = GetDrawStringCharInfoToHandle(info.data(), info.size(), string.c_str(), static_cast<int>(string.length() * sizeof(TCHAR)), font_handle, false);
				if (char_info_num < 0 || static_cast<int>(info.size()) < char_info_num) throw std::runtime_error("fail to detect draw info.");
			}
			info.resize(static_cast<std::size_t>(char_info_num));
			return info;
		}
	}

	static float draw_string_center(
		float draw_area_x_left, float draw_area_x_right, float draw_area_y_top, float draw_area_y_bottom,
		const std::basic_string<TCHAR>& string,
		unsigned int color, int font_handle,
		unsigned int edge_color = 0
	) {
		if (0 == string.length()) throw std::invalid_argument("empty string not allowed.");
		if (draw_area_x_right < draw_area_x_left || draw_area_y_bottom < draw_area_y_top) throw std::invalid_argument("");

		// 一文字ずつの描画幅情報を取得する
		const auto info = draw_string_center_impl::get_draw_string_char_info(string, font_handle);

		// ManageData->LineSpaceValidFlag ? ManageData->LineSpace : ManageData->BaseInfo.FontHeight
		const auto line_space = DxLib::GetFontLineSpaceToHandle(font_handle);
		const float area_width = draw_area_x_right - draw_area_x_left;
		const auto total_draw_width = info.back().DrawX + info.back().SizeX - info.front().DrawX;
		if (total_draw_width <= area_width) {
			// 一行ですむ場合
			const float padding = (area_width - total_draw_width) / 2.0f;
			DxLib::DrawStringFToHandle(draw_area_x_left + padding, draw_area_y_top, string.c_str(), color, font_handle, edge_color, false);
			return static_cast<float>(line_space);
		}

		// 複数行になる場合

		const float area_height = draw_area_y_bottom - draw_area_y_top;

		// 描画開始
		std::size_t current_string_byte_pos = 0;
		std::size_t line_front_string_byte_pos = 0;
		float current_y_relative = 0.0f;
		auto line_front_it = info.begin();
		for (auto it = info.begin(); it < info.end(); current_string_byte_pos += it->Bytes, ++it) {
			const auto line_width_contain_current_it_point_char = it->DrawX + it->SizeX - line_front_it->DrawX;
			if (area_width < line_width_contain_current_it_point_char) {
				using namespace std::string_literals;
				// 次の行に行く前に描画、itが指す文字は含まない
				const std::size_t str_len_byte = current_string_byte_pos - line_front_string_byte_pos;
				// it->DrawXは前の文字の右端に等しい
				// const float line_width = it->DrawX - line_front_it->DrawX;
				const float padding = 0.f;// (area_width - line_width) / 2.0f;
				const auto line_string = string.substr(line_front_string_byte_pos / sizeof(TCHAR), (str_len_byte / sizeof(TCHAR)));
				DxLib::DrawStringFToHandle(draw_area_x_left + padding, draw_area_y_top + current_y_relative, line_string.c_str(), color, font_handle, edge_color, false);
				// itが指す文字が先頭になる
				line_front_string_byte_pos = current_string_byte_pos;
				current_y_relative += static_cast<float>(line_space);
				line_front_it = it;
				if (area_height < current_y_relative) return current_y_relative;// 描画可能領域(y)を超えたら終了
			}
		}
		// 最終行の描画
		// const auto last_line_width = info.back().DrawX + info.back().SizeX - line_front_it->DrawX;
		const float padding = 0.f;// (area_width - last_line_width) / 2.0f;
		const auto line_string = string.substr(line_front_string_byte_pos / sizeof(TCHAR));
		DxLib::DrawStringFToHandle(draw_area_x_left + padding, draw_area_y_top + current_y_relative, line_string.c_str(), color, font_handle, edge_color, false);
		return current_y_relative + static_cast<float>(line_space);
	}


	class FontHandle : public DXHandle {
	public:
		enum class FontXCenter : int {
			LEFT,
			MIDDLE,
			RIGHT,
		};
		enum class FontYCenter : int {
			TOP,
			MIDDLE,
			BOTTOM,
		};
	protected:
		void Dispose_Sub(void) noexcept override {
			DeleteFontToHandle(DXHandle::get());
		}
	public:
		// 長さ取得
		int GetDrawWidth(std::basic_string_view<TCHAR> String) const noexcept { return DxLib::GetDrawNStringWidthToHandle(String.data(), String.size(), DXHandle::get(), FALSE); }
		template <typename... Args>
		int GetDrawWidthFormat(const std::string& String, Args&&... args) const noexcept { return DxLib::GetDrawFormatStringWidthToHandle(DXHandle::get(), String.c_str(), args...); }
		int GetDrawExtendWidth(float siz, std::basic_string_view<TCHAR> String) const noexcept { return DxLib::GetDrawExtendNStringWidthToHandle(static_cast<double>(siz), String.data(), String.size(), DXHandle::get(), FALSE); }
		template <typename... Args>
		int GetDrawExtendWidthFormat(float siz, const std::string& String, Args&&... args) const noexcept { return DxLib::GetDrawExtendFormatStringWidthToHandle(static_cast<double>(siz), DXHandle::get(), String.c_str(), args...); }
		// 文字描画
		template <typename... Args>
		void DrawString(FontXCenter FontX, FontYCenter FontY, int x, int y, unsigned int Color, unsigned int EdgeColor, const std::string& String, Args&&... args) const noexcept {
			switch (FontY) {
			case FontYCenter::TOP:
				break;
			case FontYCenter::MIDDLE:
				y -= GetFontSizeToHandle(DXHandle::get()) / 2;
				break;
			case FontYCenter::BOTTOM:
				y -= GetFontSizeToHandle(DXHandle::get());
				break;
			default:
				break;
			}
			switch (FontX) {
			case FontXCenter::LEFT:
				break;
			case FontXCenter::MIDDLE:
				x -= GetDrawWidthFormat(String, args...) / 2;
				break;
			case FontXCenter::RIGHT:
				x -= GetDrawWidthFormat(String, args...);
				break;
			default:
				break;
			}
			DxLib::DrawFormatString2ToHandle(x, y, Color, EdgeColor, DXHandle::get(), String.c_str(), args...);
		}
		template <typename... Args>
		void DrawExtendString(FontXCenter FontX, FontYCenter FontY, int x, int y, float xsiz, float ysiz, unsigned int Color, unsigned int EdgeColor, const std::string& String, Args&&... args) const noexcept {
			switch (FontY) {
			case FontYCenter::TOP:
				break;
			case FontYCenter::MIDDLE:
				y -= static_cast<int>(static_cast<float>(GetFontSizeToHandle(DXHandle::get()) / 2) * ysiz);
				break;
			case FontYCenter::BOTTOM:
				y -= static_cast<int>(static_cast<float>(GetFontSizeToHandle(DXHandle::get())) * ysiz);
				break;
			default:
				break;
			}
			switch (FontX) {
			case FontXCenter::LEFT:
				break;
			case FontXCenter::MIDDLE:
				x -= GetDrawExtendWidthFormat(xsiz, String.c_str(), args...) / 2;
				break;
			case FontXCenter::RIGHT:
				x -= GetDrawExtendWidthFormat(xsiz, String.c_str(), args...);
				break;
			default:
				break;
			}
			DxLib::DrawExtendFormatString2ToHandle(x, y, static_cast<double>(xsiz), static_cast<double>(ysiz), Color, EdgeColor, DXHandle::get(), String.c_str(), args...);
		}
		// 文字描画
		auto DrawStringAutoFit(int x1, int y1, int x2, int y2, unsigned int Color, unsigned int EdgeColor, const std::string& String) const noexcept {
			return draw_string_center(static_cast<float>(x1), static_cast<float>(x2), static_cast<float>(y1), static_cast<float>(y2), String, Color, DXHandle::get(), EdgeColor);
		}
	public:
		// DXフォント用ハンドル作成
		void Load(std::basic_string_view<TCHAR> FontDataPath, int EdgeSize) noexcept {
			DXHandle::SetHandleDirect(DxLib::LoadFontDataToHandleWithStrLen(FontDataPath.data(), FontDataPath.length(), EdgeSize));
		}
	};

	// フォントプール
	class FontPool : public SingletonBase<FontPool> {
	private:
		friend class SingletonBase<FontPool>;
	public:
		enum class FontType {
			MS_Gothic,			// MSゴシック
			DIZ_UD_Gothic,		// DIZ UD ゴシック
		};
		class Fonthave {
			// カスタム項目
			FontType		m_Type{ 0 };
			int				m_EdgeSize{ InvalidID };// エッジサイズ
			int				m_CustomSize{ 0 };// フォントハンドル固有のサイズ
			// 
			int				m_scaleType{ DX_DRAWMODE_BILINEAR };
			int				m_commonsize{ 0 };// フォントハンドル固有のサイズ
			FontHandle		m_Handle;
		public:
			Fonthave(void) noexcept {}
			Fonthave(const Fonthave&) = delete;
			Fonthave(Fonthave&& o) = delete;
			Fonthave& operator=(const Fonthave&) = delete;
			Fonthave& operator=(Fonthave&& o) = delete;

			~Fonthave(void) noexcept {}
		public:
			bool			Equal(FontType type, int fontSize, int edgeSize) const noexcept {
				return ((this->m_Type == type) && (this->m_CustomSize == fontSize) && (this->m_EdgeSize == edgeSize));
			}
		public:
			template <typename... Args>
			void			DrawString(int fontSize, FontHandle::FontXCenter FontX, FontHandle::FontYCenter FontY, int x, int y, unsigned int Color, unsigned int EdgeColor, const std::string& String, Args&&... args) const noexcept {
				if (fontSize == InvalidID) {
					fontSize = this->m_CustomSize;
				}
				if (fontSize == this->m_commonsize) {
					this->m_Handle.DrawString(FontX, FontY, x, y, Color, EdgeColor, String, args...);
				}
				else {
					auto prev = GetDrawMode();
					SetDrawMode(this->m_scaleType);
					this->m_Handle.DrawExtendString(FontX, FontY, x, y, static_cast<float>(fontSize) / static_cast<float>(this->m_commonsize), static_cast<float>(fontSize) / static_cast<float>(this->m_commonsize), Color, EdgeColor, String, args...);
					SetDrawMode(prev);
				}
			}

			auto			DrawStringAutoFit(int x1, int y1, int x2, int y2, unsigned int Color, unsigned int EdgeColor, const std::string& String) const noexcept {
				return this->m_Handle.DrawStringAutoFit(x1, y1, x2, y2, Color, EdgeColor, String);
			}

			template <typename... Args>
			auto			GetStringWidth(int fontSize, const std::string& String, Args&&... args) const noexcept {
				if (fontSize == InvalidID) {
					fontSize = this->m_CustomSize;
				}
				if (fontSize == this->m_commonsize) {
					return this->m_Handle.GetDrawWidthFormat(String, args...);
				}
				else {
					return this->m_Handle.GetDrawExtendWidthFormat(static_cast<float>(fontSize) / static_cast<float>(this->m_commonsize), String, args...);
				}
			}

			void			Set(FontType type, int fontSize, int edgeSize) noexcept;
		};
	private:
		std::vector<std::unique_ptr<Fonthave>> m_Pools;
	private:
		FontPool(void) noexcept {}
		FontPool(const FontPool&) = delete;
		FontPool(FontPool&& o) = delete;
		FontPool& operator=(const FontPool&) = delete;
		FontPool& operator=(FontPool&& o) = delete;

		~FontPool(void) noexcept {}
	private:
		size_t Add(FontType type, int fontSize, int edgeSize) noexcept {
			for (size_t index = 0; auto & h : this->m_Pools) {
				if (h->Equal(type, fontSize, edgeSize)) {
					return index;
				}
				index++;
			}
			this->m_Pools.emplace_back(std::make_unique<Fonthave>());
			this->m_Pools.back()->Set(type, fontSize, edgeSize);
			return this->m_Pools.size() - 1;
		}
	public:
		std::unique_ptr<Fonthave>& Get(FontType type, int fontSize, int edgeSize) noexcept { return this->m_Pools[Add(type, fontSize, edgeSize)]; }
	};


	// ローカライズプール
	class LocalizePool : public SingletonBase<LocalizePool> {
	private:
		friend class SingletonBase<LocalizePool>;
	public:
		typedef int LocalizeID;
	private:
		struct LocalizeStr {
			LocalizeID m_ID{ 0 };
			char m_Str[512]{};
		};

		std::vector<LocalizeStr> havehandle;
	private:
		LocalizePool(void) noexcept;
		LocalizePool(const LocalizePool&) = delete;
		LocalizePool(LocalizePool&& o) = delete;
		LocalizePool& operator=(const LocalizePool&) = delete;
		LocalizePool& operator=(LocalizePool&& o) = delete;

		~LocalizePool(void) noexcept {
			Dispose();
		}
	public:
		void Load(const char* Lang) noexcept;
		void Dispose(void) noexcept {
			this->havehandle.clear();
			this->havehandle.shrink_to_fit();
		}
	public:
		const char* Get(LocalizeID type) noexcept {
			for (auto& s : this->havehandle) {
				if (s.m_ID == type) {
					return s.m_Str;
				}
			}
			return "";
		}
	};

};
