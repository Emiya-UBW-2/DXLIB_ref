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
		//カメラの初期設定
		auto* OptionParts = OPTION::Instance();
		auto* DrawParts = DXDraw::Instance();
		DrawParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamBoolean(EnumSaveParam::usevr) ? 120 : OptionParts->GetParamInt(EnumSaveParam::SSAO)), 0.05f, 200.f);
		//環境光と影の初期化
		Vector3DX DefaultVec = Vector3DX::vget(0.25f, -1.f, 0.25f);
		DrawParts->SetAmbientLight(DefaultVec, GetColorF(1.f, 1.f, 1.f, 0.0f));
		Set_Sub();
		DrawParts->Update_Shadow([&]() { ShadowDraw_Far_Sub(); }, Vector3DX::zero(), true);		//遠影をセット
	}
	bool TEMPSCENE::Update() noexcept {
		auto* DrawParts = DXDraw::Instance();
		if (DrawParts->UpdateShadowActive() || m_IsFirstLoop) {
			DrawParts->Update_Shadow([&]() { ShadowDraw_Far_Sub(); }, Vector3DX::zero(), true);		//遠影をセット
		}
		auto ans = Update_Sub();
		m_IsFirstLoop = false;
		return ans;
	}
	void TEMPSCENE::Draw(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		//影をセット
		DrawParts->Update_Shadow([&] { ShadowDraw_Sub(); }, DrawParts->SetMainCamera().GetCamPos(), false);
		//画面に反映
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
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	void SceneControl::StartScene(void) noexcept {
		SetUseMaskScreenFlag(FALSE);//←一部画面でエフェクトが出なくなるため入れる
		this->m_ScenesPtr->Set();
		PadControl::Instance()->SetGuideUpdate();
		//FPS表示
		for (auto& f : FPSAvgs) {
			f = 60.f;
		}
		m_FPSAvg = 0;
	}
	bool SceneControl::Execute(void) noexcept {
		PadControl::Instance()->Execute();
		auto SelEnd = !this->m_ScenesPtr->Update();		//更新
		OptionWindowClass::Instance()->Execute();
		DXDraw::Instance()->Execute();
		SideLog::Instance()->Update();
		PopUp::Instance()->Update();
		return SelEnd;
	}
	void SceneControl::Draw(void) noexcept {
		this->m_ScenesPtr->Draw();
		//デバッグ
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
		PopUp::Instance()->Draw();
	}
	void SceneControl::NextScene(void) noexcept {
		this->m_ScenesPtr->Dispose();							//解放
		this->m_ScenesPtr = this->m_ScenesPtr->Get_Next();		//遷移
		PadControl::Instance()->Dispose();
	}
};
