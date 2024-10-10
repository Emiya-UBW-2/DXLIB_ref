#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	//ベース
	class PostPassBase {
	protected:
		bool m_PrevActive{ false };
	public:
		PostPassBase(void) noexcept {}
		virtual ~PostPassBase(void) noexcept {}
	protected:
		virtual void Load_Sub(void) noexcept {}
		virtual void Dispose_Sub(void) noexcept {}
		virtual bool IsActive_Sub(void) noexcept { return true; }
		virtual void SetEffect_Sub(GraphHandle*, GraphHandle*, GraphHandle*, GraphHandle*) noexcept {}
	public:
		bool IsActive(void) noexcept { return IsActive_Sub(); }
		void Init(void) noexcept {
			m_PrevActive = IsActive_Sub();
			if (m_PrevActive) {
				Load_Sub();
			}
		}
		void UpdateActive(void) noexcept {
			bool active = IsActive_Sub();
			if (m_PrevActive != active) {
				m_PrevActive = active;
				if (active) {
					Load_Sub();
				}
				else {
					Dispose_Sub();
				}
			}
		}
		void Dispose(void) noexcept {
			if (IsActive_Sub()) {
				Dispose_Sub();
			}
		}
		void SetEffect(GraphHandle* TargetGraph, GraphHandle* ColorGraph, GraphHandle* NormalPtr, GraphHandle* DepthPtr) noexcept {
			if (IsActive_Sub()) {
				SetEffect_Sub(TargetGraph, ColorGraph, NormalPtr, DepthPtr);
			}
		}
	};
	//
	class PostPassEffect : public SingletonBase<PostPassEffect> {
	private:
		friend class SingletonBase<PostPassEffect>;
	private:

		std::vector<std::unique_ptr<PostPassBase>> m_PostPass;
		bool										m_IsActiveGBuffer{ true };
		GraphHandle BufferScreen;	//描画スクリーン

		GraphHandle ColorScreen;	//そのまま透過なしにしたスクリーン
		GraphHandle NormalScreen;	//法線のGバッファ
		GraphHandle	DepthScreen;	//深度のGバッファ
		//
		float near_DoF = 0.f;
		float far_DoF = 0.f;
		float near_DoFMax = 0.f;
		float far_DoFMin = 0.f;
		//
		int InColorPerMin = 20;
		int InColorPerMax = 255;
		float InColorGamma = 1.1f;
	public:
		auto&			Get_MAIN_Screen(void) noexcept { return BufferScreen; }
		auto&			Get_MAINBuffer_Screen(void) noexcept { return ColorScreen; }

		auto&			Get_near_DoF(void) noexcept { return near_DoF; }
		auto&			Get_far_DoF(void) noexcept { return far_DoF; }
		auto&			Get_near_DoFMax(void) noexcept { return near_DoFMax; }
		auto&			Get_far_DoFMin(void) noexcept { return far_DoFMin; }
	public:
		//ボケ始める場所を指定(完全にボケるのはニアファーの限度)
		void Set_DoFNearFar(float near_d, float far_d, float near_m, float far_m) noexcept {
			near_DoF = near_d;
			far_DoF = far_d;
			near_DoFMax = near_m;
			far_DoFMin = far_m;
		}
		void SetLevelFilter(int inMin, int inMax, float gamma) noexcept {
			InColorPerMin = std::clamp(inMin, 0, 255);
			InColorPerMax = std::clamp(inMax, 0, 255);
			InColorGamma = std::max(1.f, gamma);
		}
	private:
		PostPassEffect(void) noexcept;
		PostPassEffect(const PostPassEffect&) = delete;
		PostPassEffect(PostPassEffect&& o) = delete;
		PostPassEffect& operator=(const PostPassEffect&) = delete;
		PostPassEffect& operator=(PostPassEffect&& o) = delete;

		~PostPassEffect(void) noexcept;
	public:
		void Update(void) noexcept;
		void DrawDoF(std::function<void()> sky_doing, std::function<void()> doing, std::function<void()> doingFront, const Camera3DInfo& cams) noexcept;
		void Draw2D(std::function<void()> doing) noexcept;
		void Draw(void) noexcept;
		void Plus_Draw(std::function<void()> doing) noexcept {
			GraphFilterBlt(BufferScreen.get(), ColorScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, 1);
			BufferScreen.SetDraw_Screen(false);
			{
				doing();
			}
		}
	private:
		void DrawGBuffer(float near_len, float far_len, std::function<void()> done, const Camera3DInfo& cams) noexcept;
		void LoadGBuffer(void) noexcept;
		void DisposeGBuffer(void) noexcept;
	};
};
