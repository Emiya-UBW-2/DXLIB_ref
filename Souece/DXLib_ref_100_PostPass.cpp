#include "DXLib_ref_100_PostPass.hpp"

namespace DXLibRef {
	// 
	const PostPassScreenBufferPool* SingletonBase<PostPassScreenBufferPool>::m_Singleton = nullptr;
	const PostPassEffect* SingletonBase<PostPassEffect>::m_Singleton = nullptr;
	// --------------------------------------------------------------------------------------------------
	// ポストプロセスエフェクト
	// --------------------------------------------------------------------------------------------------
	// 継承クラス
	class PostPassSSAO : public PostPassBase {
		static const int EXTEND = 4;
	private:
		ShaderController				m_ShaderSSAO;		// シェーダー
		ShaderController				m_ShaderBlur;		// シェーダー
	protected:
		void		Load_Sub(void) noexcept override {
			this->m_ShaderSSAO.Init("CommonData/shader/VS_SSAO.vso", "CommonData/shader/PS_SSAO.pso");
			this->m_ShaderBlur.Init("CommonData/shader/VS_SSAO.vso", "CommonData/shader/PS_BilateralBlur.pso");
		}
		void		Dispose_Sub(void) noexcept override {
			this->m_ShaderSSAO.Dispose();
			this->m_ShaderBlur.Dispose();
		}
		bool		IsActive_Sub(void) noexcept override {
			auto* OptionParts = OptionManager::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::SSAO);
		}
		void		SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph, GraphHandle* NormalPtr, GraphHandle* DepthPtr) noexcept override {
			auto* WindowSizeParts = WindowSizeControl::Instance();
			auto* CameraParts = Camera3D::Instance();
			int xsize = WindowSizeParts->GetScreenXMax() / EXTEND;
			int ysize = WindowSizeParts->GetScreenYMax() / EXTEND;

			const GraphHandle* pScreenBuffer = PostPassScreenBufferPool::Instance()->PopBlankScreen(xsize, ysize, true, false, 24);
			const GraphHandle* pScreenBuffer2 = PostPassScreenBufferPool::Instance()->PopBlankScreen(xsize, ysize, true, false, 24);
			const GraphHandle* pColorScreen = PostPassScreenBufferPool::Instance()->PopBlankScreen(xsize, ysize, true);
			const GraphHandle* pNormalScreen = PostPassScreenBufferPool::Instance()->PopBlankScreen(xsize, ysize, true);
			const GraphHandle* pDepthScreen = PostPassScreenBufferPool::Instance()->PopBlankScreen(xsize, ysize, true, true);

			pColorScreen->GraphFilterBlt(*ColorGraph, DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			pNormalScreen->GraphFilterBlt(*NormalPtr, DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			pDepthScreen->GraphFilterBlt(*DepthPtr, DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			pScreenBuffer->SetDraw_Screen();

			// シェーダーを適用
			pScreenBuffer2->SetDraw_Screen(false);
			{
				pColorScreen->SetUseTextureToShader(0);
				pNormalScreen->SetUseTextureToShader(1);
				pDepthScreen->SetUseTextureToShader(2);
				this->m_ShaderSSAO.SetPixelDispSize(xsize, ysize);
				this->m_ShaderSSAO.SetPixelParam(3, 0.0f, Scale3DRate, std::tan(CameraParts->GetMainCamera().GetCamFov() / 2.f), 0.f);

				this->m_ShaderSSAO.Draw();

				SetUseTextureToShader(0, InvalidID);
				SetUseTextureToShader(1, InvalidID);
				SetUseTextureToShader(2, InvalidID);
			}
			// ぼかしを入れる
			pScreenBuffer->SetDraw_Screen(false);
			{
				pScreenBuffer2->SetUseTextureToShader(0);	// 使用するテクスチャをセット

				this->m_ShaderBlur.SetPixelDispSize(xsize, ysize);
				this->m_ShaderBlur.Draw();

				SetUseTextureToShader(0, InvalidID);
			}
			// 
			TargetGraph->SetDraw_Screen(false);
			{
				TargetGraph->DrawGraph(0, 0, true);
				SetDrawBlendMode(DX_BLENDMODE_MULA, 255);
				pScreenBuffer->DrawExtendGraph(0, 0, WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				// pScreenBuffer2->DrawExtendGraph(0, 0, WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), true);
			}
			PostPassScreenBufferPool::Instance()->ResetUseCount(xsize, ysize, true, false, 24);
			PostPassScreenBufferPool::Instance()->ResetUseCount(xsize, ysize, true, false, 24);
			PostPassScreenBufferPool::Instance()->ResetUseCount(xsize, ysize, true);
			PostPassScreenBufferPool::Instance()->ResetUseCount(xsize, ysize, true);
			PostPassScreenBufferPool::Instance()->ResetUseCount(xsize, ysize, true, true);
		}
	};
	class PostPassSSR : public PostPassBase {
		static const int RayInterval = 200;// レイの分割間隔
		const float DepthThreshold = 17.f;
		static const int EXTEND = 4;
	private:
		GraphHandle						m_bkScreen2;	// ブレンド
		ShaderController::ScreenVertex	m_ScreenVertex;	// 頂点データ
		ShaderController				m_Shader;		// シェーダー
	public:
		PostPassSSR(void) noexcept {}
		PostPassSSR(const PostPassSSR&) = delete;
		PostPassSSR(PostPassSSR&&) = delete;
		PostPassSSR& operator=(const PostPassSSR&) = delete;
		PostPassSSR& operator=(PostPassSSR&&) = delete;

		virtual ~PostPassSSR(void) noexcept {}
	protected:
		void		Load_Sub(void) noexcept override {
			auto* WindowSizeParts = WindowSizeControl::Instance();
			int xsize = WindowSizeParts->GetScreenXMax() / EXTEND;
			int ysize = WindowSizeParts->GetScreenYMax() / EXTEND;
			{
				this->m_bkScreen2.Make(xsize, ysize, false);
				this->m_bkScreen2.SetDraw_Screen(false);
				this->m_bkScreen2.FillGraph(0, 0, 0);
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
			this->m_Shader.Init("CommonData/shader/VS_SSR.vso", "CommonData/shader/PS_SSR.pso");
		}
		void		Dispose_Sub(void) noexcept override {
			this->m_bkScreen2.Dispose();
			this->m_Shader.Dispose();
		}
		bool		IsActive_Sub(void) noexcept override {
			auto* OptionParts = OptionManager::Instance();
			return OptionParts->GetParamInt(EnumSaveParam::Reflection) > 0;
		}
		void		SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph, GraphHandle* NormalPtr, GraphHandle* DepthPtr) noexcept override {
			auto* OptionParts = OptionManager::Instance();
			auto* WindowSizeParts = WindowSizeControl::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			auto* CameraParts = Camera3D::Instance();
			int xsize = WindowSizeParts->GetScreenXMax() / EXTEND;
			int ysize = WindowSizeParts->GetScreenYMax() / EXTEND;

			const GraphHandle* pColorScreen = PostPassScreenBufferPool::Instance()->PopBlankScreen(xsize, ysize, true);
			const GraphHandle* pNormalScreen = PostPassScreenBufferPool::Instance()->PopBlankScreen(xsize, ysize, true);
			const GraphHandle* pDepthScreen = PostPassScreenBufferPool::Instance()->PopBlankScreen(xsize, ysize, true, true);
			const GraphHandle* pScreenBuffer = PostPassScreenBufferPool::Instance()->PopBlankScreen(xsize, ysize, true);

			pColorScreen->GraphFilterBlt(*ColorGraph, DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			pNormalScreen->GraphFilterBlt(*NormalPtr, DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			pDepthScreen->GraphFilterBlt(*DepthPtr, DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			pScreenBuffer->SetDraw_Screen();
			{
				pColorScreen->SetUseTextureToShader(0);
				pNormalScreen->SetUseTextureToShader(1);
				pDepthScreen->SetUseTextureToShader(2);
				if (OptionParts->GetParamBoolean(EnumProjectSettingParam::CubeMap)) {
					PostPassParts->GetCubeMapTex().SetUseTextureToShader(3);
				}
				else {
					this->m_bkScreen2.SetUseTextureToShader(3);
				}
				this->m_bkScreen2.SetUseTextureToShader(4);
				this->m_ScreenVertex.SetScreenVertex(xsize, ysize);
				this->m_Shader.SetPixelParam(3, static_cast<float>(RayInterval), Scale3DRate, std::tan(CameraParts->GetMainCamera().GetCamFov() / 2.f), DepthThreshold);
				this->m_Shader.SetPixelCameraMatrix(4, PostPassParts->GetCamViewMat(), PostPassParts->GetCamProjectionMat());
				this->m_Shader.SetPixelParam(5, static_cast<float>(OptionParts->GetParamInt(EnumSaveParam::Reflection)), OptionParts->GetParamBoolean(EnumProjectSettingParam::CubeMap) ? 1.f : 0.f, 0.f, 0.f);
				{
					this->m_Shader.Draw(this->m_ScreenVertex);
				}
				SetUseTextureToShader(0, InvalidID);
				SetUseTextureToShader(1, InvalidID);
				SetUseTextureToShader(2, InvalidID);
				SetUseTextureToShader(3, InvalidID);
				SetUseTextureToShader(4, InvalidID);
			}
			pScreenBuffer->GraphFilter(DX_GRAPH_FILTER_GAUSS, 8, 200);
			TargetGraph->SetDraw_Screen(false);
			{
				TargetGraph->DrawGraph(0, 0, true);
				pScreenBuffer->DrawExtendGraph(0, 0, WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), true);
			}
			PostPassScreenBufferPool::Instance()->ResetUseCount(xsize, ysize, true);
			PostPassScreenBufferPool::Instance()->ResetUseCount(xsize, ysize, true);
			PostPassScreenBufferPool::Instance()->ResetUseCount(xsize, ysize, true, true);
			PostPassScreenBufferPool::Instance()->ResetUseCount(xsize, ysize, true);

		}
	};
	class PostPassDoF : public PostPassBase {
	private:
		ShaderController				m_Shader;			// シェーダー
	public:
		PostPassDoF(void) noexcept {}
		PostPassDoF(const PostPassDoF&) = delete;
		PostPassDoF(PostPassDoF&&) = delete;
		PostPassDoF& operator=(const PostPassDoF&) = delete;
		PostPassDoF& operator=(PostPassDoF&&) = delete;

		virtual ~PostPassDoF(void) noexcept {}
	protected:
		void		Load_Sub(void) noexcept override {
			this->m_Shader.Init("CommonData/shader/VS_DoF.vso", "CommonData/shader/PS_DoF.pso");
		}
		void		Dispose_Sub(void) noexcept override {
			this->m_Shader.Dispose();
		}
		bool		IsActive_Sub(void) noexcept override {
			auto* OptionParts = OptionManager::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::DoF);
		}
		void		SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph, GraphHandle*, GraphHandle* DepthPtr) noexcept override {
			auto* WindowSizeParts = WindowSizeControl::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			int xsize = WindowSizeParts->GetScreenXMax();
			int ysize = WindowSizeParts->GetScreenYMax();
			const GraphHandle* pNearScreen = PostPassScreenBufferPool::Instance()->PopBlankScreen(xsize, ysize, true);
			const GraphHandle* pFarScreen = PostPassScreenBufferPool::Instance()->PopBlankScreen(xsize, ysize, true);

			pNearScreen->GraphFilterBlt(*TargetGraph, DX_GRAPH_FILTER_GAUSS, 8, 2000);
			pFarScreen->GraphFilterBlt(*TargetGraph, DX_GRAPH_FILTER_GAUSS, 8, 20);
			TargetGraph->SetDraw_Screen();
			{
				ColorGraph->SetUseTextureToShader(0);
				pNearScreen->SetUseTextureToShader(1);
				pFarScreen->SetUseTextureToShader(2);
				DepthPtr->SetUseTextureToShader(3);
				this->m_Shader.SetPixelDispSize(xsize, ysize);
				this->m_Shader.SetPixelParam(3, PostPassParts->Get_near_DoF(), PostPassParts->Get_far_DoF(), PostPassParts->Get_near_DoFMax(), PostPassParts->Get_far_DoFMin());
				{
					this->m_Shader.Draw();
				}
				SetUseTextureToShader(0, InvalidID);
				SetUseTextureToShader(1, InvalidID);
				SetUseTextureToShader(2, InvalidID);
				SetUseTextureToShader(3, InvalidID);
			}

			PostPassScreenBufferPool::Instance()->ResetUseCount(xsize, ysize, true);
			PostPassScreenBufferPool::Instance()->ResetUseCount(xsize, ysize, true);
		}
	};
	class PostPassBloom : public PostPassBase {
		static const int EXTEND = 4;
	public:
		PostPassBloom(void) noexcept {}
		PostPassBloom(const PostPassBloom&) = delete;
		PostPassBloom(PostPassBloom&&) = delete;
		PostPassBloom& operator=(const PostPassBloom&) = delete;
		PostPassBloom& operator=(PostPassBloom&&) = delete;

		virtual ~PostPassBloom(void) noexcept {}
	protected:
		bool		IsActive_Sub(void) noexcept override {
			auto* OptionParts = OptionManager::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::bloom);
		}
		void		SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*, GraphHandle*, GraphHandle*) noexcept override {
			auto* WindowSizeParts = WindowSizeControl::Instance();
			int xsize = WindowSizeParts->GetScreenXMax();
			int ysize = WindowSizeParts->GetScreenYMax();
			const GraphHandle* pScreenBuffer = PostPassScreenBufferPool::Instance()->PopBlankScreen(xsize, ysize, true);
			const GraphHandle* pGaussScreen = PostPassScreenBufferPool::Instance()->PopBlankScreen(WindowSizeParts->GetScreenXMax() / EXTEND, WindowSizeParts->GetScreenYMax() / EXTEND, true);

			pScreenBuffer->GraphFilterBlt(*TargetGraph, DX_GRAPH_FILTER_TWO_COLOR, 250, Black, 255, Gray50, 255);
			pGaussScreen->GraphFilterBlt(*pScreenBuffer, DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			pGaussScreen->GraphFilter(DX_GRAPH_FILTER_GAUSS, 8, 1000);
			TargetGraph->SetDraw_Screen(false);
			{
				auto Prev = GetDrawMode();
				SetDrawMode(DX_DRAWMODE_BILINEAR);
				SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
				pGaussScreen->DrawExtendGraph(0, 0, xsize, ysize, true);
				pGaussScreen->DrawExtendGraph(0, 0, xsize, ysize, true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				SetDrawMode(Prev);
			}
			PostPassScreenBufferPool::Instance()->ResetUseCount(xsize, ysize, true);
			PostPassScreenBufferPool::Instance()->ResetUseCount(WindowSizeParts->GetScreenXMax() / EXTEND, WindowSizeParts->GetScreenYMax() / EXTEND, true);
		}
	};
	class PostPassAberration : public PostPassBase {
	public:
		PostPassAberration(void) noexcept {}
		PostPassAberration(const PostPassAberration&) = delete;
		PostPassAberration(PostPassAberration&&) = delete;
		PostPassAberration& operator=(const PostPassAberration&) = delete;
		PostPassAberration& operator=(PostPassAberration&&) = delete;

		virtual ~PostPassAberration(void) noexcept {}
	protected:
		bool		IsActive_Sub(void) noexcept override {
			auto* OptionParts = OptionManager::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::ScreenEffect);
		}
		void		SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*, GraphHandle*, GraphHandle*) noexcept override {
			auto* PostPassParts = PostPassEffect::Instance();
			auto* WindowSizeParts = WindowSizeControl::Instance();
			int xsize = WindowSizeParts->GetScreenXMax();
			int ysize = WindowSizeParts->GetScreenYMax();
			const GraphHandle* pScreenRed = PostPassScreenBufferPool::Instance()->PopBlankScreen(xsize, ysize, true);
			const GraphHandle* pScreenGreen = PostPassScreenBufferPool::Instance()->PopBlankScreen(xsize, ysize, true);
			const GraphHandle* pScreenBlue = PostPassScreenBufferPool::Instance()->PopBlankScreen(xsize, ysize, true);

			pScreenRed->SetDraw_Screen(false);
			pScreenRed->FillGraph(0, 0, 0);
			pScreenGreen->SetDraw_Screen(false);
			pScreenGreen->FillGraph(0, 0, 0);
			pScreenBlue->SetDraw_Screen(false);
			pScreenBlue->FillGraph(0, 0, 0);
			pScreenRed->GraphBlend(*TargetGraph, 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_BLEND_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_SRC_A);
			pScreenGreen->GraphBlend(*TargetGraph, 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_BLEND_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_SRC_A);
			pScreenBlue->GraphBlend(*TargetGraph, 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_BLEND_B, DX_RGBA_SELECT_SRC_A);
			TargetGraph->SetDraw_Screen(false);
			{
				TargetGraph->FillGraph(0, 0, 0);
				SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
				pScreenRed->DrawRotaGraph(WindowSizeParts->GetScreenXMax() / 2, WindowSizeParts->GetScreenYMax() / 2, 1.f + 0.005f * PostPassParts->GetAberrationPower(), 0.f, true);
				pScreenGreen->DrawRotaGraph(WindowSizeParts->GetScreenXMax() / 2, WindowSizeParts->GetScreenYMax() / 2, 1.f, 0.f, true);
				pScreenBlue->DrawRotaGraph(WindowSizeParts->GetScreenXMax() / 2, WindowSizeParts->GetScreenYMax() / 2, 1.f - 0.005f * PostPassParts->GetAberrationPower(), 0.f, true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			PostPassScreenBufferPool::Instance()->ResetUseCount(xsize, ysize, true);
			PostPassScreenBufferPool::Instance()->ResetUseCount(xsize, ysize, true);
			PostPassScreenBufferPool::Instance()->ResetUseCount(xsize, ysize, true);
		}
	};
	class PostPassMotionBlur : public PostPassBase {
	private:
		class BlurScreen {
			static const size_t MAX = 5;
		private:
			std::array<GraphHandle, MAX>	m_screen;
			size_t							m_current{ 0 };
			int								m_alpha{ 0 };
			int								m_screenWidth{ 0 }, m_screenHeight{ 0 };
			int								m_offsetX1{ 0 }, m_offsetX2{ 0 }, m_offsetY1{ 0 }, m_offsetY2{ 0 };
			size_t							m_notBlendDraw{ 0 };
		public:
			BlurScreen(void) noexcept {}
			BlurScreen(const BlurScreen&) = delete;
			BlurScreen(BlurScreen&&) = delete;
			BlurScreen& operator=(const BlurScreen&) = delete;
			BlurScreen& operator=(BlurScreen&&) = delete;

			~BlurScreen(void) noexcept {}
		public:
			void		Init(int t_alpha, int t_offsetX1, int t_offsetY1, int t_offsetX2, int t_offsetY2) noexcept {
				auto* WindowSizeParts = WindowSizeControl::Instance();
				for (size_t loop : std::views::iota(static_cast<size_t>(0), MAX)) {
					this->m_screen[loop].Make(WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax());
				}
				this->m_current = 0;
				this->m_alpha = t_alpha;
				this->m_offsetX1 = t_offsetX1;
				this->m_offsetX2 = t_offsetX2;
				this->m_offsetY1 = t_offsetY1;
				this->m_offsetY2 = t_offsetY2;

				this->m_notBlendDraw = 0;
			}
			void		Release(void) noexcept {
				for (size_t loop : std::views::iota(static_cast<size_t>(0), MAX)) {
					this->m_screen[loop].Dispose();
				}
			}
		public:
			auto* PostRenderBlurScreen(std::function<void()> doing) noexcept {
				auto* WindowSizeParts = WindowSizeControl::Instance();
				size_t next = ((this->m_current != 0) ? this->m_current : MAX) - 1;
				this->m_screen[m_current].SetDraw_Screen();
				{
					doing();
					if (++m_notBlendDraw > MAX) {
						int drawMode = GetDrawMode();
						SetDrawMode(DX_DRAWMODE_BILINEAR);
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, this->m_alpha);
						this->m_screen[next].DrawExtendGraph(this->m_offsetX1, this->m_offsetY1, WindowSizeParts->GetScreenXMax() + this->m_offsetX2, WindowSizeParts->GetScreenYMax() + this->m_offsetY2, false);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						SetDrawMode(drawMode);
					}
				}
				auto Cur = this->m_current;
				this->m_current = next;
				return &this->m_screen[Cur];
			}
		};
	private:
		BlurScreen				m_BlurScreen;
	public:
		PostPassMotionBlur(void) noexcept {}
		PostPassMotionBlur(const PostPassMotionBlur&) = delete;
		PostPassMotionBlur(PostPassMotionBlur&&) = delete;
		PostPassMotionBlur& operator=(const PostPassMotionBlur&) = delete;
		PostPassMotionBlur& operator=(PostPassMotionBlur&&) = delete;

		virtual ~PostPassMotionBlur(void) noexcept {}
	protected:
		void		Load_Sub(void) noexcept override {
			this->m_BlurScreen.Init(96, -6, -6, 6, 6);
		}
		void		Dispose_Sub(void) noexcept override {
			this->m_BlurScreen.Release();
		}
		bool		IsActive_Sub(void) noexcept override {
			auto* OptionParts = OptionManager::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::MotionBlur);
		}
		void		SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*, GraphHandle*, GraphHandle*) noexcept override {
			GraphHandle* buf = this->m_BlurScreen.PostRenderBlurScreen([&]() {
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
		GraphHandle		m_bkScreen2;
	public:
		PostPassCornerBlur(void) noexcept {}
		PostPassCornerBlur(const PostPassCornerBlur&) = delete;
		PostPassCornerBlur(PostPassCornerBlur&&) = delete;
		PostPassCornerBlur& operator=(const PostPassCornerBlur&) = delete;
		PostPassCornerBlur& operator=(PostPassCornerBlur&&) = delete;

		virtual ~PostPassCornerBlur(void) noexcept {}
	protected:
		void		Load_Sub(void) noexcept override {
			auto* WindowSizeParts = WindowSizeControl::Instance();
			{
				this->m_bkScreen2.Make(WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), false);
				this->m_bkScreen2.SetDraw_Screen(false);
				{
					this->m_bkScreen2.FillGraph(0, 0, 0);
					int xr = WindowSizeParts->GetScreenXMax() * 60 / 100;
					int yr = WindowSizeParts->GetScreenYMax() * 70 / 100;

					DrawOval(WindowSizeParts->GetScreenXMax() / 2, WindowSizeParts->GetScreenYMax() / 2, xr, yr, White, TRUE);

					int p = 1;
					for (int r = 0; r < 255; r += p) {
						uint8_t c = static_cast<uint8_t>(255 - static_cast<int>(std::powf(static_cast<float>(255 - r) / 255.f, 1.5f) * 255.f));

						DrawOval(WindowSizeParts->GetScreenXMax() / 2, WindowSizeParts->GetScreenYMax() / 2, xr - r / p, yr - r / p, GetColorFix(c, c, c), FALSE, 2);
					}
				}
			}
			GraphHandle::SetDraw_Screen(static_cast<int>(DX_SCREEN_BACK), false);
		}
		void		Dispose_Sub(void) noexcept override {
			this->m_bkScreen2.Dispose();
		}
		bool		IsActive_Sub(void) noexcept override {
			auto* OptionParts = OptionManager::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::ScreenEffect);
		}
		void		SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*, GraphHandle*, GraphHandle*) noexcept override {
			auto* WindowSizeParts = WindowSizeControl::Instance();
			const GraphHandle* pScreenBuffer = PostPassScreenBufferPool::Instance()->PopBlankScreen(WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), true);
			const GraphHandle* pAberrationScreen = PostPassScreenBufferPool::Instance()->PopBlankScreen(WindowSizeParts->GetScreenXMax() / EXTEND, WindowSizeParts->GetScreenYMax() / EXTEND, true);

			pAberrationScreen->GraphFilterBlt(*TargetGraph, DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			pAberrationScreen->GraphFilter(DX_GRAPH_FILTER_GAUSS, 8, 1000);
			pScreenBuffer->GraphBlendBlt(*TargetGraph, this->m_bkScreen2, 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_BLEND_R);
			TargetGraph->SetDraw_Screen(false);
			{
				pAberrationScreen->DrawExtendGraph(0, 0, WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), false);
				pScreenBuffer->DrawGraph(0, 0, true);
			}
			PostPassScreenBufferPool::Instance()->ResetUseCount(WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), true);
			PostPassScreenBufferPool::Instance()->ResetUseCount(WindowSizeParts->GetScreenXMax() / EXTEND, WindowSizeParts->GetScreenYMax() / EXTEND, true);
		}
	};
	class PostPassVignette : public PostPassBase {
	private:
		GraphHandle m_bkScreen;
	public:
		PostPassVignette(void) noexcept {}
		PostPassVignette(const PostPassVignette&) = delete;
		PostPassVignette(PostPassVignette&&) = delete;
		PostPassVignette& operator=(const PostPassVignette&) = delete;
		PostPassVignette& operator=(PostPassVignette&&) = delete;

		virtual ~PostPassVignette(void) noexcept {}
	protected:
		void		Load_Sub(void) noexcept override {
			auto* WindowSizeParts = WindowSizeControl::Instance();
			{
				this->m_bkScreen.Make(WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), false);
				this->m_bkScreen.SetDraw_Screen(true);
				this->m_bkScreen.FillGraph(255, 255, 255);
				{
					int p = 1;
					for (int y = 0; y < 255; y += p) {
						uint8_t c = static_cast<uint8_t>(255 - static_cast<int>(std::powf(static_cast<float>(255 - y) / 255.f, 1.5f) * 64.f));
						DxLib::DrawLine(0, y / p, WindowSizeParts->GetScreenXMax(), y / p, GetColorFix(c, c, c));
					}
					p = 1;
					for (int y = 0; y < 255; y += p) {
						uint8_t c = static_cast<uint8_t>(255 - static_cast<int>(std::powf(static_cast<float>(255 - y) / 255.f, 1.5f) * 128.f));
						DxLib::DrawLine(0, WindowSizeParts->GetScreenYMax() - y / p, WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax() - y / p, GetColorFix(c, c, c));
					}
				}
			}
			GraphHandle::SetDraw_Screen(static_cast<int>(DX_SCREEN_BACK), false);
		}
		void		Dispose_Sub(void) noexcept override {
			this->m_bkScreen.Dispose();
		}
		bool		IsActive_Sub(void) noexcept override {
			auto* OptionParts = OptionManager::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::ScreenEffect);
		}
		void		SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*, GraphHandle*, GraphHandle*) noexcept override {
			auto* WindowSizeParts = WindowSizeControl::Instance();
			const GraphHandle* pScreenBuffer = PostPassScreenBufferPool::Instance()->PopBlankScreen(WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), true);
			pScreenBuffer->GraphBlendBlt(*TargetGraph, this->m_bkScreen, 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_BLEND_R);
			TargetGraph->SetDraw_Screen(false);
			{
				TargetGraph->FillGraph(0, 0, 0);
				pScreenBuffer->DrawGraph(0, 0, true);
			}
			PostPassScreenBufferPool::Instance()->ResetUseCount(WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), true);
		}
	};
	class PostPassDistortion : public PostPassBase {
	public:
		PostPassDistortion(void) noexcept {}
		PostPassDistortion(const PostPassDistortion&) = delete;
		PostPassDistortion(PostPassDistortion&&) = delete;
		PostPassDistortion& operator=(const PostPassDistortion&) = delete;
		PostPassDistortion& operator=(PostPassDistortion&&) = delete;

		virtual ~PostPassDistortion(void) noexcept {}
	private:
		// 画面を歪ませながら描画する関数
		void		DrawCircleScreen(
			int CenterX,			// 円の中心座標X
			int CenterY,			// 円の中心座標Y
			float Radius,			// 円のサイズ
			float Absorption,		// 内側の円に引き込まれるドット数
			const GraphHandle& ScreenHandle// 画面グラフィックハンドル
		) {
			auto* WindowSizeParts = WindowSizeControl::Instance();
			const int CIRCLE_ANGLE_VERTEX_NUM = 16;			// 円周の頂点数
			const int CIRCLE_RADIUS_VERTEX_NUM = 8;			// 半径の頂点数

			float CenterDistance;
			float GraphCenterDistance;
			float AbsorptionDistance;
			float AbsorptionMoveX, AbsorptionMoveY;
			float Angle;
			float Sin, Cos;
			COLOR_U8 DiffuseColor;
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
			for (size_t loop = 0; loop < CIRCLE_ANGLE_VERTEX_NUM; ++loop, Angle += DX_PI_F * 2.0f / CIRCLE_ANGLE_VERTEX_NUM) {
				AngleSinTable[loop] = std::sin(Angle);
				AngleCosTable[loop] = std::cos(Angle);
			}

			// 内側の盛り上がっているように見せる箇所で使用する Cos テーブルを作成する
			Angle = 0.0f;
			for (size_t loop = 0; loop < CIRCLE_RADIUS_VERTEX_NUM; ++loop, Angle += (DX_PI_F / 2.0f) / (CIRCLE_RADIUS_VERTEX_NUM - 1)) {
				InCircleCosTable[loop] = std::cos(Angle);
			}

			// ポリゴン頂点インデックスの準備
			Ind = Index;
			for (int loop = 0; loop < CIRCLE_ANGLE_VERTEX_NUM; ++loop) {
				for (WORD loop2 = 0; loop2 < CIRCLE_RADIUS_VERTEX_NUM - 1; ++loop2, Ind += 6) {
					Ind[0] = static_cast<WORD>(loop * CIRCLE_RADIUS_VERTEX_NUM + loop2);
					Ind[1] = static_cast<WORD>(Ind[0] + 1);
					if (loop == CIRCLE_ANGLE_VERTEX_NUM - 1) {
						Ind[2] = loop2;
						Ind[3] = static_cast<WORD>(loop2 + 1);
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
			for (size_t loop = 0; loop < CIRCLE_ANGLE_VERTEX_NUM; ++loop) {
				// 使用する Sin, Cos の値をセット
				Sin = AngleSinTable[loop];
				Cos = AngleCosTable[loop];

				for (int loop2 = 0; loop2 < CIRCLE_RADIUS_VERTEX_NUM; ++loop2, ++Vert) {
					// 円の中心までの距離を算出
					CenterDistance = Radius;

					// 中心に引き込まれる距離を算出
					AbsorptionDistance = Absorption * static_cast<float>(loop2) / static_cast<float>(CIRCLE_RADIUS_VERTEX_NUM - 1);

					// 中心に向かって移動する距離を算出
					AbsorptionMoveX = Cos * AbsorptionDistance;
					AbsorptionMoveY = Sin * AbsorptionDistance;

					// スクリーン座標の決定
					Vert->pos.x = Cos * CenterDistance + static_cast<float>(CenterX);
					Vert->pos.y = Sin * CenterDistance + static_cast<float>(CenterY);
					Vert->pos.z = 0.0f;

					// テクスチャ座標のセット
					Vert->u = (Vert->pos.x + AbsorptionMoveX) / static_cast<float>(WindowSizeParts->GetScreenXMax());
					Vert->v = (Vert->pos.y + AbsorptionMoveY) / static_cast<float>(WindowSizeParts->GetScreenYMax());

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
			for (size_t loop = 0; loop < CIRCLE_ANGLE_VERTEX_NUM; ++loop) {
				// 使用する Sin, Cos の値をセット
				Sin = AngleSinTable[loop];
				Cos = AngleCosTable[loop];

				for (int loop2 = 0; loop2 < CIRCLE_RADIUS_VERTEX_NUM; ++loop2, ++Vert) {
					// 円の中心までの距離を算出
					CenterDistance = InCircleCosTable[static_cast<size_t>(loop2)] * Radius;

					// 画像座標視点での円の中心までの距離を算出
					GraphCenterDistance = static_cast<float>((CIRCLE_RADIUS_VERTEX_NUM - 1) - loop2) * (Absorption + Radius) / static_cast<float>(CIRCLE_RADIUS_VERTEX_NUM - 1);

					// スクリーン座標の決定
					Vert->pos.x = Cos * CenterDistance + static_cast<float>(CenterX);
					Vert->pos.y = Sin * CenterDistance + static_cast<float>(CenterY);
					Vert->pos.z = 0.0f;

					// テクスチャ座標のセット
					Vert->u = (Cos * GraphCenterDistance + static_cast<float>(CenterX)) / static_cast<float>(WindowSizeParts->GetScreenXMax());
					Vert->v = (Sin * GraphCenterDistance + static_cast<float>(CenterY)) / static_cast<float>(WindowSizeParts->GetScreenYMax());

					// その他のパラメータをセット
					Vert->rhw = 1.0f;
					Vert->dif = DiffuseColor;
				}
			}

			// 中心の盛り上がって見える部分を描画
			DrawPrimitiveIndexed2D(Vertex, sizeof(Vertex) / sizeof(VERTEX2D), Index, sizeof(Index) / sizeof(WORD), DX_PRIMTYPE_TRIANGLELIST, ScreenHandle.get(), FALSE);
		}
	protected:
		bool		IsActive_Sub(void) noexcept override {
			auto* OptionParts = OptionManager::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::ScreenEffect) && OptionParts->GetParamBoolean(EnumProjectSettingParam::Distortion);
		}
		void		SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*, GraphHandle*, GraphHandle*) noexcept override {
			auto* WindowSizeParts = WindowSizeControl::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			const GraphHandle* pScreenBuffer = PostPassScreenBufferPool::Instance()->PopBlankScreen(WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), true);
			pScreenBuffer->SetDraw_Screen();
			{
				TargetGraph->DrawGraph(0, 0, true);
			}
			// TargetGraph->GraphFilterBlt(*pScreenBuffer, DX_GRAPH_FILTER_DOWN_SCALE, 1);
			TargetGraph->SetDraw_Screen();
			{
				// 画面を歪ませて描画
				DrawCircleScreen(
					WindowSizeParts->GetScreenXMax() / 2, WindowSizeParts->GetScreenYMax() / 2,
					static_cast<float>(WindowSizeParts->GetScreenXMax() * 2 / 3), static_cast<float>(WindowSizeParts->GetScreenY(static_cast<int>(PostPassParts->GetDistortionPer()))), *pScreenBuffer);
			}
			PostPassScreenBufferPool::Instance()->ResetUseCount(WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), true);
		}
	};
	class PostPassFXAA : public PostPassBase {
	private:
		ShaderController				m_Shader;
	protected:
		void		Load_Sub(void) noexcept override {
			this->m_Shader.Init("CommonData/shader/VS_FXAA.vso", "CommonData/shader/PS_FXAA.pso");
		}
		void		Dispose_Sub(void) noexcept override {
			this->m_Shader.Dispose();
		}
		bool		IsActive_Sub(void) noexcept override {
			auto* OptionParts = OptionManager::Instance();
			return OptionParts->GetParamBoolean(EnumSaveParam::AA);
		}
		void		SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph, GraphHandle*, GraphHandle*) noexcept override {
			auto* WindowSizeParts = WindowSizeControl::Instance();
			TargetGraph->SetDraw_Screen();
			{
				ColorGraph->SetUseTextureToShader(0);
				this->m_Shader.SetPixelDispSize(WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax());
				{
					this->m_Shader.Draw();
				}
				SetUseTextureToShader(0, InvalidID);
			}
		}
	};
	class PostPassGodRay : public PostPassBase {
		static const int EXTEND = 8;
	private:
		GraphHandle						m_Min;			// 描画スクリーン
		SoftImageHandle					m_SoftImage;
		int								GodRayRed = InvalidID;
		float							GodRayTime = 0.f;
		ShaderController::ScreenVertex	m_ScreenVertex;					// 頂点データ
		ShaderController				m_Shader;			// シェーダー
		float							range = 1.f;
	public:
		PostPassGodRay(void) noexcept {}
		PostPassGodRay(const PostPassGodRay&) = delete;
		PostPassGodRay(PostPassGodRay&&) = delete;
		PostPassGodRay& operator=(const PostPassGodRay&) = delete;
		PostPassGodRay& operator=(PostPassGodRay&&) = delete;

		virtual ~PostPassGodRay(void) noexcept {}
	protected:
		void		Load_Sub(void) noexcept override {
			this->m_Shader.Init("CommonData/shader/VS_GodRay.vso", "CommonData/shader/PS_GodRay.pso");
			this->m_Min.Make(1, 1, true);
			this->m_SoftImage.Make(1, 1);
		}
		void		Dispose_Sub(void) noexcept override {
			this->m_Shader.Dispose();
			this->m_Min.Dispose();
			this->m_SoftImage.Dispose();
		}
		bool		IsActive_Sub(void) noexcept override {
			auto* PostPassParts = PostPassEffect::Instance();
			auto* OptionParts = OptionManager::Instance();
			return (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0) && OptionParts->GetParamBoolean(EnumSaveParam::GodRay) && (PostPassParts->GetGodRayPer() > 0.f);
		}
		void		SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph, GraphHandle*, GraphHandle* DepthPtr) noexcept override {
			auto* OptionParts = OptionManager::Instance();
			auto* WindowSizeParts = WindowSizeControl::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			auto* CameraParts = Camera3D::Instance();
			auto* DXLib_refParts = DXLib_ref::Instance();
			int xsize = WindowSizeParts->GetScreenXMax() / EXTEND;
			int ysize = WindowSizeParts->GetScreenYMax() / EXTEND;

			const GraphHandle* pDepthScreen = PostPassScreenBufferPool::Instance()->PopBlankScreen(xsize, ysize, true, false);
			const GraphHandle* pScreenBuffer = PostPassScreenBufferPool::Instance()->PopBlankScreen(xsize, ysize, true);

			pDepthScreen->GraphFilterBlt(*DepthPtr, DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);

			this->m_ScreenVertex.SetScreenVertex(xsize, ysize);
			this->m_Shader.SetPixelCameraMatrix(4, PostPassParts->GetCamViewMat().inverse(), PostPassParts->GetCamProjectionMat().inverse());
			this->m_Shader.SetPixelCameraMatrix(5, PostPassParts->GetShadowDraw()->GetCamViewMatrix(false), PostPassParts->GetShadowDraw()->GetCamProjectionMatrix(false));
			this->m_Shader.SetPixelCameraMatrix(6, PostPassParts->GetShadowDraw()->GetCamViewMatrix(true), PostPassParts->GetShadowDraw()->GetCamProjectionMatrix(true));
			pScreenBuffer->SetDraw_Screen();
			{
				pDepthScreen->SetUseTextureToShader(0);
				PostPassParts->GetShadowDraw()->GetDepthScreen().SetUseTextureToShader(1);
				PostPassParts->GetShadowDraw()->GetDepthFarScreen().SetUseTextureToShader(2);
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
					this->m_Shader.SetPixelParam(3, Power, 0.f, std::tan(CameraParts->GetMainCamera().GetCamFov() / 2.f), 0.f);
					this->m_Shader.Draw(this->m_ScreenVertex);
				}
				SetUseTextureToShader(0, InvalidID);
				SetUseTextureToShader(1, InvalidID);
				SetUseTextureToShader(2, InvalidID);
			}
			GodRayTime += DXLib_refParts->GetDeltaTime();
			if (GodRayTime > 1.f) {
				GodRayTime -= 1.f;
				this->m_Min.SetDraw_Screen();
				auto Prev = GetDrawMode();
				SetDrawMode(DX_DRAWMODE_BILINEAR);
				pScreenBuffer->DrawExtendGraph(0, 0, 1, 1, true);
				SetDrawMode(Prev);
				this->m_SoftImage.GetDrawScreen(0, 0, 1, 1);
				this->m_SoftImage.GetPixel(0, 0, &GodRayRed, nullptr, nullptr, nullptr);
			}
			PostPassParts->SetGodRayPerByPostPass(1.f - std::clamp(GodRayRed / 128.f, 0.f, 1.f));

			pScreenBuffer->GraphFilter(DX_GRAPH_FILTER_GAUSS, 8, 300);
			TargetGraph->SetDraw_Screen();
			{
				ColorGraph->DrawGraph(0, 0, true);
				SetDrawBlendMode(DX_BLENDMODE_ADD, (int)(255.f * PostPassParts->GetGodRayPerRet()));
				pScreenBuffer->DrawExtendGraph(0, 0, WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			PostPassScreenBufferPool::Instance()->ResetUseCount(xsize, ysize, true, false);
			PostPassScreenBufferPool::Instance()->ResetUseCount(xsize, ysize, true);
		}
	};
	class PostPassScope : public PostPassBase {
	private:
		ShaderController				m_Shader;			// シェーダー
	public:
		PostPassScope(void) noexcept {}
		PostPassScope(const PostPassScope&) = delete;
		PostPassScope(PostPassScope&&) = delete;
		PostPassScope& operator=(const PostPassScope&) = delete;
		PostPassScope& operator=(PostPassScope&&) = delete;

		virtual ~PostPassScope(void) noexcept {}
	protected:
		void		Load_Sub(void) noexcept override {
			this->m_Shader.Init("CommonData/shader/VS_lens.vso", "CommonData/shader/PS_lens.pso");
		}
		void		Dispose_Sub(void) noexcept override {
			this->m_Shader.Dispose();
		}
		void		SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph, GraphHandle*, GraphHandle*) noexcept override {
			auto* PostPassParts = PostPassEffect::Instance();
			auto* WindowSizeParts = WindowSizeControl::Instance();
			if (!PostPassParts->is_lens()) { return; }
			// レンズ
			TargetGraph->SetDraw_Screen(false);
			{
				this->m_Shader.SetPixelDispSize(WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax());
				this->m_Shader.SetPixelParam(3, PostPassParts->zoom_xpos(), PostPassParts->zoom_ypos(), PostPassParts->zoom_size(), PostPassParts->zoom_lens());
				ColorGraph->SetUseTextureToShader(0);	// 使用するテクスチャをセット
				this->m_Shader.Draw();
				SetUseTextureToShader(0, InvalidID);
			}
		}
	};
	class PostPassBlackout : public PostPassBase {
	private:
		ShaderController				m_Shader;			// シェーダー
	public:
		PostPassBlackout(void) noexcept {}
		PostPassBlackout(const PostPassBlackout&) = delete;
		PostPassBlackout(PostPassBlackout&&) = delete;
		PostPassBlackout& operator=(const PostPassBlackout&) = delete;
		PostPassBlackout& operator=(PostPassBlackout&&) = delete;

		virtual ~PostPassBlackout(void) noexcept {}
	protected:
		void		Load_Sub(void) noexcept override {
			this->m_Shader.Init("CommonData/shader/VS_BlackOut.vso", "CommonData/shader/PS_BlackOut.pso");
		}
		void		Dispose_Sub(void) noexcept override {
			this->m_Shader.Dispose();
		}
		void		SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle* ColorGraph, GraphHandle*, GraphHandle*) noexcept override {
			auto* PostPassParts = PostPassEffect::Instance();
			auto* WindowSizeParts = WindowSizeControl::Instance();
			if (!PostPassParts->is_Blackout()) { return; }
			// レンズ
			TargetGraph->SetDraw_Screen(false);
			{
				this->m_Shader.SetPixelDispSize(WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax());
				this->m_Shader.SetPixelParam(3, PostPassParts->GetBlackoutPer(), 0.f, 0.f, 0.f);
				ColorGraph->SetUseTextureToShader(0);	// 使用するテクスチャをセット
				this->m_Shader.Draw();
				SetUseTextureToShader(0, InvalidID);
			}
		}
	};
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	void		ShadowDraw::SetupCam(Vector3DX Center, float scale) const noexcept {
		ClearDrawScreen();
		SetupCamera_Ortho(30.f * scale * Scale3DRate);		// カメラのタイプを正射影タイプにセット、描画範囲も指定
		SetCameraNearFar(0.05f * scale * Scale3DRate, 60.f * scale * Scale3DRate);		// 描画する奥行き範囲をセット
		// カメラの位置と注視点はステージ全体が見渡せる位置
		auto Vec = this->m_ShadowVec;
		if (this->m_ShadowVec.x == 0.f && this->m_ShadowVec.z == 0.f) {
			Vec.z = (0.1f);
		}
		SetCameraPositionAndTarget_UpVecY((Center - Vec.normalized() * (30.f * scale * Scale3DRate)).get(), Center.get());
	}
	void		ShadowDraw::Update(std::function<void()> Shadowdoing, Vector3DX Center, float Scale) noexcept {
		this->m_Scale = Scale;
		// 影用の深度記録画像の準備を行う
		DepthBaseScreenHandle.SetRenderTargetToShader(0);
		SetRenderTargetToShader(1, InvalidID);
		DepthScreenHandle.SetRenderTargetToShader(2);
		{
			SetupCam(Center, this->m_Scale);
			this->m_CamViewMatrix[0] = GetCameraViewMatrix();
			this->m_CamProjectionMatrix[0] = GetCameraProjectionMatrix();
			Shadowdoing();
		}
		SetRenderTargetToShader(0, InvalidID);
		SetRenderTargetToShader(1, InvalidID);
		SetRenderTargetToShader(2, InvalidID);
	}
	void		ShadowDraw::UpdateFar(std::function<void()> Shadowdoing, Vector3DX Center, float Scale) noexcept {
		this->m_ScaleFar = Scale;
		// 影用の深度記録画像の準備を行う
		DepthBaseScreenHandle.SetRenderTargetToShader(0);
		SetRenderTargetToShader(1, InvalidID);
		DepthFarScreenHandle.SetRenderTargetToShader(2);
		{
			SetupCam(Center, this->m_ScaleFar);
			this->m_CamViewMatrix[1] = GetCameraViewMatrix();
			this->m_CamProjectionMatrix[1] = GetCameraProjectionMatrix();
			Shadowdoing();
		}
		SetRenderTargetToShader(0, InvalidID);
		SetRenderTargetToShader(1, InvalidID);
		SetRenderTargetToShader(2, InvalidID);
	}
	void		ShadowDraw::SetDraw(std::function<void()> doing_rigid, std::function<void()> doing, Camera3DInfo tmp_cam) noexcept {
		auto* OptionParts = OptionManager::Instance();
		BaseShadowHandle.SetUseTextureToShader(0);				// 影用深度記録画像をテクスチャにセット
		DepthScreenHandle.SetUseTextureToShader(1);
		DepthFarScreenHandle.SetUseTextureToShader(2);
		// 影の結果を出力
		tmp_cam.SetCamInfo(tmp_cam.GetCamFov(), 0.01f * Scale3DRate, 30.f * Scale3DRate);
		BaseShadowHandle.SetDraw_Screen();
		tmp_cam.FlipCamInfo();
		{
			this->m_Shader.SetPixelParam(3, static_cast<float>(OptionParts->GetParamInt(EnumSaveParam::shadow)), this->m_Scale * 180.f, 0.f, 0.f);
			this->m_Shader.SetVertexCameraMatrix(4, this->m_CamViewMatrix[0], this->m_CamProjectionMatrix[0]);
			this->m_Shader.SetVertexCameraMatrix(5, this->m_CamViewMatrix[1], this->m_CamProjectionMatrix[1]);
			this->m_Shader.Draw_lamda(doing);
			this->m_ShaderRigid.SetPixelParam(3, static_cast<float>(OptionParts->GetParamInt(EnumSaveParam::shadow)), this->m_Scale * 180.f, 0.f, 0.f);
			this->m_ShaderRigid.SetVertexCameraMatrix(4, this->m_CamViewMatrix[0], this->m_CamProjectionMatrix[0]);
			this->m_ShaderRigid.SetVertexCameraMatrix(5, this->m_CamViewMatrix[1], this->m_CamProjectionMatrix[1]);
			this->m_ShaderRigid.Draw_lamda(doing_rigid);
		}
		SetUseTextureToShader(1, InvalidID);				// 使用テクスチャの設定を解除
		SetUseTextureToShader(2, InvalidID);				// 使用テクスチャの設定を解除
		// 後処理
		BaseShadowHandle.GraphBlend(DepthBaseScreenHandle, 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
			DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_R);
	}
	void		ShadowDraw::Draw(void) noexcept {
		auto* WindowSizeParts = WindowSizeControl::Instance();
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
		BaseShadowHandle.DrawExtendGraph(0, 0, WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		// DepthScreenHandle.DrawExtendGraph(0, 0,1080,1080, true);
	}
	void		ShadowDraw::Dispose(void) noexcept {
		BaseShadowHandle.Dispose();
		DepthBaseScreenHandle.Dispose();
		DepthScreenHandle.Dispose();
		DepthFarScreenHandle.Dispose();
		this->m_Shader.Dispose();
		this->m_ShaderRigid.Dispose();
	}
	void		ShadowDraw::SetActive(void) noexcept {
		auto* WindowSizeParts = WindowSizeControl::Instance();
		auto* OptionParts = OptionManager::Instance();
		this->m_PrevShadow = OptionParts->GetParamInt(EnumSaveParam::shadow) > 0;
		BaseShadowHandle.Make(WindowSizeParts->GetScreenXMax() / EXTEND, WindowSizeParts->GetScreenYMax() / EXTEND, TRUE);
		int size = 2 << 10;
		DepthBaseScreenHandle.Make(size, size, FALSE);			// 深度バッファ用の作成
		DepthScreenHandle.MakeDepth(size, size);					// 深度バッファの作成
		DepthFarScreenHandle.MakeDepth(size, size);				// 深度バッファの作成
		this->m_Shader.Init("CommonData/shader/VS_SoftShadow.vso", "CommonData/shader/PS_SoftShadow.pso");
		this->m_ShaderRigid.Init("CommonData/shader/VS_SoftShadow_Rigid.vso", "CommonData/shader/PS_SoftShadow.pso");
	}
	bool		ShadowDraw::UpdateActive(void) noexcept {
		auto* OptionParts = OptionManager::Instance();
		bool shadow = OptionParts->GetParamInt(EnumSaveParam::shadow) > 0;
		if (this->m_PrevShadow != shadow) {
			this->m_PrevShadow = shadow;
			if (shadow) {
				SetActive();
				return true;
			}
			else {
				Dispose();
			}
		}
		return false;
	}
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	PostPassEffect::PostPassEffect(void) noexcept {
		PostPassScreenBufferPool::Create();
		auto* WindowSizeParts = WindowSizeControl::Instance();
		auto Prev = GetCreateDrawValidGraphZBufferBitDepth();
		SetCreateDrawValidGraphZBufferBitDepth(24);
		this->m_BufferScreen.Make(WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), true);
		this->m_ColorScreen.Make(WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), false);
		SetCreateDrawValidGraphZBufferBitDepth(Prev);
		// ポストエフェクト
		int now = 0;
		this->m_PostPass[now] = std::make_unique<PostPassBloom>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassDoF>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassSSR>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassSSAO>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassGodRay>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassDistortion>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassAberration>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassMotionBlur>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassVignette>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassCornerBlur>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassFXAA>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassScope>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassBlackout>(); ++now;

		this->m_ShadowDraw = std::make_unique<ShadowDraw>();
		// シェーダー
		auto* OptionParts = OptionManager::Instance();
		if (OptionParts->GetParamBoolean(EnumProjectSettingParam::PBR)) {
			this->m_PBR_Shader.Init("CommonData/shader/VS_PBR3D.vso", "CommonData/shader/PS_PBR3D.pso");
			this->m_PBR_Shader.AddGeometryShader("CommonData/shader/GS_PBR3D.pso");
		}
	}
	void		PostPassEffect::Init(void) noexcept {
		UpdateActive();
		// 影生成
		this->m_ShadowDraw->UpdateActive();
	}
	void		PostPassEffect::Dispose(void) noexcept {
		ResetAllBuffer();
		// ポストエフェクト
		for (auto& P : this->m_PostPass) {
			if (!P) { continue; }
			P.reset();
		}
		this->m_ShadowDraw.reset();
		auto* OptionParts = OptionManager::Instance();
		if (OptionParts->GetParamBoolean(EnumProjectSettingParam::PBR)) {
			this->m_PBR_Shader.Dispose();
		}
	}
	void		PostPassEffect::UpdateActive(void) noexcept {
		auto* OptionParts = OptionManager::Instance();
		bool ActiveGBuffer = false;
		for (auto& P : this->m_PostPass) {
			if (!P) { continue; }
			if (P->IsActive()) {
				ActiveGBuffer = true;
				break;
			}
		}
		UpdateActiveGBuffer(ActiveGBuffer);
		for (auto& P : this->m_PostPass) {
			if (!P) { continue; }
			P->UpdateActive(P->IsActive());
		}
		UpdateActiveCubeMap((OptionParts->GetParamInt(EnumSaveParam::Reflection) > 0) && OptionParts->GetParamBoolean(EnumProjectSettingParam::CubeMap));
	}
	// 
	void		PostPassEffect::SetCamMat(const Camera3DInfo& camInfo) noexcept {
		this->m_CamInfo = camInfo;
		this->m_CamViewMat = this->m_CamInfo.GetViewMatrix();
		this->m_CamProjectionMat = this->m_CamInfo.GetProjectionMatrix();
	}
	void		PostPassEffect::ResetBuffer(void) noexcept {
		if (this->m_IsActiveGBuffer) {
			// リセット替わり
			this->m_ColorScreen.SetDraw_Screen();
			this->m_NormalScreen.SetDraw_Screen();
			this->m_NormalScreen.FillGraph(128, 128, 255);
			this->m_DepthScreen.SetDraw_Screen();
			this->m_DepthScreen.FillGraph(255, 0, 0);
		}
	}
	void		PostPassEffect::DrawGBuffer(float near_len, float far_len, std::function<void()> done) noexcept {
		// カラーバッファを描画対象0に、法線バッファを描画対象1に設定
		this->m_BufferScreen.SetRenderTargetToShader(0);
		if (this->m_IsActiveGBuffer) {
			this->m_NormalScreen.SetRenderTargetToShader(1);
			this->m_DepthScreen.SetRenderTargetToShader(2);
		}
		ClearDrawScreenZBuffer();
		this->m_CamInfo.FlipCamInfo();
		SetCameraNearFar(near_len, far_len);
		{
			done();
		}
		SetRenderTargetToShader(0, InvalidID);
		if (this->m_IsActiveGBuffer) {
			SetRenderTargetToShader(1, InvalidID);
			SetRenderTargetToShader(2, InvalidID);
		}
	}

	void		PostPassEffect::SetDrawShadow(const Camera3DInfo& camInfo, std::function<void()> setshadowdoing_rigid, std::function<void()> setshadowdoing) noexcept {
		auto* OptionParts = OptionManager::Instance();
		// 影
		if (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0) {
			// 影画像の用意
			this->m_ShadowDraw->SetDraw(setshadowdoing_rigid, setshadowdoing, camInfo);
			// ソフトシャドウ重ね
			this->m_BufferScreen.SetDraw_Screen(false);
			{
				this->m_ShadowDraw->Draw();
			}
		}
	}
	void		PostPassEffect::DrawPostProcess(void) noexcept {
		this->m_BufferScreen.SetDraw_Screen(false);
		// 色味補正
		this->m_BufferScreen.GraphFilter(DX_GRAPH_FILTER_LEVEL, this->m_InColorPerMin, this->m_InColorPerMax, static_cast<int>(this->m_InColorGamma * 100), 0, 255);
		PostPassScreenBufferPool::Instance()->FirstUpdate();
		// ポストパスエフェクトのbufに描画
		if (this->m_IsActiveGBuffer) {
			for (auto& P : this->m_PostPass) {
				if (!P) { continue; }
				this->m_ColorScreen.GraphFilterBlt(this->m_BufferScreen, DX_GRAPH_FILTER_DOWN_SCALE, 1);
				P->SetEffect(&this->m_BufferScreen, &this->m_ColorScreen, &this->m_NormalScreen, &this->m_DepthScreen);
			}
		}
	}
	void		PostPassEffect::ResetAllBuffer(void) noexcept {
		UpdateActiveGBuffer(false);
		for (auto& P : this->m_PostPass) {
			if (!P) { continue; }
			P->UpdateActive(false);
		}
		UpdateActiveCubeMap(false);
	}
	void		PostPassEffect::Update_Shadow(std::function<void()> doing, const Vector3DX& CenterPos, bool IsFar) noexcept {
		auto* OptionParts = OptionManager::Instance();
		if (OptionParts->GetParamInt(EnumSaveParam::shadow) > 0) {
			// 影用の深度記録画像の準備を行う
			if (!IsFar) {
				this->m_ShadowDraw->Update(doing, CenterPos, this->GetShadowScale());
			}
			else {
				this->m_ShadowDraw->UpdateFar(doing, CenterPos, this->GetShadowScale() * 4.f);
			}
		}
	}
	void		PostPassEffect::Update_CubeMap(std::function<void()> doing, const Vector3DX& CenterPos) noexcept {
		auto* OptionParts = OptionManager::Instance();
		if ((OptionParts->GetParamInt(EnumSaveParam::Reflection) > 0) && OptionParts->GetParamBoolean(EnumProjectSettingParam::CubeMap)) {
			this->m_RealTimeCubeMap.ReadyDraw(CenterPos, doing);
		}
	}
	void		PostPassEffect::DrawByPBR(std::function<void()> doing) noexcept {
		auto* OptionParts = OptionManager::Instance();
		if (OptionParts->GetParamBoolean(EnumProjectSettingParam::PBR)) {
			MATRIX view, projection;
			GetTransformToViewMatrix(&view);
			GetTransformToProjectionMatrix(&projection);
			this->m_PBR_Shader.SetGeometryCONSTBUFFER(1, &view, &projection);
			this->m_PBR_Shader.Draw_lamda(doing);
		}
		else {
			doing();
		}
	}
	// 
	void		PostPassEffect::LoadGBuffer(void) noexcept {
		auto* WindowSizeParts = WindowSizeControl::Instance();
		auto Prev = GetCreateDrawValidGraphZBufferBitDepth();
		SetCreateDrawValidGraphZBufferBitDepth(24);
		this->m_NormalScreen.Make(WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax(), false);
		this->m_DepthScreen.MakeDepth(WindowSizeParts->GetScreenXMax(), WindowSizeParts->GetScreenYMax());
		SetCreateDrawValidGraphZBufferBitDepth(Prev);
	}
	void		PostPassEffect::DisposeGBuffer(void) noexcept {
		this->m_NormalScreen.Dispose();
		this->m_DepthScreen.Dispose();
	}
};
