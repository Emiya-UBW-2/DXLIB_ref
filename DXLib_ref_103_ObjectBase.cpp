#include "DXLib_ref_103_ObjectBase.hpp"

namespace DXLibRef {
	void			ModelBaseClass::LoadModel(
		const std::shared_ptr<ObjectBaseClass>& pBase,
		PHYSICS_SETUP TYPE, const char* filepath, const char* objfilename, const char* colfilename) noexcept {
		this->m_PHYSICS_SETUP = TYPE;
		this->m_FilePath = filepath;
		this->m_ObjFileName = objfilename;
		this->m_ColFileName = colfilename;
		auto Load = [&](MV1* obj, std::string Path, std::string NameAdd, int PHYSICS_TYPE) {
			if (IsFileExist((Path + NameAdd + ".mv1").c_str())) {
				//MV1::Load(Path + ".pmx", obj, PHYSICS_TYPE);
				MV1::Load((Path + NameAdd + ".mv1").c_str(), obj, PHYSICS_TYPE);
			}
			else if (IsFileExist((Path + ".pmx").c_str())) {
				MV1::Load(Path + ".pmx", obj, PHYSICS_TYPE);
			}
			};
		//model
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
		//col
		Load(&this->m_col, this->m_FilePath + this->m_ColFileName, "", DX_LOADMODEL_PHYSICS_DISABLE);
		//フレーム
		{
			this->m_Frames.clear();
			if (pBase->GetFrameNum() > 0) {
				this->m_Frames.resize(static_cast<std::size_t>(pBase->GetFrameNum()));
			}
			for (auto& f : this->m_Frames) {
				f.first = INVALID_ID;
			}
			if (this->m_Frames.size() > 0) {
				int count = 0;
				int Max = this->m_obj.GetFrameNum();
				for (int frameNum = 0; frameNum < Max; frameNum++) {
					if (this->m_obj.GetFrameName(frameNum) == pBase->GetFrameStr(count)) {
						//そのフレームを登録
						this->m_Frames[static_cast<size_t>(count)].first = frameNum;
						this->m_Frames[static_cast<size_t>(count)].second = Matrix4x4DX::Mtrans(this->m_obj.GetFrameLocalMatrix(this->m_Frames[static_cast<size_t>(count)].first).pos());
					}
					else if (frameNum < Max - 1) {
						continue;//飛ばす
					}
					count++;
					frameNum = 0;
					if (count >= static_cast<int>(this->m_Frames.size())) {
						break;
					}
				}
			}
		}
		//シェイプ
		{
			this->m_Shapes.clear();
			if (pBase->GetShapeNum() > 0) {
				this->m_Shapes.resize(static_cast<size_t>(pBase->GetShapeNum()));
			}
			for (int j = 0; j < static_cast<int>(this->m_Shapes.size()); j++) {
				auto s = this->m_obj.SearchShape(pBase->GetShapeStr(j));
				if (s >= 0) {
					this->m_Shapes[static_cast<size_t>(j)].first = s;
					this->m_Shapes[static_cast<size_t>(j)].second = 0.f;
				}
				else {
					this->m_Shapes[static_cast<size_t>(j)].first = INVALID_ID;
					this->m_Shapes[static_cast<size_t>(j)].second = 0.f;
				}
			}
		}
	}
	void			ModelBaseClass::SaveModel(bool UseToonWhenCreateFile) noexcept {
		auto Save = [&](MV1* obj, std::string Path, std::string NameAdd, int PHYSICS_TYPE) {
			if (!IsFileExist((Path + NameAdd + ".mv1").c_str()) && IsFileExist((Path + ".pmx").c_str())) {
				MV1SetLoadModelUsePhysicsMode(PHYSICS_TYPE);
				if (!UseToonWhenCreateFile) {
					obj->SetMaterialTypeAll(DX_MATERIAL_TYPE_NORMAL);
					for (int i = 0, Max = obj->GetMaterialNum(); i < Max; i++) {
						obj->SetMaterialDifColor(i, GetColorF(1.f, 1.f, 1.f, 1.f));
						obj->SetMaterialSpcColor(i, GetColorF(0.f, 0.f, 0.f, 0.f));
						obj->SetMaterialAmbColor(i, GetColorF(0.25f, 0.25f, 0.25f, 1.f));
						obj->SetMaterialSpcPower(i, 0.1f);
					}
				}
				obj->SaveModelToMV1File(Path + NameAdd + ".mv1");
				MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
			}
			};
		//model
		switch (this->m_PHYSICS_SETUP) {
		case PHYSICS_SETUP::DISABLE:
			Save(&this->m_obj, this->m_FilePath + this->m_ObjFileName, "_DISABLE", DX_LOADMODEL_PHYSICS_DISABLE);
			break;
		case PHYSICS_SETUP::LOADCALC:
			Save(&this->m_obj, this->m_FilePath + this->m_ObjFileName, "_LOADCALC", DX_LOADMODEL_PHYSICS_LOADCALC);
			break;
		case PHYSICS_SETUP::REALTIME:
			Save(&this->m_obj, this->m_FilePath + this->m_ObjFileName, "_REALTIME", DX_LOADMODEL_PHYSICS_REALTIME);
			break;
		default:
			break;
		}
		//col
		Save(&this->m_col, this->m_FilePath + this->m_ColFileName, "", DX_LOADMODEL_PHYSICS_DISABLE);
	}
	void			ModelBaseClass::DisposeModel(void) noexcept {
		this->m_obj.Dispose();
		this->m_col.Dispose();
	}
	void			ModelBaseClass::CopyModel(const std::shared_ptr<ModelBaseClass>& pBase) noexcept {
		this->m_PHYSICS_SETUP = pBase->m_PHYSICS_SETUP;
		this->m_FilePath = pBase->m_FilePath;
		this->m_ObjFileName = pBase->m_ObjFileName;
		this->m_ColFileName = pBase->m_ColFileName;
		//model
		this->m_obj = pBase->m_obj.Duplicate();
		//col
		if (pBase->m_col.IsActive()) {
			this->m_col = pBase->m_col.Duplicate();
		}
		//フレーム
		this->m_Frames.resize(pBase->m_Frames.size());
		for (auto& f : this->m_Frames) {
			auto index = static_cast<std::size_t>(&f - &this->m_Frames.front());
			f.first = pBase->m_Frames.at(index).first;
			if (f.first != INVALID_ID) {
				f.second = pBase->m_Frames.at(index).second;
			}
		}
		//シェイプ
		this->m_Shapes.resize(pBase->m_Shapes.size());
		for (auto& f : this->m_Shapes) {
			auto index = static_cast<std::size_t>(&f - &this->m_Shapes.front());
			f.first = pBase->m_Shapes.at(index).first;
			if (f.first != INVALID_ID) {
				f.second = pBase->m_Shapes.at(index).second;
			}
		}
	}
	//
	void			ObjectBaseClass::SetAnimOnce(int ID, float speed) noexcept {
		this->GetObj().SetAnim(ID).Update(false, speed);
	}
	void			ObjectBaseClass::SetAnimLoop(int ID, float speed) noexcept {
		this->GetObj().SetAnim(ID).Update(true, speed);
	}
	//
	void			ObjectBaseClass::Init(void) noexcept {
		this->m_IsActive = true;
		this->m_IsResetPhysics = true;
		this->m_IsFirstLoop = true;
		this->m_IsDraw = false;
		Init_Sub();
	}
	//
	void			ObjectBaseClass::ExecuteCommon(void) noexcept {
		auto* DrawParts = DXDraw::Instance();
		if (this->m_IsFirstLoop) {
			this->m_PrevMat = this->GetObj().GetMatrix();
		}
		//シェイプ更新
		for (auto& f : this->m_Shapes) {
			if ((&f - &this->m_Shapes.front()) == 0) { continue; }
			this->GetObj().SetShapeRate(f.first, (1.f - this->m_Shapes[0].second) * f.second);
		}
		//物理更新
		if (this->m_PHYSICS_SETUP == PHYSICS_SETUP::REALTIME) {
			if (this->m_IsResetPhysics) {
				this->m_IsResetPhysics = false;
				this->GetObj().PhysicsResetState();
			}
			else {
				auto NowMat = this->GetObj().GetMatrix();
				int Max = 2;
				if (DrawParts->GetFps() > 120.f) {
					Max = 1;
				}
				for (int i = 0; i < Max; i++) {
					this->GetObj().SetMatrix(Lerp(this->m_PrevMat, NowMat, static_cast<float>(i + 1) / static_cast<float>(Max)));
					this->GetObj().PhysicsCalculation(1000.0f *60.f / DrawParts->GetFps() / static_cast<float>(Max));
				}
			}
			this->m_PrevMat = this->GetObj().GetMatrix();
		}
		//最初のループ終わり
		this->m_IsFirstLoop = false;
	}

