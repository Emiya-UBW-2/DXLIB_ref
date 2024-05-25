#pragma once
#include "DXLib_ref.h"

namespace DXLib_ref {
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	class TEMPSCENE {
	private:
		std::shared_ptr<TEMPSCENE> Next_ptr{nullptr};
		bool			m_IsFirstLoop{true};			//初回チェック
		bool			m_IsLoading{false};
	public://ゲッター
		void			Set_Next(const std::shared_ptr<TEMPSCENE>& Next_scenes_ptr_t) noexcept { Next_ptr = Next_scenes_ptr_t; }
		auto&			Get_Next(void) noexcept { return Next_ptr; }
		const auto&		GetIsFirstLoop(void) const noexcept { return m_IsFirstLoop; }
	public://コンストラクタ
		TEMPSCENE(void) noexcept {}
		~TEMPSCENE(void) noexcept {}
	public://メイン更新
		void Load(void) noexcept {
			if (!m_IsLoading) {
				m_IsLoading = true;
				Load_Sub();
			}
		}
		void Set(void) noexcept {
			Set_Sub();
			m_IsFirstLoop = true;
		}
		bool Update() noexcept {
			auto ans = Update_Sub();
			m_IsFirstLoop = false;
			return ans;
		}

		void CubeMapDraw(void) noexcept { CubeMap_Sub(); }
		void BG_Draw(void) noexcept { BG_Draw_Sub(); }
		void MainDraw(void) noexcept { MainDraw_Sub(); }
		void MainDrawFront(void) noexcept { MainDrawFront_Sub(); }
		void DrawUI_Base(void) noexcept { DrawUI_Base_Sub(); }
		void DrawUI_In(void) noexcept { DrawUI_In_Sub(); }
		void ShadowDraw_Far(void) noexcept { ShadowDraw_Far_Sub(); }
		void ShadowDraw(void) noexcept { ShadowDraw_Sub(); }

		void Dispose(void) noexcept { Dispose_Sub(); }
		void Dispose_Load(void) noexcept {
			if (m_IsLoading) {
				m_IsLoading = false;
				Dispose_Load_Sub();
			}
		}
	protected://継承物
		virtual void Load_Sub(void) noexcept {}
		virtual void Set_Sub(void) noexcept {}
		virtual bool Update_Sub(void) noexcept { return true; }

		virtual void CubeMap_Sub(void) noexcept {}
		virtual void BG_Draw_Sub(void) noexcept { DrawBox_2D(0, 0, y_r(1920), y_r(1080), Gray25, TRUE); }
		virtual void MainDraw_Sub(void) noexcept {}
		virtual void MainDrawFront_Sub(void) noexcept {}
		virtual void DrawUI_Base_Sub(void) noexcept {}
		virtual void DrawUI_In_Sub(void) noexcept {}
		virtual void ShadowDraw_Far_Sub(void) noexcept {}
		virtual void ShadowDraw_Sub(void) noexcept {}

		virtual void Dispose_Sub(void) noexcept {}
		virtual void Dispose_Load_Sub(void) noexcept {}

	};
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	class SceneControl {
		std::shared_ptr<TEMPSCENE> m_ScenesPtr{nullptr};
		std::array<float, 60> FPSAvgs;
		int m_FPSAvg = 0;
	public:
		SceneControl(const std::shared_ptr<TEMPSCENE>& ptr) noexcept { this->m_ScenesPtr = ptr; }
		~SceneControl(void) noexcept { this->m_ScenesPtr->Dispose(); }
	public:
		const auto& GetNowScene(void) const noexcept { return this->m_ScenesPtr; }
	public:
		void StartScene(void) noexcept;
		bool FirstExecute(void) noexcept;
		bool Execute(void) noexcept;
		void NextScene(void) noexcept;
	};
};
