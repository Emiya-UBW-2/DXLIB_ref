#include "DXLib_ref_100_PostPass.hpp"

namespace DXLib_ref {
	//--------------------------------------------------------------------------------------------------
	// ポストプロセスエフェクト
	//--------------------------------------------------------------------------------------------------
	PostPassBase::PostPassBase(void) {
		auto* DrawParts = DXDraw::Instance();
		ColorScreen = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, false);
	}
	//継承クラス
	class PostPassSSAO : public PostPassBase {
	private:
	public:
		PostPassSSAO(void) {
		}
		~PostPassSSAO(void) {
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph) noexcept {
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->Get_SSAO()) {
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
				GraphFilterBlt(m_NormalScreenPtr->get(), TargetGraph->get(), DX_GRAPH_FILTER_SSAO, ColorScreen.get(), 120.f, 0.004f, 0.01f, 0.9f, GetColor(0, 0, 0), 20.f);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
		}
	};
	class PostPassSSR : public PostPassBase {
	private:
		GraphHandle SSRColorScreen;	//そのままのGバッファ
		GraphHandle SSRNormalScreen;	//法線のGバッファ
		GraphHandle	SSRDepthScreen;	//深度のGバッファ
		GraphHandle SSRScreen;		//描画スクリーン
		GraphHandle SSRScreen2;		//描画スクリーン
		GraphHandle bkScreen2;		//SSRぼかし
		ShaderUseClass::ScreenVertex	m_SSRScreenVertex;				// 頂点データ
		ShaderUseClass		m_SSR;										// シェーダー
	public:
		PostPassSSR(void) {
			auto* DrawParts = DXDraw::Instance();
			SSRScreen = GraphHandle::Make(DrawParts->m_DispXSize / EXTEND, DrawParts->m_DispYSize / EXTEND, true);							//描画スクリーン
			SSRScreen2 = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);							//描画スクリーン
			{
				bkScreen2 = GraphHandle::Make(DrawParts->m_DispXSize / EXTEND, DrawParts->m_DispYSize / EXTEND, false);							//ふち黒
				bkScreen2.SetDraw_Screen(true);
				{
					int xr = DrawParts->m_DispXSize / EXTEND * 30 / 100;
					int yr = DrawParts->m_DispYSize / EXTEND * 60 / 100;

					DrawBox(0, 0, DrawParts->m_DispXSize / EXTEND, DrawParts->m_DispYSize / EXTEND, GetColor(0, 0, 0), TRUE);
					DrawOval(DrawParts->m_DispXSize / EXTEND / 2, DrawParts->m_DispYSize / EXTEND / 2, xr, yr, GetColor(255, 255, 255), TRUE);

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
				SSRNormalScreen = GraphHandle::Make(DrawParts->m_DispXSize / EXTEND, DrawParts->m_DispYSize / EXTEND, true);			// 法線
				{
					// 深度を描画するテクスチャの作成( 2チャンネル浮動小数点32ビットテクスチャ )
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
			m_SSRScreenVertex.SetScreenVertex(DrawParts->m_DispXSize / EXTEND, DrawParts->m_DispYSize / EXTEND);	// 頂点データの準備
			m_SSR.Init("shader/VS_SSR.vso", "shader/PS_SSR.pso");					// レンズ
		}
		~PostPassSSR(void) {
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* OptionParts = OPTION::Instance();
			auto* PostPassParts = PostPassEffect::Instance();

			if (OptionParts->Get_SSR()) {
				GraphFilterBlt(ColorScreen.get(), SSRColorScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
				GraphFilterBlt(m_NormalScreenPtr->get(), SSRNormalScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
				GraphFilterBlt(m_DepthScreenPtr->get(), SSRDepthScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
				SSRScreen.SetDraw_Screen();
				{
					int RayInterval = 50;//レイの分割間隔
					float SSRScale = 12.5f;
					float DepthThreshold = 7.f;
					SetUseTextureToShader(0, SSRColorScreen.get());	//使用するテクスチャをセット
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
					//DrawBox(0, 0, 1920, 1080, GetColor(255, 0, 0), TRUE);
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
		GraphHandle			DoFScreen;		//描画スクリーン
		ShaderUseClass::ScreenVertex	m_ScreenVertex;					// 頂点データ
		ShaderUseClass		m_DoF;			// シェーダー
	public:
		PostPassDoF(void) {
			auto* DrawParts = DXDraw::Instance();
			DoFScreen = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);							//描画スクリーン
			m_ScreenVertex.SetScreenVertex(DrawParts->m_DispXSize, DrawParts->m_DispYSize);							// 頂点データの準備
			m_DoF.Init("shader/VS_DoF.vso", "shader/PS_DoF.pso");					// DoF
		}
		~PostPassDoF(void) {
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* OptionParts = OPTION::Instance();
			auto* PostPassParts = PostPassEffect::Instance();

			if (OptionParts->Get_DoF()) {
				if (GetUseDirect3DVersion() == DX_DIRECT3D_11) {
					GraphFilterBlt(TargetGraph->get(), DoFScreen.get(), DX_GRAPH_FILTER_GAUSS, 16, 2000);
					TargetGraph->SetDraw_Screen();
					{
						SetUseTextureToShader(0, ColorScreen.get());	//使用するテクスチャをセット
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
		GraphHandle GaussScreen_;	//描画スクリーン
		GraphHandle	BufScreen;
	public:
		PostPassBloom(void) {
			auto* DrawParts = DXDraw::Instance();
			GaussScreen_ = GraphHandle::Make(DrawParts->m_DispXSize / EXTEND, DrawParts->m_DispYSize / EXTEND, true);
			BufScreen = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);							//描画スクリーン
		}
		~PostPassBloom(void) {
			GaussScreen_.Dispose();
			BufScreen.Dispose();
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* OptionParts = OPTION::Instance();

			if (OptionParts->Get_Bloom()) {
				GraphFilterBlt(TargetGraph->get(), BufScreen.get(), DX_GRAPH_FILTER_TWO_COLOR, 250, GetColor(0, 0, 0), 255, GetColor(128, 128, 128), 255);
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
	class PostPassLevelCorrect : public PostPassBase {
	private:
		GraphHandle		BufScreen;
	public:
		PostPassLevelCorrect(void) {
			auto* DrawParts = DXDraw::Instance();
			BufScreen = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);							//描画スクリーン
		}
		~PostPassLevelCorrect(void) {
			BufScreen.Dispose();
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* OptionParts = OPTION::Instance();

			if (true) {
				int input_low = 30;
				int input_high = 255;
				float gamma = 1.1f;
				int output_low = 0;
				int output_high = 255;
				if (OptionParts->Get_SSAO()) {
					output_high = 255;
				}

				//output_high = 255;

				GraphFilterBlt(TargetGraph->get(), BufScreen.get(), DX_GRAPH_FILTER_LEVEL, input_low, input_high, int(gamma * 100), output_low, output_high);
				TargetGraph->SetDraw_Screen(false);
				{
					BufScreen.DrawExtendGraph(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);
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
				buf = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);							//描画スクリーン
			}
		}
		~PostPassAberration(void) {
			for (auto&buf : BufScreen) {
				buf.Dispose();
			}
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph) noexcept {
			auto* OptionParts = OPTION::Instance();
			if (OptionParts->Get_aberration()) {
				auto* DrawParts = DXDraw::Instance();
				BufScreen[0].SetDraw_Screen(true);
				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);
				BufScreen[1].SetDraw_Screen(true);
				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);
				BufScreen[2].SetDraw_Screen(true);
				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);
				GraphBlend(BufScreen[0].get(), TargetGraph->get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
					DX_RGBA_SELECT_BLEND_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_SRC_A);
				GraphBlend(BufScreen[1].get(), TargetGraph->get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
					DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_BLEND_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_SRC_A);
				GraphBlend(BufScreen[2].get(), TargetGraph->get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
					DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_BLEND_B, DX_RGBA_SELECT_SRC_A);
				//output_high = 255;
				TargetGraph->SetDraw_Screen(true);
				{
					DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);
					SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
					BufScreen[0].DrawRotaGraph(DrawParts->m_DispXSize / 2, DrawParts->m_DispYSize / 2, 1.01f, 0.f, true);
					BufScreen[1].DrawRotaGraph(DrawParts->m_DispXSize / 2, DrawParts->m_DispYSize / 2, 1.005f, 0.f, true);
					BufScreen[2].DrawRotaGraph(DrawParts->m_DispXSize / 2, DrawParts->m_DispYSize / 2, 1.f, 0.f, true);
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
		void SetEffect_Sub(GraphHandle* TargetGraph) noexcept {
			if (true) {
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
		GraphHandle AberrationScreen;	//描画スクリーン
		GraphHandle bkScreen2;
		GraphHandle	BufScreen;
	public:
		PostPassCornerBlur(void) {
			auto* DrawParts = DXDraw::Instance();
			AberrationScreen = GraphHandle::Make(DrawParts->m_DispXSize / EXTEND, DrawParts->m_DispYSize / EXTEND, true);	//描画スクリーン
			{
				bkScreen2 = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, false);							//ふち黒
				bkScreen2.SetDraw_Screen(true);
				{
					int xr = DrawParts->m_DispXSize * 60 / 100;
					int yr = DrawParts->m_DispYSize * 70 / 100;

					DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);
					DrawOval(DrawParts->m_DispXSize / 2, DrawParts->m_DispYSize / 2, xr, yr, GetColor(255, 255, 255), TRUE);

					int r = 0, c = 0, p = 2;

					p = 1;
					for (r = 0; r < 255; r += p) {
						c = 255 - int(std::powf(float(255 - r) / 255.f, 1.5f)*255.f);

						DrawOval(DrawParts->m_DispXSize / 2, DrawParts->m_DispYSize / 2, xr - r / p, yr - r / p, GetColor(c, c, c), FALSE, 2);
					}
				}
			}
			BufScreen = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);							//描画スクリーン
		}
		~PostPassCornerBlur(void) {
			AberrationScreen.Dispose();
			bkScreen2.Dispose();
			BufScreen.Dispose();
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* OptionParts = OPTION::Instance();
			//結果描画
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
				bkScreen = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, false);							//ふち黒
				bkScreen.SetDraw_Screen(true);
				{
					int y = 0, c = 0, p = 2;
					DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(255, 255, 255), TRUE);

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
			BufScreen = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);							//描画スクリーン
		}
		~PostPassVignette(void) {
			BufScreen.Dispose();
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph) noexcept {
			auto* DrawParts = DXDraw::Instance();

			BufScreen.SetDraw_Screen();
			{
				TargetGraph->DrawGraph(0, 0, true);
			}
			GraphBlend(BufScreen.get(), bkScreen.get(), 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_BLEND_R);

			TargetGraph->SetDraw_Screen();
			{
				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);
				BufScreen.DrawGraph(0, 0, true);
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
		SkyScreen = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, false);		//空描画
		FarScreen_ = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, false);		//描画スクリーン
		NearScreen_ = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);		//描画スクリーン
		MAIN_Screen = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, false);		//最終描画用
		//
		NormalScreen = GraphHandle::Make(DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);		// 法線Gバッファ
		{
			// 深度を描画するテクスチャGバッファの作成( 2チャンネル浮動小数点32ビットテクスチャ )
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
		//
		DepthDraw.Init("shader/NormalMesh_PointLightVS.vso", "shader/NormalMesh_PointLightPS.pso");
		//ポストエフェクト
		m_PostPass.emplace_back(std::make_unique<PostPassSSAO>());
		m_PostPass.emplace_back(std::make_unique<PostPassSSR>());
		m_PostPass.emplace_back(std::make_unique<PostPassDoF>());
		m_PostPass.emplace_back(std::make_unique<PostPassBloom>());
		m_PostPass.emplace_back(std::make_unique<PostPassLevelCorrect>());
		m_PostPass.emplace_back(std::make_unique<PostPassAberration>());
		m_PostPass.emplace_back(std::make_unique<PostPassMotionBlur>());
		m_PostPass.emplace_back(std::make_unique<PostPassVignette>());
		m_PostPass.emplace_back(std::make_unique<PostPassCornerBlur>());
		for (auto& P : m_PostPass) {
			P->SetPtr(&NormalScreen, &DepthScreen);
		}
	}
	PostPassEffect::~PostPassEffect(void) noexcept {
		for (auto& P : m_PostPass) {
			P.reset();
		}
		m_PostPass.clear();
	}
	//
	void PostPassEffect::Draw(std::function<void()> sky_doing, std::function<void()> doing, const Camera3DInfo& cams) {
		//nearに描画
		auto* DrawParts = DXDraw::Instance();
		auto* OptionParts = OPTION::Instance();
		//Gバッファに描画
		auto G_Draw = [&](GraphHandle* TargetDraw, float near_len, float far_len, std::function<void()> done) {
			// カラーバッファを描画対象0に、法線バッファを描画対象1に設定
			SetRenderTargetToShader(0, TargetDraw->get());
			SetRenderTargetToShader(1, NormalScreen.get());
			SetRenderTargetToShader(2, DepthScreen.get());
			//SetRenderTargetToShader(3, SpecScreenHandle);
			SetCameraNearFar(near_len, far_len);
			SetupCamera_Perspective(cams.GetCamFov());
			SetCameraPositionAndTargetAndUpVec(cams.GetCamPos().get(), cams.GetCamVec().get(), cams.GetCamUp().get());
			{
				done();
			}
			SetRenderTargetToShader(0, -1);
			SetRenderTargetToShader(1, -1);
			SetRenderTargetToShader(2, -1);
			//SetRenderTargetToShader(3, -1);
		};
		//全ての画面を初期化
		{
			G_Draw(&FarScreen_, cams.GetCamFar() - 10.f, 1000000.f, [&]() { ClearDrawScreen(); });
			NearScreen_.SetDraw_Screen();//リセット替わり
		}
		//空
		SkyScreen.SetDraw_Screen(cams.GetCamPos() - cams.GetCamVec(), VECTOR_ref::vget(0, 0, 0), cams.GetCamUp(), cams.GetCamFov(), 10000.0f, 50000.0f);//500.0f, 10000.0f);
		{
			sky_doing();
		}
		//遠距離
		G_Draw(&FarScreen_, cams.GetCamFar() - 10.f, 1000000.f, [&]() {
			SkyScreen.DrawGraph(0, 0, FALSE);
			if (OptionParts->Get_Shadow()) {
				DrawParts->SetUseShadow();
				doing();
				DrawParts->ResetUseShadow();
			}
			else {
				doing();
			}
		});
		//遠距離の強烈なぼかし
		if (OptionParts->Get_DoF()) {
			GraphFilter(FarScreen_.get(), DX_GRAPH_FILTER_GAUSS, 16, 200);
		}
		//中間
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
		});
		//至近
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
		});
		//fovを記憶しておく
		fov = cams.GetCamFov();
		//ポストパスエフェクトの適用
		SetPostpassEffect();
	}
	//
	void PostPassEffect::Plus_Draw(std::function<void()> doing) noexcept {
		NearScreen_.SetDraw_Screen(false);
		{
			doing();
		}
		//結果描画
		MAIN_Screen.SetDraw_Screen();
		{
			NearScreen_.DrawGraph(0, 0, false);

			//NormalScreen.DrawExtendGraph(0, 0, 960, 540, false);
			//DepthScreen.DrawExtendGraph(0, 0, 960, 540, false);
		}
	}
	//
	void PostPassEffect::SetPostpassEffect(void) noexcept {
		//bufに描画
		for (auto& P : m_PostPass) {
			P->SetEffect(&NearScreen_);
		}
		//結果描画
		MAIN_Screen.SetDraw_Screen();
		{
			SetDrawBright(r_brt, g_brt, b_brt);
			NearScreen_.DrawGraph(0, 0, false);
			SetDrawBright(255, 255, 255);
			//NormalScreen.DrawExtendGraph(0, 0, 960, 540, false);
			//DepthScreen.DrawExtendGraph(0, 0, 960, 540, false);
		}
	}
	//書き込んだ深度に応じて対応
	void PostPassEffect::DrawByDepth(std::function<void()> doing) noexcept {
		DepthDraw.SetPixelParam(3, 3.f*12.5f, 0, 0, 0);
		DepthDraw.Draw_lamda([&] {
			SetUseTextureToShader(1, DepthScreen.get());
			doing();
		});
	}
};
