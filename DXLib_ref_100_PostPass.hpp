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
		GraphHandle SSRScreen;		//描画スクリーン
		//
		ShaderUseClass::ScreenVertex	m_ScreenVertex;					// 頂点データ
		ShaderUseClass		m_SSR;										// シェーダー
		ShaderUseClass		DepthDraw;									// シェーダー
		ShaderUseClass		m_DoF;										// シェーダー

		int r_brt = 255;
		int g_brt = 255;
		int b_brt = 255;
	public:
		auto& Get_BUF_Screen(void) noexcept { return BackScreen; }
		auto& Get_MAIN_Screen(void) noexcept { return MAIN_Screen; }
	public:
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




