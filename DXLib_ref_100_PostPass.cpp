#include "DXLib_ref_100_PostPass.hpp"

namespace DXLib_ref {
	//--------------------------------------------------------------------------------------------------
	// �|�X�g�v���Z�X�G�t�F�N�g
	//--------------------------------------------------------------------------------------------------
	//�p���N���X
	class PostPassSSAO : public PostPassBase {
	private:
	public:
		PostPassSSAO(void) {
		}
		~PostPassSSAO(void) {
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph) noexcept {
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->Get_SSAO()) {
				// SSAO�t�B���^�[����
					// �ϊ����Ƃ��Ė@���o�b�t�@���w��
					// �o�͐�o�b�t�@�̎w��
					//
					// �J���[�o�b�t�@���w��
					// �Օ����𒲂ׂ�͈�
					// �Օ������肷��ŏ��[�x�l
					// �Օ������肷��ő�[�x�l
					// �Օ����̉e���̋���
					// �I�N���[�W�����J���[
					// �I�N���[�W�����J���[�̋���
				GraphFilterBlt(m_NormalScreenPtr->get(), TargetGraph->get(), DX_GRAPH_FILTER_SSAO, ColorGraph->get(), 120.f, 0.004f, 0.01f, 0.9f, Black, 20.f);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
		}
	};
	class PostPassSSR : public PostPassBase {
	private:
		GraphHandle SSRColorScreen;	//���̂܂܂�G�o�b�t�@
		GraphHandle SSRNormalScreen;	//�@����G�o�b�t�@
		GraphHandle	SSRDepthScreen;	//�[�x��G�o�b�t�@
		GraphHandle SSRScreen;		//�`��X�N���[��
		GraphHandle SSRScreen2;		//�`��X�N���[��
		GraphHandle bkScreen2;		//SSR�ڂ���
		ShaderUseClass::ScreenVertex	m_SSRScreenVertex;				// ���_�f�[�^
		ShaderUseClass		m_SSR;										// �V�F�[�_�[
	public:
		PostPassSSR(void) {
			auto* DrawParts = DXDraw::Instance();
			SSRScreen = GraphHandle::Make(DrawParts->m_DispXSize / EXTEND, DrawParts->m_DispYSize / EXTEND, true);							//�`��X�N���[��
			SSRScreen2 = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);							//�`��X�N���[��
			{
				bkScreen2 = GraphHandle::Make(DrawParts->m_DispXSize / EXTEND, DrawParts->m_DispYSize / EXTEND, false);							//�ӂ���
				bkScreen2.SetDraw_Screen(true);
				{
					int xr = DrawParts->m_DispXSize / EXTEND * 30 / 100;
					int yr = DrawParts->m_DispYSize / EXTEND * 60 / 100;

					DrawBox_2D(0, 0, DrawParts->m_DispXSize / EXTEND, DrawParts->m_DispYSize / EXTEND, Black, TRUE);
					DrawOval(DrawParts->m_DispXSize / EXTEND / 2, DrawParts->m_DispYSize / EXTEND / 2, xr, yr, White, TRUE);

					int r = 0, c = 0, p = 2;

					p = 1;
					for (r = 0; r < 255; r += p) {
						c = 255 - int(std::powf(float(255 - r) / 255.f, 1.5f)*255.f);

						DrawOval(DrawParts->m_DispXSize / EXTEND / 2, DrawParts->m_DispYSize / EXTEND / 2, xr - r / p, yr - r / p, GetColor(c, c, c), FALSE, 2);
					}
				}
			}
			{
				SSRColorScreen = GraphHandle::Make(DrawParts->m_DispXSize / EXTEND, DrawParts->m_DispYSize / EXTEND, false);
				SSRNormalScreen = GraphHandle::Make(DrawParts->m_DispXSize / EXTEND, DrawParts->m_DispYSize / EXTEND, false);			// �@��
				{
					// �[�x��`�悷��e�N�X�`���̍쐬( 2�`�����l�����������_32�r�b�g�e�N�X�`�� )
					auto prevMip = GetCreateDrawValidGraphChannelNum();
					auto prevFloatType = GetDrawValidFloatTypeGraphCreateFlag();
					auto prevBit = GetCreateGraphChannelBitDepth();
					SetCreateDrawValidGraphChannelNum(2);
					SetDrawValidFloatTypeGraphCreateFlag(TRUE);
					SetCreateGraphChannelBitDepth(32);
					SSRDepthScreen = GraphHandle::Make(DrawParts->m_DispXSize / EXTEND, DrawParts->m_DispYSize / EXTEND, false);
					SetCreateDrawValidGraphChannelNum(prevMip);
					SetDrawValidFloatTypeGraphCreateFlag(prevFloatType);
					SetCreateGraphChannelBitDepth(prevBit);
				}
			}
			m_SSRScreenVertex.SetScreenVertex(DrawParts->m_DispXSize / EXTEND, DrawParts->m_DispYSize / EXTEND);	// ���_�f�[�^�̏���
			m_SSR.Init("shader/VS_SSR.vso", "shader/PS_SSR.pso");					// �����Y
		}
		~PostPassSSR(void) {
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* OptionParts = OPTION::Instance();
			auto* PostPassParts = PostPassEffect::Instance();

			if (OptionParts->Get_SSR()) {
				GraphFilterBlt(ColorGraph->get(), SSRColorScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
				GraphFilterBlt(m_NormalScreenPtr->get(), SSRNormalScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
				GraphFilterBlt(m_DepthScreenPtr->get(), SSRDepthScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
				SSRScreen.SetDraw_Screen();
				{
					int RayInterval = 50;//���C�̕����Ԋu
					float SSRScale = 12.5f;
					float DepthThreshold = 8.f;
					SetUseTextureToShader(0, SSRColorScreen.get());	//�g�p����e�N�X�`�����Z�b�g
					SetUseTextureToShader(1, SSRNormalScreen.get());
					SetUseTextureToShader(2, SSRDepthScreen.get());
					m_SSR.SetPixelDispSize(DrawParts->m_DispXSize / EXTEND, DrawParts->m_DispYSize / EXTEND);
					m_SSR.SetPixelParam(3, (float)RayInterval, SSRScale, std::tan(PostPassParts->Get_fov() / 2.f), DepthThreshold);
					{
						m_SSR.Draw(m_SSRScreenVertex);
					}
					SetUseTextureToShader(0, -1);
					SetUseTextureToShader(1, -1);
					SetUseTextureToShader(2, -1);
				}
				SSRScreen2.SetDraw_Screen();
				{
					TargetGraph->DrawGraph(0, 0, true);
					SSRScreen.DrawExtendGraph(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);
					//DrawBox_2D(0, 0, 1920, 1080, Red, TRUE);
				}
				GraphBlend(SSRScreen2.get(), bkScreen2.get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
					DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_BLEND_R);
				TargetGraph->SetDraw_Screen(false);
				{
					SSRScreen2.DrawGraph(0, 0, true);
				}
			}
			if (OptionParts->Get_Bloom()) {
				TargetGraph->SetDraw_Screen(false);
				{
				}
			}
		}
	};
	class PostPassDoF : public PostPassBase {
	private:
		GraphHandle			DoFScreen;		//�`��X�N���[��
		ShaderUseClass::ScreenVertex	m_ScreenVertex;					// ���_�f�[�^
		ShaderUseClass		m_DoF;			// �V�F�[�_�[
	public:
		PostPassDoF(void) {
			auto* DrawParts = DXDraw::Instance();
			DoFScreen = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);							//�`��X�N���[��
			m_ScreenVertex.SetScreenVertex(DrawParts->m_DispXSize, DrawParts->m_DispYSize);							// ���_�f�[�^�̏���
			m_DoF.Init("shader/VS_DoF.vso", "shader/PS_DoF.pso");					// DoF
		}
		~PostPassDoF(void) {
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* OptionParts = OPTION::Instance();
			auto* PostPassParts = PostPassEffect::Instance();

			if (OptionParts->Get_DoF()) {
				if (GetUseDirect3DVersion() == DX_DIRECT3D_11) {
					GraphFilterBlt(TargetGraph->get(), DoFScreen.get(), DX_GRAPH_FILTER_GAUSS, 16, 2000);
					TargetGraph->SetDraw_Screen();
					{
						SetUseTextureToShader(0, ColorGraph->get());	//�g�p����e�N�X�`�����Z�b�g
						SetUseTextureToShader(1, DoFScreen.get());
						SetUseTextureToShader(2, m_DepthScreenPtr->get());
						m_DoF.SetPixelDispSize(DrawParts->m_DispXSize, DrawParts->m_DispYSize);
						m_DoF.SetPixelParam(3, PostPassParts->Get_near_DoF(), PostPassParts->Get_far_DoF(), PostPassParts->Get_near_DoFMax(), PostPassParts->Get_far_DoFMin());
						{
							m_DoF.Draw(m_ScreenVertex);
						}
						SetUseTextureToShader(0, -1);
						SetUseTextureToShader(1, -1);
						SetUseTextureToShader(2, -1);
					}
				}
			}
		}
	};
	class PostPassBloom : public PostPassBase {
	private:
		GraphHandle GaussScreen_;	//�`��X�N���[��
		GraphHandle	BufScreen;
	public:
		PostPassBloom(void) {
			auto* DrawParts = DXDraw::Instance();
			GaussScreen_ = GraphHandle::Make(DrawParts->m_DispXSize / EXTEND, DrawParts->m_DispYSize / EXTEND, true);
			BufScreen = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);							//�`��X�N���[��
		}
		~PostPassBloom(void) {
			GaussScreen_.Dispose();
			BufScreen.Dispose();
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* OptionParts = OPTION::Instance();

			if (OptionParts->Get_Bloom()) {
				GraphFilterBlt(TargetGraph->get(), BufScreen.get(), DX_GRAPH_FILTER_TWO_COLOR, 250, Black, 255, Gray50, 255);
				GraphFilterBlt(BufScreen.get(), GaussScreen_.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
				GraphFilter(GaussScreen_.get(), DX_GRAPH_FILTER_GAUSS, 16, 1000);
				TargetGraph->SetDraw_Screen(false);
				{
					SetDrawMode(DX_DRAWMODE_BILINEAR);
					SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
					GaussScreen_.DrawExtendGraph(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);
					GaussScreen_.DrawExtendGraph(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
			}
		}
	};
	class PostPassAberration : public PostPassBase {
	private:
		std::array<GraphHandle, 3>		BufScreen;
	public:
		PostPassAberration(void) {
			auto* DrawParts = DXDraw::Instance();
			for (auto&buf : BufScreen) {
				buf = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);							//�`��X�N���[��
			}
		}
		~PostPassAberration(void) {
			for (auto&buf : BufScreen) {
				buf.Dispose();
			}
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*) noexcept {
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->Get_aberration()) {
				auto* DrawParts = DXDraw::Instance();
				BufScreen[0].SetDraw_Screen(true);
				DrawBox_2D(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, Black, TRUE);
				BufScreen[1].SetDraw_Screen(true);
				DrawBox_2D(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, Black, TRUE);
				BufScreen[2].SetDraw_Screen(true);
				DrawBox_2D(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, Black, TRUE);
				GraphBlend(BufScreen[0].get(), TargetGraph->get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
					DX_RGBA_SELECT_BLEND_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_SRC_A);
				GraphBlend(BufScreen[1].get(), TargetGraph->get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
					DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_BLEND_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_SRC_A);
				GraphBlend(BufScreen[2].get(), TargetGraph->get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
					DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_BLEND_B, DX_RGBA_SELECT_SRC_A);
				//output_high = 255;
				TargetGraph->SetDraw_Screen(true);
				{
					DrawBox_2D(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, Black, TRUE);
					SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
					BufScreen[0].DrawRotaGraph(DrawParts->m_DispXSize / 2, DrawParts->m_DispYSize / 2, 1.f + 0.005f*DrawParts->GetAberrationPower(), 0.f, true);
					BufScreen[1].DrawRotaGraph(DrawParts->m_DispXSize / 2, DrawParts->m_DispYSize / 2, 1.f, 0.f, true);
					BufScreen[2].DrawRotaGraph(DrawParts->m_DispXSize / 2, DrawParts->m_DispYSize / 2, 1.f - 0.005f*DrawParts->GetAberrationPower(), 0.f, true);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
			}
		}
	};
	class PostPassMotionBlur : public PostPassBase {
	private:
		class BlurScreen {
			static const int MAX = 3;
			GraphHandle m_screen[MAX];
			int m_current{ 0 };
			int m_alpha{ 0 };
			int m_screenWidth{ 0 }, m_screenHeight{ 0 };
			int m_offsetX1{ 0 }, m_offsetX2{ 0 }, m_offsetY1{ 0 }, offsetY2{ 0 };
			int m_notBlendDraw{ 0 };
		public:
			void Init(int t_alpha, int t_offsetX1, int t_offsetY1, int t_offsetX2, int t_offsetY2) {
				auto* DrawParts = DXDraw::Instance();

				for (int i = 0; i < MAX; ++i) {
					m_screen[i] = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize);
				}
				m_current = 0;
				m_alpha = t_alpha;
				m_offsetX1 = t_offsetX1;
				m_offsetX2 = t_offsetX2;
				m_offsetY1 = t_offsetY1;
				offsetY2 = t_offsetY2;

				m_notBlendDraw = 0;
			}
			void Release() {
				for (int i = 0; i < MAX; ++i)
				{
					m_screen[i].Dispose();
				}
			}
		public:
			auto* PostRenderBlurScreen(std::function<void()> doing) {
				auto* DrawParts = DXDraw::Instance();
				auto next = (m_current != 0) ? (m_current - 1) : MAX - 1;
				m_screen[m_current].SetDraw_Screen();
				{
					doing();
					if (m_notBlendDraw++ > MAX) {
						int drawMode = GetDrawMode();
						SetDrawMode(DX_DRAWMODE_BILINEAR);
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_alpha);
						m_screen[next].DrawExtendGraph(m_offsetX1, m_offsetY1, DrawParts->m_DispXSize + m_offsetX2, DrawParts->m_DispYSize + offsetY2, false);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						SetDrawMode(drawMode);
					}
				}
				auto Cur = m_current;
				m_current = next;
				return &m_screen[Cur];
			}
		};
	private:
		BlurScreen				m_BlurScreen;
	public:
		PostPassMotionBlur(void) {
			m_BlurScreen.Init(96, -6, -6, 6, 6);
		}
		~PostPassMotionBlur(void) {
			m_BlurScreen.Release();
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*) noexcept {
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->Get_MotionBlur()) {
				GraphHandle* buf = m_BlurScreen.PostRenderBlurScreen([&]() {
					TargetGraph->DrawGraph(0, 0, false);
				});
				TargetGraph->SetDraw_Screen(false);
				{
					buf->DrawGraph(0, 0, false);
				}
			}
		}
	};
	class PostPassCornerBlur : public PostPassBase {
	private:
		GraphHandle AberrationScreen;	//�`��X�N���[��
		GraphHandle bkScreen2;
		GraphHandle	BufScreen;
	public:
		PostPassCornerBlur(void) {
			auto* DrawParts = DXDraw::Instance();
			AberrationScreen = GraphHandle::Make(DrawParts->m_DispXSize / EXTEND, DrawParts->m_DispYSize / EXTEND, true);	//�`��X�N���[��
			{
				bkScreen2 = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, false);							//�ӂ���
				bkScreen2.SetDraw_Screen(true);
				{
					int xr = DrawParts->m_DispXSize * 60 / 100;
					int yr = DrawParts->m_DispYSize * 70 / 100;

					DrawBox_2D(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, Black, TRUE);
					DrawOval(DrawParts->m_DispXSize / 2, DrawParts->m_DispYSize / 2, xr, yr, White, TRUE);

					int r = 0, c = 0, p = 2;

					p = 1;
					for (r = 0; r < 255; r += p) {
						c = 255 - int(std::powf(float(255 - r) / 255.f, 1.5f)*255.f);

						DrawOval(DrawParts->m_DispXSize / 2, DrawParts->m_DispYSize / 2, xr - r / p, yr - r / p, GetColor(c, c, c), FALSE, 2);
					}
				}
			}
			BufScreen = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);							//�`��X�N���[��
		}
		~PostPassCornerBlur(void) {
			AberrationScreen.Dispose();
			bkScreen2.Dispose();
			BufScreen.Dispose();
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* OptionParts = OPTION::Instance();
			//���ʕ`��
			if (OptionParts->Get_aberration()) {
				GraphFilterBlt(TargetGraph->get(), AberrationScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
				GraphFilter(AberrationScreen.get(), DX_GRAPH_FILTER_GAUSS, 16, 1000);

				BufScreen.SetDraw_Screen();
				{
					TargetGraph->DrawGraph(0, 0, true);
				}
				GraphBlend(BufScreen.get(), bkScreen2.get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
					DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_BLEND_R);
				TargetGraph->SetDraw_Screen(false);
				{
					AberrationScreen.DrawExtendGraph(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, false);
					BufScreen.DrawGraph(0, 0, true);
				}
			}
		}
	};
	class PostPassVignette : public PostPassBase {
	private:
		GraphHandle bkScreen;
		GraphHandle	BufScreen;
	public:
		PostPassVignette(void) {
			auto* DrawParts = DXDraw::Instance();
			//auto* OptionParts = OPTION::Instance();
			{
				bkScreen = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, false);							//�ӂ���
				bkScreen.SetDraw_Screen(true);
				{
					int y = 0, c = 0, p = 2;
					DrawBox_2D(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, White, TRUE);

					p = 1;
					for (y = 0; y < 255; y += p) {
						c = 255 - int(std::powf(float(255 - y) / 255.f, 1.5f)*64.f);
						DrawLine_2D(0, y / p, DrawParts->m_DispXSize, y / p, GetColor(c, c, c));
					}
					p = 2;
					for (y = 0; y < 255; y += p) {
						c = 255 - int(std::powf(float(255 - y) / 255.f, 1.5f)*128.f);
						DrawLine_2D(0, DrawParts->m_DispYSize - y / p, DrawParts->m_DispXSize, DrawParts->m_DispYSize - y / p, GetColor(c, c, c));
					}
				}
			}
			BufScreen = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);							//�`��X�N���[��
		}
		~PostPassVignette(void) {
			BufScreen.Dispose();
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*) noexcept {
			auto* DrawParts = DXDraw::Instance();

			BufScreen.SetDraw_Screen();
			{
				TargetGraph->DrawGraph(0, 0, true);
			}
			GraphBlend(BufScreen.get(), bkScreen.get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_BLEND_R);

			TargetGraph->SetDraw_Screen();
			{
				DrawBox_2D(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, Black, TRUE);
				BufScreen.DrawGraph(0, 0, true);
			}
		}
	};

	class PostPassDistortion : public PostPassBase {
	private:
		GraphHandle	BufScreen;
	public:
		PostPassDistortion(void) {
			auto* DrawParts = DXDraw::Instance();
			BufScreen = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);							//�`��X�N���[��
		}
		~PostPassDistortion(void) {
			BufScreen.Dispose();
		}
	private:
		// ��ʂ�c�܂��Ȃ���`�悷��֐�
		void DrawCircleScreen(
			int CenterX,			// �~�̒��S���WX
			int CenterY,			// �~�̒��S���WY
			float Radius,			// �~�̃T�C�Y
			float Absorption,		// �����̉~�Ɉ������܂��h�b�g��
			const GraphHandle& ScreenHandle// ��ʃO���t�B�b�N�n���h��
		)
		{
			auto* DrawParts = DXDraw::Instance();
			const int CIRCLE_ANGLE_VERTEX_NUM = 16;			// �~���̒��_��
			const int CIRCLE_RADIUS_VERTEX_NUM = 8;			// ���a�̒��_��

			float CenterDistance;
			float GraphCenterDistance;
			float AbsorptionDistance;
			float AbsorptionMoveX, AbsorptionMoveY;
			float Angle;
			float Sin, Cos;
			COLOR_U8 DiffuseColor;
			int i;
			VERTEX2D *Vert;
			WORD *Ind;
			float AngleCosTable[CIRCLE_ANGLE_VERTEX_NUM];
			float AngleSinTable[CIRCLE_ANGLE_VERTEX_NUM];
			float InCircleCosTable[CIRCLE_RADIUS_VERTEX_NUM];

			// �X�^�b�N�ɐςނɂ͑傫���̂� static �z��ɂ��܂���
			static VERTEX2D Vertex[CIRCLE_RADIUS_VERTEX_NUM * CIRCLE_ANGLE_VERTEX_NUM];
			static WORD Index[CIRCLE_ANGLE_VERTEX_NUM * 6 * (CIRCLE_RADIUS_VERTEX_NUM - 1)];

			// �ŏ��ɕ��ʂɕ`��
			DrawGraph(0, 0, ScreenHandle.get(), FALSE);

			// �`��J���[���쐬���Ă���
			DiffuseColor = GetColorU8(255, 255, 255, 255);

			// �O�������p�� Sin, Cos �e�[�u�����쐬����
			Angle = 0.0f;
			for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; i++, Angle += DX_PI_F * 2.0f / CIRCLE_ANGLE_VERTEX_NUM)
			{
				AngleSinTable[i] = (float)sin((double)Angle);
				AngleCosTable[i] = (float)cos((double)Angle);
			}

			// �����̐���オ���Ă���悤�Ɍ�����ӏ��Ŏg�p���� Cos �e�[�u�����쐬����
			Angle = 0.0f;
			for (i = 0; i < CIRCLE_RADIUS_VERTEX_NUM; i++, Angle += (DX_PI_F / 2.0f) / (CIRCLE_RADIUS_VERTEX_NUM - 1))
			{
				InCircleCosTable[i] = (float)cos((double)Angle);
			}

			// �|���S�����_�C���f�b�N�X�̏���
			Ind = Index;
			for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; i++)
			{
				for (WORD j = 0; j < CIRCLE_RADIUS_VERTEX_NUM - 1; j++, Ind += 6)
				{
					Ind[0] = (WORD)(i * CIRCLE_RADIUS_VERTEX_NUM + j);
					Ind[1] = Ind[0] + 1;
					if (i == CIRCLE_ANGLE_VERTEX_NUM - 1)
					{
						Ind[2] = j;
						Ind[3] = j + 1;
					}
					else
					{
						Ind[2] = Ind[0] + CIRCLE_RADIUS_VERTEX_NUM;
						Ind[3] = Ind[0] + 1 + CIRCLE_RADIUS_VERTEX_NUM;
					}
					Ind[4] = Ind[2];
					Ind[5] = Ind[1];
				}
			}

			// �o�C���j�A��ԕ`��ɂ���
			SetDrawMode(DX_DRAWMODE_BILINEAR);


			// �O���̃h�[�i�c������`��

			// ���S�Ɍ������ɂ��������Ē��S�����Ƀe�N�X�`�����W�����炷
			Vert = Vertex;
			for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; i++)
			{
				// �g�p���� Sin, Cos �̒l���Z�b�g
				Sin = AngleSinTable[i];
				Cos = AngleCosTable[i];

				for (int j = 0; j < CIRCLE_RADIUS_VERTEX_NUM; j++, Vert++)
				{
					// �~�̒��S�܂ł̋������Z�o
					CenterDistance = Radius;

					// ���S�Ɉ������܂�鋗�����Z�o
					AbsorptionDistance = Absorption * j / (CIRCLE_RADIUS_VERTEX_NUM - 1);

					// ���S�Ɍ������Ĉړ����鋗�����Z�o
					AbsorptionMoveX = Cos * AbsorptionDistance;
					AbsorptionMoveY = Sin * AbsorptionDistance;

					// �X�N���[�����W�̌���
					Vert->pos.x = Cos * CenterDistance + CenterX;
					Vert->pos.y = Sin * CenterDistance + CenterY;
					Vert->pos.z = 0.0f;

					// �e�N�X�`�����W�̃Z�b�g
					Vert->u = (Vert->pos.x + AbsorptionMoveX) / DrawParts->m_DispXSize;
					Vert->v = (Vert->pos.y + AbsorptionMoveY) / DrawParts->m_DispYSize;

					// ���̑��̃p�����[�^���Z�b�g
					Vert->rhw = 1.0f;
					Vert->dif = DiffuseColor;
				}
			}

			// �c�񂾃h�[�i�c�̕`��
			DrawPrimitiveIndexed2D(Vertex, sizeof(Vertex) / sizeof(VERTEX2D), Index, sizeof(Index) / sizeof(WORD), DX_PRIMTYPE_TRIANGLELIST, ScreenHandle.get(), FALSE);


			// �����̐���オ���Ă���悤�Ɍ����镔����`��

			// Cos�e�[�u���ɂ��������ăe�N�X�`�����W�����炷
			Vert = Vertex;
			for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; i++)
			{
				// �g�p���� Sin, Cos �̒l���Z�b�g
				Sin = AngleSinTable[i];
				Cos = AngleCosTable[i];

				for (int j = 0; j < CIRCLE_RADIUS_VERTEX_NUM; j++, Vert++)
				{
					// �~�̒��S�܂ł̋������Z�o
					CenterDistance = InCircleCosTable[j] * Radius;

					// �摜���W���_�ł̉~�̒��S�܂ł̋������Z�o
					GraphCenterDistance = ((CIRCLE_RADIUS_VERTEX_NUM - 1) - j) * (Absorption + Radius) / (CIRCLE_RADIUS_VERTEX_NUM - 1);

					// �X�N���[�����W�̌���
					Vert->pos.x = Cos * CenterDistance + CenterX;
					Vert->pos.y = Sin * CenterDistance + CenterY;
					Vert->pos.z = 0.0f;

					// �e�N�X�`�����W�̃Z�b�g
					Vert->u = (Cos * GraphCenterDistance + CenterX) / DrawParts->m_DispXSize;
					Vert->v = (Sin * GraphCenterDistance + CenterY) / DrawParts->m_DispYSize;

					// ���̑��̃p�����[�^���Z�b�g
					Vert->rhw = 1.0f;
					Vert->dif = DiffuseColor;
				}
			}

			// ���S�̐���オ���Č����镔����`��
			DrawPrimitiveIndexed2D(Vertex, sizeof(Vertex) / sizeof(VERTEX2D), Index, sizeof(Index) / sizeof(WORD), DX_PRIMTYPE_TRIANGLELIST, ScreenHandle.get(), FALSE);
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*) noexcept {
			auto* DrawParts = DXDraw::Instance();

			BufScreen.SetDraw_Screen();
			{
				TargetGraph->DrawGraph(0, 0, true);
			}

			TargetGraph->SetDraw_Screen();
			{
				// ��ʂ�c�܂��ĕ`��
				DrawCircleScreen(
					DrawParts->m_DispXSize / 2, DrawParts->m_DispYSize / 2,	// ���S���W
					(float)(DrawParts->m_DispXSize * 2 / 3),	// �~�̃T�C�Y
					120.0f,	// �����Ɉ������܂��h�b�g��
					BufScreen);
				//BufScreen.DrawGraph(0, 0, true);
			}
		}
	};

	class PostPassFXAA : public PostPassBase {
	private:
		ShaderUseClass::ScreenVertex	m_ScreenVertex;					// ���_�f�[�^
		ShaderUseClass		m_FXAA;			// �V�F�[�_�[
	public:
		PostPassFXAA(void) {
			auto* DrawParts = DXDraw::Instance();
			m_ScreenVertex.SetScreenVertex(DrawParts->m_DispXSize, DrawParts->m_DispYSize);							// ���_�f�[�^�̏���
			m_FXAA.Init("shader/FXAA_VS.vso", "shader/FXAA_PS.pso");					// FXAA
		}
		~PostPassFXAA(void) {}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* OptionParts = OPTION::Instance();

			if (OptionParts->Get_FXAA()) {
				TargetGraph->SetDraw_Screen();
				{
					SetUseTextureToShader(0, ColorGraph->get());	//�g�p����e�N�X�`�����Z�b�g
					m_FXAA.SetPixelDispSize(DrawParts->m_DispXSize, DrawParts->m_DispYSize);
					{
						m_FXAA.Draw(m_ScreenVertex);
					}
					SetUseTextureToShader(0, -1);
				}
			}
		}
	};
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	const PostPassEffect* SingletonBase<PostPassEffect>::m_Singleton = nullptr;
	//
	PostPassEffect::PostPassEffect(void) {
		auto* DrawParts = DXDraw::Instance();
		FarScreen_ = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, false);		//�`��X�N���[��
		NearScreen_ = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);		//�`��X�N���[��
		MAIN_Screen = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, false);		//�ŏI�`��p
		//G�o�b�t�@
		auto* OptionParts = OPTION::Instance();
		if (!OptionParts->Get_LightMode()) {
			ColorScreen = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, false);
			NormalScreen = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, false);	// �@��G�o�b�t�@
			{
				// �[�x��`�悷��e�N�X�`��G�o�b�t�@�̍쐬( 2�`�����l�����������_32�r�b�g�e�N�X�`�� )
				auto prevMip = GetCreateDrawValidGraphChannelNum();
				auto prevFloatType = GetDrawValidFloatTypeGraphCreateFlag();
				auto prevBit = GetCreateGraphChannelBitDepth();
				SetCreateDrawValidGraphChannelNum(2);
				SetDrawValidFloatTypeGraphCreateFlag(TRUE);
				SetCreateGraphChannelBitDepth(32);
				DepthScreen = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, false);
				SetCreateDrawValidGraphChannelNum(prevMip);
				SetDrawValidFloatTypeGraphCreateFlag(prevFloatType);
				SetCreateGraphChannelBitDepth(prevBit);
			}
			//�[�x�`��p
			//DepthDraw.Init("shader/NormalMesh_PointLightVS.vso", "shader/NormalMesh_PointLightPS.pso");
			//�|�X�g�G�t�F�N�g
			m_PostPass.emplace_back(std::make_unique<PostPassSSAO>());
			m_PostPass.emplace_back(std::make_unique<PostPassSSR>());
			m_PostPass.emplace_back(std::make_unique<PostPassDoF>());
			m_PostPass.emplace_back(std::make_unique<PostPassDistortion>());
			m_PostPass.emplace_back(std::make_unique<PostPassBloom>());
			m_PostPass.emplace_back(std::make_unique<PostPassAberration>());
			m_PostPass.emplace_back(std::make_unique<PostPassMotionBlur>());
			m_PostPass.emplace_back(std::make_unique<PostPassVignette>());
			m_PostPass.emplace_back(std::make_unique<PostPassCornerBlur>());
			m_PostPass.emplace_back(std::make_unique<PostPassFXAA>());

			for (auto& P : m_PostPass) {
				P->SetPtr(&NormalScreen, &DepthScreen);
			}
		}
	}
	PostPassEffect::~PostPassEffect(void) noexcept {
		for (auto& P : m_PostPass) {
			P.reset();
		}
		m_PostPass.clear();
	}
	//
	void PostPassEffect::Draw(std::function<void()> sky_doing, std::function<void()> doing, std::function<void()> doingFront, const Camera3DInfo& cams) {
		//near�ɕ`��
		auto* DrawParts = DXDraw::Instance();
		auto* OptionParts = OPTION::Instance();
		//�S�Ẳ�ʂ�������
		{
			if (!OptionParts->Get_LightMode()) {
				NormalScreen.SetDraw_Screen();//���Z�b�g�ւ��
				DepthScreen.SetDraw_Screen();//���Z�b�g�ւ��
			}
			FarScreen_.SetDraw_Screen();//���Z�b�g�ւ��
			NearScreen_.SetDraw_Screen();//���Z�b�g�ւ��
		}
		//G�o�b�t�@�ɕ`��
		auto G_Draw = [&](GraphHandle* TargetDraw, float near_len, float far_len, std::function<void()> done) {
			// �J���[�o�b�t�@��`��Ώ�0�ɁA�@���o�b�t�@��`��Ώ�1�ɐݒ�
			SetRenderTargetToShader(0, TargetDraw->get());
			if (!OptionParts->Get_LightMode()) {
				SetRenderTargetToShader(1, NormalScreen.get());
				SetRenderTargetToShader(2, DepthScreen.get());
			}
			SetCameraNearFar(near_len, far_len);
			SetupCamera_Perspective(cams.GetCamFov());
			SetCameraPositionAndTargetAndUpVec(cams.GetCamPos().get(), cams.GetCamVec().get(), cams.GetCamUp().get());
			{
				done();
			}
			SetRenderTargetToShader(0, -1);
			SetRenderTargetToShader(1, -1);
			SetRenderTargetToShader(2, -1);
		};
		//��
		G_Draw(&FarScreen_, 1000.0f, 50000.0f, [&]() {
			sky_doing();
			   });
		//������
		G_Draw(&FarScreen_, cams.GetCamFar() - 10.f, 1000000.f, [&]() {
			if (OptionParts->Get_Shadow()) {
				DrawParts->SetUseShadow();
				doing();
				DrawParts->ResetUseShadow();
			}
			else {
				doing();
			}
			doingFront();
			   });
		//�������̋���Ȃڂ���
		if (OptionParts->Get_DoF()) {
			GraphFilter(FarScreen_.get(), DX_GRAPH_FILTER_GAUSS, 16, 200);
		}
		//����
		G_Draw(&NearScreen_, cams.GetCamNear(), cams.GetCamFar(), [&]() {
			FarScreen_.DrawGraph(0, 0, false);
			Effekseer_Sync3DSetting();
			if (OptionParts->Get_Shadow()) {
				DrawParts->SetUseShadow();
				doing();
				DrawParts->ResetUseShadow();
			}
			else {
				doing();
			}
			DrawEffekseer3D();
			doingFront();
			   });
		//����
		G_Draw(&NearScreen_, 0.1f, 0.1f + cams.GetCamNear(), [&]() {
			Effekseer_Sync3DSetting();
			if (OptionParts->Get_Shadow()) {
				DrawParts->SetUseShadow();
				doing();
				DrawParts->ResetUseShadow();
			}
			else {
				doing();
			}
			DrawEffekseer3D();
			doingFront();
			   });
		//fov���L�����Ă���
		fov = cams.GetCamFov();

		if (OPTION::Instance()->Get_Shadow()) {
			Plus_Draw([&]() {
				DrawParts->GetShadowAfterDraw();
									 });

		}
		//�|�X�g�p�X�G�t�F�N�g�̓K�p
		SetPostpassEffect();
	}
	//
	void PostPassEffect::Plus_Draw(std::function<void()> doing) noexcept {
		NearScreen_.SetDraw_Screen(false);
		{
			doing();
		}
		//���ʕ`��
		MAIN_Screen.SetDraw_Screen();
		{
			NearScreen_.DrawGraph(0, 0, false);
		}
	}
	//
	void PostPassEffect::SetPostpassEffect(void) noexcept {
		//�F���␳
		{
			int output_low = 0;
			int output_high = 255;
			GraphFilter(NearScreen_.get(), DX_GRAPH_FILTER_LEVEL, InColorPerMin, InColorPerMax, int(InColorGamma * 100), output_low, output_high);
		}
		//buf�ɕ`��
		/**/
		auto* OptionParts = OPTION::Instance();
		for (auto& P : m_PostPass) {
			if (!OptionParts->Get_LightMode()) {
				ColorScreen.SetDraw_Screen();
				{
					NearScreen_.DrawGraph(0, 0, false);
				}
			}
			P->SetEffect(&NearScreen_, &ColorScreen);
		}
		//*/
		//���ʕ`��
		MAIN_Screen.SetDraw_Screen();
		{
			SetDrawBright(r_brt, g_brt, b_brt);
			NearScreen_.DrawGraph(0, 0, false);
			SetDrawBright(255, 255, 255);
			
			//NormalScreen.DrawExtendGraph(0, 0, 1920, 1080, false);
			//DepthScreen.DrawExtendGraph(0, 0, 1920, 1080, false);
		}
	}
	//�������񂾐[�x�ɉ����đΉ�
	void PostPassEffect::DrawByDepth(std::function<void()> doing) noexcept {
		/*
		DepthDraw.SetPixelParam(3, 3.f*12.5f, 0, 0, 0);
		DepthDraw.Draw_lamda([&] {
			SetUseTextureToShader(1, DepthScreen.get());
			doing();
		});
		//*/
	}
};
