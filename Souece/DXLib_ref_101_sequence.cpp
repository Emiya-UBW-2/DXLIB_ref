#include "DXLib_ref_101_sequence.hpp"

namespace DXLibRef {
	const SceneControl* SingletonBase<SceneControl>::m_Singleton = nullptr;
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	// FPS�\��
	void SceneControl::FPSDrawer::Initialize(void) noexcept {
		// �e�X�̐��l��������
		for (auto& f : m_FPSAvgs) {
			f = FrameRate;
		}
		m_FPSAvgCount = 0;
	}
	void SceneControl::FPSDrawer::Update(void) noexcept {
		auto* DXLib_refParts = DXLib_ref::Instance();
		// m_FPSAvgCount�̔ԍ��ɑ΂��č��̃t���[�����[�g��ۑ�
		m_FPSAvgs.at(m_FPSAvgCount) = DXLib_refParts->GetFps();
		// �ۑ�����ꏊ�����炷
		++m_FPSAvgCount %= m_FPSAvgs.size();
		// �ۑ����Ă���ߋ���FPS�l�̕��ς��Ƃ�
		m_FPSAvg = 0.f;
		for (auto& f : m_FPSAvgs) {
			m_FPSAvg += f;
		}
		m_FPSAvg = m_FPSAvg / static_cast<float>(m_FPSAvgs.size());
	}
	void SceneControl::FPSDrawer::DrawFPSCounter(void) const noexcept {
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		auto* OptionParts = OptionManager::Instance();
		// FPS�̕��ϒl���ݒ肵�Ă�������l�ɑ΂��č����Ȃ�΁A�Ⴂ�Ȃ物�F�ԐF�ƕω�������
		auto color = White;
		if (m_FPSAvg > static_cast<float>(OptionParts->GetParamInt(EnumSaveParam::FpsLimit) - 2)) {
			color = Green;// �\����FPS���o�Ă���
		}
		else if (m_FPSAvg > static_cast<float>(OptionParts->GetParamInt(EnumSaveParam::FpsLimit) - 10)) {
			color = Yellow;// �\����FPS���o�Ă��Ȃ�
		}
		else {
			color = Red;// �܂�����FPS���o�Ă��Ȃ�
		}
		// FPS�l�̕\��
		DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic,
			LineHeight, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP,
			BaseScreenWidth - 8, 8, color, Black, "%5.2f FPS", m_FPSAvg);
		// �h���[�R�[��(DirectX�ɉ���`��w���𑗂�����)�̕\��
		DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic,
			LineHeight, FontSystem::FontXCenter::RIGHT, FontSystem::FontYCenter::TOP,
			BaseScreenWidth - 8, 8 + 20, White, Black, "%d Drawcall", GetDrawCallCount());
	}
	// �|�[�Y���
	void SceneControl::PauseDrawer::Update(void) noexcept {
		auto* DXLib_refParts = DXLib_ref::Instance();
		// �|�[�Y��ʂł͓_�ł̉��Z���s��
		m_PauseFlashCount += DXLib_refParts->GetDeltaTime();
		// 1�b�o������0�b�Ƀ��Z�b�g����
		if (m_PauseFlashCount > 1.f) {
			m_PauseFlashCount -= 1.f;
		}
	}
	void SceneControl::PauseDrawer::DrawPause(void) const noexcept {
		// �|�[�Y��ʂɓ����Ă��Ȃ��ꍇ�̓X���[����
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		// �������̈Ö�
		DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 128);
		DrawCtrls->SetDrawBox(WindowSystem::DrawLayer::Normal, 0, 0, BaseScreenWidth, BaseScreenHeight, Black, TRUE);
		DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
		// �J�E���g��0.5�b�ȏ�ł���� Pause �̕�����\��
		if (m_PauseFlashCount > 0.5f) {
			DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic,
				36, FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP,
				16, 16, Green, Black, "Pause");
		}
	}
	//
	void SceneControl::DrawMain(const Camera3DInfo& camInfo, bool Is3D) const noexcept {
		auto* PostPassParts = PostPassEffect::Instance();
		// �S�Ẳ�ʂ�������
		PostPassParts->ResetBuffer();
		if (Is3D) {
			// �J����
			PostPassParts->SetCamMat(camInfo);
			// ��
			PostPassParts->DrawGBuffer(1000.0f, 50000.0f, [this]() { this->m_NowScenesPtr->BG_Draw(); });
			// ������
			PostPassParts->DrawGBuffer(camInfo.GetCamFar() - 10.f, 1000000.f, [this]() {
				auto* PostPassParts = PostPassEffect::Instance();
				PostPassParts->DrawByPBR([this]() {
					this->m_NowScenesPtr->CalcOnDraw();
					this->m_NowScenesPtr->MainDraw();
					});
				this->m_NowScenesPtr->MainDrawFront();
				});
			// ����
			PostPassParts->DrawGBuffer(camInfo.GetCamNear(), camInfo.GetCamFar(), [this]() {
#if defined(_USE_EFFEKSEER_)
				Effekseer_Sync3DSetting();
#endif
				auto* PostPassParts = PostPassEffect::Instance();
				PostPassParts->DrawByPBR([this]() {
					this->m_NowScenesPtr->CalcOnDraw();
					this->m_NowScenesPtr->MainDraw();
					});
#if defined(_USE_EFFEKSEER_)
				DrawEffekseer3D();
#endif
				this->m_NowScenesPtr->MainDrawFront();
				});
			// ����
			PostPassParts->DrawGBuffer(0.1f, 0.1f + camInfo.GetCamNear(), [this]() {
#if defined(_USE_EFFEKSEER_)
				Effekseer_Sync3DSetting();
#endif
				auto* PostPassParts = PostPassEffect::Instance();
				PostPassParts->DrawByPBR([this]() {
					this->m_NowScenesPtr->CalcOnDraw();
					this->m_NowScenesPtr->MainDraw();
					});
#if defined(_USE_EFFEKSEER_)
				DrawEffekseer3D();
#endif
				this->m_NowScenesPtr->MainDrawFront();
				});
			// �e
			PostPassParts->SetDrawShadow(camInfo, [this]() { this->m_NowScenesPtr->SetShadowDraw_Rigid(); }, [this]() { this->m_NowScenesPtr->SetShadowDraw(); });
		}
		else {
			// 2D�`��
			PostPassParts->GetBufferScreen().SetDraw_Screen();
			{
				this->m_NowScenesPtr->MainDraw();
			}
			PostPassParts->Set_DoFNearFar(0.1f, 5.f, 0.05f, 6.f);	// Dof�𖳌���
		}
		// �|�X�g�v���Z�X
		PostPassParts->DrawPostProcess();
	}
	void SceneControl::DrawUICommon(void) const noexcept {
		auto* SideLogParts = SideLog::Instance();
		auto* KeyGuideParts = KeyGuide::Instance();
		//
		this->m_NowScenesPtr->DrawUI_Base();
		// �|�[�Y�`���ݒ�
		if (IsPause()) {
			m_PauseDrawer.DrawPause();
		}
		// FPS�\���`���ݒ�
		m_FPSDrawer.DrawFPSCounter();
		KeyGuideParts->Draw();
		SideLogParts->Draw();
		PopUp::Instance()->Draw(BaseScreenWidth / 2, BaseScreenHeight / 2);
#if defined(DEBUG)
		DebugClass::Instance()->DebugWindow(BaseScreenWidth - (350), (150));
#endif // DEBUG
	}
	void SceneControl::DrawUIFront(void) const noexcept {
		this->m_NowScenesPtr->DrawUI_In();
		WindowSystem::DrawControl::Instance()->Draw();
	}
	//
	void SceneControl::ChangePause(bool value) noexcept {
		auto* PopUpParts = PopUp::Instance();
		if (m_IsPauseActive != value) {
			m_IsPauseActive = value;
			auto* KeyGuideParts = KeyGuide::Instance();
			KeyGuideParts->SetGuideFlip();
		}
		//�|�b�v�A�b�v�����ׂč폜�Ƃ���
		PopUpParts->EndAll();
	}
	//
	void SceneControl::Initialize(void) noexcept {
		auto* OptionParts = OptionManager::Instance();
		auto* WindowSizeParts = WindowSizeControl::Instance();
		auto* KeyGuideParts = KeyGuide::Instance();
		auto* PostPassParts = PostPassEffect::Instance();
		auto* CameraParts = Camera3D::Instance();
		KeyGuideParts->Dispose();
		PostPassParts->ResetAllParams();
		PostPassParts->ResetAllBuffer();
		PostPassParts->UpdateActive();
		//
		this->m_NowScenesPtr->Load();
		//
		SetUseMaskScreenFlag(FALSE);// ���ꕔ��ʂŃG�t�F�N�g���o�Ȃ��Ȃ邽�ߓ����
		// �J�����̏����ݒ�
		CameraParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamBoolean(EnumSaveParam::usevr) ? 120 : OptionParts->GetParamInt(EnumSaveParam::fov)), 0.05f, 200.f);
		// �����Ɖe�̏�����
		WindowSizeParts->SetAmbientLight(Vector3DX::vget(0.25f, -1.f, 0.25f), GetColorF(1.f, 1.f, 1.f, 0.0f));
		this->m_NowScenesPtr->Set();
		KeyGuideParts->SetGuideFlip();
		// FPS�\���̏�����
		m_FPSDrawer.Initialize();
	}
	void SceneControl::Update(void) noexcept {
		auto* Pad = PadControl::Instance();
		auto* PopUpParts = PopUp::Instance();
		auto* OptionWindowParts = OptionPopup::Instance();
		auto* LocalizeParts = LocalizePool::Instance();
		auto* SideLogParts = SideLog::Instance();
		auto* CameraParts = Camera3D::Instance();
		WindowSystem::DrawControl::Instance()->ClearList();
		if (Pad->GetPadsInfo(Controls::PADS::Escape).GetKey().trigger() && !m_IsExitSelect) {
			m_IsExitSelect = true;
			PopUpParts->Add(LocalizeParts->Get(100), 480, 240,
				[this](int xmin, int ymin, int xmax, int ymax, bool) {
					auto* DrawCtrls = WindowSystem::DrawControl::Instance();
					auto* LocalizeParts = LocalizePool::Instance();
					int xp1, yp1;
					// �^�C�g��
					{
						xp1 = xmin + (24);
						yp1 = ymin + LineHeight;

						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
							FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp1, yp1, White, Black, LocalizeParts->Get(101));
					}
					// 
					{
						xp1 = (xmax + xmin) / 2 - (54);
						yp1 = ymax - LineHeight * 3;

						auto* Pad = PadControl::Instance();
						bool ret = WindowSystem::SetMsgClickBox(xp1, yp1, xp1 + (108), yp1 + LineHeight * 2, LineHeight, Gray15, false, true, LocalizeParts->Get(102));
						if (Pad->GetPadsInfo(Controls::PADS::INTERACT).GetKey().trigger() || ret) {
							// �I���t���O�𗧂Ă�
							SetEndGame();
						}
					}
				},
				[this]() { m_IsExitSelect = false; },
				[]() {},
				true
			);
		}
		if (OptionWindowParts->IsRestartSwitch() && !m_IsRestartSelect) {
			m_IsRestartSelect = true;
			PopUpParts->Add(LocalizeParts->Get(100), 480, 240,
				[this](int xmin, int ymin, int xmax, int ymax, bool) {
					auto* DrawCtrls = WindowSystem::DrawControl::Instance();
					auto* LocalizeParts = LocalizePool::Instance();
					int xp1, yp1;
					// �^�C�g��
					{
						xp1 = xmin + (24);
						yp1 = ymin + LineHeight;

						DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontSystem::FontType::MS_Gothic, LineHeight,
							FontSystem::FontXCenter::LEFT, FontSystem::FontYCenter::TOP, xp1, yp1, White, Black, LocalizeParts->Get(2101));
					}
					// 
					{
						xp1 = (xmax + xmin) / 2 - (54);
						yp1 = ymax - LineHeight * 3;

						auto* Pad = PadControl::Instance();
						bool ret = WindowSystem::SetMsgClickBox(xp1, yp1, xp1 + (108), yp1 + LineHeight * 2, LineHeight, Gray15, false, true, LocalizeParts->Get(2102));
						if (Pad->GetPadsInfo(Controls::PADS::INTERACT).GetKey().trigger() || ret) {
							SetEndGame();
							StartMe();
						}
					}
				},
				[this]() {
					m_IsRestartSelect = false;
				},
				[this]() {},
				true
			);
		}
		Pad->Update();
		m_IsEndScene = !this->m_NowScenesPtr->Update();		// �X�V
		OptionWindowParts->Update();
		Set3DSoundListenerPosAndFrontPosAndUpVec(CameraParts->SetMainCamera().GetCamPos().get(), CameraParts->SetMainCamera().GetCamVec().get(), CameraParts->SetMainCamera().GetCamUp().get());		// ���ʒu�w��
