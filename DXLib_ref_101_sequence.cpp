#include "DXLib_ref_101_sequence.hpp"

namespace DXLibRef {
	const SceneControl* SingletonBase<SceneControl>::m_Singleton = nullptr;
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------

	void TEMPSCENE::BG_Draw_Sub(void) noexcept { DrawBox_2D(0, 0, y_r(1920), y_r(1080), Gray25, TRUE); }

	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	void SceneControl::StartScene(void) noexcept {
		SetUseMaskScreenFlag(FALSE);//���ꕔ��ʂŃG�t�F�N�g���o�Ȃ��Ȃ邽�ߓ����
		auto* OptionParts = OPTION::Instance();
		auto* DrawParts = DXDraw::Instance();
		auto* Pad = PadControl::Instance();
		//�J�����̏����ݒ�
		DrawParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamBoolean(EnumSaveParam::usevr) ? 120 : OptionParts->GetParamInt(EnumSaveParam::fov)), 0.05f, 200.f);
		//�����Ɖe�̏�����
		DrawParts->SetAmbientLight(Vector3DX::vget(0.25f, -1.f, 0.25f), GetColorF(1.f, 1.f, 1.f, 0.0f));
		GetNowScene()->Set();
		Pad->SetGuideUpdate();
		//FPS�\��
		for (auto& f : FPSAvgs) {
			f = 60.f;
		}
		m_FPSAvg = 0;
	}
	bool SceneControl::FirstExecute(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
#ifdef DEBUG
		clsDx();
#endif // DEBUG
		return DrawParts->FirstExecute();
	}

	bool SceneControl::Execute(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		auto* ItemLogParts = SideLog::Instance();
		auto* PopUpParts = PopUp::Instance();
		auto* Pad = PadControl::Instance();
#ifdef DEBUG
		auto* DebugParts = DebugClass::Instance();		//�f�o�b�O
#endif // DEBUG

#ifdef DEBUG
		DebugParts->SetStartPoint();
#endif // DEBUG
		if (GetNowScene()->Get3DActive()) {
			if (DrawParts->UpdateShadowActive() || GetNowScene()->GetIsFirstLoop()) {
				DrawParts->Update_Shadow([&]() { GetNowScene()->ShadowDraw_Far(); }, Vector3DX::zero(), true);
			}
		}
		Vector3DX Pos = DrawParts->SetMainCamera().GetCamPos();Pos.y *= -1.f;
		DrawParts->Update_CubeMap([&]() { GetNowScene()->CubeMapDraw(); }, Pos);
		Pad->Execute();
		UISystem::Instance()->Update();
		auto SelEnd = !GetNowScene()->Update();		//�X�V
		OptionWindowClass::Instance()->Execute();
		DrawParts->Execute();
		ItemLogParts->Update();
		PopUpParts->Update();
		//�`��

#ifdef DEBUG
		DebugParts->SetPoint("-----DrawStart-----");
#endif // DEBUG
		//��ʂɔ��f
		if (GetNowScene()->Get3DActive()) {
			//�e���Z�b�g
			DrawParts->Update_Shadow([&] { GetNowScene()->ShadowDraw(); }, DrawParts->SetMainCamera().GetCamPos(), false);
			//��ʂɔ��f
			DrawParts->Draw(
				[&]() { GetNowScene()->BG_Draw(); },
				[&]() { GetNowScene()->MainDraw(); },
				[&]() { GetNowScene()->MainDrawFront(); },
				[&]() { GetNowScene()->DrawUI_Base(); },
				[&]() {
					GetNowScene()->DrawUI_In();
					//�ǉ��̕`�敨
					auto* ItemLogParts = SideLog::Instance();
					auto* PopUpParts = PopUp::Instance();
					auto* Fonts = FontPool::Instance();
					auto* OptionParts = OPTION::Instance();
					ItemLogParts->Draw();
					PopUpParts->Draw(y_UI(960), y_UI(540));
					{
						FPSAvgs.at(m_FPSAvg) = GetFPS();
						auto color = White;
						//�댯
						if (FPSAvgs.at(m_FPSAvg) < 45.f) {
							color = Red;
						}
						else if (FPSAvgs.at(m_FPSAvg) < 58.f) {
							color = Yellow;
						}
						//�\���I
						if (FPSAvgs.at(m_FPSAvg) > (OptionParts->GetParamInt(EnumSaveParam::FpsLimit) - 2)) {
							color = Green;
						}

						++m_FPSAvg %= ((int)FPSAvgs.size());

						float Avg = 0.f;
						for (auto& f : FPSAvgs) {
							Avg += f;
						}
						Avg = Avg / ((float)FPSAvgs.size());

						Fonts->Get(FontPool::FontType::Nomal_Edge, y_UI(18)).DrawString(-1, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
																			  y_UI((1920 - 8)), y_UI(8), White, Black, "%5.2f FPS", Avg);
					}
#ifdef DEBUG
					DebugParts->DebugWindow(y_UI(1920 - 350), y_UI(150));
#endif // DEBUG
				}
			);
		}
		else {
			//��ʂɔ��f
			PostPassEffect::Instance()->Set_DoFNearFar(0.1f, 5.f, 0.05f, 6.f);
			DrawParts->Draw2D(
				[&]() { GetNowScene()->MainDraw(); },
				[&]() { GetNowScene()->DrawUI_Base(); },
				[&]() {
					GetNowScene()->DrawUI_In();
					//�ǉ��̕`�敨
					auto* ItemLogParts = SideLog::Instance();
					auto* PopUpParts = PopUp::Instance();
					auto* Fonts = FontPool::Instance();
					auto* OptionParts = OPTION::Instance();
					ItemLogParts->Draw();
					PopUpParts->Draw(y_UI(960), y_UI(540));
					{
						FPSAvgs.at(m_FPSAvg) = GetFPS();
						auto color = White;
						//�댯
						if (FPSAvgs.at(m_FPSAvg) < 45.f) {
							color = Red;
						}
						else if (FPSAvgs.at(m_FPSAvg) < 58.f) {
							color = Yellow;
						}
						//�\���I
						if (FPSAvgs.at(m_FPSAvg) > (OptionParts->GetParamInt(EnumSaveParam::FpsLimit) - 2)) {
							color = Green;
						}

						++m_FPSAvg %= ((int)FPSAvgs.size());

						float Avg = 0.f;
						for (auto& f : FPSAvgs) {
							Avg += f;
						}
						Avg = Avg / ((float)FPSAvgs.size());

						Fonts->Get(FontPool::FontType::Nomal_Edge, y_UI(18)).DrawString(-1, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
																			  y_UI((1920 - 8)), y_UI(8), White, Black, "%5.2f FPS", Avg);
					}
#ifdef DEBUG
					DebugParts->DebugWindow(y_UI(1920 - 350), y_UI(150));
#endif // DEBUG
				}
			);
		}

		//�f�o�b�O
#ifdef DEBUG
		DebugParts->SetEndPoint();
#endif // DEBUG
		DrawParts->Screen_Flip();//��ʂ̔��f
		return SelEnd;
	}
	void SceneControl::NextScene(void) noexcept {
		auto* Pad = PadControl::Instance();
		GetNowScene()->Dispose();							//���
		if (this->m_NowScenesPtr != GetNowScene()->Get_Next()) {
			GetNowScene()->Dispose_Load();
		}
		this->m_NowScenesPtr = GetNowScene()->Get_Next();		//�J��
		Pad->Dispose();
		GetNowScene()->Load();
	}
};
