#pragma once
#include "DXLib_ref.h"
// ���T�C�Y
#define EdgeSize	WindowSizeControl::Instance()->GetUIY(2)
#define LineHeight	WindowSizeControl::Instance()->GetUIY(18)

namespace DXLibRef {

	// �Œ��A8R8G8B8���o��
	constexpr uint32_t GetColorFix(uint8_t Red, uint8_t Green, uint8_t Blue) {
		return 0xff000000 | (((uint32_t)Red) << 16) | (((uint32_t)Green) << 8) | ((uint32_t)Blue);
	}

	// �J���[�w��
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
	// �`��
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// ���ʂ��l�p
	static void DrawBoxLine_2D(int p1x, int p1y, int p2x, int p2y, const unsigned int& color, int thickness = 1) noexcept {
		if (thickness == 1) {
			DxLib::DrawBox(p1x, p1y, p2x, p2y, color, FALSE);
		}
		else {
			DxLib::DrawLine(p1x, p1y, p1x, p2y, color, thickness);
			DxLib::DrawLine(p1x, p1y, p2x, p1y, color, thickness);
			DxLib::DrawLine(p1x, p2y, p2x, p2y, color, thickness);
			DxLib::DrawLine(p2x, p1y, p2x, p2y, color, thickness);
		}
	}
	// �O���f�[�V�����̂����`��`��
	static void DrawGradationBox_2D(int x1, int y1, int x2, int y2, COLOR_U8 color1, COLOR_U8 color2, const unsigned char UorL = 255) noexcept {
		VERTEX2D Vertex[6]{};
		// ����̒��_�̏����Z�b�g
		Vertex[0].pos.x = static_cast<float>(x1);
		Vertex[0].pos.y = static_cast<float>(y1);
		Vertex[0].pos.z = 0.0f;
		Vertex[0].rhw = 1.0f;
		Vertex[0].dif = color1;
		Vertex[0].u = 0.0f;
		Vertex[0].v = 0.0f;

		// �E��̒��_�̏����Z�b�g
		Vertex[1].pos.x = static_cast<float>(x2);
		Vertex[1].pos.y = static_cast<float>(y1);
		Vertex[1].pos.z = 0.0f;
		Vertex[1].rhw = 1.0f;
		Vertex[1].dif = color2;
		Vertex[1].u = 0.0f;
		Vertex[1].v = 0.0f;

		Vertex[5] = Vertex[1];

		// �����̒��_�̏����Z�b�g
		Vertex[2].pos.x = static_cast<float>(x1);
		Vertex[2].pos.y = static_cast<float>(y2);
		Vertex[2].pos.z = 0.0f;
		Vertex[2].rhw = 1.0f;
		Vertex[2].dif = color1;
		Vertex[2].u = 0.0f;
		Vertex[2].v = 0.0f;

		Vertex[4] = Vertex[2];

		// �E���̒��_�̏����Z�b�g
		Vertex[3].pos.x = static_cast<float>(x2);
		Vertex[3].pos.y = static_cast<float>(y2);
		Vertex[3].pos.z = 0.0f;
		Vertex[3].rhw = 1.0f;
		Vertex[3].u = 0.0f;
		Vertex[3].v = 0.0f;

		switch (UorL) {
		case 3:// ��(�E)
			// ����̒��_�̏����Z�b�g
			Vertex[0].dif = color1;
			// �E��̒��_�̏����Z�b�g
			Vertex[1].dif = color2;
			Vertex[5] = Vertex[1];
			// �����̒��_�̏����Z�b�g
			Vertex[2].dif = color1;
			Vertex[4] = Vertex[2];
			// �E���̒��_�̏����Z�b�g
			Vertex[3].dif = color2;
			break;
		case 2:// �c(��)
			// ����̒��_�̏����Z�b�g
			Vertex[0].dif = color1;
			// �E��̒��_�̏����Z�b�g
			Vertex[1].dif = color1;
			Vertex[5] = Vertex[1];
			// �����̒��_�̏����Z�b�g
			Vertex[2].dif = color2;
			Vertex[4] = Vertex[2];
			// �E���̒��_�̏����Z�b�g
			Vertex[3].dif = color2;
			break;
		case 1:// ��(��)
			// ����̒��_�̏����Z�b�g
			Vertex[0].dif = color2;
			// �E��̒��_�̏����Z�b�g
			Vertex[1].dif = color1;
			Vertex[5] = Vertex[1];
			// �����̒��_�̏����Z�b�g
			Vertex[2].dif = color2;
			Vertex[4] = Vertex[2];
			// �E���̒��_�̏����Z�b�g
			Vertex[3].dif = color1;
			break;
		case 0:// �c(��)
			// ����̒��_�̏����Z�b�g
			Vertex[0].dif = color2;
			// �E��̒��_�̏����Z�b�g
			Vertex[1].dif = color2;
			Vertex[5] = Vertex[1];
			// �����̒��_�̏����Z�b�g
			Vertex[2].dif = color1;
			Vertex[4] = Vertex[2];
			// �E���̒��_�̏����Z�b�g
			Vertex[3].dif = color1;
			break;
		default:
			// ����̒��_�̏����Z�b�g
			Vertex[0].dif = color1;
			// �E��̒��_�̏����Z�b�g
			Vertex[1].dif = color1;
			Vertex[5] = Vertex[1];
			// �����̒��_�̏����Z�b�g
			Vertex[2].dif = color1;
			Vertex[4] = Vertex[2];
			// �E���̒��_�̏����Z�b�g
			Vertex[3].dif = color1;
			break;
		}
		// �|���S����2�`��
		DrawPolygon2D(Vertex, 2, DX_NONE_GRAPH, FALSE);
	}
	// 	x1,y1 Angle��0�̏ꍇ�̍�����W
	// 	x2,y2 Angle��0�̏ꍇ�̉E�����W
	// 	xminp,yminp ����p����̌Œ蒷��
	// 	xmaxp,ymaxp �E���p����̌Œ蒷��
	// 	XCenter,YCenter�@�@: �摜����]�`�悷��摜��̒��S���W(�����(0.0f,0.0f)�A�E����(1.0f,1.0f)�Ƃ�������)
	// 	Angle�@�@�@: �`��p�x�i���W�A���w��j
	// 	GrHandle�@ : �`�悷��O���t�B�b�N�̎��ʔԍ��i�O���t�B�b�N�n���h���j
	// 	TransFlag�@: �摜�̓����x��L���ɂ��邩�ǂ���( TRUE�F�L���ɂ���@FALSE�F�����ɂ��� )
	// 	TilingFlag : �p�ȊO�̕������^�C�����O���邩�g�k�����邩( TRUE�F�^�C�����O�@FALSE�F�g�k )
	static void Draw9SliceGraph(
		int x1, int y1, int x2, int y2,
		int xminp, int yminp, int xmaxp, int ymaxp,
		float XCenter, float YCenter, float Angle,
		int GrHandle, bool TransFlag, bool TilingFlag) noexcept {
		// �Œ���̃T�C�Y���w��
		if (x2 < x1 + xminp + xmaxp) {
			x2 = x1 + xminp + xmaxp;
		}
		if (y2 < y1 + yminp + ymaxp) {
			y2 = y1 + yminp + ymaxp;
		}
		// �p�ӂ��钸�_���
		std::vector<VERTEX2D> Vertex;
		std::vector<unsigned short> Index;

		float xs = static_cast<float>(x2 - x1);
		float ys = static_cast<float>(y2 - y1);

		float CenterX = static_cast<float>(x1) + xs * XCenter;
		float CenterY = static_cast<float>(y1) + ys * YCenter;

		auto SetUpPoint = [&](float xper, float yper, int xc, int yc) {
			Vertex.resize(Vertex.size() + 1);
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
			Index.emplace_back(SetUpPoint(xmin, ymin, xc, yc));// ����̒��_�̏����Z�b�g
			auto RU = SetUpPoint(xmax, ymin, xc + 1, yc);
			auto LD = SetUpPoint(xmin, ymax, xc, yc + 1);
			Index.emplace_back(RU);// �E��̒��_�̏����Z�b�g
			Index.emplace_back(LD);// �����̒��_�̏����Z�b�g
			Index.emplace_back(SetUpPoint(xmax, ymax, xc + 1, yc + 1));// �E���̒��_�̏����Z�b�g
			Index.emplace_back(LD);// �����̒��_�̏����Z�b�g
			Index.emplace_back(RU);// �E��̒��_�̏����Z�b�g
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
		// �^�C�����O
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
				// ��
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
			// ��
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
	// �J�v�Z���`��
	static bool DrawCapsule_3D(const Vector3DX& p1, const Vector3DX& p2, float range, const unsigned int& color, const unsigned int& speccolor) noexcept {
		return DxLib::DrawCapsule3D(p1.get(), p2.get(), range, 8, color, speccolor, TRUE) == TRUE;
	}
	// ���`��
	static bool DrawSphere_3D(const Vector3DX& p1, float range, const unsigned int& color, const unsigned int& speccolor) noexcept {
		return DxLib::DrawSphere3D(p1.get(), range, 8, color, speccolor, TRUE) == TRUE;
	}
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// �`��
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	namespace WindowSystem {
		// 
		class Rect2D {
			int			m_PosX{ 0 };
			int			m_PosY{ 0 };
			int			m_SizeX{ 0 };
			int			m_SizeY{ 0 };
		public:
			const auto& GetPosX() const noexcept { return this->m_PosX; }
			const auto& GetPosY() const noexcept { return this->m_PosY; }
			void			Set(int posx, int posy, int sizex, int sizey) noexcept {
				m_PosX = posx;
				m_PosY = posy;
				m_SizeX = sizex;
				m_SizeY = sizey;
			}
		public:
			bool			IsHit(const Rect2D& target) const noexcept {
				return (
					((this->m_PosX >= target.m_PosX && this->m_PosX < (target.m_PosX + target.m_SizeX)) || (target.m_PosX > this->m_PosX && target.m_PosX <= (this->m_PosX + this->m_SizeX))) &&
					((this->m_PosY >= target.m_PosY && this->m_PosY < (target.m_PosY + target.m_SizeY)) || (target.m_PosY > this->m_PosY && target.m_PosY <= (this->m_PosY + this->m_SizeY)))
					);
			}
		};
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
		};
		class DrawData {
			DrawType								m_type{ DrawType::Box };
			std::array<int, 8>						m_intParam{};
			std::array<unsigned int, 2>				m_UintParam{};
			std::array<float, 6>					m_floatParam{};
			std::array<bool, 1>						m_boolParam{};
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
			void Output() const noexcept;
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
			Back2,
			Back3,
			Back4,
			Normal,
			Front,

			Max,
		};
		// ����
		template <typename... Args>
		extern int GetMsgLen(int ySize, std::string_view String, Args&&... args) noexcept {
			return FontPool::Instance()->Get(FontPool::FontType::MS_Gothic, ySize, 3)->GetStringWidth(InvalidID, ((std::string)String).c_str(), args...);
		}
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
			~DrawControl(void) noexcept {
				for (auto& d : this->m_DrawDatas) {
					d.clear();
				}
				this->m_DrawDatas.clear();

				for (auto& d : this->m_PrevDrawDatas) {
					d.clear();
				}
				this->m_PrevDrawDatas.clear();

			}

			DrawData* GetBack(DrawLayer Layer) noexcept {
				this->m_DrawDatas.at(static_cast<int>(Layer)).resize(this->m_DrawDatas.at(static_cast<int>(Layer)).size() + 1);
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
			void	SetDrawBox(DrawLayer Layer, int x1, int y1, int x2, int y2, unsigned int color1, bool IsFill) noexcept {
				if (!IsDrawOnWindow(x1, y1, x2, y2)) { return; }				// ��ʊO�͕\�����Ȃ�
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::Box);
				Back->InputintParam(0, x1);
				Back->InputintParam(1, y1);
				Back->InputintParam(2, x2);
				Back->InputintParam(3, y2);
				Back->InputUintParam(0, color1);
				Back->InputboolParam(0, IsFill);
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
				if (!IsDrawOnWindow(x1, y1, x2, y2)) { return; }				// ��ʊO�͕\�����Ȃ�
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
			void SetDrawRotaGraph(DrawLayer Layer, const GraphHandle* pGraphHandle, int posx, int posy, float Exrate, float rad, bool trns) noexcept {
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::RotaGraph);
				Back->InputGraphHandleParam(0, pGraphHandle);
				Back->InputintParam(0, posx);
				Back->InputintParam(1, posy);
				Back->InputfloatParam(0, Exrate);
				Back->InputfloatParam(1, rad);
				Back->InputboolParam(0, trns);
			}
			// 
			void SetDrawExtendGraph(DrawLayer Layer, const GraphHandle* pGraphHandle, int x1, int y1, int x2, int y2, bool trns) noexcept {
				if (!IsDrawOnWindow(x1, y1, x2, y2)) { return; }				// ��ʊO�͕\�����Ȃ�
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
			// 
			template <typename... Args>
			void	SetString(DrawLayer Layer, FontPool::FontType type, int fontSize, FontHandle::FontXCenter FontX, FontHandle::FontYCenter FontY, int x, int y, unsigned int Color, unsigned int EdgeColor, const std::string& Str, Args&&... args) noexcept {
				if (Str == "") { return; }
				int xSize = WindowSystem::GetMsgLen(fontSize, Str.c_str(), args...);
				switch (FontX) {
				case FontHandle::FontXCenter::LEFT:
					if (!IsDrawOnWindow((x), (y - fontSize), (x + xSize), (y + fontSize))) { return; }				// ��ʊO�͕\�����Ȃ�
					break;
				case FontHandle::FontXCenter::MIDDLE:
					if (!IsDrawOnWindow((x - xSize / 2), (y - fontSize), (x + xSize / 2), (y + fontSize))) { return; }				// ��ʊO�͕\�����Ȃ�
					break;
				case FontHandle::FontXCenter::RIGHT:
					if (!IsDrawOnWindow((x - xSize), (y - fontSize), (x), (y + fontSize))) { return; }				// ��ʊO�͕\�����Ȃ�
					break;
				default:
					break;
				}

				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::String);

				Back->InputintParam(0, static_cast<int>(type));
				Back->InputintParam(1, fontSize);
				Back->InputintParam(2, static_cast<int>(FontX));
				Back->InputintParam(3, static_cast<int>(FontY));
				Back->InputintParam(4, x);
				Back->InputintParam(5, y);
				Back->InputUintParam(0, Color);
				Back->InputUintParam(1, EdgeColor);

				char ptr[1024] = "";
				snprintfDx(ptr, 1024, Str.c_str(), args...);
				Back->InputStringParam(ptr);
			}
			// 
			const auto	SetStringAutoFit(DrawLayer Layer, FontPool::FontType type, int fontSize, int x1, int y1, int x2, int y2, unsigned int Color, unsigned int EdgeColor, const std::string& Str) noexcept {
				if (Str == "") { return 0.f; }
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::StringAutoFit);

				Back->InputintParam(0, static_cast<int>(type));
				Back->InputintParam(1, fontSize);
				Back->InputintParam(2, x1);
				Back->InputintParam(3, y1);
				Back->InputintParam(4, x2);
				Back->InputintParam(5, y2);
				Back->InputUintParam(0, Color);
				Back->InputUintParam(1, EdgeColor);

				Back->InputStringParam(Str);

				return FontPool::Instance()->Get((FontPool::FontType)type, fontSize, 3)->DrawStringAutoFit(
					x1 + BaseScreenWidth, y1 + BaseScreenHeight,
					x2 + BaseScreenWidth, y2 + BaseScreenHeight,
					Color,
					EdgeColor,
					Str
				);
			}
			// 
		public:
			void	ClearList(void) noexcept {
				for (size_t index = 0; auto & d : this->m_DrawDatas) {
					auto& pd = this->m_PrevDrawDatas.at(index);
					pd.clear();
					for (auto& d2 : d) {
						pd.resize(pd.size() + 1);
						pd.back() = d2;
					}
					index++;
				}
				for (auto& d : this->m_DrawDatas) {
					d.clear();
				}
			}
			void	Draw(void) noexcept {
				auto* DrawCtrls = WindowSystem::DrawControl::Instance();
				bool IsHit = false;
				// �������ǂ����`�F�b�N
				for (size_t index = 0; auto & d : this->m_DrawDatas) {
					auto& pd = this->m_PrevDrawDatas.at(index);
					if (pd.size() == d.size()) {
						for (size_t index2 = 0; auto & d2 : d) {
							auto& pd2 = pd.at(index2);
							if (!(pd2 == d2)) {
								IsHit = true;
								break;
							}
							index2++;
						}
					}
					else {
						IsHit = true;
					}
					index++;
				}
				// 
				if (IsHit) {
					{
						auto NowScreen = GetDrawScreen();
						m_BufferScreen.SetDraw_Screen(true);
						{
							for (auto& d : this->m_DrawDatas) {
								for (auto& d2 : d) {
									d2.Output();
								}
								DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
								SetDrawBright(255, 255, 255);
							}
						}
						GraphHandle::SetDraw_Screen(NowScreen, false);
					}
				}
				// �O�ɕ`�悵�����̂����̂܂܏o��
				m_BufferScreen.DrawGraph(0, 0, true);
			}
		};
		// ��
		extern void SetBox(int xp1, int yp1, int xp2, int yp2, unsigned int colorSet) noexcept;
		// �}�E�X�ŃN���b�N�ł���{�^��
		extern bool SetClickBox(int xp1, int yp1, int xp2, int yp2, unsigned int colorSet, bool IsRepeat, bool IsActive) noexcept;
		// ����
		bool GetMsgPosOn(int* xp1, int* yp1, int ySize, int xSize, FontHandle::FontXCenter FontX) noexcept;
		// �����̒������擾
		template <typename... Args>
		extern void SetMsg(int xp1, int yp1, int ySize, FontHandle::FontXCenter FontX, unsigned int Color, unsigned int EdleColor, std::string_view String, Args&&... args) noexcept {
			if (String == "") { return; }
			//if (!GetMsgPosOn(&xp1, &yp1, ySize, GetMsgLen(ySize, String, args...), FontX)) { return; }
			DrawControl::Instance()->SetString(DrawLayer::Normal, FontPool::FontType::MS_Gothic, ySize,
				FontX, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, EdleColor, ((std::string)String).c_str(), args...);
		}
		// �N���b�N�ł��镶���t�̃{�b�N�X
		template <typename... Args>
		extern bool SetMsgClickBox(int xp1, int yp1, int xp2, int yp2, int StringYSizeMax, unsigned int defaultcolor, bool IsRepeat, bool IsActive, std::string_view String, Args&&... args) noexcept {
			bool ret = SetClickBox(xp1, yp1, xp2, yp2, defaultcolor, IsRepeat, IsActive);
			SetMsg((xp1 + xp2) / 2, (yp1 + yp2) / 2, std::min(StringYSizeMax, yp2 - yp1), FontHandle::FontXCenter::MIDDLE, White, Black, String, args...);
			return ret;
		}
		// �����t�̃{�b�N�X
		template <typename... Args>
		extern void SetMsgBox(int xp1, int yp1, int xp2, int yp2, int StringYSizeMax, unsigned int defaultcolor, std::string_view String, Args&&... args) noexcept {
			SetBox(xp1, yp1, xp2, yp2, defaultcolor);
			SetMsg((xp1 + xp2) / 2, (yp1 + yp2) / 2, std::min(StringYSizeMax, yp2 - yp1), FontHandle::FontXCenter::MIDDLE, White, Black, String, args...);
		}
		// �I���I�t�ł���{�^��
		extern bool CheckBox(int xp1, int yp1, bool switchturn) noexcept;
		// �X���C�_�[
		extern int UpDownBar(int xmin, int xmax, int yp, int value, int valueMin, int valueMax) noexcept;
		// 0~valueMax�̑I�𐧃{�^���W
		extern int UpDownBox(int xmin, int xmax, int yp, int value, int valueMax) noexcept;
		// 
		/*
		class ScrollBoxClass {
			bool		m_IsChangeScrollY{ false }
			int			m_BaseScrollY{ 0 }
			float		m_NowScrollYPer{ 0.f }
		public:
			const auto&		GetNowScrollYPer(void) const noexcept { return this->m_NowScrollYPer; }
			void			ScrollBox(int xp1, int yp1, int xp2, int yp2, float TotalPer, bool IsActive) noexcept {
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
					if (IntoMouse(xp2 - WindowSizeParts->GetUIY(24), yp1, xp2, yp2))
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
				SetBox(xp2 - WindowSizeParts->GetUIY(24), yp1, xp2, yp2, Gray50);
				SetBox(xp2 - WindowSizeParts->GetUIY(24) + WindowSizeParts->GetUIY(1), Yp_s, xp2 - WindowSizeParts->GetUIY(1), Yp_e, color);
			}
		}
		//*/
	}
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// �T�C�h���O
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

		~SideLog(void) noexcept {}
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
	// �|�b�v�A�b�v
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

		~PopUp(void) noexcept {}
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
}
