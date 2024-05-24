#include "DXLib_ref_100_PostPass.hpp"

namespace DXLib_ref {
	//--------------------------------------------------------------------------------------------------
	// �|�X�g�v���Z�X�G�t�F�N�g
	//--------------------------------------------------------------------------------------------------
	//�p���N���X
	class PostPassSSAO : public PostPassBase {
	public:
		void Load_Sub() noexcept override {
		}
		void Dispose_Sub() noexcept override {
		}
		bool IsActive_Sub() noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::SSAO);
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph) noexcept override {
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
	};
	class PostPassSSR : public PostPassBase {
	private:
		GraphHandle SSRColorScreen;	//���̂܂܂�G�o�b�t�@
		GraphHandle SSRNormalScreen;	//�@����G�o�b�t�@
		GraphHandle	SSRDepthScreen;	//�[�x��G�o�b�t�@
		GraphHandle SSRScreen;		//�`��X�N���[��
		GraphHandle SSRScreen0;		//�`��X�N���[��
		GraphHandle SSRScreen1;		//�`��X�N���[��
		GraphHandle SSRScreen2;		//�`��X�N���[��
		GraphHandle bkScreen2;		//SSR�ڂ���
		ShaderUseClass::ScreenVertex	m_SSRScreenVertex;				// ���_�f�[�^
		ShaderUseClass		m_SSR;										// �V�F�[�_�[

		int SP = 1;
		int RayInterval = 150;//���C�̕����Ԋu
		float SSRScale = 12.5f;
		float DepthThreshold = 10.f;

		static const int SSREX = 4;
		static const int SSREX2 = 8;
	public:
		void Load_Sub() noexcept override {
			SSRScreen = GraphHandle::Make(y_r(1920) / SSREX, y_r(1080) / SSREX, true);							//�`��X�N���[��
			SSRScreen0 = GraphHandle::Make(y_r(1920) / SSREX, y_r(1080) / SSREX, true);							//�`��X�N���[��
			SSRScreen1 = GraphHandle::Make(y_r(1920) / SSREX2, y_r(1080) / SSREX2, true);							//�`��X�N���[��
			SSRScreen2 = GraphHandle::Make(y_r(1920), y_r(1080), true);							//�`��X�N���[��
			{
				bkScreen2 = GraphHandle::Make(y_r(1920) / SSREX, y_r(1080) / SSREX, false);							//�ӂ���
				bkScreen2.SetDraw_Screen(true);
				{
					int xr = y_r(1920) / SSREX * 30 / 100;
					int yr = y_r(1080) / SSREX * 60 / 100;

					DrawBox_2D(0, 0, y_r(1920) / SSREX, y_r(1080) / SSREX, Black, TRUE);
					DrawOval(y_r(1920) / SSREX / 2, y_r(1080) / SSREX / 2, xr, yr, White, TRUE);

					int r = 0, c = 0, p = 2;

					p = 1;
					for (r = 0; r < 255; r += p) {
						c = 255 - int(std::powf(float(255 - r) / 255.f, 1.5f)*255.f);

						DrawOval(y_r(1920) / SSREX / 2, y_r(1080) / SSREX / 2, xr - r / p, yr - r / p, GetColor(c, c, c), FALSE, 2);
					}
				}
			}
			{
				SSRColorScreen = GraphHandle::Make(y_r(1920) / SSREX, y_r(1080) / SSREX, false);
				SSRNormalScreen = GraphHandle::Make(y_r(1920) / SSREX, y_r(1080) / SSREX, false);			// �@��
				{
					// �[�x��`�悷��e�N�X�`���̍쐬( 2�`�����l�����������_32�r�b�g�e�N�X�`�� )
					auto prevMip = GetCreateDrawValidGraphChannelNum();
					auto prevFloatType = GetDrawValidFloatTypeGraphCreateFlag();
					auto prevBit = GetCreateGraphChannelBitDepth();
					SetCreateDrawValidGraphChannelNum(2);
					SetDrawValidFloatTypeGraphCreateFlag(TRUE);
					SetCreateGraphChannelBitDepth(32);
					SSRDepthScreen = GraphHandle::Make(y_r(1920) / SSREX, y_r(1080) / SSREX, false);
					SetCreateDrawValidGraphChannelNum(prevMip);
					SetDrawValidFloatTypeGraphCreateFlag(prevFloatType);
					SetCreateGraphChannelBitDepth(prevBit);
				}
			}
			m_SSRScreenVertex.SetScreenVertex(y_r(1920) / SSREX, y_r(1080) / SSREX);	// ���_�f�[�^�̏���
			m_SSR.Init("shader/VS_SSR.vso", "shader/PS_SSR.pso");					// �����Y
		}
		void Dispose_Sub() noexcept override {
			SSRScreen.Dispose();
			SSRScreen0.Dispose();
			SSRScreen1.Dispose();
			SSRScreen2.Dispose();
			bkScreen2.Dispose();
			SSRColorScreen.Dispose();
			SSRNormalScreen.Dispose();
			SSRDepthScreen.Dispose();
			m_SSR.Dispose();
		}
		bool IsActive_Sub() noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::SSR);
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph) noexcept override {
			auto* DrawParts = DXDraw::Instance();
			auto* PostPassParts = PostPassEffect::Instance();

			GraphFilterBlt(ColorGraph->get(), SSRColorScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, SSREX);
			GraphFilterBlt(m_NormalScreenPtr->get(), SSRNormalScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, SSREX);
			GraphFilterBlt(m_DepthScreenPtr->get(), SSRDepthScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, SSREX);
			SSRScreen.SetDraw_Screen();
			{
#ifdef DEBUG
				if (CheckHitKeyWithCheck(KEY_INPUT_1) != 0) {
					RayInterval = std::max(RayInterval - 1, 0);
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_2) != 0) {
					RayInterval += 1;
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_5) != 0) {
					DepthThreshold = std::max(DepthThreshold - 0.05f, 0.f);
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_6) != 0) {
					DepthThreshold += 0.05f;
				}

				if (CheckHitKeyWithCheck(KEY_INPUT_NUMPAD1) != 0) {
					SP = 1;
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_NUMPAD2) != 0) {
					SP = 2;
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_NUMPAD3) != 0) {
					SP = 3;
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_NUMPAD4) != 0) {
					SP = 4;
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_NUMPAD5) != 0) {
					SP = 5;
				}
				if (CheckHitKeyWithCheck(KEY_INPUT_NUMPAD6) != 0) {
					SP = 6;
				}

				DebugClass::Create();
				printfDx("RayInterval   :%d\n", RayInterval);
				printfDx("DepthThreshold:%f\n", DepthThreshold);
