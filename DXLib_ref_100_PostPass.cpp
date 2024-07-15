#include "DXLib_ref_100_PostPass.hpp"

namespace DXLibRef {
	//--------------------------------------------------------------------------------------------------
	// ポストプロセスエフェクト
	//--------------------------------------------------------------------------------------------------
	//継承クラス
	class PostPassSSAO : public PostPassBase {
	public:
		void Load_Sub(void) noexcept override {}
		void Dispose_Sub(void) noexcept override {}
		bool IsActive_Sub(void) noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::SSAO);
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph, GraphHandle* NormalPtr, GraphHandle*) noexcept override {
			// SSAOフィルター処理
				// 変換元として法線バッファを指定
				// 出力先バッファの指定
				//
				// カラーバッファを指定
				// 遮蔽物を調べる範囲
				// 遮蔽物判定する最小深度値
				// 遮蔽物判定する最大深度値
				// 遮蔽物の影響の強さ
				// オクルージョンカラー
				// オクルージョンカラーの強さ
			GraphFilterBlt(NormalPtr->get(), TargetGraph->get(), DX_GRAPH_FILTER_SSAO, ColorGraph->get(), 120.f, 0.004f, 0.01f, 0.9f, Black, 20.f);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
	};
	class PostPassSSR : public PostPassBase {
	private:
		GraphHandle SSRColorScreen;	//そのままのGバッファ
		GraphHandle SSRNormalScreen;	//法線のGバッファ
		GraphHandle	SSRDepthScreen;	//深度のGバッファ
		GraphHandle SSRScreen;		//描画スクリーン
		GraphHandle bkScreen2;		//ブレンド
		ShaderUseClass::ScreenVertex	m_SSRScreenVertex;				// 頂点データ
		ShaderUseClass		m_Shader;										// シェーダー

		int RayInterval = 200;//レイの分割間隔
		float DepthThreshold = 17.f;

		static const int EXTEND = 4;
	public:
		PostPassSSR() {}
		PostPassSSR(const PostPassSSR&) = delete;
		PostPassSSR(PostPassSSR&& o) = delete;
		PostPassSSR& operator=(const PostPassSSR&) = delete;
		PostPassSSR& operator=(PostPassSSR&& o) = delete;

		virtual ~PostPassSSR(void) noexcept {}
	public:
		void Load_Sub(void) noexcept override {
			auto* DrawParts = DXDraw::Instance();
			int xsize = DrawParts->GetScreenY(1920) / EXTEND;
			int ysize = DrawParts->GetScreenY(1080) / EXTEND;
			SSRScreen = GraphHandle::Make(xsize, ysize, true);
			{
				bkScreen2 = GraphHandle::Make(xsize, ysize, false);
				bkScreen2.SetDraw_Screen();
				{
					int xr = xsize * 30 / 100;
					int yr = ysize * 60 / 100;

					DrawBox_2D(0, 0, xsize, ysize, Black, TRUE);
					DrawOval(xsize / 2, ysize / 2, xr, yr, White, TRUE);

					int r = 0, c = 0, p = 2;

					p = 1;
					for (r = 0; r < 255; r += p) {
						c = 255 - int(std::powf(float(255 - r) / 255.f, 1.5f)*255.f);

						DrawOval(xsize / 2, ysize / 2, xr - r / p, yr - r / p, GetColor(c, c, c), FALSE, 2);
					}
				}
			}
			SSRColorScreen = GraphHandle::Make(xsize, ysize, false);
			SSRNormalScreen = GraphHandle::Make(xsize, ysize, false);
			SSRDepthScreen = GraphHandle::MakeDepth(xsize, ysize);
			m_SSRScreenVertex.SetScreenVertex(xsize, ysize);
			m_Shader.Init("shader/VS_SSR.vso", "shader/PS_SSR.pso");
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

			GraphFilterBlt(ColorGraph->get(), SSRColorScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			GraphFilterBlt(NormalPtr->get(), SSRNormalScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			GraphFilterBlt(DepthPtr->get(), SSRDepthScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
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

				printfDx("RayInterval   :%d\n", RayInterval);
				printfDx("DepthThreshold:%f\n", DepthThreshold);
#endif // DEBUG
				SetUseTextureToShader(0, SSRColorScreen.get());
				SetUseTextureToShader(1, SSRNormalScreen.get());
				SetUseTextureToShader(2, SSRDepthScreen.get());
				SetUseTextureToShader(3, DrawParts->GetCubeMapTex().get());
				SetUseTextureToShader(4, bkScreen2.get());
				m_Shader.SetPixelParam(3, (float)RayInterval, 12.5f, std::tan(DrawParts->GetMainCamera().GetCamFov() / 2.f), DepthThreshold);
				m_Shader.SetPixelCameraMatrix(4, DrawParts->GetCamViewMatrix(), DrawParts->GetCamProjectionMatrix());
				m_Shader.SetPixelParam(5, (float)OptionParts->GetParamInt(EnumSaveParam::Reflection),0.f,0.f, 0.f);
				{
					m_Shader.Draw(m_SSRScreenVertex);
				}
				SetUseTextureToShader(0, -1);
				SetUseTextureToShader(1, -1);
				SetUseTextureToShader(2, -1);
				SetUseTextureToShader(3, -1);
				SetUseTextureToShader(4, -1);
			}
			GraphFilter(SSRScreen.get(), DX_GRAPH_FILTER_GAUSS, 8, 200);
			TargetGraph->SetDraw_Screen(false);
			{
				TargetGraph->DrawGraph(0, 0, true);
				SSRScreen.DrawExtendGraph(0, 0, DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), true);
			}
		}
	};
	class PostPassDoF : public PostPassBase {
	private:
		GraphHandle			DoFScreen;		//描画スクリーン
		ShaderUseClass::ScreenVertex	m_ScreenVertex;					// 頂点データ
		ShaderUseClass		m_Shader;			// シェーダー
	public:
		PostPassDoF() {}
		PostPassDoF(const PostPassDoF&) = delete;
		PostPassDoF(PostPassDoF&& o) = delete;
		PostPassDoF& operator=(const PostPassDoF&) = delete;
		PostPassDoF& operator=(PostPassDoF&& o) = delete;

		virtual ~PostPassDoF(void) noexcept {}
	public:
		void Load_Sub(void) noexcept override {
			auto* DrawParts = DXDraw::Instance();
			DoFScreen = GraphHandle::Make(DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), true);
			m_ScreenVertex.SetScreenVertex(DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080));
			m_Shader.Init("shader/VS_DoF.vso", "shader/PS_DoF.pso");
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
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph, GraphHandle* , GraphHandle* DepthPtr) noexcept override {
			auto* DrawParts = DXDraw::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			GraphFilterBlt(TargetGraph->get(), DoFScreen.get(), DX_GRAPH_FILTER_GAUSS, 16, 2000);
			TargetGraph->SetDraw_Screen();
			{
				SetUseTextureToShader(0, ColorGraph->get());
				SetUseTextureToShader(1, DoFScreen.get());
				SetUseTextureToShader(2, DepthPtr->get());
				m_Shader.SetPixelDispSize(DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080));
				m_Shader.SetPixelParam(3, PostPassParts->Get_near_DoF(), PostPassParts->Get_far_DoF(), PostPassParts->Get_near_DoFMax(), PostPassParts->Get_far_DoFMin());
				{
					m_Shader.Draw(m_ScreenVertex);
				}
				SetUseTextureToShader(0, -1);
				SetUseTextureToShader(1, -1);
				SetUseTextureToShader(2, -1);
			}
		}
	};
	class PostPassBloom : public PostPassBase {
	private:
		static const int EXTEND = 4;
	private:
		GraphHandle GaussScreen_;	//描画スクリーン
		GraphHandle	BufScreen;
	public:
		PostPassBloom() {}
		PostPassBloom(const PostPassBloom&) = delete;
		PostPassBloom(PostPassBloom&& o) = delete;
		PostPassBloom& operator=(const PostPassBloom&) = delete;
		PostPassBloom& operator=(PostPassBloom&& o) = delete;

		virtual ~PostPassBloom(void) noexcept {}
	public:
		void Load_Sub(void) noexcept override {
			auto* DrawParts = DXDraw::Instance();
			GaussScreen_ = GraphHandle::Make(DrawParts->GetScreenY(1920) / EXTEND, DrawParts->GetScreenY(1080) / EXTEND, true);
			BufScreen = GraphHandle::Make(DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), true);
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
			auto* DrawParts = DXDraw::Instance();
			GraphFilterBlt(TargetGraph->get(), BufScreen.get(), DX_GRAPH_FILTER_TWO_COLOR, 250, Black, 255, Gray50, 255);
			GraphFilterBlt(BufScreen.get(), GaussScreen_.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			GraphFilter(GaussScreen_.get(), DX_GRAPH_FILTER_GAUSS, 16, 1000);
			TargetGraph->SetDraw_Screen(false);
			{
				auto Prev = GetDrawMode();
				SetDrawMode(DX_DRAWMODE_BILINEAR);
				SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
				GaussScreen_.DrawExtendGraph(0, 0, DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), true);
				GaussScreen_.DrawExtendGraph(0, 0, DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				SetDrawMode(Prev);
			}
		}
	};
	class PostPassAberration : public PostPassBase {
	private:
		std::array<GraphHandle, 3>		BufScreen;
	public:
		PostPassAberration() {}
		PostPassAberration(const PostPassAberration&) = delete;
		PostPassAberration(PostPassAberration&& o) = delete;
		PostPassAberration& operator=(const PostPassAberration&) = delete;
		PostPassAberration& operator=(PostPassAberration&& o) = delete;

		virtual ~PostPassAberration(void) noexcept {}
	public:
		void Load_Sub(void) noexcept override {
			auto* DrawParts = DXDraw::Instance();
			for (auto&buf : BufScreen) {
				buf = GraphHandle::Make(DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), true);
			}
		}
		void Dispose_Sub(void) noexcept override {
			for (auto&buf : BufScreen) {
				buf.Dispose();
			}
		}
		bool IsActive_Sub(void) noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::ScreenEffect);
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*, GraphHandle*, GraphHandle*) noexcept override {
			auto* DrawParts = DXDraw::Instance();
			BufScreen[0].SetDraw_Screen(true);
				DrawBox_2D(0, 0, DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), Black, TRUE);
			BufScreen[1].SetDraw_Screen(true);
				DrawBox_2D(0, 0, DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), Black, TRUE);
			BufScreen[2].SetDraw_Screen(true);
				DrawBox_2D(0, 0, DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), Black, TRUE);
			GraphBlend(BufScreen[0].get(), TargetGraph->get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
					   DX_RGBA_SELECT_BLEND_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_SRC_A);
			GraphBlend(BufScreen[1].get(), TargetGraph->get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
					   DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_BLEND_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_SRC_A);
			GraphBlend(BufScreen[2].get(), TargetGraph->get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
					   DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_BLEND_B, DX_RGBA_SELECT_SRC_A);
			TargetGraph->SetDraw_Screen(true);
			{
				DrawBox_2D(0, 0, DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), Black, TRUE);
				SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
				BufScreen[0].DrawRotaGraph(DrawParts->GetScreenY(1920) / 2, DrawParts->GetScreenY(1080) / 2, 1.f + 0.005f*DrawParts->GetAberrationPower(), 0.f, true);
				BufScreen[1].DrawRotaGraph(DrawParts->GetScreenY(1920) / 2, DrawParts->GetScreenY(1080) / 2, 1.f, 0.f, true);
				BufScreen[2].DrawRotaGraph(DrawParts->GetScreenY(1920) / 2, DrawParts->GetScreenY(1080) / 2, 1.f - 0.005f*DrawParts->GetAberrationPower(), 0.f, true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
		}
	};
	class PostPassMotionBlur : public PostPassBase {
	private:
		class BlurScreen {
			static const int MAX = 3;
			std::array<GraphHandle,MAX> m_screen;
			int m_current{0};
			int m_alpha{0};
			int m_screenWidth{0}, m_screenHeight{0};
			int m_offsetX1{0}, m_offsetX2{0}, m_offsetY1{0}, offsetY2{0};
			int m_notBlendDraw{0};
		public:
			BlurScreen() {}
			BlurScreen(const BlurScreen&) = delete;
			BlurScreen(BlurScreen&& o) = delete;
			BlurScreen& operator=(const BlurScreen&) = delete;
			BlurScreen& operator=(BlurScreen&& o) = delete;

			~BlurScreen(void) noexcept {}
		public:
			void Init(int t_alpha, int t_offsetX1, int t_offsetY1, int t_offsetX2, int t_offsetY2) {
				auto* DrawParts = DXDraw::Instance();
				for (int i = 0; i < MAX; ++i) {
					m_screen.at(static_cast<size_t>(i)) = GraphHandle::Make(DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080));
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
					m_screen.at(static_cast<size_t>(i)).Dispose();
				}
			}
		public:
			auto* PostRenderBlurScreen(std::function<void()> doing) {
				auto* DrawParts = DXDraw::Instance();
				auto next = (m_current != 0) ? (m_current - 1) : MAX - 1;
				m_screen[static_cast<size_t>(m_current)].SetDraw_Screen();
				{
					doing();
					if (m_notBlendDraw++ > MAX) {
						int drawMode = GetDrawMode();
						SetDrawMode(DX_DRAWMODE_BILINEAR);
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_alpha);
						m_screen[static_cast<size_t>(next)].DrawExtendGraph(m_offsetX1, m_offsetY1, DrawParts->GetScreenY(1920) + m_offsetX2, DrawParts->GetScreenY(1080) + offsetY2, false);
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
		PostPassMotionBlur() {}
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
		GraphHandle AberrationScreen;	//描画スクリーン
		GraphHandle bkScreen2;
		GraphHandle	BufScreen;
	public:
		PostPassCornerBlur() {}
		PostPassCornerBlur(const PostPassCornerBlur&) = delete;
		PostPassCornerBlur(PostPassCornerBlur&& o) = delete;
		PostPassCornerBlur& operator=(const PostPassCornerBlur&) = delete;
		PostPassCornerBlur& operator=(PostPassCornerBlur&& o) = delete;

		virtual ~PostPassCornerBlur(void) noexcept {}
	public:
		void Load_Sub(void) noexcept override {
			auto* DrawParts = DXDraw::Instance();
			AberrationScreen = GraphHandle::Make(DrawParts->GetScreenY(1920) / EXTEND, DrawParts->GetScreenY(1080) / EXTEND, true);
			{
				bkScreen2 = GraphHandle::Make(DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), false);
				bkScreen2.SetDraw_Screen(true);
				{
					int xr = DrawParts->GetScreenY(1920) * 60 / 100;
					int yr = DrawParts->GetScreenY(1080) * 70 / 100;

					DrawBox_2D(0, 0, DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), Black, TRUE);
					DrawOval(DrawParts->GetScreenY(1920) / 2, DrawParts->GetScreenY(1080) / 2, xr, yr, White, TRUE);

					int r = 0, c = 0, p = 2;

					p = 1;
					for (r = 0; r < 255; r += p) {
						c = 255 - int(std::powf(float(255 - r) / 255.f, 1.5f)*255.f);

						DrawOval(DrawParts->GetScreenY(1920) / 2, DrawParts->GetScreenY(1080) / 2, xr - r / p, yr - r / p, GetColor(c, c, c), FALSE, 2);
					}
				}
			}
			BufScreen = GraphHandle::Make(DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), true);
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
			auto* DrawParts = DXDraw::Instance();
			GraphFilterBlt(TargetGraph->get(), AberrationScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			GraphFilter(AberrationScreen.get(), DX_GRAPH_FILTER_GAUSS, 16, 1000);
			GraphBlendBlt(TargetGraph->get(), bkScreen2.get(), BufScreen.get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
					   DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_BLEND_R);
			TargetGraph->SetDraw_Screen(false);
			{
				AberrationScreen.DrawExtendGraph(0, 0, DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), false);
				BufScreen.DrawGraph(0, 0, true);
			}
		}
	};
	class PostPassVignette : public PostPassBase {
	private:
		GraphHandle bkScreen;
		GraphHandle	BufScreen;
	public:
		PostPassVignette() {}
		PostPassVignette(const PostPassVignette&) = delete;
		PostPassVignette(PostPassVignette&& o) = delete;
		PostPassVignette& operator=(const PostPassVignette&) = delete;
		PostPassVignette& operator=(PostPassVignette&& o) = delete;

		virtual ~PostPassVignette(void) noexcept {}
	public:
		void Load_Sub(void) noexcept override {
			auto* DrawParts = DXDraw::Instance();
			{
				bkScreen = GraphHandle::Make(DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), false);
				bkScreen.SetDraw_Screen(true);
				{
					int y = 0, c = 0, p = 2;
					DrawBox_2D(0, 0, DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), White, TRUE);

					p = 1;
					for (y = 0; y < 255; y += p) {
						c = 255 - int(std::powf(float(255 - y) / 255.f, 1.5f)*64.f);
						DrawLine_2D(0, y / p, DrawParts->GetScreenY(1920), y / p, GetColor(c, c, c));
					}
					p = 2;
					for (y = 0; y < 255; y += p) {
						c = 255 - int(std::powf(float(255 - y) / 255.f, 1.5f)*128.f);
						DrawLine_2D(0, DrawParts->GetScreenY(1080) - y / p, DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080) - y / p, GetColor(c, c, c));
					}
				}
			}
			BufScreen = GraphHandle::Make(DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), true);
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
			auto* DrawParts = DXDraw::Instance();
			GraphBlendBlt(TargetGraph->get(), bkScreen.get(), BufScreen.get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
					   DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_BLEND_R);
			TargetGraph->SetDraw_Screen();
			{
				DrawBox_2D(0, 0, DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), Black, TRUE);
				BufScreen.DrawGraph(0, 0, true);
			}
		}
	};
	class PostPassDistortion : public PostPassBase {
	private:
		GraphHandle	BufScreen;
	public:
		PostPassDistortion() {}
		PostPassDistortion(const PostPassDistortion&) = delete;
		PostPassDistortion(PostPassDistortion&& o) = delete;
		PostPassDistortion& operator=(const PostPassDistortion&) = delete;
		PostPassDistortion& operator=(PostPassDistortion&& o) = delete;

		virtual ~PostPassDistortion(void) noexcept {}
	private:
		// 画面を歪ませながら描画する関数
		void DrawCircleScreen(
			int CenterX,			// 円の中心座標X
			int CenterY,			// 円の中心座標Y
			float Radius,			// 円のサイズ
			float Absorption,		// 内側の円に引き込まれるドット数
			const GraphHandle& ScreenHandle// 画面グラフィックハンドル
		) {
			auto* DrawParts = DXDraw::Instance();
			const int CIRCLE_ANGLE_VERTEX_NUM = 16;			// 円周の頂点数
			const int CIRCLE_RADIUS_VERTEX_NUM = 8;			// 半径の頂点数

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
			std::array<float, CIRCLE_ANGLE_VERTEX_NUM> AngleCosTable;
			std::array<float, CIRCLE_ANGLE_VERTEX_NUM> AngleSinTable;
			std::array<float, CIRCLE_RADIUS_VERTEX_NUM> InCircleCosTable;

			// スタックに積むには大きいので static 配列にしました
			static VERTEX2D Vertex[CIRCLE_RADIUS_VERTEX_NUM * CIRCLE_ANGLE_VERTEX_NUM];
			static WORD Index[CIRCLE_ANGLE_VERTEX_NUM * 6 * (CIRCLE_RADIUS_VERTEX_NUM - 1)];

			// 最初に普通に描画
			DrawGraph(0, 0, ScreenHandle.get(), FALSE);

			// 描画カラーを作成しておく
			DiffuseColor = GetColorU8(255, 255, 255, 255);

			// 外周部分用の Sin, Cos テーブルを作成する
			Angle = 0.0f;
			for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; i++, Angle += DX_PI_F * 2.0f / CIRCLE_ANGLE_VERTEX_NUM) {
				AngleSinTable.at(static_cast<size_t>(i)) = (float)sin((double)Angle);
				AngleCosTable.at(static_cast<size_t>(i)) = (float)cos((double)Angle);
			}

			// 内側の盛り上がっているように見せる箇所で使用する Cos テーブルを作成する
			Angle = 0.0f;
			for (i = 0; i < CIRCLE_RADIUS_VERTEX_NUM; i++, Angle += (DX_PI_F / 2.0f) / (CIRCLE_RADIUS_VERTEX_NUM - 1)) {
				InCircleCosTable.at(static_cast<size_t>(i)) = (float)cos((double)Angle);
			}

			// ポリゴン頂点インデックスの準備
			Ind = Index;
			for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; i++) {
				for (WORD j = 0; j < CIRCLE_RADIUS_VERTEX_NUM - 1; j++, Ind += 6) {
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

			// バイリニア補間描画にする
			SetDrawMode(DX_DRAWMODE_BILINEAR);


			// 外側のドーナツ部分を描画

			// 中心に向かうにしたがって中心方向にテクスチャ座標をずらす
			Vert = Vertex;
			for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; i++) {
				// 使用する Sin, Cos の値をセット
				Sin = AngleSinTable.at(static_cast<size_t>(i));
				Cos = AngleCosTable.at(static_cast<size_t>(i));

				for (int j = 0; j < CIRCLE_RADIUS_VERTEX_NUM; j++, Vert++) {
					// 円の中心までの距離を算出
					CenterDistance = Radius;

					// 中心に引き込まれる距離を算出
					AbsorptionDistance = Absorption * j / (CIRCLE_RADIUS_VERTEX_NUM - 1);

					// 中心に向かって移動する距離を算出
					AbsorptionMoveX = Cos * AbsorptionDistance;
					AbsorptionMoveY = Sin * AbsorptionDistance;

					// スクリーン座標の決定
					Vert->pos.x = Cos * CenterDistance + CenterX;
					Vert->pos.y = Sin * CenterDistance + CenterY;
					Vert->pos.z = 0.0f;

					// テクスチャ座標のセット
					Vert->u = (Vert->pos.x + AbsorptionMoveX) / DrawParts->GetScreenY(1920);
					Vert->v = (Vert->pos.y + AbsorptionMoveY) / DrawParts->GetScreenY(1080);

					// その他のパラメータをセット
					Vert->rhw = 1.0f;
					Vert->dif = DiffuseColor;
				}
			}

			// 歪んだドーナツの描画
			DrawPrimitiveIndexed2D(Vertex, sizeof(Vertex) / sizeof(VERTEX2D), Index, sizeof(Index) / sizeof(WORD), DX_PRIMTYPE_TRIANGLELIST, ScreenHandle.get(), FALSE);


			// 内側の盛り上がっているように見える部分を描画

			// Cosテーブルにしたがってテクスチャ座標をずらす
			Vert = Vertex;
			for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; i++) {
				// 使用する Sin, Cos の値をセット
				Sin = AngleSinTable.at(static_cast<size_t>(i));
				Cos = AngleCosTable.at(static_cast<size_t>(i));

				for (int j = 0; j < CIRCLE_RADIUS_VERTEX_NUM; j++, Vert++) {
					// 円の中心までの距離を算出
					CenterDistance = InCircleCosTable[static_cast<size_t>(j)] * Radius;

					// 画像座標視点での円の中心までの距離を算出
					GraphCenterDistance = ((CIRCLE_RADIUS_VERTEX_NUM - 1) - j) * (Absorption + Radius) / (CIRCLE_RADIUS_VERTEX_NUM - 1);

					// スクリーン座標の決定
					Vert->pos.x = Cos * CenterDistance + CenterX;
					Vert->pos.y = Sin * CenterDistance + CenterY;
					Vert->pos.z = 0.0f;

					// テクスチャ座標のセット
					Vert->u = (Cos * GraphCenterDistance + CenterX) / DrawParts->GetScreenY(1920);
					Vert->v = (Sin * GraphCenterDistance + CenterY) / DrawParts->GetScreenY(1080);

					// その他のパラメータをセット
					Vert->rhw = 1.0f;
					Vert->dif = DiffuseColor;
				}
			}

			// 中心の盛り上がって見える部分を描画
			DrawPrimitiveIndexed2D(Vertex, sizeof(Vertex) / sizeof(VERTEX2D), Index, sizeof(Index) / sizeof(WORD), DX_PRIMTYPE_TRIANGLELIST, ScreenHandle.get(), FALSE);
		}
	public:
		void Load_Sub(void) noexcept override {
			auto* DrawParts = DXDraw::Instance();
			BufScreen = GraphHandle::Make(DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), true);
		}
		void Dispose_Sub(void) noexcept override {
			BufScreen.Dispose();
		}
		bool IsActive_Sub(void) noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::ScreenEffect) && OptionParts->GetParamBoolean(EnumSaveParam::Distortion);
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*, GraphHandle*, GraphHandle*) noexcept override {
			auto* DrawParts = DXDraw::Instance();
			BufScreen.SetDraw_Screen();
			{
				TargetGraph->DrawGraph(0, 0, true);
			}
			//GraphFilterBlt(BufScreen.get(), TargetGraph->get(), DX_GRAPH_FILTER_DOWN_SCALE, 1);
			TargetGraph->SetDraw_Screen();
			{
				// 画面を歪ませて描画
				DrawCircleScreen(
					DrawParts->GetScreenY(1920 / 2), DrawParts->GetScreenY(1080 / 2),
					(float)DrawParts->GetScreenY(1920 * 2 / 3), 120.0f, BufScreen);
			}
		}
	};
	class PostPassFXAA : public PostPassBase {
	private:
		ShaderUseClass::ScreenVertex	m_ScreenVertex;
		ShaderUseClass		m_Shader;
	public:
		void Load_Sub(void) noexcept override {
			auto* DrawParts = DXDraw::Instance();
			m_ScreenVertex.SetScreenVertex(DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080));
			m_Shader.Init("shader/VS_FXAA.vso", "shader/PS_FXAA.pso");
		}
		void Dispose_Sub(void) noexcept override {
			m_Shader.Dispose();
		}
		bool IsActive_Sub(void) noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::AA);
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph, GraphHandle* , GraphHandle*) noexcept override {
			auto* DrawParts = DXDraw::Instance();
			TargetGraph->SetDraw_Screen();
			{
				SetUseTextureToShader(0, ColorGraph->get());
				m_Shader.SetPixelDispSize(DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080));
				{
					m_Shader.Draw(m_ScreenVertex);
				}
				SetUseTextureToShader(0, -1);
			}
		}
	};
	class PostPassGodRay : public PostPassBase {
	private:
		GraphHandle SSRColorScreen;	//そのままのGバッファ
		GraphHandle	SSRDepthScreen;	//深度のGバッファ
		GraphHandle SSRScreen;		//描画スクリーン

		ShaderUseClass::ScreenVertex	m_ScreenVertex;					// 頂点データ
		ShaderUseClass		m_Shader;			// シェーダー
		float range = 1.f;

		static const int EXTEND = 4;
	public:
		PostPassGodRay() {}
		PostPassGodRay(const PostPassGodRay&) = delete;
		PostPassGodRay(PostPassGodRay&& o) = delete;
		PostPassGodRay& operator=(const PostPassGodRay&) = delete;
		PostPassGodRay& operator=(PostPassGodRay&& o) = delete;

		virtual ~PostPassGodRay(void) noexcept {}
	public:
		void Load_Sub(void) noexcept override {
			auto* DrawParts = DXDraw::Instance();
			m_ScreenVertex.SetScreenVertex(DrawParts->GetScreenY(1920) / EXTEND, DrawParts->GetScreenY(1080) / EXTEND);
			m_Shader.Init("shader/VS_GodRay.vso", "shader/PS_GodRay.pso");
			SSRScreen = GraphHandle::Make(DrawParts->GetScreenY(1920) / EXTEND, DrawParts->GetScreenY(1080) / EXTEND, true);
			SSRDepthScreen = GraphHandle::MakeDepth(DrawParts->GetScreenY(1920) / EXTEND, DrawParts->GetScreenY(1080) / EXTEND);
		}
		void Dispose_Sub(void) noexcept override {
			SSRScreen.Dispose();
			SSRDepthScreen.Dispose();
			m_Shader.Dispose();
		}
		bool IsActive_Sub(void) noexcept override {
			auto* OptionParts = OPTION::Instance();
			return (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0) && OptionParts->GetParamBoolean(EnumSaveParam::GodRay);
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph, GraphHandle* , GraphHandle* DepthPtr) noexcept override {
			auto* OptionParts = OPTION::Instance();
			auto* DrawParts = DXDraw::Instance();
			GraphFilterBlt(DepthPtr->get(), SSRDepthScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);

			m_Shader.SetPixelCameraMatrix(4, DrawParts->GetCamViewMatrix().inverse(), DrawParts->GetCamProjectionMatrix().inverse());
			m_Shader.SetPixelCameraMatrix(5, DrawParts->GetShadowDraw()->GetCamViewMatrix(false), DrawParts->GetShadowDraw()->GetCamProjectionMatrix(false));
			m_Shader.SetPixelCameraMatrix(6, DrawParts->GetShadowDraw()->GetCamViewMatrix(true), DrawParts->GetShadowDraw()->GetCamProjectionMatrix(true));
			SSRScreen.SetDraw_Screen();
			{
				SetUseTextureToShader(0, SSRDepthScreen.get());
				SetUseTextureToShader(1, DrawParts->GetShadowDraw()->GetDepthScreen().get());
				SetUseTextureToShader(2, DrawParts->GetShadowDraw()->GetDepthFarScreen().get());
				{
					float Power = 1.f;
					switch (OptionParts->GetParamInt(EnumSaveParam::shadow)) {
						case 1:
							Power = 20.f;
							break;
						case 2:
							Power = 35.f;
							break;
						case 3:
							Power = 50.f;
							break;
						default:
							break;
					}
					m_Shader.SetPixelParam(3, Power, 0.f, std::tan(DrawParts->GetMainCamera().GetCamFov() / 2.f), 0.f);
					m_Shader.Draw(m_ScreenVertex);
				}
				SetUseTextureToShader(0, -1);
				SetUseTextureToShader(1, -1);
				SetUseTextureToShader(2, -1);
			}
			GraphFilter(SSRScreen.get(), DX_GRAPH_FILTER_GAUSS, 16, 300);
			TargetGraph->SetDraw_Screen();
			{
				ColorGraph->DrawGraph(0, 0, true);
				SetDrawBlendMode(DX_BLENDMODE_ADD, 255 / 2);
				SSRScreen.DrawExtendGraph(0, 0, DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
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
		BufferScreen = GraphHandle::Make(DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), true);
		ColorScreen = GraphHandle::Make(DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), false);
		//ポストエフェクト
		m_PostPass.emplace_back(std::make_unique<PostPassBloom>());
		m_PostPass.emplace_back(std::make_unique<PostPassSSAO>());
		m_PostPass.emplace_back(std::make_unique<PostPassGodRay>());
		m_PostPass.emplace_back(std::make_unique<PostPassDoF>());
		m_PostPass.emplace_back(std::make_unique<PostPassSSR>());
		m_PostPass.emplace_back(std::make_unique<PostPassDistortion>());
		m_PostPass.emplace_back(std::make_unique<PostPassAberration>());
		m_PostPass.emplace_back(std::make_unique<PostPassMotionBlur>());
		m_PostPass.emplace_back(std::make_unique<PostPassVignette>());
		m_PostPass.emplace_back(std::make_unique<PostPassCornerBlur>());
		m_PostPass.emplace_back(std::make_unique<PostPassFXAA>());
		for (auto& P : m_PostPass) {
			P->Init();
		}
		//Gバッファ
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
		//Gバッファ
		if (!m_IsActiveGBuffer) {
			DisposeGBuffer();
		}
		//ポストエフェクト
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
		if (m_IsActiveGBuffer!= ActiveGBuffer) {
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
	void PostPassEffect::DrawDoF(std::function<void()> sky_doing, std::function<void()> doing, std::function<void()> doingFront, const Camera3DInfo& camInfo) {
		//全ての画面を初期化
		if (m_IsActiveGBuffer) {
			//リセット替わり
			ColorScreen.SetDraw_Screen();
			NormalScreen.SetDraw_Screen();
			DepthScreen.SetDraw_Screen();
		}
		//空
		DrawGBuffer(1000.0f, 50000.0f, [&]() { sky_doing(); }, camInfo);
		//遠距離
		DrawGBuffer(camInfo.GetCamFar() - 10.f, 1000000.f, [&]() {
			doing();
			doingFront();
			}, camInfo);
		//中間
		DrawGBuffer(camInfo.GetCamNear(), camInfo.GetCamFar(), [&]() {
			Effekseer_Sync3DSetting();
			doing();
			DrawEffekseer3D();
			doingFront();
			}, camInfo);
		//至近
		DrawGBuffer(0.1f, 0.1f + camInfo.GetCamNear(), [&]() {
			Effekseer_Sync3DSetting();
			doing();
			DrawEffekseer3D();
			doingFront();
			}, camInfo);
		BufferScreen.SetDraw_Screen(false);
	}
	void PostPassEffect::Draw2D(std::function<void()> doing) {
		auto* DrawParts = DXDraw::Instance();
		//全ての画面を初期化
		if (m_IsActiveGBuffer) {
			//リセット替わり
			ColorScreen.SetDraw_Screen();
			NormalScreen.SetDraw_Screen();
				DrawBox_2D(0, 0, DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), GetColor(128,128,255), TRUE);
			DepthScreen.SetDraw_Screen();
				DrawBox_2D(0, 0, DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), Red, TRUE);
		}
		BufferScreen.SetDraw_Screen();
		{
			doing();
		}
		BufferScreen.SetDraw_Screen(false);
	}
	void PostPassEffect::Draw() {
		//色味補正
		GraphFilter(BufferScreen.get(), DX_GRAPH_FILTER_LEVEL, InColorPerMin, InColorPerMax, int(InColorGamma * 100), 0, 255);
		//ポストパスエフェクトのbufに描画
		if (m_IsActiveGBuffer) {
			for (auto& P : m_PostPass) {
				GraphFilterBlt(BufferScreen.get(), ColorScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, 1);
				P->SetEffect(&BufferScreen, &ColorScreen, &NormalScreen, &DepthScreen);
			}
		}
	}
	//
	void PostPassEffect::DrawGBuffer(float near_len, float far_len, std::function<void()> done, const Camera3DInfo& camInfo) {
		// カラーバッファを描画対象0に、法線バッファを描画対象1に設定
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
		SetRenderTargetToShader(0, -1);
		if (m_IsActiveGBuffer) {
			SetRenderTargetToShader(1, -1);
			SetRenderTargetToShader(2, -1);
		}
	}
	void PostPassEffect::LoadGBuffer(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		NormalScreen = GraphHandle::Make(DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080), false);
		DepthScreen = GraphHandle::MakeDepth(DrawParts->GetScreenY(1920), DrawParts->GetScreenY(1080));
	}
	void PostPassEffect::DisposeGBuffer(void) noexcept {
		NormalScreen.Dispose();
		DepthScreen.Dispose();
	}
};
