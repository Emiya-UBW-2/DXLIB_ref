#include "DXLib_ref_100_PostPass.hpp"

namespace DXLibRef {
	// --------------------------------------------------------------------------------------------------
	// ポストプロセスエフェクト
	// --------------------------------------------------------------------------------------------------
	// 継承クラス
	class PostPassSSAO : public PostPassBase {
		ShaderUseClass::ScreenVertex	m_ScreenVertex;				// 頂点データ
		ShaderUseClass		m_ShaderSSAO;										// シェーダー
		ShaderUseClass		m_ShaderBlur;										// シェーダー
		GraphHandle SSRScreen;		// 描画スクリーン
		GraphHandle SSRScreen2;		// 描画スクリーン
	public:
		void Load_Sub(void) noexcept override {
			auto* DrawParts = DXDraw::Instance();
			int xsize = DrawParts->GetScreenXMax();
			int ysize = DrawParts->GetScreenYMax();
			m_ScreenVertex.SetScreenVertex(xsize, ysize);
			m_ShaderSSAO.Init("CommonData/shader/VS_SSAO.vso", "CommonData/shader/PS_SSAO.pso");
			m_ShaderBlur.Init("CommonData/shader/VS_SSAO.vso", "CommonData/shader/PS_BilateralBlur.pso");
			auto Prev = GetCreateDrawValidGraphZBufferBitDepth();
			SetCreateDrawValidGraphZBufferBitDepth(24);
			SSRScreen.Make(xsize, ysize, true);
			SSRScreen2.Make(xsize, ysize, true);
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
			// SSRシェーダーを適用
			SSRScreen2.SetDraw_Screen(false);
			{
				ColorGraph->SetUseTextureToShader(0);
				NormalPtr->SetUseTextureToShader(1);
				DepthPtr->SetUseTextureToShader(2);
				m_ShaderSSAO.SetPixelDispSize(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax());
				m_ShaderSSAO.SetPixelParam(3, 0.0f, Scale3DRate, std::tan(DrawParts->GetMainCamera().GetCamFov() / 2.f), 0.f);

				m_ShaderSSAO.Draw(m_ScreenVertex);

				SetUseTextureToShader(0, InvalidID);
				SetUseTextureToShader(1, InvalidID);
				SetUseTextureToShader(2, InvalidID);
			}
			// SSRシェーダーにぼかしを入れる
			SSRScreen.SetDraw_Screen(false);
			{
				SSRScreen2.SetUseTextureToShader(0);	// 使用するテクスチャをセット

				m_ShaderBlur.SetPixelDispSize(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax());
				m_ShaderBlur.Draw(m_ScreenVertex);

				SetUseTextureToShader(0, InvalidID);
			}
			// 
			TargetGraph->SetDraw_Screen(false);
			{
				TargetGraph->DrawGraph(0, 0, true);
				SetDrawBlendMode(DX_BLENDMODE_MULA, 255);
				SSRScreen.DrawExtendGraph(0, 0, DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				// SSRScreen2.DrawExtendGraph(0, 0, DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), true);
			}
		}
	};
	class PostPassSSR : public PostPassBase {
	private:
		GraphHandle SSRColorScreen;	// そのままのGバッファ
		GraphHandle SSRNormalScreen;	// 法線のGバッファ
		GraphHandle	SSRDepthScreen;	// 深度のGバッファ
		GraphHandle SSRScreen;		// 描画スクリーン
		GraphHandle bkScreen2;		// ブレンド
		ShaderUseClass::ScreenVertex	m_SSRScreenVertex;				// 頂点データ
		ShaderUseClass		m_Shader;										// シェーダー

