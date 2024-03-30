#include "DXLib_ref_101_sequence.hpp"

namespace DXLib_ref {
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	//�J�n
	void SceneControl::StartScene(void) noexcept {
		SetUseMaskScreenFlag(FALSE);//���ꕔ��ʂŃG�t�F�N�g���o�Ȃ��Ȃ邽�ߓ����
		//�J�����̏����ݒ�
		auto* OptionParts = OPTION::Instance();
		auto* DrawParts = DXDraw::Instance();
		DrawParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->Get_useVR() ? 120 : OptionParts->Get_Fov()), 0.05f, 200.f);
		//�����Ɖe�̏�����
		VECTOR_ref DefaultVec = VECTOR_ref::vget(0.25f, -1.f, 0.25f);
		DrawParts->SetAmbientLight(DefaultVec, GetColorF(1.f, 1.f, 1.f, 0.0f));
		DrawParts->SetShadow(DefaultVec, VECTOR_ref::vget(-100.f, -10.f, -100.f), VECTOR_ref::vget(100.f, 10.f, 100.f), 0);
		DrawParts->SetShadow(DefaultVec, VECTOR_ref::vget(-100.f, -10.f, -100.f), VECTOR_ref::vget(100.f, 10.f, 100.f), 1);
		DrawParts->SetShadow(DefaultVec, VECTOR_ref::vget(-100.f, -10.f, -100.f), VECTOR_ref::vget(100.f, 10.f, 100.f), 2);
		DrawParts->SetIsUpdateShadow(0, true);
		DrawParts->SetIsUpdateShadow(1, true);
		this->m_ScenesPtr->Set();
		DrawParts->SetIsUpdateShadow(2, true);
		DrawParts->Update_Shadow([&]() {this->m_ScenesPtr->ShadowDraw_Far(); }, VECTOR_ref::zero(), 2);		//���e���Z�b�g
		DrawParts->SetIsUpdateShadow(2, false);
		PadControl::Instance()->SetGuideUpdate();
		//FPS�\��
		for (auto& f : FPSAvgs) {
			f = 60.f;
		}
		m_FPSAvg = 0;
	}
	//
	bool SceneControl::Execute(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		PadControl::Instance()->Execute();
		auto SelEnd = !this->m_ScenesPtr->Update();		//�X�V
		Set3DSoundListenerPosAndFrontPosAndUpVec(DrawParts->SetMainCamera().GetCamPos().get(), DrawParts->SetMainCamera().GetCamVec().get(), DrawParts->SetMainCamera().GetCamUp().get());		//���ʒu�w��
		OptionWindowClass::Instance()->Execute();
		DrawParts->Execute();
		return SelEnd;
	}
	//�`��
	void SceneControl::Draw(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		EffectResource::Instance()->Calc(DrawParts->IsPause());//�G�t�F�N�V�A�̃A�v�f��60FPS�����ɕύX
		//�e���Z�b�g
		DrawParts->Update_Shadow([&] { this->m_ScenesPtr->ShadowDraw(); }, DrawParts->SetMainCamera().GetCamPos(), 0);
		DrawParts->Update_Shadow([&] { this->m_ScenesPtr->ShadowDraw_NearFar(); }, DrawParts->SetMainCamera().GetCamPos(), 1);
		DrawParts->Update_Shadow([&] {this->m_ScenesPtr->ShadowDraw_Far(); }, DrawParts->SetMainCamera().GetCamPos(), 2);
		//��ʂɔ��f
		DrawParts->Draw(
			[&](const Camera3DInfo& cams) {
				auto* PostPassParts = PostPassEffect::Instance();
				PostPassParts->Draw(
					[&]() { this->m_ScenesPtr->BG_Draw(); },
					[&]() {
						this->m_ScenesPtr->MainDraw();
						//PostPassParts->DrawByDepth([&] { this->m_ScenesPtr->MainDrawbyDepth(); });//�����`�悷��K�v�̂�����̂��ł�����C�����ėL����
					},
					[&]() {
						this->m_ScenesPtr->MainDrawFront();
						//PostPassParts->DrawByDepth([&] { this->m_ScenesPtr->MainDrawbyDepth(); });//�����`�悷��K�v�̂�����̂��ł�����C�����ėL����
					},
						cams);					//�`��
			},
			[&]() { this->m_ScenesPtr->DrawUI_Base(); },
				[&]() { this->m_ScenesPtr->DrawUI_In(); });
		//�f�o�b�O
		{
			FPSAvgs.at(m_FPSAvg) = GetFPS();
			++m_FPSAvg %= ((int)FPSAvgs.size());

			float Avg = 0.f;
			for (auto& f : FPSAvgs) {
				Avg += f;
			}
			Avg = Avg / ((float)FPSAvgs.size());

			auto* Fonts = FontPool::Instance();
			Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, y_r(1920 - 8), y_r(8), GetColor(255, 255, 255), GetColor(0, 0, 0), "%5.2f FPS", Avg);
		}
	}
	//
	void SceneControl::NextScene(void) noexcept {
		this->m_ScenesPtr->Dispose();							//���
		this->m_ScenesPtr = this->m_ScenesPtr->Get_Next();		//�J��
		PadControl::Instance()->Dispose();
	}
	//
};
