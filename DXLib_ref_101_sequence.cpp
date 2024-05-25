#include "DXLib_ref_101_sequence.hpp"

namespace DXLib_ref {
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
		this->m_ScenesPtr->Set();
		Pad->SetGuideUpdate();
		//FPS�\��
		for (auto& f : FPSAvgs) {
			f = 60.f;
		}
		m_FPSAvg = 0;
	}
	bool SceneControl::FirstExecute(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		return DrawParts->FirstExecute();
	}

	bool SceneControl::Execute(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		auto* ItemLogParts = SideLog::Instance();
		auto* PopUpParts = PopUp::Instance();
		auto* Pad = PadControl::Instance();
		auto* OptionParts = OPTION::Instance();
#ifdef DEBUG
		auto* DebugParts = DebugClass::Instance();		//�f�o�b�O
#endif // DEBUG

#ifdef DEBUG
		clsDx();
		DebugParts->SetStartPoint();
#endif // DEBUG
		if (DrawParts->UpdateShadowActive() || this->m_ScenesPtr->GetIsFirstLoop()) {
			DrawParts->Update_Shadow([&]() { this->m_ScenesPtr->ShadowDraw_Far(); }, Vector3DX::zero(), true);
		}
		if (OptionParts->GetParamInt(EnumSaveParam::Reflection) > 0) {
			DrawParts->Update_CubeMap([&]() { this->m_ScenesPtr->CubeMapDraw(); }, DrawParts->SetMainCamera().GetCamPos());
		}
		Pad->Execute();
		auto SelEnd = !this->m_ScenesPtr->Update();		//�X�V
		OptionWindowClass::Instance()->Execute();
		DrawParts->Execute();
		ItemLogParts->Update();
		PopUpParts->Update();
		//�`��

#ifdef DEBUG
		DebugParts->SetPoint("-----DrawStart-----");
#endif // DEBUG
		//�e���Z�b�g
		DrawParts->Update_Shadow([&] { this->m_ScenesPtr->ShadowDraw(); }, DrawParts->SetMainCamera().GetCamPos(), false);
		//��ʂɔ��f
		DrawParts->Draw(
			[&]() { this->m_ScenesPtr->BG_Draw(); },
			[&]() { this->m_ScenesPtr->MainDraw(); },
			[&]() { this->m_ScenesPtr->MainDrawFront(); },
			[&]() { this->m_ScenesPtr->DrawUI_Base(); },
			[&]() {
				this->m_ScenesPtr->DrawUI_In();
				//�ǉ��̕`�敨
				auto* ItemLogParts = SideLog::Instance();
				auto* PopUpParts = PopUp::Instance();
				auto* Fonts = FontPool::Instance();
				auto* OptionParts = OPTION::Instance();
				ItemLogParts->Draw();
				PopUpParts->Draw(y_r(960), y_r(540));
				{
					FPSAvgs.at(m_FPSAvg) = DXDraw::Instance()->GetFps();
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

					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
						y_r(1920 - 8), y_r(8), White, Black, "%5.2f FPS", Avg);
				}
			}
		);

		//�f�o�b�O
#ifdef DEBUG
		DebugParts->DebugWindow(DXDraw::Instance()->GetDispXSize() - y_r(250), y_r(150));
		//TestDrawShadowMap(DrawParts->m_Shadow[0].GetHandle(), 0, 0, 960, 540);
#endif // DEBUG
		DrawParts->Screen_Flip();//��ʂ̔��f
		return SelEnd;
	}
	void SceneControl::NextScene(void) noexcept {
		auto* Pad = PadControl::Instance();
		this->m_ScenesPtr->Dispose();							//���
		this->m_ScenesPtr = this->m_ScenesPtr->Get_Next();		//�J��
		Pad->Dispose();
	}
};
