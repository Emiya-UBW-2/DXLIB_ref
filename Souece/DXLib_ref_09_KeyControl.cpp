#include "DXLib_ref.h"
// #include "DXLib_ref_09_KeyControl.hpp"

#include <fstream>

namespace DXLibRef {
	// �V���O���g���̎��Ԓ�`
	const PadControl* SingletonBase<PadControl>::m_Singleton = nullptr;
	// ���͏����Ɋ�Â��čX�V
	void			switchs::Update(bool key) noexcept {
		auto* DXLib_refParts = DXLib_ref::Instance();
		//�������ƋL�^
		m_press = key;
		//��������J�E���g�A�b�v�A��������J�E���g�_�E��
		m_presscount = static_cast<int8_t>(std::clamp(static_cast<int>(m_presscount) + (m_press ? 1 : -1), 0, 2));
		//���s�[�g����
		m_repeat = trigger();//�������u�Ԃ��A��������...
		//�����Ă����莞�Ԍ�A���Ԋu��true��
		if (m_press) {
			m_repeatcount -= DXLib_refParts->GetDeltaTime();
			if (m_repeatcount <= 0.f) {
				m_repeatcount += m_RepeatTime;
				m_repeat = true;
			}
		}
		else {
			//�������牟���Ă���̃J�E���g�����Z�b�g
			m_repeatcount = m_RepeatWaitTime;
		}
	}
	//
	bool IntoMouse(int x1, int y1, int x2, int y2) noexcept {
		auto* Pad = PadControl::Instance();
		int mx = Pad->GetMS_X();
		int my = Pad->GetMS_Y();
		return HitPointToRectangle(mx, my, x1, y1, x2, y2);
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
			for (size_t index = 0; auto& p : this->m_PadsInfo) {
				if (LEFT == Controls::PADSStr[index]) {
					p.SetAssign(Controls::GetStrtoID(RIGHT.c_str(), GetControlType()));
					break;
				}
				++index;
			}
		}
	}
	void PadControl::Save(void) const noexcept {
		std::ofstream outputfile(GetSavePath());
		for (size_t index = 0; auto & p : this->m_PadsInfo) {
			outputfile << (std::string)Controls::PADSStr[index] + "=" + Controls::GetIDtoStr(p.GetAssign(), GetControlType()) + "\n";
			++index;
		}
		outputfile.close();
	}
	void PadControl::Update(void) noexcept {
		auto* SceneParts = SceneControl::Instance();
		auto* WindowSizeParts = WindowSizeControl::Instance();
		auto* OptionParts = OptionManager::Instance();
		auto* KeyGuideParts = KeyGuide::Instance();
		// �R���g���[���^�C�v����
		{
			//��{��PC
			Controls::ControlType NextControlType = Controls::ControlType::PC;
			//�Q�[���p�b�h�����m������
			if (GetJoypadNum() > 0) {
				//�R���g���[���^�C�v�ɍ��킹���ݒ���s��
				NextControlType = (Controls::ControlType)OptionParts->GetParamInt(EnumSaveParam::ControlType);
			}
			//�ȑO�̐ݒ�ƈقȂ�ꍇ��
			if (m_ControlType != NextControlType) {
				Save();									//���̃R���g���[���p�b�h�̐ݒ��ۑ�
				m_ControlType = NextControlType;		//�ύX
				Load();									//���̃R���g���[���p�b�h�̏������[�h
				KeyGuideParts->SetGuideFlip();			//�K�C�h�\�����A�b�v�f�[�g
			}
		}
		//�}�E�X���W���擾���Ă���
		int PrevX = m_MouseX;
		int PrevY = m_MouseY;
		//�}�E�X���W���擾���Ă���
		GetMousePoint(&m_MouseX, &m_MouseY);
		m_MouseX = m_MouseX * 1000 / WindowSizeParts->GetUIY(1000);//UI���W�n�ɍ��킹�ĕϊ�
		m_MouseY = m_MouseY * 1000 / WindowSizeParts->GetUIY(1000);//UI���W�n�ɍ��킹�ĕϊ�
		// �^�C�v�ɍ��킹������
		switch (GetControlType()) {
		case Controls::ControlType::XBox:
		{
			SetMouseDispFlag(TRUE);//�}�E�X�͕K���\��
			XINPUT_STATE input;
			GetJoypadXInputState(DX_INPUT_PAD1, &input);
			// ���X�e�B�b�N
			{
				int LS_X = input.ThumbLX;
				int LS_Y = -input.ThumbLY;
				if (GetPadsInfo(Controls::PADS::MOVE_STICK).GetAssign() == 0xF002) {
					LS_X = input.ThumbRX;
					LS_Y = -input.ThumbRY;
				}
				//XInput��-1000~1000�ɂȂ�悤�ɕϊ�
				LS_X = 1000 * LS_X / 32768;
				LS_Y = 1000 * LS_Y / 32768;
				//�ϊ��ɂ��덷������������̂Ńf�b�h�]�[�����w��
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
				this->m_PadsInfo.at(static_cast<size_t>(Controls::PADS::MOVE_W)).Update(w_key);
				this->m_PadsInfo.at(static_cast<size_t>(Controls::PADS::MOVE_S)).Update(s_key);
				this->m_PadsInfo.at(static_cast<size_t>(Controls::PADS::MOVE_A)).Update(a_key);
				this->m_PadsInfo.at(static_cast<size_t>(Controls::PADS::MOVE_D)).Update(d_key);
			}
			// �E�X�e�B�b�N
			{
				int RS_X = input.ThumbRX;
				int RS_Y = -input.ThumbRY;
				if (GetPadsInfo(Controls::PADS::DIR_STICK).GetAssign() == 0xF001) {
					RS_X = input.ThumbLX;
					RS_Y = -input.ThumbLY;
				}
				RS_X = 1000 * RS_X / 32768;
				RS_Y = 1000 * RS_Y / 32768;

				if (!SceneParts->IsPause()) {
					m_Look_XradAdd = std::clamp(static_cast<float>(RS_X) / 100.f * 4.f * OptionParts->GetParamFloat(EnumSaveParam::Xsensing), -180.f, 180.f);
					m_Look_YradAdd = std::clamp(-static_cast<float>(RS_Y) / 100.f * 4.f * OptionParts->GetParamFloat(EnumSaveParam::Ysensing), -180.f, 180.f);
				}
				else {
					m_Look_XradAdd = 0.f;
					m_Look_YradAdd = 0.f;
				}
			}
		}
		break;
		case Controls::ControlType::PS4:
		{
			SetMouseDispFlag(TRUE);//�}�E�X�͕K���\��
			DINPUT_JOYSTATE input;
			GetJoypadDirectInputState(DX_INPUT_PAD1, &input);
			// ���X�e�B�b�N
			{
				int LS_X = input.X;
				int LS_Y = input.Y;
				if (GetPadsInfo(Controls::PADS::MOVE_STICK).GetAssign() == 0xF002) {
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
				this->m_PadsInfo.at(static_cast<size_t>(Controls::PADS::MOVE_W)).Update(w_key);
				this->m_PadsInfo.at(static_cast<size_t>(Controls::PADS::MOVE_S)).Update(s_key);
				this->m_PadsInfo.at(static_cast<size_t>(Controls::PADS::MOVE_A)).Update(a_key);
				this->m_PadsInfo.at(static_cast<size_t>(Controls::PADS::MOVE_D)).Update(d_key);
			}
			// �E�X�e�B�b�N
			{
				int RS_X = input.Z;
				int RS_Y = input.Rz;
				if (GetPadsInfo(Controls::PADS::DIR_STICK).GetAssign() == 0xF001) {
					RS_X = input.X;
					RS_Y = input.Y;
				}
				if (!SceneParts->IsPause()) {
					m_Look_XradAdd = static_cast<float>(RS_X) / 100.f * 4.f * OptionParts->GetParamFloat(EnumSaveParam::Xsensing);
					m_Look_YradAdd = -static_cast<float>(RS_Y) / 100.f * 4.f * OptionParts->GetParamFloat(EnumSaveParam::Ysensing);
				}
				else {
					m_Look_XradAdd = 0.f;
					m_Look_YradAdd = 0.f;
				}
			}
		}
		break;
		case Controls::ControlType::PC:
		{
			//�}�E�X�ɂ�����(FPS�ATPS)�̏ꍇ
			if (m_MouseMoveEnable) {
				//�őO�ʂŃ|�[�Y���łȂ��ꍇ
				if ((GetMainWindowHandle() == GetForegroundWindow()) && !SceneParts->IsPause()) {
					// ���_�ړ��͑O�t���[������̈ړ���
					m_Look_XradAdd = static_cast<float>(m_MouseX - BaseScreenWidth / 2 * 1000 / WindowSizeParts->GetUIY(1000)) * 2.f * OptionParts->GetParamFloat(EnumSaveParam::Xsensing);
					m_Look_YradAdd = -static_cast<float>(m_MouseY - BaseScreenHeight / 2 * 1000 / WindowSizeParts->GetUIY(1000)) * 2.f * OptionParts->GetParamFloat(EnumSaveParam::Ysensing);
					//�ړ������Z�b�g
					SetMousePoint(BaseScreenWidth / 2, BaseScreenHeight / 2);
					//�}�E�X��\�����Ȃ�
					SetMouseDispFlag(FALSE);
				}
				else {
					//�}�E�X��\��
					SetMouseDispFlag(TRUE);
					// ���_�ړ��͂Ȃ��Ƃ��ď㏑��
					m_Look_XradAdd = 0.f;
					m_Look_YradAdd = 0.f;
				}
			}
			else {
				// ���_�ړ��͑O�t���[������̈ړ���
				m_Look_XradAdd = static_cast<float>(m_MouseX - PrevX) * 2.f * OptionParts->GetParamFloat(EnumSaveParam::Xsensing);
				m_Look_YradAdd = -static_cast<float>(m_MouseY - PrevY) * 2.f * OptionParts->GetParamFloat(EnumSaveParam::Ysensing);
				//�}�E�X��\��
				SetMouseDispFlag(TRUE);
			}
			//�ړ�
			for (size_t index = 0; auto & p : this->m_PadsInfo) {
				Controls::PADS Pads = static_cast<Controls::PADS>(index);
				++index;
				if (
					Pads == Controls::PADS::MOVE_W ||
					Pads == Controls::PADS::MOVE_A ||
					Pads == Controls::PADS::MOVE_S ||
					Pads == Controls::PADS::MOVE_D
					) {
					p.Update(Controls::GetButtonPress(GetControlType(), GetPadsInfo(Pads).GetAssign()));
				}
			}
		}
		break;
		default:
			break;
		}
		// �{�^��
		for (size_t index = 0; auto& p : this->m_PadsInfo) {
			Controls::PADS Pads = static_cast<Controls::PADS>(index);
			++index;
			if (
				Pads == Controls::PADS::MOVE_W ||
				Pads == Controls::PADS::MOVE_A ||
				Pads == Controls::PADS::MOVE_S ||
				Pads == Controls::PADS::MOVE_D
				) {
				continue;
			}
			p.Update(Controls::GetButtonPress(GetControlType(), GetPadsInfo(Pads).GetAssign()));
		}
		// ���̑��������
		{
			m_MouseClick.Update(GetWindowActiveFlag() ? ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) : false);
			if (GetWindowActiveFlag()) {
				m_MouseWheelRot = GetMouseWheelRotVol();
			}
			else {
				GetMouseWheelRotVol();//�O��Ƃ̍���������d�g�݂Ȃ̂ŃA�N�e�B�u�łȂ��ꍇ���ʂ��Ă͂����悤��
				m_MouseWheelRot = 0;
			}
		}
		if (m_PrevIsActiveLS != (m_MouseMoveEnable && GetWindowActiveFlag() == TRUE)) {
			m_Look_XradAdd = 0.f;
			m_Look_YradAdd = 0.f;
		}
		m_PrevIsActiveLS = (m_MouseMoveEnable && GetWindowActiveFlag() == TRUE);
	}
};