		int RayInterval = 200;// レイの分割間隔
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
			auto* DrawParts = DXDraw::Instance();
			int xsize = DrawParts->GetScreenXMax() / EXTEND;
			int ysize = DrawParts->GetScreenYMax() / EXTEND;
			SSRScreen.Make(xsize, ysize, true);
			{
				bkScreen2.Make(xsize, ysize, false);
				bkScreen2.SetDraw_Screen(false);
				bkScreen2.FillGraph(0, 0, 0);
				{
					int xr = xsize * 30 / 100;
					int yr = ysize * 60 / 100;

					DrawOval(xsize / 2, ysize / 2, xr, yr, White, TRUE);

					int p = 1;
					for (int r = 0; r < 255; r += p) {
						uint8_t c = static_cast<uint8_t>(255 - static_cast<int>(std::powf(static_cast<float>(255 - r) / 255.f, 1.5f) * 255.f));

						DrawOval(xsize / 2, ysize / 2, xr - r / p, yr - r / p, GetColorFix(c, c, c), FALSE, 2);
					}
				}
			}
			SSRColorScreen.Make(xsize, ysize, false);
			SSRNormalScreen.Make(xsize, ysize, false);
			SSRDepthScreen.MakeDepth(xsize, ysize);
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

			SSRColorScreen.GraphFilterBlt(*ColorGraph, DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			SSRNormalScreen.GraphFilterBlt(*NormalPtr, DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			SSRDepthScreen.GraphFilterBlt(*DepthPtr, DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
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
				SSRColorScreen.SetUseTextureToShader(0);
				SSRNormalScreen.SetUseTextureToShader(1);
				SSRDepthScreen.SetUseTextureToShader(2);
				if (OptionParts->GetParamBoolean(EnumProjectSettingParam::CubeMap)) {
					DrawParts->GetCubeMapTex().SetUseTextureToShader(3);
				}
				else {
					bkScreen2.SetUseTextureToShader(3);
				}
				bkScreen2.SetUseTextureToShader(4);
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
			SSRScreen.GraphFilter(DX_GRAPH_FILTER_GAUSS, 8, 200);
			TargetGraph->SetDraw_Screen(false);
			{
				TargetGraph->DrawGraph(0, 0, true);
				SSRScreen.DrawExtendGraph(0, 0, DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), true);
			}
		}
	};
	class PostPassDoF : public PostPassBase {
	private:
		GraphHandle			DoFScreen;		// 描画スクリーン
		ShaderUseClass::ScreenVertex	m_ScreenVertex;					// 頂点データ
		ShaderUseClass		m_Shader;			// シェーダー
	public:
		PostPassDoF(void) noexcept {}
		PostPassDoF(const PostPassDoF&) = delete;
		PostPassDoF(PostPassDoF&& o) = delete;
		PostPassDoF& operator=(const PostPassDoF&) = delete;
		PostPassDoF& operator=(PostPassDoF&& o) = delete;

