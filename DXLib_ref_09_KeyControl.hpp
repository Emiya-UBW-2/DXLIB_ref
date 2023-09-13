#pragma once
#include "DXLib_ref.h"

namespace DXLib_ref {
	enum class ControlType :int {
		PS4,
		PC,
	};

	class PadControl : public SingletonBase<PadControl> {
	private:
		friend class SingletonBase<PadControl>;
	private:
		switchs m_OptionKey;
		//左スティック
		switchs Move_UpKey;
		switchs Move_DownKey;
		switchs Move_LeftKey;
		switchs Move_RightKey;
		switchs m_Move_Run;
		//右スティック
		float Look_XradAdd{ 0.f };
		float Look_YradAdd{ 0.f };
		switchs m_Look_On;				//フリールック
		//L1R1L2R2
		switchs m_QKey;
		switchs m_EKey;
		switchs m_ADSKey;
		switchs m_ShotKey;
		//ボタン
		switchs OKKey;
		switchs NGKey;
		switchs m_AccelKey;
		switchs m_BrakeKey;
		//ガイド等のコントロール
		bool		m_IsUpdate{ true };
		ControlType	m_ControlType{ ControlType::PC };
	private:
		PadControl() {
			m_IsUpdate = true;
		}
		~PadControl() {}
	public:
		void SetGuideUpdate() noexcept { m_IsUpdate = true; }
	public:
		//
		const auto& GetOptionKey() const noexcept { return m_OptionKey; }
		//
		const auto& GetUpKey() const noexcept { return Move_UpKey; }
		const auto& GetDownKey() const noexcept { return Move_DownKey; }
		const auto& GetLeftKey() const noexcept { return Move_LeftKey; }
		const auto& GetRightKey() const noexcept { return Move_RightKey; }
		const auto& GetRunKey() const noexcept { return m_Move_Run; }
		//
		const auto& GetLS_X() const noexcept { return Look_XradAdd; }
		const auto& GetLS_Y() const noexcept { return Look_YradAdd; }
		const auto& GetFreeLook() const noexcept { return m_Look_On; }
		//
		const auto& GetQKey() const noexcept { return m_QKey; }
		const auto& GetEKey() const noexcept { return m_EKey; }
		const auto& GetLookKey() const noexcept { return m_ADSKey; }
		const auto& GetShotKey() const noexcept { return m_ShotKey; }
		//
		const auto& GetOKKey() const noexcept { return OKKey; }
		const auto& GetNGKey() const noexcept { return NGKey; }
		const auto& GetAccelKey() const noexcept { return m_AccelKey; }
		const auto& GetBrakeKey() const noexcept { return m_BrakeKey; }
		//
	public:
		void Execute(void) noexcept;
		void ChangeGuide(std::function<void()>Guide_Pad_PS4, std::function<void()>Guide_Key) noexcept;
	};

	class KeyGuideClass : public SingletonBase<KeyGuideClass> {
	private:
		friend class SingletonBase<KeyGuideClass>;
	private:
		class Keys {
		public:
			int xsize{ 0 }, ysize{ 0 };
			GraphHandle GuideImg;
			std::string GuideString;
		public:
			void AddGuide(std::string_view ImgName, std::string_view GuideStr) noexcept;
			void Reset() noexcept;
			int Draw(int x, int y) const noexcept;
		};
	private:
		std::vector<std::unique_ptr<Keys>>	Key;
	public:
		void Reset(void) noexcept;
		void AddGuide(std::string_view ImgName, std::string_view GuideStr) noexcept;
		void Draw() const noexcept;
	};
};