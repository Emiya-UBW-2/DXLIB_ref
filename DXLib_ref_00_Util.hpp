#pragma once
#include "DXLib_ref.h"
/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*DXLIB�ɒ��ڂ������̂Ȃ��֗����m																											*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/

//��`�Ɠ_�Ƃ̔���
#define in2_(mx, my, x1, y1, x2, y2) ((mx) >= (x1) && (mx) <= (x2) && (my) >= (y1) && (my) <= (y2))

namespace DXLib_ref {
	//--------------------------------------------------------------------------------------------------
	//�V���O���g��
	//--------------------------------------------------------------------------------------------------
	template <class T>
	class SingletonBase {
	private:
		static const T* m_Singleton;
	public:
		static void Create(void) {
			m_Singleton = new T();
		}
		static T* Instance(void) {
			if (m_Singleton == nullptr) {
				MessageBox(NULL, "Failed Instance Create", "", MB_OK);
				exit(-1);
			}
			//if (m_Singleton == nullptr) { m_Singleton = new T(); }
			return (T*)m_Singleton;
		}
	protected:
		SingletonBase() {}
		virtual ~SingletonBase() {}
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
	static const auto GetFileNamesInDirectory(std::string path_t) noexcept {
		std::vector<WIN32_FIND_DATA> data_t;
		WIN32_FIND_DATA win32fdt;
		HANDLE hFind = FindFirstFile((path_t + "*").c_str(), &win32fdt);
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (win32fdt.cFileName[0] != '.') {
					data_t.resize(data_t.size() + 1);
					data_t.back() = win32fdt;
				}

			} while (FindNextFile(hFind, &win32fdt));
		} //else{ return false; }
		FindClose(hFind);
		return data_t;
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
	static void CreateOurProcess(char* szCmd, DWORD flag, bool fWait) {
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
	static void StartMe(void) {
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
	inline static float deg2rad(T p1) {
		std::is_arithmetic<T>::value;
		return float(p1) * DX_PI_F / 180.f;
	}
	//���W�A������p�x��
	template <class T, typename std::enable_if<std::is_arithmetic<T>::value>::type*& = enabler>
	inline static float rad2deg(T p1) {
		return float(p1) * 180.f / DX_PI_F;
	}

	//�]���藝
	static float GetCosFormula(float a, float b, float c) {
		if (b + c > a && c + a > b && a + b > c) {
			return std::clamp((b * b + c * c - a * a) / (2.f * b*c), -1.f, 1.f);
		}
		return 1.f;
	}

	//--------------------------------------------------------------------------------------------------
	// �����ϊ�
	//--------------------------------------------------------------------------------------------------
	/*wstring��string�֕ϊ�*/
	static std::string WStringToString(std::wstring_view oWString) {
		// wstring �� SJIS
		int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, oWString.data(), int(oWString.size() + 1), nullptr, 0, NULL, NULL);
		// �o�b�t�@�̎擾
		CHAR* cpMultiByte = new CHAR[iBufferSize];
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
	static std::wstring StringToWString(std::string_view oString) {
		// SJIS �� wstring
		int iBufferSize = MultiByteToWideChar(CP_ACP, 0, oString.data(), int(oString.size() + 1), nullptr, 0);
		// �o�b�t�@�̎擾
		wchar_t* cpUCS2 = new wchar_t[iBufferSize];
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
	// �L�[�X�C�b�`
	//--------------------------------------------------------------------------------------------------
	//�L�[��������
	class switchs {
		bool		m_on{ false };//�I���I�t����
		uint8_t		m_press{ 0 };//�v�b�V������
	public:
		switchs(void) noexcept {
			Set(false);
			m_press = 0;
		};
		~switchs(void) noexcept { }
		//�g�p�O�̗p��
		void			Set(bool on) noexcept { m_on = on; }
		//�X�V
		void			Execute(bool key) noexcept {
			m_press = std::clamp<uint8_t>(m_press + 1, 0, (key ? 2 : 0));
			if (trigger()) {
				m_on ^= 1;
			}
		}
		//�I���I�t�̎擾
		const bool on(void) const noexcept { return m_on; }
		//�������u��
		const bool trigger(void) const noexcept { return m_press == 1; }
		//�����Ă����
		const bool press(void) const noexcept { return m_press != 0; }
		//�����Ă����
		const bool release(void) const noexcept { return m_press == 0; }
	};

	//--------------------------------------------------------------------------------------------------
	// �⊮
	//--------------------------------------------------------------------------------------------------
	//�C�[�W���O
	enum class EasingType {
		OutExpo,
	};
	//���`�⊮
	template <class T>
	static T Lerp(const T& A, const T& B, float Per) noexcept {
		if (Per == 0.f) {
			return A;
		}
		else if (Per == 1.f) {
			return B;
		}
		else {
			return A + (T)((B - A)*Per);
		}
	}
	template <class T>
	static void Easing(T* A, const T& B, float ratio, EasingType EasingType) {
		switch (EasingType) {
		case DXLib_ref::EasingType::OutExpo:
			*A = Lerp(*A, B, (1.f - std::powf(ratio, 60.f / GetFPS())));
			break;
		default:
			break;
		}
	};
	//--------------------------------------------------------------------------------------------------
	// �t�H���g�t�@�C���Ǘ�
	//--------------------------------------------------------------------------------------------------
	class FontInstallClass {
		DESIGNVECTOR	m_Font;
		std::string		m_Path;
	public:
		void Install(LPCSTR font_path) {
			m_Path = font_path;
			if (AddFontResourceEx(m_Path.c_str(), FR_PRIVATE, &m_Font) == 0) {
				MessageBox(NULL, "�t�H���g�Ǎ����s", "", MB_OK);
			}
		}
		void Remove() {
			if (RemoveFontResourceEx(m_Path.c_str(), FR_PRIVATE, &m_Font) == 0) {
				MessageBox(NULL, "�t�H���g�Ǎ��폜", "", MB_OK);
			}
		}
	};
}
