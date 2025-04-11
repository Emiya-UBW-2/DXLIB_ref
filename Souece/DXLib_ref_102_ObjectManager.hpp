#pragma once
#include "DXLib_ref_103_ObjectBase.hpp"

namespace DXLibRef {
	class ModelBaseClass;
	class ObjectBaseClass;

	using SharedModel = std::shared_ptr<ModelBaseClass>;
	using SharedObj = std::shared_ptr<ObjectBaseClass>;

	class ObjectManager : public SingletonBase<ObjectManager> {
	private:
		friend class SingletonBase<ObjectManager>;
	private:
		std::vector<SharedModel>	m_Model;
		std::vector<SharedObj>		m_Object;
		std::vector<std::vector<SharedObj*>>	m_ObjectPtr;
		int							m_LastUniqueID{ 0 };
		switchs						m_ResetP;
	private:
		ObjectManager(void) noexcept {
			this->m_Object.reserve(256);
			this->m_ObjectPtr.reserve(10);
		}
		ObjectManager(const ObjectManager&) = delete;
		ObjectManager(ObjectManager&& o) = delete;
		ObjectManager& operator=(const ObjectManager&) = delete;
		ObjectManager& operator=(ObjectManager&& o) = delete;
	private:
		void			AddObject(const SharedObj& NewObj) noexcept;
		void			LoadModel(const SharedObj& pObj, const SharedObj& pAnim, const char* filepath, const char* objfilename = "model", const char* colfilename = "col") noexcept;
	public:
		//���f�����g�p�o�[�W����
		void			InitObject(const SharedObj& pObj) noexcept;
		//���f���g�p�o�[�W����
		void			InitObject(const SharedObj& pObj, const char* filepath, const char* objfilename = "model", const char* colfilename = "col") noexcept;
		//�A�j���[�V�����������f���o�[�W����
		void			InitObject(const SharedObj& pObj, const SharedObj& pAnim, const char* filepath, const char* objfilename = "model", const char* colfilename = "col") noexcept;
	public:
		SharedObj*		GetObj(int ModelType, int num) noexcept { return this->m_ObjectPtr.at(ModelType).at(num); }
	public:
		void			DelObj(const SharedObj& ptr) noexcept;
	public:
		void			ExecuteObject(void) noexcept;
		void			LateExecuteObject(void) noexcept;
		void			Draw(bool IsCheckDraw, int Range) noexcept;
		void			Draw_Shadow(void) noexcept;
		void			DeleteAll(void) noexcept;
	};
};
