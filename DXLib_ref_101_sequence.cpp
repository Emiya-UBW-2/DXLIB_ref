#include "DXLib_ref_101_sequence.hpp"

namespace DXLib_ref {
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	void TEMPSCENE::Load(void) noexcept {
		if (!m_IsLoading) {
			m_IsLoading = true;
			Load_Sub();
		}
	}
	void TEMPSCENE::Set(void) noexcept {
		m_IsFirstLoop = true;
		Set_Sub();
	}
	bool TEMPSCENE::Update() noexcept {
		auto ans = Update_Sub();
		m_IsFirstLoop = false;
		return ans;
	}
	void TEMPSCENE::Draw(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		//�e���Z�b�g
		DrawParts->Update_Shadow([&] { ShadowDraw_Far_Sub(); }, DrawParts->SetMainCamera().GetCamPos(), 2);
		DrawParts->Update_Shadow([&] { ShadowDraw_NearFar_Sub(); }, DrawParts->SetMainCamera().GetCamPos(), 1);
		DrawParts->Update_Shadow([&] { ShadowDraw_Sub(); }, DrawParts->SetMainCamera().GetCamPos(), 0);
		DrawParts->Update_NearShadow([&] {MainDraw_Sub(); });
		//��ʂɔ��f
		DrawParts->Draw(
			[&]() { BG_Draw_Sub(); },
			[&]() { MainDraw_Sub(); },
			[&]() { MainDrawFront_Sub(); },
			[&]() { DrawUI_Base_Sub(); },
			[&]() { DrawUI_In_Sub(); }
		);
	}
	void TEMPSCENE::Dispose(void) noexcept { Dispose_Sub(); }
	void TEMPSCENE::Dispose_Load(void) noexcept {
		if (m_IsLoading) {
			m_IsLoading = false;
			Dispose_Load_Sub();
		}
	}

	//�J�n
	void SceneControl::StartScene(void) noexcept {
		SetUseMaskScreenFlag(FALSE);//���ꕔ��ʂŃG�t�F�N�g���o�Ȃ��Ȃ邽�ߓ����
		//�J�����̏����ݒ�
		auto* OptionParts = OPTION::Instance();
		auto* DrawParts = DXDraw::Instance();
		DrawParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamBoolean(EnumSaveParam::usevr) ? 120 : OptionParts->GetParamInt(EnumSaveParam::SSAO)), 0.05f, 200.f);
		//�����Ɖe�̏�����
		Vector3DX DefaultVec = Vector3DX::vget(0.25f, -1.f, 0.25f);
		DrawParts->SetAmbientLight(DefaultVec, GetColorF(1.f, 1.f, 1.f, 0.0f));
		DrawParts->SetShadow(DefaultVec, Vector3DX::vget(-100.f, -10.f, -100.f), Vector3DX::vget(100.f, 10.f, 100.f), 0);
		DrawParts->SetShadow(DefaultVec, Vector3DX::vget(-100.f, -10.f, -100.f), Vector3DX::vget(100.f, 10.f, 100.f), 1);
		DrawParts->SetShadow(DefaultVec, Vector3DX::vget(-100.f, -10.f, -100.f), Vector3DX::vget(100.f, 10.f, 100.f), 2);
		DrawParts->SetIsUpdateShadow(0, true);
		DrawParts->SetIsUpdateShadow(1, true);
		this->m_ScenesPtr->Set();
		DrawParts->SetIsUpdateShadow(2, true);
		DrawParts->Update_Shadow([&]() {this->m_ScenesPtr->ShadowDraw_Far(); }, Vector3DX::zero(), 2);		//���e���Z�b�g
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
		PadControl::Instance()->Execute();
		auto SelEnd = !this->m_ScenesPtr->Update();		//�X�V
		OptionWindowClass::Instance()->Execute();
		DXDraw::Instance()->Execute();
		SideLog::Instance()->Update();
		return SelEnd;
	}
	//�`��
	void SceneControl::Draw(void) noexcept {
		this->m_ScenesPtr->Draw();
		//�f�o�b�O
		{
			FPSAvgs.at(m_FPSAvg) = DXDraw::Instance()->GetFps();
			++m_FPSAvg %= ((int)FPSAvgs.size());

			float Avg = 0.f;
			for (auto& f : FPSAvgs) {
				Avg += f;
			}
			Avg = Avg / ((float)FPSAvgs.size());

			auto* Fonts = FontPool::Instance();
			Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, y_r(1920 - 8), y_r(8), White, Black, "%5.2f FPS", Avg);
		}
		//
		SideLog::Instance()->Draw();
	}
	//
	void SceneControl::NextScene(void) noexcept {
		this->m_ScenesPtr->Dispose();							//���
		this->m_ScenesPtr = this->m_ScenesPtr->Get_Next();		//�J��
		PadControl::Instance()->Dispose();
	}
	//
};