		virtual ~PostPassDoF(void) noexcept {}
	public:
		void Load_Sub(void) noexcept override {
			auto* DrawParts = DXDraw::Instance();
			DoFScreen.Make(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), true);
			m_ScreenVertex.SetScreenVertex(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax());
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
			auto* DrawParts = DXDraw::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			DoFScreen.GraphFilterBlt(*TargetGraph, DX_GRAPH_FILTER_GAUSS, 16, 2000);
			TargetGraph->SetDraw_Screen();
			{
				ColorGraph->SetUseTextureToShader(0);
				DoFScreen.SetUseTextureToShader(1);
				DepthPtr->SetUseTextureToShader(2);
				m_Shader.SetPixelDispSize(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax());
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
		GraphHandle GaussScreen_;	// 描画スクリーン
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
			auto* DrawParts = DXDraw::Instance();
			GaussScreen_.Make(DrawParts->GetScreenXMax() / EXTEND, DrawParts->GetScreenYMax() / EXTEND, true);
			BufScreen.Make(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), true);
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
			BufScreen.GraphFilterBlt(*TargetGraph, DX_GRAPH_FILTER_TWO_COLOR, 250, Black, 255, Gray50, 255);
			GaussScreen_.GraphFilterBlt(BufScreen, DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			GaussScreen_.GraphFilter(DX_GRAPH_FILTER_GAUSS, 16, 1000);
			TargetGraph->SetDraw_Screen(false);
			{
				auto Prev = GetDrawMode();
				SetDrawMode(DX_DRAWMODE_BILINEAR);
				SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
				GaussScreen_.DrawExtendGraph(0, 0, DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), true);
				GaussScreen_.DrawExtendGraph(0, 0, DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), true);
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
			auto* DrawParts = DXDraw::Instance();
			for (auto& buf : BufScreen) {
				buf.Make(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), true);
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
			auto* PostPassParts = PostPassEffect::Instance();
			auto* DrawParts = DXDraw::Instance();
			BufScreen.at(0).SetDraw_Screen(false);
			BufScreen.at(0).FillGraph(0, 0, 0);
			BufScreen.at(1).SetDraw_Screen(false);
			BufScreen.at(1).FillGraph(0, 0, 0);
			BufScreen.at(2).SetDraw_Screen(false);
			BufScreen.at(2).FillGraph(0, 0, 0);
			BufScreen.at(0).GraphBlend(*TargetGraph, 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_BLEND_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_SRC_A);
			BufScreen.at(1).GraphBlend(*TargetGraph, 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_BLEND_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_SRC_A);
			BufScreen.at(2).GraphBlend(*TargetGraph, 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_BLEND_B, DX_RGBA_SELECT_SRC_A);
			TargetGraph->SetDraw_Screen(false);
			{
				TargetGraph->FillGraph(0, 0, 0);
				SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
				BufScreen.at(0).DrawRotaGraph(DrawParts->GetScreenXMax() / 2, DrawParts->GetScreenYMax() / 2, 1.f + 0.005f * PostPassParts->GetAberrationPower(), 0.f, true);
				BufScreen.at(1).DrawRotaGraph(DrawParts->GetScreenXMax() / 2, DrawParts->GetScreenYMax() / 2, 1.f, 0.f, true);
				BufScreen.at(2).DrawRotaGraph(DrawParts->GetScreenXMax() / 2, DrawParts->GetScreenYMax() / 2, 1.f - 0.005f * PostPassParts->GetAberrationPower(), 0.f, true);
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
				auto* DrawParts = DXDraw::Instance();
				for (int i : std::views::iota(0, MAX)) {
					m_screen.at(static_cast<size_t>(i)).Make(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax());
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
				auto* DrawParts = DXDraw::Instance();
				auto next = (m_current != 0) ? (m_current - 1) : MAX - 1;
				m_screen[static_cast<size_t>(m_current)].SetDraw_Screen();
				{
					doing();
					if (++m_notBlendDraw > MAX) {
						int drawMode = GetDrawMode();
						SetDrawMode(DX_DRAWMODE_BILINEAR);
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_alpha);
						m_screen[static_cast<size_t>(next)].DrawExtendGraph(m_offsetX1, m_offsetY1, DrawParts->GetScreenXMax() + m_offsetX2, DrawParts->GetScreenYMax() + offsetY2, false);
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
		GraphHandle AberrationScreen;	// 描画スクリーン
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
			auto* DrawParts = DXDraw::Instance();
			AberrationScreen.Make(DrawParts->GetScreenXMax() / EXTEND, DrawParts->GetScreenYMax() / EXTEND, true);
			{
				bkScreen2.Make(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), false);
				bkScreen2.SetDraw_Screen(false);
				{
					bkScreen2.FillGraph(0, 0, 0);
					int xr = DrawParts->GetScreenXMax() * 60 / 100;
					int yr = DrawParts->GetScreenYMax() * 70 / 100;

					DrawOval(DrawParts->GetScreenXMax() / 2, DrawParts->GetScreenYMax() / 2, xr, yr, White, TRUE);

					int p = 1;
					for (int r = 0; r < 255; r += p) {
						uint8_t c = static_cast<uint8_t>(255 - static_cast<int>(std::powf(static_cast<float>(255 - r) / 255.f, 1.5f) * 255.f));

						DrawOval(DrawParts->GetScreenXMax() / 2, DrawParts->GetScreenYMax() / 2, xr - r / p, yr - r / p, GetColorFix(c, c, c), FALSE, 2);
					}
				}
			}
			BufScreen.Make(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), true);
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
			AberrationScreen.GraphFilterBlt(*TargetGraph, DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			AberrationScreen.GraphFilter(DX_GRAPH_FILTER_GAUSS, 16, 1000);
			BufScreen.GraphBlendBlt(*TargetGraph, bkScreen2, 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_BLEND_R);
			TargetGraph->SetDraw_Screen(false);
			{
				AberrationScreen.DrawExtendGraph(0, 0, DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), false);
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
			auto* DrawParts = DXDraw::Instance();
			{
				bkScreen.Make(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), false);
				bkScreen.SetDraw_Screen(true);
				bkScreen.FillGraph(255, 255, 255);
				{
					int p = 1;
					for (int y = 0; y < 255; y += p) {
						uint8_t c = static_cast<uint8_t>(255 - static_cast<int>(std::powf(static_cast<float>(255 - y) / 255.f, 1.5f) * 64.f));
						DxLib::DrawLine(0, y / p, DrawParts->GetScreenXMax(), y / p, GetColorFix(c, c, c));
					}
					p = 1;
					for (int y = 0; y < 255; y += p) {
						uint8_t c = static_cast<uint8_t>(255 - static_cast<int>(std::powf(static_cast<float>(255 - y) / 255.f, 1.5f) * 128.f));
						DxLib::DrawLine(0, DrawParts->GetScreenYMax() - y / p, DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax() - y / p, GetColorFix(c, c, c));
					}
				}
			}
			BufScreen.Make(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), true);
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
			BufScreen.GraphBlendBlt(*TargetGraph, bkScreen, 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_BLEND_R);
			TargetGraph->SetDraw_Screen(false);
			{
				TargetGraph->FillGraph(0, 0, 0);
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
			VERTEX2D* Vert;
			WORD* Ind;
			std::array<float, CIRCLE_ANGLE_VERTEX_NUM> AngleCosTable{};
			std::array<float, CIRCLE_ANGLE_VERTEX_NUM> AngleSinTable{};
			std::array<float, CIRCLE_RADIUS_VERTEX_NUM> InCircleCosTable{};

			// スタックに積むには大きいので static 配列にしました
			static VERTEX2D Vertex[CIRCLE_RADIUS_VERTEX_NUM * CIRCLE_ANGLE_VERTEX_NUM];
			static WORD Index[CIRCLE_ANGLE_VERTEX_NUM * 6 * (CIRCLE_RADIUS_VERTEX_NUM - 1)];

			// 最初に普通に描画
			ScreenHandle.DrawGraph(0, 0, false);

			// 描画カラーを作成しておく
			DiffuseColor = GetColorU8(255, 255, 255, 255);

			// 外周部分用の Sin, Cos テーブルを作成する
			Angle = 0.0f;
			for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; ++i, Angle += DX_PI_F * 2.0f / CIRCLE_ANGLE_VERTEX_NUM) {
				AngleSinTable.at(static_cast<size_t>(i)) = std::sin(Angle);
				AngleCosTable.at(static_cast<size_t>(i)) = std::cos(Angle);
			}

