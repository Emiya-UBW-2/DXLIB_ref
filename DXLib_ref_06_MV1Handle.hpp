/*=============================================================================
 Copyright (C) 2020 yumetodo <yume-wikijp@live.jp>
 Distributed under the Boost Software License, Version 1.0.
 (See https://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	class MV1 {
	public:
		class AnimControler {
			int		m_handle{ 0 };
			float	m_per{ 0.f };
			float	m_time{ 0.f };
			float	m_AllTime{ 0.f };

			float	per_prev{ -1.f };
			float	time_prev{ -1.f };
		public:
			auto&			GetPer(void) noexcept { return this->m_per; }
			auto&			GetTime(void) noexcept { return this->m_time; }
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
			void			Set(MV1* model, int AnimIndex, const MV1* model_haveanim = nullptr) {
				this->m_handle = model->AttachAnim(AnimIndex, model_haveanim);
				this->Reset();
				UpdateAnim(model);
				this->m_AllTime = MV1GetAttachAnimTotalTime(model->GetHandle(), this->m_handle);
			}
			void			Update(const bool& loop, float speed);
			void			UpdateAnim(MV1* model) noexcept {
				if (this->time_prev != this->m_time) {
					MV1SetAttachAnimTime(model->GetHandle(), this->m_handle, this->m_time);
					this->time_prev = this->m_time;
				}
				if (this->per_prev != this->m_per) {
					MV1SetAttachAnimBlendRate(model->GetHandle(), this->m_handle, this->m_per);
					this->per_prev = this->m_per;
				}
			}
		};
	private:
		int							m_handle;
		std::vector<AnimControler>	m_AnimControler;
		static constexpr int invalid_handle = INVALID_ID;
	public:
		/*コンストラクタ*/
		MV1(void) noexcept { m_handle = invalid_handle; }
		MV1(int h) noexcept : m_handle(h) {}
		MV1(const MV1&) = delete;
		MV1(MV1&& o) noexcept : m_handle(o.m_handle) { o.m_handle = invalid_handle; }
		/*オペレーター*/
		MV1& operator=(const MV1&) = delete;
		MV1& operator=(MV1&& o) noexcept {
			this->m_handle = o.m_handle;
			o.m_handle = invalid_handle;
			return *this;
		}
		/*デストラクタ*/
		~MV1(void) noexcept {
			Dispose();
		}
	public:
		/*読み込み*/
		static void		Load(std::basic_string_view<TCHAR> FileName, MV1* _ModelHandle, int mode = DX_LOADMODEL_PHYSICS_LOADCALC, float Grav = M_GR) noexcept {
			if (mode != DX_LOADMODEL_PHYSICS_LOADCALC) {
				MV1SetLoadModelUsePhysicsMode(mode);
			}
			if (Grav != M_GR) {
				//MV1SetLoadModelPhysicsWorldGravity(Grav);
			}
			*_ModelHandle = DxLib::MV1LoadModelWithStrLen(FileName.data(), FileName.length());
			if (Grav != M_GR) {
				//MV1SetLoadModelPhysicsWorldGravity(M_GR);
			}
			if (mode != DX_LOADMODEL_PHYSICS_LOADCALC) {
				MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
			}
			_ModelHandle->m_AnimControler.clear();
			return;
		}
		static void		SetAnime(MV1* _ModelHandle, const MV1& _Have_Anim)/* noexcept*/ {
			//元用、アニメ用が読めていない場合は待つ
			{
				while (ProcessMessage() == 0) {
					if ((CheckHandleASyncLoad(_ModelHandle->GetHandle()) == TRUE) && (CheckHandleASyncLoad(_Have_Anim.GetHandle()) == TRUE)) { continue; }
					break;
				}
			}
			//エラー
			if (MV1GetAnimNum(_Have_Anim.GetHandle()) < 0) {
				printfDx("error");
				WaitKey();
			}
			_ModelHandle->m_AnimControler.resize(static_cast<size_t>(MV1GetAnimNum(_Have_Anim.GetHandle())));
			if (_ModelHandle->m_AnimControler.size() > 0) {
				for (int i = 0; i < int(_ModelHandle->m_AnimControler.size()); ++i) {
					_ModelHandle->m_AnimControler.at(static_cast<size_t>(i)).Set(_ModelHandle, i, &_Have_Anim);
				}
			}
			return;
		}
		static void		LoadonAnime(void) noexcept {
			//これに切り替え
			//MV1::Load(model[Max].Path, &(model[Max].obj));
			//model[Max].obj.SetAnime(&(model[Max].obj), model[Max].obj);
			return;
		}
		MV1				Duplicate(void) const noexcept { return DxLib::MV1DuplicateModel(GetHandle()); }
		/*削除*/
		void			Dispose(void) noexcept {
			if (this->m_handle != invalid_handle) {
				MV1DeleteModel(this->m_handle);
				m_AnimControler.clear();
				this->m_handle = invalid_handle;
			}
		}
	public:
		//getter
		const int&		GetHandle(void) const noexcept { return this->m_handle; }
		auto			IsActive(void) const noexcept { return GetHandle() != invalid_handle; }
		auto			GetPosition(void) const noexcept { return MV1GetPosition(GetHandle()); }
		auto			GetOpacityRate(void) const noexcept { return MV1GetOpacityRate(GetHandle()); }
		Matrix4x4DX		GetFrameLocalMatrix(int id) const noexcept { return MV1GetFrameLocalMatrix(GetHandle(), id); }
		Matrix4x4DX		GetFrameLocalWorldMatrix(int id) const noexcept { return MV1GetFrameLocalWorldMatrix(GetHandle(), id); }
		Matrix4x4DX		GetMatrix(void) const noexcept { return MV1GetMatrix(GetHandle()); }
		auto			GetFramePosition(int p1) const noexcept { return MV1GetFramePosition(GetHandle(), p1); }
		auto			GetFrameNum(void) const noexcept { return MV1GetFrameNum(GetHandle()); }
		auto			GetFrameParent(int p1) const noexcept { return MV1GetFrameParent(GetHandle(), p1); }
		auto			GetFrameChild(int p1, int p2) const noexcept { return MV1GetFrameChild(GetHandle(), p1, p2); }
		auto			GetFrameChildNum(int p1) const noexcept { return MV1GetFrameChildNum(GetHandle(), p1); }
		std::string		GetFrameName(int p1) const noexcept { return MV1GetFrameName(GetHandle(), p1); }
		auto			GetMaterialNum(void) const noexcept { return MV1GetMaterialNum(GetHandle()); }
		std::string		GetMaterialName(int p1) const noexcept { return MV1GetMaterialName(GetHandle(), p1); }
		auto			GetMeshNum(void) const noexcept { return MV1GetMeshNum(GetHandle()); }
		auto			GetMeshMaxPosition(int p1) const noexcept { return MV1GetMeshMaxPosition(GetHandle(), p1); }
		auto			GetMeshMinPosition(int p1) const noexcept { return MV1GetMeshMinPosition(GetHandle(), p1); }
		auto			GetMeshSemiTransState(int p1) const noexcept { return (MV1GetMeshSemiTransState(GetHandle(), p1) == TRUE); }
		auto			SearchShape(const char* str) const noexcept { return MV1SearchShape(GetHandle(), str); }
	public:
		//描画
		auto			DrawModel(void) const noexcept { return MV1DrawModel(GetHandle()) == TRUE; }
		auto			DrawFrame(int p1) const noexcept { return MV1DrawFrame(GetHandle(), p1) == TRUE; }
		auto			DrawMesh(int p1) const noexcept { return MV1DrawMesh(GetHandle(), p1) == TRUE; }
	public:
		/*モデル*/
		auto			SetPosition(const Vector3DX& p1) const noexcept { return MV1SetPosition(GetHandle(), p1.get()) == TRUE; }
		auto			SetRotationZYAxis(const Vector3DX& zaxis, const Vector3DX& yaxis, float zrad) const noexcept { return MV1SetRotationZYAxis(GetHandle(), zaxis.get(), yaxis.get(), zrad) == TRUE; }
		auto			SetMatrix(const Matrix4x4DX& mat) const noexcept { return MV1SetMatrix(GetHandle(), mat.get()) == TRUE; }
		auto			SetOpacityRate(float p1) const noexcept { return MV1SetOpacityRate(GetHandle(), p1) == TRUE; }
		auto			SetScale(const Vector3DX& p1) const noexcept { return MV1SetScale(GetHandle(), p1.get()) == TRUE; }
		auto			SetTextureGraphHandle(int p1, const GraphHandle& p2, bool trans) const noexcept { return MV1SetTextureGraphHandle(GetHandle(), p1, p2.get(), trans ? TRUE : FALSE) == TRUE; }
		auto			SetFrameLocalMatrix(int id, Matrix4x4DX mat) const noexcept { return MV1SetFrameUserLocalMatrix(GetHandle(), id, mat.get()) == TRUE; }
		void			ResetFrameUserLocalMatrix(int p1) const noexcept { MV1ResetFrameUserLocalMatrix(GetHandle(), p1); }
		void			SetMaterialDrawAlphaTestAll(bool Enable, int mode, int param) const noexcept { MV1SetMaterialDrawAlphaTestAll(GetHandle(), Enable ? TRUE : FALSE, mode, param); }
		void			SetMaterialTypeAll(int mode) const noexcept { MV1SetMaterialTypeAll(GetHandle(), mode); }
		auto			SetShapeRate(int p1, float p2) const noexcept { return MV1SetShapeRate(GetHandle(), p1, p2) == TRUE; }

		void			SetMaterialDifColor(int p1, COLOR_F color) const noexcept { MV1SetMaterialDifColor(GetHandle(), p1, color); }
		void			SetMaterialSpcColor(int p1, COLOR_F color) const noexcept { MV1SetMaterialSpcColor(GetHandle(), p1, color); }
		void			SetMaterialAmbColor(int p1, COLOR_F color) const noexcept { MV1SetMaterialAmbColor(GetHandle(), p1, color); }
		void			SetMaterialSpcPower(int p1,float power) const noexcept { MV1SetMaterialSpcPower(GetHandle(), p1, power); }
		void			SaveModelToMV1File(std::basic_string_view<TCHAR> FileName) const noexcept { MV1SaveModelToMV1FileWithStrLen(GetHandle(), FileName.data(), FileName.length()); }

		/*アニメーション*/
		auto&			SetAnim(int p1) noexcept { return this->m_AnimControler.at(static_cast<size_t>(p1)); }
		auto&			GetAnim(int p1) const noexcept { return this->m_AnimControler.at(static_cast<size_t>(p1)); }
		auto			GetAnimNum(void) const noexcept { return this->m_AnimControler.size(); }
		int				AttachAnim(int AnimIndex, const MV1* model_haveanim = nullptr) noexcept {
			if (model_haveanim && (this->GetHandle() != model_haveanim->GetHandle())) {
				return MV1AttachAnim(this->GetHandle(), AnimIndex, model_haveanim->GetHandle());
			}
			else {
				return MV1AttachAnim(this->GetHandle(), AnimIndex);
			}
		}
		auto			UpdateAnim(int p1) noexcept { SetAnim(p1).UpdateAnim(this); }
		auto			UpdateAnimAll(void) noexcept {
			for (auto& a : this->m_AnimControler) {
				a.UpdateAnim(this);
			}
		}
		/*物理演算*/
		auto			PhysicsResetState(void) const noexcept { return MV1PhysicsResetState(GetHandle()) == TRUE; }
		auto			PhysicsCalculation(float p1) const noexcept { return MV1PhysicsCalculation(GetHandle(), p1) == TRUE; }
		/*当たり判定*/
		auto			SetupCollInfo(int x = 32, int y = 8, int z = 32, int frame = INVALID_ID, int mesh = INVALID_ID) const noexcept {
			return MV1SetupCollInfo(GetHandle(), frame, x, y, z, mesh) == 0;
		}
		auto			RefreshCollInfo(int frame = INVALID_ID, int mesh = INVALID_ID) const noexcept {
			return MV1RefreshCollInfo(GetHandle(), frame, mesh) == 0;
		}
		auto			CollCheck_Line(const Vector3DX& start, const Vector3DX& end, int frame = INVALID_ID, int mesh = INVALID_ID) const noexcept {
			return MV1CollCheck_Line(GetHandle(), frame, start.get(), end.get(), mesh);
		}
		auto			CollCheck_Sphere(const Vector3DX& startpos, float range, int frame = INVALID_ID, int mesh = INVALID_ID) const noexcept {
			return MV1CollCheck_Sphere(GetHandle(), frame, startpos.get(), range, mesh);
		}
		auto			CollCheck_Capsule(const Vector3DX& startpos, const Vector3DX& endpos, float range, int frame = INVALID_ID, int mesh = INVALID_ID) const noexcept {
			return MV1CollCheck_Capsule(GetHandle(), frame, startpos.get(), endpos.get(), range, mesh);
		}
	};
};
