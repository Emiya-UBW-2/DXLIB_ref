#pragma once
#include "DXLib_ref.h"
// リサイズ
constexpr auto EdgeSize = (2);
constexpr auto LineHeight = (18);

namespace DXLibRef {

	// 固定でA8R8G8B8を出す
	constexpr uint32_t GetColorFix(uint8_t Red, uint8_t Green, uint8_t Blue) {
		return 0xff000000 | (((uint32_t)Red) << 16) | (((uint32_t)Green) << 8) | ((uint32_t)Blue);
	}

	// カラー指定
	static constexpr unsigned int Red{ GetColorFix(255, 0, 0) };
	static constexpr unsigned int Red25{ GetColorFix(192, 0, 0) };
	static constexpr unsigned int Red50{ GetColorFix(128, 0, 0) };

	static constexpr unsigned int Green{ GetColorFix(0, 255, 0) };// GetColorFix(43, 255, 91)
	static constexpr unsigned int DarkGreen{ GetColorFix(0, 64, 0) };// GetColorFix(21, 128, 45)

	static constexpr unsigned int Blue{ GetColorFix(0, 0, 255) };

	static constexpr unsigned int Yellow{ GetColorFix(255, 255, 0) };

	static constexpr unsigned int WhiteSel{ GetColorFix(216, 255, 216) };

