#include	"DXLib_ref_102_ObjectManager.hpp"

namespace DXLib_ref {
	const ObjectManager* SingletonBase<ObjectManager>::m_Singleton = nullptr;

	void			ObjectManager::AddObject(const SharedObj& NewObj) noexcept {
		this->m_Object.resize(this->m_Object.size() + 1);
		this->m_Object.back() = NewObj;
	}
	void			ObjectManager::LoadModel(const SharedObj& pObj, const SharedObj& pAnim, const char* filepath, const char* objfilename, const char* colfilename) const noexcept {
		for (auto& o : this->m_Object) {
			if (!o->GetIsBaseModel()) { continue; }
			if (!o->GetPathCompare(filepath, objfilename, colfilename)) { continue; }
			pObj->CopyModel(o);
			if (pAnim) {
				MV1::SetAnime(&pObj->GetObj(), pAnim->GetObj());
			}
			return;
		}
		pObj->LoadModel(PHYSICS_SETUP::DISABLE, filepath, objfilename, colfilename);
		pObj->SaveModel(false);
		if (pAnim) {
			MV1::SetAnime(&pObj->GetObj(), pAnim->GetObj());
		}
	}
	SharedObj*		ObjectManager::GetObj(int ModelType, int num) noexcept {
		int cnt = 0;
		for (auto&o : this->m_Object) {
			if (o->GetobjType() == ModelType) {
				if (cnt == num) {
					return &o;
				}
				cnt++;
			}
		}
		return nullptr;
	}
	void			ObjectManager::DelObj(SharedObj* ptr) noexcept {
		for (auto&o : this->m_Object) {
			if (o == *ptr) {
				//���Ԃ̈ێ��̂��߂�����erase
				o->Dispose();
				this->m_Object.erase(this->m_Object.begin() + (&o - &this->m_Object.front()));
				break;
			}
		}
	}

	void			ObjectManager::ExecuteObject(void) noexcept {
		//�I�u�W�F�N�g���������ꍇ�ɔ����Ĕ͈�for�͎g��Ȃ�
		for (int i = 0; i < this->m_Object.size(); i++) {
			auto& o = this->m_Object[i];
			if (!o->GetIsDelete()) {
				o->FirstExecute();
			}
		}
		//�����A�b�v�f�[�g
		this->m_ResetP.Execute(CheckHitKeyWithCheck(KEY_INPUT_P) != 0);
		for (int i = 0; i < this->m_Object.size(); i++) {
			auto& o = this->m_Object[i];
			if (!o->GetIsDelete()) {
				if (this->m_ResetP.trigger()) { o->SetResetP(true); }
				o->ExecuteCommon();
			}
		}
		//�I�u�W�F�N�g�̔r���`�F�b�N
		for (int i = 0; i < this->m_Object.size(); i++) {
			auto& o = this->m_Object[i];
			if (o->GetIsDelete()) {
				//���Ԃ̈ێ��̂��߂�����erase
				o->Dispose();
				this->m_Object.erase(this->m_Object.begin() + i);
				i--;
			}
		}
	}
	void			ObjectManager::LateExecuteObject(void) noexcept {
		for (auto&o : this->m_Object) {
			o->LateExecute();
			Vector3DX campos; campos.z = (-1.f);
			o->SetScreenPosition(campos, -1.f);
		}
	}

	void			ObjectManager::Draw_Depth(void) noexcept {
		for (auto& o : this->m_Object) {
			o->Depth_Draw();
		}
	}
	void			ObjectManager::Draw() noexcept {
		for (auto& o : this->m_Object) {
			o->CheckDraw();
			o->Draw(false);
		}
		for (auto& o : this->m_Object) {
			o->Draw(true);
		}
	}
	void			ObjectManager::Draw_Shadow(void) noexcept {
		for (auto& o : this->m_Object) {
			o->DrawShadow();
		}
	}

	void			ObjectManager::DeleteAll(void) noexcept {
		for (auto& o : this->m_Object) {
			if (o) {
				o->Dispose();
			}
		}
		this->m_Object.clear();
	}
};
