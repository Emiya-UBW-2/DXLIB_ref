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
		void Load(void) noexcept;
		void Set(void) noexcept;
		bool Update() noexcept;
		void Draw(void) noexcept;
		void Dispose(void) noexcept;
		void Dispose_Load(void) noexcept;
	protected://継承物
		virtual void Load_Sub(void) noexcept {}
		virtual void Set_Sub(void) noexcept {}
		virtual bool Update_Sub(void) noexcept { return true; }
		virtual void Dispose_Sub(void) noexcept {}
		virtual void Dispose_Load_Sub(void) noexcept {}

		virtual void BG_Draw_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			DrawBox_2D(0, 0, DrawParts->GetDispXSize(), DrawParts->GetDispYSize(), Gray25, TRUE);
		}
		virtual void Depth_Draw_Sub(void) noexcept {}
		virtual void CubeMap_Sub(void) noexcept {}
		virtual void MainDraw_Sub(void) noexcept {}
		virtual void MainDrawFront_Sub(void) noexcept {}
		virtual void MainDrawbyDepth_Sub(void) noexcept {}

		virtual void DrawUI_Base_Sub(void) noexcept {}
		virtual void DrawUI_In_Sub(void) noexcept {}

		virtual void ShadowDraw_Far_Sub(void) noexcept {}
		virtual void ShadowDraw_Sub(void) noexcept {}
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
