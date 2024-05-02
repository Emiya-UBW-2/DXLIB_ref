#include "DXLib_ref_101_sequence.hpp"

namespace DXLib_ref {
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	void TEMPSCENE::Draw(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		//影をセット
		DrawParts->Update_Shadow([&] {ShadowDraw_Far(); }, DrawParts->SetMainCamera().GetCamPos(), 2);
		DrawParts->Update_Shadow([&] { ShadowDraw_NearFar_Sub(); }, DrawParts->SetMainCamera().GetCamPos(), 1);
		DrawParts->Update_Shadow([&] { ShadowDraw_Sub(); }, DrawParts->SetMainCamera().GetCamPos(), 0);
		DrawParts->Update_NearShadow([&] {MainDraw_Sub(); });
		//画面に反映
		DrawParts->Draw(
			[&](const Camera3DInfo& cams) {
				auto* PostPassParts = PostPassEffect::Instance();
				PostPassParts->Draw(
					[&]() { BG_Draw_Sub(); },
					[&]() { MainDraw_Sub(); },
					[&]() { MainDrawFront_Sub(); },
					cams);
			},
			[&]() { DrawUI_Base_Sub(); },
			[&]() { DrawUI_In_Sub(); }
		);
	}

	//開始
	void SceneControl::StartScene(void) noexcept {
		SetUseMaskScreenFlag(FALSE);//←一部画面でエフェクトが出なくなるため入れる
		//カメラの初期設定
		auto* OptionParts = OPTION::Instance();
		auto* DrawParts = DXDraw::Instance();
		DrawParts->SetMainCamera().SetCamInfo(deg2rad(OptionParts->GetParamBoolean(EnumSaveParam::usevr) ? 120 : OptionParts->GetParamInt(EnumSaveParam::SSAO)), 0.05f, 200.f);
		//環境光と影の初期化
		Vector3DX DefaultVec = Vector3DX::vget(0.25f, -1.f, 0.25f);
		DrawParts->SetAmbientLight(DefaultVec, GetColorF(1.f, 1.f, 1.f, 0.0f));
		DrawParts->SetShadow(DefaultVec, Vector3DX::vget(-100.f, -10.f, -100.f), Vector3DX::vget(100.f, 10.f, 100.f), 0);
		DrawParts->SetShadow(DefaultVec, Vector3DX::vget(-100.f, -10.f, -100.f), Vector3DX::vget(100.f, 10.f, 100.f), 1);
		DrawParts->SetShadow(DefaultVec, Vector3DX::vget(-100.f, -10.f, -100.f), Vector3DX::vget(100.f, 10.f, 100.f), 2);
		DrawParts->SetIsUpdateShadow(0, true);
		DrawParts->SetIsUpdateShadow(1, true);
		this->m_ScenesPtr->Set();
		DrawParts->SetIsUpdateShadow(2, true);
		DrawParts->Update_Shadow([&]() {this->m_ScenesPtr->ShadowDraw_Far(); }, Vector3DX::zero(), 2);		//遠影をセット
		DrawParts->SetIsUpdateShadow(2, false);
		PadControl::Instance()->SetGuideUpdate();
		//FPS表示
		for (auto& f : FPSAvgs) {
			f = 60.f;
		}
		m_FPSAvg = 0;
	}
	//
	bool SceneControl::Execute(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		PadControl::Instance()->Execute();
		auto SelEnd = !this->m_ScenesPtr->Update();		//更新
		Set3DSoundListenerPosAndFrontPosAndUpVec(DrawParts->SetMainCamera().GetCamPos().get(), DrawParts->SetMainCamera().GetCamVec().get(), DrawParts->SetMainCamera().GetCamUp().get());		//音位置指定
		OptionWindowClass::Instance()->Execute();
		DrawParts->Execute();
		return SelEnd;
	}
	//描画
	void SceneControl::Draw(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		//
		EffectResource::Instance()->Calc(DrawParts->IsPause());//エフェクシアのアプデを60FPS相当に変更
		//
		this->m_ScenesPtr->Draw();
		//デバッグ
		{
			FPSAvgs.at(m_FPSAvg) = GetFPS();
			++m_FPSAvg %= ((int)FPSAvgs.size());

			float Avg = 0.f;
			for (auto& f : FPSAvgs) {
				Avg += f;
			}
			Avg = Avg / ((float)FPSAvgs.size());

			auto* Fonts = FontPool::Instance();
			Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, y_r(1920 - 8), y_r(8), White, Black, "%5.2f FPS", Avg);
		}
	}
	//
	void SceneControl::NextScene(void) noexcept {
		this->m_ScenesPtr->Dispose();							//解放
		this->m_ScenesPtr = this->m_ScenesPtr->Get_Next();		//遷移
		PadControl::Instance()->Dispose();
	}
	//
};