			// 内側の盛り上がっているように見せる箇所で使用する Cos テーブルを作成する
			Angle = 0.0f;
			for (i = 0; i < CIRCLE_RADIUS_VERTEX_NUM; ++i, Angle += (DX_PI_F / 2.0f) / (CIRCLE_RADIUS_VERTEX_NUM - 1)) {
				InCircleCosTable.at(static_cast<size_t>(i)) = std::cos(Angle);
			}

			// ポリゴン頂点インデックスの準備
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

			// バイリニア補間描画にする
			SetDrawMode(DX_DRAWMODE_BILINEAR);


			// 外側のドーナツ部分を描画

			// 中心に向かうにしたがって中心方向にテクスチャ座標をずらす
			Vert = Vertex;
			for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; ++i) {
				// 使用する Sin, Cos の値をセット
				Sin = AngleSinTable.at(static_cast<size_t>(i));
				Cos = AngleCosTable.at(static_cast<size_t>(i));

				for (int j = 0; j < CIRCLE_RADIUS_VERTEX_NUM; ++j, ++Vert) {
					// 円の中心までの距離を算出
					CenterDistance = Radius;

					// 中心に引き込まれる距離を算出
					AbsorptionDistance = Absorption * static_cast<float>(j) / static_cast<float>(CIRCLE_RADIUS_VERTEX_NUM - 1);

					// 中心に向かって移動する距離を算出
					AbsorptionMoveX = Cos * AbsorptionDistance;
					AbsorptionMoveY = Sin * AbsorptionDistance;

					// スクリーン座標の決定
					Vert->pos.x = Cos * CenterDistance + static_cast<float>(CenterX);
					Vert->pos.y = Sin * CenterDistance + static_cast<float>(CenterY);
					Vert->pos.z = 0.0f;

					// テクスチャ座標のセット
					Vert->u = (Vert->pos.x + AbsorptionMoveX) / static_cast<float>(DrawParts->GetScreenXMax());
					Vert->v = (Vert->pos.y + AbsorptionMoveY) / static_cast<float>(DrawParts->GetScreenYMax());

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
			for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; ++i) {
				// 使用する Sin, Cos の値をセット
				Sin = AngleSinTable.at(static_cast<size_t>(i));
				Cos = AngleCosTable.at(static_cast<size_t>(i));

				for (int j = 0; j < CIRCLE_RADIUS_VERTEX_NUM; ++j, ++Vert) {
					// 円の中心までの距離を算出
					CenterDistance = InCircleCosTable[static_cast<size_t>(j)] * Radius;

					// 画像座標視点での円の中心までの距離を算出
					GraphCenterDistance = static_cast<float>((CIRCLE_RADIUS_VERTEX_NUM - 1) - j) * (Absorption + Radius) / static_cast<float>(CIRCLE_RADIUS_VERTEX_NUM - 1);

					// スクリーン座標の決定
					Vert->pos.x = Cos * CenterDistance + static_cast<float>(CenterX);
					Vert->pos.y = Sin * CenterDistance + static_cast<float>(CenterY);
					Vert->pos.z = 0.0f;

					// テクスチャ座標のセット
					Vert->u = (Cos * GraphCenterDistance + static_cast<float>(CenterX)) / static_cast<float>(DrawParts->GetScreenXMax());
					Vert->v = (Sin * GraphCenterDistance + static_cast<float>(CenterY)) / static_cast<float>(DrawParts->GetScreenYMax());

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
			BufScreen.Make(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), true);
		}
		void Dispose_Sub(void) noexcept override {
			BufScreen.Dispose();
		}
		bool IsActive_Sub(void) noexcept override {
			auto* OptionParts = OPTION::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::ScreenEffect) && OptionParts->GetParamBoolean(EnumProjectSettingParam::Distortion);
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*, GraphHandle*, GraphHandle*) noexcept override {
			auto* DrawParts = DXDraw::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			BufScreen.SetDraw_Screen();
			{
				TargetGraph->DrawGraph(0, 0, true);
			}
			// TargetGraph->GraphFilterBlt(BufScreen, DX_GRAPH_FILTER_DOWN_SCALE, 1);
			TargetGraph->SetDraw_Screen();
			{
				// 画面を歪ませて描画
				DrawCircleScreen(
					DrawParts->GetScreenXMax() / 2, DrawParts->GetScreenYMax() / 2,
					static_cast<float>(DrawParts->GetScreenXMax() * 2 / 3), static_cast<float>(DrawParts->GetScreenY(static_cast<int>(PostPassParts->GetDistortionPer()))), BufScreen);
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
			m_ScreenVertex.SetScreenVertex(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax());
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
			auto* DrawParts = DXDraw::Instance();
			TargetGraph->SetDraw_Screen();
			{
				ColorGraph->SetUseTextureToShader(0);
				m_Shader.SetPixelDispSize(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax());
				{
					m_Shader.Draw(m_ScreenVertex);
				}
				SetUseTextureToShader(0, InvalidID);
			}
		}
	};
	class PostPassGodRay : public PostPassBase {
	private:
		GraphHandle SSRColorScreen;	// そのままのGバッファ
		GraphHandle	SSRDepthScreen;	// 深度のGバッファ
		GraphHandle SSRScreen;		// 描画スクリーン

