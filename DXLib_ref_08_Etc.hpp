#pragma once
#include "DXLib_ref.h"

//���T�C�Y
#define x_r(p1) (int(p1) * DXDraw::Instance()->m_DispXSize / 1920)
#define y_r(p1) (int(p1) * DXDraw::Instance()->m_DispYSize / 1080)

namespace DXLib_ref {
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*�֐�																																		*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	//Matrix�ł̃C�[�W���O
	static void Easing_Matrix(MATRIX_ref* A, const MATRIX_ref& B, float ratio, EasingType EasingType) noexcept {
		VECTOR_ref basex = A->xvec();
		VECTOR_ref basey = A->yvec();
		VECTOR_ref basez = A->zvec();
		Easing(&basex, B.xvec(), ratio, EasingType);
		Easing(&basey, B.yvec(), ratio, EasingType);
		Easing(&basez, B.zvec(), ratio, EasingType);

		*A = MATRIX_ref::Axis1(
			basex.Norm(),
			basey.Norm(),
			basez.Norm()).GetRot();
	}
	//Matrix�ł̐��`�⊮
	static MATRIX_ref Lerp_Matrix(const MATRIX_ref& A, const MATRIX_ref& B, float Per) noexcept {
		return MATRIX_ref::Axis1(
			Lerp(A.xvec(), B.xvec(), Per).Norm(),
			Lerp(A.yvec(), B.yvec(), Per).Norm(),
			Lerp(A.zvec(), B.zvec(), Per).Norm());
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
	static bool GetHitCapsuleToTriangle(const VECTOR_ref& startpos, const VECTOR_ref& endpos, float size, const VECTOR_ref& tri_p1, const VECTOR_ref& tri_p2, const VECTOR_ref& tri_p3) {
		return HitCheck_Capsule_Triangle(startpos.get(), endpos.get(), size, tri_p1.get(), tri_p2.get(), tri_p3.get()) == TRUE;
	}
	//�����Ɠ_�̈�ԋ߂��_
	static float GetMinLenSegmentToPoint(const VECTOR_ref& startpos, const VECTOR_ref& endpos, const VECTOR_ref& tgt) {
		return Segment_Point_MinLength(startpos.get(), endpos.get(), tgt.get());
	}
	//���ʂƓ_�̈�ԋ߂��_
	static VECTOR_ref GetMinPosSegmentToPoint(const VECTOR_ref& startpos, const VECTOR_ref& endpos, const VECTOR_ref& tgt) {
		return Plane_Point_MinLength_Position(startpos.get(), endpos.get(), tgt.get());
	}
	//�����_��
	static float GetRandf(float m_arg) noexcept { return -m_arg + (float)(GetRand((int)(m_arg * 2.f * 10000.f))) / 10000.f; }
	//�������m�̌�������
	static bool GetSegmenttoSegment(const VECTOR_ref &SegmentAPos1, const VECTOR_ref &SegmentAPos2, const VECTOR_ref &SegmentBPos1, const VECTOR_ref &SegmentBPos2, SEGMENT_SEGMENT_RESULT *Result) noexcept {
		VECTOR Pos1t = SegmentAPos1.get();
		VECTOR Pos2t = SegmentAPos2.get();
		VECTOR PosAt = SegmentBPos1.get();
		VECTOR PosBt = SegmentBPos2.get();

		Segment_Segment_Analyse(&Pos1t, &Pos2t, &PosAt, &PosBt, Result);
		float len = 0.001f;
		return (Result->SegA_SegB_MinDist_Square <= (len*len));
	}
	//�����Ɠ_�̍Őڋߓ_������͂���
	static bool GetSegmenttoPoint(const VECTOR_ref &SegmentAPos1, const VECTOR_ref &SegmentAPos2, const VECTOR_ref &PointPos, SEGMENT_POINT_RESULT *Result) noexcept {
		VECTOR Pos1t = SegmentAPos1.get();
		VECTOR Pos2t = SegmentAPos2.get();
		VECTOR PosAt = PointPos.get();

		Segment_Point_Analyse(&Pos1t, &Pos2t, &PosAt, Result);
		float len = 0.001f;
		return (Result->Seg_Point_MinDist_Square <= (len*len));
	}
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
	static void DrawEdgeBox_2D(int p1x, int p1y, int p2x, int p2y, const unsigned int& color, const unsigned int& EdgeColor) noexcept {
		int p = -1;
		DrawBox(p1x + p, p1y + p, p2x - p, p2y - p, EdgeColor, FALSE);
		p = 0;
		DrawBox(p1x + p, p1y + p, p2x - p, p2y - p, color, FALSE);
		p = 1;
		DrawBox(p1x + p, p1y + p, p2x - p, p2y - p, EdgeColor, FALSE);
	}
	//���ʂ��l�p
	static void DrawBoxLine_2D(int p1x, int p1y, int p2x, int p2y, const unsigned int& color, int thickness = 1) noexcept {
		DxLib::DrawLine(p1x, p1y, p1x, p2y, color, thickness);
		DxLib::DrawLine(p1x, p1y, p2x, p1y, color, thickness);
		DxLib::DrawLine(p1x, p2y, p2x, p2y, color, thickness);
		DxLib::DrawLine(p2x, p1y, p2x, p2y, color, thickness);
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
	static bool DrawCapsule_3D(const VECTOR_ref& p1, const VECTOR_ref& p2, float range, const unsigned int& color, const unsigned int& speccolor) noexcept {
		return DxLib::DrawCapsule3D(p1.get(), p2.get(), range, 8, color, speccolor, TRUE) == TRUE;
	}
	//���`��
	static bool DrawSphere_3D(const VECTOR_ref& p1, float range, const unsigned int& color, const unsigned int& speccolor) noexcept {
		return DxLib::DrawSphere3D(p1.get(), range, 8, color, speccolor, TRUE) == TRUE;
	}

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
		VECTOR_ref	m_WorldPos;
		VECTOR_ref	m_LocalPos;
	public:
		void			operator=(const frames& tgt) noexcept {
			this->m_FrameID = tgt.m_FrameID;
			this->m_WorldPos = tgt.m_WorldPos;
			this->m_LocalPos = tgt.m_LocalPos;
		}
		void			Set(int i, const MV1& obj) noexcept {
			m_FrameID = i;
			m_WorldPos = obj.frame(i);
			if (obj.frame_parent(i) >= 0) {
				m_LocalPos = obj.frame(i) - obj.frame((int)obj.frame_parent(i));
			}
			else {
				m_LocalPos = obj.frame(i);//
			}
		}
		const auto&		GetFrameID() const noexcept { return m_FrameID; }
		const auto&		GetFrameWorldPosition() const noexcept { return m_WorldPos; }
		const auto&		GetFrameLocalPosition() const noexcept { return m_LocalPos; }
	};
	//�ʒu���
	class moves {
	public:
		VECTOR_ref pos;		//���W
		VECTOR_ref repos;	//�O�t���[���̍��W
		VECTOR_ref vec;		//����
		MATRIX_ref mat;		//��]

		const MATRIX_ref MatIn(void) const noexcept { return mat * MATRIX_ref::Mtrans(pos); }

		void			operator=(const moves&tgt) {
			this->pos = tgt.pos;
			this->repos = tgt.repos;
			this->vec = tgt.vec;
			this->mat = tgt.mat;
		}

		void			SetPos(const VECTOR_ref& tgt) {
			this->repos = this->pos;
			this->pos = tgt;
		}
		void			Update_Physics(float speed_randam = 0.f, float rate = 1.f) {
			this->pos += this->vec*((float)((1000 - int(1000.f*speed_randam)) + GetRand(int(1000.f*speed_randam) * 2)) / 1000.f);
			this->vec.yadd(M_GR / powf((GetFPS() / rate), 2.f));

			//this->gun_m.pos += this->gun_m.vec;
			//this->gun_m.vec.yadd(M_GR / std::powf(GetFPS(), 2.f));
		}

		void			HitGround(const MV1_COLL_RESULT_POLY& colres, float hight) {//0.005f
			this->pos = VECTOR_ref(colres.HitPosition) + VECTOR_ref(colres.Normal)*hight;
			this->mat *= MATRIX_ref::RotVec2(this->mat.xvec(), VECTOR_ref(colres.Normal)*-1.f);
		}
	};

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*�V�F�[�_�[																																*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	//�V�F�[�_�[
	class shaders {
	public:
		class shader_Vertex {
			VERTEX3DSHADER Screen_vertex[6] = { 0.0f };
		public:
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
	private:
		int m_VertexShaderhandle{ -1 };
		int m_VertexShadercbhandle{ -1 };

		int m_PixelShaderhandle{ -1 };
		int m_PixelShadercbhandle{ -1 };
		int m_PixelShadercbhandle2{ -1 };
	public:
		void			Init(std::string VertexShader, std::string PixelShader) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) {
				return;
			}
			this->m_VertexShadercbhandle = CreateShaderConstantBuffer(sizeof(float) * 4);
			this->m_VertexShaderhandle = LoadVertexShader(("shader/" + VertexShader).c_str());		// ���_�V�F�[�_�[�o�C�i���R�[�h�̓ǂݍ���
			this->m_PixelShadercbhandle = CreateShaderConstantBuffer(sizeof(float) * 4);
			this->m_PixelShadercbhandle2 = CreateShaderConstantBuffer(sizeof(float) * 4);
			this->m_PixelShaderhandle = LoadPixelShader(("shader/" + PixelShader).c_str());			// �s�N�Z���V�F�[�_�[�o�C�i���R�[�h�̓ǂݍ���
		}
		//
		void			SetVertexParam(float param1, float param2, float param3, float param4) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) {
				return;
			}
			FLOAT4* f4 = (FLOAT4 *)GetBufferShaderConstantBuffer(this->m_VertexShadercbhandle);		// ���_�V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			UpdateShaderConstantBuffer(this->m_VertexShadercbhandle);								// ���_�V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
			SetShaderConstantBuffer(this->m_VertexShadercbhandle, DX_SHADERTYPE_VERTEX, 4);			// ���_�V�F�[�_�[�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^�S�ɃZ�b�g
		}
		//
		void			SetPixelDispSize(int dispx, int dispy) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) {
				return;
			}
			FLOAT2* dispsize = (FLOAT2*)GetBufferShaderConstantBuffer(this->m_PixelShadercbhandle);	// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
			dispsize->u = (float)dispx;
			dispsize->v = (float)dispy;
			UpdateShaderConstantBuffer(this->m_PixelShadercbhandle);								// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
			SetShaderConstantBuffer(this->m_PixelShadercbhandle, DX_SHADERTYPE_PIXEL, 2);			// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^2�ɃZ�b�g
		}
		void			SetPixelParam(float param1, float param2, float param3, float param4) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) {
				return;
			}
			FLOAT4* f4 = (FLOAT4*)GetBufferShaderConstantBuffer(this->m_PixelShadercbhandle2);		// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			UpdateShaderConstantBuffer(this->m_PixelShadercbhandle2);								// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
			SetShaderConstantBuffer(this->m_PixelShadercbhandle2, DX_SHADERTYPE_PIXEL, 3);			// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^3�ɃZ�b�g
		}

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
		void			Draw(shader_Vertex& Screenvertex) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) {
				return;
			}
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

		void ReadyDraw(const VECTOR_ref& Pos, const std::function<void()>& Doing) {
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
};
