#include "DXLib_ref_101_sequence.hpp"

namespace DXLibRef {
	const SceneControl* SingletonBase<SceneControl>::m_Singleton = nullptr;
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------

	void TEMPSCENE::CubeMapDraw(void) const noexcept {
		if (!Get3DActive()) { return; }
		auto* DrawParts = DXDraw::Instance();
		Vector3DX Pos = DrawParts->SetMainCamera().GetCamPos(); Pos.y *= -1.f;
		DrawParts->Update_CubeMap([this]() { CubeMap_Sub(); }, Pos);
	}

	void TEMPSCENE::ShadowDraw_Far(void) const noexcept {
		if (!Get3DActive()) { return; }
		auto* DrawParts = DXDraw::Instance();
		if (DrawParts->UpdateShadowActive() || GetIsFirstLoop()) {
			DrawParts->Update_Shadow([this]() { ShadowDraw_Far_Sub(); }, Vector3DX::zero(), GetShadowScale() * 4.f, true);
		}
	}

	void TEMPSCENE::ShadowDraw(void) const noexcept {
		if (!Get3DActive()) { return; }
		auto* DrawParts = DXDraw::Instance();
		DrawParts->Update_Shadow([this] { ShadowDraw_Sub(); }, DrawParts->SetMainCamera().GetCamPos(), GetShadowScale(), false);
	}

	void TEMPSCENE::Draw3DVR(std::function<void()> doingUI) noexcept {
		// ‰æ–Ê‚É”½‰f
		auto* DrawParts = DXDraw::Instance();
		DrawParts->Draw3DVR(
			[this]() { BG_Draw_Sub(); },
			[this]() { SetShadowDraw_Rigid_Sub(); },
			[this]() { SetShadowDraw_Sub(); },
			[this]() {
				CalcOnDraw_Sub();
				MainDraw_Sub();
			},
			[this]() { MainDrawFront_Sub(); },
			[&]() {
				DrawUI_Base_Sub();
				// ’Ç‰Á‚Ì•`‰æ•¨
				doingUI();
			},
			[this]() {
				DrawUI_In_Sub();
				WindowSystem::DrawControl::Instance()->Draw();
			}
		);
	}

	void TEMPSCENE::Draw3D(std::function<void()> doingUI) noexcept {
		// ‰æ–Ê‚É”½‰f
		auto* DrawParts = DXDraw::Instance();
		// •`‰æ
		DrawParts->Draw3DMain(
			[this]() { BG_Draw_Sub(); },
			[this]() { SetShadowDraw_Rigid_Sub(); },
			[this]() { SetShadowDraw_Sub(); },
			[this]() {
				CalcOnDraw_Sub();
				MainDraw_Sub();
			},
			[this]() { MainDrawFront_Sub(); },
			DrawParts->GetMainCamera());
		// ƒfƒBƒXƒvƒŒƒC•`‰æ
		DrawParts->DrawFlipDisplay([&]() {
			DrawUI_Base_Sub();
			// ’Ç‰Á‚Ì•`‰æ•¨
			doingUI();
			DrawUI_In_Sub();
			WindowSystem::DrawControl::Instance()->Draw();
			});
	}

	void TEMPSCENE::Draw2D(std::function<void()> doingUI) const noexcept {
		auto* DrawParts = DXDraw::Instance();
		// •`‰æ
		DrawParts->Draw2DMain([this]() { MainDraw_Sub(); });
		// ƒfƒBƒXƒvƒŒƒC•`‰æ
		DrawParts->DrawFlipDisplay([&]() {
			DrawUI_Base_Sub();
			// ’Ç‰Á‚Ì•`‰æ•¨
			doingUI();
			DrawUI_In_Sub();
			WindowSystem::DrawControl::Instance()->Draw();
			});
	}

	void TEMPSCENE::BG_Draw_Sub(void) const noexcept {
		FillGraph(GetDrawScreen(), 192, 192, 192);
	}

	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
};
