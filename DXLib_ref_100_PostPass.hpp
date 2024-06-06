#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	//
	static const int EXTEND = 4;
	//ベース
	class PostPassBase {
	protected:
		bool m_PrevActive{false};
	public:
		PostPassBase(void) {}
		virtual ~PostPassBase(void) {}
	protected:
		virtual void Load_Sub() noexcept {}
		virtual void Dispose_Sub() noexcept {}
		virtual bool IsActive_Sub() noexcept { return true; }
		virtual void SetEffect_Sub(GraphHandle*, GraphHandle*, GraphHandle*, GraphHandle*) noexcept {}
	public:
		bool IsActive() noexcept { return IsActive_Sub(); }
		void Init()noexcept {
			bool active = IsActive_Sub();
			m_PrevActive = active;
			if (active) {
				Load_Sub();
			}
		}
		void UpdateActive() noexcept {
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
		void Dispose() noexcept {
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
		bool										m_IsActiveGBuffer{true};
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
		auto& Get_MAIN_Screen(void) noexcept { return BufferScreen; }

		auto& Get_near_DoF(void) noexcept { return near_DoF; }
		auto& Get_far_DoF(void) noexcept { return far_DoF; }
		auto& Get_near_DoFMax(void) noexcept { return near_DoFMax; }
		auto& Get_far_DoFMin(void) noexcept { return far_DoFMin; }
	public:
		//ボケ始める場所を指定(完全にボケるのはニアファーの限度)
		void Set_DoFNearFar(float near_d, float far_d, float near_m, float far_m) {
			near_DoF = near_d;
			far_DoF = far_d;
			near_DoFMax = near_m;
			far_DoFMin = far_m;
		}
		void SetLevelFilter(int inMin, int inMax, float gamma) {
			InColorPerMin = std::clamp(inMin, 0, 255);
			InColorPerMax = std::clamp(inMax, 0, 255);
			InColorGamma = std::max(1.f, gamma);
		}
	private:
		PostPassEffect(void);
		~PostPassEffect(void) noexcept;
	public:
		void Update() noexcept;
		void DrawDoF(std::function<void()> sky_doing, std::function<void()> doing, std::function<void()> doingFront, const Camera3DInfo& cams);
		void Draw();
		void Plus_Draw(std::function<void()> doing) noexcept {
			BufferScreen.SetDraw_Screen(false);
			{
				doing();
			}
		}
	private:
		void DrawGBuffer(float near_len, float far_len, std::function<void()> done, const Camera3DInfo& cams);
		void LoadGBuffer() noexcept;
		void DisposeGBuffer() noexcept;
	};
};
