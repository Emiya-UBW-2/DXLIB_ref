#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	// --------------------------------------------------------------------------------------------------
	// �t�@�C������
	// --------------------------------------------------------------------------------------------------
	// �f�B���N�g�����̃t�@�C������
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
		} // else{ return false; }
		FindClose(hFind);
	}
	// �t�@�C���I���_�C�A���O
	class DialogManager {
		OPENFILENAME	ofn;
		TCHAR			strFile[MAX_PATH]{ 0 };
		TCHAR			cdir[MAX_PATH]{ 0 };
	public:
		DialogManager(void) noexcept {
			char Path[MAX_PATH];
			GetModuleFileName(NULL, Path, MAX_PATH);			// EXE�̂���t�H���_�̃p�X���擾
			SetCurrentDirectory(Path);							// �J�����g�f�B���N�g���̐ݒ�
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
			ofn.lpstrTitle = "�J��";
			return GetOpenFileName(&ofn);
		}
		bool			Save(void) noexcept {
			ofn.lpstrTitle = "�ۑ�";
			return GetSaveFileName(&ofn);
		}
		const auto* GetPath(void) noexcept {
			std::string str = strFile;
			return (str.find(cdir) != std::string::npos) ? &strFile[strlen(cdir) + 1] : strFile;
		}
	};

	// --------------------------------------------------------------------------------------------------
	// �N��
	// --------------------------------------------------------------------------------------------------
	// �N��
	static void CreateOurProcess(char* szCmd, DWORD flag, bool fWait) noexcept {
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		memset(&si, 0, sizeof(STARTUPINFO));
		memset(&pi, 0, sizeof(PROCESS_INFORMATION));
		si.cb = sizeof(STARTUPINFO);
		CreateProcess(NULL, szCmd, NULL, NULL, FALSE, flag, NULL, NULL, &si, &pi);
		if (fWait) WaitForSingleObject(pi.hProcess, INFINITE);	// �I����҂�.
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	// ���g�𑽏d�N��
	static void StartMe(void) noexcept {
		char Path[MAX_PATH];
		// EXE�̂���t�H���_�̃p�X���擾
		::GetModuleFileName(NULL, Path, MAX_PATH);
		CreateOurProcess(Path, SW_HIDE, false);
	}
	// �t�@�C�������݂��邩�`�F�b�N
	static bool IsFileExist(const char* Path) noexcept {
		return std::filesystem::is_regular_file(Path);
		//DXLIB_Init��ʂ��Ă���łȂ��Ɠ��삵�Ȃ�
		/*
		FILEINFO FileInfo;
		return (FileRead_findFirst(Path, &FileInfo) != (DWORD_PTR)InvalidID);
		//*/
	}
	// �N���b�v�{�[�h�ɉ摜���R�s�[
	static auto GetClipBoardGraphHandle(GraphHandle* RetHandle) noexcept {
		HWND  hwnd = GetMainWindowHandle();

		if (IsClipboardFormatAvailable(CF_BITMAP) == FALSE) {
			return false;
		}	// �w�肵���`���̃f�[�^���N���b�v�{�[�h�ɂ��邩��₢���킹��
		if (OpenClipboard(hwnd) == FALSE) {
			return false;
		}						// �I�[�v��
		HBITMAP hBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);					// �f�[�^�擾
		CloseClipboard();														// �N���[�Y
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

		RetHandle->CreateGraphFromBmp(&bmpInfo, bmpData);	// �n���h���ɕϊ�
		delete[] bmpData;
		return true;
	}

}
