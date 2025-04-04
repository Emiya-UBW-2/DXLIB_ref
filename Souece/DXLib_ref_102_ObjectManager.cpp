#include "DXLib_ref_102_ObjectManager.hpp"

namespace DXLibRef {
	const ObjectManager* SingletonBase<ObjectManager>::m_Singleton = nullptr;

	void			ObjectManager::AddObject(const SharedObj& NewObj) noexcept {
		this->m_Object.emplace_back(NewObj);
		this->m_Object.back()->SetObjectID(this->m_LastUniqueID);
		++this->m_LastUniqueID;
	}
	void			ObjectManager::LoadModel(const SharedObj& pObj, const SharedObj& pAnim, const char* filepath, const char* objfilename, const char* colfilename) noexcept {
		const SharedModel* Ptr = nullptr;
		for (auto& o : this->m_Model) {
			if (!o->GetPathCompare(filepath, objfilename, colfilename)) {
				continue;
			}
			Ptr = &o;
			break;
		}
		if (!Ptr) {
			m_Model.emplace_back(std::make_shared<ModelBaseClass>());
			m_Model.back()->LoadModel(pObj, PHYSICS_SETUP::DISABLE, filepath, objfilename, colfilename);
			m_Model.back()->SaveModel(false);
			Ptr = &m_Model.back();
		}
		pObj->CopyModel(*Ptr);
		pObj->SetupCol();
		if (pAnim) {
			MV1::SetAnime(&pObj->SetObj(), pAnim->GetObj());
		}
	}
	SharedObj* ObjectManager::GetObj(int ModelType, int num) noexcept {
		int cnt = 0;
		for (auto& o : this->m_Object) {
			if (o->GetobjType() == ModelType) {
				if (cnt == num) {
					return &o;
				}
				++cnt;
			}
		}
		return nullptr;
	}
	void			ObjectManager::DelObj(SharedObj* ptr) noexcept {
		for (size_t index = 0; auto & o : this->m_Object) {
			if (o == *ptr) {
				// 順番の維持のためここはerase
				o->Dispose();
				this->m_Object.erase(this->m_Object.begin() + index);
				break;
			}
			index++;
		}
	}
	void			ObjectManager::ExecuteObject(void) noexcept {
		// オブジェクトが増えた場合に備えて範囲forは使わない
		for (int i = 0; i < static_cast<int>(this->m_Object.size()); i++) {
			auto& o = this->m_Object.at(static_cast<size_t>(i));
			if (!o->GetIsDelete()) {
				o->FirstExecute();
			}
		}
		// 物理アップデート
		//this->m_ResetP.Update(CheckHitKey(KEY_INPUT_P) != 0);
		for (int i = 0; i < static_cast<int>(this->m_Object.size()); i++) {
			auto& o = this->m_Object.at(static_cast<size_t>(i));
			/*
			if (!o->GetIsDelete()) {
				if (this->m_ResetP.trigger()) {
					o->SetResetP(true);
				}
			}
			//*/
			if (!o->GetIsDelete()) {
				o->ExecuteCommon();
			}
		}
		// オブジェクトの排除チェック
		for (int i = 0, Max = static_cast<int>(this->m_Object.size()); i < Max; i++) {
			auto& o = this->m_Object.at(static_cast<size_t>(i));
			if (o->GetIsDelete()) {
				// 順番の維持のためここはerase
				o->Dispose();
				this->m_Object.erase(this->m_Object.begin() + i);
				i--;
				Max--;
			}
		}
	}
	void			ObjectManager::LateExecuteObject(void) noexcept {
		for (auto& o : this->m_Object) {
			o->LateExecute();
		}
	}
	void			ObjectManager::Draw(bool IsCheckDraw, int Range) noexcept {
		for (auto& o : this->m_Object) {
			if (IsCheckDraw) {
				o->CheckDraw(Range);
			}
			o->Draw(false, Range);
		}
		for (auto& o : this->m_Object) {
			o->Draw(true, Range);
		}
	}
	void			ObjectManager::Draw_Shadow(void) noexcept {
		for (auto& o : this->m_Object) {
			o->DrawShadow();
		}
	}
	void			ObjectManager::DeleteAll(void) noexcept {
		for (int i = 0; i < static_cast<int>(this->m_Object.size()); i++) {
			auto& o = this->m_Object.at(static_cast<size_t>(i));
			if (o) {
				o->Dispose();
			}
		}
		this->m_Object.clear();
		for (int i = 0; i < static_cast<int>(this->m_Model.size()); i++) {
			auto& o = this->m_Model.at(static_cast<size_t>(i));
			if (o) {
				o->DisposeModel();
			}
		}
		this->m_Model.clear();
		this->m_LastUniqueID = 0;// 一旦ユニークIDもリセット
	}
};
