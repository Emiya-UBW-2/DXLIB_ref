/*=============================================================================
  Copyright (C) 2020 yumetodo <yume-wikijp@live.jp>
  Distributed under the Boost Software License, Version 1.0.
  (See https://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#pragma once
#include "DXLib_ref.h"

namespace DXLib_ref {
	class MV1 {
	public:
		class ani {
		public:
			int handle = 0;

			float per_prev = -1.f;
			float time_prev = -1.f;

			float per = 0.f;
			float time = 0.f;
			float alltime = 0.f;

			void GoStart(void) noexcept { this->time = 0.f; }
			void GoEnd(void) noexcept { this->time = this->alltime; }
			const auto GetTimePer(void) const noexcept { return (this->alltime > 0) ? this->time / this->alltime : 1.f; }

			const auto TimeStart(void) const noexcept { return time <= 0; }
			const auto TimeEnd(void) const noexcept { return time >= alltime; }

			void Reset(void) noexcept {
				this->per = 0.f;
				this->time = 0.f;
			}

			void set(MV1* model, int AnimIndex, const MV1* model_haveanim = nullptr) {
				if (model_haveanim && (model->get() != model_haveanim->get())) {
					this->handle = MV1AttachAnim(model->get(), AnimIndex, model_haveanim->get());
				}
				else {
					this->handle = MV1AttachAnim(model->get(), AnimIndex);
				}
				this->Reset();
				if (this->per_prev != this->per) {
					MV1SetAttachAnimBlendRate(model->get(), this->handle, this->per);
					this->per_prev = this->per;
				}
				this->alltime = MV1GetAttachAnimTotalTime(model->get(), this->handle);
			}

			void Update(const bool& loop, float speed) {
				this->time += 30.f / GetFPS()*speed;
				if (loop) {
					if (speed >= 0.f) {
						if (this->time >= this->alltime) {
							this->time = 0.f;
						}
					}
					else {
						if (this->time <= 0.f) {
							this->time = this->alltime;
						}
					}
				}
			}

			//void update_(void) noexcept {
				//MV1SetAttachAnimTime(this->handle_, a.handle, a.time);
				//MV1SetAttachAnimBlendRate(this->handle_, a.handle, a.per);
			//}
		};
	private:
		int handle_;
		std::vector<ani> anime;
		MV1(int h) noexcept : handle_(h) {}
		static constexpr int invalid_handle = -1;
	public:
		/*コンストラクタ*/
		MV1(void) noexcept { handle_ = invalid_handle; }
		MV1(const MV1&) = delete;
		MV1(MV1&& o) noexcept : handle_(o.handle_) { o.handle_ = invalid_handle; }
		/*オペレーター*/
		MV1& operator=(const MV1&) = delete;
		MV1& operator=(MV1&& o) noexcept {
			this->handle_ = o.handle_;
			o.handle_ = invalid_handle;
			return *this;
		}
		/*デストラクタ*/
		~MV1(void) noexcept {
			Dispose();
		}
		//
		bool IsActive(void) const noexcept { return this->handle_ != invalid_handle; }
		int get(void) const noexcept { return this->handle_; }
		/*モデル*/
		bool SetPosition(const Vector3DX& p1) const noexcept { return MV1SetPosition(this->handle_, p1.get()) == TRUE; }
		Vector3DX GetPosition(void) const noexcept { return MV1GetPosition(this->handle_); }
		bool SetRotationZYAxis(const Vector3DX& zaxis, const Vector3DX& yaxis, float zrad) const noexcept { return MV1SetRotationZYAxis(this->handle_, zaxis.get(), yaxis.get(), zrad) == TRUE; }
		bool SetMatrix(const Matrix4x4DX& mat) const noexcept { return MV1SetMatrix(this->handle_, mat.get()) == TRUE; }
		bool DrawModel(void) const noexcept { return MV1DrawModel(this->handle_) == TRUE; }
		bool SetOpacityRate(float p1) const noexcept { return MV1SetOpacityRate(this->handle_, p1) == TRUE; }
		float GetOpacityRate(void) const noexcept { return MV1GetOpacityRate(this->handle_); }
		bool SetScale(const Vector3DX& p1) const noexcept { return MV1SetScale(this->handle_, p1.get()) == TRUE; }
		/*テクスチャ*/
		bool SetTextureGraphHandle(int p1, const GraphHandle& p2, bool trans) const noexcept { return MV1SetTextureGraphHandle(this->handle_, p1, p2.get(), trans ? TRUE : FALSE) == TRUE; }
		/*フレーム*/
		Vector3DX frame(int p1) const noexcept { return MV1GetFramePosition(this->handle_, p1); }
		size_t frame_num(void) const noexcept {
			if (IsActive()) {
				return MV1GetFrameNum(this->handle_);
			}
			else {
				return 0;
			}
		}
		int frame_parent(int p1) const noexcept { return MV1GetFrameParent(this->handle_, p1); }
		size_t frame_child(int p1, int p2) const noexcept { return MV1GetFrameChild(this->handle_, p1, p2); }
		size_t frame_child_num(int p1) const noexcept { return MV1GetFrameChildNum(this->handle_, p1); }
		bool SetFrameLocalMatrix(int id, Matrix4x4DX mat) const noexcept { return MV1SetFrameUserLocalMatrix(this->handle_, id, mat.get()) == TRUE; }

		Matrix4x4DX GetFrameLocalMatrix(int id) const noexcept { return MV1GetFrameLocalMatrix(this->handle_, id); }
		Matrix4x4DX GetFrameLocalWorldMatrix(int id) const noexcept { return MV1GetFrameLocalWorldMatrix(this->handle_, id); }
		Matrix4x4DX GetMatrix(void) const noexcept { return MV1GetMatrix(this->handle_); }

		const bool DrawFrame(int p1) const noexcept { return MV1DrawFrame(this->handle_, p1) == TRUE; }
		const std::string frame_name(const size_t& p1) const noexcept { return MV1GetFrameName(this->handle_, int(p1)); }

		void frame_Reset(int p1) const noexcept { MV1ResetFrameUserLocalMatrix(this->handle_, p1); }
		/*マテリアル*/
		size_t material_num(void) const noexcept { return MV1GetMaterialNum(this->handle_); }
		void material_AlphaTestAll(bool Enable, int mode, int param) const noexcept {
			MV1SetMaterialDrawAlphaTestAll(this->handle_, Enable ? TRUE : FALSE, mode, param);
		}
		std::string material_name(int p1) const noexcept { return MV1GetMaterialName(this->handle_, p1); }
		/*メッシュ*/
		size_t mesh_num(void) const noexcept {
			if (IsActive()) {
				return MV1GetMeshNum(this->handle_);
			}
			else {
				return 0;
			}
		}
		Vector3DX mesh_maxpos(int p1) const noexcept { return MV1GetMeshMaxPosition(this->handle_, p1); }
		Vector3DX mesh_minpos(int p1) const noexcept { return MV1GetMeshMinPosition(this->handle_, p1); }
		bool DrawMesh(int p1) const noexcept { return MV1DrawMesh(this->handle_, p1) == TRUE; }
		/*シェイプ*/
		int SearchShape(const char* str) const noexcept { return MV1SearchShape(this->handle_, str); }
		bool SetShapeRate(int p1, float p2) const noexcept { return MV1SetShapeRate(this->handle_, p1, p2) == TRUE; }
		/*アニメーション*/
		bool work_anime(void) noexcept {
			for (auto& a : this->anime) {
				if (a.time_prev != a.time) {
					MV1SetAttachAnimTime(this->handle_, a.handle, a.time);
					a.time_prev = a.time;
				}
				if (a.per_prev != a.per) {
					MV1SetAttachAnimBlendRate(this->handle_, a.handle, a.per);
					a.per_prev = a.per;
				}
			}
			return true;
		}
		auto& get_anime(const size_t& p1) noexcept { return this->anime[std::clamp<size_t>(p1, 0, this->anime.size() - 1)]; }
		auto& getanime(const size_t& p1) const noexcept { return this->anime[std::clamp<size_t>(p1, 0, this->anime.size() - 1)]; }
		const auto& get_anime(void) const noexcept { return this->anime; }

		bool work_anime(const size_t& p1) noexcept {
			auto& a = get_anime(p1);
			if (a.time_prev != a.time) {
				MV1SetAttachAnimTime(this->handle_, a.handle, a.time);
				a.time_prev = a.time;
			}
			if (a.per_prev != a.per) {
				MV1SetAttachAnimBlendRate(this->handle_, a.handle, a.per);
				a.per_prev = a.per;
			}
			return true;
		}
		/*物理演算*/
		bool PhysicsResetState(void) const noexcept { return MV1PhysicsResetState(this->handle_) == TRUE; }
		bool PhysicsCalculation(float p1) const noexcept { return MV1PhysicsCalculation(this->handle_, p1) == TRUE; }
		/*当たり判定*/
		bool SetupCollInfo(int x = 32, int y = 8, int z = 32, int frame = -1, int mesh = -1) const noexcept {
			return MV1SetupCollInfo(this->handle_, frame, x, y, z, mesh) == 0;
		}
		bool RefreshCollInfo(int frame = -1, int mesh = -1) const noexcept {
			return MV1RefreshCollInfo(this->handle_, frame, mesh) == 0;
		}
		const auto CollCheck_Line(const Vector3DX& start, const Vector3DX& end, int frame = -1, int mesh = -1) const noexcept {
			return MV1CollCheck_Line(this->handle_, frame, start.get(), end.get(), mesh);
		}
		const auto CollCheck_Sphere(const Vector3DX& startpos, float range, int frame = -1, int mesh = -1) const noexcept {
			return MV1CollCheck_Sphere(this->handle_, frame, startpos.get(), range, mesh);
		}

		const auto CollCheck_Capsule(const Vector3DX& startpos, const Vector3DX& endpos, float range, int frame = -1, int mesh = -1) const noexcept {
			return MV1CollCheck_Capsule(this->handle_, frame, startpos.get(), endpos.get(), range, mesh);
		}

		/*読み込み*/
		static void Load(std::basic_string_view<TCHAR> FileName, MV1* _ModelHandle, int mode = DX_LOADMODEL_PHYSICS_LOADCALC, float Grav = M_GR) noexcept {
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
			_ModelHandle->anime.clear();
			return;
		}
		static void SetAnime(MV1* _ModelHandle, const MV1& _Have_Anim)/* noexcept*/ {
			//元用、アニメ用が読めていない場合は待つ
			{
				while (ProcessMessage() == 0) {
					if ((CheckHandleASyncLoad(_ModelHandle->get()) == TRUE) && (CheckHandleASyncLoad(_Have_Anim.get()) == TRUE)) { continue; }
					break;
				}
			}
			//エラー
			if (MV1GetAnimNum(_Have_Anim.get()) < 0) {
				printfDx("error");
				WaitKey();
			}
			_ModelHandle->anime.resize(MV1GetAnimNum(_Have_Anim.get()));
			if (_ModelHandle->anime.size() > 0) {
				for (int i = 0; i < int(_ModelHandle->anime.size()); i++) {
					_ModelHandle->anime[i].set(_ModelHandle, i, &_Have_Anim);
				}
			}
			return;
		}
		static void LoadonAnime() noexcept {
			//これに切り替え
			//MV1::Load(model[Max].Path, &(model[Max].obj));
			//model[Max].obj.SetAnime(&(model[Max].obj), model[Max].obj);
			return;
		}

		MV1 Duplicate(void) const noexcept { return DxLib::MV1DuplicateModel(this->handle_); }

		void DuplicateonAnime() const noexcept {
			//これに切り替え
			//model[Max].obj = model[i].obj.Duplicate();
			//model[Max].obj.SetAnime(&(model[Max].obj), model[Max].obj);
			return;
		}

		/*削除*/
		void Dispose(void) noexcept {
			if (this->handle_ != invalid_handle) {
				MV1DeleteModel(this->handle_);
				anime.clear();
				this->handle_ = invalid_handle;
			}
		}
	};
};
