/*=============================================================================
  Copyright (C) 2020 yumetodo <yume-wikijp@live.jp>
  Distributed under the Boost Software License, Version 1.0.
  (See https://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#pragma once
#include "DXLib_ref.h"

namespace DXLib_ref {

	namespace draw_string_center_impl {
		static std::vector<DRAWCHARINFO> get_draw_string_char_info(const std::basic_string<TCHAR>& string, int font_handle) {
			std::vector<DRAWCHARINFO> info;
			info.resize(string.size());
			auto char_info_num = GetDrawStringCharInfoToHandle(info.data(), info.size(), string.c_str(), (int)(string.length() * sizeof(TCHAR)), font_handle, false);
			if (char_info_num < 0) throw std::runtime_error("fail in function DxLib::GetDrawStringCharInfoToHandle");
			if (info.size() < static_cast<std::size_t>(char_info_num)) {
				info.resize(char_info_num + 1);
				//再取得
				char_info_num = GetDrawStringCharInfoToHandle(info.data(), info.size(), string.c_str(), (int)(string.length() * sizeof(TCHAR)), font_handle, false);
				if (char_info_num < 0 || info.size() < static_cast<std::size_t>(char_info_num)) throw std::runtime_error("fail to detect draw info.");
			}
			info.resize(char_info_num);
			return info;
		}
	}

	static float draw_string_center(
		float draw_area_x_left, float draw_area_x_right, float draw_area_y_top, float draw_area_y_bottom,
		const std::basic_string<TCHAR>& string,
		unsigned int color, int font_handle,
		unsigned int edge_color = 0
	)
	{
		if (0 == string.length())  throw std::invalid_argument("empty string not allowed.");
		if (draw_area_x_right < draw_area_x_left || draw_area_y_bottom < draw_area_y_top) throw std::invalid_argument("");

		//一文字ずつの描画幅情報を取得する
		const auto info = draw_string_center_impl::get_draw_string_char_info(string, font_handle);

		//ManageData->LineSpaceValidFlag ? ManageData->LineSpace : ManageData->BaseInfo.FontHeight
		const auto line_space = DxLib::GetFontLineSpaceToHandle(font_handle);
		const float area_width = draw_area_x_right - draw_area_x_left;
		const auto total_draw_width = info.back().DrawX + info.back().SizeX - info.front().DrawX;
		if (total_draw_width <= area_width) {
			//一行ですむ場合
			const float padding = (area_width - total_draw_width) / 2.0f;
			DxLib::DrawStringFToHandle(draw_area_x_left + padding, draw_area_y_top, string.c_str(), color, font_handle, edge_color, false);
			return static_cast<float>(line_space);
		}

		//複数行になる場合

		const float area_height = draw_area_y_bottom - draw_area_y_top;

		//描画開始
		std::size_t current_string_byte_pos = 0;
		std::size_t line_front_string_byte_pos = 0;
		float current_y_relative = 0.0f;
		auto line_front_it = info.begin();
		for (auto it = info.begin(); it < info.end(); current_string_byte_pos += it->Bytes, ++it) {
			const auto line_width_contain_current_it_point_char = it->DrawX + it->SizeX - line_front_it->DrawX;
			if (area_width < line_width_contain_current_it_point_char) {
				using namespace std::string_literals;
				//次の行に行く前に描画、itが指す文字は含まない
				const std::size_t str_len_byte = current_string_byte_pos - line_front_string_byte_pos;
				//it->DrawXは前の文字の右端に等しい
				const float line_width = it->DrawX - line_front_it->DrawX;
				const float padding = 0.f;//(area_width - line_width) / 2.0f;
				const auto line_string = string.substr(line_front_string_byte_pos / sizeof(TCHAR), (str_len_byte / sizeof(TCHAR)));
				DxLib::DrawStringFToHandle(draw_area_x_left + padding, draw_area_y_top + current_y_relative, line_string.c_str(), color, font_handle, edge_color, false);
				//itが指す文字が先頭になる
				line_front_string_byte_pos = current_string_byte_pos;
				current_y_relative += line_space;
				line_front_it = it;
				if (area_height < current_y_relative) return current_y_relative;//描画可能領域(y)を超えたら終了
			}
		}
		//最終行の描画
		const auto last_line_width = info.back().DrawX + info.back().SizeX - line_front_it->DrawX;
		const float padding = 0.f;// (area_width - last_line_width) / 2.0f;
		const auto line_string = string.substr(line_front_string_byte_pos / sizeof(TCHAR));
		DxLib::DrawStringFToHandle(draw_area_x_left + padding, draw_area_y_top + current_y_relative, line_string.c_str(), color, font_handle, edge_color, false);
		return current_y_relative + line_space;
	}


	class FontHandle {
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
	private:
		int handle_;
		constexpr FontHandle(int h) : handle_(h) {}
		static constexpr int invalid_handle = -1;
	public:
		constexpr FontHandle(void) : handle_(invalid_handle) {}
		FontHandle(const FontHandle&) = delete;
		FontHandle(FontHandle&& o) noexcept : handle_(o.handle_) {
			o.handle_ = invalid_handle;
		}
		FontHandle& operator=(const FontHandle&) = delete;
		FontHandle& operator=(FontHandle&& o) noexcept {
			this->handle_ = o.handle_;
			o.handle_ = invalid_handle;
			return *this;
		}
		~FontHandle(void) noexcept {
			Dispose();
		}
		void Dispose(void) noexcept {
			if (invalid_handle != this->handle_) {
				DeleteFontToHandle(this->handle_);
				this->handle_ = invalid_handle;
			}
		}
		int get(void) const noexcept { return handle_; }
		//長さ取得
		int GetDrawWidth(std::basic_string_view<TCHAR> String) const noexcept {
			return DxLib::GetDrawNStringWidthToHandle(String.data(), String.size(), this->handle_, FALSE);
		}
		template <typename... Args>
		int GetDrawWidthFormat(const std::string& String, Args&&... args) const noexcept {
			return DxLib::GetDrawFormatStringWidthToHandle(this->handle_, String.c_str(), args...);
		}
		int GetDrawExtendWidth(float siz, std::basic_string_view<TCHAR> String) const noexcept {
			return DxLib::GetDrawExtendNStringWidthToHandle(double(siz), String.data(), String.size(), this->handle_, FALSE);
		}
		template <typename... Args>
		int GetDrawExtendWidthFormat(float siz, const std::string& String, Args&&... args) const noexcept {
			return DxLib::GetDrawExtendFormatStringWidthToHandle(double(siz), this->handle_, String.c_str(), args...);
		}
		//文字描画
		template <typename... Args>
		void DrawString(FontXCenter FontX, FontYCenter FontY, int x, int y, unsigned int Color, unsigned int EdgeColor, const std::string& String, Args&&... args) const noexcept {
			switch (FontY) {
			case FontYCenter::TOP:
				break;
			case FontYCenter::MIDDLE:
				y -= GetFontSizeToHandle(this->handle_) / 2;
				break;
			case FontYCenter::BOTTOM:
				y -= GetFontSizeToHandle(this->handle_);
				break;
			default:
				break;
			}
			switch (FontX) {
			case FontXCenter::LEFT:
				DxLib::DrawFormatString2ToHandle(x, y, Color, EdgeColor, this->handle_, String.c_str(), args...);
				break;
			case FontXCenter::MIDDLE:
				DxLib::DrawFormatString2ToHandle(x - GetDrawWidthFormat(String, args...) / 2, y, Color, EdgeColor, this->handle_, String.c_str(), args...);
				break;
			case FontXCenter::RIGHT:
				DxLib::DrawFormatString2ToHandle(x - GetDrawWidthFormat(String, args...), y, Color, EdgeColor, this->handle_, String.c_str(), args...);
				break;
			default:
				break;
			}
		}
		template <typename... Args>
		void DrawExtendString(FontXCenter FontX, FontYCenter FontY, int x, int y, float xsiz, float ysiz, unsigned int Color, unsigned int EdgeColor, const std::string& String, Args&&... args) const noexcept {
			switch (FontY) {
			case FontYCenter::TOP:
				break;
			case FontYCenter::MIDDLE:
				y -= (int)((float)(GetFontSizeToHandle(this->handle_) / 2)*ysiz);
				break;
			case FontYCenter::BOTTOM:
				y -= (int)((float)(GetFontSizeToHandle(this->handle_))*ysiz);
				break;
			default:
				break;
			}
			switch (FontX) {
			case FontXCenter::LEFT:
				DxLib::DrawExtendFormatString2ToHandle(x, y, (double)xsiz, (double)ysiz, Color, EdgeColor, this->handle_, String.c_str(), args...);
				break;
			case FontXCenter::MIDDLE:
				DxLib::DrawExtendFormatString2ToHandle(x - GetDrawExtendWidthFormat(xsiz, String.c_str(), args...) / 2, y, (double)xsiz, (double)ysiz, Color, EdgeColor, this->handle_, String.c_str(), args...);
				break;
			case FontXCenter::RIGHT:
				DxLib::DrawExtendFormatString2ToHandle(x - GetDrawExtendWidthFormat(xsiz, String.c_str(), args...), y, (double)xsiz, (double)ysiz, Color, EdgeColor, this->handle_, String.c_str(), args...);
				break;
			default:
				break;
			}
		}

		//文字描画
		const auto DrawStringAutoFit(int x1, int y1, int x2, int y2, unsigned int Color, unsigned int EdgeColor, const std::string& String) const noexcept {
			return draw_string_center((float)x1, (float)x2, (float)y1, (float)y2, String, Color, this->handle_, EdgeColor);
		}
		//ハンドル作成
		static FontHandle Create(std::basic_string_view<TCHAR> FontName, int Size, int FontType = -1, int CharSet = -1, int EdgeSize = -1, bool Italic = false) {
			return { DxLib::CreateFontToHandleWithStrLen(FontName.data(), FontName.length(), Size, Size / 3, FontType, CharSet, EdgeSize, Italic) };
		}
		static FontHandle Create(int Size, int FontType = -1, int CharSet = -1, int EdgeSize = -1, bool Italic = false) {
			return { DxLib::CreateFontToHandle(nullptr, Size, Size / 3, FontType, CharSet, EdgeSize, Italic) };
		}
	};

	//ローカライズプール
	class LocalizePool : public SingletonBase<LocalizePool> {
	private:
		friend class SingletonBase<LocalizePool>;
	public:
		typedef int LocalizeID;
	private:
		struct LocalizeStr {
			LocalizeID m_ID{0};
			char m_Str[256]{};
		};

		std::vector<LocalizeStr> havehandle;
	private:
		LocalizePool();
		~LocalizePool() {
			Dispose();
		}
	public:
		void Load(const char* Lang);
		void Dispose() {
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

	//フォントプール
	class FontPool : public SingletonBase<FontPool> {
	private:
		friend class SingletonBase<FontPool>;
	public:
		enum class FontType {
			Nomal_Edge,		//MSゴシック、エッジ付き細
			Nomal_EdgeL,	//MSゴシック、エッジ付き太
			Nomal_AA,		//MSゴシック、AA
			Nomal_ItalicAA,	//MSゴシック、イタリックAA
			Gothic_Edge,	//MSゴシック、エッジ付き細
			Gothic_AA,	//MSゴシック、AA
		};
		class Fonthave {
			int				m_scaleType{ DX_DRAWMODE_BILINEAR };

			FontType		m_Type{ 0 };
			int				m_fontsize{ 0 };

			int				m_size{ 0 };

			FontHandle		m_Handle;
		public:
			const auto&		Get_type(void)const noexcept { return this->m_Type; }
			const auto&		Get_fontsize(void)const noexcept { return this->m_fontsize; }
			template <typename... Args>
			void DrawString(int fontSize, FontHandle::FontXCenter FontX, FontHandle::FontYCenter FontY, int x, int y, unsigned int Color, unsigned int EdgeColor, const std::string& String, Args&&... args) const noexcept {
				if (this->m_fontsize != -1) {
					this->m_Handle.DrawString(FontX, FontY, x, y, Color, EdgeColor, String, args...);
				}
				else {
					auto prev = GetDrawMode();
					SetDrawMode(this->m_scaleType);
					this->m_Handle.DrawExtendString(FontX, FontY, x, y, (float)fontSize / (float)this->m_size, (float)fontSize / (float)this->m_size, Color, EdgeColor, String, args...);
					SetDrawMode(prev);
				}
			}

			const auto DrawStringAutoFit(int x1, int y1, int x2, int y2, unsigned int Color, unsigned int EdgeColor, const std::string& String) const noexcept {
				return this->m_Handle.DrawStringAutoFit(x1, y1, x2, y2, Color, EdgeColor, String);
			}

			template <typename... Args>
			const auto GetStringWidth(int fontSize, const std::string& String, Args&&... args) const noexcept {
				if (this->m_fontsize != -1) {
					return this->m_Handle.GetDrawWidthFormat(String, args...);
				}
				else {
					return this->m_Handle.GetDrawExtendWidthFormat((float)fontSize / (float)this->m_size, String, args...);
				}
			}

			void			Set(FontType type, int fontSize = -1) noexcept {
				this->m_fontsize = fontSize;
				this->m_Type = type;
				switch (this->m_Type) {
				case FontType::Nomal_Edge:
					this->m_size = 32;
					this->m_scaleType = DX_DRAWMODE_BILINEAR;
					if (this->m_fontsize != -1) {
						this->m_Handle = FontHandle::Create(LocalizePool::Instance()->Get(0), this->m_fontsize, DX_FONTTYPE_EDGE, -1, 1);
					}
					else {
						this->m_Handle = FontHandle::Create(LocalizePool::Instance()->Get(0), this->m_size, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 1);
					}
					break;
				case FontType::Nomal_EdgeL:
					this->m_size = 32;
					this->m_scaleType = DX_DRAWMODE_BILINEAR;
					if (this->m_fontsize != -1) {
						this->m_Handle = FontHandle::Create(LocalizePool::Instance()->Get(0), this->m_fontsize, DX_FONTTYPE_EDGE, -1, 3);
					}
					else {
						this->m_Handle = FontHandle::Create(LocalizePool::Instance()->Get(0), this->m_size, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 3);
					}
					break;
				case FontType::Nomal_AA:
					this->m_size = 92;
					this->m_scaleType = DX_DRAWMODE_NEAREST;
					if (this->m_fontsize != -1) {
						this->m_Handle = FontHandle::Create(LocalizePool::Instance()->Get(0), this->m_fontsize, DX_FONTTYPE_NORMAL, -1, -1);
					}
					else {
						this->m_Handle = FontHandle::Create(LocalizePool::Instance()->Get(0), this->m_size, DX_FONTTYPE_ANTIALIASING, -1, -1);
					}
					break;
				case FontType::Nomal_ItalicAA:
					this->m_size = 92;
					this->m_scaleType = DX_DRAWMODE_NEAREST;
					this->m_Handle = FontHandle::Create(LocalizePool::Instance()->Get(0), (this->m_fontsize != -1) ? this->m_fontsize : this->m_size, DX_FONTTYPE_NORMAL, -1, -1, true);
					break;
				case FontType::Gothic_Edge:
					this->m_size = 32;
					this->m_scaleType = DX_DRAWMODE_BILINEAR;
					if (this->m_fontsize != -1) {
						this->m_Handle = FontHandle::Create(LocalizePool::Instance()->Get(1), this->m_fontsize, DX_FONTTYPE_EDGE, -1, 1);
					}
					else {
						this->m_Handle = FontHandle::Create(LocalizePool::Instance()->Get(1), this->m_size, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 1);
					}
					break;
				case FontType::Gothic_AA:
					this->m_size = 32;
					this->m_scaleType = DX_DRAWMODE_BILINEAR;
					if (this->m_fontsize != -1) {
						this->m_Handle = FontHandle::Create(LocalizePool::Instance()->Get(1), this->m_fontsize, DX_FONTTYPE_NORMAL, -1, -1);
					}
					else {
						this->m_Handle = FontHandle::Create(LocalizePool::Instance()->Get(1), this->m_size, DX_FONTTYPE_ANTIALIASING, -1, -1);
					}
					break;
				default:
					break;
				}
			}
		};
	private:
		std::vector<Fonthave> havehandle;
		size_t Add(FontType type, int fontSize = -1) noexcept {
			for (auto& h : this->havehandle) {
				if (h.Get_type() == type && h.Get_fontsize() == fontSize) {
					return &h - &this->havehandle.front();
				}
			}
			this->havehandle.resize(this->havehandle.size() + 1);
			this->havehandle.back().Set(type, fontSize);
			return this->havehandle.size() - 1;
		}
	public:
		Fonthave& Get(FontType type, int fontSize = -1) noexcept { return this->havehandle[Add(type, fontSize)]; }
	};
};
