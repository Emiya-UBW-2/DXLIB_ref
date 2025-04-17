#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	namespace Controls {
		enum class ControlType :int {
			XBox,
			PS4,
			PC,
			Max,
		};
		static const char* ControlTypeStr[] = {
			"XBox",
			"PS4",
			"PC",
		};

		enum class PADS :int {
			MOVE_W,
			MOVE_A,
			MOVE_S,
			MOVE_D,
			MOVE_STICK,
			DIR_UP,
			DIR_DOWN,
			DIR_LEFT,
			DIR_RIGHT,
			DIR_STICK,
			LEAN_L,
			LEAN_R,
			RELOAD,
			INTERACT,
			THROW,
			MELEE,
			JUMP,
			INVENTORY,
			RUN,
			WALK,
			SHOT,
			AIM,
			ULT,
			SQUAT,
			PRONE,
			CHECK,
			Escape,

			MAX,
		};
		static const char* PADSStr[static_cast<int>(PADS::MAX)] = {
			"MOVE_W",
			"MOVE_A",
			"MOVE_S",
			"MOVE_D",
			"MOVE_STICK",
			"DIR_UP",
			"DIR_DOWN",
			"DIR_LEFT",
			"DIR_RIGHT",
			"DIR_STICK",
			"LEAN_L",
			"LEAN_R",
			"RELOAD",
			"INTERACT",
			"THROW",
			"MELEE",
			"JUMP",
			"INVENTORY",
			"RUN",
			"WALK",
			"SHOT",
			"AIM",
			"ULT",
			"SQUAT",
			"PRONE",
			"CHECK",
			"ESCAPE",
		};

		static const size_t KeyNum = 109 + 3;
		static const int KeyID[KeyNum] = {
			KEY_INPUT_BACK,
			KEY_INPUT_TAB,
			KEY_INPUT_RETURN,

			KEY_INPUT_LSHIFT,
			KEY_INPUT_RSHIFT,
			KEY_INPUT_LCONTROL,
			KEY_INPUT_RCONTROL,
			KEY_INPUT_ESCAPE,
			KEY_INPUT_SPACE,
			KEY_INPUT_PGUP,
			KEY_INPUT_PGDN,
			KEY_INPUT_END,
			KEY_INPUT_HOME,
			KEY_INPUT_LEFT,
			KEY_INPUT_UP,
			KEY_INPUT_RIGHT,
			KEY_INPUT_DOWN,
			KEY_INPUT_INSERT,
			KEY_INPUT_DELETE,

			KEY_INPUT_MINUS,
			KEY_INPUT_YEN,
			KEY_INPUT_PREVTRACK,
			KEY_INPUT_PERIOD,
			KEY_INPUT_SLASH,
			KEY_INPUT_LALT,
			KEY_INPUT_RALT,
			KEY_INPUT_SCROLL,
			KEY_INPUT_SEMICOLON,
			KEY_INPUT_COLON,
			KEY_INPUT_LBRACKET,
			KEY_INPUT_RBRACKET,
			KEY_INPUT_AT,
			KEY_INPUT_BACKSLASH,
			KEY_INPUT_COMMA,
			KEY_INPUT_KANJI,
			KEY_INPUT_CONVERT,
			KEY_INPUT_NOCONVERT,
			KEY_INPUT_KANA,
			KEY_INPUT_APPS,
			KEY_INPUT_CAPSLOCK,
			KEY_INPUT_SYSRQ,
			KEY_INPUT_PAUSE,
			KEY_INPUT_LWIN,
			KEY_INPUT_RWIN,

			KEY_INPUT_NUMLOCK,
			KEY_INPUT_NUMPAD0,
			KEY_INPUT_NUMPAD1,
			KEY_INPUT_NUMPAD2,
			KEY_INPUT_NUMPAD3,
			KEY_INPUT_NUMPAD4,
			KEY_INPUT_NUMPAD5,
			KEY_INPUT_NUMPAD6,
			KEY_INPUT_NUMPAD7,
			KEY_INPUT_NUMPAD8,
			KEY_INPUT_NUMPAD9,
			KEY_INPUT_MULTIPLY,
			KEY_INPUT_ADD,
			KEY_INPUT_SUBTRACT,
			KEY_INPUT_DECIMAL,
			KEY_INPUT_DIVIDE,
			KEY_INPUT_NUMPADENTER,

			KEY_INPUT_F1,
			KEY_INPUT_F2,
			KEY_INPUT_F3,
			KEY_INPUT_F4,
			KEY_INPUT_F5,
			KEY_INPUT_F6,
			KEY_INPUT_F7,
			KEY_INPUT_F8,
			KEY_INPUT_F9,
			KEY_INPUT_F10,
			KEY_INPUT_F11,
			KEY_INPUT_F12,

			KEY_INPUT_A,
			KEY_INPUT_B,
			KEY_INPUT_C,
			KEY_INPUT_D,
			KEY_INPUT_E,
			KEY_INPUT_F,
			KEY_INPUT_G,
			KEY_INPUT_H,
			KEY_INPUT_I,
			KEY_INPUT_J,
			KEY_INPUT_K,
			KEY_INPUT_L,
			KEY_INPUT_M,
			KEY_INPUT_N,
			KEY_INPUT_O,
			KEY_INPUT_P,
			KEY_INPUT_Q,
			KEY_INPUT_R,
			KEY_INPUT_S,
			KEY_INPUT_T,
			KEY_INPUT_U,
			KEY_INPUT_V,
			KEY_INPUT_W,
			KEY_INPUT_X,
			KEY_INPUT_Y,
			KEY_INPUT_Z,

			KEY_INPUT_0 ,
			KEY_INPUT_1,
			KEY_INPUT_2,
			KEY_INPUT_3,
			KEY_INPUT_4,
			KEY_INPUT_5,
			KEY_INPUT_6,
			KEY_INPUT_7,
			KEY_INPUT_8,
			KEY_INPUT_9,

			MOUSE_INPUT_LEFT | 0xF00,
			MOUSE_INPUT_RIGHT | 0xF00,
			MOUSE_INPUT_MIDDLE | 0xF00,
		};
		static const char* KeyStr[KeyNum] = {
			"BACK",
			"TAB",
			"RETURN",

			"LSHIFT",
			"RSHIFT",
			"LCONTROL",
			"RCONTROL",
			"ESCAPE",
			"SPACE",
			"PGUP",
			"PGDN",
			"END",
			"HOME",
			"LEFT",
			"UP",
			"RIGHT",
			"DOWN",
			"INSERT",
			"DELETE",

			"MINUS",
			"YEN",
			"PREVTRACK",
			"PERIOD",
			"SLASH",
			"LALT",
			"RALT",
			"SCROLL",
			"SEMICOLON",
			"COLON",
			"LBRACKET",
			"RBRACKET",
			"AT",
			"BACKSLASH",
			"COMMA",
			"KANJI",
			"CONVERT",
			"NOCONVERT",
			"KANA",
			"APPS",
			"CAPSLOCK",
			"SYSRQ",
			"PAUSE",
			"LWIN",
			"RWIN",

			"NUMLOCK",
			"NUMPAD0",
			"NUMPAD1",
			"NUMPAD2",
			"NUMPAD3",
			"NUMPAD4",
			"NUMPAD5",
			"NUMPAD6",
			"NUMPAD7",
			"NUMPAD8",
			"NUMPAD9",
			"MULTIPLY",
			"ADD",
			"SUBTRACT",
			"DECIMAL",
			"DIVIDE",
			"NUMPADENTER",

			"F1",
			"F2",
			"F3",
			"F4",
			"F5",
			"F6",
			"F7",
			"F8",
			"F9",
			"F10",
			"F11",
			"F12",

			"A",
			"B",
			"C",
			"D",
			"E",
			"F",
			"G",
			"H",
			"I",
			"J",
			"K",
			"L",
			"M",
			"N",
			"O",
			"P",
			"Q",
			"R",
			"S",
			"T",
			"U",
			"V",
			"W",
			"X",
			"Y",
			"Z",

			"0",
			"1",
			"2",
			"3",
			"4",
			"5",
			"6",
			"7",
			"8",
			"9",

			"LMOUSE",
			"RMOUSE",
			"WMOUSE",
		};
		static const char* KeyGuidePath[KeyNum] = {
			"BackSpace",
			"Tab",
			"Enter",

			"Shift",
			"Shift",
			"Ctrl",
			"Ctrl",
			"ESC",
			"none",
			"PageUp",
			"PageDown",
			"End",
			"Home",
			"hidari",
			"ue",
			"migi",
			"sita",
			"Ins",
			"Del",

			"mainasu",
			"en",
			"quotation",
			"period",
			"slash",
			"Alt",
			"Alt",
			"ScrollLock",
			"semikoron",
			"coron",
			"kakko3",
			"kakko4",
			"atto",
			"backslash",
			"comma",
			"kanji",
			"Henkan",
			"Muhenkan",
			"kana",
			"F",
			"CapsLock",
			"PrintScreen",
			"Pause",
			"W",
			"W",

			"NumLock",
			"0",
			"1",
			"2",
			"3",
			"4",
			"5",
			"6",
			"7",
			"8",
			"9",
			"asutarisku",
			"purasu",
			"mainasu",
			"period",
			"slash",
			"Enter",

			"F1",
			"F2",
			"F3",
			"F4",
			"F5",
			"F6",
			"F7",
			"F8",
			"F9",
			"F10",
			"F11",
			"F12",

			"A",
			"B",
			"C",
			"D",
			"E",
			"F",
			"G",
			"H",
			"I",
			"J",
			"K",
			"L",
			"M",
			"N",
			"O",
			"P",
			"Q",
			"R",
			"S",
			"T",
			"U",
			"V",
			"W",
			"X",
			"Y",
			"Z",

			"0",
			"1",
			"2",
			"3",
			"4",
			"5",
			"6",
			"7",
			"8",
			"9",

			"LM",
			"RM",
			"MM",
		};

		static const size_t DS4Num = 12 - 1 + 2 + 4 + 1;
		static const int DS4ID[DS4Num] = {
			0,
			1,
			2,
			3,
			4,
			5,
			6,
			7,
			9,
			10,
			11,
			0xF001,
			0xF002,
			0xF010,
			0xF020,
			0xF040,
			0xF080,
			8,
		};
		static const char* DS4Str[DS4Num] = {
			"SQUARE",
			"NG",
			"OK",
			"TRIANGLE",
			"L1",
			"R1",
			"L2",
			"R2",
			"OPTION",
			"L3",
			"R3",
			"LSTICK",
			"RSTICK",
			"LEFT",
			"UP",
			"RIGHT",
			"DOWN",
			"ESCAPE",
		};
		static const char* DS4GuidePath[DS4Num] = {
			"square",
			"ng",
			"ok",
			"triangle",
			"L1",
			"R1",
			"L2",
			"R2",
			"option",
			"L_stick",
			"R_stick",
			"L_stick",
			"R_stick",
			"left",
			"up",
			"right",
			"down",

			"share",
		};

		static const size_t XBoxNum = 12 - 1 + 2 + 4 + 1;
		static const int XBoxID[XBoxNum] = {
			14,
			12,
			13,
			15,

			8,
			9,

			0xF100,
			0xF200,

			4,

			6,
			7,

			0xF001,
			0xF002,
			2,
			0,
			3,
			1,

			5,
		};
		static const char* XBoxStr[XBoxNum] = {
			"SQUARE",
			"NG",
			"OK",
			"TRIANGLE",

			"L1",
			"R1",

			"L2",
			"R2",

			"OPTION",

			"L3",
			"R3",

			"LSTICK",
			"RSTICK",
			"LEFT",
			"UP",
			"RIGHT",
			"DOWN",

			"ESCAPE",
		};
		static const char* XBoxGuidePath[XBoxNum] = {
			"square",
			"ng",
			"ok",
			"triangle",
			"L1",
			"R1",
			"L2",
			"R2",
			"option",
			"L_stick",
			"R_stick",
			"L_stick",
			"R_stick",
			"left",
			"up",
			"right",
			"down",

			"share",
		};
		//文字からKEY_INPUT_BACKなどのIDを取得
		static const auto GetStrtoID(const char* Str, ControlType controlType) noexcept {
			switch (controlType) {
			case ControlType::XBox:
				for (size_t loop = 0; loop < XBoxNum; ++loop) {
					if (strcmpDx(XBoxStr[loop], Str) == 0) {
						return XBoxID[loop];
					}
				}
				break;
			case ControlType::PS4:
				for (size_t loop = 0; loop < DS4Num; ++loop) {
					if (strcmpDx(DS4Str[loop], Str) == 0) {
						return DS4ID[loop];
					}
				}
				break;
			case ControlType::PC:
				for (size_t loop = 0; loop < KeyNum; ++loop) {
					if (strcmpDx(KeyStr[loop], Str) == 0) {
						return KeyID[loop];
					}
				}
				break;
			default:
				break;
			}
			return InvalidID;
		}
		//KEY_INPUT_BACKなどのIDから文字を取得
		static const std::string	GetIDtoStr(int ID, ControlType controlType) noexcept {
			switch (controlType) {
			case ControlType::XBox:
				for (size_t loop = 0; loop < XBoxNum; ++loop) {
					if (XBoxID[loop] == ID) {
						return XBoxStr[loop];
					}
				}
				break;
			case ControlType::PS4:
				for (size_t loop = 0; loop < DS4Num; ++loop) {
					if (DS4ID[loop] == ID) {
						return DS4Str[loop];
					}
				}
				break;
			case ControlType::PC:
				for (size_t loop = 0; loop < KeyNum; ++loop) {
					if (KeyID[loop] == ID) {
						return KeyStr[loop];
					}
				}
				break;
			default:
				break;
			}
			return "NONE";
		};

		static const int GetIDtoOffset(int ID, ControlType controlType) noexcept {
			switch (controlType) {
			case ControlType::XBox:
				for (size_t loop = 0; loop < XBoxNum; ++loop) {
					if (XBoxID[loop] == ID) {
						return static_cast<int>(loop + KeyNum + XBoxNum);
					}
				}
				break;
			case ControlType::PS4:
				for (size_t loop = 0; loop < DS4Num; ++loop) {
					if (DS4ID[loop] == ID) {
						return static_cast<int>(loop + KeyNum);
					}
				}
				break;
			case ControlType::PC:
				for (size_t loop = 0; loop < KeyNum; ++loop) {
					if (KeyID[loop] == ID) {
						return static_cast<int>(loop);
					}
				}
				break;
			default:
				break;
			}
			return InvalidID;
		}

		static bool GetButtonPress(ControlType controlType, int ID) noexcept {
			switch (controlType) {
			case ControlType::XBox:
			{
				XINPUT_STATE input;
				GetJoypadXInputState(DX_INPUT_PAD1, &input);
				// ボタン
				if (ID >= 0xF100) {
					switch (ID) {
					case 0xF100:
						return input.LeftTrigger > 0;
					case 0xF200:
						return input.RightTrigger > 0;
					default:
						break;
					}
				}
				else if (0 <= ID && ID < 16) {
					return (input.Buttons[ID] != 0);
				}
			}
			break;
			case ControlType::PS4:
			{
				DINPUT_JOYSTATE input;
				GetJoypadDirectInputState(DX_INPUT_PAD1, &input);
				// ボタン
				if (ID >= 0xF010) {
					// 十字キー
					float deg = static_cast<float>(input.POV[0]) / 100.f;
					bool w_key = false;
					bool s_key = false;
					bool a_key = false;
					bool d_key = false;
					if (input.POV[0] != 0xffffffff) {
						w_key = (310.f <= deg || deg <= 50.f);
						s_key = (130.f <= deg && deg <= 230.f);
						a_key = (220.f <= deg && deg <= 320.f);
						d_key = (40.f <= deg && deg <= 140.f);
					}
					switch (ID) {
					case 0xF010:
						return a_key;
					case 0xF020:
						return w_key;
					case 0xF040:
						return d_key;
					case 0xF080:
						return s_key;
					default:
						break;
					}
				}
				else if (0 <= ID && ID < 32) {
					return (input.Buttons[ID] != 0);
				}
			}
			break;
			case ControlType::PC:
				// ボタン
				if (0 <= ID) {
					if ((ID & 0xF00) != 0) {
						if (GetWindowActiveFlag()) {
							return ((GetMouseInput() & (ID & 0xFF)) != 0);
						}
						else {
							return 0;
						}
					}
					else {
						if (GetWindowActiveFlag()) {
							return (CheckHitKey(ID) != 0);
						}
						else {
							return 0;
						}
					}
				}
				break;
			default:
				break;
			}
			return false;
		}
	}
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// キー押し判定(押した瞬間やリピート、離した瞬間などにも対応)
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class switchs {
		const float	m_RepeatWaitTime = 0.5f;			//リピート時の1～2打間の時間間隔
		const float	m_RepeatTime = 0.04f;				//リピート時の2打以降の判定間隔
	private:
		bool		m_prevpress{ false };				// 押し続けたかどうかの判定判定
		bool		m_press{ false };					// 押し続けたかどうかの判定判定
		bool		m_repeat{ false };					// 押し続けた際の繰り返し判定
		float		m_repeatcount{ 0.f };				// 繰り返し判定に使うタイマー
	public:
		//コンストラクタ
		switchs(void) noexcept {
			this->m_repeatcount = 0.f;
			this->m_prevpress = false;
			this->m_press = false;
		}
		//デストラクタ
		~switchs(void) noexcept {}
	public:
		// 更新
		void			Update(bool key) noexcept;
		// 押した瞬間
		bool			trigger(void) const noexcept { return this->m_press && !this->m_prevpress; }
		// 押している間
		bool			press(void) const noexcept { return this->m_press; }
		// 押している間リピート
		bool			repeat(void) const noexcept { return this->m_repeat; }
		// 離した瞬間
		bool			release_trigger(void) const noexcept { return !this->m_press && this->m_prevpress; }
		// 離している間
		bool			release(void) const noexcept { return !this->m_press; }
	};
	// マウスと矩形の判定
	extern bool IntoMouse(int x1, int y1, int x2, int y2) noexcept;
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	class PadControl : public SingletonBase<PadControl> {
	private:
		friend class SingletonBase<PadControl>;
	private:
		class PadsInfo {
			switchs	m_Key;
			int		m_assign = 0;
			int		m_reserve = 0;
			bool	m_IsUse{ true };
		public:
			void SetAssign(int select) noexcept {
				this->m_assign = select;
				this->m_reserve = this->m_assign;
			}
			void SetReserve(int select) noexcept { this->m_reserve = select; }
			void ResetAssign(void) noexcept { this->m_reserve = this->m_assign; }
			void FlipAssign(void) noexcept { this->m_assign = this->m_reserve; }
			void Update(bool Press) noexcept { this->m_Key.Update(Press); }
			void SetUse(bool value) noexcept { this->m_IsUse = value; }
		public:
			const auto& GetReserve(void) const noexcept { return this->m_reserve; }
			const auto& GetAssign(void) const noexcept { return this->m_assign; }
			const auto& IsUse(void) const noexcept { return this->m_IsUse; }
			const auto& GetKey(void) const noexcept { return this->m_Key; }
			const auto IsEnableSelectReserve(void) const noexcept { return this->m_reserve != InvalidID; }
		};
	private:
		std::array<PadsInfo, static_cast<size_t>(Controls::PADS::MAX)>	m_PadsInfo;								// ボタン入力を保持
		float													m_Look_XradAdd{ 0.f };						// 右スティック入力を保持
		float													m_Look_YradAdd{ 0.f };						// 右スティック入力を保持
		int														m_MouseX{ 0 };								//マウスのDXLIBからの値を保持しておく
		int														m_MouseY{ 0 };								//マウスのDXLIBからの値を保持しておく
		switchs													m_MouseClick;								//左クリック
		int														m_MouseWheelRot{ 0 };						//マウスホイールの回転量を保持しておく
		// 
		bool													m_MouseMoveEnable{ false };					//FPSなどのマウスを表示しない操作方法を用いるかどうか
		Controls::ControlType									m_ControlType{ Controls::ControlType::PC };	//現在のコントロールタイプ
		bool													m_PrevIsActiveLS{ false };
	private:
		// コンストラクタ
		PadControl(void) noexcept { Load(); }
		PadControl(const PadControl&) = delete;		// コピーしてはいけないので通常のコンストラクタ以外をすべてdelete
		PadControl(PadControl&& o) = delete;
		PadControl& operator=(const PadControl&) = delete;
		PadControl& operator=(PadControl&& o) = delete;
		// デストラクタはシングルトンなので呼ばれません
	public:
		//今認識しているコントロールタイプを得る
		const auto& GetControlType(void) const noexcept { return this->m_ControlType; }
		//視点移動に相当する入力を得る
		const auto& GetLS_X(void) const noexcept { return this->m_Look_XradAdd; }
		const auto& GetLS_Y(void) const noexcept { return this->m_Look_YradAdd; }
		//マウスの位置や入力を返す(UIのクリックなど専用)
		const auto& GetMS_X(void) const noexcept { return this->m_MouseX; }
		const auto& GetMS_Y(void) const noexcept { return this->m_MouseY; }
		const auto& GetMouseClick(void) const noexcept { return this->m_MouseClick; }
		const auto& GetMouseWheelRot(void) const noexcept { return this->m_MouseWheelRot; }
		//各キーコンフィグに対応した入力を入れる
		const auto& GetPadsInfo(Controls::PADS select) const noexcept { return this->m_PadsInfo.at(static_cast<size_t>(select)); }

		void SetIsUseButton(Controls::PADS select, bool IsUse) noexcept { this->m_PadsInfo.at(static_cast<size_t>(select)).SetUse(IsUse); }
	private:
		//セーブデータが入る場所を占めるパス
		const char* GetSavePath(void) const noexcept {
			switch (GetControlType()) {
			case Controls::ControlType::XBox:
				return "Save/KeyConfig_XBox.txt";
			case Controls::ControlType::PS4:
				return "Save/KeyConfig_DS4.txt";
			case Controls::ControlType::PC:
				return "Save/KeyConfig_PC.txt";
			default:
				break;
			}
			return "NONE";
		};
		const char* GetBasePath(void) const noexcept {
			switch (GetControlType()) {
			case Controls::ControlType::XBox:
				return "data/KeyConfigBase_XBox.txt";
			case Controls::ControlType::PS4:
				return "data/KeyConfigBase_DS4.txt";
			case Controls::ControlType::PC:
				return "data/KeyConfigBase_PC.txt";
			default:
				break;
			}
			return "NONE";
		};
		//キーコンフィグの各ボタンの設定
		void ChangeConfigOnce(Controls::PADS select, int SetID) noexcept {
			bool isHit = false;
			// 変更不可のもので設定したいものとIDがかぶっている場合不可能
			for (auto& p : this->m_PadsInfo) {
				if (!p.IsUse() && (p.GetAssign() == SetID)) {
					isHit = true;
					break;
				}
			}
			if (isHit) { return; }
			// 既に適用済のものがあった場合そいつを無効化してやる
			for (auto& p : this->m_PadsInfo) {
				if (p.GetReserve() == SetID) {
					p.SetReserve(InvalidID);
					break;
				}
			}
			//設定
			this->m_PadsInfo.at(static_cast<size_t>(select)).SetReserve(SetID);
		}
	public:
		//FPSなどのマウスを表示しない操作方法を用いるかどうか指定
		void SetMouseMoveEnable(bool value) noexcept { this->m_MouseMoveEnable = value; }
		//キーコンフィグとしてReserveに暫定値を入れる処理
		//これを通る間、同じキーを押すと自身のキーを外す。キーアサインが外れているか違うキーを押すとそのキーを設定する
		bool ChangeConfig(Controls::PADS select) noexcept {
			auto& P = this->m_PadsInfo.at(static_cast<size_t>(select));
			auto Prev = P.GetReserve();

			//変更前と同じキーを押してる
			if (P.GetKey().press()) {
				if (P.GetKey().trigger()) {//押した瞬間だけ
					// SetIDとselectで設定しているボタンが同じだったら=> SetID==Assign
					// Reserveに何か入っていたら空に、入っていなかったらAssignとする
					if (P.IsEnableSelectReserve()) {
						P.SetReserve(InvalidID);
					}
					else {
						ChangeConfigOnce(select, P.GetAssign());
					}
					//元と違う設定になっていたらtrue
					return true;
				}
				//押し続けている場合は以下の処理を通さない
				return false;
			}
			//押されていたらそのキーに設定する
			switch (GetControlType()) {
			case Controls::ControlType::XBox:
				for (size_t loop = 0; loop < Controls::XBoxNum; ++loop) {
					if (Controls::GetButtonPress(GetControlType(), Controls::XBoxID[loop])) {
						ChangeConfigOnce(select, Controls::XBoxID[loop]);
						break;
					}
				}
				break;
			case Controls::ControlType::PS4:
				for (size_t loop = 0; loop < Controls::DS4Num; ++loop) {
					if (Controls::GetButtonPress(GetControlType(), Controls::DS4ID[loop])) {
						ChangeConfigOnce(select, Controls::DS4ID[loop]);
						break;
					}
				}
				break;
			case Controls::ControlType::PC:
				for (size_t loop = 0; loop < Controls::KeyNum; ++loop) {
					if (Controls::GetButtonPress(GetControlType(), Controls::KeyID[loop])) {
						ChangeConfigOnce(select, Controls::KeyID[loop]);
						break;
					}
				}
				break;
			default:
				break;
			}
			//元と違う設定になっていたらtrue
			return (Prev != P.GetReserve());
		}
		//適用していない変更のリセット
		void ResetAssign(void) noexcept {
			for (auto& p : this->m_PadsInfo) {
				p.ResetAssign();
			}
		}
		//適用していない変更を適用
		void FlipAssign(void) noexcept {
			for (auto& p : this->m_PadsInfo) {
				p.FlipAssign();
			}
		}
	public:
		//所定のファイルから今設定されているキーコンフィグを読み取る
		void Load(void) noexcept;
		//今設定している状態をファイルに保存
		void Save(void) const noexcept;
	public:
		//入力処理とコントロールタイプの更新
		void Update(void) noexcept;
	};
	// 入力
	class InputControl {
	private:
		float				m_AddxRad{ 0.f };
		float				m_AddyRad{ 0.f };
		float				m_xRad{ 0.f };
		float				m_yRad{ 0.f };
		size_t				m_Flag{ 0 };
		size_t				m_Prev{ 0 };
	public:
		InputControl(void) noexcept {}
		InputControl(const InputControl& tgt) noexcept { *this = tgt; }
		InputControl(InputControl&& tgt) noexcept { *this = tgt; }
		// InputControl& operator=(const InputControl&) = delete;
		// InputControl& operator=(InputControl&& o) = delete;
		virtual ~InputControl(void) noexcept {}
	public:
		void			ResetKeyInput(void) noexcept {
			this->m_Prev = this->m_Flag;
			this->m_Flag = 0;
		}
		void			ResetAllInput(void) noexcept {
			this->m_AddxRad = 0.f;
			this->m_AddyRad = 0.f;
			ResetKeyInput();
		}
		void			SetAddxRad(float AddxRad) noexcept { this->m_AddxRad = AddxRad; }
		void			SetAddyRad(float AddyRad) noexcept { this->m_AddyRad = AddyRad; }
		void			SetxRad(float xRad) noexcept { this->m_xRad = xRad; }
		void			SetyRad(float yRad) noexcept { this->m_yRad = yRad; }
		void			SetInputPADS(Controls::PADS select, bool value) noexcept {
			if (value) {
				this->m_Flag |= (static_cast<size_t>(1) << static_cast<size_t>(select));
			}
			else {
				this->m_Flag &= ~(static_cast<size_t>(1) << static_cast<size_t>(select));
			}
		}
	public:
		const auto&		GetAddxRad(void) const noexcept { return this->m_AddxRad; }
		const auto&		GetAddyRad(void) const noexcept { return this->m_AddyRad; }
		const auto&		GetxRad(void) const noexcept { return this->m_xRad; }
		const auto&		GetyRad(void) const noexcept { return this->m_yRad; }
		auto			GetPADSPress(Controls::PADS select) const noexcept { return (this->m_Flag & (static_cast<size_t>(1) << static_cast<size_t>(select))) != 0; }
		auto			GetPADSTrigger(Controls::PADS select) const noexcept { return !((this->m_Prev & (static_cast<size_t>(1) << static_cast<size_t>(select))) != 0) && GetPADSPress(select); }
	public:
		void			SetKeyInputFlags(const InputControl& o) noexcept {
			this->m_Prev = o.m_Prev;
			this->m_Flag = o.m_Flag;
		}
	public:
		auto operator+(const InputControl& o) const noexcept {
			InputControl tmp;
			tmp.m_AddxRad = this->m_AddxRad + o.m_AddxRad;
			tmp.m_AddyRad = this->m_AddyRad + o.m_AddyRad;
			tmp.m_xRad = this->m_xRad + o.m_xRad;
			tmp.m_yRad = this->m_yRad + o.m_yRad;
			tmp.m_Prev = this->m_Flag;
			tmp.m_Flag = this->m_Flag;
			return tmp;
		}
		auto operator-(const InputControl& o) const noexcept {
			InputControl tmp;
			tmp.m_AddxRad = this->m_AddxRad - o.m_AddxRad;
			tmp.m_AddyRad = this->m_AddyRad - o.m_AddyRad;
			tmp.m_xRad = this->m_xRad - o.m_xRad;
			tmp.m_yRad = this->m_yRad - o.m_yRad;
			tmp.m_Prev = this->m_Flag;
			tmp.m_Flag = this->m_Flag;
			return tmp;
		}
		auto operator*(float per) const noexcept {
			InputControl tmp;
			tmp.m_AddxRad = this->m_AddxRad * per;
			tmp.m_AddyRad = this->m_AddyRad * per;
			tmp.m_xRad = this->m_xRad * per;
			tmp.m_yRad = this->m_yRad * per;
			tmp.m_Prev = this->m_Flag;
			tmp.m_Flag = this->m_Flag;
			return tmp;
		}
		void operator=(const InputControl& o) noexcept {
			this->m_AddxRad = o.m_AddxRad;
			this->m_AddyRad = o.m_AddyRad;
			this->m_xRad = o.m_xRad;
			this->m_yRad = o.m_yRad;
			this->m_Prev = o.m_Prev;
			this->m_Flag = o.m_Flag;
		}
	};
}
