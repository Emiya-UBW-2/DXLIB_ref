#pragma once
#include "DXLib_ref.h"

namespace DXLib_ref {
	class TEMPSCENE {
	private:
		bool			m_SelEnd{ false };
		std::shared_ptr<TEMPSCENE> Next_ptr{ nullptr };
		bool			m_IsUpdateDraw{ true };

		std::array<VECTOR_ref, 3>		m_ShadowVec;
		VECTOR_ref		m_NearShadowMax, m_NearShadowMin;
		VECTOR_ref		m_MiddleShadowMax, m_MiddleShadowMin;
		VECTOR_ref		m_FarShadowMax, m_FarShadowMin;
		VECTOR_ref		m_LightVec;
		COLOR_F			m_LightColorF{ GetColorF(0, 0, 0, 0) };

		std::array<shaderparam, 2> m_ShaderParam;		//シェーダーパラメーター
		bool			m_IsFirstLoop{ true };			//初回チェック
		bool					m_isUpdateFarShadow{ false };
	public://ゲッター
		void			Set_Next(const std::shared_ptr<TEMPSCENE>& Next_scenes_ptr_t) noexcept { Next_ptr = Next_scenes_ptr_t; }
		auto&			Get_Next(void) noexcept { return Next_ptr; }

		void			SetAmbientLight(const VECTOR_ref& Lightvec, const COLOR_F& LightColor) noexcept {
			m_LightVec = Lightvec;
			m_LightColorF = LightColor;//GetColorF(0.12f, 0.11f, 0.10f, 0.0f)
		}
		void			SetShadowDir(const VECTOR_ref& Vec, int shadowSelect) noexcept {
			m_ShadowVec[shadowSelect] = Vec;
		}

		void			SetFarShadow(const VECTOR_ref& FarShadowMinSize, const VECTOR_ref& FarShadowMaxSize) noexcept {
			m_FarShadowMax = FarShadowMaxSize;
			m_FarShadowMin = FarShadowMinSize;
		}
		void			SetMiddleShadow(const VECTOR_ref& MiddleShadowMinSize, const VECTOR_ref& MiddleShadowMaxSize) noexcept {
			m_MiddleShadowMax = MiddleShadowMaxSize;
			m_MiddleShadowMin = MiddleShadowMinSize;
		}
		void			SetNearShadow(const VECTOR_ref& NearShadowMinSize, const VECTOR_ref& NearShadowMaxSize) noexcept {
			m_NearShadowMax = NearShadowMaxSize;
			m_NearShadowMin = NearShadowMinSize;
		}
		const auto&		GetShadowVec(int shadowSelect) const noexcept { return m_ShadowVec[shadowSelect]; }
		const auto&		GetNearShadowMax(void) const noexcept { return m_NearShadowMax; }
		const auto&		GetNearShadowMin(void) const noexcept { return m_NearShadowMin; }
		const auto&		GetMiddleShadowMax(void) const noexcept { return m_MiddleShadowMax; }
		const auto&		GetMiddleShadowMin(void) const noexcept { return m_MiddleShadowMin; }
		const auto&		GetFarShadowMax(void) const noexcept { return m_FarShadowMax; }
		const auto&		GetFarShadowMin(void) const noexcept { return m_FarShadowMin; }
		const auto&		GetLightVec(void) const noexcept { return m_LightVec; }
		const auto&		GetLightColorF(void) const noexcept { return m_LightColorF; }

		const auto&		is_lens(void) const noexcept { return m_ShaderParam[0].use; }
		const auto&		xp_lens(void) const noexcept { return m_ShaderParam[0].param[0]; }
		const auto&		yp_lens(void) const noexcept { return m_ShaderParam[0].param[1]; }
		const auto&		size_lens(void) const noexcept { return m_ShaderParam[0].param[2]; }
		const auto&		zoom_lens(void) const noexcept { return m_ShaderParam[0].param[3]; }

		void			Set_is_lens(bool value) noexcept { m_ShaderParam[0].use = value; }
		void			Set_xp_lens(float value) noexcept { m_ShaderParam[0].param[0] = value; }
		void			Set_yp_lens(float value) noexcept { m_ShaderParam[0].param[1] = value; }
		void			Set_size_lens(float value) noexcept { m_ShaderParam[0].param[2] = value; }
		void			Set_zoom_lens(float value) noexcept { m_ShaderParam[0].param[3] = value; }

