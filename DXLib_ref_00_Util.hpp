#pragma once
#include "DXLib_ref.h"
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*DXLIB�ɒ��ڂ������̂Ȃ��֗����m																											*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/

namespace DXLibRef {
	//--------------------------------------------------------------------------------------------------
	//�V���O���g��
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
	//�q�̃T���v��
	/*
	class A : public SingletonBase<A> {
	private:
		friend class SingletonBase<A>;
	}
	//*/

	//--------------------------------------------------------------------------------------------------
	// �t�@�C������
	//--------------------------------------------------------------------------------------------------
	//�f�B���N�g�����̃t�@�C������
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
	//�t�@�C���I���_�C�A���O
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
			ofn.lpstrTitle = "�J��";
			return GetOpenFileName(&ofn);
		}
		bool			Save(void) noexcept {
			ofn.lpstrTitle = "�ۑ�";
			return GetSaveFileName(&ofn);
		}
		const auto*		GetPath(void) noexcept {
			std::string str = strFile;
			return (str.find(cdir) != std::string::npos) ? &strFile[strlen(cdir) + 1] : strFile;
		}
	};

	//--------------------------------------------------------------------------------------------------
	// �N��
	//--------------------------------------------------------------------------------------------------
	//�N��
	static void CreateOurProcess(char* szCmd, DWORD flag, bool fWait) noexcept {
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		memset(&si, 0, sizeof(STARTUPINFO));
		memset(&pi, 0, sizeof(PROCESS_INFORMATION));
		si.cb = sizeof(STARTUPINFO);
		CreateProcess(NULL, szCmd, NULL, NULL, FALSE, flag, NULL, NULL, &si, &pi);
		if (fWait) WaitForSingleObject(pi.hProcess, INFINITE);	//�I����҂�.
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	//���g�𑽏d�N��
	static void StartMe(void) noexcept {
		char Path[MAX_PATH];
		// EXE�̂���t�H���_�̃p�X���擾
		::GetModuleFileName(NULL, Path, MAX_PATH);
		CreateOurProcess(Path, SW_HIDE, false);
	}

	//--------------------------------------------------------------------------------------------------
	// �p�x�ϊ�
	//--------------------------------------------------------------------------------------------------
	//�p�x���烉�W�A����
	extern void* enabler;// �_�~�[�ϐ�
	template <class T, typename std::enable_if<std::is_arithmetic<T>::value>::type*& = enabler>
	constexpr float deg2rad(T p1) noexcept { return float(p1) * DX_PI_F / 180.f; }
	//���W�A������p�x��
	template <class T, typename std::enable_if<std::is_arithmetic<T>::value>::type*& = enabler>
	constexpr float rad2deg(T p1) noexcept { return float(p1) * 180.f / DX_PI_F; }

	//�]���藝
	constexpr float GetCosFormula(float a, float b, float c) noexcept {
		if (b + c > a && c + a > b && a + b > c) {
			return std::clamp((b * b + c * c - a * a) / (2.f * b*c), -1.f, 1.f);
		}
		return 1.f;
	}

	//--------------------------------------------------------------------------------------------------
	// �����ϊ�
	//--------------------------------------------------------------------------------------------------
	/*wstring��string�֕ϊ�*/
	static std::string WStringToString(std::wstring_view oWString) noexcept {
		// wstring �� SJIS
		int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, oWString.data(), int(oWString.size() + 1), nullptr, 0, NULL, NULL);
		// �o�b�t�@�̎擾
		CHAR* cpMultiByte = new CHAR[static_cast<size_t>(iBufferSize)];
		// wstring �� SJIS
		WideCharToMultiByte(CP_OEMCP, 0, oWString.data(), int(oWString.size() + 1), cpMultiByte, iBufferSize, NULL, NULL);
		// string�̐���
		std::string oRet(cpMultiByte, cpMultiByte + iBufferSize - 1);
		// �o�b�t�@�̔j��
		delete[] cpMultiByte;
		// �ϊ����ʂ�Ԃ�
		return oRet;
	}
	/*string��wstring�֕ϊ�����*/
	static std::wstring StringToWString(std::string_view oString) noexcept {
		// SJIS �� wstring
		int iBufferSize = MultiByteToWideChar(CP_ACP, 0, oString.data(), int(oString.size() + 1), nullptr, 0);
		// �o�b�t�@�̎擾
		wchar_t* cpUCS2 = new wchar_t[static_cast<size_t>(iBufferSize)];
		// SJIS �� wstring
		MultiByteToWideChar(CP_ACP, 0, oString.data(), int(oString.size() + 1), cpUCS2, iBufferSize);
		// string�̐���
		std::wstring oRet(cpUCS2, cpUCS2 + iBufferSize - 1);
		// �o�b�t�@�̔j��
		delete[] cpUCS2;
		// �ϊ����ʂ�Ԃ�
		return oRet;
	}
	//--------------------------------------------------------------------------------------------------
	// �t�H���g�t�@�C���Ǘ�
	//--------------------------------------------------------------------------------------------------
	class FontInstallClass {
		std::string		m_Path;
	public:
		void Install(LPCSTR font_path) noexcept {
			m_Path = font_path;
			if (AddFontResourceEx(m_Path.c_str(), FR_PRIVATE, NULL) == 0) {
				MessageBox(NULL, "�t�H���g�Ǎ����s", "", MB_OK);
			}
		}
		void Remove() noexcept {
			if (RemoveFontResourceEx(m_Path.c_str(), FR_PRIVATE, NULL) == 0) {
				MessageBox(NULL, "�t�H���g�Ǎ��폜", "", MB_OK);
			}
		}
	};
}
