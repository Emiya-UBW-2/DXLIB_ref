#include "DXLib_ref.h"
//#include "DXLib_ref_08_Etc.hpp"

#define LineHeight	y_r(18)

namespace DXLib_ref {
	const SaveDataClass* SingletonBase<SaveDataClass>::m_Singleton = nullptr;
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------

	static VECTOR_ref GetScreenPos(const VECTOR_ref&campos, const VECTOR_ref&camvec, const VECTOR_ref&camup, float fov, float near_t, float far_t, const VECTOR_ref&worldpos) noexcept {
		int ScrX = y_r(1920);
		int ScrY = y_r(1080);
		// ビュー行列と射影行列の取得
		MATRIX mat_view;					// ビュー行列
		VECTOR vec_from = campos.get();		// カメラの位置
		VECTOR vec_lookat = camvec.get();   // カメラの注視点
		VECTOR vec_up = camup.get();        // カメラの上方向
		CreateLookAtMatrix(&mat_view, &vec_from, &vec_lookat, &vec_up);
		SetCameraNearFar(near_t, far_t);
		SetupCamera_Perspective(fov);
		MATRIX proj = GetCameraProjectionMatrix();
		// ビューポート行列（スクリーン行列）の作成
		float w = (float)ScrX / 2.0f;
		float h = (float)ScrY / 2.0f;
		MATRIX viewport = {
			w , 0 , 0 , 0 ,
			0 ,-h , 0 , 0 ,
			0 , 0 , 1 , 0 ,
			w , h , 0 , 1
		};
		VECTOR screenPos, tmp = worldpos.get();
		// ビュー変換とプロジェクション変換
		tmp = VTransform(tmp, mat_view);
		tmp = VTransform(tmp, proj);
		// zで割って-1~1の範囲に収める
		tmp.x /= tmp.z; tmp.y /= tmp.z; tmp.z /= tmp.z;
		// スクリーン変換
		screenPos = VTransform(tmp, viewport);
		screenPos.z = -1.f;
		if ((camvec - campos).dot(worldpos - campos) > 0.f) {
			screenPos.z = 0.5f;
		}
		return screenPos;
	}

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
			int xp1, yp1;
			xp1 = y_r(960) + WinSizeX / 2 - y_r(140);
			yp1 = y_r(540) - WinSizeY / 2 + LineHeight / 4 + LineHeight / 2;
			bool MouseOver = in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xp1, yp1, xp1 + y_r(108), yp1 + LineHeight * 2);
			if (KeyTrigger || Pad->GetKey(PADS::RELOAD).trigger() || (MouseOver && Pad->GetMouseClick().trigger())) {
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

		auto White = GetColor(255, 255, 255);
		auto Red = GetColor(255, 0, 0);
		auto Gray50 = GetColor(128, 128, 128);
		auto Black = GetColor(0, 0, 0);
		int xp1, yp1;

		//背景
		if (m_ActivePer > 1.f / 255.f) {
			auto per = std::clamp(m_ActivePer * 0.3f, 0.f, 1.f);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
			DrawBox_2D(y_r(960) - WinSizeX / 2, y_r(540) - WinSizeY / 2, y_r(960) + WinSizeX / 2, y_r(540) + WinSizeY / 2, Gray50, true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

			//タイトル
			{
				xp1 = y_r(960) - WinSizeX / 2 + y_r(32);
				yp1 = y_r(540) - WinSizeY / 2 + LineHeight / 4;

				int xpos = xp1 + y_r(6);
				int ypos = yp1 + (yp1 + LineHeight * 2 - yp1) / 2;

				Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(LineHeight * 2, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE,
																	   xpos, ypos, White, Black, m_WindwoName);
			}
			//
			{
				xp1 = y_r(960) + WinSizeX / 2 - y_r(140);
				yp1 = y_r(540) - WinSizeY / 2 + LineHeight / 4 + LineHeight / 2;

				bool MouseOver = in2_(Pad->GetMS_X(), Pad->GetMS_Y(), xp1, yp1, xp1 + y_r(108), yp1 + LineHeight * 2);
				int xpos = xp1 + (xp1 + y_r(108) - xp1) / 2;
				int ypos = yp1 + (yp1 + LineHeight * 2 - yp1) / 2;

				DrawBox_2D(xp1, yp1, xp1 + y_r(108), yp1 + LineHeight * 2, MouseOver ? White : Red, true);
				Fonts->Get(FontPool::FontType::Gothic_Edge).DrawString(std::min(LineHeight, yp1 + LineHeight * 2 - yp1), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::MIDDLE,
																	   xpos, ypos, White, Black, "閉じる");
			}
			//背景
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
