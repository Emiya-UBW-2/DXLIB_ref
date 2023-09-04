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
		m_MainCamera.SetCamInfo(deg2rad(OptionParts->Get_useVR() ? 120 : OptionParts->Get_Fov()), 0.05f, 200.f);//1P
		//�����Ɖe�̏�����
		auto* DrawParts = DXDraw::Instance();
		DrawParts->SetAmbientLight(GetLightVec(), GetLightColorF());
		DXDraw::Instance()->m_PauseActive.Set(true);
	}
	bool TEMPSCENE::Update() noexcept {
		auto* Pad = PadControl::Instance();
		Pad->Execute();
		DXDraw::Instance()->m_PauseActive.Execute(Pad->GetOptionKey().press());		//�|�[�Y
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
			(this->m_ScenesPtr->GetFarShadowMax() + this->m_ScenesPtr->GetFarShadowMin()) / 2,
			(this->m_ScenesPtr->GetFarShadowMax() - this->m_ScenesPtr->GetFarShadowMin()) / 2,
			2);
		this->m_SelEnd = false;
		Pad->SetGuideUpdate();
	}
	//
	bool SceneControl::Execute(void) noexcept {
#ifdef DEBUG
		//auto* DebugParts = DebugClass::Instance();
#endif // DEBUG
		//�X�V
		auto SelEnd = !this->m_ScenesPtr->Update();
		m_SelEnd = this->m_ScenesPtr->GetisEnd();
		//���ʒu�w��
		Set3DSoundListenerPosAndFrontPosAndUpVec(this->m_ScenesPtr->GetMainCamera().GetCamPos().get(), this->m_ScenesPtr->GetMainCamera().GetCamVec().get(), this->m_ScenesPtr->GetMainCamera().GetCamUp().get());
		//
		DXDraw::Instance()->Execute();
		OptionWindowClass::Instance()->Execute();
		//
		return SelEnd || m_SelEnd;
	}
	//�`��
	void SceneControl::Draw(void) noexcept {
		if (!this->m_ScenesPtr->GetIsUpdateDraw()) { return; }
		auto* DrawParts = DXDraw::Instance();
		auto* PostPassParts = PostPassEffect::Instance();
		//�e���Z�b�g
		DrawParts->Update_Shadow([&] { this->m_ScenesPtr->ShadowDraw(); },
			this->m_ScenesPtr->GetMainCamera().GetCamPos() + (this->m_ScenesPtr->GetNearShadowMax() + this->m_ScenesPtr->GetNearShadowMin()) / 2,
			(this->m_ScenesPtr->GetNearShadowMax() - this->m_ScenesPtr->GetNearShadowMin()) / 2,
			0);
		DrawParts->Update_Shadow([&] { this->m_ScenesPtr->ShadowDraw_NearFar(); },
			this->m_ScenesPtr->GetMainCamera().GetCamPos() + (this->m_ScenesPtr->GetMiddleShadowMax() + this->m_ScenesPtr->GetMiddleShadowMin()) / 2,
			(this->m_ScenesPtr->GetMiddleShadowMax() - this->m_ScenesPtr->GetMiddleShadowMin()) / 2,
			1);
		//��ʂɔ��f
		DrawParts->Draw(
			this->m_ScenesPtr->GetMainCamera(),
			[&]() { this->m_ScenesPtr->BG_Draw(); },
			[&]() {
			this->m_ScenesPtr->MainDraw();
			//�����`�悷��K�v�̂�����̂��ł�����C�����ėL����
			//PostPassParts->DrawByDepth([&] { this->m_ScenesPtr->MainDrawbyDepth(); });
		},
			[&]() { this->m_ScenesPtr->DrawUI_Base(); },
			[&]() { this->m_ScenesPtr->DrawUI_In(); },
			[&]() {
			//2D�V�F�[�_�[�𔽉f����i�K
				{
					SetUseTextureToShader(0, PostPassParts->Get_MAIN_Screen().get());	//�g�p����e�N�X�`�����Z�b�g
					//�����Y�`��
					if (this->m_ScenesPtr->is_lens()) {
						this->m_Shader2D[0].SetPixelDispSize(DrawParts->m_DispXSize, DrawParts->m_DispYSize);
						this->m_Shader2D[0].SetPixelParam(3, this->m_ScenesPtr->xp_lens(), this->m_ScenesPtr->yp_lens(), this->m_ScenesPtr->size_lens(), this->m_ScenesPtr->zoom_lens());
						PostPassParts->Get_BUF_Screen().SetDraw_Screen(false);
						{
							this->m_Shader2D[0].Draw(this->m_ScreenVertex);
						}
						PostPassParts->FlipBuftoMain();
					}
					//�`��
					if (this->m_ScenesPtr->is_Blackout()) {
						this->m_Shader2D[1].SetPixelDispSize(DrawParts->m_DispXSize, DrawParts->m_DispYSize);
						this->m_Shader2D[1].SetPixelParam(3, this->m_ScenesPtr->Per_Blackout(), 0, 0, 0);
						PostPassParts->Get_BUF_Screen().SetDraw_Screen(false);
						{
							this->m_Shader2D[1].Draw(this->m_ScreenVertex);
						}
						PostPassParts->FlipBuftoMain();
					}
					SetUseTextureToShader(0, -1);
				}
		});
		OptionWindowClass::Instance()->Draw();
		KeyGuideClass::Instance()->Draw();
		if (DXDraw::Instance()->IsPause()) {
			if (m_PauseFlashCount > 0.25f) {
				auto* Fonts = FontPool::Instance();
				Fonts->Get(FontPool::FontType::HUD_EdgeL).DrawString(y_r(36), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, y_r(16), y_r(16), GetColor(255, 255, 255), GetColor(0, 0, 0), "Pause");
			}
			m_PauseFlashCount += 1.f / GetFPS();
			if (m_PauseFlashCount > 0.5f) { m_PauseFlashCount = 0.f; }
		}
	}
	//
	void SceneControl::NextScene(void) noexcept {
		this->m_ScenesPtr->Dispose();							//���
		this->m_ScenesPtr = this->m_ScenesPtr->Get_Next();		//�J��
		KeyGuideClass::Instance()->Reset();
	}
	//
};
