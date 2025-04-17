#include "DXLib_ref_103_ObjectBase.hpp"

namespace DXLibRef {
	void			ResourceModel::LoadModel(PHYSICS_SETUP TYPE, const char* filepath, const char* objfilename, const char* colfilename) noexcept {
		this->m_PHYSICS_SETUP = TYPE;
		this->m_FilePath = filepath;
		this->m_ObjFileName = objfilename;
		this->m_ColFileName = colfilename;
		auto Load = [](MV1* obj, std::string Path, std::string NameAdd, int PHYSICS_TYPE) {
			if (IsFileExist((Path + NameAdd + ".mv1").c_str())) {
				// MV1::Load(Path + ".pmx", obj, PHYSICS_TYPE);
				MV1::Load((Path + NameAdd + ".mv1").c_str(), obj, PHYSICS_TYPE);
			}
			else if (IsFileExist((Path + ".pmx").c_str())) {
				MV1::Load(Path + ".pmx", obj, PHYSICS_TYPE);
			}
			};
		// model
		switch (this->m_PHYSICS_SETUP) {
		case PHYSICS_SETUP::DISABLE:
			Load(&this->m_obj, this->m_FilePath + this->m_ObjFileName, "_DISABLE", DX_LOADMODEL_PHYSICS_DISABLE);
			break;
		case PHYSICS_SETUP::LOADCALC:
			Load(&this->m_obj, this->m_FilePath + this->m_ObjFileName, "_LOADCALC", DX_LOADMODEL_PHYSICS_LOADCALC);
			break;
		case PHYSICS_SETUP::REALTIME:
			Load(&this->m_obj, this->m_FilePath + this->m_ObjFileName, "_REALTIME", DX_LOADMODEL_PHYSICS_REALTIME);
			break;
		default:
			break;
		}
		// col
		Load(&this->m_col, this->m_FilePath + this->m_ColFileName, "", DX_LOADMODEL_PHYSICS_DISABLE);
	}
	void			ResourceModel::LoadModelData(const std::shared_ptr<BaseObject>& pBase) noexcept {
		if (this->m_IsEndLoadData) { return; }
		// フレーム
		{
			this->m_Frames.clear();
			if (pBase->GetFrameNum() > 0) {
				this->m_Frames.resize(static_cast<size_t>(pBase->GetFrameNum()));
			}
			for (auto& f : this->m_Frames) {
				f.first = InvalidID;
			}
			if (this->m_Frames.size() > 0) {
				size_t count = 0;
				for (int frameNum = 0, Max = this->m_obj.GetFrameNum(); frameNum < Max; ++frameNum) {
					if (this->m_obj.GetFrameName(frameNum) == pBase->GetFrameStr(static_cast<int>(count))) {
						// そのフレームを登録
						this->m_Frames[count].first = frameNum;
						this->m_Frames[count].second = Matrix4x4DX::Mtrans(this->m_obj.GetFrameLocalMatrix(this->m_Frames[count].first).pos());
					}
					else if (frameNum < Max - 1) {
						continue;// 飛ばす
					}
					++count;
					frameNum = 0;
					if (count >= this->m_Frames.size()) {
						break;
					}
				}
			}
		}
		// フレーム
		{
			this->m_Materials.clear();
			if (pBase->GetMaterialNum() > 0) {
				this->m_Materials.resize(static_cast<size_t>(pBase->GetMaterialNum()));
			}
			for (auto& f : this->m_Materials) {
				f = InvalidID;
			}
			if (this->m_Materials.size() > 0) {
				size_t count = 0;
				for (int frameNum = 0, Max = this->m_obj.GetMaterialNum(); frameNum < Max; ++frameNum) {
					if (this->m_obj.GetMaterialName(frameNum) == pBase->GetMaterialStr(static_cast<int>(count))) {
						// そのフレームを登録
						this->m_Materials[count] = frameNum;
					}
					else if (frameNum < Max - 1) {
						continue;// 飛ばす
					}
					++count;
					frameNum = 0;
					if (count >= this->m_Materials.size()) {
						break;
					}
				}
			}
		}
		// シェイプ
		{
			this->m_Shapes.clear();
			if (pBase->GetShapeNum() > 0) {
				this->m_Shapes.resize(static_cast<size_t>(pBase->GetShapeNum()));
			}
			for (size_t loop : std::views::iota(static_cast<size_t>(0), this->m_Shapes.size())) {
				auto s = this->m_obj.SearchShape(pBase->GetShapeStr(static_cast<int>(loop)));
				if (s >= 0) {
					this->m_Shapes[loop].first = s;
					this->m_Shapes[loop].second = 0.f;
				}
				else {
					this->m_Shapes[loop].first = InvalidID;
					this->m_Shapes[loop].second = 0.f;
				}
			}
		}
		this->m_IsEndLoadData = true;
	}

