#pragma once
#include "DXLib_ref.h"

namespace DXLib_ref {
	class TEMPSCENE {
	private:
		std::shared_ptr<TEMPSCENE> Next_ptr{nullptr};

		bool			m_IsFirstLoop{true};			//初回チェック
	public://ゲッター
		void			Set_Next(const std::shared_ptr<TEMPSCENE>& Next_scenes_ptr_t) noexcept { Next_ptr = Next_scenes_ptr_t; }
		auto&			Get_Next(void) noexcept { return Next_ptr; }

		const auto&		GetIsFirstLoop(void) const noexcept { return m_IsFirstLoop; }
	public://コンストラクタ
		TEMPSCENE(void) noexcept {}
		~TEMPSCENE(void) noexcept {}
	public://メイン更新
		void Load(void) noexcept { Load_Sub(); }		//事前読み込み　ローダーなど別のシーンで呼ぶ

		void Set(void) noexcept {
			m_IsFirstLoop = true;
			Set_Sub();
		}
		bool Update() noexcept {
			auto ans = Update_Sub();
			m_IsFirstLoop = false;
			return ans;
		}
		void Dispose(void) noexcept { Dispose_Sub(); }

		void BG_Draw(void) noexcept { BG_Draw_Sub(); }
		void Depth_Draw(void) noexcept { Depth_Draw_Sub(); }
		void MainDraw(void) noexcept { MainDraw_Sub(); }
		void MainDrawFront(void) noexcept { MainDrawFront_Sub(); }
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
			DrawBox_2D(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispXSize, GetColor(192, 192, 192), TRUE);
		}

		virtual void Depth_Draw_Sub(void) noexcept {}

		virtual void ShadowDraw_Far_Sub(void) noexcept {}
		virtual void ShadowDraw_NearFar_Sub(void) noexcept {}
		virtual void ShadowDraw_Sub(void) noexcept {}

		virtual void MainDraw_Sub(void) noexcept {}
		virtual void MainDrawFront_Sub(void) noexcept {}
		virtual void MainDrawbyDepth_Sub(void) noexcept {}

		virtual void DrawUI_Base_Sub(void) noexcept {}//VR上で表示する
		virtual void DrawUI_In_Sub(void) noexcept {}//カメラ
	};
	//
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
		void StartScene(void) noexcept;		//開始
		bool Execute(void) noexcept;		//
		void Draw(void) noexcept;			//描画
		void NextScene(void) noexcept;		//
	};
};
