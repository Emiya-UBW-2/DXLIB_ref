#include "DXLib_ref_102_ObjectManager.hpp"

namespace DXLibRef {
	const ObjectManager* SingletonBase<ObjectManager>::m_Singleton = nullptr;

	void			ObjectManager::AddObject(const SharedObj& NewObj) noexcept {
		for (auto& object : this->m_ObjectList) {
			if (object) { continue; }
			object = NewObj;
			object->SetObjectID(this->m_LastUniqueID);
			return;
		}
		this->m_ObjectList.emplace_back(NewObj);
		this->m_ObjectList.back()->SetObjectID(this->m_LastUniqueID);
		++this->m_LastUniqueID;
	}
	const SharedModel*	ObjectManager::LoadModelBefore(std::string_view filepath, std::string_view objfilename, std::string_view colfilename) noexcept {
		auto Find = std::find_if(this->m_ModelList.begin(), this->m_ModelList.end(), [&](const SharedModel& tgt) {return tgt && tgt->GetPathCompare(filepath, objfilename, colfilename); });
		if (Find != this->m_ModelList.end()) {
			return &*Find;
		}
		else {
			this->m_ModelList.emplace_back(std::make_shared<ResourceModel>());
			this->m_ModelList.back()->LoadModel(PHYSICS_SETUP::DISABLE, filepath, objfilename, colfilename);
			return  &this->m_ModelList.back();
		}
	}
	void			ObjectManager::LoadModelAfter(const SharedObj& pObj, const SharedObj& pAnim, std::string_view filepath, std::string_view objfilename, std::string_view colfilename) noexcept {
		const SharedModel* Ptr = LoadModelBefore(filepath, objfilename, colfilename);
		if (!(*Ptr)->m_IsEndLoadData) {
			(*Ptr)->LoadModelData(pObj);
			(*Ptr)->SaveModel(false);
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
	void			ObjectManager::InitObject(const SharedObj& pObj, std::string_view filepath, std::string_view objfilename, std::string_view colfilename) noexcept {
		AddObject(pObj);
		LoadModelAfter(pObj, pObj, filepath, objfilename, colfilename);
		pObj->Init();
	}
	void			ObjectManager::InitObject(const SharedObj& pObj, const SharedObj& pAnim, std::string_view filepath, std::string_view objfilename, std::string_view colfilename) noexcept {
		AddObject(pObj);
		LoadModelAfter(pObj, pAnim, filepath, objfilename, colfilename);
		pObj->Init();
	}
	SharedObj*		ObjectManager::GetObj(int ModelType, int num) noexcept {
		int cnt = 0;
		for (auto& object : this->m_ObjectList) {
			if (!object) { continue; }
			if (object->GetobjType() == ModelType) {
				if (cnt == num) {
					return &object;
				}
				++cnt;
			}
		}
		return nullptr;
	}
	void ObjectManager::DelObj(const SharedObj& ptr) noexcept {
		//実体削除
		for (auto& object : this->m_ObjectList) {
			if (!object) { continue; }
			if (object == ptr) {
				object->Dispose();
				object.reset();
				break;
			}
		}
	}
	//
	void			ObjectManager::UpdateObject(void) noexcept {
		// オブジェクトが増えた場合に備えて範囲forは使わない
		for (size_t loop = 0; loop < this->m_ObjectList.size(); ++loop) {
			auto& object = this->m_ObjectList[loop];
			if (!object) { continue; }
			object->FirstUpdate();
		}
		// 物理など共通項目のアップデート
		//this->m_ResetP.Update(CheckHitKey(KEY_INPUT_P) != 0);
		for (auto& object : this->m_ObjectList) {
			if (!object) { continue; }
			if (object->GetIsDelete()) { continue; }
			/*
			if (this->m_ResetP.trigger()) {
				object->SetResetP(true);
			}
			//*/
			object->UpdateCommon();
		}
		// オブジェクトの排除チェック
		for (auto& object : this->m_ObjectList) {
			if (!object) { continue; }
			if (!object->GetIsDelete()) { continue; }
			object->Dispose();
			object.reset();
		}
	}
	void			ObjectManager::LateUpdateObject(void) noexcept {
		for (auto& object : this->m_ObjectList) {
			if (!object) { continue; }
			object->LateUpdate();
		}
	}
	void			ObjectManager::Draw(bool IsCheckDraw, int Range) noexcept {
		for (auto& object : this->m_ObjectList) {
			if (!object) { continue; }
			if (!object->IsActive()) { continue; }
			if (IsCheckDraw) {
				object->CheckDraw(Range);
			}
			object->Draw(false, Range);
		}
		for (auto& object : this->m_ObjectList) {
			if (!object) { continue; }
			if (!object->IsActive()) { continue; }
			object->Draw(true, Range);
		}
	}
	void			ObjectManager::Draw_Shadow(void) noexcept {
		for (auto& object : this->m_ObjectList) {
			if (!object) { continue; }
			if (!object->IsActive()) { continue; }
			object->DrawShadow();
		}
	}
	void			ObjectManager::DeleteAll(void) noexcept {
		for (auto& object : this->m_ObjectList) {
			if (!object) { continue; }
			object->Dispose();
			object.reset();
		}
		this->m_ObjectList.clear();

		for (auto& model : this->m_ModelList) {
			if (!model) { continue; }
			model->DisposeModel();
			model.reset();
		}
		this->m_ModelList.clear();

		this->m_LastUniqueID = 0;// 一旦ユニークIDもリセット
	}
};
