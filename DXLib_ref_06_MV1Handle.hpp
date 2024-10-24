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
			int		m_handle{ 0 };
			float	m_per{ 0.f };
			float	m_time{ 0.f };
			float	m_AllTime{ 0.f };

			float	per_prev{ -1.f };
			float	time_prev{ -1.f };
		public:
			auto& GetPer(void) noexcept { return this->m_per; }
			auto& GetTime(void) noexcept { return this->m_time; }
			auto			GetTimePer(void) const noexcept { return (this->m_AllTime > 0) ? this->m_time / this->m_AllTime : 1.f; }
			auto			TimeStart(void) const noexcept { return m_time <= 0; }
			auto			TimeEnd(void) const noexcept { return m_time >= m_AllTime; }
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
			void			Set(MV1* model, int AnimIndex, const MV1* model_haveanim = nullptr) noexcept {
				this->m_handle = model->AttachAnim(AnimIndex, model_haveanim);
				this->Reset();
				UpdateAnim(model);
				this->m_AllTime = MV1GetAttachAnimTotalTime(model->get(), this->m_handle);
			}
			void			Update(const bool& loop, float speed) noexcept;
			void			UpdateAnim(MV1* model) noexcept {
				if (this->time_prev != this->m_time) {
					MV1SetAttachAnimTime(model->get(), this->m_handle, this->m_time);
					this->time_prev = this->m_time;
				}
				if (this->per_prev != this->m_per) {
					MV1SetAttachAnimBlendRate(model->get(), this->m_handle, this->m_per);
					this->per_prev = this->m_per;
				}
			}
		};
	private:
		std::vector<AnimControler>	m_AnimControler;
	protected:
		void			Dispose_Sub(void) noexcept override {
			MV1DeleteModel(DXHandle::get());
			m_AnimControler.clear();
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
			targetMV1->m_AnimControler.resize(static_cast<size_t>(MV1GetAnimNum(animPaletteMV1.get())));
			if (targetMV1->GetAnimNum() > 0) {
				for (int i : std::views::iota(0, static_cast<int>(targetMV1->GetAnimNum()))) {
					targetMV1->SetAnim(i).Set(targetMV1, i, &animPaletteMV1);
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
		Matrix4x4DX		GetFrameLocalMatrix(int id) const noexcept { return MV1GetFrameLocalMatrix(DXHandle::get(), id); }
		Matrix4x4DX		GetFrameLocalWorldMatrix(int id) const noexcept { return MV1GetFrameLocalWorldMatrix(DXHandle::get(), id); }
		Matrix4x4DX		GetMatrix(void) const noexcept { return MV1GetMatrix(DXHandle::get()); }
		auto			GetFramePosition(int p1) const noexcept { return MV1GetFramePosition(DXHandle::get(), p1); }
		auto			GetFrameNum(void) const noexcept { return MV1GetFrameNum(DXHandle::get()); }
		auto			GetFrameParent(int p1) const noexcept { return MV1GetFrameParent(DXHandle::get(), p1); }
		auto			GetFrameChild(int p1, int p2) const noexcept { return MV1GetFrameChild(DXHandle::get(), p1, p2); }
		auto			GetFrameChildNum(int p1) const noexcept { return MV1GetFrameChildNum(DXHandle::get(), p1); }
		std::string		GetFrameName(int p1) const noexcept { return MV1GetFrameName(DXHandle::get(), p1); }
		auto			GetMaterialNum(void) const noexcept { return MV1GetMaterialNum(DXHandle::get()); }
		std::string		GetMaterialName(int p1) const noexcept { return MV1GetMaterialName(DXHandle::get(), p1); }
		auto			GetMeshNum(void) const noexcept { return MV1GetMeshNum(DXHandle::get()); }
		auto			GetMeshMaxPosition(int p1) const noexcept { return MV1GetMeshMaxPosition(DXHandle::get(), p1); }
		auto			GetMeshMinPosition(int p1) const noexcept { return MV1GetMeshMinPosition(DXHandle::get(), p1); }
		auto			GetMeshSemiTransState(int p1) const noexcept { return (MV1GetMeshSemiTransState(DXHandle::get(), p1) == TRUE); }
		auto			SearchShape(const char* str) const noexcept { return MV1SearchShape(DXHandle::get(), str); }

		auto			GetParentFrameWorldMatrix(int frame) const noexcept { return GetFrameLocalWorldMatrix(GetFrameParent(frame)); }
		auto			GetChildFrameWorldMatrix(int frame, int ID) const noexcept { return GetFrameLocalWorldMatrix(GetFrameChild(frame, ID)); }
	public:
		// 描画
		auto			DrawModel(void) const noexcept { return MV1DrawModel(DXHandle::get()) == TRUE; }
		auto			DrawFrame(int p1) const noexcept { return MV1DrawFrame(DXHandle::get(), p1) == TRUE; }
		auto			DrawMesh(int p1) const noexcept { return MV1DrawMesh(DXHandle::get(), p1) == TRUE; }
	public:
		/*モデル*/
		auto			SetPosition(const Vector3DX& p1) const noexcept { return MV1SetPosition(DXHandle::get(), p1.get()) == TRUE; }
		auto			SetRotationZYAxis(const Vector3DX& zaxis, const Vector3DX& yaxis, float zrad) const noexcept { return MV1SetRotationZYAxis(DXHandle::get(), zaxis.get(), yaxis.get(), zrad) == TRUE; }
		auto			SetMatrix(const Matrix4x4DX& mat) const noexcept { return MV1SetMatrix(DXHandle::get(), mat.get()) == TRUE; }
		auto			SetOpacityRate(float p1) const noexcept { return MV1SetOpacityRate(DXHandle::get(), p1) == TRUE; }
		auto			SetScale(const Vector3DX& p1) const noexcept { return MV1SetScale(DXHandle::get(), p1.get()) == TRUE; }
		auto			SetTextureGraphHandle(int p1, const GraphHandle& p2, bool trans) const noexcept { return MV1SetTextureGraphHandle(DXHandle::get(), p1, p2.get(), trans ? TRUE : FALSE) == TRUE; }
		auto			SetFrameLocalMatrix(int id, Matrix4x4DX mat) const noexcept { return MV1SetFrameUserLocalMatrix(DXHandle::get(), id, mat.get()) == TRUE; }
		void			ResetFrameUserLocalMatrix(int p1) const noexcept { MV1ResetFrameUserLocalMatrix(DXHandle::get(), p1); }
		void			SetMaterialDrawAlphaTestAll(bool Enable, int mode, int param) const noexcept { MV1SetMaterialDrawAlphaTestAll(DXHandle::get(), Enable ? TRUE : FALSE, mode, param); }
		void			SetMaterialTypeAll(int mode) const noexcept { MV1SetMaterialTypeAll(DXHandle::get(), mode); }
		auto			SetShapeRate(int p1, float p2) const noexcept { return MV1SetShapeRate(DXHandle::get(), p1, p2) == TRUE; }

		void			SetDifColorScale(COLOR_F color) const noexcept { MV1SetDifColorScale(DXHandle::get(), color); }
		void			SetAmbColorScale(COLOR_F color) const noexcept { MV1SetAmbColorScale(DXHandle::get(), color); }
		void			SetSpcColorScale(COLOR_F color) const noexcept { MV1SetSpcColorScale(DXHandle::get(), color); }
		void			SetMaterialDifColor(int p1, COLOR_F color) const noexcept { MV1SetMaterialDifColor(DXHandle::get(), p1, color); }
		void			SetMaterialSpcColor(int p1, COLOR_F color) const noexcept { MV1SetMaterialSpcColor(DXHandle::get(), p1, color); }
		void			SetMaterialAmbColor(int p1, COLOR_F color) const noexcept { MV1SetMaterialAmbColor(DXHandle::get(), p1, color); }
		void			SetMaterialSpcPower(int p1, float power) const noexcept { MV1SetMaterialSpcPower(DXHandle::get(), p1, power); }
		void			SetMaterialDrawAddColorAll(int r, int g, int b) const noexcept { MV1SetMaterialDrawAddColorAll(DXHandle::get(), r, g, b); }
		void			SaveModelToMV1File(std::basic_string_view<TCHAR> FileName, int  SaveType = MV1_SAVETYPE_NORMAL, int AnimMHandle = -1, int AnimNameCheck = TRUE, int Normal8BitFlag = 1, int Position16BitFlag = 1, int Weight8BitFlag = 0, int Anim16BitFlag = 1) const noexcept { MV1SaveModelToMV1FileWithStrLen(DXHandle::get(), FileName.data(), FileName.length(), SaveType, AnimMHandle, AnimNameCheck, Normal8BitFlag, Position16BitFlag, Weight8BitFlag, Anim16BitFlag); }
		void			SetPrioritizePhysicsOverAnimFlag(bool p1) const noexcept { MV1SetPrioritizePhysicsOverAnimFlag(DXHandle::get(), p1 ? TRUE : FALSE); }

		/*アニメーション*/
		AnimControler& SetAnim(int p1) noexcept { return this->m_AnimControler.at(static_cast<size_t>(p1)); }
		const AnimControler& GetAnim(int p1) const noexcept { return this->m_AnimControler.at(static_cast<size_t>(p1)); }
		size_t			GetAnimNum(void) const noexcept { return this->m_AnimControler.size(); }
		int				AttachAnim(int AnimIndex, const MV1* model_haveanim = nullptr) noexcept {
			if (model_haveanim && (this->DXHandle::get() != model_haveanim->DXHandle::get())) {
				return MV1AttachAnim(this->DXHandle::get(), AnimIndex, model_haveanim->DXHandle::get());
			}
			else {
				return MV1AttachAnim(this->DXHandle::get(), AnimIndex);
			}
		}
		auto			UpdateAnim(int p1) noexcept { SetAnim(p1).UpdateAnim(this); }
		auto			UpdateAnimAll(void) noexcept {
			for (auto& a : this->m_AnimControler) {
				a.UpdateAnim(this);
			}
		}
		/*物理演算*/
		auto			PhysicsResetState(void) const noexcept { return MV1PhysicsResetState(DXHandle::get()) == TRUE; }
		auto			PhysicsCalculation(float p1) const noexcept { return MV1PhysicsCalculation(DXHandle::get(), p1) == TRUE; }
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
