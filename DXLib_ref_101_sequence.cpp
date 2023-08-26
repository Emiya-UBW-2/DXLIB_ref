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
		//カメラの初期設定
		auto* OptionParts = OPTION::Instance();
		m_MainCamera.SetCamInfo(deg2rad(OptionParts->Get_useVR() ? 120 : OptionParts->Get_Fov()), 0.05f, 200.f);//1P
		//環境光と影の初期化
		auto* DrawParts = DXDraw::Instance();
		DrawParts->SetAmbientLight(GetLightVec(), GetLightColorF());
	}
	bool TEMPSCENE::Update(bool* isPause) noexcept {
		auto ans = Update_Sub(isPause);
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
		this->m_ScreenVertex.SetScreenVertex(DrawParts->m_DispXSize, DrawParts->m_DispYSize);								// 頂点データの準備
		this->m_Shader2D[0].Init("VS_lens.vso", "PS_lens.pso");																//レンズ
		this->m_Shader2D[1].Init("DepthVS.vso", "DepthPS.pso");						//レンズ
		this->m_ScenesPtr = ptr;
	}
	SceneControl::~SceneControl(void) noexcept {
		this->m_ScenesPtr->Dispose();
	}
	//開始
	void SceneControl::StartScene(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		SetUseMaskScreenFlag(FALSE);//←一部画面でエフェクトが出なくなるため入れる
		this->m_ScenesPtr->Set();
		//遠影をセット
		DrawParts->Update_Shadow(
			[&]() {this->m_ScenesPtr->ShadowDraw_Far(); },
			(this->m_ScenesPtr->GetFarShadowMax() + this->m_ScenesPtr->GetFarShadowMin()) / 2,
			(this->m_ScenesPtr->GetFarShadowMax() - this->m_ScenesPtr->GetFarShadowMin()) / 2,
			2);
		this->m_SelEnd = false;
		this->m_SelPause = false;
	}
	//
	bool SceneControl::Execute(void) noexcept {
#ifdef DEBUG
		//auto* DebugParts = DebugClass::Instance();
#endif // DEBUG
		this->m_SelPause = false;
		//更新
		auto SelEnd = !this->m_ScenesPtr->Update(&this->m_SelPause);
		m_SelEnd = this->m_ScenesPtr->GetisEnd();
		//音位置指定
		Set3DSoundListenerPosAndFrontPosAndUpVec(this->m_ScenesPtr->GetMainCamera().GetCamPos().get(), this->m_ScenesPtr->GetMainCamera().GetCamVec().get(), this->m_ScenesPtr->GetMainCamera().GetCamUp().get());
#ifdef _USE_OPENVR_
		//VR空間に適用
		DXDraw::Instance()->Execute();
#endif
		return SelEnd || m_SelEnd;
	}
	//描画
	void SceneControl::Draw(void) noexcept {
		if (!this->m_ScenesPtr->GetIsUpdateDraw()) { return; }
		auto* DrawParts = DXDraw::Instance();
		auto* PostPassParts = PostPassEffect::Instance();
		//影をセット
		DrawParts->Update_Shadow([&] { this->m_ScenesPtr->ShadowDraw(); },
			this->m_ScenesPtr->GetMainCamera().GetCamPos() + (this->m_ScenesPtr->GetNearShadowMax() + this->m_ScenesPtr->GetNearShadowMin()) / 2,
			(this->m_ScenesPtr->GetNearShadowMax() - this->m_ScenesPtr->GetNearShadowMin()) / 2,
			0);
		DrawParts->Update_Shadow([&] { this->m_ScenesPtr->ShadowDraw_NearFar(); },
			this->m_ScenesPtr->GetMainCamera().GetCamPos() + (this->m_ScenesPtr->GetMiddleShadowMax() + this->m_ScenesPtr->GetMiddleShadowMin()) / 2,
			(this->m_ScenesPtr->GetMiddleShadowMax() - this->m_ScenesPtr->GetMiddleShadowMin()) / 2,
			1);
		//深度描画
		PostPassParts->SetDepth(this->m_ScenesPtr->GetMainCamera(), [&] {this->m_ScenesPtr->Depth_Draw(); });	//深度書き込み
		//画面に反映
		DrawParts->Draw(
			this->m_ScenesPtr->GetMainCamera(),
			[&]() { this->m_ScenesPtr->BG_Draw(); },
			[&]() {
			this->m_ScenesPtr->MainDraw();
			PostPassParts->DrawByDepth([&] {
				this->m_ScenesPtr->MainDrawbyDepth();
			});
		},
			[&]() { this->m_ScenesPtr->DrawUI_Base(); },
			[&]() { this->m_ScenesPtr->DrawUI_In(); },
			[&]() {
			//2Dシェーダーを反映する段階
				{
					SetUseTextureToShader(0, PostPassParts->Get_MAIN_Screen().get());	//使用するテクスチャをセット
					//レンズ描画
					if (this->m_ScenesPtr->is_lens()) {
						this->m_Shader2D[0].SetPixelDispSize(DrawParts->m_DispXSize, DrawParts->m_DispYSize);
						this->m_Shader2D[0].SetPixelParam(this->m_ScenesPtr->xp_lens(), this->m_ScenesPtr->yp_lens(), this->m_ScenesPtr->size_lens(), this->m_ScenesPtr->zoom_lens());
						PostPassParts->Get_BUF_Screen().SetDraw_Screen(false);
						{
							this->m_Shader2D[0].Draw(this->m_ScreenVertex);
						}
						PostPassParts->FlipBuftoMain();
					}
					//描画
					if (this->m_ScenesPtr->is_Blackout()) {
						this->m_Shader2D[1].SetPixelDispSize(DrawParts->m_DispXSize, DrawParts->m_DispYSize);
						this->m_Shader2D[1].SetPixelParam(this->m_ScenesPtr->Per_Blackout(), 0, 0, 0);
						PostPassParts->Get_BUF_Screen().SetDraw_Screen(false);
						{
							this->m_Shader2D[1].Draw(this->m_ScreenVertex);
						}
						PostPassParts->FlipBuftoMain();
					}
					SetUseTextureToShader(0, -1);
				}
		});
	}
	//
	void SceneControl::NextScene(void) noexcept {
		this->m_ScenesPtr->Dispose();							//解放
		this->m_ScenesPtr = this->m_ScenesPtr->Get_Next();		//遷移
	}
	//
};
