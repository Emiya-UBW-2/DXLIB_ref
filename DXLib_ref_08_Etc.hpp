#pragma once
#include "DXLib_ref.h"

//���T�C�Y
#define x_r(p1) (int(p1) * DXDraw::Instance()->GetDispXSize() / 1920)
#define y_r(p1) (int(p1) * DXDraw::Instance()->GetDispYSize() / 1080)

#define EdgeSize	y_r(2)
#define LineHeight	y_r(18)

namespace DXLib_ref {
	//--------------------------------------------------------------------------------------------------
	// �⊮
	//--------------------------------------------------------------------------------------------------
	//�C�[�W���O
	enum class EasingType {
		OutExpo,
	};
	//���`�⊮
	extern float GetEasingRatio(EasingType EasingType, float ratio);
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
		*A = Lerp(*A, B, GetEasingRatio(EasingType, ratio));
	};
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*�֐�																																		*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	//Matrix�ł̐��`�⊮
	static Matrix4x4DX Lerp_Matrix(const Matrix4x4DX& A, const Matrix4x4DX& B, float Per) noexcept {
		return Matrix4x4DX::Axis1(
			Lerp(A.yvec(), B.yvec(), Per).normalized(),
			Lerp(A.zvec(), B.zvec(), Per).normalized(),
			Lerp(A.pos(), B.pos(), Per));
	}
	//Matrix�ł̃C�[�W���O
	static void Easing_Matrix(Matrix4x4DX* A, const Matrix4x4DX& B, float ratio, EasingType EasingType) noexcept {
		*A = Lerp_Matrix(*A, B, GetEasingRatio(EasingType, ratio));
	}
	//�N���b�v�{�[�h�ɉ摜���R�s�[
	static const auto GetClipBoardGraphHandle(GraphHandle* RetHandle) noexcept {
		HWND   hwnd = GetMainWindowHandle();

		if (IsClipboardFormatAvailable(CF_BITMAP) == FALSE) { return false; }	// �w�肵���`���̃f�[�^���N���b�v�{�[�h�ɂ��邩��₢���킹��
		if (OpenClipboard(hwnd) == FALSE) { return false; }						// �I�[�v��
		HBITMAP hBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);					// �f�[�^�擾
		CloseClipboard();														// �N���[�Y
		if (hBitmap == NULL) { return false; }

		BITMAP bitmap;
		GetObject(hBitmap, sizeof(BITMAP), &bitmap);

		BITMAPINFO bmpInfo;
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biWidth = bitmap.bmWidth;
		bmpInfo.bmiHeader.biHeight = bitmap.bmHeight;
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biBitCount = 32;
		bmpInfo.bmiHeader.biCompression = BI_RGB;

		size_t Siz = (size_t)(bitmap.bmWidth * bitmap.bmHeight * (LONG)4);
		BYTE *bmpData = new BYTE[Siz];
		HDC hDC = GetDC(hwnd);
		GetDIBits(hDC, hBitmap, 0, bitmap.bmHeight, (void*)bmpData, &bmpInfo, DIB_RGB_COLORS);
		ReleaseDC(hwnd, hDC);
		DeleteObject(hBitmap);

		int graphhandle = CreateGraphFromBmp(&bmpInfo, bmpData);	// �n���h���ɕϊ�
		delete[] bmpData;
		*RetHandle = graphhandle;
		return true;
	}
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*DXLIB���b�p�[																																*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	//--------------------------------------------------------------------------------------------------
	// ���Z�⏕
	//--------------------------------------------------------------------------------------------------
	//�O�p�ƃJ�v�Z���Ƃ̔���
	static bool GetHitCapsuleToTriangle(const Vector3DX& startpos, const Vector3DX& endpos, float size, const Vector3DX& tri_p1, const Vector3DX& tri_p2, const Vector3DX& tri_p3) {
		return HitCheck_Capsule_Triangle(startpos.get(), endpos.get(), size, tri_p1.get(), tri_p2.get(), tri_p3.get()) == TRUE;
	}
	//�����ƒ����̈�ԋ߂�����
	static float GetMinLenSegmentToSegment(const Vector3DX& startpos, const Vector3DX& endpos, const Vector3DX& tgtstartpos, const Vector3DX& tgtendpos) {
		return Segment_Segment_MinLength(startpos.get(), endpos.get(), tgtstartpos.get(), tgtendpos.get());
	}
	//�����Ɠ_�̈�ԋ߂��_
	static float GetMinLenSegmentToPoint(const Vector3DX& startpos, const Vector3DX& endpos, const Vector3DX& tgt) {
		return Segment_Point_MinLength(startpos.get(), endpos.get(), tgt.get());
	}
	//���ʂƓ_�̈�ԋ߂��_
	static Vector3DX GetMinPosSegmentToPoint(const Vector3DX& startpos, const Vector3DX& endpos, const Vector3DX& tgt) {
		return Plane_Point_MinLength_Position(startpos.get(), endpos.get(), tgt.get());
	}

	//�����_��
	static float GetRandf(float m_arg) noexcept { return -m_arg + (float)(GetRand((int)(m_arg * 2.f * 10000.f))) / 10000.f; }
	//�������m�̌�������
	static bool GetSegmenttoSegment(const Vector3DX &SegmentAPos1, const Vector3DX &SegmentAPos2, const Vector3DX &SegmentBPos1, const Vector3DX &SegmentBPos2, SEGMENT_SEGMENT_RESULT *Result) noexcept {
		VECTOR Pos1t = SegmentAPos1.get();
		VECTOR Pos2t = SegmentAPos2.get();
		VECTOR PosAt = SegmentBPos1.get();
		VECTOR PosBt = SegmentBPos2.get();

		Segment_Segment_Analyse(&Pos1t, &Pos2t, &PosAt, &PosBt, Result);
		float len = 0.001f;
		return (Result->SegA_SegB_MinDist_Square <= (len*len));
	}
	//�����Ɠ_�̍Őڋߓ_������͂���
	static bool GetSegmenttoPoint(const Vector3DX &SegmentAPos1, const Vector3DX &SegmentAPos2, const Vector3DX &PointPos, SEGMENT_POINT_RESULT *Result) noexcept {
		VECTOR Pos1t = SegmentAPos1.get();
		VECTOR Pos2t = SegmentAPos2.get();
		VECTOR PosAt = PointPos.get();

		Segment_Point_Analyse(&Pos1t, &Pos2t, &PosAt, Result);
		float len = 0.001f;
		return (Result->Seg_Point_MinDist_Square <= (len*len));
	}

	//�J���������ʏ�̍��W���擾
	static Vector3DX GetScreenPos(const Vector3DX&campos, const Vector3DX&camvec, const Vector3DX&camup, float fov, float near_t, float far_t, const Vector3DX&worldpos) noexcept;
	//--------------------------------------------------------------------------------------------------
	// �E�B���h�E�A�N�e�B�u�`�F�b�N�t���L�[����
	//--------------------------------------------------------------------------------------------------
	static int CheckHitKeyWithCheck(int KeyCode) noexcept {
		if (GetWindowActiveFlag()) {
			return CheckHitKey(KeyCode);
		}
		else {
			return 0;
		}
	}
	static int GetMouseInputWithCheck(void) noexcept {
		if (GetWindowActiveFlag()) {
			return GetMouseInput();
		}
		else {
			return 0;
		}
	}
	static int GetMouseWheelRotVolWithCheck(int CounterReset = TRUE) noexcept {
		if (GetWindowActiveFlag()) {
			return GetMouseWheelRotVol(CounterReset);
		}
		else {
			GetMouseWheelRotVol(CounterReset);
			return 0;
		}
	}
	//--------------------------------------------------------------------------------------------------
	// �`��
	//--------------------------------------------------------------------------------------------------
	//���̕`��
	static bool DrawLine_2D(int p1x, int p1y, int p2x, int p2y, const unsigned int& color, int thickness = 1) noexcept {
		return DxLib::DrawLine(p1x, p1y, p2x, p2y, color, thickness) == TRUE;
	}
	//���t���l�p
	static bool DrawBox_2D(int p1x, int p1y, int p2x, int p2y, const unsigned int& color, bool IsFill) noexcept {
		return DxLib::DrawBox(p1x, p1y, p2x, p2y, color, IsFill ? TRUE : FALSE) == TRUE;
	}
	//���ʂ��l�p
	static void DrawBoxLine_2D(int p1x, int p1y, int p2x, int p2y, const unsigned int& color, int thickness = 1) noexcept {
		DrawLine_2D(p1x, p1y, p1x, p2y, color, thickness);
		DrawLine_2D(p1x, p1y, p2x, p1y, color, thickness);
		DrawLine_2D(p1x, p2y, p2x, p2y, color, thickness);
		DrawLine_2D(p2x, p1y, p2x, p2y, color, thickness);
	}
	//�O���f�[�V�����̂����`��`��
	static void DrawGradationBox_2D(int x1, int y1, int x2, int y2, COLOR_U8 color1, COLOR_U8 color2, const unsigned char UorL = 255) noexcept {
		VERTEX2D Vertex[6];
		// ����̒��_�̏����Z�b�g
		Vertex[0].pos.x = (float)x1;
		Vertex[0].pos.y = (float)y1;
		Vertex[0].pos.z = 0.0f;
		Vertex[0].rhw = 1.0f;
		Vertex[0].dif = color1;
		Vertex[0].u = 0.0f;
		Vertex[0].v = 0.0f;

		// �E��̒��_�̏����Z�b�g
		Vertex[1].pos.x = (float)x2;
		Vertex[1].pos.y = (float)y1;
		Vertex[1].pos.z = 0.0f;
		Vertex[1].rhw = 1.0f;
		Vertex[1].dif = color2;
		Vertex[1].u = 0.0f;
		Vertex[1].v = 0.0f;

		Vertex[5] = Vertex[1];

		// �����̒��_�̏����Z�b�g
		Vertex[2].pos.x = (float)x1;
		Vertex[2].pos.y = (float)y2;
		Vertex[2].pos.z = 0.0f;
		Vertex[2].rhw = 1.0f;
		Vertex[2].dif = color1;
		Vertex[2].u = 0.0f;
		Vertex[2].v = 0.0f;

		Vertex[4] = Vertex[2];

		// �E���̒��_�̏����Z�b�g
		Vertex[3].pos.x = (float)x2;
		Vertex[3].pos.y = (float)y2;
		Vertex[3].pos.z = 0.0f;
		Vertex[3].rhw = 1.0f;
		Vertex[3].u = 0.0f;
		Vertex[3].v = 0.0f;

		switch (UorL)
		{
		case 3://��(�E)
			// ����̒��_�̏����Z�b�g
			Vertex[0].dif = color1;
			// �E��̒��_�̏����Z�b�g
			Vertex[1].dif = color2;
			Vertex[5] = Vertex[1];
			// �����̒��_�̏����Z�b�g
			Vertex[2].dif = color1;
			Vertex[4] = Vertex[2];
			// �E���̒��_�̏����Z�b�g
			Vertex[3].dif = color2;
			break;
		case 2://�c(��)
			// ����̒��_�̏����Z�b�g
			Vertex[0].dif = color1;
			// �E��̒��_�̏����Z�b�g
			Vertex[1].dif = color1;
			Vertex[5] = Vertex[1];
			// �����̒��_�̏����Z�b�g
			Vertex[2].dif = color2;
			Vertex[4] = Vertex[2];
			// �E���̒��_�̏����Z�b�g
			Vertex[3].dif = color2;
			break;
		case 1://��(��)
			// ����̒��_�̏����Z�b�g
			Vertex[0].dif = color2;
			// �E��̒��_�̏����Z�b�g
			Vertex[1].dif = color1;
			Vertex[5] = Vertex[1];
			// �����̒��_�̏����Z�b�g
			Vertex[2].dif = color2;
			Vertex[4] = Vertex[2];
			// �E���̒��_�̏����Z�b�g
			Vertex[3].dif = color1;
			break;
		case 0://�c(��)
			// ����̒��_�̏����Z�b�g
			Vertex[0].dif = color2;
			// �E��̒��_�̏����Z�b�g
			Vertex[1].dif = color2;
			Vertex[5] = Vertex[1];
			// �����̒��_�̏����Z�b�g
			Vertex[2].dif = color1;
			Vertex[4] = Vertex[2];
			// �E���̒��_�̏����Z�b�g
			Vertex[3].dif = color1;
			break;
		default:
			// ����̒��_�̏����Z�b�g
			Vertex[0].dif = color1;
			// �E��̒��_�̏����Z�b�g
			Vertex[1].dif = color1;
			Vertex[5] = Vertex[1];
			// �����̒��_�̏����Z�b�g
			Vertex[2].dif = color1;
			Vertex[4] = Vertex[2];
			// �E���̒��_�̏����Z�b�g
			Vertex[3].dif = color1;
			break;
		}
		// �|���S����2�`��
		DrawPolygon2D(Vertex, 2, DX_NONE_GRAPH, FALSE);
	}
	//�J�v�Z���`��
	static bool DrawCapsule_3D(const Vector3DX& p1, const Vector3DX& p2, float range, const unsigned int& color, const unsigned int& speccolor) noexcept {
		return DxLib::DrawCapsule3D(p1.get(), p2.get(), range, 8, color, speccolor, TRUE) == TRUE;
	}
	//���`��
	static bool DrawSphere_3D(const Vector3DX& p1, float range, const unsigned int& color, const unsigned int& speccolor) noexcept {
		return DxLib::DrawSphere3D(p1.get(), range, 8, color, speccolor, TRUE) == TRUE;
	}

	//�J���[�w��
	static const unsigned int Red{GetColor(255, 0, 0)};

	static const unsigned int Green{GetColor(0, 255, 0)};//GetColor(43, 255, 91)
	static const unsigned int DarkGreen{GetColor(0, 64, 0)};//GetColor(21, 128, 45)

	static const unsigned int Blue{GetColor(0, 0, 255)};

	static const unsigned int Yellow{GetColor(255, 255, 0)};

	static const unsigned int WhiteSel{GetColor(216, 255, 216)};

	static const unsigned int White{GetColor(255, 255, 255)};
	static const unsigned int Gray15{GetColor(216, 216, 216)};
	static const unsigned int Gray25{GetColor(192, 192, 192)};
	static const unsigned int Gray50{GetColor(128, 128, 128)};
	static const unsigned int Gray65{GetColor(96, 96, 96)};
	static const unsigned int Gray75{GetColor(64, 64, 64)};
	static const unsigned int Black{GetColor(0, 0, 0)};

	namespace WindowSystem {
		//��
		extern void SetBox(int xp1, int yp1, int xp2, int yp2, unsigned int colorSet);
		extern bool SetClickBox(int xp1, int yp1, int xp2, int yp2, unsigned int colorSet);
		//����
		template <typename... Args>
		extern const int GetMsgLen(int size, std::string_view String, Args&&... args) {
			auto* Fonts = FontPool::Instance();
			return Fonts->Get(FontPool::FontType::Nomal_EdgeL, size).GetStringWidth(-1, ((std::string)String).c_str(), args...) + y_r(6) + 2;//�G�b�W��:
		}

		const bool GetMsgPos(int* xp1, int *yp1, int xp2, int yp2, int size, int xSize, FontHandle::FontXCenter FontX);

		template <typename... Args>
		extern const int SetMsg(int xp1, int yp1, int xp2, int yp2, int size, FontHandle::FontXCenter FontX, unsigned int Color, unsigned int EdleColor, std::string_view String, Args&&... args) {
			if (String == "") { return 0; }
			auto* Fonts = FontPool::Instance();
			int xSize = GetMsgLen(size, String, args...);
			if (!GetMsgPos(&xp1, &yp1, xp2, yp2, size, xSize, FontX)) {
				return 0;
			}
			Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(size, FontX, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, EdleColor, ((std::string)String).c_str(), args...);
			return xSize;//�G�b�W��
		};
		//
		template <typename... Args>
		extern const int GetMsgLenWW(int size, std::string_view String, Args&&... args) {
			auto* Fonts = FontPool::Instance();
			return Fonts->Get(FontPool::FontType::WW_Gothic, size).GetStringWidth(-1, ((std::string)String).c_str(), args...) + y_r(6) + 2;//�G�b�W��:
		}

		template <typename... Args>
		extern const int SetMsgWW(int xp1, int yp1, int xp2, int yp2, int size, FontHandle::FontXCenter FontX, unsigned int Color, unsigned int EdleColor, std::string_view String, Args&&... args) {
			if (String == "") { return 0; }
			auto* Fonts = FontPool::Instance();
			int xSize = GetMsgLenWW(size, String, args...);
			if (!GetMsgPos(&xp1, &yp1, xp2, yp2, size, xSize, FontX)) {
				return 0;
			}
			Fonts->Get(FontPool::FontType::WW_Gothic).DrawString(size, FontX, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, EdleColor, ((std::string)String).c_str(), args...);
			return xSize;//�G�b�W��
		};
		//
		template <typename... Args>
		extern bool SetMsgClickBox(int xp1, int yp1, int xp2, int yp2, unsigned int defaultcolor, std::string_view String, Args&&... args) {
			bool ret = SetClickBox(xp1, yp1, xp2, yp2, defaultcolor);
			SetMsgWW(xp1, yp1, xp2, yp2, std::min(LineHeight, yp2 - yp1), FontHandle::FontXCenter::MIDDLE, White, Black, String, args...);
			return ret;
		};
		template <typename... Args>
		extern void SetMsgBox(int xp1, int yp1, int xp2, int yp2, unsigned int defaultcolor, std::string_view String, Args&&... args) {
			SetBox(xp1, yp1, xp2, yp2, defaultcolor);
			SetMsg(xp1, yp1, xp2, yp2, std::min(LineHeight, yp2 - yp1), FontHandle::FontXCenter::MIDDLE, White, Black, String, args...);
		};

		extern bool CheckBox(int xp1, int yp1, bool switchturn);

		extern int UpDownBar(int xmin, int xmax, int yp, int value, int valueMin, int valueMax);
		//
		/*
		class ScrollBoxClass {
			bool		m_IsChangeScrollY{ false };
			int			m_BaseScrollY{ 0 };
			float		m_NowScrollYPer{ 0.f };
		public:
			const auto&		GetNowScrollYPer(void) const noexcept { return this->m_NowScrollYPer; }
			void			ScrollBox(int xp1, int yp1, int xp2, int yp2, float TotalPer, bool IsActive) {
				auto* Pad = PadControl::Instance();
				unsigned int color = Gray25;

				int length = (int)((float)(yp2 - yp1) / TotalPer);
				float Total = (float)(yp2 - yp1 - length);
				int Yp_t = (int)(Total * this->m_NowScrollYPer);
				int Yp_s = std::max(yp1, yp1 + Yp_t);
				int Yp_e = std::min(yp2, Yp_s + length);

				if (IsActive) {
					if (in2_(Pad->GetMouseX(), Pad->GetMouseY(), xp1, yp1, xp2, yp2)) {
						if (Pad->GetWheelAdd() != 0.f) {
							m_NowScrollYPer = std::clamp(m_NowScrollYPer + (float)(-Pad->GetWheelAdd() * 3) / Total, 0.f, 1.f);
						}
					}
					if (in2_(Pad->GetMouseX(), Pad->GetMouseY(), xp2 - y_r(24), yp1, xp2, yp2)) {
						if (Pad->GetINTERACTKey().trigger()) {
							m_IsChangeScrollY = true;
						}

						if (!m_IsChangeScrollY) {
							HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
							SetCursor(hCursor);
						}
					}
					if (m_IsChangeScrollY) {
						if (Pad->GetINTERACTKey().press()) {
							color = White;
							m_NowScrollYPer = std::clamp((float)(Pad->GetMouseY() - this->m_BaseScrollY) / Total, 0.f, 1.f);

							HCURSOR hCursor = LoadCursor(NULL, IDC_SIZENS);
							SetCursor(hCursor);
						}
						else {
							m_IsChangeScrollY = false;
						}
					}
					else {
						m_BaseScrollY = Pad->GetMouseY() - Yp_t;
						if (Pad->GetMouseY() < Yp_s) {
							m_BaseScrollY += Yp_s - Pad->GetMouseY();
						}

						if (Pad->GetMouseY() > Yp_e) {
							m_BaseScrollY += Yp_e - Pad->GetMouseY();
						}
					}
				}
				SetBox(xp2 - y_r(24), yp1, xp2, yp2, Gray50);
				SetBox(xp2 - y_r(24) + y_r(1), Yp_s, xp2 - y_r(1), Yp_e, color);
			};
		};
		//*/
	};

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*�N���X																																	*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	//�����񂩂琔�l�����o��
	class getparams {
		static const int charLength = 512;
	public:
		//�����񂩂�=���E�̒l�擾
		static const std::string getleft(std::string tempname) { return tempname.substr(0, tempname.find('=')); }
		//�����񂩂�=���E�̒l�擾
		static const std::string getright(std::string tempname) { return tempname.substr(tempname.find('=') + 1); }
	public:
		//���ӂ̏����Ƃ�
		static const std::string		getcmd(int p1, int *p2) {
			char mstr[charLength]; /*tank*/
			FileRead_gets(mstr, charLength, p1);
			*p2 = std::stoi(getright(mstr));
			return getleft(mstr);
		}
		//1�s���̂܂܎擾
		static auto						Getstr(int p1) {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return std::string(mstr);
		}
		//=���E�̒l�擾
		static const std::string		_str(int p1) {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return getright(mstr);
		}
		static const long int			_int(int p1) {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return std::stoi(getright(mstr));
		}
		static const long int			_long(int p1) {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return std::stol(getright(mstr));
		}
		static const unsigned long int	_ulong(int p2) {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p2);
			return std::stoul(getright(mstr));
		}
		static const float				_float(int p1) {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return std::stof(getright(mstr));
		}
		static const bool				_bool(int p1) {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return (getright(mstr).find("true") != std::string::npos);
		}
	};
	//���f���̃t���[�����ێ�
	class frames {
		int			m_FrameID{ -1 };
		Matrix4x4DX	m_WorldPos;
		Matrix4x4DX	m_LocalPos;
	public:
		void			operator=(const frames& tgt) noexcept {
			this->m_FrameID = tgt.m_FrameID;
			this->m_WorldPos = tgt.m_WorldPos;
			this->m_LocalPos = tgt.m_LocalPos;
		}
		void			Set(int i, const MV1& obj) noexcept {
			m_FrameID = i;
			m_WorldPos = obj.GetFrameLocalWorldMatrix(i);
			if (obj.frame_parent(i) >= 0) {
				m_LocalPos = obj.GetFrameLocalMatrix(i);
			}
			else {
				m_LocalPos = obj.GetFrameLocalWorldMatrix(i);//
			}
		}
		const auto&		GetFrameID() const noexcept { return m_FrameID; }
		const auto&		GetFrameWorldPosition() const noexcept { return m_WorldPos; }
		const auto&		GetFrameLocalPosition() const noexcept { return m_LocalPos; }
	};
	//�ʒu���
	class moves {
	public:
		Vector3DX pos;		//���W
		Vector3DX posbuf;	//���W
		Vector3DX repos;	//�O�t���[���̍��W
		float		Speed{0.f};
		Vector3DX vec;		//����
		Matrix4x4DX mat;		//��]
		Vector3DX rad;		//��]

		const Matrix4x4DX MatIn(void) const noexcept { return mat * Matrix4x4DX::Mtrans(pos); }

		void			operator=(const moves&tgt) {
			this->pos = tgt.pos;
			this->repos = tgt.repos;
			this->vec = tgt.vec;
			this->mat = tgt.mat;
			this->rad = tgt.rad;
		}
		const auto operator+(const moves& o) const noexcept {
			//mat,repos�Ɋւ��Ă͕K�v�ɉ�����1
			moves tmp;
			tmp.pos = this->pos + o.pos;
			tmp.vec = this->vec + o.vec;
			tmp.rad = this->rad + o.rad;
			return tmp;
		}
		const auto operator-(const moves& o) const noexcept {
			//mat,repos�Ɋւ��Ă͕K�v�ɉ�����1
			moves tmp;
			tmp.pos = this->pos - o.pos;
			tmp.vec = this->vec - o.vec;
			tmp.rad = this->rad - o.rad;

			return tmp;
		}
		const auto operator*(float per) const noexcept {
			//mat,repos�Ɋւ��Ă͕K�v�ɉ�����1
			moves tmp;
			tmp.pos = this->pos*per;
			tmp.vec = this->vec*per;
			tmp.rad = this->rad*per;
			return tmp;
		}


		void			UpdatePos(const Vector3DX& tgt) {
			this->repos = this->pos;
			this->pos = tgt;
		}
		void			UpdatePosBuf(const Vector3DX& tgt, float per) {
			this->posbuf = tgt;
			auto Repos = this->pos;
			Easing(&this->pos, this->posbuf, per, EasingType::OutExpo);
			this->Speed = (this->pos - Repos).magnitude();
			this->repos = this->posbuf;
		}

		void			Update_Physics(float speed_randam = 0.f, float rate = 1.f);

		void			HitGround(const MV1_COLL_RESULT_POLY& colres, float hight) {//0.005f
			this->pos = Vector3DX(colres.HitPosition) + Vector3DX(colres.Normal)*hight;
			this->mat *= Matrix4x4DX::RotVec2(this->mat.xvec(), Vector3DX(colres.Normal)*-1.f);
		}
	};
	//�L�[��������
	class switchs {
		bool		m_on{false};//�I���I�t����
		bool		m_press{false};//�I���I�t����
		bool		m_repeat{false};//�I���I�t����
		int8_t		m_presscount{0};//�v�b�V������
		float		m_repeatcount{30.f};//�v�b�V������
	public:
		switchs(void) noexcept {
			Set(false);
			m_presscount = 0;
			m_repeatcount = 30.f;
			m_press = false;
		};
		~switchs(void) noexcept {}
		//�g�p�O�̗p��
		void			Set(bool on) noexcept { m_on = on; }
		//�X�V
		void			Execute(bool key) noexcept;
		//�I���I�t�̎擾
		const bool on(void) const noexcept { return m_on; }
		//�������u��
		const bool trigger(void) const noexcept { return m_press && (m_presscount == 1); }
		//�����Ă����
		const bool press(void) const noexcept { return m_press; }
		//�����Ă����
		const bool repeat(void) const noexcept { return m_repeat; }
		//�������u��
		const bool release_trigger(void) const noexcept { return (!m_press) && (m_presscount == 1); }
		//�����Ă����
		const bool release(void) const noexcept { return !m_press; }
	};
	// 2�����U��q���Z
	class Pendulum2D {
		float	m_PendulumLength = 10.f;
		float	m_PendulumMass = 2.f;
		float	m_drag_coeff = 2.02f;

		float	m_rad = deg2rad(12.f);
		float	m_vel = 0.f;
	public:
		void Init(float Length, float N, float rad) {
			m_PendulumLength = Length;
			m_PendulumMass = N;
			m_rad = rad;
			m_vel = 0.f;
		}
		void Update();
	public:
		const auto GetRad() const noexcept { return this->m_rad; }
		void AddRad(float value) noexcept { this->m_rad += value; }
	};

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*�V�F�[�_�[																																*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	//�V�F�[�_�[���g�p����ۂ̕⏕�N���X
	class ShaderUseClass {
	public:
		//2D�ɃV�F�[�_�[��K�p����ۂɎg�p�����ʃT�C�Y�̒��_���
		class ScreenVertex {
			VERTEX3DSHADER Screen_vertex[6] = {};
		public:
			// ���_�f�[�^�̎擾
			const auto*		GetScreenVertex() noexcept { return Screen_vertex; }
			// ���_�f�[�^�̏���
			void			SetScreenVertex(int dispx, int dispy) noexcept {
				int xp1 = 0;
				int yp1 = dispy;
				int xp2 = dispx;
				int yp2 = 0;
				Screen_vertex[0].pos = VGet((float)xp1, (float)yp1, 0.0f);
				Screen_vertex[1].pos = VGet((float)xp2, (float)yp1, 0.0f);
				Screen_vertex[2].pos = VGet((float)xp1, (float)yp2, 0.0f);
				Screen_vertex[3].pos = VGet((float)xp2, (float)yp2, 0.0f);
				Screen_vertex[0].dif = GetColorU8(255, 255, 255, 255);
				Screen_vertex[1].dif = GetColorU8(255, 255, 255, 255);
				Screen_vertex[2].dif = GetColorU8(255, 255, 255, 255);
				Screen_vertex[3].dif = GetColorU8(255, 255, 255, 255);
				Screen_vertex[0].u = 0.0f; Screen_vertex[0].v = 0.0f;
				Screen_vertex[1].u = 1.0f; Screen_vertex[1].v = 0.0f;
				Screen_vertex[2].u = 0.0f; Screen_vertex[3].v = 1.0f;
				Screen_vertex[3].u = 1.0f; Screen_vertex[2].v = 1.0f;
				Screen_vertex[4] = Screen_vertex[2];
				Screen_vertex[5] = Screen_vertex[1];
			}
		};

		// �e�p�̐[�x�L�^�摜���쐬�����ۂ̃J�����̃r���[�s��Ǝˉe�s��
		struct LIGHTCAMERA_MATRIX {
			MATRIX ViewMatrix;
			MATRIX ProjectionMatrix;
		};
	private:
		//�V�F�[�_�[�n���h��
		int m_VertexShaderhandle{-1};
		int m_PixelShaderhandle{-1};
		//�V�F�[�_�[�ɓn���ǉ��p�����[�^�[��z����n���h��
		int LightCameraMatrixConstantBufferHandle;	// �e�p�̐[�x�L�^�摜���쐬�����ۂ̃J�����̃r���[�s��Ǝˉe�s���ݒ肷�邽�߂̒萔�o�b�t�@
		std::array<int, 4> m_VertexShadercbhandle{-1};
		int m_PixelShaderSendDispSizeHandle{-1};
		std::array<int, 4> m_PixelShadercbhandle{-1};

	public:
		ShaderUseClass() {
			//�V�F�[�_�[�n���h��
			m_VertexShaderhandle = -1;
			m_PixelShaderhandle = -1;
			//�V�F�[�_�[�ɓn���ǉ��p�����[�^�[��z����n���h��
			for (auto& h : m_VertexShadercbhandle) { h = -1; }
			m_PixelShaderSendDispSizeHandle = -1;
			for (auto& h : m_PixelShadercbhandle) { h = -1; }
		}
		~ShaderUseClass() {
			Dispose();
		}
	public:
		//������
		void			Init(const char* VertexShader, const char* PixelShader) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			//���_�V�F�[�_�[����
			for (auto& h : m_VertexShadercbhandle) {
				h = CreateShaderConstantBuffer(sizeof(float) * 4);
			}
			LightCameraMatrixConstantBufferHandle = CreateShaderConstantBuffer(sizeof(LIGHTCAMERA_MATRIX));		// �e�p�̐[�x�L�^�摜���쐬�����ۂ̃J�����̃r���[�s��Ǝˉe�s���ݒ肷�邽�߂̒萔�o�b�t�@�̍쐬
			this->m_VertexShaderhandle = LoadVertexShader(VertexShader);		// ���_�V�F�[�_�[�o�C�i���R�[�h�̓ǂݍ���
			//�s�N�Z���V�F�[�_�\����
			this->m_PixelShaderSendDispSizeHandle = CreateShaderConstantBuffer(sizeof(float) * 4);
			for (auto& h : m_PixelShadercbhandle) {
				h = CreateShaderConstantBuffer(sizeof(float) * 4);
			}
			this->m_PixelShaderhandle = LoadPixelShader(PixelShader);			// �s�N�Z���V�F�[�_�[�o�C�i���R�[�h�̓ǂݍ���
		}
		//��n��
		void			Dispose() noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			//���_�V�F�[�_�[����
			DeleteShaderConstantBuffer(this->LightCameraMatrixConstantBufferHandle);
			for (auto& h : m_VertexShadercbhandle) {
				DeleteShaderConstantBuffer(h);
			}
			DeleteShader(this->m_VertexShaderhandle);
			//�s�N�Z���V�F�[�_�\����
			DeleteShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle);
			for (auto& h : m_PixelShadercbhandle) {
				DeleteShaderConstantBuffer(h);
			}
			DeleteShader(this->m_PixelShaderhandle);
		}
	public:
		//���_�V�F�[�_�\��Slot�Ԗڂ̃��W�X�^�ɏ����Z�b�g(Slot>=4)
		void			SetVertexCameraMatrix(int Slot) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			// �ݒ肵���J�����̃r���[�s��Ǝˉe�s����擾���Ă���
			LIGHTCAMERA_MATRIX* LightCameraMatrixConst = (LIGHTCAMERA_MATRIX*)GetBufferShaderConstantBuffer(LightCameraMatrixConstantBufferHandle);
			LightCameraMatrixConst->ViewMatrix = GetCameraViewMatrix();
			LightCameraMatrixConst->ProjectionMatrix = GetCameraProjectionMatrix();
			UpdateShaderConstantBuffer(LightCameraMatrixConstantBufferHandle);
			SetShaderConstantBuffer(LightCameraMatrixConstantBufferHandle, DX_SHADERTYPE_VERTEX, Slot);		// �e�p�[�x�L�^�摜��`�悵���Ƃ��̃J�����̃r���[�s��Ǝˉe�s���萔�ɐݒ肷��
		}
		//���_�V�F�[�_�\��Slot�Ԗڂ̃��W�X�^�ɏ����Z�b�g(Slot>=4)
		void			SetVertexParam(int Slot, float param1, float param2, float param3, float param4) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			FLOAT4* f4 = (FLOAT4 *)GetBufferShaderConstantBuffer(this->m_VertexShadercbhandle[0]);		// ���_�V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			UpdateShaderConstantBuffer(this->m_VertexShadercbhandle[0]);								// ���_�V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
			SetShaderConstantBuffer(this->m_VertexShadercbhandle[0], DX_SHADERTYPE_VERTEX, Slot);		// ���_�V�F�[�_�[�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^�S�ɃZ�b�g
		}
		//�s�N�Z���V�F�[�_�\��2�Ԗڂ̃��W�X�^�ɉ�ʃT�C�Y�̏����Z�b�g
		void			SetPixelDispSize(int dispx, int dispy) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			FLOAT2* dispsize = (FLOAT2*)GetBufferShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle);	// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
			dispsize->u = (float)dispx;
			dispsize->v = (float)dispy;
			UpdateShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle);									// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
			SetShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle, DX_SHADERTYPE_PIXEL, 2);				// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^2�ɃZ�b�g
		}
		//�s�N�Z���V�F�[�_�\��Slot�Ԗڂ̃��W�X�^�ɏ����Z�b�g(Slot>=3)
		void			SetPixelParam(int Slot, float param1, float param2, float param3, float param4) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			FLOAT4* f4 = (FLOAT4*)GetBufferShaderConstantBuffer(this->m_PixelShadercbhandle[0]);				// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			UpdateShaderConstantBuffer(this->m_PixelShadercbhandle[0]);											// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
			SetShaderConstantBuffer(this->m_PixelShadercbhandle[0], DX_SHADERTYPE_PIXEL, Slot);					// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^3�ɃZ�b�g
		}
		//3D��ԂɓK�p����ꍇ�̊֐�(������3D�`��̃����_������)
		void			Draw_lamda(std::function<void()> doing) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) {
				doing();
				return;
			}
			SetUseVertexShader(this->m_VertexShaderhandle);											// �g�p���钸�_�V�F�[�_�[���Z�b�g
			SetUsePixelShader(this->m_PixelShaderhandle);											// �g�p����s�N�Z���V�F�[�_�[���Z�b�g
			MV1SetUseOrigShader(TRUE);
			doing();
			MV1SetUseOrigShader(FALSE);
			SetUseVertexShader(-1);
			SetUsePixelShader(-1);
		}
		//2D�摜�ɓK�p����ꍇ�̊֐�
		void			Draw(ScreenVertex& Screenvertex) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			Draw_lamda([&] {DrawPolygon3DToShader(Screenvertex.GetScreenVertex(), 2); });
		}
	};
	//�V�F�[�_�[�p�����[�^�[
	struct shaderparam {
		bool			use{ false };
		float			param[4]{ 0,0,0,0 };
	};
	//�L���[�u�}�b�v����
	class RealTimeCubeMap {
	private:
		int dynamicCubeTex;		// ���͂���鏬�������f���������f�肱�܂��邽�߂̕`��Ώۂɂł���L���[�u�}�b�v�e�N�X�`��
		VECTOR lookAt[6];	// �f�肱�ގ��͂̊���`�悷��ۂ̃J�����̒����_
		VECTOR up[6];		// �ڂ肱�ގ��͂̊���`�悷��ۂ̃J�����̏����
		int MIPLEVEL = 2;
	public:
		void Init() {
			// �`��Ώۂɂł���L���[�u�}�b�v�e�N�X�`�����쐬
			SetCreateDrawValidGraphMipLevels(MIPLEVEL);
			SetCubeMapTextureCreateFlag(TRUE);
			dynamicCubeTex = MakeScreen(512, 512, TRUE);
			SetCubeMapTextureCreateFlag(FALSE);
			SetCreateDrawValidGraphMipLevels(0);
			// �f�肱�ފ���`�悷��ۂɎg�p����J�����̒����_�ƃJ�����̏������ݒ�
			lookAt[0] = VGet(1.0f, 0.0f, 0.0f);
			lookAt[1] = VGet(-1.0f, 0.0f, 0.0f);
			lookAt[2] = VGet(0.0f, 1.0f, 0.0f);
			lookAt[3] = VGet(0.0f, -1.0f, 0.0f);
			lookAt[4] = VGet(0.0f, 0.0f, 1.0f);
			lookAt[5] = VGet(0.0f, 0.0f, -1.0f);
			up[0] = VGet(0.0f, 1.0f, 0.0f);
			up[1] = VGet(0.0f, 1.0f, 0.0f);
			up[2] = VGet(0.0f, 0.0f, -1.0f);
			up[3] = VGet(0.0f, 0.0f, 1.0f);
			up[4] = VGet(0.0f, 1.0f, 0.0f);
			up[5] = VGet(0.0f, 1.0f, 0.0f);
		}

		void ReadyDraw(const Vector3DX& Pos, const std::function<void()>& Doing) {
			for (int i = 0; i < 6; i++) {		// �f�肱�ފ���`�悷��ʂ̐������J��Ԃ�
				for (int j = 0; j < MIPLEVEL; j++) {			// �~�b�v�}�b�v�̐������J��Ԃ�
					SetRenderTargetToShader(0, dynamicCubeTex, i, j);		// �`���ԍ��O�Ԃ̕`��Ώۂ�`��Ώۂɂł���L���[�u�}�b�v��i�Ԗڂ̖ʂɐݒ�
					ClearDrawScreen();										// �N���A
					{
						SetupCamera_Perspective(90.0f / 180.0f * DX_PI_F);								// �J�����̉�p��90�x�ɐݒ�
						SetCameraNearFar(0.5f, 1000.0f);												// Near�N���b�v�ʂ�Far�N���b�v�ʂ̋�����ݒ�
						SetCameraPositionAndTargetAndUpVec(Pos.get(), (Pos + lookAt[i]).get(), up[i]);	// �J�����̈ʒu�ƒ����_�A�J�����̏������ݒ�
						Doing();
					}
				}
			}
		}

		auto GetCubeMapTex() { return dynamicCubeTex; }
	};


	//--------------------------------------------------------------------------------------------------
	// �ėp�Z�[�u�f�[�^
	//--------------------------------------------------------------------------------------------------
	typedef std::pair<std::string, int64_t> SaveParam;
	class SaveDataClass : public SingletonBase<SaveDataClass> {
	private:
		friend class SingletonBase<SaveDataClass>;
	private:
		std::vector<SaveParam> m_data;
	private:
		SaveDataClass() {
			Load();
		}
		~SaveDataClass() {}
	public:
		SaveParam* GetData(std::string_view Name) noexcept {
			for (auto& d : m_data) {
				if (d.first == Name) {
					return &d;
				}
			}
			return nullptr;
		}
	public:
		void SetParam(std::string_view Name, int64_t value) noexcept {
			auto* Data = GetData(Name);
			if (Data) {
				Data->second = value;
			}
			else {
				m_data.emplace_back(std::make_pair((std::string)Name, value));
			}
		}
		auto GetParam(std::string_view Name) noexcept {
			auto* Data = GetData(Name);
			if (Data) {
				return Data->second;
			}
			return (int64_t)-1;
		}
	public:
		void Save() noexcept {
			std::ofstream outputfile("Save/new.svf");
			for (auto& d : m_data) {
				outputfile << d.first + "=" + std::to_string(d.second) + "\n";
			}
			outputfile.close();
		}
		void Load() noexcept {

			m_data.clear();

			std::ifstream inputputfile("Save/new.svf");
			std::string line;
			while (std::getline(inputputfile, line)) {
				auto Start = line.find("=");
				if (Start != std::string::npos) {
					m_data.emplace_back(std::make_pair(line.substr(0, Start), std::stoi(line.substr(Start + 1))));
				}
			}
			inputputfile.close();
		}
		void Reset() noexcept {
			m_data.clear();
		}
	};

	//--------------------------------------------------------------------------------------------------
	// �T�C�h���O
	//--------------------------------------------------------------------------------------------------

	class SideLog : public SingletonBase<SideLog> {
	private:
		friend class SingletonBase<SideLog>;
	private:
		class SideLogData {
			unsigned int m_Color{0};
			char m_Message[64]{};
			float m_TimeMax{-1.f};
			float m_Time{-1.f};
			float m_Flip{0.f};
			float m_Flip_Y{0.f};
		public:
			void AddFlip(float value) noexcept { m_Flip += value; }
			template <typename... Args>
			void SetData(float second, unsigned int Color, const char* Mes, Args&&... args) noexcept {
				snprintfDx(m_Message, 64, Mes, args...);
				m_TimeMax = second;
				m_Time = m_TimeMax;
				m_Flip = 0.f;
				m_Flip_Y = -1.f;
				m_Color = Color;
			}
			void UpdateActive() noexcept;
		public:
			const float GetFlip() { return m_Flip_Y; }
			const float ActivePer() { return (m_Time > 1.f) ? std::clamp((m_TimeMax - m_Time)*5.f + 0.1f, 0.f, 1.f) : std::clamp(m_Time, 0.f, 1.f); }
			const char* GetMsg() { return m_Message; }
			const unsigned int GetMsgColor() { return m_Color; }
		};
	private:
		std::array<SideLogData, 16> data;
		int LastSel{0};
	public:
		template <typename... Args>
		void AddLog(float second, unsigned int Color, const char* Mes, Args&&... args) noexcept {
			for (auto& d : data) {
				d.AddFlip(1.f);
			}
			data.at(LastSel).SetData(second, Color, Mes, args...);
			++LastSel %= ((int)data.size());
		}
		void Update() noexcept {
			for (auto& d : data) {
				if (d.ActivePer() > 0.f) {
					d.UpdateActive();
				}
			}
		}
		void Draw() noexcept;
	};

	//--------------------------------------------------------------------------------------------------
	// �|�b�v�A�b�v
	//--------------------------------------------------------------------------------------------------
	class PopUpDrawClass {
	private:
		bool m_Active{false};
		bool m_ActiveSwitch{false};
		float m_ActivePer{0.f};
		char m_WindwoName[64]{};

		int WinSizeX{720};
		int WinSizeY{720};

		std::function<void(int xsize,int ysize, bool EndSwitch)> m_Doing;
	public:
		PopUpDrawClass() {}
		~PopUpDrawClass() {}
	public:
		void			Set(const char* WindowName, int sizex, int sizey, std::function<void(int xsize, int ysize, bool EndSwitch)> doing) noexcept;
		void			Update(bool KeyTrigger) noexcept;
		void			Draw(void) noexcept;
	public:
		const auto& IsActive() const noexcept { return m_Active; }
		const auto& GetActiveSwitch() const noexcept { return m_ActiveSwitch; }
	};
};
