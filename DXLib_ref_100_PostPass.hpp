#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	// ベース
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
		void UpdateActive(bool active) noexcept {
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
	struct shaderparam {
		bool					use{ false };
		std::array<float, 4>	param{ 0,0,0,0 };
	};
	// 
	class PostPassEffect : public SingletonBase<PostPassEffect> {
	private:
		friend class SingletonBase<PostPassEffect>;
	private:

		std::vector<std::unique_ptr<PostPassBase>> m_PostPass;
		bool										m_IsActiveGBuffer{ true };
		GraphHandle BufferScreen;	// 描画スクリーン

		GraphHandle ColorScreen;	// そのまま透過なしにしたスクリーン
		GraphHandle NormalScreen;	// 法線のGバッファ
		GraphHandle	DepthScreen;	// 深度のGバッファ
		// 
		float near_DoF = 0.f;
		float far_DoF = 0.f;
		float near_DoFMax = 0.f;
		float far_DoFMin = 0.f;
		// 
		int InColorPerMin = 20;
		int InColorPerMax = 255;
		float InColorGamma = 1.1f;
		Matrix4x4DX					m_CamViewMat{};
		Matrix4x4DX					m_CamProjectionMat{};
		Camera3DInfo				m_CamInfo{};

		std::array<shaderparam, 2>	m_Shader2D;
		float						m_AberrationPower{ 1.f };
	public:
		const auto& GetCamViewMat(void) const noexcept { return m_CamViewMat; }
		const auto& GetCamProjectionMat(void) const noexcept { return m_CamProjectionMat; }
	public:
		auto& GetBufferScreen(void) noexcept { return BufferScreen; }
		auto& GetColorBuffer(void) noexcept { return ColorScreen; }

		auto& Get_near_DoF(void) noexcept { return near_DoF; }
		auto& Get_far_DoF(void) noexcept { return far_DoF; }
		auto& Get_near_DoFMax(void) noexcept { return near_DoFMax; }
		auto& Get_far_DoFMin(void) noexcept { return far_DoFMin; }

		const auto&		is_lens(void) const noexcept { return m_Shader2D.at(0).use; }
		const auto&		zoom_lens(void) const noexcept { return m_Shader2D.at(0).param.at(3); }
		void			Set_is_lens(bool value) noexcept { m_Shader2D.at(0).use = value; }
		void			Set_xp_lens(float value) noexcept { m_Shader2D.at(0).param.at(0) = value; }
		void			Set_yp_lens(float value) noexcept { m_Shader2D.at(0).param.at(1) = value; }
		void			Set_size_lens(float value) noexcept { m_Shader2D.at(0).param.at(2) = value; }
		void			Set_zoom_lens(float value) noexcept { m_Shader2D.at(0).param.at(3) = value; }
		void			Set_is_Blackout(bool value) noexcept { m_Shader2D.at(1).use = value; }
		void			Set_Per_Blackout(float value) noexcept { m_Shader2D.at(1).param.at(0) = value; }
		const auto&		GetLensParam(void) const noexcept { return m_Shader2D.at(0); }
		const auto&		GetBlackoutParam(void) const noexcept { return m_Shader2D.at(1); }

		const auto&		GetAberrationPower(void) const noexcept { return m_AberrationPower; }
		void			SetAberrationPower(float value) noexcept { m_AberrationPower = value; }
	public:
		// ボケ始める場所を指定(完全にボケるのはニアファーの限度)
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
		void SetCamMat(const Camera3DInfo& camInfo) noexcept;
		void ResetBuffer(void) noexcept;
		void DrawGBuffer(float near_len, float far_len, std::function<void()> done) noexcept;
		void DrawPostProcess(void) noexcept;
		void ResetAllBuffer(void) noexcept;
	private:
		void LoadGBuffer(void) noexcept;
		void DisposeGBuffer(void) noexcept;
	};
};
