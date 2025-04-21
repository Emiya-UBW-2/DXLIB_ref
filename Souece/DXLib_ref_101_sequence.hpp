#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	class TEMPSCENE {
	private:
		bool			m_IsUpdateFarShadow{ true };			// 初回チェック
		bool			m_IsFirstLoop{ true };			// 初回チェック
		bool			m_IsLoading{ false };
		bool			m_IsLoadingEnd{ false };
		std::array<std::shared_ptr<TEMPSCENE>, 10>	m_pNextSceneList{ nullptr };
		size_t			m_Next_Select{ 0 };
		bool			m_Is3DActive{ true };
	public:
		void		SetNextSceneList(int index, const std::shared_ptr<TEMPSCENE>& Next_scenes_ptr_t) noexcept { this->m_pNextSceneList[static_cast<size_t>(index)] = Next_scenes_ptr_t; }
		auto&		Get_Next(void) noexcept { return this->m_pNextSceneList[this->m_Next_Select]; }
		void		SetNextSelect(size_t value) noexcept { this->m_Next_Select = value; }
		void		Set3DActive(bool value) noexcept { this->m_Is3DActive = value; }
		const auto	PopIsUpdateFarShadow(void) noexcept {
			auto Ret = this->m_IsUpdateFarShadow;
			this->m_IsUpdateFarShadow = false;
			return Ret;
		}
		void		SetIsUpdateFarShadowActive() noexcept { this->m_IsUpdateFarShadow = true; }
	public:// ゲッター
		const auto& GetIsFirstLoop(void) const noexcept { return this->m_IsFirstLoop; }
		const auto& Get3DActive(void) const noexcept { return this->m_Is3DActive; }
	public:// コンストラクタ
		TEMPSCENE(void) noexcept {}
		TEMPSCENE(const TEMPSCENE&) = delete;
		TEMPSCENE(TEMPSCENE&&) = delete;
		TEMPSCENE& operator=(const TEMPSCENE&) = delete;
		TEMPSCENE& operator=(TEMPSCENE&&) = delete;
		virtual ~TEMPSCENE(void) noexcept {}
	public:// メイン更新
		void Load(void) noexcept {
			if (!this->m_IsLoading) {
				this->m_IsLoading = true;
				Load_Sub();
			}
		}
		void LoadEnd(void) noexcept {
			if (!this->m_IsLoadingEnd) {
				this->m_IsLoadingEnd = true;
				LoadEnd_Sub();
			}
		}
		void Set(void) noexcept {
			this->m_Next_Select = 0;
			Set_Sub();
			this->m_IsFirstLoop = true;
		}
		bool Update(void) noexcept {
			auto ans = Update_Sub();
			this->m_IsFirstLoop = false;
			return ans;
		}
		void Dispose(void) noexcept { Dispose_Sub(); }
		void Dispose_Load(void) noexcept {
			if (this->m_IsLoading) {
				this->m_IsLoading = false;
				this->m_IsLoadingEnd = false;
				Dispose_Load_Sub();
			}
		}
	public:
		void CubeMap(void) noexcept { CubeMap_Sub(); }
		void BG_Draw(void) noexcept { BG_Draw_Sub(); }
		void SetShadowDraw_Rigid(void) noexcept { SetShadowDraw_Rigid_Sub(); }
		void SetShadowDraw(void) noexcept { SetShadowDraw_Sub(); }
		void MainDraw(int Range) noexcept { MainDraw_Sub(Range); }
		void MainDrawFront(int Range) noexcept { MainDrawFront_Sub(Range); }
		void DrawUI_Base(void) noexcept { DrawUI_Base_Sub(); }
		void DrawUI_In(void) noexcept { DrawUI_In_Sub(); }
		void ShadowDraw_Far(void) noexcept { ShadowDraw_Far_Sub(); }
		void ShadowDraw(void) noexcept { ShadowDraw_Sub(); }
	protected:// 継承物
		virtual void Load_Sub(void) noexcept {}
		virtual void LoadEnd_Sub(void) noexcept {}
		virtual void Set_Sub(void) noexcept {}
		virtual bool Update_Sub(void) noexcept { return true; }

		virtual void CubeMap_Sub(void) const noexcept {}
		virtual void BG_Draw_Sub(void) const noexcept { FillGraph(GetDrawScreen(), 192, 192, 192); }
		virtual void SetShadowDraw_Rigid_Sub(void) const noexcept {}
		virtual void SetShadowDraw_Sub(void) const noexcept {}
		virtual void MainDraw_Sub(int) const noexcept {}
		virtual void MainDrawFront_Sub(int) const noexcept {}
		virtual void DrawUI_Base_Sub(void) const noexcept {}
		virtual void DrawUI_In_Sub(void) const noexcept {}
		virtual void ShadowDraw_Far_Sub(void) const noexcept {}
		virtual void ShadowDraw_Sub(void) const noexcept {}

		virtual void Dispose_Sub(void) noexcept {}
		virtual void Dispose_Load_Sub(void) noexcept {}
	};
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	class SceneControl : public SingletonBase<SceneControl> {
	private:
		friend class SingletonBase<SceneControl>;
	private:
		// FPS表示用クラス
		class FPSDrawer {
			static const size_t			m_Size{ 60 };
		private:
			std::array<float, m_Size>	m_FPSAvgs{};
			size_t						m_FPSAvgCount{ 0 };
			float						m_FPSAvg{ 0.f };
		public:
			// FPS表示
			void	Initialize(void) noexcept;
			void	Update(void) noexcept;
			void	DrawFPSCounter(void) const noexcept;
		};
		// ポーズ画面表示用クラス
		class PauseDrawer {
			float						m_PauseFlashCount{ 0.f };
		public:
			void	Update(void) noexcept;
			void	DrawPause(void) const noexcept;
		};
	private:
		std::shared_ptr<TEMPSCENE>	m_NowScenesPtr;
		bool						m_IsEndScene{ false };		// 現在のシーンが終了したフラグ
		bool						m_IsEndGame{ false };		// ゲーム終了フラグ
		bool						m_IsExitSelect{ false };	// 終了ポップアップが開いているかのフラグ
		bool						m_IsRestartSelect{ false };	// 再起動ポップアップが開いているかのフラグ
		FPSDrawer					m_FPSDrawer;				// FPS表示用クラスの実体
		PauseDrawer					m_PauseDrawer;				// ポーズ画面表示用クラスの実体
		bool						m_IsPauseActive{ false };	// ポーズ中かどうかのフラグ
	private:
		SceneControl(void) noexcept {}
		SceneControl(const SceneControl&) = delete;
		SceneControl(SceneControl&&) = delete;
		SceneControl& operator=(const SceneControl&) = delete;
		SceneControl& operator=(SceneControl&&) = delete;
	public:
		const auto& GetNowScene(void) const noexcept { return this->m_NowScenesPtr; }
		void		SetIsUpdateFarShadowActive() noexcept { this->m_NowScenesPtr->SetIsUpdateFarShadowActive(); }
	private:
		void	DrawMain(const Camera3DInfo& camInfo, bool Is3D) const noexcept;
		void	DrawUICommon(void) const noexcept;
		void	DrawUIFront(void) const noexcept;
	public:
		const auto&		IsEndGame(void) const noexcept { return this->m_IsEndGame; }
		const auto&		IsPause(void) const noexcept { return this->m_IsPauseActive; }
		const auto&		IsExit(void) const noexcept { return this->m_IsExitSelect; }
		const auto&		IsRestart(void) const noexcept { return this->m_IsRestartSelect; }
		const auto		IsEndScene(void) const noexcept { return this->m_IsEndScene || this->m_IsEndGame; }

		void			SetEndGame(void) noexcept { this->m_IsEndGame = true; }
		void			ChangePause(bool value) noexcept;
		void			SetFirstScene(const std::shared_ptr<TEMPSCENE>& ptr) noexcept { this->m_NowScenesPtr = ptr; }
	public:
		void			Initialize(void) noexcept;
		void			Update(void) noexcept;
		void			DrawMainLoop(void) const noexcept;
		void			NextMainLoop(void) noexcept;
	};
}
