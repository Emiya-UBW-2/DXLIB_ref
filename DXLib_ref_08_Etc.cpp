#include "DXLib_ref.h"

namespace DXLib_ref {
	const SaveDataClass* SingletonBase<SaveDataClass>::m_Singleton = nullptr;
	const SideLog* SingletonBase<SideLog>::m_Singleton = nullptr;
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	float GetEasingRatio(EasingType EasingType, float ratio) {
		auto* DrawParts = DXDraw::Instance();
		switch (EasingType) {
			case DXLib_ref::EasingType::OutExpo:
				return (1.f - std::powf(ratio, 60.f / DrawParts->GetFps()));
			default:
				return 1.f;
		}
	}
	static Vector3DX GetScreenPos(const Vector3DX&campos, const Vector3DX&camvec, const Vector3DX&camup, float fov, float near_t, float far_t, const Vector3DX&worldpos) noexcept {
		int ScrX = y_r(1920);
		int ScrY = y_r(1080);
		// �r���[�s��Ǝˉe�s��̎擾
		MATRIX mat_view;					// �r���[�s��
		VECTOR vec_from = campos.get();		// �J�����̈ʒu
		VECTOR vec_lookat = camvec.get();   // �J�����̒����_
		VECTOR vec_up = camup.get();        // �J�����̏����
		CreateLookAtMatrix(&mat_view, &vec_from, &vec_lookat, &vec_up);
		SetCameraNearFar(near_t, far_t);
		SetupCamera_Perspective(fov);
		MATRIX proj = GetCameraProjectionMatrix();
		// �r���[�|�[�g�s��i�X�N���[���s��j�̍쐬
		float w = (float)ScrX / 2.0f;
		float h = (float)ScrY / 2.0f;
		MATRIX viewport = {
			w , 0 , 0 , 0 ,
			0 ,-h , 0 , 0 ,
			0 , 0 , 1 , 0 ,
			w , h , 0 , 1
		};
		VECTOR screenPos, tmp = worldpos.get();
		// �r���[�ϊ��ƃv���W�F�N�V�����ϊ�
		tmp = VTransform(tmp, mat_view);
		tmp = VTransform(tmp, proj);
		// z�Ŋ�����-1~1�͈̔͂Ɏ��߂�
		tmp.x /= tmp.z; tmp.y /= tmp.z; tmp.z /= tmp.z;
		// �X�N���[���ϊ�
		screenPos = VTransform(tmp, viewport);
		screenPos.z = -1.f;
		if (Vector3DX::Dot(camvec - campos, worldpos - campos) > 0.f) {
			screenPos.z = 0.5f;
		}
		return screenPos;
	}
	//
	namespace WindowSystem {
		//��
		void SetBox(int xp1, int yp1, int xp2, int yp2, unsigned int colorSet) {
			DrawBox_2D(xp1, yp1, xp2, yp2, colorSet, true);
		};
		bool SetClickBox(int xp1, int yp1, int xp2, int yp2, unsigned int colorSet) {
			auto* Pad = PadControl::Instance();
			bool MouseOver = in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xp1, yp1, xp2, yp2);
			SetBox(xp1, yp1, xp2, yp2, MouseOver ? (Pad->GetMouseClick().press() ? Gray25 : White) : colorSet);
			return (MouseOver && Pad->GetMouseClick().trigger());
		};
		//����
		const bool GetMsgPos(int* xp1, int *yp1, int xp2, int yp2, int size, int xSize, FontHandle::FontXCenter FontX) {
			auto* DrawParts = DXDraw::Instance();
			*yp1 = *yp1 + (yp2 - *yp1) / 2;
			if ((*yp1 - size / 2) > DrawParts->GetDispYSize() || (*yp1 + size / 2) < 0) { return false; }				//��ʊO�͕\�����Ȃ�
			switch (FontX) {
				case FontHandle::FontXCenter::LEFT:
					*xp1 = *xp1 + y_r(6);
					if ((*xp1) > DrawParts->GetDispXSize() || (*xp1 + xSize) < 0) { return false; }						//��ʊO�͕\�����Ȃ�
					break;
				case FontHandle::FontXCenter::MIDDLE:
					*xp1 = *xp1 + (xp2 - *xp1) / 2;
					if ((*xp1 - xSize / 2) > DrawParts->GetDispXSize() || (*xp1 + xSize / 2) < 0) { return false; }		//��ʊO�͕\�����Ȃ�
					break;
				case FontHandle::FontXCenter::RIGHT:
					*xp1 = xp2 - y_r(6);
					if ((*xp1 - xSize) > DrawParts->GetDispXSize() || (*xp1) < 0) { return false; }						//��ʊO�͕\�����Ȃ�
					break;
				default:
					break;
			}
			return true;
		};
		//
		bool CheckBox(int xp1, int yp1, bool switchturn) {
			int xp3 = xp1 + EdgeSize;
			int yp3 = yp1 + EdgeSize;
			int xp4 = xp1 + LineHeight * 2 - EdgeSize;
			int yp4 = yp1 + LineHeight - EdgeSize;

			auto* Pad = PadControl::Instance();
			bool MouseOver = in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xp3, yp3, xp4, yp4);
			if (MouseOver && Pad->GetMouseClick().trigger()) {
				switchturn ^= 1;
				auto* SE = SoundPool::Instance();
				SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}
			unsigned int color = Gray25;
			SetBox(xp3 + y_r(5), yp3 + y_r(5), xp4 - y_r(5), yp4 - y_r(5), Black);
			xp4 = xp1 + LineHeight * (switchturn ? 1 : 0) - EdgeSize;
			SetBox(xp3 + y_r(5), yp3 + y_r(5), xp4 + y_r(5), yp4 - y_r(5), Gray50);
			xp3 = xp1 + LineHeight * (switchturn ? 1 : 0) + EdgeSize;
			xp4 = xp1 + LineHeight * (switchturn ? 2 : 1) - EdgeSize;
			SetBox(xp3, yp3, xp4, yp4, color);
			return switchturn;
		}
		//
		int UpDownBar(int xmin, int xmax, int yp, int value, int valueMin, int valueMax) {
			int xp = 0;
			{
				int xpmin = xmin + 1;
				int xpmax = xmax - 1;

				auto* Pad = PadControl::Instance();
				bool MouseOver = in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xpmin, yp, xpmin + (xpmax - xpmin), yp + LineHeight);
				if (MouseOver && Pad->GetMouseClick().trigger()) {
					auto* SE = SoundPool::Instance();
					SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
				if (MouseOver && Pad->GetMouseClick().press()) {
					value = ((valueMax - valueMin) * (Pad->GetMS_X() - xpmin) / (xpmax - xpmin)) + valueMin;
				}

				SetBox(xpmin, yp, xpmin + (xpmax - xpmin), yp + LineHeight, DarkGreen);
				SetBox(xpmin, yp, xpmin + (xpmax - xpmin)*std::clamp(value - valueMin, 0, valueMax - valueMin) / (valueMax - valueMin), yp + LineHeight,
					   MouseOver ? (Pad->GetMouseClick().press() ? Gray25 : White) : Green);
			}
			xp = (xmin + (xmax - xmin) / 2);
			SetMsgWW(xp, yp, xp, yp + LineHeight, LineHeight, FontHandle::FontXCenter::MIDDLE, White, Black, "%03d", value);

			return value;
		}
	};
	//
	void			moves::Update_Physics(float speed_randam, float rate) {
		auto* DrawParts = DXDraw::Instance();
		this->pos += this->vec*((float)((1000 - int(1000.f*speed_randam)) + GetRand(int(1000.f*speed_randam) * 2)) / 1000.f);
		this->vec.y += M_GR / powf((DrawParts->GetFps() / rate), 2.f);

		//this->gun_m.pos += this->gun_m.vec;
		//this->gun_m.vec.yadd(M_GR / std::powf(DrawParts->GetFps(), 2.f));
	}
	void			switchs::Execute(bool key) noexcept {
		auto* DrawParts = DXDraw::Instance();
		m_press = key;
		if (m_press) {
			m_presscount = std::clamp<int8_t>(m_presscount + 1, 0, 2);

			m_repeat = trigger();
			m_repeatcount -= 60.f / DrawParts->GetFps();
			if (m_repeatcount <= 0.f) {
				m_repeatcount += 2.f;
				m_repeat = true;
			}
		}
		else {
			m_presscount = std::clamp<int8_t>(m_presscount - 1, 0, 2);

			m_repeat = false;
			m_repeatcount = 30.f;
		}
		if (trigger()) {
			m_on ^= 1;
		}
	}
	void			Pendulum2D::Update() {
		auto* DrawParts = DXDraw::Instance();
		m_vel += (-9.8f / this->m_PendulumLength * std::sin(m_rad) - this->m_drag_coeff / this->m_PendulumMass * this->m_vel) / DrawParts->GetFps();
		m_rad += this->m_vel / DrawParts->GetFps();
	}
	//
	void SideLog::SideLogData::UpdateActive() noexcept {
		auto* DrawParts = DXDraw::Instance();
		if (m_Time > 0.f) {
			m_Time -= 1.f / DrawParts->GetFps();
		}
		else {
			m_Time = -1.f;
		}
		Easing(&m_Flip_Y, m_Flip, 0.9f, EasingType::OutExpo);
	}
	void SideLog::Draw() noexcept {
		auto* Fonts = FontPool::Instance();

		int xp1, yp1;
		xp1 = y_r(64);
		yp1 = y_r(256);

		for (auto& d : data) {
			if (d.ActivePer() > 0.f) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*d.ActivePer()), 0, 255));
				int yp = yp1 - y_r(24.f * d.GetFlip());
				DrawBox_2D(
					xp1 - y_r(6), yp + y_r(18),
					xp1 - y_r(6) + (int)(std::max(Fonts->Get(FontPool::FontType::Nomal_Edge).GetStringWidth(y_r(18), d.GetMsg()), y_r(200))*d.ActivePer()), yp + y_r(18) + y_r(5),
					Black, TRUE);
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
																	  xp1, yp, d.GetMsgColor(), Black, d.GetMsg());
			}
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	//
	void			PopUpDrawClass::Set(const char* WindowName, int sizex, int sizey, std::function<void(int xsize, int ysize, bool EndSwitch)> doing) noexcept {
		sprintf_s(m_WindwoName, 64, WindowName);
		WinSizeX = sizex;
		WinSizeY = sizey;
		m_Doing = doing;
	}
	void			PopUpDrawClass::Update(bool KeyTrigger) noexcept {
		auto* SE = SoundPool::Instance();
		auto* Pad = PadControl::Instance();
		m_ActiveSwitch = false;
		Easing(&m_ActivePer, m_Active ? 1.f : 0.f, m_Active ? 0.7f : 0.3f, EasingType::OutExpo);

		if (!m_Active) {
			if (KeyTrigger) {
				Pad->SetGuideUpdate();
				m_Active = true;
				m_ActiveSwitch = true;
				SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
			}
		}
		else {
			Pad->SetMouseMoveEnable(false);
			if (KeyTrigger) {
				SE->Get((int)SoundEnumCommon::UI_CANCEL).Play(0, DX_PLAYTYPE_BACK, TRUE);
				m_Active = false;
				m_ActiveSwitch = true;
				Pad->SetGuideUpdate();
			}
		}
	}
	void			PopUpDrawClass::Draw(void) noexcept {
		auto* Pad = PadControl::Instance();
		auto* Fonts = FontPool::Instance();
		auto* SE = SoundPool::Instance();

		int xp1, yp1;

		//�w�i
		if (m_ActivePer > 1.f / 255.f) {
			auto per = std::clamp(m_ActivePer * 0.3f, 0.f, 1.f);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
			DrawBox_2D(y_r(960) - WinSizeX / 2, y_r(540) - WinSizeY / 2, y_r(960) + WinSizeX / 2, y_r(540) + WinSizeY / 2, Gray50, true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

			//�^�C�g��
			{
				xp1 = y_r(960) - WinSizeX / 2 + y_r(32);
				yp1 = y_r(540) - WinSizeY / 2 + LineHeight / 4;

				int xpos = xp1 + y_r(6);
				int ypos = yp1 + (yp1 + LineHeight * 2 - yp1) / 2;

				Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(LineHeight * 2, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE,
																	   xpos, ypos, White, Black, m_WindwoName);
			}
			//
			if (m_Active) {
				xp1 = y_r(960) + WinSizeX / 2 - y_r(140);
				yp1 = y_r(540) - WinSizeY / 2 + LineHeight / 4 + LineHeight / 2;
				if (Pad->GetKey(PADS::RELOAD).trigger() || WindowSystem::SetMsgClickBox(xp1, yp1 + y_r(5), xp1 + y_r(108), yp1 + LineHeight * 2 - y_r(5), Red, LocalizePool::Instance()->Get(20))) {
					SE->Get((int)SoundEnumCommon::UI_CANCEL).Play(0, DX_PLAYTYPE_BACK, TRUE);
					m_Active = false;
					m_ActiveSwitch = true;
					Pad->SetGuideUpdate();
				}
			}
			//�w�i
			{
				xp1 = y_r(960) - WinSizeX / 2;
				yp1 = y_r(540) - WinSizeY / 2 + LineHeight * 3;

				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*0.3), 0, 255));
				DrawBox_2D(xp1 + y_r(24), yp1, xp1 + WinSizeX - y_r(24), y_r(540) + WinSizeY / 2 - LineHeight, Gray50, true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			//
			m_Doing(WinSizeX, WinSizeY, m_ActiveSwitch);
			//
		}
	}
};
