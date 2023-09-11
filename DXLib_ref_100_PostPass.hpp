#pragma once
#include "DXLib_ref.h"

namespace DXLib_ref {
	//
	static const int EXTEND = 4;
	//ベース
	class PostPassBase {
	protected:
	public:
		PostPassBase(void) {}
		virtual ~PostPassBase(void) {}
	protected:
		virtual void SetEffect_Sub(GraphHandle*) noexcept {}
	public:
		void SetEffect(GraphHandle* TargetGraph) noexcept { SetEffect_Sub(TargetGraph); }
	};
	//
	class PostPassEffect : public SingletonBase<PostPassEffect> {
	private:
		friend class SingletonBase<PostPassEffect>;
	private:

		std::vector<std::unique_ptr<PostPassBase>> m_PostPass;

		GraphHandle SkyScreen;		//空描画
		GraphHandle FarScreen_;		//描画スクリーン
		GraphHandle DoFScreen;		//描画スクリーン
		GraphHandle NearScreen_;	//描画スクリーン

		GraphHandle BackScreen;		//描画スクリーン

		GraphHandle MAIN_Screen;	//描画スクリーン

		GraphHandle ColorScreen;	//そのままのGバッファ
		GraphHandle NormalScreen;	//法線のGバッファ
		GraphHandle	DepthScreen;	//深度のGバッファ
		//
		GraphHandle SSRColorScreen;	//そのままのGバッファ
		GraphHandle SSRNormalScreen;	//法線のGバッファ
		GraphHandle	SSRDepthScreen;	//深度のGバッファ
		GraphHandle SSRScreen;		//描画スクリーン
		GraphHandle SSRScreen2;		//描画スクリーン
		GraphHandle bkScreen2;		//SSRぼかし
		//
		ShaderUseClass::ScreenVertex	m_ScreenVertex;					// 頂点データ
		ShaderUseClass::ScreenVertex	m_SSRScreenVertex;				// 頂点データ
		ShaderUseClass		m_SSR;										// シェーダー
		ShaderUseClass		DepthDraw;									// シェーダー
		ShaderUseClass		m_DoF;										// シェーダー

		float near_DoF = 0.f;
		float far_DoF = 0.f;
		float near_DoFMax = 0.f;
		float far_DoFMin = 0.f;

		int r_brt = 255;
		int g_brt = 255;
		int b_brt = 255;
	public:
		auto& Get_BUF_Screen(void) noexcept { return BackScreen; }
		auto& Get_MAIN_Screen(void) noexcept { return MAIN_Screen; }
	public:
		//ボケ始める場所を指定(完全にボケるのはニアファーの限度)
		void Set_DoFNearFar(float near_d,float far_d, float near_m, float far_m) {
			near_DoF = near_d;
			far_DoF = far_d;
			near_DoFMax = near_m;
			far_DoFMin = far_m;
		}
		void Set_Bright(int r, int g, int b) {
			r_brt = std::clamp(r, 0, 255);
			g_brt = std::clamp(g, 0, 255);
			b_brt = std::clamp(b, 0, 255);
		}
	private:
		PostPassEffect(void);
		~PostPassEffect(void) noexcept;
	public:
		//
		void BUF_Draw(std::function<void()> sky_doing, std::function<void()> doing, const Camera3DInfo& cams);
		//
		void FlipBuftoMain(void) noexcept;
		//
		void SetPostpassEffect(void) noexcept;
		//書き込んだ深度に応じて対応
		void DrawByDepth(std::function<void()> doing) noexcept;
	};
};