		ShaderUseClass::ScreenVertex	m_ScreenVertex;					// 頂点データ
		ShaderUseClass		m_Shader;			// シェーダー
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
			auto* DrawParts = DXDraw::Instance();
			m_ScreenVertex.SetScreenVertex(DrawParts->GetScreenXMax() / EXTEND, DrawParts->GetScreenYMax() / EXTEND);
			m_Shader.Init("CommonData/shader/VS_GodRay.vso", "CommonData/shader/PS_GodRay.pso");
			SSRScreen.Make(DrawParts->GetScreenXMax() / EXTEND, DrawParts->GetScreenYMax() / EXTEND, true);
			SSRDepthScreen.MakeDepth(DrawParts->GetScreenXMax() / EXTEND, DrawParts->GetScreenYMax() / EXTEND);
		}
		void Dispose_Sub(void) noexcept override {
			SSRScreen.Dispose();
			SSRDepthScreen.Dispose();
			m_Shader.Dispose();
		}
		bool IsActive_Sub(void) noexcept override {
			auto* PostPassParts = PostPassEffect::Instance();
			auto* OptionParts = OPTION::Instance();
			return (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0) && OptionParts->GetParamBoolean(EnumProjectSettingParam::GodRay) && (PostPassParts->GetGodRayPer() > 0.f);
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph, GraphHandle*, GraphHandle* DepthPtr) noexcept override {
			auto* OptionParts = OPTION::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			SSRDepthScreen.GraphFilterBlt(*DepthPtr, DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);

			m_Shader.SetPixelCameraMatrix(4, PostPassParts->GetCamViewMat().inverse(), PostPassParts->GetCamProjectionMat().inverse());
			m_Shader.SetPixelCameraMatrix(5, DrawParts->GetShadowDraw()->GetCamViewMatrix(false), DrawParts->GetShadowDraw()->GetCamProjectionMatrix(false));
			// m_Shader.SetPixelCameraMatrix(6, DrawParts->GetShadowDraw()->GetCamViewMatrix(true), DrawParts->GetShadowDraw()->GetCamProjectionMatrix(true));
			SSRScreen.SetDraw_Screen();
			{
				SSRDepthScreen.SetUseTextureToShader(0);
				DrawParts->GetShadowDraw()->GetDepthScreen().SetUseTextureToShader(1);
				// DrawParts->GetShadowDraw()->GetDepthFarScreen().SetUseTextureToShader(2);
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
				// SetUseTextureToShader(2, InvalidID);
			}
			SSRScreen.GraphFilter(DX_GRAPH_FILTER_GAUSS, 16, 300);
			TargetGraph->SetDraw_Screen();
			{
				ColorGraph->DrawGraph(0, 0, true);
				SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(255.f * PostPassParts->GetGodRayPer()));
				SSRScreen.DrawExtendGraph(0, 0, DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
		}
	};
	class PostPassScope : public PostPassBase {
	private:
		ShaderUseClass::ScreenVertex	m_ScreenVertex;					// 頂点データ
		ShaderUseClass		m_Shader;			// シェーダー
	public:
		PostPassScope(void) noexcept {}
		PostPassScope(const PostPassScope&) = delete;
		PostPassScope(PostPassScope&& o) = delete;
		PostPassScope& operator=(const PostPassScope&) = delete;
		PostPassScope& operator=(PostPassScope&& o) = delete;