	void			ObjectBaseClass::DrawShadow(void) noexcept {
		if (this->m_IsActive && this->m_IsDraw) {
			this->GetObj().DrawModel();
		}
	}
	void			ObjectBaseClass::CheckDraw(void) noexcept {
		this->m_IsDraw = false;
		this->m_DistanceToCam = (this->GetObj().GetMatrix().pos() - GetScreenPosition()).magnitude();
		if (CheckCameraViewClip_Box(
			(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-1.f*12.5f, -0.f*12.5f, -1.f*12.5f)).get(),
			(this->GetObj().GetMatrix().pos() + Vector3DX::vget(1.f*12.5f, 1.f*12.5f, 1.f*12.5f)).get()) == FALSE
			) {
			this->m_IsDraw |= true;
		}

		Vector3DX campos = ConvWorldPosToScreenPos(this->GetMove().GetPos().get());
		if (0.f < campos.z && campos.z < 1.f) {
			this->SetScreenPosition(campos, std::max(20.f / ((this->GetMove().GetPos() - GetCameraPosition()).magnitude() / 2.f), 0.2f));
		}
	}
	void			ObjectBaseClass::Draw(bool isDrawSemiTrans) noexcept {
		if (this->m_IsActive && this->m_IsDraw) {
			if (CheckCameraViewClip_Box(
				(this->GetObj().GetMatrix().pos() + Vector3DX::vget(-1.f*12.5f, -0.f*12.5f, -1.f*12.5f)).get(),
				(this->GetObj().GetMatrix().pos() + Vector3DX::vget(1.f*12.5f, 1.f*12.5f, 1.f*12.5f)).get()) == FALSE
				) {
				for (int i = 0; i < static_cast<int>(this->GetObj().GetMeshNum()); i++) {
					if (this->GetObj().GetMeshSemiTransState(i) == isDrawSemiTrans) {
						this->GetObj().DrawMesh(i);
					}
				}
			}
		}
	}
	//
	void			ObjectBaseClass::Dispose(void) noexcept {
		DisposeModel();
		Dispose_Sub();
	}
};
