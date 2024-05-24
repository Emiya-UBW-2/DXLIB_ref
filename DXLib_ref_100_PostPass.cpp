#include "DXLib_ref_100_PostPass.hpp"

namespace DXLib_ref {
	//--------------------------------------------------------------------------------------------------
	// ポストプロセスエフェクト
	//--------------------------------------------------------------------------------------------------
	//継承クラス
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
			GraphFilterBlt(m_NormalScreenPtr->get(), TargetGraph->get(), DX_GRAPH_FILTER_SSAO, ColorGraph->get(), 120.f, 0.004f, 0.01f, 0.9f, Black, 20.f);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
	};
	class PostPassSSR : public PostPassBase {
	private:
		GraphHandle SSRColorScreen;	//そのままのGバッファ
		GraphHandle SSRNormalScreen;	//法線のGバッファ
		GraphHandle	SSRDepthScreen;	//深度のGバッファ
		GraphHandle SSRScreen;		//描画スクリーン
		GraphHandle SSRScreen0;		//描画スクリーン
		GraphHandle SSRScreen1;		//描画スクリーン
		GraphHandle SSRScreen2;		//描画スクリーン
		GraphHandle bkScreen2;		//SSRぼかし
		ShaderUseClass::ScreenVertex	m_SSRScreenVertex;				// 頂点データ
		ShaderUseClass		m_SSR;										// シェーダー

		int SP = 1;
		int RayInterval = 150;//レイの分割間隔
		float SSRScale = 12.5f;
		float DepthThreshold = 10.f;

		static const int SSREX = 4;
		static const int SSREX2 = 8;
	public:
		void Load_Sub() noexcept override {
			SSRScreen = GraphHandle::Make(y_r(1920) / SSREX, y_r(1080) / SSREX, true);							//描画スクリーン
			SSRScreen0 = GraphHandle::Make(y_r(1920) / SSREX, y_r(1080) / SSREX, true);							//描画スクリーン
			SSRScreen1 = GraphHandle::Make(y_r(1920) / SSREX2, y_r(1080) / SSREX2, true);							//描画スクリーン
			SSRScreen2 = GraphHandle::Make(y_r(1920), y_r(1080), true);							//描画スクリーン
			{
				bkScreen2 = GraphHandle::Make(y_r(1920) / SSREX, y_r(1080) / SSREX, false);							//ふち黒
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
				SSRNormalScreen = GraphHandle::Make(y_r(1920) / SSREX, y_r(1080) / SSREX, false);			// 法線
				{
					// 深度を描画するテクスチャの作成( 2チャンネル浮動小数点32ビットテクスチャ )
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
			m_SSRScreenVertex.SetScreenVertex(y_r(1920) / SSREX, y_r(1080) / SSREX);	// 頂点データの準備
			m_SSR.Init("shader/VS_SSR.vso", "shader/PS_SSR.pso");					// レンズ
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
				SetUseTextureToShader(0, SSRColorScreen.get());	//使用するテクスチャをセット
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
		GraphHandle			DoFScreen;		//描画スクリーン
		ShaderUseClass::ScreenVertex	m_ScreenVertex;					// 頂点データ
		ShaderUseClass		m_DoF;			// シェーダー
	public:
		void Load_Sub() noexcept override {
			DoFScreen = GraphHandle::Make(y_r(1920), y_r(1080), true);							//描画スクリーン
			m_ScreenVertex.SetScreenVertex(y_r(1920), y_r(1080));							// 頂点データの準備
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
				SetUseTextureToShader(0, ColorGraph->get());	//使用するテクスチャをセット
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
		GraphHandle GaussScreen_;	//描画スクリーン
		GraphHandle	BufScreen;
	public:
		void Load_Sub() noexcept override {
			GaussScreen_ = GraphHandle::Make(y_r(1920) / EXTEND, y_r(1080) / EXTEND, true);
			BufScreen = GraphHandle::Make(y_r(1920), y_r(1080), true);							//描画スクリーン
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
				buf = GraphHandle::Make(y_r(1920), y_r(1080), true);							//描画スクリーン
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
		GraphHandle AberrationScreen;	//描画スクリーン
		GraphHandle bkScreen2;
		GraphHandle	BufScreen;
	public:
		PostPassCornerBlur(void) {
			AberrationScreen = GraphHandle::Make(y_r(1920) / EXTEND, y_r(1080) / EXTEND, true);	//描画スクリーン
			{
				bkScreen2 = GraphHandle::Make(y_r(1920), y_r(1080), false);							//ふち黒
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
			BufScreen = GraphHandle::Make(y_r(1920), y_r(1080), true);							//描画スクリーン
		}
		~PostPassCornerBlur(void) {
			AberrationScreen.Dispose();
			bkScreen2.Dispose();
			BufScreen.Dispose();
		}
	public:
		void SetEffect_Sub(GraphHandle* TargetGraph, GraphHandle*) noexcept override {
			auto* OptionParts = OPTION::Instance();
			//結果描画
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
				bkScreen = GraphHandle::Make(y_r(1920), y_r(1080), false);							//ふち黒
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
			BufScreen = GraphHandle::Make(y_r(1920), y_r(1080), true);							//描画スクリーン
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
			BufScreen = GraphHandle::Make(y_r(1920), y_r(1080), true);							//描画スクリーン
		}
		~PostPassDistortion(void) {
			BufScreen.Dispose();
		}
	private:
		// 画面を歪ませながら描画する関数
		void DrawCircleScreen(
			int CenterX,			// 円の中心座標X
			int CenterY,			// 円の中心座標Y
			float Radius,			// 円のサイズ
			float Absorption,		// 内側の円に引き込まれるドット数
			const GraphHandle& ScreenHandle// 画面グラフィックハンドル
		) {
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
			float AngleCosTable[CIRCLE_ANGLE_VERTEX_NUM];
			float AngleSinTable[CIRCLE_ANGLE_VERTEX_NUM];
			float InCircleCosTable[CIRCLE_RADIUS_VERTEX_NUM];

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
				AngleSinTable[i] = (float)sin((double)Angle);
				AngleCosTable[i] = (float)cos((double)Angle);
			}

			// 内側の盛り上がっているように見せる箇所で使用する Cos テーブルを作成する
			Angle = 0.0f;
			for (i = 0; i < CIRCLE_RADIUS_VERTEX_NUM; i++, Angle += (DX_PI_F / 2.0f) / (CIRCLE_RADIUS_VERTEX_NUM - 1)) {
				InCircleCosTable[i] = (float)cos((double)Angle);
			}

			// ポリゴン頂点インデックスの準備
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

			// バイリニア補間描画にする
			SetDrawMode(DX_DRAWMODE_BILINEAR);


			// 外側のドーナツ部分を描画

			// 中心に向かうにしたがって中心方向にテクスチャ座標をずらす
			Vert = Vertex;
			for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; i++) {
				// 使用する Sin, Cos の値をセット
				Sin = AngleSinTable[i];
				Cos = AngleCosTable[i];

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
					Vert->u = (Vert->pos.x + AbsorptionMoveX) / y_r(1920);
					Vert->v = (Vert->pos.y + AbsorptionMoveY) / y_r(1080);

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
				Sin = AngleSinTable[i];
				Cos = AngleCosTable[i];

				for (int j = 0; j < CIRCLE_RADIUS_VERTEX_NUM; j++, Vert++) {
					// 円の中心までの距離を算出
					CenterDistance = InCircleCosTable[j] * Radius;

					// 画像座標視点での円の中心までの距離を算出
					GraphCenterDistance = ((CIRCLE_RADIUS_VERTEX_NUM - 1) - j) * (Absorption + Radius) / (CIRCLE_RADIUS_VERTEX_NUM - 1);

					// スクリーン座標の決定
					Vert->pos.x = Cos * CenterDistance + CenterX;
					Vert->pos.y = Sin * CenterDistance + CenterY;
					Vert->pos.z = 0.0f;

					// テクスチャ座標のセット
					Vert->u = (Cos * GraphCenterDistance + CenterX) / y_r(1920);
					Vert->v = (Sin * GraphCenterDistance + CenterY) / y_r(1080);

					// その他のパラメータをセット
					Vert->rhw = 1.0f;
					Vert->dif = DiffuseColor;
				}
			}

			// 中心の盛り上がって見える部分を描画
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
				// 画面を歪ませて描画
				DrawCircleScreen(
					y_r(1920) / 2, y_r(1080) / 2,	// 中心座標
					(float)(y_r(1920) * 2 / 3),	// 円のサイズ
					120.0f,	// 内側に引き込まれるドット数
					BufScreen);
				//BufScreen.DrawGraph(0, 0, true);
			}
		}
	};

	class PostPassFXAA : public PostPassBase {
	private:
		ShaderUseClass::ScreenVertex	m_ScreenVertex;					// 頂点データ
		ShaderUseClass		m_FXAA;			// シェーダー
	public:
		void Load_Sub() noexcept override {
			m_ScreenVertex.SetScreenVertex(y_r(1920), y_r(1080));							// 頂点データの準備
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
				SetUseTextureToShader(0, ColorGraph->get());	//使用するテクスチャをセット
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
		FarScreen_ = GraphHandle::Make(y_r(1920), y_r(1080), false);		//描画スクリーン
		NearScreen_ = GraphHandle::Make(y_r(1920), y_r(1080), true);		//描画スクリーン
		MAIN_Screen = GraphHandle::Make(y_r(1920), y_r(1080), false);		//最終描画用
		//Gバッファ
		LoadGBuffer();
		//ポストエフェクト
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
		//Gバッファ
		DisposeGBuffer();
		//ポストエフェクト
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
		fov = cams.GetCamFov();		//fovを記憶しておく
		//全ての画面を初期化
		{
			NormalScreen.SetDraw_Screen();//リセット替わり
			DepthScreen.SetDraw_Screen();//リセット替わり
			FarScreen_.SetDraw_Screen();//リセット替わり
			NearScreen_.SetDraw_Screen();//リセット替わり
		}
		//空
		DrawGBuffer(&FarScreen_, 1000.0f, 50000.0f, [&]() { sky_doing(); }, cams);
		//遠距離
		DrawGBuffer(&FarScreen_, cams.GetCamFar() - 10.f, 1000000.f, [&]() {
			doing();
			doingFront();
			}, cams);
		//遠距離の強烈なぼかし
		if (OptionParts->GetParamBoolean(EnumSaveParam::DoF)) {
			GraphFilter(FarScreen_.get(), DX_GRAPH_FILTER_GAUSS, 16, 200);
		}
		//中間
		DrawGBuffer(&NearScreen_, cams.GetCamNear(), cams.GetCamFar(), [&]() {
			FarScreen_.DrawGraph(0, 0, false);
			Effekseer_Sync3DSetting();
			doing();
			DrawEffekseer3D();
			doingFront();
			}, cams);
		//至近
		DrawGBuffer(&NearScreen_, 0.1f, 0.1f + cams.GetCamNear(), [&]() {
			Effekseer_Sync3DSetting();
			doing();
			DrawEffekseer3D();
			doingFront();
			}, cams);

		//ポストパスエフェクトのアップデート
		MAIN_Screen.SetDraw_Screen(cams);
		for (auto& P : m_PostPass) {
			P->Update();
		}
	}
	void PostPassEffect::Draw() {
		//色味補正
		{
			int output_low = 0;
			int output_high = 255;
			GraphFilter(NearScreen_.get(), DX_GRAPH_FILTER_LEVEL, InColorPerMin, InColorPerMax, int(InColorGamma * 100), output_low, output_high);
		}
		//ポストパスエフェクトのbufに描画
		for (auto& P : m_PostPass) {
			ColorScreen.SetDraw_Screen();
			{
				NearScreen_.DrawGraph(0, 0, false);
			}
			P->SetEffect(&NearScreen_, &ColorScreen);
		}
		//ポストパスエフェクトの結果描画
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
		// カラーバッファを描画対象0に、法線バッファを描画対象1に設定
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
		NormalScreen = GraphHandle::Make(y_r(1920), y_r(1080), false);	// 法線Gバッファ
		{
			// 深度を描画するテクスチャGバッファの作成( 2チャンネル浮動小数点32ビットテクスチャ )
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