		virtual ~PostPassScope(void) noexcept {}
	public:
		void Load_Sub(void) noexcept override {
			auto* DrawParts = DXDraw::Instance();
			m_ScreenVertex.SetScreenVertex(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax());
			m_Shader.Init("CommonData/shader/VS_lens.vso", "CommonData/shader/PS_lens.pso");
		}
		void Dispose_Sub(void) noexcept override {
			m_Shader.Dispose();
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph, GraphHandle*, GraphHandle*) noexcept override {
			auto* PostPassParts = PostPassEffect::Instance();
			auto* DrawParts = DXDraw::Instance();
			if (!PostPassParts->GetLensParam().use) { return; }
			// レンズ
			TargetGraph->SetDraw_Screen(false);
			{
				m_Shader.SetPixelDispSize(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax());
				m_Shader.SetPixelParam(3, PostPassParts->GetLensParam().param.at(0), PostPassParts->GetLensParam().param.at(1), PostPassParts->GetLensParam().param.at(2), PostPassParts->GetLensParam().param.at(3));
				ColorGraph->SetUseTextureToShader(0);	// 使用するテクスチャをセット
				m_Shader.Draw(this->m_ScreenVertex);
				SetUseTextureToShader(0, InvalidID);
			}
		}
	};
	class PostPassBlackout : public PostPassBase {
	private:
		ShaderUseClass::ScreenVertex	m_ScreenVertex;					// 頂点データ
		ShaderUseClass		m_Shader;			// シェーダー
	public:
		PostPassBlackout(void) noexcept {}
		PostPassBlackout(const PostPassBlackout&) = delete;
		PostPassBlackout(PostPassBlackout&& o) = delete;
		PostPassBlackout& operator=(const PostPassBlackout&) = delete;
		PostPassBlackout& operator=(PostPassBlackout&& o) = delete;