#if defined(_USE_OPENVR_)
		VRControl::Instance()->Execute();
#endif
		Camera3D::Instance()->Update();
		SideLogParts->Update();
		PopUpParts->Update();
		// �|�[�Y��ʂ̍X�V
		if (IsPause()) {
			m_PauseDrawer.Update();
		}
		// �|�[�Y���͂ɂ��I���I�t
		if (Pad->GetPadsInfo(Controls::PADS::INVENTORY).GetKey().trigger()) {
			ChangePause(!IsPause());
		}
		// FPS�\���@�\�̍X�V
		m_FPSDrawer.Update();
	}
	void SceneControl::DrawMainLoop(void) const noexcept {
		auto* OptionParts = OptionManager::Instance();
		auto* PostPassParts = PostPassEffect::Instance();
		auto* WindowSizeParts = WindowSizeControl::Instance();
		auto* CameraParts = Camera3D::Instance();
		if (this->m_NowScenesPtr->Get3DActive()) {
			// �L���[�u�}�b�v���Z�b�g
			Vector3DX Pos = CameraParts->GetMainCamera().GetCamPos(); Pos.y *= -1.f;
			PostPassParts->Update_CubeMap([this]() { this->m_NowScenesPtr->CubeMap(); }, Pos);
			// �e���Z�b�g
			if (PostPassParts->UpdateShadowActive() || this->m_NowScenesPtr->GetIsFirstLoop()) {
				PostPassParts->Update_Shadow(
					[this]() { this->m_NowScenesPtr->ShadowDraw_Far(); },
					Vector3DX::zero(),
					this->m_NowScenesPtr->GetShadowScale() * 4.f, true);
			}
			PostPassParts->Update_Shadow(
				[this] { this->m_NowScenesPtr->ShadowDraw(); },
				CameraParts->GetMainCamera().GetCamPos(),
				this->m_NowScenesPtr->GetShadowScale(), false);
		}
		// ��ʂɔ��f
		if (this->m_NowScenesPtr->Get3DActive()) {
			if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
#if defined(_USE_OPENVR_)
				auto* VRParts = VRControl::Instance();
				// UI���X�N���[���ɕ`�悵�Ă���
				VRParts->SetUpBackUI([this]() { DrawUICommon(); });
				// VR�Ɉڂ�
				for (char i = 0; i < 2; ++i) {
					Camera3DInfo tmp_cam = CameraParts->GetMainCamera();
					tmp_cam.SetCamPos(
						tmp_cam.GetCamPos() + VRParts->GetEyePosition(i),
						tmp_cam.GetCamVec() + VRParts->GetEyePosition(i),
						tmp_cam.GetCamUp()
					);
					DrawMain(tmp_cam, true);
					// ���ꂼ��̖ڂɓ��e�𑗐M
					VRParts->SubmitDraw(i, PostPassParts->GetBufferScreen(), [this]() { DrawUIFront(); });
				}
				// �f�B�X�v���C�`��
				GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
				{
					FillGraph(GetDrawScreen(), 0, 0, 0);
					if (VRParts->GetOutBuffer()) {
						VRParts->GetOutBuffer()->DrawRotaGraph(WindowSizeParts->GetUIY(BaseScreenWidth) / 2, WindowSizeParts->GetUIY(BaseScreenHeight) / 2, 0.5f, 0, false);
					}
				}
#endif
			}
			else {
				DrawMain(CameraParts->GetMainCamera(), true);
			}
		}
		else {
			DrawMain(CameraParts->GetMainCamera(), false);
		}
		if (!OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
			// �f�B�X�v���C�`��
			GraphHandle::SetDraw_Screen(static_cast<int>(DX_SCREEN_BACK), true);
			{
				int Prev = GetDrawMode();
				SetDrawMode(DX_DRAWMODE_BILINEAR);
				PostPassParts->GetBufferScreen().DrawExtendGraph(0, 0, WindowSizeParts->GetUIY(BaseScreenWidth), WindowSizeParts->GetUIY(BaseScreenHeight), false);
				SetDrawMode(Prev);
				//
				DrawUICommon();
				DrawUIFront();
			}
		}
	}
	void SceneControl::NextMainLoop(void) noexcept {
		LightPool::Instance()->Dispose();
		this->m_NowScenesPtr->Dispose();										// ���̃V�[������̉��
		if (this->IsEndGame()) {
			this->m_NowScenesPtr->Dispose_Load();								// ���[�h���Ă����f�[�^��j��
		}
		else {
			if (this->m_NowScenesPtr != this->m_NowScenesPtr->Get_Next()) {	// ���̃V�[���Ǝ��̃V�[���Ƃ��ʂ̃V�[���Ȃ�
				this->m_NowScenesPtr->Dispose_Load();								// ���[�h���Ă����f�[�^��j��
			}
			this->m_NowScenesPtr = this->m_NowScenesPtr->Get_Next();	// ���̃V�[���֑J��
			Initialize();
		}
	}
}
