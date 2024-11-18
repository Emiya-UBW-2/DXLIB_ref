#include "DXLib_ref.h"
// #include "DXLib_ref_09_KeyControl.hpp"

#include <fstream>

namespace DXLibRef {
	// シングルトンの実態定義
	const PadControl* SingletonBase<PadControl>::m_Singleton = nullptr;
	// 入力処理に基づいて更新
	void			switchs::Update(bool key) noexcept {
		auto* DXLib_refParts = DXLib_ref::Instance();
		//押したと記録
		m_press = key;
		//押したらカウントアップ、離したらカウントダウン
		m_presscount = static_cast<int8_t>(std::clamp(static_cast<int>(m_presscount) + (m_press ? 1 : -1), 0, 2));
		//リピート処理
		m_repeat = trigger();//押した瞬間か、もしくは...
		//押してから一定時間後、一定間隔でtrueに
		if (m_press) {
			m_repeatcount -= DXLib_refParts->GetDeltaTime();
			if (m_repeatcount <= 0.f) {
				m_repeatcount += m_RepeatTime;
				m_repeat = true;
			}
		}
		else {
			//離したら押してからのカウントをリセット
			m_repeatcount = m_RepeatWaitTime;
		}
	}
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	void PadControl::Load(void) noexcept {
		FileStreamDX FileStream;
		if (IsFileExist(GetSavePath())) {
			FileStream.Open(GetSavePath());
		}
		else {
			FileStream.Open(GetBasePath());
		}
		while (true) {
			if (FileStream.ComeEof()) { break; }
			auto ALL = FileStream.SeekLineAndGetStr();
			if (ALL == "") {
				continue;
			}
			auto LEFT = FileStreamDX::getleft(ALL);
			auto RIGHT = FileStreamDX::getright(ALL);
			for (size_t i = 0; i < static_cast<size_t>(PADS::MAX); ++i) {
				if (LEFT == PADSStr[i]) {
					m_PadsInfo.at(i).SetAssign(GetStrtoID(RIGHT.c_str()));
					break;
				}
			}
		}
	}
	void PadControl::Save(void) const noexcept {
		std::ofstream outputfile(GetSavePath());
		for (size_t i = 0; i < static_cast<size_t>(PADS::MAX); ++i) {
			PADS Pads = static_cast<PADS>(i);
			outputfile << (std::string)PADSStr[i] + "=" + GetIDtoStr(GetPadsInfo(Pads).GetAssign()) + "\n";
		}
		outputfile.close();
	}
	void PadControl::Update(void) noexcept {
		auto* SceneParts = SceneControl::Instance();
		auto* WindowSizeParts = WindowSizeControl::Instance();
		auto* OptionParts = OPTION::Instance();
		auto* KeyGuideParts = KeyGuide::Instance();
		// コントロールタイプ決定
		{
			//基本はPC
			ControlType NextControlType = ControlType::PC;
			//ゲームパッドを検知したら
			if (GetJoypadNum() > 0) {
				//コントロールタイプに合わせた設定を行う
				NextControlType = (ControlType)OptionParts->GetParamInt(EnumSaveParam::ControlType);
			}
			//以前の設定と異なる場合は
			if (m_ControlType != NextControlType) {
				Save();									//元のコントロールパッドの設定を保存
				m_ControlType = NextControlType;		//変更
				Load();									//次のコントロールパッドの情報をロード
				KeyGuideParts->SetGuideFlip();			//ガイド表示をアップデート
			}
		}
		//マウス座標を取得しておく
		int PrevX = MouseX;
		int PrevY = MouseY;
		//マウス座標を取得しておく
		GetMousePoint(&MouseX, &MouseY);
		MouseX = MouseX * 1000 / WindowSizeParts->GetUIY(1000);//UI座標系に合わせて変換
		MouseY = MouseY * 1000 / WindowSizeParts->GetUIY(1000);//UI座標系に合わせて変換
		// タイプに合わせた操作
		switch (m_ControlType) {
		case ControlType::XBox:
		{
			SetMouseDispFlag(TRUE);//マウスは必ず表示
			XINPUT_STATE input;
			GetJoypadXInputState(DX_INPUT_PAD1, &input);
			// 左スティック
			{
				int LS_X = input.ThumbLX;
				int LS_Y = -input.ThumbLY;
				if (GetPadsInfo(PADS::MOVE_STICK).GetAssign() == 0xF002) {
					LS_X = input.ThumbRX;
					LS_Y = -input.ThumbRY;
				}
				//XInputが-1000~1000になるように変換
				LS_X = 1000 * LS_X / 32768;
				LS_Y = 1000 * LS_Y / 32768;
				//変換により誤差が発生しうるのでデッドゾーンを指定
				if (abs(LS_X) < 10) { LS_X = 0; }
				if (abs(LS_Y) < 10) { LS_Y = 0; }

				float deg = rad2deg(atan2f(static_cast<float>(LS_X), static_cast<float>(-LS_Y)));
				bool w_key = false;
				bool s_key = false;
				bool a_key = false;
				bool d_key = false;
				if (!(LS_X == 0 && LS_Y == 0)) {
					w_key = (-50.f <= deg && deg <= 50.f);
					a_key = (-140.f <= deg && deg <= -40.f);
					s_key = (130.f <= deg || deg <= -130.f);
					d_key = (40.f <= deg && deg <= 140.f);
				}
				m_PadsInfo.at(static_cast<size_t>(PADS::MOVE_W)).Update(w_key);
				m_PadsInfo.at(static_cast<size_t>(PADS::MOVE_S)).Update(s_key);
				m_PadsInfo.at(static_cast<size_t>(PADS::MOVE_A)).Update(a_key);
				m_PadsInfo.at(static_cast<size_t>(PADS::MOVE_D)).Update(d_key);
			}
			// 右スティック
			{
				int RS_X = input.ThumbRX;
				int RS_Y = -input.ThumbRY;
				if (GetPadsInfo(PADS::DIR_STICK).GetAssign() == 0xF001) {
					RS_X = input.ThumbLX;
					RS_Y = -input.ThumbLY;
				}
				RS_X = 1000 * RS_X / 32768;
				RS_Y = 1000 * RS_Y / 32768;

				if (!SceneParts->IsPause()) {
					Look_XradAdd = std::clamp(static_cast<float>(RS_X) / 100.f * 4.f * OptionParts->GetParamFloat(EnumSaveParam::Xsensing), -180.f, 180.f);
					Look_YradAdd = std::clamp(-static_cast<float>(RS_Y) / 100.f * 4.f * OptionParts->GetParamFloat(EnumSaveParam::Ysensing), -180.f, 180.f);
				}
				else {
					Look_XradAdd = 0.f;
					Look_YradAdd = 0.f;
				}
			}
		}
		break;
		case ControlType::PS4:
		{
			SetMouseDispFlag(TRUE);//マウスは必ず表示
			DINPUT_JOYSTATE input;
			GetJoypadDirectInputState(DX_INPUT_PAD1, &input);
			// 左スティック
			{
				int LS_X = input.X;
				int LS_Y = input.Y;
				if (GetPadsInfo(PADS::MOVE_STICK).GetAssign() == 0xF002) {
					LS_X = input.Z;
					LS_Y = input.Rz;
				}

				float deg = rad2deg(atan2f(static_cast<float>(LS_X), static_cast<float>(-LS_Y)));
				bool w_key = false;
				bool s_key = false;
				bool a_key = false;
				bool d_key = false;
				if (!(LS_X == 0 && LS_Y == 0)) {
					w_key = (-50.f <= deg && deg <= 50.f);
					a_key = (-140.f <= deg && deg <= -40.f);
					s_key = (130.f <= deg || deg <= -130.f);
					d_key = (40.f <= deg && deg <= 140.f);
				}
				m_PadsInfo.at(static_cast<size_t>(PADS::MOVE_W)).Update(w_key);
				m_PadsInfo.at(static_cast<size_t>(PADS::MOVE_S)).Update(s_key);
				m_PadsInfo.at(static_cast<size_t>(PADS::MOVE_A)).Update(a_key);
				m_PadsInfo.at(static_cast<size_t>(PADS::MOVE_D)).Update(d_key);
			}
			// 右スティック
			{
				int RS_X = input.Z;
				int RS_Y = input.Rz;
				if (GetPadsInfo(PADS::DIR_STICK).GetAssign() == 0xF001) {
					RS_X = input.X;
					RS_Y = input.Y;
				}
				if (!SceneParts->IsPause()) {
					Look_XradAdd = static_cast<float>(RS_X) / 100.f * 4.f * OptionParts->GetParamFloat(EnumSaveParam::Xsensing);
					Look_YradAdd = -static_cast<float>(RS_Y) / 100.f * 4.f * OptionParts->GetParamFloat(EnumSaveParam::Ysensing);
				}
				else {
					Look_XradAdd = 0.f;
					Look_YradAdd = 0.f;
				}
			}
		}
		break;
		case ControlType::PC:
		{
			//マウスによる入力(FPS、TPS)の場合
			if (m_MouseMoveEnable) {
				//最前面でポーズ中でない場合
				if ((GetMainWindowHandle() == GetForegroundWindow()) && !SceneParts->IsPause()) {
					// 視点移動は前フレームからの移動量
					Look_XradAdd = static_cast<float>(MouseX - BaseScreenWidth / 2 * 1000 / WindowSizeParts->GetUIY(1000)) * 2.f * OptionParts->GetParamFloat(EnumSaveParam::Xsensing);
					Look_YradAdd = -static_cast<float>(MouseY - BaseScreenHeight / 2 * 1000 / WindowSizeParts->GetUIY(1000)) * 2.f * OptionParts->GetParamFloat(EnumSaveParam::Ysensing);
					//移動をリセット
					SetMousePoint(BaseScreenWidth / 2, BaseScreenHeight / 2);
					//マウスを表示しない
					SetMouseDispFlag(FALSE);
				}
				else {
					//マウスを表示
					SetMouseDispFlag(TRUE);
					// 視点移動はなしとして上書き
					Look_XradAdd = 0.f;
					Look_YradAdd = 0.f;
				}
			}
			else {
				// 視点移動は前フレームからの移動量
				Look_XradAdd = static_cast<float>(MouseX - PrevX) * 2.f * OptionParts->GetParamFloat(EnumSaveParam::Xsensing);
				Look_YradAdd = -static_cast<float>(MouseY - PrevY) * 2.f * OptionParts->GetParamFloat(EnumSaveParam::Ysensing);
				//マウスを表示
				SetMouseDispFlag(TRUE);
			}
			//移動
			m_PadsInfo.at(static_cast<size_t>(PADS::MOVE_W)).Update(GetButtonPress(GetPadsInfo(PADS::MOVE_W).GetAssign()));
			m_PadsInfo.at(static_cast<size_t>(PADS::MOVE_A)).Update(GetButtonPress(GetPadsInfo(PADS::MOVE_A).GetAssign()));
			m_PadsInfo.at(static_cast<size_t>(PADS::MOVE_S)).Update(GetButtonPress(GetPadsInfo(PADS::MOVE_S).GetAssign()));
			m_PadsInfo.at(static_cast<size_t>(PADS::MOVE_D)).Update(GetButtonPress(GetPadsInfo(PADS::MOVE_D).GetAssign()));
		}
		break;
		default:
			break;
		}
		// ボタン
		for (size_t i = 0; i < static_cast<size_t>(PADS::MAX); ++i) {
			PADS Pads = static_cast<PADS>(i);
			if (
				Pads == PADS::MOVE_W ||
				Pads == PADS::MOVE_A ||
				Pads == PADS::MOVE_S ||
				Pads == PADS::MOVE_D
				) {
				continue;
			}
			m_PadsInfo.at(i).Update(GetButtonPress(GetPadsInfo(Pads).GetAssign()));
		}
		// その他特殊入力
		{
			MouseClick.Update(GetWindowActiveFlag() ? ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) : false);
			if (GetWindowActiveFlag()) {
				MouseWheelRot = GetMouseWheelRotVol();
			}
			else {
				GetMouseWheelRotVol();//前回との差分が入る仕組みなのでアクティブでない場合も通してはおくように
				MouseWheelRot = 0;
			}
		}
	}
};
