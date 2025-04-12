#include "DXLib_ref_102_ObjectManager.hpp"

namespace DXLibRef {
	const ObjectManager* SingletonBase<ObjectManager>::m_Singleton = nullptr;

	void			ObjectManager::AddObject(const SharedObj& NewObj) noexcept {
		for (auto& o : this->m_Object) {
			if (o) { continue; }
			o = NewObj;
			o->SetObjectID(this->m_LastUniqueID);
			return;
		}
		this->m_Object.emplace_back(NewObj);
		this->m_Object.back()->SetObjectID(this->m_LastUniqueID);
		++this->m_LastUniqueID;
	}
	void			ObjectManager::LoadModel(const SharedObj& pObj, const SharedObj& pAnim, const char* filepath, const char* objfilename, const char* colfilename) noexcept {
		const SharedModel* Ptr = nullptr;
		auto Find = std::find_if(this->m_Model.begin(), this->m_Model.end(), [&](const SharedModel& tgt) {return tgt && tgt->GetPathCompare(filepath, objfilename, colfilename); });
		if (Find != this->m_Model.end()) {
			Ptr = &*Find;
		}
		else {
			this->m_Model.emplace_back(std::make_shared<ModelBaseClass>());
			this->m_Model.back()->LoadModel(pObj, PHYSICS_SETUP::DISABLE, filepath, objfilename, colfilename);
			this->m_Model.back()->SaveModel(false);
			Ptr = &this->m_Model.back();
		}
		pObj->CopyModel(*Ptr);
		pObj->SetupCol();
		if (pAnim) {
			MV1::SetAnime(&pObj->SetObj(), pAnim->GetObj());
		}
	}
	//
	void			ObjectManager::InitObject(const SharedObj& pObj) noexcept {
		AddObject(pObj);
		pObj->Init();
	}
	void			ObjectManager::InitObject(const SharedObj& pObj, const char* filepath, const char* objfilename, const char* colfilename) noexcept {
		AddObject(pObj);
		LoadModel(pObj, pObj, filepath, objfilename, colfilename);
		pObj->Init();
	}
	void			ObjectManager::InitObject(const SharedObj& pObj, const SharedObj& pAnim, const char* filepath, const char* objfilename, const char* colfilename) noexcept {
		AddObject(pObj);
		LoadModel(pObj, pAnim, filepath, objfilename, colfilename);
		pObj->Init();
	}
	SharedObj*		ObjectManager::GetObj(int ModelType, int num) noexcept {
		int cnt = 0;
		for (auto& o : this->m_Object) {
			if (!o) { continue; }
			if (o->GetobjType() == ModelType) {
				if (cnt == num) {
					return &o;
				}
				++cnt;
			}
		}
		return nullptr;
	}
	void ObjectManager::DelObj(const SharedObj& ptr) noexcept {
		//���̍폜
		for (auto& o : this->m_Object) {
			if (!o) { continue; }
			if (o == ptr) {
				o->Dispose();
				o.reset();
				break;
			}
		}
	}
	//
	void			ObjectManager::ExecuteObject(void) noexcept {
		// �I�u�W�F�N�g���������ꍇ�ɔ����Ĕ͈�for�͎g��Ȃ�
		for (int loop = 0; loop < static_cast<int>(this->m_Object.size()); loop++) {
			auto& o = this->m_Object.at(static_cast<size_t>(loop));
			if (!o) { continue; }
			if (o->GetIsDelete()) { continue; }
			o->FirstExecute();
		}
		// �����A�b�v�f�[�g
		//this->m_ResetP.Update(CheckHitKey(KEY_INPUT_P) != 0);
		for (int loop = 0; loop < static_cast<int>(this->m_Object.size()); loop++) {
			auto& o = this->m_Object.at(static_cast<size_t>(loop));
			if (!o) { continue; }
			if (o->GetIsDelete()) { continue; }
			/*
			if (this->m_ResetP.trigger()) {
				o->SetResetP(true);
			}
			//*/
			o->ExecuteCommon();
		}
		// �I�u�W�F�N�g�̔r���`�F�b�N
		for (int loop = 0; loop < static_cast<int>(this->m_Object.size()); loop++) {
			auto& o = this->m_Object.at(static_cast<size_t>(loop));
			if (!o) { continue; }
			if (!o->GetIsDelete()) { continue; }
			o->Dispose();
			o.reset();
		}
	}
	void			ObjectManager::LateExecuteObject(void) noexcept {
		for (auto& o : this->m_Object) {
			if (!o) { continue; }
			o->LateExecute();
		}
	}
	void			ObjectManager::Draw(bool IsCheckDraw, int Range) noexcept {
		for (auto& o : this->m_Object) {
			if (!o) { continue; }
			if (IsCheckDraw) {
				o->CheckDraw(Range);
			}
			o->Draw(false, Range);
		}
		for (auto& o : this->m_Object) {
			if (!o) { continue; }
			o->Draw(true, Range);
		}
	}
	void			ObjectManager::Draw_Shadow(void) noexcept {
		for (auto& o : this->m_Object) {
			if (!o) { continue; }
			o->DrawShadow();
		}
	}
	void			ObjectManager::DeleteAll(void) noexcept {
		for (int loop = 0; loop < static_cast<int>(this->m_Object.size()); loop++) {
			auto& o = this->m_Object.at(static_cast<size_t>(loop));
			if (!o) { continue; }
			o->Dispose();
			o.reset();
		}
		this->m_Object.clear();

		for (auto& o : this->m_Model) {
			if (!o) { continue; }
			o->DisposeModel();
			o.reset();
		}
		this->m_Model.clear();

		this->m_LastUniqueID = 0;// ��U���j�[�NID�����Z�b�g
	}
};
