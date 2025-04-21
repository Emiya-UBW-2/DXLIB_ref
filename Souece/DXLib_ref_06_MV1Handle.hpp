/*=============================================================================
 Copyright (C) 2020 yumetodo <yume-wikijp@live.jp>
 Distributed under the Boost Software License, Version 1.0.
 (See https:// www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	class MV1 : public DXHandle {
	public:
		class AnimControler {
			class MV1AnimeHandle : public DXHandle {
				const MV1* m_pBaseModel{};
			protected:
				void			Dispose_Sub(void) noexcept override {
					MV1DetachAnim(m_pBaseModel->get(), DXHandle::get());
					m_pBaseModel = nullptr;
				}
			public:
				void			Attach(MV1* model, size_t AnimIndex, const MV1* model_haveanim = nullptr) noexcept {
					m_pBaseModel = model;
					if (model_haveanim && (m_pBaseModel->get() != model_haveanim->get())) {
						DXHandle::SetHandleDirect(MV1AttachAnim(m_pBaseModel->get(), static_cast<int>(AnimIndex), model_haveanim->get()));
					}
					else {
						DXHandle::SetHandleDirect(MV1AttachAnim(m_pBaseModel->get(), static_cast<int>(AnimIndex)));
					}
				}
				const auto		GetTotalTime(void) const noexcept { return MV1GetAttachAnimTotalTime(m_pBaseModel->get(), DXHandle::get()); }
				void			SetTime(float Time) noexcept { MV1SetAttachAnimTime(m_pBaseModel->get(), DXHandle::get(), Time); }
				void			SetBlendRate(float Per) noexcept { MV1SetAttachAnimBlendRate(m_pBaseModel->get(), DXHandle::get(), Per); }
			};
		private:
			MV1AnimeHandle	m_handle;
			float			m_per{ 0.f };
			float			m_time{ 0.f };
			float			m_AllTime{ 0.f };

			float			m_per_prev{ -1.f };
			float			m_time_prev{ -1.f };
		public:
			const auto&		GetPer(void) const noexcept { return this->m_per; }
			const auto&		GetTime(void) const noexcept { return this->m_time; }
			const auto		GetTimePer(void) const noexcept { return (this->m_AllTime > 0) ? this->m_time / this->m_AllTime : 1.f; }
			const auto		TimeStart(void) const noexcept { return this->m_time <= 0; }
			const auto		TimeEnd(void) const noexcept { return this->m_time >= this->m_AllTime; }
		public:
			void			GoStart(void) noexcept { this->m_time = 0.f; }
			void			GoEnd(void) noexcept { this->m_time = this->m_AllTime; }
			void			Reset(void) noexcept {
				this->m_per = 0.f;
				this->m_time = 0.f;
			}
			void			SetPer(float value) noexcept { this->m_per = value; }
			void			SetTime(float value) noexcept { this->m_time = value; }
		public:
			void			Set(MV1* model, size_t AnimIndex, const MV1* model_haveanim = nullptr) noexcept {
				this->m_handle.Attach(model, AnimIndex, model_haveanim);
				Reset();
				UpdateAnim();
				this->m_AllTime = this->m_handle.GetTotalTime();
			}
			void			Update(bool loop, float speed) noexcept;
			void			UpdateAnim(void) noexcept {
				if (this->m_time_prev != this->m_time) {
					this->m_handle.SetTime(this->m_time);
					this->m_time_prev = this->m_time;
				}
				if (this->m_per_prev != this->m_per) {
					this->m_handle.SetBlendRate(this->m_per);
					this->m_per_prev = this->m_per;
				}
			}
		};
	private:
		std::vector<AnimControler>	m_AnimControler;
		size_t m_AnimControlerSize{};
	protected:
		void			Dispose_Sub(void) noexcept override {
			MV1DeleteModel(DXHandle::get());
			this->m_AnimControler.clear();
			this->m_AnimControlerSize = 0;
		}
	public:
		/*読み込み*/
		static void		Load(std::basic_string_view<TCHAR> FileName, MV1* targetMV1, int mode = DX_LOADMODEL_PHYSICS_LOADCALC/*, float Grav = GravityRate*/) noexcept {
			if (mode != DX_LOADMODEL_PHYSICS_LOADCALC) {
				MV1SetLoadModelUsePhysicsMode(mode);
			}
			/*
			if (Grav != GravityRate) {
				MV1SetLoadModelPhysicsWorldGravity(Grav);
			}
			//*/
			targetMV1->SetHandleDirect(DxLib::MV1LoadModelWithStrLen(FileName.data(), FileName.length()));
			/*
			if (Grav != GravityRate) {
				MV1SetLoadModelPhysicsWorldGravity(GravityRate);
			}
			//*/
			if (mode != DX_LOADMODEL_PHYSICS_LOADCALC) {
				MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
			}
			targetMV1->m_AnimControler.clear();
			targetMV1->m_AnimControlerSize = 0;
			return;
		}
		static void		SetAnime(MV1* targetMV1, const MV1& animPaletteMV1) noexcept {
			// 元用、アニメ用が読めていない場合は待つ
			{
				while (ProcessMessage() == 0) {
					if ((CheckHandleASyncLoad(targetMV1->get()) == TRUE) && (CheckHandleASyncLoad(animPaletteMV1.get()) == TRUE)) {
						continue;
					}
					break;
				}
			}
			// エラー
			if (MV1GetAnimNum(animPaletteMV1.get()) < 0) {
				printfDx("error");
				WaitKey();
			}
			targetMV1->m_AnimControlerSize = static_cast<size_t>(MV1GetAnimNum(animPaletteMV1.get()));
			targetMV1->m_AnimControler.resize(targetMV1->m_AnimControlerSize);
			if (targetMV1->GetAnimNum() > 0) {
				for (size_t loop : std::views::iota(static_cast<size_t>(0), targetMV1->GetAnimNum())) {
					targetMV1->SetAnim(loop).Set(targetMV1, loop, &animPaletteMV1);
				}
			}
			return;
		}
		void			Duplicate(const MV1& o) noexcept {
			DXHandle::SetHandleDirect(DxLib::MV1DuplicateModel(o.get()));
		}
	public:
		// getter
		auto			GetPosition(void) const noexcept { return MV1GetPosition(DXHandle::get()); }
		auto			GetOpacityRate(void) const noexcept { return MV1GetOpacityRate(DXHandle::get()); }
		Matrix4x4DX		GetFrameLocalMatrix(int frame) const noexcept { return MV1GetFrameLocalMatrix(DXHandle::get(), frame); }
		Matrix4x4DX		GetFrameLocalWorldMatrix(int frame) const noexcept { return MV1GetFrameLocalWorldMatrix(DXHandle::get(), frame); }
		Matrix4x4DX		GetMatrix(void) const noexcept { return MV1GetMatrix(DXHandle::get()); }
		Vector3DX		GetFramePosition(int frame) const noexcept { return MV1GetFramePosition(DXHandle::get(), frame); }
		auto			GetFrameNum(void) const noexcept { return MV1GetFrameNum(DXHandle::get()); }
		auto			GetFrameParent(int frame) const noexcept { return MV1GetFrameParent(DXHandle::get(), frame); }
		auto			GetFrameChild(int frame, int p2) const noexcept { return MV1GetFrameChild(DXHandle::get(), frame, p2); }
		auto			GetFrameChildNum(int frame) const noexcept { return MV1GetFrameChildNum(DXHandle::get(), frame); }
		std::string		GetFrameName(int frame) const noexcept { return MV1GetFrameName(DXHandle::get(), frame); }
		auto			GetMaterialNum(void) const noexcept { return MV1GetMaterialNum(DXHandle::get()); }
		std::string		GetMaterialName(int material) const noexcept { return MV1GetMaterialName(DXHandle::get(), material); }
		auto			GetMeshNum(void) const noexcept { return MV1GetMeshNum(DXHandle::get()); }
		auto			GetMeshMaxPosition(int mesh) const noexcept { return MV1GetMeshMaxPosition(DXHandle::get(), mesh); }
		auto			GetMeshMinPosition(int mesh) const noexcept { return MV1GetMeshMinPosition(DXHandle::get(), mesh); }
		auto			GetMeshSemiTransState(int mesh) const noexcept { return (MV1GetMeshSemiTransState(DXHandle::get(), mesh) == TRUE); }
		auto			SearchShape(const char* str) const noexcept { return MV1SearchShape(DXHandle::get(), str); }

		auto			GetParentFrameWorldMatrix(int frame) const noexcept { return GetFrameLocalWorldMatrix(GetFrameParent(frame)); }
		auto			GetChildFrameWorldMatrix(int frame, int ID) const noexcept { return GetFrameLocalWorldMatrix(GetFrameChild(frame, ID)); }
	public:
		// 描画
		auto			DrawModel(void) const noexcept { return MV1DrawModel(DXHandle::get()) == TRUE; }
		auto			DrawFrame(int frame) const noexcept { return MV1DrawFrame(DXHandle::get(), frame) == TRUE; }
		auto			DrawMesh(int mesh) const noexcept { return MV1DrawMesh(DXHandle::get(), mesh) == TRUE; }
	public:
		/*モデル*/
		auto			SetPosition(const Vector3DX& pos) const noexcept { return MV1SetPosition(DXHandle::get(), pos.get()) == TRUE; }
		auto			SetRotationZYAxis(const Vector3DX& zaxis, const Vector3DX& yaxis, float zrad) const noexcept { return MV1SetRotationZYAxis(DXHandle::get(), zaxis.get(), yaxis.get(), zrad) == TRUE; }
		auto			SetMatrix(const Matrix4x4DX& mat) const noexcept { return MV1SetMatrix(DXHandle::get(), mat.get()) == TRUE; }
		auto			SetOpacityRate(float alpha) const noexcept { return MV1SetOpacityRate(DXHandle::get(), alpha) == TRUE; }
		auto			SetScale(const Vector3DX& scale) const noexcept { return MV1SetScale(DXHandle::get(), scale.get()) == TRUE; }
		auto			SetTextureGraphHandle(int texindex, const GraphHandle& texHandle, bool trans) const noexcept { return MV1SetTextureGraphHandle(DXHandle::get(), texindex, texHandle.get(), trans ? TRUE : FALSE) == TRUE; }
		auto			SetFrameLocalMatrix(int frame, Matrix4x4DX mat) const noexcept { return MV1SetFrameUserLocalMatrix(DXHandle::get(), frame, mat.get()) == TRUE; }
		void			ResetFrameUserLocalMatrix(int frame) const noexcept { MV1ResetFrameUserLocalMatrix(DXHandle::get(), frame); }
		void			SetMaterialDrawAlphaTestAll(bool Enable, int mode, int param) const noexcept { MV1SetMaterialDrawAlphaTestAll(DXHandle::get(), Enable ? TRUE : FALSE, mode, param); }
		void			SetMaterialTypeAll(int mode) const noexcept { MV1SetMaterialTypeAll(DXHandle::get(), mode); }
		auto			SetShapeRate(int Shapeindex, float Per) const noexcept { return MV1SetShapeRate(DXHandle::get(), Shapeindex, Per) == TRUE; }

		void			SetDifColorScale(COLOR_F color) const noexcept { MV1SetDifColorScale(DXHandle::get(), color); }
		void			SetAmbColorScale(COLOR_F color) const noexcept { MV1SetAmbColorScale(DXHandle::get(), color); }
		void			SetSpcColorScale(COLOR_F color) const noexcept { MV1SetSpcColorScale(DXHandle::get(), color); }
		void			SetMaterialDifColor(int material, COLOR_F color) const noexcept { MV1SetMaterialDifColor(DXHandle::get(), material, color); }
		void			SetMaterialSpcColor(int material, COLOR_F color) const noexcept { MV1SetMaterialSpcColor(DXHandle::get(), material, color); }
		void			SetMaterialAmbColor(int material, COLOR_F color) const noexcept { MV1SetMaterialAmbColor(DXHandle::get(), material, color); }
		void			SetMaterialSpcPower(int material, float power) const noexcept { MV1SetMaterialSpcPower(DXHandle::get(), material, power); }
		void			SetMaterialDrawAddColorAll(int r, int g, int b) const noexcept { MV1SetMaterialDrawAddColorAll(DXHandle::get(), r, g, b); }
		void			SetMaterialOpacityRate(int material, float alpha) const noexcept { MV1SetMeshOpacityRate(DXHandle::get(), material, alpha); }
		
		void			SaveModelToMV1File(std::basic_string_view<TCHAR> FileName, int  SaveType = MV1_SAVETYPE_NORMAL, int AnimMHandle = InvalidID, int AnimNameCheck = TRUE, int Normal8BitFlag = 1, int Position16BitFlag = 1, int Weight8BitFlag = 0, int Anim16BitFlag = 1) const noexcept { MV1SaveModelToMV1FileWithStrLen(DXHandle::get(), FileName.data(), FileName.length(), SaveType, AnimMHandle, AnimNameCheck, Normal8BitFlag, Position16BitFlag, Weight8BitFlag, Anim16BitFlag); }
		void			SetPrioritizePhysicsOverAnimFlag(bool flag) const noexcept { MV1SetPrioritizePhysicsOverAnimFlag(DXHandle::get(), flag ? TRUE : FALSE); }

		/*アニメーション*/
		AnimControler&	SetAnim(size_t animeID) noexcept { return this->m_AnimControler[animeID]; }
		const auto&		GetAnim(size_t animeID) const noexcept { return this->m_AnimControler[animeID]; }
		size_t			GetAnimNum(void) const noexcept { return this->m_AnimControlerSize; }
		auto			UpdateAnim(size_t animeID) noexcept { SetAnim(animeID).UpdateAnim(); }
		auto			UpdateAnimAll(void) noexcept {
			for (auto& a : this->m_AnimControler) {
				a.UpdateAnim();
			}
		}
		/*物理演算*/
		auto			PhysicsResetState(void) const noexcept { return MV1PhysicsResetState(DXHandle::get()) == TRUE; }
		auto			PhysicsCalculation(float TimeMS) const noexcept { return MV1PhysicsCalculation(DXHandle::get(), TimeMS) == TRUE; }
		/*当たり判定*/
		auto			SetupCollInfo(int x = 32, int y = 8, int z = 32, int frame = InvalidID, int mesh = InvalidID) const noexcept {
			return MV1SetupCollInfo(DXHandle::get(), frame, x, y, z, mesh) == 0;
		}
		auto			TerminateCollInfo(int frame = InvalidID, int mesh = InvalidID) const noexcept {
			return MV1TerminateCollInfo(DXHandle::get(),frame, mesh) == 0;
		}

		auto			RefreshCollInfo(int frame = InvalidID, int mesh = InvalidID) const noexcept {
			return MV1RefreshCollInfo(DXHandle::get(), frame, mesh) == 0;
		}
		auto			CollCheck_Line(const Vector3DX& start, const Vector3DX& end, int frame = InvalidID, int mesh = InvalidID) const noexcept {
			return MV1CollCheck_Line(DXHandle::get(), frame, start.get(), end.get(), mesh);
		}
		auto			CollCheck_Sphere(const Vector3DX& startpos, float range, int frame = InvalidID, int mesh = InvalidID) const noexcept {
			return MV1CollCheck_Sphere(DXHandle::get(), frame, startpos.get(), range, mesh);
		}
		auto			CollCheck_Capsule(const Vector3DX& startpos, const Vector3DX& endpos, float range, int frame = InvalidID, int mesh = InvalidID) const noexcept {
			return MV1CollCheck_Capsule(DXHandle::get(), frame, startpos.get(), endpos.get(), range, mesh);
		}
	};
};