	void			ResourceModel::SaveModel(bool UseToonWhenCreateFile) noexcept {
		auto Save = [&](MV1* obj, std::string NameAdd, int PHYSICS_TYPE) {
			if (!IsFileExist((this->m_FilePath + this->m_ObjFileName + NameAdd + ".mv1").c_str()) && IsFileExist((this->m_FilePath + this->m_ObjFileName + ".pmx").c_str())) {
				MV1SetLoadModelUsePhysicsMode(PHYSICS_TYPE);
				if (!UseToonWhenCreateFile) {
					obj->SetMaterialTypeAll(DX_MATERIAL_TYPE_NORMAL);
					if (obj->GetMaterialNum() > 0) {
						for (int loop : std::views::iota(0, obj->GetMaterialNum())) {
							/*
							// テクスチャ追加前のテクスチャ数を取得しておく
							int TexIndex = MV1GetTextureNum(obj->get()());
							// モデルで使用するテクスチャを追加する
							MV1AddTexture(obj->get()(), "NrmTex", (this->m_FilePath + "NormalMap.png").c_str());
							// 指定のマテリアル( ここでは例として3番のマテリアル )で使用する法線マップを設定する
							MV1SetMaterialNormalMapTexture(obj->get()(), loop, TexIndex);
							//*/

							obj->SetMaterialDifColor(loop, GetColorF(1.f, 1.f, 1.f, 1.f));
							obj->SetMaterialSpcColor(loop, GetColorF(0.f, 0.f, 0.f, 0.f));
							obj->SetMaterialAmbColor(loop, GetColorF(0.25f, 0.25f, 0.25f, 1.f));
							obj->SetMaterialSpcPower(loop, 0.1f);
						}
					}
				}
				obj->SaveModelToMV1File(this->m_FilePath + this->m_ObjFileName + NameAdd + ".mv1");
				MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
			}
			};
		// model
		switch (this->m_PHYSICS_SETUP) {
		case PHYSICS_SETUP::DISABLE:
			Save(&this->m_obj, "_DISABLE", DX_LOADMODEL_PHYSICS_DISABLE);
			break;
		case PHYSICS_SETUP::LOADCALC:
			Save(&this->m_obj, "_LOADCALC", DX_LOADMODEL_PHYSICS_LOADCALC);
			break;
		case PHYSICS_SETUP::REALTIME:
			Save(&this->m_obj, "_REALTIME", DX_LOADMODEL_PHYSICS_REALTIME);
			break;
		default:
			break;
		}
		// col
		Save(&this->m_col, "", DX_LOADMODEL_PHYSICS_DISABLE);
	}
	void			ResourceModel::DisposeModel(void) noexcept {
		this->m_obj.Dispose();
		this->m_col.Dispose();
	}
	void			ResourceModel::CopyModel(const std::shared_ptr<ResourceModel>& pBase) noexcept {
		this->m_PHYSICS_SETUP = pBase->m_PHYSICS_SETUP;
		this->m_FilePath = pBase->m_FilePath;
		this->m_ObjFileName = pBase->m_ObjFileName;
		this->m_ColFileName = pBase->m_ColFileName;
		// model
		this->m_obj.Duplicate(pBase->m_obj);
		// col
		if (pBase->m_col.IsActive()) {
			this->m_col.Duplicate(pBase->m_col);
		}
		// フレーム
		this->m_Frames.resize(pBase->m_Frames.size());
		for (size_t index = 0; auto & f : this->m_Frames) {
			f.first = pBase->m_Frames.at(index).first;
			if (f.first != InvalidID) {
				f.second = pBase->m_Frames.at(index).second;
			}
			++index;
		}
		// フレーム
		this->m_Materials.resize(pBase->m_Materials.size());
		for (size_t index = 0; auto & f : this->m_Materials) {
			f = pBase->m_Materials.at(index);
			++index;
		}
		// シェイプ
		this->m_Shapes.resize(pBase->m_Shapes.size());
		for (size_t index = 0; auto & f : this->m_Shapes) {
			f.first = pBase->m_Shapes.at(index).first;
			if (f.first != InvalidID) {
				f.second = pBase->m_Shapes.at(index).second;
			}
			++index;
		}
	}
	// 
	void			BaseObject::SetAnimOnce(int ID, float speed) noexcept {
		this->SetObj().SetAnim(ID).Update(false, speed);
	}
	void			BaseObject::SetAnimLoop(int ID, float speed) noexcept {
		this->SetObj().SetAnim(ID).Update(true, speed);
	}
	// 
	void			BaseObject::Init(void) noexcept {
		SetActive(true);
		this->m_IsResetPhysics = true;
		this->m_IsFirstLoop = true;
		Init_Sub();
	}
	// 
	void			BaseObject::ExecuteCommon(void) noexcept {
		auto* DXLib_refParts = DXLib_ref::Instance();
		if (this->m_IsFirstLoop) {
			this->m_PrevMat = GetObj().GetMatrix();
		}
		// シェイプ更新
		for (size_t index = 0; const auto & f : GetShapesList()) {
			if (index == 0) {
				continue;
			}
			GetObj().SetShapeRate(f.first, (1.f - GetShapesList().at(0).second) * f.second);
			++index;
		}
		// 物理更新
		if (GetPhysicsSetup() == PHYSICS_SETUP::REALTIME) {
			if (this->m_IsResetPhysics) {
				this->m_IsResetPhysics = false;
				GetObj().PhysicsResetState();
			}
			else {
				auto NowMat = GetObj().GetMatrix();
				int Max = 2;
				if (DXLib_refParts->GetFps() > FrameRate * Max) {
					Max = 1;
				}
				for (int loop : std::views::iota(0, Max)) {
					GetObj().SetMatrix(Lerp(this->m_PrevMat, NowMat, static_cast<float>(loop + 1) / static_cast<float>(Max)));
					GetObj().PhysicsCalculation(1000.0f * FrameRate * DXLib_refParts->GetDeltaTime() / static_cast<float>(Max));
				}
			}
			this->m_PrevMat = GetObj().GetMatrix();
		}
		// 最初のループ終わり
		this->m_IsFirstLoop = false;
		for (auto& d : this->m_IsDraw) {
			d = false;
		}
	}

