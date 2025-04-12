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
	private:
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
		bool										m_IsEndLoadData{};
	public:
		ModelBaseClass(void) noexcept { m_IsEndLoadData = false; }
		ModelBaseClass(const ModelBaseClass&) = delete;
		ModelBaseClass(ModelBaseClass&& o) = delete;
		ModelBaseClass& operator=(const ModelBaseClass&) = delete;
		ModelBaseClass& operator=(ModelBaseClass&& o) = delete;

		virtual ~ModelBaseClass(void) noexcept {}
	public:
		const auto&		GetPhysicsSetup(void) const noexcept { return this->m_PHYSICS_SETUP; }
		const auto&		GetShapesList(void) const noexcept { return this->m_Shapes; }

		auto&			SetObj(void) noexcept { return this->m_obj; }
		const auto&		GetObj(void) const noexcept { return this->m_obj; }
		auto&			SetCol(void) noexcept { return this->m_col; }
		const auto&		GetCol(void) const noexcept { return this->m_col; }
		bool			HaveFrame(int frame) const noexcept { return this->m_Frames[static_cast<size_t>(frame)].first != InvalidID; }
		const auto&		GetFrame(int frame) const noexcept { return this->m_Frames[static_cast<size_t>(frame)].first; }
		const auto&		GetFrameBaseLocalMat(int frame) const noexcept { return this->m_Frames[static_cast<size_t>(frame)].second; }
		bool			HaveMaterial(int frame) const noexcept { return this->m_Materials[static_cast<size_t>(frame)] != InvalidID; }
		const auto&		GetMaterial(int frame) const noexcept { return this->m_Materials[static_cast<size_t>(frame)]; }
		const auto&		GetFilePath(void) const noexcept { return this->m_FilePath; }
	public:
		auto			GetPathCompare(const char* filepath, const char* objfilename, const char* colfilename) const noexcept {
			return ((this->m_FilePath == filepath) && (this->m_ObjFileName == objfilename) && (this->m_ColFileName == colfilename));
		}
	public:
		void			SetShapePer(int pShape, float Per) noexcept { this->m_Shapes[static_cast<size_t>(pShape)].second = Per; }
	public:
		void			LoadModel(PHYSICS_SETUP TYPE, const char* filepath, const char* objfilename = "model", const char* colfilename = "col") noexcept;
		void			LoadModelData(const std::shared_ptr<ObjectBaseClass>& pBase) noexcept;
		void			SaveModel(bool UseToonWhenCreateFile) noexcept;
		void			CopyModel(const std::shared_ptr<ModelBaseClass>& pBase) noexcept;
	public:
		void			DisposeModel(void) noexcept;
	};

	class ObjectBaseClass : public ModelBaseClass {
	private:
		bool										m_IsActive{ true };
		std::array<bool, 3>							m_IsDraw{ true };
		bool										m_IsDelete{ false };
		Vector3DX									m_MinAABB = Vector3DX::vget(-1.f * Scale3DRate, -0.f * Scale3DRate, -1.f * Scale3DRate);
		Vector3DX									m_MaxAABB = Vector3DX::vget(1.f * Scale3DRate, 1.f * Scale3DRate, 1.f * Scale3DRate);

		int											m_ObjectID{ InvalidID };
		bool										m_ColActive{ false };
		moves										m_move;
		Matrix4x4DX									m_PrevMat;// 物理更新のため
		bool										m_IsResetPhysics{ true };
		bool										m_IsFirstLoop{ true };
	protected:
		int											m_objType{ 0 };
		const auto&			IsFirstLoop(void) const noexcept { return this->m_IsFirstLoop; }
	public:
		// 
		auto&				SetMove(void) noexcept { return this->m_move; }
		// 
		void				SetAnimOnce(int ID, float speed) noexcept;
		void				SetAnimLoop(int ID, float speed) noexcept;
	public:
		const auto&			GetObjectID(void) const noexcept { return this->m_ObjectID; }
		const auto&			GetobjType(void) const noexcept { return this->m_objType; }
		const auto&			GetMove(void) const noexcept { return this->m_move; }
		const auto&			IsActive(void) const noexcept { return this->m_IsActive; }
		const auto&			GetIsDelete(void) const noexcept { return this->m_IsDelete; }
		const auto			IsDraw(int Range) const noexcept { return (Range == -1) ? true : this->m_IsDraw.at(Range); }
	public:
		void				SetMinAABB(const Vector3DX& value) noexcept { this->m_MinAABB = value; }
		void				SetMaxAABB(const Vector3DX& value) noexcept { this->m_MaxAABB = value; }
		void				SetObjectID(int value) noexcept { this->m_ObjectID = value; }
		void				SetActive(bool value) noexcept { this->m_IsActive = value; }
		void				SetDelete(void) noexcept { this->m_IsDelete = true; }
		void				SetResetP(bool value) noexcept { this->m_IsResetPhysics = value; }
		void				ResetMove(const Matrix3x3DX& RotMat, const Vector3DX& pos) noexcept {
			this->m_move.SetVec(Vector3DX::zero());
			this->m_move.SetMat(RotMat);
			this->m_move.SetPos(pos);
			this->m_move.Update(0.f, 0.f);
			UpdateObjMatrix(this->m_move.GetMat(), this->m_move.GetPos());
		}
		void				UpdateObjMatrix(const Matrix3x3DX& RotMat, const Vector3DX& pos) noexcept {
			GetObj().SetMatrix(RotMat.Get44DX() * Matrix4x4DX::Mtrans(pos));
			if (GetCol().IsActive()) {
				SetCol().SetMatrix(RotMat.Get44DX() * Matrix4x4DX::Mtrans(pos));
				this->m_ColActive = false;
			}
		}
		// 判定起動
		void			SetupCol(void) noexcept {
			if (GetCol().IsActive()) {
				for (int i : std::views::iota(0, static_cast<int>(GetCol().GetMeshNum()))) {
					SetCol().SetupCollInfo(8, 8, 8, InvalidID, i);
				}
			}
		}
		auto			RefreshCol(const Vector3DX& StartPos, const Vector3DX& EndPos, float pRange) noexcept {
			if (this->m_ColActive) {
				return true;
			}				// すでに起動しているなら無視
			if (GetMinLenSegmentToPoint(StartPos, EndPos, m_move.GetPos()) <= pRange) {
				// 判定起動
				this->m_ColActive = true;
				for (int i : std::views::iota(0, static_cast<int>(GetCol().GetMeshNum()))) {
					SetCol().RefreshCollInfo(InvalidID, i);
				}
				return true;
			}
			return false;
		}
		// 判定取得
		auto			GetColCapsule(const Vector3DX& StartPos, const Vector3DX& EndPos, float range, const int sel = 0) const noexcept { return GetCol().CollCheck_Capsule(StartPos, EndPos, range, InvalidID, sel); }
		auto			GetColLine(const Vector3DX& StartPos, const Vector3DX& EndPos, const int sel = 0) const noexcept { return GetCol().CollCheck_Line(StartPos, EndPos, InvalidID, sel); }
		void			GetColNearestInAllMesh(const Vector3DX& StartPos, Vector3DX* EndPos) const noexcept {
			MV1_COLL_RESULT_POLY colres;
			for (int i : std::views::iota(0, static_cast<int>(GetCol().GetMeshNum()))) {
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
		void			CheckDraw(int Range) noexcept;
		virtual void	Draw(bool isDrawSemiTrans, int Range) noexcept;
		void			Dispose(void) noexcept;
	protected:
		virtual void	Init_Sub(void) noexcept {}
		virtual void	CheckDraw_Sub(int) noexcept {}
		virtual void	Dispose_Sub(void) noexcept {}
	};
};