#endif // DEBUG
				SetUseTextureToShader(0, SSRColorScreen.get());	//�g�p����e�N�X�`�����Z�b�g
				SetUseTextureToShader(1, SSRNormalScreen.get());
				SetUseTextureToShader(2, SSRDepthScreen.get());
				SetUseTextureToShader(3, DrawParts->GetCubeMapTex().get());
				auto pos = DrawParts->GetMainCamera().GetCamPos();
				m_SSR.SetPixelParam(3, (float)RayInterval, SSRScale, std::tan(PostPassParts->Get_fov() / 2.f), DepthThreshold);
				{
					m_SSR.Draw(m_SSRScreenVertex);
				}
				SetUseTextureToShader(0, -1);
				SetUseTextureToShader(1, -1);
				SetUseTextureToShader(2, -1);
				SetUseTextureToShader(3, -1);
			}
			SSRScreen0.SetDraw_Screen();
			{
				int i = SP;
				for (int x = -i;x <= i;x++) {
					for (int y = -i;y <= i;y++) {
						SSRScreen.DrawGraph(x, y, true);
					}
				}
			}
			//GraphBlend(SSRScreen0.get(), SSRScreen.get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
			//		   DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_BLEND_A);

			GraphFilterBlt(SSRScreen0.get(), SSRScreen1.get(), DX_GRAPH_FILTER_DOWN_SCALE, SSREX2 / SSREX);
			SSRScreen2.SetDraw_Screen();
			{
				TargetGraph->DrawGraph(0, 0, true);
				SSRScreen1.DrawExtendGraph(0, 0, y_r(1920), y_r(1080), true);
				//DrawBox_2D(0, 0, 1920, 1080, Red, TRUE);
			}
			GraphBlend(SSRScreen2.get(), bkScreen2.get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
					   DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_BLEND_R);
			TargetGraph->SetDraw_Screen(false);
			{
				SSRScreen2.DrawGraph(0, 0, true);

				//SSRScreen0.DrawExtendGraph(0, 0, y_r(1920) / 2, y_r(1080) / 2, true);
			}
		}
		void Update_Sub() noexcept override {
			m_SSR.SetPixelCameraMatrix(4);
		}
	};
	class PostPassDoF : public PostPassBase {
	private:
		GraphHandle			DoFScreen;		//�`��X�N���[��
		ShaderUseClass::ScreenVertex	m_ScreenVertex;					// ���_�f�[�^
		ShaderUseClass		m_DoF;			// �V�F�[�_�[
	public:
		void Load_Sub() noexcept override {
			DoFScreen = GraphHandle::Make(y_r(1920), y_r(1080), true);							//�`��X�N���[��
			m_ScreenVertex.SetScreenVertex(y_r(1920), y_r(1080));							// ���_�f�[�^�̏���
			m_DoF.Init("shader/VS_DoF.vso", "shader/PS_DoF.pso");					// DoF
		}
		void Dispose_Sub() noexcept override {
			DoFScreen.Dispose();
			m_DoF.Dispose();
		}
		bool IsActive_Sub() noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::DoF);
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph) noexcept override {
			auto* PostPassParts = PostPassEffect::Instance();
			GraphFilterBlt(TargetGraph->get(), DoFScreen.get(), DX_GRAPH_FILTER_GAUSS, 16, 2000);
			TargetGraph->SetDraw_Screen();
			{
				SetUseTextureToShader(0, ColorGraph->get());	//�g�p����e�N�X�`�����Z�b�g
				SetUseTextureToShader(1, DoFScreen.get());
				SetUseTextureToShader(2, m_DepthScreenPtr->get());
				m_DoF.SetPixelDispSize(y_r(1920), y_r(1080));
				m_DoF.SetPixelParam(3, PostPassParts->Get_near_DoF(), PostPassParts->Get_far_DoF(), PostPassParts->Get_near_DoFMax(), PostPassParts->Get_far_DoFMin());
				{
					m_DoF.Draw(m_ScreenVertex);
				}
				SetUseTextureToShader(0, -1);
				SetUseTextureToShader(1, -1);
				SetUseTextureToShader(2, -1);
			}
		}
	};
	class PostPassBloom : public PostPassBase {
	private:
		GraphHandle GaussScreen_;	//�`��X�N���[��
		GraphHandle	BufScreen;
	public:
		void Load_Sub() noexcept override {
			GaussScreen_ = GraphHandle::Make(y_r(1920) / EXTEND, y_r(1080) / EXTEND, true);
			BufScreen = GraphHandle::Make(y_r(1920), y_r(1080), true);							//�`��X�N���[��
		}
		void Dispose_Sub() noexcept override {
			GaussScreen_.Dispose();
			BufScreen.Dispose();
		}
		bool IsActive_Sub() noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::bloom);
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*) noexcept override {
			GraphFilterBlt(TargetGraph->get(), BufScreen.get(), DX_GRAPH_FILTER_TWO_COLOR, 250, Black, 255, Gray50, 255);
			GraphFilterBlt(BufScreen.get(), GaussScreen_.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			GraphFilter(GaussScreen_.get(), DX_GRAPH_FILTER_GAUSS, 16, 1000);
			TargetGraph->SetDraw_Screen(false);
			{
				SetDrawMode(DX_DRAWMODE_BILINEAR);
				SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
				GaussScreen_.DrawExtendGraph(0, 0, y_r(1920), y_r(1080), true);
				GaussScreen_.DrawExtendGraph(0, 0, y_r(1920), y_r(1080), true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
		}
	};
	class PostPassAberration : public PostPassBase {
	private:
		std::array<GraphHandle, 3>		BufScreen;
	public:
		void Load_Sub() noexcept override {
			for (auto&buf : BufScreen) {
				buf = GraphHandle::Make(y_r(1920), y_r(1080), true);							//�`��X�N���[��
			}
		}
		void Dispose_Sub() noexcept override {
			for (auto&buf : BufScreen) {
				buf.Dispose();
			}
		}
		bool IsActive_Sub() noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::aberration);
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*) noexcept override {
			auto* DrawParts = DXDraw::Instance();
			BufScreen[0].SetDraw_Screen(true);
			DrawBox_2D(0, 0, y_r(1920), y_r(1080), Black, TRUE);
			BufScreen[1].SetDraw_Screen(true);
			DrawBox_2D(0, 0, y_r(1920), y_r(1080), Black, TRUE);
			BufScreen[2].SetDraw_Screen(true);
			DrawBox_2D(0, 0, y_r(1920), y_r(1080), Black, TRUE);
			GraphBlend(BufScreen[0].get(), TargetGraph->get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
					   DX_RGBA_SELECT_BLEND_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_SRC_A);
			GraphBlend(BufScreen[1].get(), TargetGraph->get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
					   DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_BLEND_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_SRC_A);
			GraphBlend(BufScreen[2].get(), TargetGraph->get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
					   DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_BLEND_B, DX_RGBA_SELECT_SRC_A);
			//output_high = 255;
			TargetGraph->SetDraw_Screen(true);
			{
				DrawBox_2D(0, 0, y_r(1920), y_r(1080), Black, TRUE);
				SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
				BufScreen[0].DrawRotaGraph(y_r(1920) / 2, y_r(1080) / 2, 1.f + 0.005f*DrawParts->GetAberrationPower(), 0.f, true);
				BufScreen[1].DrawRotaGraph(y_r(1920) / 2, y_r(1080) / 2, 1.f, 0.f, true);
				BufScreen[2].DrawRotaGraph(y_r(1920) / 2, y_r(1080) / 2, 1.f - 0.005f*DrawParts->GetAberrationPower(), 0.f, true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
		}
	};
	class PostPassMotionBlur : public PostPassBase {
	private:
		class BlurScreen {
			static const int MAX = 3;
			GraphHandle m_screen[MAX];
			int m_current{0};
			int m_alpha{0};
			int m_screenWidth{0}, m_screenHeight{0};
			int m_offsetX1{0}, m_offsetX2{0}, m_offsetY1{0}, offsetY2{0};
			int m_notBlendDraw{0};
		public:
			void Init(int t_alpha, int t_offsetX1, int t_offsetY1, int t_offsetX2, int t_offsetY2) {
				for (int i = 0; i < MAX; ++i) {
					m_screen[i] = GraphHandle::Make(y_r(1920), y_r(1080));
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
				for (int i = 0; i < MAX; ++i) {
					m_screen[i].Dispose();
				}
			}
		public:
			auto* PostRenderBlurScreen(std::function<void()> doing) {
				auto next = (m_current != 0) ? (m_current - 1) : MAX - 1;
				m_screen[m_current].SetDraw_Screen();
				{
					doing();
					if (m_notBlendDraw++ > MAX) {
						int drawMode = GetDrawMode();
						SetDrawMode(DX_DRAWMODE_BILINEAR);
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_alpha);
						m_screen[next].DrawExtendGraph(m_offsetX1, m_offsetY1, y_r(1920) + m_offsetX2, y_r(1080) + offsetY2, false);
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
		void Load_Sub() noexcept override {
			m_BlurScreen.Init(96, -6, -6, 6, 6);
		}
		void Dispose_Sub() noexcept override {
			m_BlurScreen.Release();
		}
		bool IsActive_Sub() noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::MotionBlur);
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*) noexcept override {
			GraphHandle* buf = m_BlurScreen.PostRenderBlurScreen([&]() {
				TargetGraph->DrawGraph(0, 0, false);
																 });
			TargetGraph->SetDraw_Screen(false);
			{
				buf->DrawGraph(0, 0, false);
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
			AberrationScreen = GraphHandle::Make(y_r(1920) / EXTEND, y_r(1080) / EXTEND, true);	//�`��X�N���[��
			{
				bkScreen2 = GraphHandle::Make(y_r(1920), y_r(1080), false);							//�ӂ���
				bkScreen2.SetDraw_Screen(true);
				{
					int xr = y_r(1920) * 60 / 100;
					int yr = y_r(1080) * 70 / 100;

					DrawBox_2D(0, 0, y_r(1920), y_r(1080), Black, TRUE);
					DrawOval(y_r(1920) / 2, y_r(1080) / 2, xr, yr, White, TRUE);

					int r = 0, c = 0, p = 2;

					p = 1;
					for (r = 0; r < 255; r += p) {
						c = 255 - int(std::powf(float(255 - r) / 255.f, 1.5f)*255.f);

						DrawOval(y_r(1920) / 2, y_r(1080) / 2, xr - r / p, yr - r / p, GetColor(c, c, c), FALSE, 2);
					}
				}
			}
			BufScreen = GraphHandle::Make(y_r(1920), y_r(1080), true);							//�`��X�N���[��
		}
		~PostPassCornerBlur(void) {
			AberrationScreen.Dispose();
			bkScreen2.Dispose();
			BufScreen.Dispose();
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*) noexcept override {
			auto* OptionParts = OPTION::Instance();
			//���ʕ`��
			if (OptionParts->GetParamBoolean(EnumSaveParam::aberration)) {
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
					AberrationScreen.DrawExtendGraph(0, 0, y_r(1920), y_r(1080), false);
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
			{
				bkScreen = GraphHandle::Make(y_r(1920), y_r(1080), false);							//�ӂ���
				bkScreen.SetDraw_Screen(true);
				{
					int y = 0, c = 0, p = 2;
					DrawBox_2D(0, 0, y_r(1920), y_r(1080), White, TRUE);

					p = 1;
					for (y = 0; y < 255; y += p) {
						c = 255 - int(std::powf(float(255 - y) / 255.f, 1.5f)*64.f);
						DrawLine_2D(0, y / p, y_r(1920), y / p, GetColor(c, c, c));
					}
					p = 2;
					for (y = 0; y < 255; y += p) {
						c = 255 - int(std::powf(float(255 - y) / 255.f, 1.5f)*128.f);
						DrawLine_2D(0, y_r(1080) - y / p, y_r(1920), y_r(1080) - y / p, GetColor(c, c, c));
					}
				}
			}
			BufScreen = GraphHandle::Make(y_r(1920), y_r(1080), true);							//�`��X�N���[��
		}
		~PostPassVignette(void) {
			BufScreen.Dispose();
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*) noexcept override {
			BufScreen.SetDraw_Screen();
			{
				TargetGraph->DrawGraph(0, 0, true);
			}
			GraphBlend(BufScreen.get(), bkScreen.get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
					   DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_BLEND_R);

			TargetGraph->SetDraw_Screen();
			{
				DrawBox_2D(0, 0, y_r(1920), y_r(1080), Black, TRUE);
				BufScreen.DrawGraph(0, 0, true);
			}
		}
	};

	class PostPassDistortion : public PostPassBase {
	private:
		GraphHandle	BufScreen;
	public:
		PostPassDistortion(void) {
			BufScreen = GraphHandle::Make(y_r(1920), y_r(1080), true);							//�`��X�N���[��
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
		) {
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
			for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; i++, Angle += DX_PI_F * 2.0f / CIRCLE_ANGLE_VERTEX_NUM) {
				AngleSinTable[i] = (float)sin((double)Angle);
				AngleCosTable[i] = (float)cos((double)Angle);
			}

			// �����̐���オ���Ă���悤�Ɍ�����ӏ��Ŏg�p���� Cos �e�[�u�����쐬����
			Angle = 0.0f;
			for (i = 0; i < CIRCLE_RADIUS_VERTEX_NUM; i++, Angle += (DX_PI_F / 2.0f) / (CIRCLE_RADIUS_VERTEX_NUM - 1)) {
				InCircleCosTable[i] = (float)cos((double)Angle);
			}

			// �|���S�����_�C���f�b�N�X�̏���
			Ind = Index;
			for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; i++) {
				for (WORD j = 0; j < CIRCLE_RADIUS_VERTEX_NUM - 1; j++, Ind += 6) {
					Ind[0] = (WORD)(i * CIRCLE_RADIUS_VERTEX_NUM + j);
					Ind[1] = Ind[0] + 1;
					if (i == CIRCLE_ANGLE_VERTEX_NUM - 1) {
						Ind[2] = j;
						Ind[3] = j + 1;
					}
					else {
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
			for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; i++) {
				// �g�p���� Sin, Cos �̒l���Z�b�g
				Sin = AngleSinTable[i];
				Cos = AngleCosTable[i];

				for (int j = 0; j < CIRCLE_RADIUS_VERTEX_NUM; j++, Vert++) {
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
					Vert->u = (Vert->pos.x + AbsorptionMoveX) / y_r(1920);
					Vert->v = (Vert->pos.y + AbsorptionMoveY) / y_r(1080);

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
			for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; i++) {
				// �g�p���� Sin, Cos �̒l���Z�b�g
				Sin = AngleSinTable[i];
				Cos = AngleCosTable[i];

				for (int j = 0; j < CIRCLE_RADIUS_VERTEX_NUM; j++, Vert++) {
					// �~�̒��S�܂ł̋������Z�o
					CenterDistance = InCircleCosTable[j] * Radius;

					// �摜���W���_�ł̉~�̒��S�܂ł̋������Z�o
					GraphCenterDistance = ((CIRCLE_RADIUS_VERTEX_NUM - 1) - j) * (Absorption + Radius) / (CIRCLE_RADIUS_VERTEX_NUM - 1);

					// �X�N���[�����W�̌���
					Vert->pos.x = Cos * CenterDistance + CenterX;
					Vert->pos.y = Sin * CenterDistance + CenterY;
					Vert->pos.z = 0.0f;

					// �e�N�X�`�����W�̃Z�b�g
					Vert->u = (Cos * GraphCenterDistance + CenterX) / y_r(1920);
					Vert->v = (Sin * GraphCenterDistance + CenterY) / y_r(1080);

					// ���̑��̃p�����[�^���Z�b�g
					Vert->rhw = 1.0f;
					Vert->dif = DiffuseColor;
				}
			}

			// ���S�̐���オ���Č����镔����`��
			DrawPrimitiveIndexed2D(Vertex, sizeof(Vertex) / sizeof(VERTEX2D), Index, sizeof(Index) / sizeof(WORD), DX_PRIMTYPE_TRIANGLELIST, ScreenHandle.get(), FALSE);
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*) noexcept override {
			BufScreen.SetDraw_Screen();
			{
				TargetGraph->DrawGraph(0, 0, true);
			}

			TargetGraph->SetDraw_Screen();
			{
				// ��ʂ�c�܂��ĕ`��
				DrawCircleScreen(
					y_r(1920) / 2, y_r(1080) / 2,	// ���S���W
					(float)(y_r(1920) * 2 / 3),	// �~�̃T�C�Y
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
		void Load_Sub() noexcept override {
			m_ScreenVertex.SetScreenVertex(y_r(1920), y_r(1080));							// ���_�f�[�^�̏���
			m_FXAA.Init("shader/FXAA_VS.vso", "shader/FXAA_PS.pso");					// FXAA
		}
		void Dispose_Sub() noexcept override {
			m_FXAA.Dispose();
		}
		bool IsActive_Sub() noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::AA);
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph) noexcept override {
			TargetGraph->SetDraw_Screen();
			{
				SetUseTextureToShader(0, ColorGraph->get());	//�g�p����e�N�X�`�����Z�b�g
				m_FXAA.SetPixelDispSize(y_r(1920), y_r(1080));
				{
					m_FXAA.Draw(m_ScreenVertex);
				}
				SetUseTextureToShader(0, -1);
			}
		}
	};
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	const PostPassEffect* SingletonBase<PostPassEffect>::m_Singleton = nullptr;
	//
	PostPassEffect::PostPassEffect(void) {
		FarScreen_ = GraphHandle::Make(y_r(1920), y_r(1080), false);		//�`��X�N���[��
		NearScreen_ = GraphHandle::Make(y_r(1920), y_r(1080), true);		//�`��X�N���[��
		MAIN_Screen = GraphHandle::Make(y_r(1920), y_r(1080), false);		//�ŏI�`��p
		//G�o�b�t�@
		LoadGBuffer();
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
			P->Init(&NormalScreen, &DepthScreen);
		}
	}
	PostPassEffect::~PostPassEffect(void) noexcept {
		//G�o�b�t�@
		DisposeGBuffer();
		//�|�X�g�G�t�F�N�g
		for (auto& P : m_PostPass) {
			P.reset();
		}
		m_PostPass.clear();
	}
	//
	void PostPassEffect::Update() noexcept {
		for (auto& P : m_PostPass) {
			P->UpdateActive();
		}
	}
	void PostPassEffect::DrawDoF(std::function<void()> sky_doing, std::function<void()> doing, std::function<void()> doingFront, const Camera3DInfo& cams) {
		auto* OptionParts = OPTION::Instance();
		fov = cams.GetCamFov();		//fov���L�����Ă���
		//�S�Ẳ�ʂ�������
		{
			NormalScreen.SetDraw_Screen();//���Z�b�g�ւ��
			DepthScreen.SetDraw_Screen();//���Z�b�g�ւ��
			FarScreen_.SetDraw_Screen();//���Z�b�g�ւ��
			NearScreen_.SetDraw_Screen();//���Z�b�g�ւ��
		}
		//��
		DrawGBuffer(&FarScreen_, 1000.0f, 50000.0f, [&]() { sky_doing(); }, cams);
		//������
		DrawGBuffer(&FarScreen_, cams.GetCamFar() - 10.f, 1000000.f, [&]() {
			doing();
			doingFront();
			}, cams);
		//�������̋���Ȃڂ���
		if (OptionParts->GetParamBoolean(EnumSaveParam::DoF)) {
			GraphFilter(FarScreen_.get(), DX_GRAPH_FILTER_GAUSS, 16, 200);
		}
		//����
		DrawGBuffer(&NearScreen_, cams.GetCamNear(), cams.GetCamFar(), [&]() {
			FarScreen_.DrawGraph(0, 0, false);
			Effekseer_Sync3DSetting();
			doing();
			DrawEffekseer3D();
			doingFront();
			}, cams);
		//����
		DrawGBuffer(&NearScreen_, 0.1f, 0.1f + cams.GetCamNear(), [&]() {
			Effekseer_Sync3DSetting();
			doing();
			DrawEffekseer3D();
			doingFront();
			}, cams);

		//�|�X�g�p�X�G�t�F�N�g�̃A�b�v�f�[�g
		MAIN_Screen.SetDraw_Screen(cams);
		for (auto& P : m_PostPass) {
			P->Update();
		}
	}
	void PostPassEffect::Draw() {
		//�F���␳
		{
			int output_low = 0;
			int output_high = 255;
			GraphFilter(NearScreen_.get(), DX_GRAPH_FILTER_LEVEL, InColorPerMin, InColorPerMax, int(InColorGamma * 100), output_low, output_high);
		}
		//�|�X�g�p�X�G�t�F�N�g��buf�ɕ`��
		for (auto& P : m_PostPass) {
			ColorScreen.SetDraw_Screen();
			{
				NearScreen_.DrawGraph(0, 0, false);
			}
			P->SetEffect(&NearScreen_, &ColorScreen);
		}
		//�|�X�g�p�X�G�t�F�N�g�̌��ʕ`��
		MAIN_Screen.SetDraw_Screen();
		{
			SetDrawBright(r_brt, g_brt, b_brt);
			NearScreen_.DrawGraph(0, 0, false);
			SetDrawBright(255, 255, 255);

			//NormalScreen.DrawExtendGraph(0, 0, 1920, 1080, false);
			//DepthScreen.DrawExtendGraph(0, 0, 1920, 1080, false);
		}
	}
	//
	void PostPassEffect::DrawGBuffer(GraphHandle* TargetDraw, float near_len, float far_len, std::function<void()> done, const Camera3DInfo& cams) {
		// �J���[�o�b�t�@��`��Ώ�0�ɁA�@���o�b�t�@��`��Ώ�1�ɐݒ�
		SetRenderTargetToShader(0, TargetDraw->get());
		SetRenderTargetToShader(1, NormalScreen.get());
		SetRenderTargetToShader(2, DepthScreen.get());
		SetCameraNearFar(near_len, far_len);
		SetupCamera_Perspective(cams.GetCamFov());
		SetCameraPositionAndTargetAndUpVec(cams.GetCamPos().get(), cams.GetCamVec().get(), cams.GetCamUp().get());
		{
			done();
		}
		SetRenderTargetToShader(0, -1);
		SetRenderTargetToShader(1, -1);
		SetRenderTargetToShader(2, -1);
	}
	void PostPassEffect::LoadGBuffer() noexcept {
		ColorScreen = GraphHandle::Make(y_r(1920), y_r(1080), false);
		NormalScreen = GraphHandle::Make(y_r(1920), y_r(1080), false);	// �@��G�o�b�t�@
		{
			// �[�x��`�悷��e�N�X�`��G�o�b�t�@�̍쐬( 2�`�����l�����������_32�r�b�g�e�N�X�`�� )
			auto prevMip = GetCreateDrawValidGraphChannelNum();
			auto prevFloatType = GetDrawValidFloatTypeGraphCreateFlag();
			auto prevBit = GetCreateGraphChannelBitDepth();
			SetCreateDrawValidGraphChannelNum(2);
			SetDrawValidFloatTypeGraphCreateFlag(TRUE);
			SetCreateGraphChannelBitDepth(32);
			DepthScreen = GraphHandle::Make(y_r(1920), y_r(1080), false);
			SetCreateDrawValidGraphChannelNum(prevMip);
			SetDrawValidFloatTypeGraphCreateFlag(prevFloatType);
			SetCreateGraphChannelBitDepth(prevBit);
		}
	}
	void PostPassEffect::DisposeGBuffer() noexcept {
		ColorScreen.Dispose();
		NormalScreen.Dispose();
		DepthScreen.Dispose();
	}

};
