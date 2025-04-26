#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	// --------------------------------------------------------------------------------------------------
	// ファイル操作
	// --------------------------------------------------------------------------------------------------
	// ディレクトリ内のファイル走査
	static void GetFileNamesInDirectory(const char* pPath, std::vector<WIN32_FIND_DATA>* pData) noexcept {
		pData->clear();
		WIN32_FIND_DATA win32fdt;
		HANDLE hFind = FindFirstFile(pPath, &win32fdt);
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (win32fdt.cFileName[0] != '.') {
					pData->emplace_back(win32fdt);
				}

			} while (FindNextFile(hFind, &win32fdt));
		} // else{ return false; }
		FindClose(hFind);
	}
	// ファイル選択ダイアログ
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
		~DialogManager(void) noexcept {}
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
		const auto* GetPath(void) noexcept {
			std::string str = strFile;
			return (str.find(cdir) != std::string::npos) ? &strFile[strlen(cdir) + 1] : strFile;
		}
	};

	// --------------------------------------------------------------------------------------------------
	// 起動
	// --------------------------------------------------------------------------------------------------
	// 起動
	static void CreateOurProcess(char* szCmd, DWORD flag, bool fWait) noexcept {
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		memset(&si, 0, sizeof(STARTUPINFO));
		memset(&pi, 0, sizeof(PROCESS_INFORMATION));
		si.cb = sizeof(STARTUPINFO);
		CreateProcess(NULL, szCmd, NULL, NULL, FALSE, flag, NULL, NULL, &si, &pi);
		if (fWait) WaitForSingleObject(pi.hProcess, INFINITE);	// 終了を待つ.
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	// 自身を多重起動
	static void StartMe(void) noexcept {
		char Path[MAX_PATH];
		// EXEのあるフォルダのパスを取得
		::GetModuleFileName(NULL, Path, MAX_PATH);
		CreateOurProcess(Path, SW_HIDE, false);
	}
	// ファイルが存在するかチェック
	static bool IsFileExist(const char* Path) noexcept {
		return std::filesystem::is_regular_file(Path);
		//DXLIB_Initを通ってからでないと動作しない
		/*
		FILEINFO FileInfo;
		return (FileRead_findFirst(Path, &FileInfo) != (DWORD_PTR)InvalidID);
		//*/
	}
	// クリップボードに画像をコピー
	static auto GetClipBoardGraphHandle(GraphHandle* RetHandle) noexcept {
		HWND  hwnd = GetMainWindowHandle();

		if (IsClipboardFormatAvailable(CF_BITMAP) == FALSE) {
			return false;
		}	// 指定した形式のデータがクリップボードにあるかを問い合わせる
		if (OpenClipboard(hwnd) == FALSE) {
			return false;
		}						// オープン
		HBITMAP hBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);					// データ取得
		CloseClipboard();														// クローズ
		if (hBitmap == NULL) {
			return false;
		}

		BITMAP bitmap{};
		GetObject(hBitmap, sizeof(BITMAP), &bitmap);

		BITMAPINFO bmpInfo{};
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biWidth = bitmap.bmWidth;
		bmpInfo.bmiHeader.biHeight = bitmap.bmHeight;
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biBitCount = 32;
		bmpInfo.bmiHeader.biCompression = BI_RGB;

		size_t Siz = static_cast<size_t>(bitmap.bmWidth * bitmap.bmHeight * (LONG)4);
		BYTE* bmpData = new BYTE[Siz];
		HDC hDC = GetDC(hwnd);
		GetDIBits(hDC, hBitmap, 0, static_cast<UINT>(bitmap.bmHeight), (void*)bmpData, &bmpInfo, DIB_RGB_COLORS);
		ReleaseDC(hwnd, hDC);
		DeleteObject(hBitmap);

		RetHandle->CreateGraphFromBmp(&bmpInfo, bmpData);	// ハンドルに変換
		delete[] bmpData;
		return true;
	}
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*DXLIBのファイル読み込み補助クラス																											*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class FileStreamDX {
		int mdata = InvalidID;
	public:
		//コンストラクタ
		FileStreamDX(void) noexcept {}
		//宣言時にファイルオープン版
		FileStreamDX(const char* FilePath) noexcept { Open(FilePath); }
		//デストラクタ
		~FileStreamDX(void) noexcept { Close(); }
	public:
		//ファイルを開き、探索ポイントを始点に移動
		void Open(const char* FilePath) noexcept { mdata = DxLib::FileRead_open(FilePath, FALSE); }
		// 1行そのまま取得し、次の行に探索ポイントを移る
		std::string SeekLineAndGetStr(void) const noexcept {
			const int charLength = 512;
			char mstr[charLength] = "";
			DxLib::FileRead_gets(mstr, charLength, mdata);
			return std::string(mstr);
		}
		// 探索ポイントが終端(EOF)で終わる
		bool ComeEof(void) const noexcept { return DxLib::FileRead_eof(mdata) != 0; }
		//　閉じる
		void Close(void) noexcept {
			if (mdata != InvalidID) {
				DxLib::FileRead_close(mdata);
				mdata = InvalidID;
			}
		}
	public:
		// 文字列から=より右の値取得
		static std::string getleft(std::string tempname) noexcept { return tempname.substr(0, tempname.find('=')); }
		// 文字列から=より右の値取得
		static std::string getright(std::string tempname) noexcept { return tempname.substr(tempname.find('=') + 1); }
	};

}
