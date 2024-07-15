#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	class TEMPSCENE {
	private:
		bool			m_IsFirstLoop{true};			//初回チェック
		bool			m_IsLoading{false};

		std::array<std::shared_ptr<TEMPSCENE>, 10> Next_ptr{nullptr};
		size_t			m_Next_Select{0};

		bool			m_Is3DActive{true};
	public:
		void		SetNextSceneList(int index, const std::shared_ptr<TEMPSCENE>& Next_scenes_ptr_t) noexcept { Next_ptr.at(static_cast<std::size_t>(index)) = Next_scenes_ptr_t; }
		auto&		Get_Next(void) noexcept { return Next_ptr.at(this->m_Next_Select); }
		void		SetNextSelect(size_t value) noexcept { this->m_Next_Select = value; }
		void		Set3DActive(bool value) noexcept { m_Is3DActive = value; }
	public://ゲッター
		const auto&		GetIsFirstLoop(void) const noexcept { return m_IsFirstLoop; }
		const auto&		Get3DActive(void) const noexcept { return m_Is3DActive; }
	public://コンストラクタ
		TEMPSCENE(void) noexcept {}
		TEMPSCENE(const TEMPSCENE&) = delete;
		TEMPSCENE(TEMPSCENE&& o) = delete;
		TEMPSCENE& operator=(const TEMPSCENE&) = delete;
		TEMPSCENE& operator=(TEMPSCENE&& o) = delete;

		virtual ~TEMPSCENE(void) noexcept {}
	public://メイン更新
		void Load(void) noexcept {
			if (!m_IsLoading) {
				m_IsLoading = true;
				Load_Sub();
			}
		}
		void Set(void) noexcept {
			this->m_Next_Select = 0;
			Set_Sub();
			m_IsFirstLoop = true;
		}
		bool Update(void) noexcept {
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
		virtual void BG_Draw_Sub(void) noexcept;
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
	class SceneControl : public SingletonBase<SceneControl> {
	private:
		friend class SingletonBase<SceneControl>;
	private:
		std::vector<std::shared_ptr<TEMPSCENE>> m_ScenesPtr{};
		std::shared_ptr<TEMPSCENE> m_NowScenesPtr;
		std::array<float, 60> FPSAvgs{};
		int m_FPSAvg = 0;
	private:
		SceneControl(void) noexcept {}
		SceneControl(const SceneControl&) = delete;
		SceneControl(SceneControl&& o) = delete;
		SceneControl& operator=(const SceneControl&) = delete;
		SceneControl& operator=(SceneControl&& o) = delete;

		~SceneControl(void) noexcept {
			for (auto& s : this->m_ScenesPtr) {
				s->Dispose();
			}
		}
	public:
		const auto& GetNowScene(void) const noexcept { return this->m_NowScenesPtr; }
	public:
		void AddList(const std::shared_ptr<TEMPSCENE>& ptr) noexcept {
			this->m_ScenesPtr.emplace_back(ptr);
			if (this->m_ScenesPtr.size() == 1) {
				this->m_NowScenesPtr = this->m_ScenesPtr.back();
			}
		}
		void StartScene(void) noexcept;
		bool FirstExecute(void) noexcept;
		bool Execute(void) noexcept;
		void NextScene(void) noexcept;
	};
};