	static constexpr unsigned int White{ GetColorFix(255, 255, 255) };
	static constexpr unsigned int Gray05{ GetColorFix(255 * 95 / 100, 255 * 95 / 100, 255 * 95 / 100) };
	static constexpr unsigned int Gray10{ GetColorFix(255 * 90 / 100, 255 * 90 / 100, 255 * 90 / 100) };
	static constexpr unsigned int Gray15{ GetColorFix(255 * 85 / 100, 255 * 85 / 100, 255 * 85 / 100) };
	static constexpr unsigned int Gray20{ GetColorFix(255 * 80 / 100, 255 * 80 / 100, 255 * 80 / 100) };
	static constexpr unsigned int Gray25{ GetColorFix(255 * 75 / 100, 255 * 75 / 100, 255 * 75 / 100) };
	static constexpr unsigned int Gray30{ GetColorFix(255 * 70 / 100, 255 * 70 / 100, 255 * 70 / 100) };
	static constexpr unsigned int Gray35{ GetColorFix(255 * 65 / 100, 255 * 65 / 100, 255 * 65 / 100) };
	static constexpr unsigned int Gray40{ GetColorFix(255 * 60 / 100, 255 * 60 / 100, 255 * 60 / 100) };
	static constexpr unsigned int Gray45{ GetColorFix(255 * 55 / 100, 255 * 55 / 100, 255 * 55 / 100) };
	static constexpr unsigned int Gray50{ GetColorFix(255 * 50 / 100, 255 * 50 / 100, 255 * 50 / 100) };
	static constexpr unsigned int Gray55{ GetColorFix(255 * 45 / 100, 255 * 45 / 100, 255 * 45 / 100) };
	static constexpr unsigned int Gray60{ GetColorFix(255 * 40 / 100, 255 * 40 / 100, 255 * 40 / 100) };
	static constexpr unsigned int Gray65{ GetColorFix(255 * 35 / 100, 255 * 35 / 100, 255 * 35 / 100) };
	static constexpr unsigned int Gray70{ GetColorFix(255 * 30 / 100, 255 * 30 / 100, 255 * 30 / 100) };
	static constexpr unsigned int Gray75{ GetColorFix(255 * 25 / 100, 255 * 25 / 100, 255 * 25 / 100) };
	static constexpr unsigned int Gray80{ GetColorFix(255 * 20 / 100, 255 * 20 / 100, 255 * 20 / 100) };
	static constexpr unsigned int Gray85{ GetColorFix(255 * 15 / 100, 255 * 15 / 100, 255 * 15 / 100) };
	static constexpr unsigned int Gray90{ GetColorFix(255 * 10 / 100, 255 * 10 / 100, 255 * 10 / 100) };
	static constexpr unsigned int Gray95{ GetColorFix(255 * 5 / 100, 255 * 5 / 100, 255 * 5 / 100) };
	static constexpr unsigned int Black{ GetColorFix(0, 0, 0) };
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// 描画
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// グラデーションのある矩形を描画
	static void DrawGradationBox_2D(int x1, int y1, int x2, int y2, COLOR_U8 color1, COLOR_U8 color2, const unsigned char UorL = 255) noexcept {
		VERTEX2D Vertex[6]{};
		// 左上の頂点の情報をセット
		Vertex[0].pos.x = static_cast<float>(x1);
		Vertex[0].pos.y = static_cast<float>(y1);
		Vertex[0].pos.z = 0.0f;
		Vertex[0].rhw = 1.0f;
		Vertex[0].dif = color1;
		Vertex[0].u = 0.0f;
		Vertex[0].v = 0.0f;

		// 右上の頂点の情報をセット
		Vertex[1].pos.x = static_cast<float>(x2);
		Vertex[1].pos.y = static_cast<float>(y1);
		Vertex[1].pos.z = 0.0f;
		Vertex[1].rhw = 1.0f;
		Vertex[1].dif = color2;
		Vertex[1].u = 0.0f;
		Vertex[1].v = 0.0f;

		Vertex[5] = Vertex[1];

		// 左下の頂点の情報をセット
		Vertex[2].pos.x = static_cast<float>(x1);
		Vertex[2].pos.y = static_cast<float>(y2);
		Vertex[2].pos.z = 0.0f;
		Vertex[2].rhw = 1.0f;
		Vertex[2].dif = color1;
		Vertex[2].u = 0.0f;
		Vertex[2].v = 0.0f;

		Vertex[4] = Vertex[2];

		// 右下の頂点の情報をセット
		Vertex[3].pos.x = static_cast<float>(x2);
		Vertex[3].pos.y = static_cast<float>(y2);
		Vertex[3].pos.z = 0.0f;
		Vertex[3].rhw = 1.0f;
		Vertex[3].u = 0.0f;
		Vertex[3].v = 0.0f;

		switch (UorL) {
		case 3:// 横(右)
			// 左上の頂点の情報をセット
			Vertex[0].dif = color1;
			// 右上の頂点の情報をセット
			Vertex[1].dif = color2;
			Vertex[5] = Vertex[1];
			// 左下の頂点の情報をセット
			Vertex[2].dif = color1;
			Vertex[4] = Vertex[2];
			// 右下の頂点の情報をセット
			Vertex[3].dif = color2;
			break;
		case 2:// 縦(下)
			// 左上の頂点の情報をセット
			Vertex[0].dif = color1;
			// 右上の頂点の情報をセット
			Vertex[1].dif = color1;
			Vertex[5] = Vertex[1];
			// 左下の頂点の情報をセット
			Vertex[2].dif = color2;
			Vertex[4] = Vertex[2];
			// 右下の頂点の情報をセット
			Vertex[3].dif = color2;
			break;
		case 1:// 横(左)
			// 左上の頂点の情報をセット
			Vertex[0].dif = color2;
			// 右上の頂点の情報をセット
			Vertex[1].dif = color1;
			Vertex[5] = Vertex[1];
			// 左下の頂点の情報をセット
			Vertex[2].dif = color2;
			Vertex[4] = Vertex[2];
			// 右下の頂点の情報をセット
			Vertex[3].dif = color1;
			break;
		case 0:// 縦(上)
			// 左上の頂点の情報をセット
			Vertex[0].dif = color2;
			// 右上の頂点の情報をセット
			Vertex[1].dif = color2;
			Vertex[5] = Vertex[1];
			// 左下の頂点の情報をセット
			Vertex[2].dif = color1;
			Vertex[4] = Vertex[2];
			// 右下の頂点の情報をセット
			Vertex[3].dif = color1;
			break;
		default:
			// 左上の頂点の情報をセット
			Vertex[0].dif = color1;
			// 右上の頂点の情報をセット
			Vertex[1].dif = color1;
			Vertex[5] = Vertex[1];
			// 左下の頂点の情報をセット
			Vertex[2].dif = color1;
			Vertex[4] = Vertex[2];
			// 右下の頂点の情報をセット
			Vertex[3].dif = color1;
			break;
		}
		// ポリゴンを2個描画
		DrawPolygon2D(Vertex, 2, DX_NONE_GRAPH, FALSE);
	}
	// 	x1,y1 Angleが0の場合の左上座標
	// 	x2,y2 Angleが0の場合の右下座標
	// 	xminp,yminp 左上角からの固定長さ
	// 	xmaxp,ymaxp 右下角からの固定長さ
	// 	XCenter,YCenter　　: 画像を回転描画する画像上の中心座標(左上を(0.0f,0.0f)、右下を(1.0f,1.0f)とした割合)
	// 	Angle　　　: 描画角度（ラジアン指定）
	// 	GrHandle　 : 描画するグラフィックの識別番号（グラフィックハンドル）
	// 	TransFlag　: 画像の透明度を有効にするかどうか( TRUE：有効にする　FALSE：無効にする )
	// 	TilingFlag : 角以外の部分をタイリングするか拡縮させるか( TRUE：タイリング　FALSE：拡縮 )
	static void Draw9SliceGraph(
		int x1, int y1, int x2, int y2,
		int xminp, int yminp, int xmaxp, int ymaxp,
		float XCenter, float YCenter, float Angle,
		int GrHandle, bool TransFlag, bool TilingFlag) noexcept {
		// 最低限のサイズを指定
		if (x2 < x1 + xminp + xmaxp) {
			x2 = x1 + xminp + xmaxp;
		}
		if (y2 < y1 + yminp + ymaxp) {
			y2 = y1 + yminp + ymaxp;
		}
		// 用意する頂点情報
		std::vector<VERTEX2D> Vertex;
		std::vector<unsigned short> Index;

		float xs = static_cast<float>(x2 - x1);
		float ys = static_cast<float>(y2 - y1);

		float CenterX = static_cast<float>(x1) + xs * XCenter;
		float CenterY = static_cast<float>(y1) + ys * YCenter;

		auto SetUpPoint = [&](float xper, float yper, int xc, int yc) {
			Vertex.emplace_back();
			Vertex.back().pos = VGet(
				static_cast<float>(x1) + xs * xper - CenterX,
				static_cast<float>(y1) + ys * yper - CenterY,
				0.f);

			Vertex.back().pos = VGet(
				CenterX + Vertex.back().pos.x * std::cos(Angle) - Vertex.back().pos.y * std::sin(Angle),
				CenterY + Vertex.back().pos.x * std::sin(Angle) + Vertex.back().pos.y * std::cos(Angle),
				0.f);

			Vertex.back().rhw = 1.0f;
			Vertex.back().dif = GetColorU8(255, 255, 255, 255);
			Vertex.back().u = static_cast<float>(xc) / 3.f;
			Vertex.back().v = static_cast<float>(yc) / 3.f;
			return (unsigned short)(Vertex.size() - 1);
			};
		auto SetUpBox = [&](float xmin, float ymin, float xmax, float ymax, int xc, int yc) {
			Index.emplace_back(SetUpPoint(xmin, ymin, xc, yc));// 左上の頂点の情報をセット
			auto RU = SetUpPoint(xmax, ymin, xc + 1, yc);
			auto LD = SetUpPoint(xmin, ymax, xc, yc + 1);
			Index.emplace_back(RU);// 右上の頂点の情報をセット
			Index.emplace_back(LD);// 左下の頂点の情報をセット
			Index.emplace_back(SetUpPoint(xmax, ymax, xc + 1, yc + 1));// 右下の頂点の情報をセット
			Index.emplace_back(LD);// 左下の頂点の情報をセット
			Index.emplace_back(RU);// 右上の頂点の情報をセット
			};

		float xminpt = static_cast<float>(xminp) / xs;
		float xmaxpt = static_cast<float>(xmaxp) / xs;
		float xmaxt = 1.f - xmaxpt;
		float xmidt = xmaxt - xminpt;

		float yminpt = static_cast<float>(yminp) / ys;
		float ymaxpt = static_cast<float>(ymaxp) / ys;
		float ymaxt = 1.f - ymaxpt;
		float ymidt = ymaxt - yminpt;

		int xtile = 1;
		int ytile = 1;
		// タイリング
		if (TilingFlag) {
			xtile = static_cast<int>(xmidt / ((xminpt + xmaxpt) / 2.f)) + 1;
			if (xtile <= 0) {
				xtile = 1;
			}
			ytile = static_cast<int>(ymidt / ((yminpt + ymaxpt) / 2.f)) + 1;
			if (ytile <= 0) {
				ytile = 1;
			}
		}

		Vertex.reserve(static_cast<size_t>(3 * 2 * ((xtile + 2) * (ytile + 2))));
		float xmin = 0.f;
		float xmax = xminpt;
		int xc = 0;
		for (int x : std::views::iota(0, xtile + 2)) {
			float ymin = 0.f;
			float ymax = yminpt;
			int yc = 0;
			for (int y : std::views::iota(0, ytile + 2)) {
				SetUpBox(xmin, ymin, xmax, ymax, xc, yc);
				// 次
				ymin = ymax;
				ymax = TilingFlag ? (ymin + ymidt / static_cast<float>(ytile)) : ymaxt;
				if (y == 0) {
					yc = 1;
				}
				if (y == ytile) {
					ymax = 1.f;
					yc = 2;
				}
			}
			// 次
			xmin = xmax;
			xmax = TilingFlag ? (xmin + xmidt / static_cast<float>(xtile)) : xmaxt;
			if (x == 0) {
				xc = 1;
			}
			if (x == xtile) {
				xmax = 1.f;
				xc = 2;
			}
		}
		DrawPolygonIndexed2D(Vertex.data(), static_cast<int>(Vertex.size()), Index.data(), static_cast<int>(Index.size()) / 3, GrHandle, TransFlag ? TRUE : FALSE);
	}
	// カプセル描画
	static bool DrawCapsule_3D(const Vector3DX& p1, const Vector3DX& p2, float range, const unsigned int& color, const unsigned int& speccolor) noexcept {
		return DxLib::DrawCapsule3D(p1.get(), p2.get(), range, 8, color, speccolor, TRUE) == TRUE;
	}
	// 球描画
	static bool DrawSphere_3D(const Vector3DX& p1, float range, const unsigned int& color, const unsigned int& speccolor) noexcept {
		return DxLib::DrawSphere3D(p1.get(), range, 8, color, speccolor, TRUE) == TRUE;
	}
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// 描画
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	namespace WindowSystem {
		// 
		enum class DrawType : int {
			Alpha,
			Add,
			Bright,
			Box,
			Quadrangle,
			Circle,
			Line,
			String,
			StringAutoFit,
			RotaGraph,
			ExtendGraph,
			CircleGauge,
			NineSliceGraph,
		};
		class DrawData {
			DrawType								m_type{ DrawType::Box };
			std::array<int, 8>						m_intParam{};
			std::array<unsigned int, 2>				m_UintParam{};
			std::array<float, 6>					m_floatParam{};
			std::array<bool, 2>						m_boolParam{};
			std::array<const GraphHandle*, 1>		m_GraphHandleParam{};
			std::string								m_string;
		public:
			void InputType(DrawType type) noexcept { this->m_type = type; }
			void InputintParam(int ID, int param) noexcept { this->m_intParam.at(ID) = param; }
			void InputUintParam(int ID, unsigned int param) noexcept { this->m_UintParam.at(ID) = param; }
			void InputfloatParam(int ID, float param) noexcept { this->m_floatParam.at(ID) = param; }
			void InputboolParam(int ID, bool param) noexcept { this->m_boolParam.at(ID) = param; }
			void InputGraphHandleParam(int ID, const GraphHandle* param) noexcept { this->m_GraphHandleParam.at(ID) = param; }
			void InputStringParam(std::string_view param) noexcept { this->m_string = param; }
		public:
			void Output(void) const noexcept;
		public:
			void		operator=(const DrawData& tgt) noexcept {
				this->m_type = tgt.m_type;
				this->m_intParam = tgt.m_intParam;
				this->m_UintParam = tgt.m_UintParam;
				this->m_floatParam = tgt.m_floatParam;
				this->m_boolParam = tgt.m_boolParam;
				this->m_GraphHandleParam = tgt.m_GraphHandleParam;
				this->m_string = tgt.m_string;
			}
			bool		operator==(const DrawData& tgt) const noexcept {
				return (
					this->m_type == tgt.m_type &&
					this->m_intParam == tgt.m_intParam &&
					this->m_UintParam == tgt.m_UintParam &&
					this->m_floatParam == tgt.m_floatParam &&
					this->m_boolParam == tgt.m_boolParam &&
					this->m_GraphHandleParam == tgt.m_GraphHandleParam &&
					this->m_string == tgt.m_string
					);
			}
		};
		// 
		enum class DrawLayer : int {
			BackGround,
			Back,
			Normal,
			Front,

			Max,
		};
		// 
		class DrawControl : public SingletonBase<DrawControl> {
		private:
			friend class SingletonBase<DrawControl>;
		private:
			std::vector<std::vector<DrawData>>	m_DrawDatas;
			std::vector<std::vector<DrawData>>	m_PrevDrawDatas;

			GraphHandle				m_BufferScreen;
		private:
			DrawControl(void) noexcept;
		private:
			DrawData* GetBack(DrawLayer Layer) noexcept {
				this->m_DrawDatas.at(static_cast<int>(Layer)).emplace_back();
				return &this->m_DrawDatas.at(static_cast<int>(Layer)).back();
			}
		public:
			bool	IsDrawOnWindow(int x1, int y1, int x2, int y2) noexcept;
			// 
			void	SetAlpha(DrawLayer Layer, int Alpha) noexcept {
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::Alpha);
				Back->InputintParam(0, Alpha);
			}
			// 
			void	SetAdd(DrawLayer Layer, int Add) noexcept {
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::Add);
				Back->InputintParam(0, Add);
			}
			// 
			void	SetBright(DrawLayer Layer, int valueR, int valueG, int valueB) noexcept {
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::Bright);
				Back->InputintParam(0, valueR);
				Back->InputintParam(1, valueG);
				Back->InputintParam(2, valueB);
			}
			// 
			void	SetDrawBox(DrawLayer Layer, int x1, int y1, int x2, int y2, unsigned int color1, bool IsFill, int thickness = 1) noexcept {
				if (!IsDrawOnWindow(x1, y1, x2, y2)) { return; }				// 画面外は表示しない
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::Box);
				Back->InputintParam(0, x1);
				Back->InputintParam(1, y1);
				Back->InputintParam(2, x2);
				Back->InputintParam(3, y2);
				Back->InputUintParam(0, color1);
				Back->InputboolParam(0, IsFill);
				Back->InputintParam(4, thickness);
			}
			// 
			void	SetDrawQuadrangle(DrawLayer Layer, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int color1, bool IsFill) noexcept {
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::Quadrangle);
				Back->InputintParam(0, x1);
				Back->InputintParam(1, y1);
				Back->InputintParam(2, x2);
				Back->InputintParam(3, y2);
				Back->InputintParam(4, x3);
				Back->InputintParam(5, y3);
				Back->InputintParam(6, x4);
				Back->InputintParam(7, y4);
				Back->InputUintParam(0, color1);
				Back->InputboolParam(0, IsFill);
			}
			// 
			void	SetDrawCircle(DrawLayer Layer, int x1, int y1, int radius, unsigned int color1, bool IsFill = true, int LineThickness = 1) {
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::Circle);
				Back->InputintParam(0, x1);
				Back->InputintParam(1, y1);
				Back->InputintParam(2, radius);
				Back->InputUintParam(0, color1);
				Back->InputboolParam(0, IsFill);
				Back->InputintParam(3, LineThickness);
			}
			// 
			void	SetDrawLine(DrawLayer Layer, int x1, int y1, int x2, int y2, unsigned int color1, int   Thickness = 1) noexcept {
				if (!IsDrawOnWindow(x1, y1, x2, y2)) { return; }				// 画面外は表示しない
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::Line);
				Back->InputintParam(0, x1);
				Back->InputintParam(1, y1);
				Back->InputintParam(2, x2);
				Back->InputintParam(3, y2);
				Back->InputUintParam(0, color1);
				Back->InputintParam(4, Thickness);
			}
			// 
			void	SetDrawRotaGraph(DrawLayer Layer, const GraphHandle* pGraphHandle, int posx, int posy, float Exrate, float rad, bool trns) noexcept {
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::RotaGraph);
				Back->InputGraphHandleParam(0, pGraphHandle);
				Back->InputintParam(0, posx);
				Back->InputintParam(1, posy);
				Back->InputfloatParam(0, Exrate);
				Back->InputfloatParam(1, rad);
				Back->InputboolParam(0, trns);
			}
			void	SetDrawExtendGraph(DrawLayer Layer, const GraphHandle* pGraphHandle, int x1, int y1, int x2, int y2, bool trns) noexcept {
				if (!IsDrawOnWindow(x1, y1, x2, y2)) { return; }				// 画面外は表示しない
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::ExtendGraph);
				Back->InputGraphHandleParam(0, pGraphHandle);
				Back->InputintParam(0, x1);
				Back->InputintParam(1, y1);
				Back->InputintParam(2, x2);
				Back->InputintParam(3, y2);
				Back->InputboolParam(0, trns);
			}
			// 
			template <typename... Args>
			void	SetString(DrawLayer Layer, FontSystem::FontType type, int fontSize, FontSystem::FontXCenter FontX, FontSystem::FontYCenter FontY, int x, int y, unsigned int Color, unsigned int EdgeColor, const std::string& Str, Args&&... args) noexcept {
				if (Str == "") { return; }
				int xSize = FontSystem::FontPool::Instance()->Get(FontSystem::FontType::MS_Gothic, fontSize, 3)->GetStringWidth(Str.c_str(), args...);
				switch (FontX) {
				case FontSystem::FontXCenter::LEFT:
					if (!IsDrawOnWindow((x), (y - fontSize), (x + xSize), (y + fontSize))) { return; }				// 画面外は表示しない
					break;
				case FontSystem::FontXCenter::MIDDLE:
					if (!IsDrawOnWindow((x - xSize / 2), (y - fontSize), (x + xSize / 2), (y + fontSize))) { return; }				// 画面外は表示しない
					break;
				case FontSystem::FontXCenter::RIGHT:
					if (!IsDrawOnWindow((x - xSize), (y - fontSize), (x), (y + fontSize))) { return; }				// 画面外は表示しない
					break;
				default:
					break;
				}

				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::String);

				Back->InputintParam(0, (int)type);
				Back->InputintParam(1, fontSize);
				Back->InputintParam(2, (int)FontX);
				Back->InputintParam(3, (int)FontY);
				Back->InputintParam(4, x);
				Back->InputintParam(5, y);
				Back->InputUintParam(0, Color);
				Back->InputUintParam(1, EdgeColor);

				char ptr[1024] = "";
				snprintfDx(ptr, 1024, Str.c_str(), args...);
				Back->InputStringParam(ptr);
			}
			// 
			const auto	SetStringAutoFit(DrawLayer Layer, FontSystem::FontType type, int fontSize, int x1, int y1, int x2, int y2, unsigned int Color, unsigned int EdgeColor, const std::string& Str) noexcept {
				if (Str == "") { return 0.f; }
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::StringAutoFit);
				Back->InputintParam(0, (int)type);
				Back->InputintParam(1, fontSize);
				Back->InputintParam(2, x1);
				Back->InputintParam(3, y1);
				Back->InputintParam(4, x2);
				Back->InputintParam(5, y2);
				Back->InputUintParam(0, Color);
				Back->InputUintParam(1, EdgeColor);
				Back->InputStringParam(Str);
				return FontSystem::FontPool::Instance()->Get((FontSystem::FontType)type, fontSize, 3)->DrawStringAutoFit(
					x1 + BaseScreenWidth, y1 + BaseScreenHeight,
					x2 + BaseScreenWidth, y2 + BaseScreenHeight,
					Color,
					EdgeColor,
					Str
				);
			}
			// 
			void SetDrawCircleGauge(DrawLayer Layer, const GraphHandle* pGraphHandle, int   CenterX, int   CenterY, float Percent, float StartPercent = 0.f, float Scale = 1.0f) noexcept {
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::CircleGauge);
				Back->InputGraphHandleParam(0, pGraphHandle);
				Back->InputintParam(0, CenterX);
				Back->InputintParam(1, CenterY);
				Back->InputfloatParam(0, Percent);
				Back->InputfloatParam(1, StartPercent);
				Back->InputfloatParam(2, Scale);
			}

			void NineSliceGraph(DrawLayer Layer, const GraphHandle* pGraphHandle,
				int x1, int y1, int x2, int y2,
				int xminp, int yminp, int xmaxp, int ymaxp,
				float XCenter, float YCenter, float Angle,
				bool TransFlag, bool TilingFlag) noexcept {
				if (!IsDrawOnWindow(x1, y1, x2, y2)) { return; }				// 画面外は表示しない
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::NineSliceGraph);
				Back->InputGraphHandleParam(0, pGraphHandle);
				Back->InputintParam(0, x1);
				Back->InputintParam(1, y1);
				Back->InputintParam(2, x2);
				Back->InputintParam(3, y2);
				Back->InputintParam(4, xminp);
				Back->InputintParam(5, yminp);
				Back->InputintParam(6, xmaxp);
				Back->InputintParam(7, ymaxp);
				Back->InputfloatParam(0, XCenter);
				Back->InputfloatParam(1, YCenter);
				Back->InputfloatParam(2, Angle);
				Back->InputboolParam(0, TransFlag);
				Back->InputboolParam(1, TilingFlag);
			}
			// 
		public:
			void	ClearList(void) noexcept;
			void	Draw(void) noexcept;
			void	Dispose(void) noexcept;
		};
		// クリックできる文字付のボックス
		template <typename... Args>
		extern bool SetMsgClickBox(int xp1, int yp1, int xp2, int yp2, int StringYSizeMax, unsigned int defaultcolor, bool IsRepeat, bool IsActive, std::string_view String, Args&&... args) noexcept {
			auto* DrawCtrls = DrawControl::Instance();
			auto* Pad = PadControl::Instance();
			bool MouseOver = IsActive && IntoMouse(xp1, yp1, xp2, yp2);
			DrawCtrls->SetDrawBox(DrawLayer::Normal, xp1, yp1, xp2, yp2, MouseOver ? (Pad->GetMouseClick().press() ? Gray25 : White) : defaultcolor, true);
			DrawCtrls->SetString(DrawLayer::Normal, FontSystem::FontType::MS_Gothic, std::min(StringYSizeMax, yp2 - yp1),
				FontSystem::FontXCenter::MIDDLE, FontSystem::FontYCenter::MIDDLE, (xp1 + xp2) / 2, (yp1 + yp2) / 2, White, Black,
				((std::string)String).c_str(), args...);
			return (MouseOver && (IsRepeat ? Pad->GetMouseClick().repeat() : Pad->GetMouseClick().trigger()));
		}
		// 
		/*
		class ScrollBoxClass {
			bool		m_IsChangeScrollY{ false }
			int			m_BaseScrollY{ 0 }
			float		m_NowScrollYPer{ 0.f }
		public:
			const auto&		GetNowScrollYPer(void) const noexcept { return this->m_NowScrollYPer; }
			void			ScrollBox(int xp1, int yp1, int xp2, int yp2, float TotalPer, bool IsActive) noexcept {
				auto* DrawCtrls = DrawControl::Instance();
				auto* WindowSizeParts = WindowSizeControl::Instance();
				auto* Pad = PadControl::Instance();
				unsigned int color = Gray25;

				int length = static_cast<int>(static_cast<float>(yp2 - yp1) / TotalPer);
				float Total = static_cast<float>(yp2 - yp1 - length);
				int Yp_t = static_cast<int>(Total * this->m_NowScrollYPer);
				int Yp_s = std::max(yp1, yp1 + Yp_t);
				int Yp_e = std::min(yp2, Yp_s + length);

				if (IsActive)
		{
					if (IntoMouse(xp1, yp1, xp2, yp2))
		{
						if (Pad->GetWheelAdd() != 0.f)
		{
							m_NowScrollYPer = std::clamp(m_NowScrollYPer + static_cast<float>(-Pad->GetWheelAdd() * 3) / Total, 0.f, 1.f);
						}
					}
					if (IntoMouse(xp2 - (24), yp1, xp2, yp2))
		{
						if (Pad->GetINTERACTKey().trigger())
		{
							m_IsChangeScrollY = true;
						}

						if (!m_IsChangeScrollY)
		{
							HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
							SetCursor(hCursor);
						}
					}
					if (m_IsChangeScrollY)
		{
						if (Pad->GetINTERACTKey().press())
		{
							color = White;
							m_NowScrollYPer = std::clamp(static_cast<float>(Pad->GetMouseY() - this->m_BaseScrollY) / Total, 0.f, 1.f);

							HCURSOR hCursor = LoadCursor(NULL, IDC_SIZENS);
							SetCursor(hCursor);
						}
						else {
							m_IsChangeScrollY = false;
						}
					}
					else {
						m_BaseScrollY = Pad->GetMouseY() - Yp_t;
						if (Pad->GetMouseY() < Yp_s)
		{
							m_BaseScrollY += Yp_s - Pad->GetMouseY();
						}

						if (Pad->GetMouseY() > Yp_e)
		{
							m_BaseScrollY += Yp_e - Pad->GetMouseY();
						}
					}
				}
				DrawCtrls->SetDrawBox(DrawLayer::Normal, xp2 - (24), yp1, xp2, yp2, Gray50, true);
				DrawCtrls->SetDrawBox(DrawLayer::Normal, xp2 - (24) + (1), Yp_s, xp2 - (1), Yp_e, color, true);
			}
		}
		//*/
	}
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// サイドログ
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class SideLog : public SingletonBase<SideLog> {
	private:
		friend class SingletonBase<SideLog>;
	private:
		class SideLogData {
			unsigned int m_Color{ 0 };
			char m_Message[64]{};
			float m_TimeStart{ -1.f };
			float m_TimeMax{ -1.f };
			float m_Time{ -1.f };
			float m_Flip{ 0.f };
			float m_Flip_Y{ 0.f };
		public:
			void AddFlip(float value) noexcept { m_Flip += value; }
			template <typename... Args>
			void SetData(float second, float startSec, unsigned int Color, const char* Mes, Args&&... args) noexcept {
				snprintfDx(m_Message, 64, Mes, args...);
				m_TimeStart = startSec;
				m_TimeMax = second;
				m_Time = m_TimeStart + m_TimeMax;
				m_Flip = 0.f;
				m_Flip_Y = -1.f;
				m_Color = Color;
			}
			void UpdateActive(void) noexcept;
		public:
			float GetFlip(void) const noexcept { return m_Flip_Y; }
			float ActivePer(void) const noexcept {
				if (m_Time > m_TimeMax) {
					return 0.f;
				}
				else if (m_Time > 1.f) {
					return std::clamp((m_TimeMax - m_Time) * 10.f + 0.1f, 0.f, 1.f);
				}
				else {
					return std::clamp(m_Time, 0.f, 1.f);
				}
			}
			const char* GetMsg(void) const noexcept { return m_Message; }
			unsigned int GetMsgColor(void) const noexcept { return m_Color; }
		};
	private:
		std::array<SideLogData, 16> data;
		size_t		m_LastSel{ 0 };

		GraphHandle					m_SelectBackImage;
		float SelYadd{ 0.f };
	private:
		SideLog(void) noexcept {
			this->m_SelectBackImage.Load("CommonData/UI/select.png");
		}
		SideLog(const SideLog&) = delete;
		SideLog(SideLog&& o) = delete;
		SideLog& operator=(const SideLog&) = delete;
		SideLog& operator=(SideLog&& o) = delete;
	public:
		template <typename... Args>
		void Add(float second, float startSec, unsigned int Color, const char* Mes, Args&&... args) noexcept {
			for (auto& d : data) {
				d.AddFlip(1.f);
			}
			data.at(m_LastSel).SetData(second, startSec, Color, Mes, args...);
			++m_LastSel %= data.size();
			SelYadd = 10.f;
		}
		void Update(void) noexcept {
			for (auto& d : data) {
				d.UpdateActive();
			}
			Easing(&SelYadd, 0.f, 0.9f, EasingType::OutExpo);
		}
		void Draw(void) noexcept;
	};

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// ポップアップ
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class PopUp : public SingletonBase<PopUp> {
	private:
		friend class SingletonBase<PopUp>;
	private:
		class PopUpDrawClass {
		private:
			bool m_Active{ false };
			bool m_ActiveSwitch{ false };
			float m_ActivePer{ 0.f };
			char m_WindwoName[64]{};

			int WinSizeX{ 720 };
			int WinSizeY{ 720 };

			std::function<void(int xmin, int ymin, int xmax, int ymax, bool EndSwitch)> m_Doing{ nullptr };
			std::function<void()> m_ExitDoing{ nullptr };
			std::function<void()> m_GuideDoing{ nullptr };
		public:
			PopUpDrawClass(void) noexcept {}
			PopUpDrawClass(const PopUpDrawClass&) = delete;
			PopUpDrawClass(PopUpDrawClass&& o) = delete;
			PopUpDrawClass& operator=(const PopUpDrawClass&) = delete;
			PopUpDrawClass& operator=(PopUpDrawClass&& o) = delete;

			~PopUpDrawClass(void) noexcept {}
		public:
			void			Set(const char* WindowName, int sizex, int sizey,
				std::function<void(int xmin, int ymin, int xmax, int ymax, bool EndSwitch)> doing,
				std::function<void()> ExitDoing,
				std::function<void()> GuideDoing
			) noexcept {
				strcpy_sDx(m_WindwoName, 64, WindowName);
				WinSizeX = sizex;
				WinSizeY = sizey;
				m_Doing = doing;
				m_ExitDoing = ExitDoing;
				m_GuideDoing = GuideDoing;
			}
			void			Start(void) noexcept;
			void			End(void) noexcept;
			void			Update(void) noexcept;
			void			Draw(int xcenter, int ycenter) noexcept;
		public:
			auto IsEnd(void) const noexcept { return !m_Active && !(m_ActivePer > 1.f / 255.f); }
		};
	private:
		std::array<PopUpDrawClass, 24> que;
		size_t NowSel{ 0 };
		size_t LastSel{ 0 };
		bool PrevPause{ false };
	private:
		PopUp(void) noexcept {}
		PopUp(const PopUp&) = delete;
		PopUp(PopUp&& o) = delete;
		PopUp& operator=(const PopUp&) = delete;
		PopUp& operator=(PopUp&& o) = delete;
	public:
		auto IsActivePop(void) const noexcept { return (NowSel != LastSel); }
	public:
		void Add(const char* WindowName, int sizex, int sizey,
			std::function<void(int xmin, int ymin, int xmax, int ymax, bool EndSwitch)> doing,
			std::function<void()> ExitDoing,
			std::function<void()> GuideDoing,
			bool IsInsert = false) noexcept;
		void EndAll(void) noexcept;
		void Update(void) noexcept;
		void Draw(int xcenter, int ycenter) noexcept {
			if (!IsActivePop()) {
				return;
			}
			que.at(static_cast<size_t>(NowSel)).Draw(xcenter, ycenter);
		}
	};

	// --------------------------------------------------------------------------------------------------
	// キーガイド
	// --------------------------------------------------------------------------------------------------
	class KeyGuide : public SingletonBase<KeyGuide> {
	private:
		friend class SingletonBase<KeyGuide>;
	private:
		// コンストラクタ
		KeyGuide(void) noexcept;
		// コピーしてはいけないので通常のコンストラクタ以外をすべてdelete
		KeyGuide(const KeyGuide&) = delete;
		KeyGuide(KeyGuide&& o) = delete;
		KeyGuide& operator=(const KeyGuide&) = delete;
		KeyGuide& operator=(KeyGuide&& o) = delete;
		// デストラクタはシングルトンなので呼ばれません
	private:
		class KeyGuideGraph {
			int xsize{ 0 }, ysize{ 0 };
			GraphHandle GuideImg;
		public:
			KeyGuideGraph(void) noexcept {}
			KeyGuideGraph(const KeyGuideGraph&) = delete;
			KeyGuideGraph(KeyGuideGraph&& o) = delete;
			KeyGuideGraph& operator=(const KeyGuideGraph&) = delete;
			KeyGuideGraph& operator=(KeyGuideGraph&& o) = delete;

			~KeyGuideGraph(void) noexcept {}
		public:
			void AddGuide(int x, int y, int xs, int ys, const GraphHandle& baseImage) noexcept {
				GuideImg.DerivationGraph(x, y, xs, ys, baseImage);
				xsize = xs;
				ysize = ys;
			}
			void Dispose(void) noexcept { GuideImg.Dispose(); }
			int GetDrawSize(void) const noexcept;
			void Draw(int x, int y) const noexcept;
		};
		class KeyGuideOnce {
			std::shared_ptr<KeyGuideGraph> m_GuideGraph;
			std::string GuideString;
		public:
			KeyGuideOnce(void) noexcept {}
			KeyGuideOnce(const KeyGuideOnce&) = delete;
			KeyGuideOnce(KeyGuideOnce&& o) = delete;
			KeyGuideOnce& operator=(const KeyGuideOnce&) = delete;
			KeyGuideOnce& operator=(KeyGuideOnce&& o) = delete;

			~KeyGuideOnce(void) noexcept {}
		public:
			void AddGuide(const std::shared_ptr<KeyGuideGraph>& pGuide, const std::string& GuideStr) noexcept {
				m_GuideGraph = pGuide;
				GuideString = GuideStr;
			}
			void Dispose(void) noexcept {
				if (m_GuideGraph) {
					m_GuideGraph.reset();
				}
				GuideString = "";
			}
			int GetDrawSize(void) const noexcept;
			void Draw(int x, int y) const noexcept;
		};
	private:
		bool													m_IsFlipGuide{ true };				// ガイドの更新フラグ
		GraphHandle												m_GuideBaseImage;					// 分割前の画像
		std::vector<std::shared_ptr<KeyGuideGraph>>				m_DerivationGuideImage;				// 分割後の画像
		std::vector<std::unique_ptr<KeyGuideOnce>>				m_Key;								// ガイド
	public:
		static const int GetIDtoOffset(int ID, ControlType controlType) noexcept {
			switch (controlType) {
			case ControlType::XBox:
				for (size_t i = 0; i < static_cast<size_t>(XBoxNum); ++i) {
					if (XBoxID[i] == ID) {
						return static_cast<int>(i + KeyNum + XBoxNum);
					}
				}
				break;
			case ControlType::PS4:
				for (size_t i = 0; i < static_cast<size_t>(DS4Num); ++i) {
					if (DS4ID[i] == ID) {
						return static_cast<int>(i + KeyNum);
					}
				}
				break;
			case ControlType::PC:
				for (size_t i = 0; i < static_cast<size_t>(KeyNum); ++i) {
					if (KeyID[i] == ID) {
						return static_cast<int>(i);
					}
				}
				break;
			default:
				break;
			}
			return InvalidID;
		}

		static const int GetPADStoOffset(PADS PAD) noexcept {
			auto* Pad = PadControl::Instance();
			return GetIDtoOffset(Pad->GetPadsInfo(PAD).GetAssign(), Pad->GetControlType());
		}
	public:
		void SetGuideFlip(void) noexcept { m_IsFlipGuide = true; }
		void ChangeGuide(std::function<void()>Guide_Pad) noexcept;
		void AddGuide(int graphOffset, const std::string& GuideStr) noexcept {
			m_Key.emplace_back(std::make_unique<KeyGuideOnce>());
			m_Key.back()->AddGuide((graphOffset != InvalidID) ? m_DerivationGuideImage.at(graphOffset) : nullptr, GuideStr);
		}
		void Dispose(void) noexcept {
			for (auto& k : m_Key) {
				k->Dispose();
				k.reset();
			}
			m_Key.clear();
		}
	public:
		//ガイド表示の描画
		void Draw(void) const noexcept;
		//キー単体の描画
		void DrawButton(int x, int y, int graphOffset) const noexcept { m_DerivationGuideImage.at(graphOffset)->Draw(x, y); }
		int GetDrawSize(int graphOffset) const noexcept { return m_DerivationGuideImage.at(graphOffset)->GetDrawSize(); }
	};
}
