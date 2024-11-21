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
		class PopUpDrawClass {
		private:
			bool m_Active{ false };
			bool m_ActiveSwitch{ false };
			float m_ActivePer{ 0.f };
			char m_WindwoName[64]{};

			int WinSizeX{ 720 };
			int WinSizeY{ 720 };

			std::function<void(int xmin, int ymin, int xmax, int ymax, bool EndSwitch)> m_Doing{ nullptr };
			std::function<void()> m_ExitDoing{ nullptr };
			std::function<void()> m_GuideDoing{ nullptr };
		public:
			PopUpDrawClass(void) noexcept {}
			PopUpDrawClass(const PopUpDrawClass&) = delete;
			PopUpDrawClass(PopUpDrawClass&& o) = delete;
			PopUpDrawClass& operator=(const PopUpDrawClass&) = delete;
			PopUpDrawClass& operator=(PopUpDrawClass&& o) = delete;

			~PopUpDrawClass(void) noexcept {}
		public:
			void			Set(const char* WindowName, int sizex, int sizey,
				std::function<void(int xmin, int ymin, int xmax, int ymax, bool EndSwitch)> doing,
				std::function<void()> ExitDoing,
				std::function<void()> GuideDoing
			) noexcept {
				strcpy_sDx(m_WindwoName, 64, WindowName);
				WinSizeX = sizex;
				WinSizeY = sizey;
				m_Doing = doing;
				m_ExitDoing = ExitDoing;
				m_GuideDoing = GuideDoing;
			}
			void			Start(void) noexcept;
			void			End(void) noexcept;
			void			Update(void) noexcept;
			void			Draw(int xcenter, int ycenter) noexcept;
		public:
			auto IsEnd(void) const noexcept { return !m_Active && !(m_ActivePer > 1.f / 255.f); }
		};
	private:
		std::array<PopUpDrawClass, 24> que;
		size_t NowSel{ 0 };
		size_t LastSel{ 0 };
		bool PrevPause{ false };
	private:
		PopUp(void) noexcept {}
		PopUp(const PopUp&) = delete;
		PopUp(PopUp&& o) = delete;
		PopUp& operator=(const PopUp&) = delete;
		PopUp& operator=(PopUp&& o) = delete;
	public:
		auto IsActivePop(void) const noexcept { return (NowSel != LastSel); }
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
			que.at(static_cast<size_t>(NowSel)).Draw(xcenter, ycenter);
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
			std::string m_Name;
			int m_Info{ 0 };
			std::function<void()> m_LeftPush;
			std::function<void()> m_RightPush;
			std::function<void()> m_OKPush;
			std::function<void()> m_AnyDoing;
			std::function<void(int xpos, int ypos, bool isMine)> m_Draw;
		public:
			float selanim{ 0.f };
		public:
			const auto& GetName(void) const noexcept { return m_Name; }
			const auto& GetInfoTextID(void) const noexcept { return m_Info; }

			void GetLeftPush(void) const noexcept { m_LeftPush(); }
			void GetRightPush(void) const noexcept { m_RightPush(); }
			void GetOKPush(void) const noexcept { m_OKPush(); }
			void GetAnyDoing(void) const noexcept { m_AnyDoing(); }
		public:
			void Init(const char* name, int infoTextID, std::function<void()> LeftPush, std::function<void()> RightPush, std::function<void()> OKPush,
				std::function<void()> AnyDoing,
				std::function<void(int xpos, int ypos, bool isMine)> draw) noexcept {
				selanim = 0;

				m_Name = name;
				m_Info = infoTextID;
				m_LeftPush = LeftPush;
				m_RightPush = RightPush;
				m_OKPush = OKPush;
				m_AnyDoing = AnyDoing;
				m_Draw = draw;
			}
			void Draw(int xpos, int ypos, bool isMine) const noexcept;
		};
		class OptionTabsInfo {
		private:
		protected:
			int m_id{ 0 };
			std::string m_name;
			std::vector<OptionElementsInfo> m_Elements;
		protected:
			virtual void Init_Sub(void) noexcept {}
		public:
			const auto& GetID(void) const noexcept { return m_id; }
		public:
			OptionTabsInfo(void) noexcept {}
			virtual ~OptionTabsInfo(void) noexcept {}
		public:
			void Init(int ID, const char* name) noexcept {
				m_id = ID;
				m_name = name;
				Init_Sub();
			}
			void Execute(int* select, bool CanPress) noexcept;
			void Draw(int xpos, int ypos, int xsize, bool isActive, int* TabSel, int* select) noexcept;

			void DrawInfo(int xpos, int ypos, int select) noexcept;
		};

		class SoundTabsInfo :public OptionTabsInfo {
		public:
			SoundTabsInfo(void) noexcept {}
			virtual ~SoundTabsInfo(void) noexcept {}
		protected:
			void Init_Sub(void) noexcept override;
		};
		class GraphicTabsInfo :public OptionTabsInfo {
			static const int	FrameLimitsNum = 10;
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
			GraphicTabsInfo(GraphicTabsInfo&& o) = delete;
			GraphicTabsInfo& operator=(const GraphicTabsInfo&) = delete;
			GraphicTabsInfo& operator=(GraphicTabsInfo&& o) = delete;

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
		int m_tabsel{ 0 };
		int m_select{ 0 };
		std::array<std::unique_ptr<OptionTabsInfo>, 4> m_Tabs;
		bool						m_Active{ false };
		bool						m_ActiveSwitch{ false };
		bool						m_RestartSwitch{ false };
	public:
		void SetRestart(void) noexcept { m_RestartSwitch = true; }
		void SetActive(void) noexcept { m_ActiveSwitch = true; }
		auto IsRestartSwitch(void) noexcept {
			if (!m_Active && m_RestartSwitch) {
				m_RestartSwitch = false;
				return true;
			}
			return false;
		}
		const auto& IsActive(void) const noexcept { return m_Active; }
	private:
		OptionPopup(void) noexcept {}
		OptionPopup(const OptionPopup&) = delete;
		OptionPopup(OptionPopup&& o) = delete;
		OptionPopup& operator=(const OptionPopup&) = delete;
		OptionPopup& operator=(OptionPopup&& o) = delete;
	public:
		void Init(void) noexcept;
		void Update(void)noexcept;
	};
}
