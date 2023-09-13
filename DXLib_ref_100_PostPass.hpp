#pragma once
#include "DXLib_ref.h"

namespace DXLib_ref {
	//
	static const int EXTEND = 4;
	//ベース
	class PostPassBase {
	protected:
		//Gバッファ
		GraphHandle* m_NormalScreenPtr{ nullptr };
		GraphHandle* m_DepthScreenPtr{ nullptr };
		GraphHandle ColorScreen;	//そのまま透過なしにしたスクリーン
	public:
		PostPassBase(void);
		virtual ~PostPassBase(void) {}
	protected:
		virtual void SetEffect_Sub(GraphHandle*) noexcept {}
	public:
		void SetPtr(GraphHandle* NormalPtr, GraphHandle* DepthPtr)noexcept {
			m_NormalScreenPtr = NormalPtr;
			m_DepthScreenPtr = DepthPtr;
		}
	public:
		void SetEffect(GraphHandle* TargetGraph) noexcept {
			ColorScreen.SetDraw_Screen();
			{
				TargetGraph->DrawGraph(0, 0, false);
			}
			SetEffect_Sub(TargetGraph);
		}
	};
	//
	class PostPassEffect : public SingletonBase<PostPassEffect> {
	private:
		friend class SingletonBase<PostPassEffect>;
	private:

		std::vector<std::unique_ptr<PostPassBase>> m_PostPass;

		GraphHandle SkyScreen;		//空描画
		GraphHandle FarScreen_;		//描画スクリーン
		GraphHandle NearScreen_;	//描画スクリーン

		GraphHandle MAIN_Screen;	//描画スクリーン

		GraphHandle NormalScreen;	//法線のGバッファ
		GraphHandle	DepthScreen;	//深度のGバッファ
		//
		ShaderUseClass		DepthDraw;									// シェーダー
		//
		float fov = 0.f;
		float near_DoF = 0.f;
		float far_DoF = 0.f;
		float near_DoFMax = 0.f;
		float far_DoFMin = 0.f;

		int r_brt = 255;
		int g_brt = 255;
		int b_brt = 255;
	public:
		auto& Get_MAIN_Screen(void) noexcept { return MAIN_Screen; }

		auto& Get_fov(void) noexcept { return fov; }
		auto& Get_near_DoF(void) noexcept { return near_DoF; }
		auto& Get_far_DoF(void) noexcept { return far_DoF; }
		auto& Get_near_DoFMax(void) noexcept { return near_DoFMax; }
		auto& Get_far_DoFMin(void) noexcept { return far_DoFMin; }
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
		void Draw(std::function<void()> sky_doing, std::function<void()> doing, const Camera3DInfo& cams);
		//
		void Plus_Draw(std::function<void()> doing) noexcept;
		//
		void SetPostpassEffect(void) noexcept;
		//書き込んだ深度に応じて対応
		void DrawByDepth(std::function<void()> doing) noexcept;
	};
};




