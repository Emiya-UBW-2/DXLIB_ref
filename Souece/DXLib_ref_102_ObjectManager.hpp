#pragma once
#include "DXLib_ref_103_ObjectBase.hpp"

namespace DXLibRef {
	class ResourceModel;
	class BaseObject;

	using SharedModel = std::shared_ptr<ResourceModel>;
	using SharedObj = std::shared_ptr<BaseObject>;

	class ObjectManager : public SingletonBase<ObjectManager> {
	private:
		friend class SingletonBase<ObjectManager>;
	private:
		std::vector<SharedModel>	m_ModelList;
		std::vector<SharedObj>		m_ObjectList;
		int							m_LastUniqueID{ 0 };
		//switchs						m_ResetP;
	private:
		ObjectManager(void) noexcept {
			this->m_ModelList.reserve(24);
			this->m_ObjectList.reserve(256);
		}
		ObjectManager(const ObjectManager&) = delete;
		ObjectManager(ObjectManager&&) = delete;
		ObjectManager& operator=(const ObjectManager&) = delete;
		ObjectManager& operator=(ObjectManager&&) = delete;
	private:
		void			AddObject(const SharedObj& NewObj) noexcept;
		void			LoadModelAfter(const SharedObj& pObj, const SharedObj& pAnim, std::string_view filepath, std::string_view objfilename = "model", std::string_view colfilename = "col") noexcept;
	public:
		//あらかじめモデルを読み込んでおく
		const SharedModel*	LoadModelBefore(std::string_view filepath, std::string_view objfilename = "model", std::string_view colfilename = "col") noexcept;
		//モデル未使用バージョン
		void			InitObject(const SharedObj& pObj) noexcept;
		//モデル使用バージョン
		void			InitObject(const SharedObj& pObj, std::string_view filepath, std::string_view objfilename = "model", std::string_view colfilename = "col") noexcept;
		//アニメーション兼務モデルバージョン
		void			InitObject(const SharedObj& pObj, const SharedObj& pAnim, std::string_view filepath, std::string_view objfilename = "model", std::string_view colfilename = "col") noexcept;
	public:
		SharedObj*		GetObj(int ModelType, int num) noexcept;
	public:
		void			DelObj(const SharedObj& ptr) noexcept;
	public:
		void			UpdateObject(void) noexcept;
		void			LateUpdateObject(void) noexcept;
		void			Draw(bool IsCheckDraw, int Range) noexcept;
		void			Draw_Shadow(void) noexcept;
		void			DeleteAll(void) noexcept;
	};
};
