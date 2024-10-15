#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	class TEMPSCENE {
	private:
		bool			m_IsFirstLoop{ true };			//初回チェック
		bool			m_IsLoading{ false };

		std::array<std::shared_ptr<TEMPSCENE>, 10> Next_ptr{ nullptr };
		size_t			m_Next_Select{ 0 };

		bool			m_Is3DActive{ true };
		float			m_ShadowScale{ 1.f };
	public:
		void		SetNextSceneList(int index, const std::shared_ptr<TEMPSCENE>& Next_scenes_ptr_t) noexcept { Next_ptr.at(static_cast<std::size_t>(index)) = Next_scenes_ptr_t; }
		auto&			Get_Next(void) noexcept { return Next_ptr.at(this->m_Next_Select); }
		void		SetNextSelect(size_t value) noexcept { this->m_Next_Select = value; }
		void		Set3DActive(bool value) noexcept { m_Is3DActive = value; }
		void SetShadowScale(float value) noexcept { m_ShadowScale = value; }
	public://ゲッター
		const auto&			GetIsFirstLoop(void) const noexcept { return m_IsFirstLoop; }
		const auto&			Get3DActive(void) const noexcept { return m_Is3DActive; }
		const auto&			GetShadowScale(void) const noexcept { return m_ShadowScale; }
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

		void CubeMapDraw(void) noexcept;
		void ShadowDraw_Far(void) noexcept;
		void ShadowDraw(void) noexcept;

		void Draw3DVR(std::function<void()> doingUI) noexcept;
		void Draw3D(std::function<void()> doingUI) noexcept;
		void Draw2D(std::function<void()> doingUI) noexcept;

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
		virtual void SetShadowDraw_Rigid_Sub(void) noexcept {}
		virtual void SetShadowDraw_Sub(void) noexcept {}
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
		std::shared_ptr<TEMPSCENE>	m_NowScenesPtr;
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
		const auto&			GetNowScene(void) const noexcept { return this->m_NowScenesPtr; }
	public:
		void	AddList(const std::shared_ptr<TEMPSCENE>& ptr) noexcept {
			this->m_ScenesPtr.emplace_back(ptr);
			if (this->m_ScenesPtr.size() == 1) {
				this->m_NowScenesPtr = this->m_ScenesPtr.back();
			}
		}
	public:
		void	NextScene(void) noexcept {
			GetNowScene()->Dispose();							//解放
			if (GetNowScene() != GetNowScene()->Get_Next()) {
				GetNowScene()->Dispose_Load();
			}
			this->m_NowScenesPtr = GetNowScene()->Get_Next();		//遷移
		}
	};
};