		const auto&		is_Blackout(void) const noexcept { return m_ShaderParam[1].use; }
		const auto&		Per_Blackout(void) const noexcept { return m_ShaderParam[1].param[0]; }

		void			Set_is_Blackout(bool value) noexcept { m_ShaderParam[1].use = value; }
		void			Set_Per_Blackout(float value) noexcept { m_ShaderParam[1].param[0] = value; }

		const auto&		GetIsFirstLoop(void) const noexcept { return m_IsFirstLoop; }

		const auto&		GetisEnd(void) const noexcept { return this->m_SelEnd; }
		void			SetisEnd(bool value) noexcept { this->m_SelEnd = value; }

		const auto&		GetIsUpdateDraw(void) const noexcept { return this->m_IsUpdateDraw; }
		void			SetIsUpdateDraw(bool value) noexcept { this->m_IsUpdateDraw = value; }

		const auto&		GetisUpdateFarShadow(void) const noexcept { return this->m_isUpdateFarShadow; }
		void SetisUpdateFarShadow(bool value) noexcept { m_isUpdateFarShadow = value; }
	public://コンストラクタ
		TEMPSCENE(void) noexcept;
		~TEMPSCENE(void) noexcept;
	public://メイン更新
		//事前読み込み　ローダーなど別のシーンで呼ぶ
		void Load(void) noexcept { Load_Sub(); }

		void Set(void) noexcept;
		bool Update() noexcept;
		void Dispose(void) noexcept;

		void BG_Draw(void) noexcept { BG_Draw_Sub(); }
		void Depth_Draw(void) noexcept { Depth_Draw_Sub(); }
		void MainDraw(void) noexcept { MainDraw_Sub(); }
		void MainDrawbyDepth(void) noexcept { MainDrawbyDepth_Sub(); }
		void DrawUI_Base(void) noexcept { DrawUI_Base_Sub(); }
		void DrawUI_In(void) noexcept { DrawUI_In_Sub(); }

		void ShadowDraw_Far(void) noexcept { ShadowDraw_Far_Sub(); }
		void ShadowDraw_NearFar(void) noexcept { ShadowDraw_NearFar_Sub(); }
		void ShadowDraw(void) noexcept { ShadowDraw_Sub(); }
	protected://継承物
		virtual void Load_Sub(void) noexcept {}

		virtual void Set_Sub(void) noexcept {}
		virtual bool Update_Sub(void) noexcept { return true; }
		virtual void Dispose_Sub(void) noexcept {}

		virtual void BG_Draw_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispXSize, GetColor(192, 192, 192), TRUE);
		}

		virtual void Depth_Draw_Sub(void) noexcept {}

		virtual void ShadowDraw_Far_Sub(void) noexcept {}
		virtual void ShadowDraw_NearFar_Sub(void) noexcept {}
		virtual void ShadowDraw_Sub(void) noexcept {}

		virtual void MainDraw_Sub(void) noexcept {}
		virtual void MainDrawbyDepth_Sub(void) noexcept {}

		virtual void DrawUI_Base_Sub(void) noexcept {}//VR上で表示する
		virtual void DrawUI_In_Sub(void) noexcept {}//カメラ
	};
	//
	class SceneControl {
		bool					m_SelEnd{ false };
		std::shared_ptr<TEMPSCENE> m_ScenesPtr{ nullptr };
		ShaderUseClass::ScreenVertex	m_ScreenVertex;								// 頂点データ
		std::array<ShaderUseClass, 2>	m_Shader2D;

		float					m_PauseFlashCount{ 0.f };
		bool					m_isUpdateFarShadow{ false };
	public:
		SceneControl(const std::shared_ptr<TEMPSCENE>& ptr) noexcept;
		~SceneControl(void) noexcept;
	public:
		const auto& isEnd(void) const noexcept { return this->m_SelEnd; }
		void SetisUpdateFarShadow(bool value) noexcept { m_isUpdateFarShadow = value; }
		
	public:
		void StartScene(void) noexcept;		//開始
		bool Execute(void) noexcept;		//
		void Draw(void) noexcept;			//描画
		void NextScene(void) noexcept;		//
	};
};
