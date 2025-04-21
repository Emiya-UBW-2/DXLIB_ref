#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// ポップアップ
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class PopUp : public SingletonBase<PopUp> {
	private:
		friend class SingletonBase<PopUp>;
	private:
		class PopUpDraw {
		private:
			bool																		m_Active{ false };
			bool																		m_ActiveSwitch{ false };
			float																		m_ActivePer{ 0.f };
			char																		m_WindwoName[64]{};

			int WinSizeX{ 720 };
			int WinSizeY{ 720 };

			std::function<void(int xmin, int ymin, int xmax, int ymax, bool EndSwitch)>	m_Doing{ nullptr };
			std::function<void()>														m_ExitDoing{ nullptr };
			std::function<void()>														m_GuideDoing{ nullptr };
		public:
			PopUpDraw(void) noexcept {}
			PopUpDraw(const PopUpDraw&) = delete;
			PopUpDraw(PopUpDraw&&) = delete;
			PopUpDraw& operator=(const PopUpDraw&) = delete;
			PopUpDraw& operator=(PopUpDraw&&) = delete;

			~PopUpDraw(void) noexcept {}
		public:
			void			Set(const char* WindowName, int sizex, int sizey,
				std::function<void(int xmin, int ymin, int xmax, int ymax, bool EndSwitch)> doing,
				std::function<void()> ExitDoing,
				std::function<void()> GuideDoing
			) noexcept {
				strcpy_sDx(this->m_WindwoName, 64, WindowName);
				WinSizeX = sizex;
				WinSizeY = sizey;
				this->m_Doing = doing;
				this->m_ExitDoing = ExitDoing;
				this->m_GuideDoing = GuideDoing;
			}
			void			Start(void) noexcept;
			void			End(void) noexcept;
			void			Update(void) noexcept;
			void			Draw(int xcenter, int ycenter) noexcept;
		public:
			auto IsEnd(void) const noexcept { return !this->m_Active && !(this->m_ActivePer > 1.f / 255.f); }
		};
	private:
		static const size_t			m_Size{ 24 };
		std::array<PopUpDraw, m_Size>	m_que;
		size_t						m_NowSel{ 0 };
		size_t						m_LastSel{ 0 };
		bool						m_PrevPause{ false };
	private:
		PopUp(void) noexcept {}
		PopUp(const PopUp&) = delete;
		PopUp(PopUp&&) = delete;
		PopUp& operator=(const PopUp&) = delete;
		PopUp& operator=(PopUp&&) = delete;
	public:
		auto IsActivePop(void) const noexcept { return (this->m_NowSel != this->m_LastSel); }
	public:
		void Add(const char* WindowName, int sizex, int sizey,
			std::function<void(int xmin, int ymin, int xmax, int ymax, bool EndSwitch)> doing,
			std::function<void()> ExitDoing,
			std::function<void()> GuideDoing,
			bool IsInsert = false) noexcept;
		void EndAll(void) noexcept;
		void Update(void) noexcept;
		void Draw(int xcenter, int ycenter) noexcept {
			if (!IsActivePop()) {
				return;
			}
			this->m_que[this->m_NowSel].Draw(xcenter, ycenter);
		}
	};
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// オプションポップアップ
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class OptionPopup : public SingletonBase<OptionPopup> {
	private:
		friend class SingletonBase<OptionPopup>;
	private:
		class OptionElementsInfo {
		private:
			std::string												m_Name;
			int														m_Info{ 0 };
			std::function<void()>									m_LeftPush;
			std::function<void()>									m_RightPush;
			std::function<void()>									m_OKPush;
			std::function<void()>									m_AnyDoing;
			std::function<void(int xpos, int ypos, bool isMine)>	m_Draw;
		public:
			float													m_selanim{ 0.f };
		public:
			const auto& GetName(void) const noexcept { return this->m_Name; }
			const auto& GetInfoTextID(void) const noexcept { return this->m_Info; }

			void GetLeftPush(void) const noexcept { this->m_LeftPush(); }
			void GetRightPush(void) const noexcept { this->m_RightPush(); }
			void GetOKPush(void) const noexcept { this->m_OKPush(); }
			void GetAnyDoing(void) const noexcept { this->m_AnyDoing(); }
		public:
			void Init(const char* name, int infoTextID, std::function<void()> LeftPush, std::function<void()> RightPush, std::function<void()> OKPush,
				std::function<void()> AnyDoing,
				std::function<void(int xpos, int ypos, bool isMine)> draw) noexcept {
				this->m_selanim = 0;

				this->m_Name = name;
				this->m_Info = infoTextID;
				this->m_LeftPush = LeftPush;
				this->m_RightPush = RightPush;
				this->m_OKPush = OKPush;
				this->m_AnyDoing = AnyDoing;
				this->m_Draw = draw;
			}
			void Draw(int xpos, int ypos, bool isMine) const noexcept;
		};
		class OptionTabsInfo {
		private:
		protected:
			size_t							m_id{ 0 };
			std::string						m_name;
			std::vector<OptionElementsInfo>	m_Elements;
			size_t							m_Size{};
		protected:
			virtual void Init_Sub(void) noexcept {}
		public:
			const auto& GetID(void) const noexcept { return this->m_id; }
		public:
			OptionTabsInfo(void) noexcept {}
			virtual ~OptionTabsInfo(void) noexcept {}
		public:
			void Init(size_t ID, const char* name) noexcept {
				this->m_id = ID;
				this->m_name = name;
				Init_Sub();
				this->m_Size = this->m_Elements.size();
			}
			void Update(size_t* select, bool CanPress) noexcept;
			void Draw(int xpos, int ypos, int xsize, bool isActive, size_t* TabSel, size_t* select) noexcept;

			void DrawInfo(int xpos, int ypos, size_t select) noexcept;
		};

		class SoundTabsInfo :public OptionTabsInfo {
		public:
			SoundTabsInfo(void) noexcept {}
			virtual ~SoundTabsInfo(void) noexcept {}
		protected:
			void Init_Sub(void) noexcept override;
		};
		class GraphicTabsInfo :public OptionTabsInfo {
			static const size_t	FrameLimitsNum = 10;
			const int	FrameLimits[FrameLimitsNum] = {
				30,
				60,
				75,
				90,
				120,
				144,
				240,
				300,
				360,
				1000,
			};
		private:
			int RefreshRate{ FrameLimits[1] };
		public:
			GraphicTabsInfo(void) noexcept {}
			GraphicTabsInfo(const GraphicTabsInfo&) = delete;
			GraphicTabsInfo(GraphicTabsInfo&&) = delete;
			GraphicTabsInfo& operator=(const GraphicTabsInfo&) = delete;
			GraphicTabsInfo& operator=(GraphicTabsInfo&&) = delete;

			virtual ~GraphicTabsInfo(void) noexcept {}
		protected:
			void Init_Sub(void) noexcept override;
		};
		class ControlTabsInfo :public OptionTabsInfo {
		public:
			ControlTabsInfo(void) noexcept {}
			virtual ~ControlTabsInfo(void) noexcept {}
		protected:
			void Init_Sub(void) noexcept override;
		private:
			void KeyDraw(int xpos, int ypos, bool isMine, Controls::PADS Sel) noexcept;
		};
		class ElseTabsInfo :public OptionTabsInfo {
		public:
			ElseTabsInfo(void) noexcept {}
			virtual ~ElseTabsInfo(void) noexcept {}
		protected:
			void Init_Sub(void) noexcept override;
		};
	private:
		static const size_t							m_Size{ 4 };

		size_t											m_tabsel{ 0 };
		size_t											m_select{ 0 };
		std::array<std::unique_ptr<OptionTabsInfo>, m_Size>	m_Tabs;
		bool											m_Active{ false };
		bool											m_ActiveSwitch{ false };
		bool											m_RestartSwitch{ false };
	public:
		void SetRestart(void) noexcept { this->m_RestartSwitch = true; }
		void SetActive(void) noexcept { this->m_ActiveSwitch = true; }
		auto IsRestartSwitch(void) noexcept {
			if (!this->m_Active && this->m_RestartSwitch) {
				this->m_RestartSwitch = false;
				return true;
			}
			return false;
		}
		const auto& IsActive(void) const noexcept { return this->m_Active; }
	private:
		OptionPopup(void) noexcept {}
		OptionPopup(const OptionPopup&) = delete;
		OptionPopup(OptionPopup&&) = delete;
		OptionPopup& operator=(const OptionPopup&) = delete;
		OptionPopup& operator=(OptionPopup&&) = delete;
	public:
		void Init(void) noexcept;
		void Update(void)noexcept;
	};
}
