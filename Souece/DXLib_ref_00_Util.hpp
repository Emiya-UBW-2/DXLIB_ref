#pragma once
// #include "DXLib_ref.h"
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*DXLIBに直接かかわりのない便利モノ																											*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/

namespace DXLibRef {
	// --------------------------------------------------------------------------------------------------
	// シングルトン
	// --------------------------------------------------------------------------------------------------
	template <class T>
	class SingletonBase {
	private:
		static const T*	m_Singleton;
	public:
		static void Create(void) noexcept {
			m_Singleton = new T();
		}
		static T* Instance(void) noexcept {
			if (m_Singleton == nullptr) {
				MessageBox(NULL, "Failed Instance Create", "", MB_OK);
				exit(InvalidID);
			}
			// if (m_Singleton == nullptr) { m_Singleton = new T(); }
			return (T*)m_Singleton;
		}
		static void Release(void) noexcept {
			delete m_Singleton;
		}
	protected:
		SingletonBase(void) noexcept {}
		virtual ~SingletonBase(void) noexcept {}
	private:
		SingletonBase(const SingletonBase&) = delete;
		SingletonBase& operator=(const SingletonBase&) = delete;
		SingletonBase(SingletonBase&&) = delete;
		SingletonBase& operator=(SingletonBase&&) = delete;
	};
	// 子のサンプル
	/*
	class A : public SingletonBase<A> {
	private:
		friend class SingletonBase<A>;
	}
	//*/

	// --------------------------------------------------------------------------------------------------
	// DXLIBのハンドル操作系の基底クラス
	// --------------------------------------------------------------------------------------------------
	class DXHandle {
	private:
		int		m_handle{ InvalidID };
	protected:
		constexpr DXHandle(int h) noexcept : m_handle(h) {}
	public:
		constexpr DXHandle(void) noexcept : m_handle(InvalidID) {}
		DXHandle(const DXHandle&) = delete;
		DXHandle(DXHandle&& o) noexcept : m_handle(o.get()) { o.SetHandleDirect(InvalidID); }
		DXHandle& operator=(const DXHandle&) = delete;
		DXHandle& operator=(DXHandle&& o) noexcept {
			SetHandleDirect(o.get());
			o.SetHandleDirect(InvalidID);
			return *this;
		}

		virtual ~DXHandle(void) noexcept {
			Dispose();
		}
	public:
		const int get(void) const noexcept { return this->m_handle; }
		constexpr bool IsActive(void) const noexcept { return this->m_handle != InvalidID; }
		constexpr explicit operator bool(void) const noexcept { return IsActive(); }
	public:
		void Dispose(void) noexcept {
			if (IsActive()) {
				Dispose_Sub();
				SetHandleDirect(InvalidID);
			}
		}
	protected:
		void SetHandleDirect(int handle) noexcept { this->m_handle = handle; }
		virtual void Dispose_Sub(void) noexcept {}
	};

	// --------------------------------------------------------------------------------------------------
	// 文字変換
	// --------------------------------------------------------------------------------------------------
	/*wstringをstringへ変換*/
	static std::string WStringToString(std::wstring_view oWString) noexcept {
		// wstring → SJIS
		int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, oWString.data(), static_cast<int>(oWString.size() + 1), nullptr, 0, NULL, NULL);
		// バッファの取得
		CHAR* cpMultiByte = new CHAR[static_cast<size_t>(iBufferSize)];
		// wstring → SJIS
		WideCharToMultiByte(CP_OEMCP, 0, oWString.data(), static_cast<int>(oWString.size() + 1), cpMultiByte, iBufferSize, NULL, NULL);
		// stringの生成
		std::string oRet(cpMultiByte, cpMultiByte + iBufferSize - 1);
		// バッファの破棄
		delete[] cpMultiByte;
		// 変換結果を返す
		return oRet;
	}
	/*stringをwstringへ変換する*/
	static std::wstring StringToWString(std::string_view oString) noexcept {
		// SJIS → wstring
		int iBufferSize = MultiByteToWideChar(CP_ACP, 0, oString.data(), static_cast<int>(oString.size() + 1), nullptr, 0);
		// バッファの取得
		wchar_t* cpUCS2 = new wchar_t[static_cast<size_t>(iBufferSize)];
		// SJIS → wstring
		MultiByteToWideChar(CP_ACP, 0, oString.data(), static_cast<int>(oString.size() + 1), cpUCS2, iBufferSize);
		// stringの生成
		std::wstring oRet(cpUCS2, cpUCS2 + iBufferSize - 1);
		// バッファの破棄
		delete[] cpUCS2;
		// 変換結果を返す
		return oRet;
	}
}
