#include "DXLib_ref_100_PostPass.hpp"

namespace DXLibRef {
	//--------------------------------------------------------------------------------------------------
	// �|�X�g�v���Z�X�G�t�F�N�g
	//--------------------------------------------------------------------------------------------------
	//�p���N���X
	class PostPassSSAO : public PostPassBase {
		ShaderUseClass::ScreenVertex	m_ScreenVertex;				// ���_�f�[�^
		ShaderUseClass		m_ShaderSSAO;										// �V�F�[�_�[
		ShaderUseClass		m_ShaderBlur;										// �V�F�[�_�[
		GraphHandle SSRScreen;		//�`��X�N���[��
		GraphHandle SSRScreen2;		//�`��X�N���[��
		float INTENSITY = 1.0f;
		float SCALE = 0.f;
		float BIAS = 0.33f;
		float SAMPLE_RAD = 4.5f;
		float MAX_DISTANCE = 1500.0f;
	public:
		void Load_Sub(void) noexcept override {
			int xsize = ScreenWidth;
			int ysize = ScreenHeight;
			m_ScreenVertex.SetScreenVertex(xsize, ysize);
			m_ShaderSSAO.Init("CommonData/shader/VS_SSAO.vso", "CommonData/shader/PS_SSAO.pso");
			m_ShaderBlur.Init("CommonData/shader/VS_SSAO.vso", "CommonData/shader/PS_BilateralBlur.pso");
			auto Prev = GetCreateDrawValidGraphZBufferBitDepth();
			SetCreateDrawValidGraphZBufferBitDepth(24);
			SSRScreen = GraphHandle::Make(xsize, ysize, true);
			SSRScreen2 = GraphHandle::Make(xsize, ysize, true);
			SetCreateDrawValidGraphZBufferBitDepth(Prev);
		}
		void Dispose_Sub(void) noexcept override {
			SSRScreen.Dispose();
			SSRScreen2.Dispose();
			m_ShaderSSAO.Dispose();
			m_ShaderBlur.Dispose();
		}
		bool IsActive_Sub(void) noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::SSAO);
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph, GraphHandle* NormalPtr, GraphHandle* DepthPtr) noexcept override {
			auto* DrawParts = DXDraw::Instance();
#if defined(DEBUG) && FALSE
			switch (GetInputChar(TRUE)) {
			case 'r': INTENSITY += 1.f; break;
			case 'f': INTENSITY -= 1.f; break;
			case 't': SCALE += 0.1f; break;
			case 'g': SCALE -= 0.1f; break;
			case 'y': BIAS += 0.1f; break;
			case 'h': BIAS -= 0.1f; break;
			case 'u': SAMPLE_RAD += 1.f; break;
			case 'j': SAMPLE_RAD -= 1.f; break;
			case 'i': MAX_DISTANCE += 100.f; break;
			case 'k': MAX_DISTANCE -= 100.f; break;
			default:break;
			}
			printfDx("INTENSITY    %f\n", INTENSITY);
			printfDx("SCALE        %f\n", SCALE);
			printfDx("BIAS         %f\n", BIAS);
			printfDx("SAMPLE_RAD   %f\n", SAMPLE_RAD);
			printfDx("MAX_DISTANCE %f\n", MAX_DISTANCE);
#endif // DEBUG
			//SSR�V�F�[�_�[��K�p
			SSRScreen2.SetDraw_Screen(false);
			{
				SetUseTextureToShader(0, ColorGraph->get());
				SetUseTextureToShader(1, NormalPtr->get());
				SetUseTextureToShader(2, DepthPtr->get());
				m_ShaderSSAO.SetPixelDispSize(ScreenWidth, ScreenHeight);
				m_ShaderSSAO.SetPixelParam(3, 0.0f, Scale3DRate, std::tan(DrawParts->GetMainCamera().GetCamFov() / 2.f), INTENSITY);
				m_ShaderSSAO.SetPixelParam(4, SCALE, BIAS, SAMPLE_RAD, MAX_DISTANCE);

				m_ShaderSSAO.Draw(m_ScreenVertex);

				SetUseTextureToShader(0, InvalidID);
				SetUseTextureToShader(1, InvalidID);
				SetUseTextureToShader(2, InvalidID);
			}
			//SSR�V�F�[�_�[�ɂڂ���������
			SSRScreen.SetDraw_Screen(false);
			{
				SetUseTextureToShader(0, SSRScreen2.get());	//�g�p����e�N�X�`�����Z�b�g

				m_ShaderBlur.SetPixelDispSize(ScreenWidth, ScreenHeight);
				m_ShaderBlur.Draw(m_ScreenVertex);

				SetUseTextureToShader(0, InvalidID);
			}
			//
			TargetGraph->SetDraw_Screen(false);
			{
				TargetGraph->DrawGraph(0, 0, true);
				SetDrawBlendMode(DX_BLENDMODE_MULA, 255);
				SSRScreen.DrawExtendGraph(0, 0, ScreenWidth, ScreenHeight, true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				//SSRScreen2.DrawExtendGraph(0, 0, ScreenWidth, ScreenHeight, true);
			}
		}
	};
	class PostPassSSR : public PostPassBase {
	private:
		GraphHandle SSRColorScreen;	//���̂܂܂�G�o�b�t�@
		GraphHandle SSRNormalScreen;	//�@����G�o�b�t�@
		GraphHandle	SSRDepthScreen;	//�[�x��G�o�b�t�@
		GraphHandle SSRScreen;		//�`��X�N���[��
		GraphHandle bkScreen2;		//�u�����h
		ShaderUseClass::ScreenVertex	m_SSRScreenVertex;				// ���_�f�[�^
		ShaderUseClass		m_Shader;										// �V�F�[�_�[

		int RayInterval = 200;//���C�̕����Ԋu
		float DepthThreshold = 17.f;

		static const int EXTEND = 4;
	public:
		PostPassSSR(void) noexcept {}
		PostPassSSR(const PostPassSSR&) = delete;
		PostPassSSR(PostPassSSR&& o) = delete;
		PostPassSSR& operator=(const PostPassSSR&) = delete;
		PostPassSSR& operator=(PostPassSSR&& o) = delete;

		virtual ~PostPassSSR(void) noexcept {}
	public:
		void Load_Sub(void) noexcept override {
			int xsize = ScreenWidth / EXTEND;
			int ysize = ScreenHeight / EXTEND;
			SSRScreen = GraphHandle::Make(xsize, ysize, true);
			{
				bkScreen2 = GraphHandle::Make(xsize, ysize, false);
				bkScreen2.SetDraw_Screen(false);
				FillGraph(bkScreen2.get(), 0, 0, 0);
				{
					int xr = xsize * 30 / 100;
					int yr = ysize * 60 / 100;

					DrawOval(xsize / 2, ysize / 2, xr, yr, White, TRUE);

					int r = 0, c = 0, p = 2;

					p = 1;
					for (r = 0; r < 255; r += p) {
						c = 255 - int(std::powf(float(255 - r) / 255.f, 1.5f) * 255.f);

						DrawOval(xsize / 2, ysize / 2, xr - r / p, yr - r / p, GetColor(c, c, c), FALSE, 2);
					}
				}
			}
			SSRColorScreen = GraphHandle::Make(xsize, ysize, false);
			SSRNormalScreen = GraphHandle::Make(xsize, ysize, false);
			SSRDepthScreen = GraphHandle::MakeDepth(xsize, ysize);
			m_SSRScreenVertex.SetScreenVertex(xsize, ysize);
			m_Shader.Init("CommonData/shader/VS_SSR.vso", "CommonData/shader/PS_SSR.pso");
		}
		void Dispose_Sub(void) noexcept override {
			SSRScreen.Dispose();
			bkScreen2.Dispose();
			SSRColorScreen.Dispose();
			SSRNormalScreen.Dispose();
			SSRDepthScreen.Dispose();
			m_Shader.Dispose();
		}
		bool IsActive_Sub(void) noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamInt(EnumSaveParam::Reflection) > 0;
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph, GraphHandle* NormalPtr, GraphHandle* DepthPtr) noexcept override {
			auto* OptionParts = OPTION::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* PostPassParts = PostPassEffect::Instance();

			GraphFilterBlt(ColorGraph->get(), SSRColorScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			GraphFilterBlt(NormalPtr->get(), SSRNormalScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			GraphFilterBlt(DepthPtr->get(), SSRDepthScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			SSRScreen.SetDraw_Screen();
			{
#if defined(DEBUG) && FALSE
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

				printfDx("RayInterval  :%d\n", RayInterval);
				printfDx("DepthThreshold:%f\n", DepthThreshold);
#endif // DEBUG
				SetUseTextureToShader(0, SSRColorScreen.get());
				SetUseTextureToShader(1, SSRNormalScreen.get());
				SetUseTextureToShader(2, SSRDepthScreen.get());
				if (OptionParts->GetParamBoolean(EnumProjectSettingParam::CubeMap)) {
					SetUseTextureToShader(3, DrawParts->GetCubeMapTex().get());
				}
				else {
					SetUseTextureToShader(3, bkScreen2.get());
				}
				SetUseTextureToShader(4, bkScreen2.get());
				m_Shader.SetPixelParam(3, static_cast<float>(RayInterval), Scale3DRate, std::tan(DrawParts->GetMainCamera().GetCamFov() / 2.f), DepthThreshold);
				m_Shader.SetPixelCameraMatrix(4, PostPassParts->GetCamViewMat(), PostPassParts->GetCamProjectionMat());
				m_Shader.SetPixelParam(5, static_cast<float>(OptionParts->GetParamInt(EnumSaveParam::Reflection)), OptionParts->GetParamBoolean(EnumProjectSettingParam::CubeMap) ? 1.f : 0.f, 0.f, 0.f);
				{
					m_Shader.Draw(m_SSRScreenVertex);
				}
				SetUseTextureToShader(0, InvalidID);
				SetUseTextureToShader(1, InvalidID);
				SetUseTextureToShader(2, InvalidID);
				SetUseTextureToShader(3, InvalidID);
				SetUseTextureToShader(4, InvalidID);
			}
			GraphFilter(SSRScreen.get(), DX_GRAPH_FILTER_GAUSS, 8, 200);
			TargetGraph->SetDraw_Screen(false);
			{
				TargetGraph->DrawGraph(0, 0, true);
				SSRScreen.DrawExtendGraph(0, 0, ScreenWidth, ScreenHeight, true);
			}
		}
	};
	class PostPassDoF : public PostPassBase {
	private:
		GraphHandle			DoFScreen;		//�`��X�N���[��
		ShaderUseClass::ScreenVertex	m_ScreenVertex;					// ���_�f�[�^
		ShaderUseClass		m_Shader;			// �V�F�[�_�[
	public:
		PostPassDoF(void) noexcept {}
		PostPassDoF(const PostPassDoF&) = delete;
		PostPassDoF(PostPassDoF&& o) = delete;
		PostPassDoF& operator=(const PostPassDoF&) = delete;
		PostPassDoF& operator=(PostPassDoF&& o) = delete;

		virtual ~PostPassDoF(void) noexcept {}
	public:
		void Load_Sub(void) noexcept override {
			DoFScreen = GraphHandle::Make(ScreenWidth, ScreenHeight, true);
			m_ScreenVertex.SetScreenVertex(ScreenWidth, ScreenHeight);
			m_Shader.Init("CommonData/shader/VS_DoF.vso", "CommonData/shader/PS_DoF.pso");
		}
		void Dispose_Sub(void) noexcept override {
			DoFScreen.Dispose();
			m_Shader.Dispose();
		}
		bool IsActive_Sub(void) noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::DoF);
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph, GraphHandle*, GraphHandle* DepthPtr) noexcept override {
			auto* PostPassParts = PostPassEffect::Instance();
			GraphFilterBlt(TargetGraph->get(), DoFScreen.get(), DX_GRAPH_FILTER_GAUSS, 16, 2000);
			TargetGraph->SetDraw_Screen();
			{
				SetUseTextureToShader(0, ColorGraph->get());
				SetUseTextureToShader(1, DoFScreen.get());
				SetUseTextureToShader(2, DepthPtr->get());
				m_Shader.SetPixelDispSize(ScreenWidth, ScreenHeight);
				m_Shader.SetPixelParam(3, PostPassParts->Get_near_DoF(), PostPassParts->Get_far_DoF(), PostPassParts->Get_near_DoFMax(), PostPassParts->Get_far_DoFMin());
				{
					m_Shader.Draw(m_ScreenVertex);
				}
				SetUseTextureToShader(0, InvalidID);
				SetUseTextureToShader(1, InvalidID);
				SetUseTextureToShader(2, InvalidID);
			}
		}
	};
	class PostPassBloom : public PostPassBase {
	private:
		static const int EXTEND = 4;
	private:
		GraphHandle GaussScreen_;	//�`��X�N���[��
		GraphHandle	BufScreen;
	public:
		PostPassBloom(void) noexcept {}
		PostPassBloom(const PostPassBloom&) = delete;
		PostPassBloom(PostPassBloom&& o) = delete;
		PostPassBloom& operator=(const PostPassBloom&) = delete;
		PostPassBloom& operator=(PostPassBloom&& o) = delete;

		virtual ~PostPassBloom(void) noexcept {}
	public:
		void Load_Sub(void) noexcept override {
			GaussScreen_ = GraphHandle::Make(ScreenWidth / EXTEND, ScreenHeight / EXTEND, true);
			BufScreen = GraphHandle::Make(ScreenWidth, ScreenHeight, true);
		}
		void Dispose_Sub(void) noexcept override {
			GaussScreen_.Dispose();
			BufScreen.Dispose();
		}
		bool IsActive_Sub(void) noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::bloom);
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*, GraphHandle*, GraphHandle*) noexcept override {
			GraphFilterBlt(TargetGraph->get(), BufScreen.get(), DX_GRAPH_FILTER_TWO_COLOR, 250, Black, 255, Gray50, 255);
			GraphFilterBlt(BufScreen.get(), GaussScreen_.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			GraphFilter(GaussScreen_.get(), DX_GRAPH_FILTER_GAUSS, 16, 1000);
			TargetGraph->SetDraw_Screen(false);
			{
				auto Prev = GetDrawMode();
				SetDrawMode(DX_DRAWMODE_BILINEAR);
				SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
				GaussScreen_.DrawExtendGraph(0, 0, ScreenWidth, ScreenHeight, true);
				GaussScreen_.DrawExtendGraph(0, 0, ScreenWidth, ScreenHeight, true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				SetDrawMode(Prev);
			}
		}
	};
	class PostPassAberration : public PostPassBase {
	private:
		std::array<GraphHandle, 3>		BufScreen;
	public:
		PostPassAberration(void) noexcept {}
		PostPassAberration(const PostPassAberration&) = delete;
		PostPassAberration(PostPassAberration&& o) = delete;
		PostPassAberration& operator=(const PostPassAberration&) = delete;
		PostPassAberration& operator=(PostPassAberration&& o) = delete;

		virtual ~PostPassAberration(void) noexcept {}
	public:
		void Load_Sub(void) noexcept override {
			for (auto& buf : BufScreen) {
				buf = GraphHandle::Make(ScreenWidth, ScreenHeight, true);
			}
		}
		void Dispose_Sub(void) noexcept override {
			for (auto& buf : BufScreen) {
				buf.Dispose();
			}
		}
		bool IsActive_Sub(void) noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::ScreenEffect);
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*, GraphHandle*, GraphHandle*) noexcept override {
			auto* DrawParts = DXDraw::Instance();
			BufScreen[0].SetDraw_Screen(false);
			FillGraph(BufScreen[0].get(), 0, 0, 0);
			BufScreen[1].SetDraw_Screen(false);
			FillGraph(BufScreen[1].get(), 0, 0, 0);
			BufScreen[2].SetDraw_Screen(false);
			FillGraph(BufScreen[2].get(), 0, 0, 0);
			GraphBlend(BufScreen[0].get(), TargetGraph->get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_BLEND_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_SRC_A);
			GraphBlend(BufScreen[1].get(), TargetGraph->get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_BLEND_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_SRC_A);
			GraphBlend(BufScreen[2].get(), TargetGraph->get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_BLEND_B, DX_RGBA_SELECT_SRC_A);
			TargetGraph->SetDraw_Screen(false);
			{
				FillGraph(TargetGraph->get(), 0, 0, 0);
				SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
				BufScreen[0].DrawRotaGraph(ScreenWidth / 2, ScreenHeight / 2, 1.f + 0.005f * DrawParts->GetAberrationPower(), 0.f, true);
				BufScreen[1].DrawRotaGraph(ScreenWidth / 2, ScreenHeight / 2, 1.f, 0.f, true);
				BufScreen[2].DrawRotaGraph(ScreenWidth / 2, ScreenHeight / 2, 1.f - 0.005f * DrawParts->GetAberrationPower(), 0.f, true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
		}
	};
	class PostPassMotionBlur : public PostPassBase {
	private:
		class BlurScreen {
			static const int MAX = 3;
			std::array<GraphHandle, MAX> m_screen;
			int m_current{ 0 };
			int m_alpha{ 0 };
			int m_screenWidth{ 0 }, m_screenHeight{ 0 };
			int m_offsetX1{ 0 }, m_offsetX2{ 0 }, m_offsetY1{ 0 }, offsetY2{ 0 };
			int m_notBlendDraw{ 0 };
		public:
			BlurScreen(void) noexcept {}
			BlurScreen(const BlurScreen&) = delete;
			BlurScreen(BlurScreen&& o) = delete;
			BlurScreen& operator=(const BlurScreen&) = delete;
			BlurScreen& operator=(BlurScreen&& o) = delete;

			~BlurScreen(void) noexcept {}
		public:
			void Init(int t_alpha, int t_offsetX1, int t_offsetY1, int t_offsetX2, int t_offsetY2) noexcept {
				for (int i : std::views::iota(0, MAX)) {
					m_screen.at(static_cast<size_t>(i)) = GraphHandle::Make(ScreenWidth, ScreenHeight);
				}
				m_current = 0;
				m_alpha = t_alpha;
				m_offsetX1 = t_offsetX1;
				m_offsetX2 = t_offsetX2;
				m_offsetY1 = t_offsetY1;
				offsetY2 = t_offsetY2;

				m_notBlendDraw = 0;
			}
			void Release(void) noexcept {
				for (int i : std::views::iota(0, MAX)) {
					m_screen.at(static_cast<size_t>(i)).Dispose();
				}
			}
		public:
			auto* PostRenderBlurScreen(std::function<void()> doing) noexcept {
				auto next = (m_current != 0) ? (m_current - 1) : MAX - 1;
				m_screen[static_cast<size_t>(m_current)].SetDraw_Screen();
				{
					doing();
					if (++m_notBlendDraw > MAX) {
						int drawMode = GetDrawMode();
						SetDrawMode(DX_DRAWMODE_BILINEAR);
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_alpha);
						m_screen[static_cast<size_t>(next)].DrawExtendGraph(m_offsetX1, m_offsetY1, ScreenWidth + m_offsetX2, ScreenHeight + offsetY2, false);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						SetDrawMode(drawMode);
					}
				}
				auto Cur = m_current;
				m_current = next;
				return &m_screen[static_cast<size_t>(Cur)];
			}
		};
	private:
		BlurScreen				m_BlurScreen;
	public:
		PostPassMotionBlur(void) noexcept {}
		PostPassMotionBlur(const PostPassMotionBlur&) = delete;
		PostPassMotionBlur(PostPassMotionBlur&& o) = delete;
		PostPassMotionBlur& operator=(const PostPassMotionBlur&) = delete;
		PostPassMotionBlur& operator=(PostPassMotionBlur&& o) = delete;

		virtual ~PostPassMotionBlur(void) noexcept {}
	public:
		void Load_Sub(void) noexcept override {
			m_BlurScreen.Init(96, -6, -6, 6, 6);
		}
		void Dispose_Sub(void) noexcept override {
			m_BlurScreen.Release();
		}
		bool IsActive_Sub(void) noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::MotionBlur);
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*, GraphHandle*, GraphHandle*) noexcept override {
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
		static const int EXTEND = 4;
	private:
		GraphHandle AberrationScreen;	//�`��X�N���[��
		GraphHandle bkScreen2;
		GraphHandle	BufScreen;
	public:
		PostPassCornerBlur(void) noexcept {}
		PostPassCornerBlur(const PostPassCornerBlur&) = delete;
		PostPassCornerBlur(PostPassCornerBlur&& o) = delete;
		PostPassCornerBlur& operator=(const PostPassCornerBlur&) = delete;
		PostPassCornerBlur& operator=(PostPassCornerBlur&& o) = delete;

		virtual ~PostPassCornerBlur(void) noexcept {}
	public:
		void Load_Sub(void) noexcept override {
			AberrationScreen = GraphHandle::Make(ScreenWidth / EXTEND, ScreenHeight / EXTEND, true);
			{
				bkScreen2 = GraphHandle::Make(ScreenWidth, ScreenHeight, false);
				bkScreen2.SetDraw_Screen(false);
				{
					FillGraph(bkScreen2.get(), 0, 0, 0);
					int xr = ScreenWidth * 60 / 100;
					int yr = ScreenHeight * 70 / 100;

					DrawOval(ScreenWidth / 2, ScreenHeight / 2, xr, yr, White, TRUE);

					int r = 0, c = 0, p = 2;

					p = 1;
					for (r = 0; r < 255; r += p) {
						c = 255 - int(std::powf(float(255 - r) / 255.f, 1.5f) * 255.f);

						DrawOval(ScreenWidth / 2, ScreenHeight / 2, xr - r / p, yr - r / p, GetColor(c, c, c), FALSE, 2);
					}
				}
			}
			BufScreen = GraphHandle::Make(ScreenWidth, ScreenHeight, true);
		}
		void Dispose_Sub(void) noexcept override {
			AberrationScreen.Dispose();
			bkScreen2.Dispose();
			BufScreen.Dispose();
		}
		bool IsActive_Sub(void) noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::ScreenEffect);
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*, GraphHandle*, GraphHandle*) noexcept override {
			GraphFilterBlt(TargetGraph->get(), AberrationScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			GraphFilter(AberrationScreen.get(), DX_GRAPH_FILTER_GAUSS, 16, 1000);
			GraphBlendBlt(TargetGraph->get(), bkScreen2.get(), BufScreen.get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_BLEND_R);
			TargetGraph->SetDraw_Screen(false);
			{
				AberrationScreen.DrawExtendGraph(0, 0, ScreenWidth, ScreenHeight, false);
				BufScreen.DrawGraph(0, 0, true);
			}
		}
	};
	class PostPassVignette : public PostPassBase {
	private:
		GraphHandle bkScreen;
		GraphHandle	BufScreen;
	public:
		PostPassVignette(void) noexcept {}
		PostPassVignette(const PostPassVignette&) = delete;
		PostPassVignette(PostPassVignette&& o) = delete;
		PostPassVignette& operator=(const PostPassVignette&) = delete;
		PostPassVignette& operator=(PostPassVignette&& o) = delete;

		virtual ~PostPassVignette(void) noexcept {}
	public:
		void Load_Sub(void) noexcept override {
			{
				bkScreen = GraphHandle::Make(ScreenWidth, ScreenHeight, false);
				bkScreen.SetDraw_Screen(true);
				FillGraph(bkScreen.get(), 255, 255, 255);
				{
					int y = 0, c = 0, p = 2;
					p = 1;
					for (y = 0; y < 255; y += p) {
						c = 255 - int(std::powf(float(255 - y) / 255.f, 1.5f) * 64.f);
						DrawLine_2D(0, y / p, ScreenWidth, y / p, GetColor(c, c, c));
					}
					p = 2;
					for (y = 0; y < 255; y += p) {
						c = 255 - int(std::powf(float(255 - y) / 255.f, 1.5f) * 128.f);
						DrawLine_2D(0, ScreenHeight - y / p, ScreenWidth, ScreenHeight - y / p, GetColor(c, c, c));
					}
				}
			}
			BufScreen = GraphHandle::Make(ScreenWidth, ScreenHeight, true);
		}
		void Dispose_Sub(void) noexcept override {
			bkScreen.Dispose();
			BufScreen.Dispose();
		}
		bool IsActive_Sub(void) noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::ScreenEffect);
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*, GraphHandle*, GraphHandle*) noexcept override {
			GraphBlendBlt(TargetGraph->get(), bkScreen.get(), BufScreen.get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_BLEND_R);
			TargetGraph->SetDraw_Screen(false);
			{
				FillGraph(TargetGraph->get(), 0, 0, 0);
				BufScreen.DrawGraph(0, 0, true);
			}
		}
	};
	class PostPassDistortion : public PostPassBase {
	private:
		GraphHandle	BufScreen;
	public:
		PostPassDistortion(void) noexcept {}
		PostPassDistortion(const PostPassDistortion&) = delete;
		PostPassDistortion(PostPassDistortion&& o) = delete;
		PostPassDistortion& operator=(const PostPassDistortion&) = delete;
		PostPassDistortion& operator=(PostPassDistortion&& o) = delete;

		virtual ~PostPassDistortion(void) noexcept {}
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
			VERTEX2D* Vert;
			WORD* Ind;
			std::array<float, CIRCLE_ANGLE_VERTEX_NUM> AngleCosTable{};
			std::array<float, CIRCLE_ANGLE_VERTEX_NUM> AngleSinTable{};
			std::array<float, CIRCLE_RADIUS_VERTEX_NUM> InCircleCosTable{};

			// �X�^�b�N�ɐςނɂ͑傫���̂� static �z��ɂ��܂���
			static VERTEX2D Vertex[CIRCLE_RADIUS_VERTEX_NUM * CIRCLE_ANGLE_VERTEX_NUM];
			static WORD Index[CIRCLE_ANGLE_VERTEX_NUM * 6 * (CIRCLE_RADIUS_VERTEX_NUM - 1)];

			// �ŏ��ɕ��ʂɕ`��
			DrawGraph(0, 0, ScreenHandle.get(), FALSE);

			// �`��J���[���쐬���Ă���
			DiffuseColor = GetColorU8(255, 255, 255, 255);

			// �O�������p�� Sin, Cos �e�[�u�����쐬����
			Angle = 0.0f;
			for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; ++i, Angle += DX_PI_F * 2.0f / CIRCLE_ANGLE_VERTEX_NUM) {
				AngleSinTable.at(static_cast<size_t>(i)) = std::sin(Angle);
				AngleCosTable.at(static_cast<size_t>(i)) = std::cos(Angle);
			}

			// �����̐���オ���Ă���悤�Ɍ�����ӏ��Ŏg�p���� Cos �e�[�u�����쐬����
			Angle = 0.0f;
			for (i = 0; i < CIRCLE_RADIUS_VERTEX_NUM; ++i, Angle += (DX_PI_F / 2.0f) / (CIRCLE_RADIUS_VERTEX_NUM - 1)) {
				InCircleCosTable.at(static_cast<size_t>(i)) = std::cos(Angle);
			}

			// �|���S�����_�C���f�b�N�X�̏���
			Ind = Index;
			for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; ++i) {
				for (WORD j = 0; j < CIRCLE_RADIUS_VERTEX_NUM - 1; ++j, Ind += 6) {
					Ind[0] = static_cast<WORD>(i * CIRCLE_RADIUS_VERTEX_NUM + j);
					Ind[1] = static_cast<WORD>(Ind[0] + 1);
					if (i == CIRCLE_ANGLE_VERTEX_NUM - 1) {
						Ind[2] = j;
						Ind[3] = static_cast<WORD>(j + 1);
					}
					else {
						Ind[2] = static_cast<WORD>(Ind[0] + CIRCLE_RADIUS_VERTEX_NUM);
						Ind[3] = static_cast<WORD>(Ind[0] + 1 + CIRCLE_RADIUS_VERTEX_NUM);
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
			for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; ++i) {
				// �g�p���� Sin, Cos �̒l���Z�b�g
				Sin = AngleSinTable.at(static_cast<size_t>(i));
				Cos = AngleCosTable.at(static_cast<size_t>(i));

				for (int j = 0; j < CIRCLE_RADIUS_VERTEX_NUM; ++j, ++Vert) {
					// �~�̒��S�܂ł̋������Z�o
					CenterDistance = Radius;

					// ���S�Ɉ������܂�鋗�����Z�o
					AbsorptionDistance = Absorption * static_cast<float>(j) / static_cast<float>(CIRCLE_RADIUS_VERTEX_NUM - 1);

					// ���S�Ɍ������Ĉړ����鋗�����Z�o
					AbsorptionMoveX = Cos * AbsorptionDistance;
					AbsorptionMoveY = Sin * AbsorptionDistance;

					// �X�N���[�����W�̌���
					Vert->pos.x = Cos * CenterDistance + static_cast<float>(CenterX);
					Vert->pos.y = Sin * CenterDistance + static_cast<float>(CenterY);
					Vert->pos.z = 0.0f;

					// �e�N�X�`�����W�̃Z�b�g
					Vert->u = (Vert->pos.x + AbsorptionMoveX) / static_cast<float>(ScreenWidth);
					Vert->v = (Vert->pos.y + AbsorptionMoveY) / static_cast<float>(ScreenHeight);

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
			for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; ++i) {
				// �g�p���� Sin, Cos �̒l���Z�b�g
				Sin = AngleSinTable.at(static_cast<size_t>(i));
				Cos = AngleCosTable.at(static_cast<size_t>(i));

				for (int j = 0; j < CIRCLE_RADIUS_VERTEX_NUM; ++j, ++Vert) {
					// �~�̒��S�܂ł̋������Z�o
					CenterDistance = InCircleCosTable[static_cast<size_t>(j)] * Radius;

					// �摜���W���_�ł̉~�̒��S�܂ł̋������Z�o
					GraphCenterDistance = static_cast<float>((CIRCLE_RADIUS_VERTEX_NUM - 1) - j) * (Absorption + Radius) / static_cast<float>(CIRCLE_RADIUS_VERTEX_NUM - 1);

					// �X�N���[�����W�̌���
					Vert->pos.x = Cos * CenterDistance + static_cast<float>(CenterX);
					Vert->pos.y = Sin * CenterDistance + static_cast<float>(CenterY);
					Vert->pos.z = 0.0f;

					// �e�N�X�`�����W�̃Z�b�g
					Vert->u = (Cos * GraphCenterDistance + static_cast<float>(CenterX)) / static_cast<float>(ScreenWidth);
					Vert->v = (Sin * GraphCenterDistance + static_cast<float>(CenterY)) / static_cast<float>(ScreenHeight);

					// ���̑��̃p�����[�^���Z�b�g
					Vert->rhw = 1.0f;
					Vert->dif = DiffuseColor;
				}
			}

			// ���S�̐���オ���Č����镔����`��
			DrawPrimitiveIndexed2D(Vertex, sizeof(Vertex) / sizeof(VERTEX2D), Index, sizeof(Index) / sizeof(WORD), DX_PRIMTYPE_TRIANGLELIST, ScreenHandle.get(), FALSE);
		}
	public:
		void Load_Sub(void) noexcept override {
			BufScreen = GraphHandle::Make(ScreenWidth, ScreenHeight, true);
		}
		void Dispose_Sub(void) noexcept override {
			BufScreen.Dispose();
		}
		bool IsActive_Sub(void) noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::ScreenEffect) && OptionParts->GetParamBoolean(EnumProjectSettingParam::Distortion);
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*, GraphHandle*, GraphHandle*) noexcept override {
			BufScreen.SetDraw_Screen();
			{
				TargetGraph->DrawGraph(0, 0, true);
			}
			//GraphFilterBlt(BufScreen.get(), TargetGraph->get(), DX_GRAPH_FILTER_DOWN_SCALE, 1);
			TargetGraph->SetDraw_Screen();
			{
				// ��ʂ�c�܂��ĕ`��
				DrawCircleScreen(
					ScreenWidth / 2, ScreenHeight / 2,
					static_cast<float>(ScreenWidth * 2 / 3), 120.0f, BufScreen);
			}
		}
	};
	class PostPassFXAA : public PostPassBase {
	private:
		ShaderUseClass::ScreenVertex	m_ScreenVertex;
		ShaderUseClass		m_Shader;
	public:
		void Load_Sub(void) noexcept override {
			m_ScreenVertex.SetScreenVertex(ScreenWidth, ScreenHeight);
			m_Shader.Init("CommonData/shader/VS_FXAA.vso", "CommonData/shader/PS_FXAA.pso");
		}
		void Dispose_Sub(void) noexcept override {
			m_Shader.Dispose();
		}
		bool IsActive_Sub(void) noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::AA);
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph, GraphHandle*, GraphHandle*) noexcept override {
			TargetGraph->SetDraw_Screen();
			{
				SetUseTextureToShader(0, ColorGraph->get());
				m_Shader.SetPixelDispSize(ScreenWidth, ScreenHeight);
				{
					m_Shader.Draw(m_ScreenVertex);
				}
				SetUseTextureToShader(0, InvalidID);
			}
		}
	};
	class PostPassGodRay : public PostPassBase {
	private:
		GraphHandle SSRColorScreen;	//���̂܂܂�G�o�b�t�@
		GraphHandle	SSRDepthScreen;	//�[�x��G�o�b�t�@
		GraphHandle SSRScreen;		//�`��X�N���[��

		ShaderUseClass::ScreenVertex	m_ScreenVertex;					// ���_�f�[�^
		ShaderUseClass		m_Shader;			// �V�F�[�_�[
		float range = 1.f;

		static const int EXTEND = 8;
	public:
		PostPassGodRay(void) noexcept {}
		PostPassGodRay(const PostPassGodRay&) = delete;
		PostPassGodRay(PostPassGodRay&& o) = delete;
		PostPassGodRay& operator=(const PostPassGodRay&) = delete;
		PostPassGodRay& operator=(PostPassGodRay&& o) = delete;

		virtual ~PostPassGodRay(void) noexcept {}
	public:
		void Load_Sub(void) noexcept override {
			m_ScreenVertex.SetScreenVertex(ScreenWidth / EXTEND, ScreenHeight / EXTEND);
			m_Shader.Init("CommonData/shader/VS_GodRay.vso", "CommonData/shader/PS_GodRay.pso");
			SSRScreen = GraphHandle::Make(ScreenWidth / EXTEND, ScreenHeight / EXTEND, true);
			SSRDepthScreen = GraphHandle::MakeDepth(ScreenWidth / EXTEND, ScreenHeight / EXTEND);
		}
		void Dispose_Sub(void) noexcept override {
			SSRScreen.Dispose();
			SSRDepthScreen.Dispose();
			m_Shader.Dispose();
		}
		bool IsActive_Sub(void) noexcept override {
			auto* OptionParts = OPTION::Instance();
			return (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0) && OptionParts->GetParamBoolean(EnumProjectSettingParam::GodRay);
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph, GraphHandle*, GraphHandle* DepthPtr) noexcept override {
			auto* OptionParts = OPTION::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			GraphFilterBlt(DepthPtr->get(), SSRDepthScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);

			m_Shader.SetPixelCameraMatrix(4, PostPassParts->GetCamViewMat().inverse(), PostPassParts->GetCamProjectionMat().inverse());
			m_Shader.SetPixelCameraMatrix(5, DrawParts->GetShadowDraw()->GetCamViewMatrix(false), DrawParts->GetShadowDraw()->GetCamProjectionMatrix(false));
			//m_Shader.SetPixelCameraMatrix(6, DrawParts->GetShadowDraw()->GetCamViewMatrix(true), DrawParts->GetShadowDraw()->GetCamProjectionMatrix(true));
			SSRScreen.SetDraw_Screen();
			{
				SetUseTextureToShader(0, SSRDepthScreen.get());
				SetUseTextureToShader(1, DrawParts->GetShadowDraw()->GetDepthScreen().get());
				//SetUseTextureToShader(2, DrawParts->GetShadowDraw()->GetDepthFarScreen().get());
				{
					float Power = 1.f;
					switch (OptionParts->GetParamInt(EnumSaveParam::shadow)) {
					case 1:
						Power = 15.f;
						break;
					case 2:
						Power = 20.f;
						break;
					case 3:
						Power = 35.f;
						break;
					default:
						break;
					}
					m_Shader.SetPixelParam(3, Power, 0.f, std::tan(DrawParts->GetMainCamera().GetCamFov() / 2.f), 0.f);
					m_Shader.Draw(m_ScreenVertex);
				}
				SetUseTextureToShader(0, InvalidID);
				SetUseTextureToShader(1, InvalidID);
				//SetUseTextureToShader(2, InvalidID);
			}
			GraphFilter(SSRScreen.get(), DX_GRAPH_FILTER_GAUSS, 16, 300);
			TargetGraph->SetDraw_Screen();
			{
				ColorGraph->DrawGraph(0, 0, true);
				SetDrawBlendMode(DX_BLENDMODE_ADD, 255 / 2);
				SSRScreen.DrawExtendGraph(0, 0, ScreenWidth, ScreenHeight, true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
		}
	};
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	const PostPassEffect* SingletonBase<PostPassEffect>::m_Singleton = nullptr;
	//
	PostPassEffect::PostPassEffect(void) noexcept {
		auto Prev = GetCreateDrawValidGraphZBufferBitDepth();
		SetCreateDrawValidGraphZBufferBitDepth(24);
		BufferScreen = GraphHandle::Make(ScreenWidth, ScreenHeight, true);
		ColorScreen = GraphHandle::Make(ScreenWidth, ScreenHeight, false);
		SetCreateDrawValidGraphZBufferBitDepth(Prev);
		//�|�X�g�G�t�F�N�g
		m_PostPass.emplace_back(std::make_unique<PostPassBloom>());
		m_PostPass.emplace_back(std::make_unique<PostPassGodRay>());
		m_PostPass.emplace_back(std::make_unique<PostPassDoF>());
		m_PostPass.emplace_back(std::make_unique<PostPassSSR>());
		m_PostPass.emplace_back(std::make_unique<PostPassSSAO>());
		m_PostPass.emplace_back(std::make_unique<PostPassDistortion>());
		m_PostPass.emplace_back(std::make_unique<PostPassAberration>());
		m_PostPass.emplace_back(std::make_unique<PostPassMotionBlur>());
		m_PostPass.emplace_back(std::make_unique<PostPassVignette>());
		m_PostPass.emplace_back(std::make_unique<PostPassCornerBlur>());
		m_PostPass.emplace_back(std::make_unique<PostPassFXAA>());
		for (auto& P : m_PostPass) {
			P->Init();
		}
		//G�o�b�t�@
		bool ActiveGBuffer = false;
		for (auto& P : m_PostPass) {
			if (P->IsActive()) {
				ActiveGBuffer = true;
			}
		}
		m_IsActiveGBuffer = ActiveGBuffer;
		if (m_IsActiveGBuffer) {
			LoadGBuffer();
		}
	}
	PostPassEffect::~PostPassEffect(void) noexcept {
		//G�o�b�t�@
		if (!m_IsActiveGBuffer) {
			DisposeGBuffer();
		}
		//�|�X�g�G�t�F�N�g
		for (auto& P : m_PostPass) {
			P.reset();
		}
		m_PostPass.clear();
	}
	//
	void PostPassEffect::Update(void) noexcept {
		bool ActiveGBuffer = false;
		for (auto& P : m_PostPass) {
			if (P->IsActive()) {
				ActiveGBuffer = true;
			}
		}
		if (m_IsActiveGBuffer != ActiveGBuffer) {
			m_IsActiveGBuffer = ActiveGBuffer;
			if (m_IsActiveGBuffer) {
				LoadGBuffer();
			}
			else {
				DisposeGBuffer();
			}
		}
		for (auto& P : m_PostPass) {
			P->UpdateActive();
		}
	}
	void PostPassEffect::DrawDoF(std::function<void()> sky_doing, std::function<void()> doing, std::function<void()> doingFront, const Camera3DInfo& camInfo) noexcept {
		m_CamViewMat = camInfo.GetViewMatrix();
		m_CamProjectionMat = camInfo.GetProjectionMatrix();
		//�S�Ẳ�ʂ�������
		if (m_IsActiveGBuffer) {
			//���Z�b�g�ւ��
			ColorScreen.SetDraw_Screen();
			NormalScreen.SetDraw_Screen();
			DepthScreen.SetDraw_Screen();
		}
		//��
		DrawGBuffer(1000.0f, 50000.0f, sky_doing, camInfo);
		//������
		DrawGBuffer(camInfo.GetCamFar() - 10.f, 1000000.f, [&]() {
			doing();
			doingFront();
			}, camInfo);
		//����
		DrawGBuffer(camInfo.GetCamNear(), camInfo.GetCamFar(), [&]() {
#ifdef _USE_EFFEKSEER_
			Effekseer_Sync3DSetting();
#endif
			doing();
#ifdef _USE_EFFEKSEER_
			DrawEffekseer3D();
#endif
			doingFront();
			}, camInfo);
		//����
		DrawGBuffer(0.1f, 0.1f + camInfo.GetCamNear(), [&]() {
#ifdef _USE_EFFEKSEER_
			Effekseer_Sync3DSetting();
#endif
			doing();
#ifdef _USE_EFFEKSEER_
			DrawEffekseer3D();
#endif
			doingFront();
			}, camInfo);
		BufferScreen.SetDraw_Screen(false);
	}
	void PostPassEffect::Draw2D(std::function<void()> doing) noexcept {
		//�S�Ẳ�ʂ�������
		if (m_IsActiveGBuffer) {
			//���Z�b�g�ւ��
			ColorScreen.SetDraw_Screen();
			NormalScreen.SetDraw_Screen();
			FillGraph(NormalScreen.get(), 128, 128, 255);
			DepthScreen.SetDraw_Screen();
			FillGraph(DepthScreen.get(), 255, 0, 0);
		}
		BufferScreen.SetDraw_Screen();
		{
			doing();
		}
		BufferScreen.SetDraw_Screen(false);
	}
	void PostPassEffect::DrawPostProcess(void) noexcept {
		//�F���␳
		GraphFilter(BufferScreen.get(), DX_GRAPH_FILTER_LEVEL, InColorPerMin, InColorPerMax, int(InColorGamma * 100), 0, 255);
		//�|�X�g�p�X�G�t�F�N�g��buf�ɕ`��
		if (m_IsActiveGBuffer) {
			for (auto& P : m_PostPass) {
				GraphFilterBlt(BufferScreen.get(), ColorScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, 1);
				P->SetEffect(&BufferScreen, &ColorScreen, &NormalScreen, &DepthScreen);
			}
		}
	}
	//
	void PostPassEffect::DrawGBuffer(float near_len, float far_len, std::function<void()> done, const Camera3DInfo& camInfo) noexcept {
		// �J���[�o�b�t�@��`��Ώ�0�ɁA�@���o�b�t�@��`��Ώ�1�ɐݒ�
		SetRenderTargetToShader(0, BufferScreen.get());
		if (m_IsActiveGBuffer) {
			SetRenderTargetToShader(1, NormalScreen.get());
			SetRenderTargetToShader(2, DepthScreen.get());
		}
		ClearDrawScreenZBuffer();
		camInfo.FlipCamInfo();
		SetCameraNearFar(near_len, far_len);
		{
			done();
		}
		SetRenderTargetToShader(0, InvalidID);
		if (m_IsActiveGBuffer) {
			SetRenderTargetToShader(1, InvalidID);
			SetRenderTargetToShader(2, InvalidID);
		}
	}
	void PostPassEffect::LoadGBuffer(void) noexcept {
		auto Prev = GetCreateDrawValidGraphZBufferBitDepth();
		SetCreateDrawValidGraphZBufferBitDepth(24);
		NormalScreen = GraphHandle::Make(ScreenWidth, ScreenHeight, false);
		DepthScreen = GraphHandle::MakeDepth(ScreenWidth, ScreenHeight);
		SetCreateDrawValidGraphZBufferBitDepth(Prev);
	}
	void PostPassEffect::DisposeGBuffer(void) noexcept {
		NormalScreen.Dispose();
		DepthScreen.Dispose();
	}
};
