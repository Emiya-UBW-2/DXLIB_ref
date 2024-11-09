#include "DXLib_ref_101_sequence.hpp"

namespace DXLibRef {
	const SceneControl* SingletonBase<SceneControl>::m_Singleton = nullptr;
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	// FPS�\��
	void SceneControl::FPSDrawer::Initialize(void) noexcept {
		// �e�X�̐��l��������
		for (auto& f : FPSAvgs) {
			f = FrameRate;
		}
		m_FPSAvgCount = 0;
	}
	void SceneControl::FPSDrawer::Update(void) noexcept {
		// m_FPSAvgCount�̔ԍ��ɑ΂��č��̃t���[�����[�g��ۑ�
		FPSAvgs.at(m_FPSAvgCount) = DXLib_ref::Instance()->GetFps();
		// �ۑ�����ꏊ�����炷
		++m_FPSAvgCount %= FPSAvgs.size();
		// �ۑ����Ă���ߋ���FPS�l�̕��ς��Ƃ�
		m_FPSAvg = 0.f;
		for (auto& f : FPSAvgs) {
			m_FPSAvg += f;
		}
		m_FPSAvg = m_FPSAvg / static_cast<float>(FPSAvgs.size());
	}
	void SceneControl::FPSDrawer::DrawFPSCounter(void) const noexcept {
		auto* DrawCtrls = WindowSystem::DrawControl::Instance();
		auto* OptionParts = OPTION::Instance();
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
		DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
			LineHeight, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
			BaseScreenWidth - 8, 8, color, Black, "%5.2f FPS", m_FPSAvg);
		// �h���[�R�[��(DirectX�ɉ���`��w���𑗂�����)�̕\��
		DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
			LineHeight, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
			BaseScreenWidth - 8, 8 + 20, White, Black, "%d Drawcall", GetDrawCallCount());
	}
	// �|�[�Y���
	void SceneControl::PauseDrawer::Update(void) noexcept {
		// �|�[�Y��ʂł͓_�ł̉��Z���s��
		m_PauseFlashCount += DXLib_ref::Instance()->GetDeltaTime();
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
			DrawCtrls->SetString(WindowSystem::DrawLayer::Normal, FontPool::FontType::MS_Gothic,
				36, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
				16, 16, Green, Black, "Pause");
		}
	}
	//
	void SceneControl::Draw3DMain(const Camera3DInfo& camInfo) const noexcept {
		auto* PostPassParts = PostPassEffect::Instance();
		PostPassParts->SetCamMat(camInfo);
		// �S�Ẳ�ʂ�������
		PostPassParts->ResetBuffer();
		// ��
		PostPassParts->DrawGBuffer(1000.0f, 50000.0f, [this]() { this->m_NowScenesPtr->BG_Draw(); });
		// ������
		PostPassParts->DrawGBuffer(camInfo.GetCamFar() - 10.f, 1000000.f, [&]() {
			auto* PostPassParts = PostPassEffect::Instance();
			PostPassParts->DrawByPBR([this]() {
				this->m_NowScenesPtr->CalcOnDraw();
				this->m_NowScenesPtr->MainDraw();
				});
			this->m_NowScenesPtr->MainDrawFront();
			});
		// ����
		PostPassParts->DrawGBuffer(camInfo.GetCamNear(), camInfo.GetCamFar(), [&]() {
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
		PostPassParts->DrawGBuffer(0.1f, 0.1f + camInfo.GetCamNear(), [&]() {
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
		// �|�X�g�v���Z�X
		PostPassParts->DrawPostProcess(camInfo, [this]() { this->m_NowScenesPtr->SetShadowDraw_Rigid(); }, [this]() { this->m_NowScenesPtr->SetShadowDraw(); });
	}
	void SceneControl::DrawUICommon(void) const noexcept {
		auto* WindowSizeParts = WindowSizeControl::Instance();
		auto* SideLogParts = SideLog::Instance();
		//
		this->m_NowScenesPtr->DrawUI_Base();
		// �|�[�Y�`���ݒ�
		if (IsPause()) {
			m_PauseDrawer.DrawPause();
		}
		// FPS�\���`���ݒ�
		m_FPSDrawer.DrawFPSCounter();
		PadControl::Instance()->Draw();
		SideLogParts->Draw();
		PopUp::Instance()->Draw(WindowSizeParts->GetUIXMax() / 2, WindowSizeParts->GetUIYMax() / 2);
#if defined(DEBUG)
		DebugClass::Instance()->DebugWindow(WindowSizeParts->GetUIXMax() - WindowSizeParts->GetUIY(350), WindowSizeParts->GetUIY(150));
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
			auto* Pad = PadControl::Instance();
			Pad->SetGuideUpdate();
		}
		//�|�b�v�A�b�v�����ׂč폜�Ƃ���
		PopUpParts->EndAll();
	}
	//
	void SceneControl::Initialize(void) noexcept {
		auto* OptionParts = OPTION::Instance();
		auto* WindowSizeParts = WindowSizeControl::Instance();
		auto* Pad = PadControl::Instance();
		auto* PostPassParts = PostPassEffect::Instance();
		Pad->Dispose();
		PostPassParts->ResetAllBuffer();
		//
		this->m_NowScenesPtr->Load();
		//
		SetUseMaskScreenFlag(FALSE);// ���ꕔ��ʂŃG�t�F�N�g���o�Ȃ��Ȃ邽�ߓ����
		// �J�����̏����ݒ�
		WindowSizeParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamBoolean(EnumSaveParam::usevr) ? 120 : OptionParts->GetParamInt(EnumSaveParam::fov)), 0.05f, 200.f);
		// �����Ɖe�̏�����
		WindowSizeParts->SetAmbientLight(Vector3DX::vget(0.25f, -1.f, 0.25f), GetColorF(1.f, 1.f, 1.f, 0.0f));
		this->m_NowScenesPtr->Set();
		Pad->SetGuideUpdate();
		// FPS�\���̏�����
		m_FPSDrawer.Initialize();
	}
	void SceneControl::Update(void) noexcept {
		auto* WindowSizeParts = WindowSizeControl::Instance();
		auto* Pad = PadControl::Instance();
		auto* PopUpParts = PopUp::Instance();
		auto* OptionWindowParts = OptionWindowClass::Instance();
		auto* LocalizeParts = LocalizePool::Instance();
		auto* SideLogParts = SideLog::Instance();
		auto* PostPassParts = PostPassEffect::Instance();
		WindowSystem::DrawControl::Instance()->ClearList();
		if (Pad->GetEsc().trigger() && !m_IsExitSelect) {
			m_IsExitSelect = true;
			PopUpParts->Add(LocalizeParts->Get(100), 480, 240,
				[this](int xmin, int ymin, int xmax, int ymax, bool) {
					auto* WindowSizeParts = WindowSizeControl::Instance();
					auto* LocalizeParts = LocalizePool::Instance();
					int xp1, yp1;
					// �^�C�g��
					{
						xp1 = xmin + WindowSizeParts->GetUIY(24);
						yp1 = ymin + LineHeight;

						WindowSystem::SetMsg(xp1, yp1 + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, LocalizeParts->Get(101));
					}
					// 
					{
						xp1 = (xmax + xmin) / 2 - WindowSizeParts->GetUIY(54);
						yp1 = ymax - LineHeight * 3;

						auto* Pad = PadControl::Instance();
						bool ret = WindowSystem::SetMsgClickBox(xp1, yp1, xp1 + WindowSizeParts->GetUIY(108), yp1 + LineHeight * 2, LineHeight, Gray15, false, true, LocalizeParts->Get(102));
						if (Pad->GetKey(PADS::INTERACT).trigger() || ret) {
							// �I���t���O�𗧂Ă�
							this->m_IsEndGame = true;
						}
					}
				},
				[this]() {
					m_IsExitSelect = false;
				},
				[]() {},
				true
			);
		}
		if (OptionWindowParts->IsRestartSwitch() && !m_IsRestartSelect) {
			m_IsRestartSelect = true;
			PopUpParts->Add(LocalizeParts->Get(100), 480, 240,
				[this](int xmin, int ymin, int xmax, int ymax, bool) {
					auto* WindowSizeParts = WindowSizeControl::Instance();
					auto* LocalizeParts = LocalizePool::Instance();
					int xp1, yp1;
					// �^�C�g��
					{
						xp1 = xmin + WindowSizeParts->GetUIY(24);
						yp1 = ymin + LineHeight;

						WindowSystem::SetMsg(xp1, yp1 + LineHeight / 2, LineHeight, FontHandle::FontXCenter::LEFT, White, Black, LocalizeParts->Get(2101));
					}
					// 
					{
						xp1 = (xmax + xmin) / 2 - WindowSizeParts->GetUIY(54);
						yp1 = ymax - LineHeight * 3;

						auto* Pad = PadControl::Instance();
						bool ret = WindowSystem::SetMsgClickBox(xp1, yp1, xp1 + WindowSizeParts->GetUIY(108), yp1 + LineHeight * 2, LineHeight, Gray15, false, true, LocalizeParts->Get(2102));
						if (Pad->GetKey(PADS::INTERACT).trigger() || ret) {
							this->m_IsEndGame = true;
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
		Set3DSoundListenerPosAndFrontPosAndUpVec(WindowSizeParts->SetMainCamera().GetCamPos().get(), WindowSizeParts->SetMainCamera().GetCamVec().get(), WindowSizeParts->SetMainCamera().GetCamUp().get());		// ���ʒu�w��
#if defined(_USE_OPENVR_)
		VRControl::Instance()->Execute();
#endif
		PostPassParts->Update();
		CameraShake::Instance()->Update();
		SideLogParts->Update();
		PopUpParts->Update();
		// �|�[�Y��ʂ̍X�V
		if (IsPause()) {
			m_PauseDrawer.Update();
		}
		// �|�[�Y���͂ɂ��I���I�t
		if (Pad->GetKey(PADS::INVENTORY).trigger()) {
			ChangePause(!IsPause());
		}
		// FPS�\���@�\�̍X�V
		m_FPSDrawer.Update();
	}
	void SceneControl::DrawMainLoop(void) const noexcept {
		auto* OptionParts = OPTION::Instance();
		auto* PostPassParts = PostPassEffect::Instance();
		auto* WindowSizeParts = WindowSizeControl::Instance();
		if (this->m_NowScenesPtr->Get3DActive()) {
			// �L���[�u�}�b�v���Z�b�g
			Vector3DX Pos = WindowSizeParts->SetMainCamera().GetCamPos(); Pos.y *= -1.f;
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
				WindowSizeParts->SetMainCamera().GetCamPos(),
				this->m_NowScenesPtr->GetShadowScale(), false);
		}
		// ��ʂɔ��f
		if (this->m_NowScenesPtr->Get3DActive()) {
			if (OptionParts->GetParamBoolean(EnumSaveParam::usevr)) {
#if defined(_USE_OPENVR_)
				// UI���X�N���[���ɕ`�悵�Ă���
				VRControl::Instance()->SetUpBackUI([&]() { DrawUICommon(); });
				// VR�Ɉڂ�
				for (char i = 0; i < 2; ++i) {
					Camera3DInfo tmp_cam = WindowSizeParts->GetMainCamera();
					tmp_cam.SetCamPos(
						tmp_cam.GetCamPos() + VRControl::Instance()->GetEyePosition(i),
						tmp_cam.GetCamVec() + VRControl::Instance()->GetEyePosition(i),
						tmp_cam.GetCamUp()
					);
					Draw3DMain(tmp_cam);
					// ���ꂼ��̖ڂɓ��e�𑗐M
					VRControl::Instance()->SubmitDraw(i, PostPassParts->GetBufferScreen(), [this]() { DrawUIFront(); });
				}
				// �f�B�X�v���C�`��
				GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
				{
					FillGraph(GetDrawScreen(), 0, 0, 0);
					if (VRControl::Instance()->GetOutBuffer()) {
						VRControl::Instance()->GetOutBuffer()->DrawRotaGraph(WindowSizeParts->GetScreenXMax() / 2, WindowSizeParts->GetScreenYMax() / 2, 0.5f, 0, false);
					}
				}
#endif
			}
			else {
				Draw3DMain(WindowSizeParts->GetMainCamera());
				// �f�B�X�v���C�`��
				GraphHandle::SetDraw_Screen(static_cast<int>(DX_SCREEN_BACK), true);
				{
					int Prev = GetDrawMode();
					SetDrawMode(DX_DRAWMODE_BILINEAR);
					PostPassParts->GetBufferScreen().DrawExtendGraph(0, 0, WindowSizeParts->GetUIXMax(), WindowSizeParts->GetUIYMax(), false);
					SetDrawMode(Prev);
					//
					DrawUICommon();
					DrawUIFront();
				}
			}
		}
		else {
			PostPassParts->Set_DoFNearFar(0.1f, 5.f, 0.05f, 6.f);			// Dof�𖳌���
			PostPassParts->ResetBuffer();			// �S�Ẳ�ʂ�������
			// 2D�`��
			PostPassParts->GetBufferScreen().SetDraw_Screen();
			{
				this->m_NowScenesPtr->MainDraw();
			}
			PostPassParts->DrawPostProcess(Camera3DInfo(), []() {}, []() {});			// �|�X�g�v���Z�X
			// �f�B�X�v���C�`��
			GraphHandle::SetDraw_Screen(static_cast<int>(DX_SCREEN_BACK), true);
			{
				int Prev = GetDrawMode();
				SetDrawMode(DX_DRAWMODE_BILINEAR);
				PostPassParts->GetBufferScreen().DrawExtendGraph(0, 0, WindowSizeParts->GetUIXMax(), WindowSizeParts->GetUIYMax(), false);
				SetDrawMode(Prev);
				//
				DrawUICommon();
				DrawUIFront();
			}
		}
	}
	void SceneControl::ExitMainLoop(void) noexcept {
		this->m_NowScenesPtr->Dispose();										// ���̃V�[������̉��
		if (this->m_NowScenesPtr != this->m_NowScenesPtr->Get_Next()) {	// ���̃V�[���Ǝ��̃V�[���Ƃ��ʂ̃V�[���Ȃ�
			this->m_NowScenesPtr->Dispose_Load();								// ���[�h���Ă����f�[�^��j��
		}
		this->m_NowScenesPtr = this->m_NowScenesPtr->Get_Next();	// ���̃V�[���֑J��
	}
};
