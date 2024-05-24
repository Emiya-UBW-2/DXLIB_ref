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
		auto* OptionParts = OPTION::Instance();
		auto* DrawParts = DXDraw::Instance();
		//�J�����̏����ݒ�
		DrawParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamBoolean(EnumSaveParam::usevr) ? 120 : OptionParts->GetParamInt(EnumSaveParam::fov)), 0.05f, 200.f);
		//�����Ɖe�̏�����
		DrawParts->SetAmbientLight(Vector3DX::vget(0.25f, -1.f, 0.25f), GetColorF(1.f, 1.f, 1.f, 0.0f));
		//
		Set_Sub();
		m_IsFirstLoop = true;
	}
	bool TEMPSCENE::Update() noexcept {
		auto* DrawParts = DXDraw::Instance();
		if (DrawParts->UpdateShadowActive() || m_IsFirstLoop) {
			DrawParts->Update_Shadow([&]() { ShadowDraw_Far_Sub(); }, Vector3DX::zero(), true);		//���e���Z�b�g
			DrawParts->Update_CubeMap([&]() { CubeMap_Sub(); }, Vector3DX::vget(0.f, 1.f*12.5f, 0.f));		//���e���Z�b�g
		}
		auto ans = Update_Sub();
		m_IsFirstLoop = false;
		return ans;
	}
	void TEMPSCENE::Draw(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		//�e���Z�b�g
		DrawParts->Update_Shadow([&] { ShadowDraw_Sub(); }, DrawParts->SetMainCamera().GetCamPos(), false);
		//��ʂɔ��f
		DrawParts->Draw(
			[&]() { BG_Draw_Sub(); },
			[&]() { MainDraw_Sub(); },
			[&]() { MainDrawFront_Sub(); },
			[&]() { DrawUI_Base_Sub(); },
			[&]() {
				auto* ItemLogParts = SideLog::Instance();
				auto* PopUpParts = PopUp::Instance();
				DrawUI_In_Sub();
				ItemLogParts->Draw();
				PopUpParts->Draw(y_r(960), y_r(540));
			}
		);
	}
	void TEMPSCENE::Dispose(void) noexcept { Dispose_Sub(); }
	void TEMPSCENE::Dispose_Load(void) noexcept {
		if (m_IsLoading) {
			m_IsLoading = false;
			Dispose_Load_Sub();
		}
	}
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	void SceneControl::StartScene(void) noexcept {
		SetUseMaskScreenFlag(FALSE);//���ꕔ��ʂŃG�t�F�N�g���o�Ȃ��Ȃ邽�ߓ����
		this->m_ScenesPtr->Set();
		PadControl::Instance()->SetGuideUpdate();
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
		auto* OptionParts = OPTION::Instance();
		auto* ItemLogParts = SideLog::Instance();
		auto* PopUpParts = PopUp::Instance();
		PadControl::Instance()->Execute();
		auto SelEnd = !this->m_ScenesPtr->Update();		//�X�V
		OptionWindowClass::Instance()->Execute();
		DrawParts->Execute();
		ItemLogParts->Update();
		PopUpParts->Update();
		//�`��
		this->m_ScenesPtr->Draw();

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

			auto* Fonts = FontPool::Instance();
			Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(18, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
																  DrawParts->GetDispXSize() - 8, 8, White, Black, "%5.2f FPS", Avg);
		}
		return SelEnd;
	}
	void SceneControl::NextScene(void) noexcept {
		this->m_ScenesPtr->Dispose();							//���
		this->m_ScenesPtr = this->m_ScenesPtr->Get_Next();		//�J��
		PadControl::Instance()->Dispose();
	}
};