	void			BaseObject::DrawShadow(void) noexcept {
		if (!IsActive()) { return; }
		if (!GetObj().IsActive()) { return; }
		GetObj().DrawModel();
	}
	void			BaseObject::CheckDraw(int Range) noexcept {
		if (!IsActive()) { return; }
		if (Range == InvalidID) { return; }
		if (CheckCameraViewClip_Box(
			(GetObj().GetMatrix().pos() + this->m_MinAABB).get(),
			(GetObj().GetMatrix().pos() + this->m_MaxAABB).get()) == FALSE
			) {
			this->m_IsDraw.at(Range) |= true;
		}
		CheckDraw_Sub(Range);
	}
	void			BaseObject::Draw(bool isDrawSemiTrans, int Range) noexcept {
		if (!IsActive()) { return; }
		if (!IsDraw(Range)) { return; }
		if (!GetObj().IsActive()) { return; }
		for (int loop : std::views::iota(0, static_cast<int>(GetObj().GetMeshNum()))) {
			if (GetObj().GetMeshSemiTransState(loop) == isDrawSemiTrans) {
				GetObj().DrawMesh(loop);
			}
		}
	}
	// 
	void			BaseObject::Dispose(void) noexcept {
		DisposeModel();
		Dispose_Sub();
	}
};