		virtual ~PostPassBlackout(void) noexcept {}
	public:
		void Load_Sub(void) noexcept override {
			auto* DrawParts = DXDraw::Instance();
			m_ScreenVertex.SetScreenVertex(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax());
			m_Shader.Init("CommonData/shader/VS_BlackOut.vso", "CommonData/shader/PS_BlackOut.pso");
		}
		void Dispose_Sub(void) noexcept override {
			m_Shader.Dispose();
		}
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph, GraphHandle*, GraphHandle*) noexcept override {
			auto* PostPassParts = PostPassEffect::Instance();
			auto* DrawParts = DXDraw::Instance();
			if (!PostPassParts->GetBlackoutParam().use) { return; }
			// レンズ
			TargetGraph->SetDraw_Screen(false);
			{
				m_Shader.SetPixelDispSize(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax());
				m_Shader.SetPixelParam(3, PostPassParts->GetBlackoutParam().param.at(0), PostPassParts->GetBlackoutParam().param.at(1), PostPassParts->GetBlackoutParam().param.at(2), PostPassParts->GetBlackoutParam().param.at(3));
				ColorGraph->SetUseTextureToShader(0);	// 使用するテクスチャをセット
				m_Shader.Draw(this->m_ScreenVertex);
				SetUseTextureToShader(0, InvalidID);
			}
		}
	};
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	const PostPassEffect* SingletonBase<PostPassEffect>::m_Singleton = nullptr;
	// 
	PostPassEffect::PostPassEffect(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		auto Prev = GetCreateDrawValidGraphZBufferBitDepth();
		SetCreateDrawValidGraphZBufferBitDepth(24);
		BufferScreen.Make(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), true);
		ColorScreen.Make(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), false);
		SetCreateDrawValidGraphZBufferBitDepth(Prev);
		// ポストエフェクト
		m_PostPass.emplace_back(std::make_unique<PostPassBloom>());
		m_PostPass.emplace_back(std::make_unique<PostPassDoF>());
		m_PostPass.emplace_back(std::make_unique<PostPassSSR>());
		m_PostPass.emplace_back(std::make_unique<PostPassSSAO>());
		m_PostPass.emplace_back(std::make_unique<PostPassGodRay>());
		m_PostPass.emplace_back(std::make_unique<PostPassDistortion>());
		m_PostPass.emplace_back(std::make_unique<PostPassAberration>());
		m_PostPass.emplace_back(std::make_unique<PostPassMotionBlur>());
		m_PostPass.emplace_back(std::make_unique<PostPassVignette>());
		m_PostPass.emplace_back(std::make_unique<PostPassCornerBlur>());
		m_PostPass.emplace_back(std::make_unique<PostPassFXAA>());
		m_PostPass.emplace_back(std::make_unique<PostPassScope>());
		m_PostPass.emplace_back(std::make_unique<PostPassBlackout>());
	}
	PostPassEffect::~PostPassEffect(void) noexcept {
		// Gバッファ
		if (!m_IsActiveGBuffer) {
			DisposeGBuffer();
		}
		// ポストエフェクト
		for (auto& P : m_PostPass) {
			P.reset();
		}
		m_PostPass.clear();
	}
	void PostPassEffect::Init(void) noexcept {
		for (auto& P : m_PostPass) {
			P->Init();
		}
		// Gバッファ
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
			P->UpdateActive(P->IsActive());
		}
	}
	void PostPassEffect::SetCamMat(const Camera3DInfo& camInfo) noexcept {
		m_CamInfo = camInfo;
		m_CamViewMat = m_CamInfo.GetViewMatrix();
		m_CamProjectionMat = m_CamInfo.GetProjectionMatrix();
	}
	void PostPassEffect::ResetBuffer(void) noexcept {
		if (m_IsActiveGBuffer) {
			// リセット替わり
			ColorScreen.SetDraw_Screen();
			NormalScreen.SetDraw_Screen();
			NormalScreen.FillGraph(128, 128, 255);
			DepthScreen.SetDraw_Screen();
			DepthScreen.FillGraph(255, 0, 0);
		}
	}
	void PostPassEffect::DrawGBuffer(float near_len, float far_len, std::function<void()> done) noexcept {
		// カラーバッファを描画対象0に、法線バッファを描画対象1に設定
		BufferScreen.SetRenderTargetToShader(0);
		if (m_IsActiveGBuffer) {
			NormalScreen.SetRenderTargetToShader(1);
			DepthScreen.SetRenderTargetToShader(2);
		}
		ClearDrawScreenZBuffer();
		m_CamInfo.FlipCamInfo();
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
	void PostPassEffect::DrawPostProcess(void) noexcept {
		BufferScreen.SetDraw_Screen(false);
		// 色味補正
		BufferScreen.GraphFilter(DX_GRAPH_FILTER_LEVEL, InColorPerMin, InColorPerMax, static_cast<int>(InColorGamma * 100), 0, 255);
		// ポストパスエフェクトのbufに描画
		if (m_IsActiveGBuffer) {
			for (auto& P : m_PostPass) {
				ColorScreen.GraphFilterBlt(BufferScreen, DX_GRAPH_FILTER_DOWN_SCALE, 1);
				P->SetEffect(&BufferScreen, &ColorScreen, &NormalScreen, &DepthScreen);
			}
		}
	}
	void PostPassEffect::ResetAllBuffer(void) noexcept {
		bool ActiveGBuffer = false;
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
			P->UpdateActive(false);
		}
	}
	// 
	void PostPassEffect::LoadGBuffer(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		auto Prev = GetCreateDrawValidGraphZBufferBitDepth();
		SetCreateDrawValidGraphZBufferBitDepth(24);
		NormalScreen.Make(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax(), false);
		DepthScreen.MakeDepth(DrawParts->GetScreenXMax(), DrawParts->GetScreenYMax());
		SetCreateDrawValidGraphZBufferBitDepth(Prev);
	}
	void PostPassEffect::DisposeGBuffer(void) noexcept {
		NormalScreen.Dispose();
		DepthScreen.Dispose();
	}
};
