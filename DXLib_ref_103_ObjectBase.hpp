#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	enum class PHYSICS_SETUP {
		DISABLE,
		LOADCALC,
		REALTIME,
	};
	class ObjectBaseClass;

	class ModelBaseClass {
	protected:
		PHYSICS_SETUP								m_PHYSICS_SETUP{ PHYSICS_SETUP::DISABLE };
		MV1											m_obj;
		MV1											m_col;
		std::vector<std::pair<int, Matrix4x4DX>>	m_Frames;
		std::vector<int>							m_Materials;
		std::vector<std::pair<int, float>>			m_Shapes;
		std::string									m_FilePath;
		std::string									m_ObjFileName;
		std::string									m_ColFileName;
	public:
		ModelBaseClass(void) noexcept {}
		ModelBaseClass(const ModelBaseClass&) = delete;
		ModelBaseClass(ModelBaseClass&& o) = delete;
		ModelBaseClass& operator=(const ModelBaseClass&) = delete;
		ModelBaseClass& operator=(ModelBaseClass&& o) = delete;

		virtual ~ModelBaseClass(void) noexcept {}
	public:
		auto		GetPathCompare(const char* filepath, const char* objfilename, const char* colfilename) const noexcept {
			return ((this->m_FilePath == filepath) && (this->m_ObjFileName == objfilename) && (this->m_ColFileName == colfilename));
		}
	public:
		void			SetShapePer(int pShape, float Per) noexcept { this->m_Shapes[static_cast<size_t>(pShape)].second = Per; }
	public:
		void			LoadModel(
			const std::shared_ptr<ObjectBaseClass>& pBase,
			PHYSICS_SETUP TYPE, const char* filepath, const char* objfilename = "model", const char* colfilename = "col") noexcept;
		void			SaveModel(bool UseToonWhenCreateFile) noexcept;
		void			CopyModel(const std::shared_ptr<ModelBaseClass>& pBase) noexcept;
	public:
		void			DisposeModel(void) noexcept;
	};

	class ObjectBaseClass : public ModelBaseClass {
	protected:
		int											m_ObjectID{ INVALID_ID };
		bool										m_ColActive{ false };
		moves										m_move;
		Matrix4x4DX									m_PrevMat;//物理更新のため
		int											m_objType{ 0 };
		bool										m_IsActive{ true };
		bool										m_IsDelete{ false };
		bool										m_IsResetPhysics{ true };
		bool										m_IsFirstLoop{ true };
		bool										m_IsDraw{ true };
		float										m_DistanceToCam{ 0.f };
		Vector3DX									m_ScreenPosition;
		float										m_CameraSize{ 0.f };
		ShaderUseClass* m_UseShader{ nullptr };
	public:
		auto& GetObj(void) noexcept { return this->m_obj; }
		const auto& GetObj_const(void) const noexcept { return this->m_obj; }
		//
		bool		HaveFrame(int frame) const noexcept { return this->m_Frames[static_cast<size_t>(frame)].first != INVALID_ID; }
		const auto& GetFrame(int frame) const noexcept { return this->m_Frames[static_cast<size_t>(frame)].first; }
		const auto& GetFrameBaseLocalMat(int frame) const noexcept { return this->m_Frames[static_cast<size_t>(frame)].second; }

		bool		HaveMaterial(int frame) const noexcept { return this->m_Materials[static_cast<size_t>(frame)] != INVALID_ID; }
		const auto& GetMaterial(int frame) const noexcept { return this->m_Materials[static_cast<size_t>(frame)]; }
		

		const auto& GetFilePath(void) const noexcept { return this->m_FilePath; }
		auto& SetMove(void) noexcept { return this->m_move; }
		//
		void			SetAnimOnce(int ID, float speed) noexcept;
		void			SetAnimLoop(int ID, float speed) noexcept;
	public:
		const auto& GetObjectID(void) const noexcept { return this->m_ObjectID; }
		const auto& GetobjType(void) const noexcept { return this->m_objType; }
		const auto& GetScreenPosition(void) const noexcept { return this->m_ScreenPosition; }
		const auto& GetCameraSize(void) const noexcept { return this->m_CameraSize; }
		const auto& GetMove(void) const noexcept { return this->m_move; }
		const auto& IsActive(void) const noexcept { return this->m_IsActive; }
		const auto& GetIsDelete(void) const noexcept { return this->m_IsDelete; }
	public:
		void			SetObjectID(int value) noexcept { this->m_ObjectID = value; }
		void			SetActive(bool value) noexcept { this->m_IsActive = value; }
		void			SetDelete(void) noexcept { this->m_IsDelete = true; }
		void			SetResetP(bool value) noexcept { this->m_IsResetPhysics = value; }
		void			SetScreenPosition(const Vector3DX& value, float size) noexcept {
			this->m_ScreenPosition = value;
			this->m_CameraSize = size;
		}
		void			SetUseShader(ShaderUseClass* value) noexcept { this->m_UseShader = value; }
		void			ResetMove(const Matrix3x3DX& RotMat, const Vector3DX& pos) noexcept {
			this->m_move.SetVec(Vector3DX::zero());
			this->m_move.SetMat(RotMat);
			this->m_move.SetPos(pos);
			this->m_move.Update(0.f, 0.f);
			UpdateObjMatrix(this->m_move.GetMat(), this->m_move.GetPos());
		}
		void			UpdateObjMatrix(const Matrix3x3DX& RotMat, const Vector3DX& pos) noexcept {
			this->GetObj().SetMatrix(RotMat.Get44DX() * Matrix4x4DX::Mtrans(pos));
			if (this->m_col.IsActive()) {
				this->m_col.SetMatrix(RotMat.Get44DX() * Matrix4x4DX::Mtrans(pos));
				this->m_ColActive = false;
			}
		}
		//判定起動
		void			SetupCol(void) noexcept {
			if (this->m_col.IsActive()) {
				for (int i = 0; i < static_cast<int>(this->m_col.GetMeshNum()); ++i) {
					this->m_col.SetupCollInfo(8, 8, 8, INVALID_ID, i);
				}
			}
		}
		auto			RefreshCol(const Vector3DX& StartPos, const Vector3DX& EndPos, float pRange) noexcept {
			if (this->m_ColActive) {
				return true;
			}				//すでに起動しているなら無視
			if (GetMinLenSegmentToPoint(StartPos, EndPos, m_move.GetPos()) <= pRange) {
				//判定起動
				this->m_ColActive = true;
				for (int i = 0; i < static_cast<int>(this->m_col.GetMeshNum()); ++i) {
					this->m_col.RefreshCollInfo(INVALID_ID, i);
				}
				return true;
			}
			return false;
		}
		//判定取得
		auto			GetColCapsule(const Vector3DX& StartPos, const Vector3DX& EndPos, float range, const int sel = 0) const noexcept { return this->m_col.CollCheck_Capsule(StartPos, EndPos, range, INVALID_ID, sel); }
		auto			GetColLine(const Vector3DX& StartPos, const Vector3DX& EndPos, const int sel = 0) const noexcept { return this->m_col.CollCheck_Line(StartPos, EndPos, INVALID_ID, sel); }
		void			GetColNearestInAllMesh(const Vector3DX& StartPos, Vector3DX* EndPos) const noexcept {
			MV1_COLL_RESULT_POLY colres;
			for (int i = 0; i < static_cast<int>(this->m_col.GetMeshNum()); ++i) {
				colres = GetColLine(StartPos, *EndPos, i);
				if (colres.HitFlag == TRUE) {
					*EndPos = colres.HitPosition;
				}
			}
		}
	public:
		virtual int	GetFrameNum(void) noexcept { return 0; }
		virtual const char* GetFrameStr(int) noexcept { return nullptr; }

		virtual int	GetMaterialNum(void) noexcept { return 0; }
		virtual const char* GetMaterialStr(int) noexcept { return nullptr; }

		virtual int	GetShapeNum(void) noexcept { return 0; }
		virtual const char* GetShapeStr(int) noexcept { return nullptr; }
	public:
		ObjectBaseClass(void) noexcept {}
		ObjectBaseClass(const ObjectBaseClass&) = delete;
		ObjectBaseClass(ObjectBaseClass&& o) = delete;
		ObjectBaseClass& operator=(const ObjectBaseClass&) = delete;
		ObjectBaseClass& operator=(ObjectBaseClass&& o) = delete;

		virtual ~ObjectBaseClass(void) noexcept {}
	public:
		void			Init(void) noexcept;
		virtual void	FirstExecute(void) noexcept {}
		void			ExecuteCommon(void) noexcept;
		virtual void	LateExecute(void) noexcept {}
		virtual void	DrawShadow(void) noexcept;
		virtual void	CheckDraw(void) noexcept;
		virtual void	Draw(bool isDrawSemiTrans) noexcept;
		void			Dispose(void) noexcept;
	protected:
		virtual void	Init_Sub(void) noexcept {}
		virtual void	Dispose_Sub(void) noexcept {}
	};
};
