#pragma once
#include "DXLib_ref.h"
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*DXLIBに直接かかわりのない便利モノ																											*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/

namespace DXLibRef {
	//--------------------------------------------------------------------------------------------------
	//シングルトン
	//--------------------------------------------------------------------------------------------------
	template <class T>
	class SingletonBase {
	private:
		static const T* m_Singleton;
	public:
		static void Create(void) noexcept {
			m_Singleton = new T();
		}
		static T* Instance(void) noexcept {
			if (m_Singleton == nullptr) {
				MessageBox(NULL, "Failed Instance Create", "", MB_OK);
				exit(-1);
			}
			//if (m_Singleton == nullptr) { m_Singleton = new T(); }
			return (T*)m_Singleton;
		}
	protected:
		SingletonBase() noexcept {}
		virtual ~SingletonBase() noexcept {}
	private:
		SingletonBase(const SingletonBase &) = delete;
		SingletonBase& operator=(const SingletonBase &) = delete;
		SingletonBase(SingletonBase &&) = delete;
		SingletonBase& operator=(SingletonBase &&) = delete;
	};
	//子のサンプル
	/*
	class A : public SingletonBase<A> {
	private:
		friend class SingletonBase<A>;
	}
	//*/

	//--------------------------------------------------------------------------------------------------
	// ファイル操作
	//--------------------------------------------------------------------------------------------------
	//ディレクトリ内のファイル走査
	static void GetFileNamesInDirectory(const char* pPath, std::vector<WIN32_FIND_DATA>* pData) noexcept {
		pData->clear();
		WIN32_FIND_DATA win32fdt;
		HANDLE hFind = FindFirstFile(pPath, &win32fdt);
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (win32fdt.cFileName[0] != '.') {
					pData->resize(pData->size() + 1);
					pData->back() = win32fdt;
				}

			} while (FindNextFile(hFind, &win32fdt));
		} //else{ return false; }
		FindClose(hFind);
	}
	//ファイル選択ダイアログ
	class DialogManager {
		OPENFILENAME	ofn;
		TCHAR			strFile[MAX_PATH]{ 0 };
		TCHAR			cdir[MAX_PATH]{ 0 };
	public:
		DialogManager(void) noexcept {
			char Path[MAX_PATH];
			GetModuleFileName(NULL, Path, MAX_PATH);			// EXEのあるフォルダのパスを取得
			SetCurrentDirectory(Path);							// カレントディレクトリの設定
		}
		~DialogManager(void) noexcept { }
	public:
		void			Init(void) noexcept {
			GetCurrentDirectory(MAX_PATH, cdir);
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = GetMainWindowHandle();
			ofn.lpstrFilter =
				TEXT("Picture files {*.bmp}\0*.bmp\0")
				TEXT("Picture files {*.dds}\0*.dds\0")
				TEXT("Picture files {*.jpg}\0*.jpg\0")
				TEXT("Picture files {*.png}\0*.png\0");
			ofn.lpstrCustomFilter = NULL;
			ofn.nMaxCustFilter = NULL;
			ofn.nFilterIndex = 0;
			ofn.lpstrFile = strFile;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
			ofn.lpstrInitialDir = cdir;
			ofn.lpstrTitle = "";
		}
		bool			Open(void) noexcept {
			ofn.lpstrTitle = "開く";
			return GetOpenFileName(&ofn);
		}
		bool			Save(void) noexcept {
			ofn.lpstrTitle = "保存";
			return GetSaveFileName(&ofn);
		}
		const auto*		GetPath(void) noexcept {
			std::string str = strFile;
			return (str.find(cdir) != std::string::npos) ? &strFile[strlen(cdir) + 1] : strFile;
		}
	};

	//--------------------------------------------------------------------------------------------------
	// 起動
	//--------------------------------------------------------------------------------------------------
	//起動
	static void CreateOurProcess(char* szCmd, DWORD flag, bool fWait) noexcept {
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		memset(&si, 0, sizeof(STARTUPINFO));
		memset(&pi, 0, sizeof(PROCESS_INFORMATION));
		si.cb = sizeof(STARTUPINFO);
		CreateProcess(NULL, szCmd, NULL, NULL, FALSE, flag, NULL, NULL, &si, &pi);
		if (fWait) WaitForSingleObject(pi.hProcess, INFINITE);	//終了を待つ.
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	//自身を多重起動
	static void StartMe(void) noexcept {
		char Path[MAX_PATH];
		// EXEのあるフォルダのパスを取得
		::GetModuleFileName(NULL, Path, MAX_PATH);
		CreateOurProcess(Path, SW_HIDE, false);
	}

	//--------------------------------------------------------------------------------------------------
	// 角度変換
	//--------------------------------------------------------------------------------------------------
	//角度からラジアンに
	extern void* enabler;// ダミー変数
	template <class T, typename std::enable_if<std::is_arithmetic<T>::value>::type*& = enabler>
	constexpr float deg2rad(T p1) noexcept { return float(p1) * DX_PI_F / 180.f; }
	//ラジアンから角度に
	template <class T, typename std::enable_if<std::is_arithmetic<T>::value>::type*& = enabler>
	constexpr float rad2deg(T p1) noexcept { return float(p1) * 180.f / DX_PI_F; }

	//余弦定理
	constexpr float GetCosFormula(float a, float b, float c) noexcept {
		if (b + c > a && c + a > b && a + b > c) {
			return std::clamp((b * b + c * c - a * a) / (2.f * b*c), -1.f, 1.f);
		}
		return 1.f;
	}

	//--------------------------------------------------------------------------------------------------
	// 文字変換
	//--------------------------------------------------------------------------------------------------
	/*wstringをstringへ変換*/
	static std::string WStringToString(std::wstring_view oWString) noexcept {
		// wstring → SJIS
		int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, oWString.data(), int(oWString.size() + 1), nullptr, 0, NULL, NULL);
		// バッファの取得
		CHAR* cpMultiByte = new CHAR[static_cast<size_t>(iBufferSize)];
		// wstring → SJIS
		WideCharToMultiByte(CP_OEMCP, 0, oWString.data(), int(oWString.size() + 1), cpMultiByte, iBufferSize, NULL, NULL);
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
		int iBufferSize = MultiByteToWideChar(CP_ACP, 0, oString.data(), int(oString.size() + 1), nullptr, 0);
		// バッファの取得
		wchar_t* cpUCS2 = new wchar_t[static_cast<size_t>(iBufferSize)];
		// SJIS → wstring
		MultiByteToWideChar(CP_ACP, 0, oString.data(), int(oString.size() + 1), cpUCS2, iBufferSize);
		// stringの生成
		std::wstring oRet(cpUCS2, cpUCS2 + iBufferSize - 1);
		// バッファの破棄
		delete[] cpUCS2;
		// 変換結果を返す
		return oRet;
	}
	//--------------------------------------------------------------------------------------------------
	// フォントファイル管理
	//--------------------------------------------------------------------------------------------------
	class FontInstallClass {
		std::string		m_Path;
	public:
		void Install(LPCSTR font_path) noexcept {
			m_Path = font_path;
			if (AddFontResourceEx(m_Path.c_str(), FR_PRIVATE, NULL) == 0) {
				MessageBox(NULL, "フォント読込失敗", "", MB_OK);
			}
		}
		void Remove() noexcept {
			if (RemoveFontResourceEx(m_Path.c_str(), FR_PRIVATE, NULL) == 0) {
				MessageBox(NULL, "フォント読込削除", "", MB_OK);
			}
		}
	};
}
