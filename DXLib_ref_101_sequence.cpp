#include "DXLib_ref_101_sequence.hpp"

namespace DXLib_ref {
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	TEMPSCENE::TEMPSCENE(void) noexcept {}
	TEMPSCENE::~TEMPSCENE(void) noexcept {}

	void TEMPSCENE::Set(void) noexcept {
		m_IsFirstLoop = true;
		Set_Sub();
		//�J�����̏����ݒ�
		auto* OptionParts = OPTION::Instance();
		auto* DrawParts = DXDraw::Instance();
		DrawParts->SetMainCamera().SetCamInfo(
			deg2rad(OptionParts->Get_useVR() ? 120 : OptionParts->Get_Fov()),
			0.05f,
			200.f
		);
	}
	bool TEMPSCENE::Update() noexcept {
		auto* Pad = PadControl::Instance();
		Pad->Execute();
		//�����Ɖe�̏�����
		auto* DrawParts = DXDraw::Instance();
		DrawParts->SetAmbientLight(GetLightVec(), GetLightColorF());
		DrawParts->SetShadowDir(GetShadowVec(0), 0);
		DrawParts->SetShadowDir(GetShadowVec(1), 1);
		DrawParts->SetShadowDir(GetShadowVec(2), 2);
		auto ans = Update_Sub();
		m_IsFirstLoop = false;
		return ans;
	}
	void TEMPSCENE::Dispose(void) noexcept {
		Dispose_Sub();
	}

	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	SceneControl::SceneControl(const std::shared_ptr<TEMPSCENE>& ptr) noexcept {
		auto* DrawParts = DXDraw::Instance();
		this->m_ScreenVertex.SetScreenVertex(DrawParts->m_DispXSize, DrawParts->m_DispYSize);								// ���_�f�[�^�̏���
		this->m_Shader2D[0].Init("shader/VS_lens.vso", "shader/PS_lens.pso");																//�����Y
		this->m_Shader2D[1].Init("shader/DepthVS.vso", "shader/DepthPS.pso");						//�����Y
		this->m_ScenesPtr = ptr;
	}
	SceneControl::~SceneControl(void) noexcept {
		this->m_ScenesPtr->Dispose();
	}
	//�J�n
	void SceneControl::StartScene(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		auto* Pad = PadControl::Instance();
		SetUseMaskScreenFlag(FALSE);//���ꕔ��ʂŃG�t�F�N�g���o�Ȃ��Ȃ邽�ߓ����
		this->m_ScenesPtr->Set();
		//���e���Z�b�g
		DrawParts->Update_Shadow(
			[&]() {this->m_ScenesPtr->ShadowDraw_Far(); },
			this->m_ScenesPtr->GetFarShadowMax(), this->m_ScenesPtr->GetFarShadowMin(),
			2);
		this->m_SelEnd = false;
		Pad->SetGuideUpdate();
	}
	//
	bool SceneControl::Execute(void) noexcept {
#ifdef DEBUG
		//auto* DebugParts = DebugClass::Instance();
#endif // DEBUG
		auto* DrawParts = DXDraw::Instance();
		//
		SetisUpdateFarShadow(this->m_ScenesPtr->GetisUpdateFarShadow());
		//�X�V
		auto SelEnd = !this->m_ScenesPtr->Update();
		m_SelEnd = this->m_ScenesPtr->GetisEnd();
		//���ʒu�w��
		Set3DSoundListenerPosAndFrontPosAndUpVec(DrawParts->SetMainCamera().GetCamPos().get(), DrawParts->SetMainCamera().GetCamVec().get(), DrawParts->SetMainCamera().GetCamUp().get());
		//
		DrawParts->Execute();
		OptionWindowClass::Instance()->Execute();
		//
		return SelEnd || m_SelEnd;
	}
	//�`��
	void SceneControl::Draw(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		auto* PostPassParts = PostPassEffect::Instance();
		EffectResource::Instance()->Calc(DrawParts->IsPause());//�G�t�F�N�V�A�̃A�v�f��60FPS�����ɕύX
		if (!this->m_ScenesPtr->GetIsUpdateDraw()) { return; }
		//�e���Z�b�g
		DrawParts->Update_Shadow([&] { this->m_ScenesPtr->ShadowDraw(); },
			DrawParts->SetMainCamera().GetCamPos() + this->m_ScenesPtr->GetNearShadowMax(),
			DrawParts->SetMainCamera().GetCamPos() + this->m_ScenesPtr->GetNearShadowMin(),
			0);
		DrawParts->Update_Shadow([&] { this->m_ScenesPtr->ShadowDraw_NearFar(); },
			DrawParts->SetMainCamera().GetCamPos() + this->m_ScenesPtr->GetMiddleShadowMax(),
			DrawParts->SetMainCamera().GetCamPos() + this->m_ScenesPtr->GetMiddleShadowMin(),
			1);
		if (m_isUpdateFarShadow) {
			DrawParts->Update_Shadow([&]() {this->m_ScenesPtr->ShadowDraw_Far(); },
				DrawParts->SetMainCamera().GetCamPos() + this->m_ScenesPtr->GetFarShadowMax(),
				DrawParts->SetMainCamera().GetCamPos() + this->m_ScenesPtr->GetFarShadowMin(),
				2);
		}
		//��ʂɔ��f
		DrawParts->Draw(
			[&](const Camera3DInfo& cams) {
				PostPassParts->Draw(
					[&]() { this->m_ScenesPtr->BG_Draw(); },
					[&]() {
						this->m_ScenesPtr->MainDraw();
						//�����`�悷��K�v�̂�����̂��ł�����C�����ėL����
						//PostPassParts->DrawByDepth([&] { this->m_ScenesPtr->MainDrawbyDepth(); });
					}, cams);					//�`��
				//����������ʂɑ΂��Č㏈����2D�V�F�[�_�[�𔽉f
				if (this->m_ScenesPtr->is_lens()) {
					//�����Y
					PostPassParts->Plus_Draw([&]() {
						this->m_Shader2D[0].SetPixelDispSize(DrawParts->m_DispXSize, DrawParts->m_DispYSize);
						this->m_Shader2D[0].SetPixelParam(3, this->m_ScenesPtr->xp_lens(), this->m_ScenesPtr->yp_lens(), this->m_ScenesPtr->size_lens(), this->m_ScenesPtr->zoom_lens());
						SetUseTextureToShader(0, PostPassParts->Get_MAIN_Screen().get());	//�g�p����e�N�X�`�����Z�b�g
						this->m_Shader2D[0].Draw(this->m_ScreenVertex);
						SetUseTextureToShader(0, -1);
					});
				}
				if (this->m_ScenesPtr->is_Blackout()) {
					//�u���b�N�A�E�g
					PostPassParts->Plus_Draw([&]() {
						this->m_Shader2D[1].SetPixelDispSize(DrawParts->m_DispXSize, DrawParts->m_DispYSize);
						this->m_Shader2D[1].SetPixelParam(3, this->m_ScenesPtr->Per_Blackout(), 0, 0, 0);
						SetUseTextureToShader(0, PostPassParts->Get_MAIN_Screen().get());	//�g�p����e�N�X�`�����Z�b�g
						this->m_Shader2D[1].Draw(this->m_ScreenVertex);
						SetUseTextureToShader(0, -1);
					});
				}
			},
			[&]() { this->m_ScenesPtr->DrawUI_Base(); },
			[&]() {
				if (DXDraw::Instance()->IsPause()) {
					//
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*0.5f), 0, 255));
					DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
					//
					if (m_PauseFlashCount > 0.5f) {
						auto* Fonts = FontPool::Instance();
						Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_r(36), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, y_r(16), y_r(16), GetColor(0, 255, 0), GetColor(0, 0, 0), "Pause");
					}
					m_PauseFlashCount += 1.f / GetFPS();
					if (m_PauseFlashCount > 1.f) { m_PauseFlashCount = 0.f; }
				}
				this->m_ScenesPtr->DrawUI_In();
			}
		);
		OptionWindowClass::Instance()->Draw();
		KeyGuideClass::Instance()->Draw();
	}
	//
	void SceneControl::NextScene(void) noexcept {
		this->m_ScenesPtr->Dispose();							//���
		this->m_ScenesPtr = this->m_ScenesPtr->Get_Next();		//�J��
		KeyGuideClass::Instance()->Reset();
	}
	//
};
