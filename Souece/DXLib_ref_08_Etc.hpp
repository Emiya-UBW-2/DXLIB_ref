#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// �⊮
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// �C�[�W���O
	enum class EasingType {
		OutExpo,
	};
	extern float GetEasingRatio(EasingType EasingType, float ratio) noexcept;
	// ���`�⊮
	template <class T>
	inline T Lerp(const T& A, const T& B, float Per) noexcept {
		if (Per == 0.f) {
			return A;
		}
		else if (Per == 1.f) {
			return B;
		}
		else {
			return A + (T)((B - A) * Per);
		}
	}
	// Matrix�ł̐��`�⊮
	inline Matrix4x4DX Lerp(const Matrix4x4DX& A, const Matrix4x4DX& B, float Per) noexcept {
		return Matrix4x4DX::Axis1(
			Lerp(A.yvec(), B.yvec(), Per).normalized(),
			Lerp(A.zvec(), B.zvec(), Per).normalized(),
			Lerp(A.pos(), B.pos(), Per));
	}
	inline Matrix3x3DX Lerp(const Matrix3x3DX& A, const Matrix3x3DX& B, float Per) noexcept {
		return Matrix3x3DX::Axis1(
			Lerp(A.yvec(), B.yvec(), Per).normalized(),
			Lerp(A.zvec(), B.zvec(), Per).normalized());
	}
	// 
	template <class T>
	static void Easing(T* A, const T& B, float ratio, EasingType EasingType) noexcept { *A = Lerp(*A, B, GetEasingRatio(EasingType, ratio)); }
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*�֐�																																		*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// �}�E�X�Ƌ�`�̔���
	extern bool IntoMouse(int x1, int y1, int x2, int y2) noexcept;
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*DXLIB���b�p�[																																*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// �����_��
	static float GetRandf(float m_arg) noexcept { return -m_arg + static_cast<float>(GetRand(static_cast<int>(m_arg * 2.f * 10000.f))) / 10000.f; }
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// IK
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	static void			IK_move_Arm(
		const Vector3DX& Localyvec,
		const Vector3DX& Localzvec,
		MV1* pObj,
		float XPer,
		int Arm, const Matrix4x4DX& FrameBaseLocalMatArm,
		int Arm2, const Matrix4x4DX& FrameBaseLocalMatArm2,
		int Wrist, const Matrix4x4DX& FrameBaseLocalMatWrist,
		const Vector3DX& DirPos, const Vector3DX& Diryvec, const Vector3DX& Dirzvec) noexcept {

		pObj->ResetFrameUserLocalMatrix(Arm);
		pObj->ResetFrameUserLocalMatrix(Arm2);
		pObj->ResetFrameUserLocalMatrix(Wrist);
		auto matBase = pObj->GetParentFrameWorldMatrix(Arm).rotation().inverse();

		// �
		auto vec_a1 = Matrix4x4DX::Vtrans((DirPos - pObj->GetFramePosition(Arm)).normalized(), matBase);// �
		auto vec_a1L1 = Vector3DX(Vector3DX::vget(XPer, -1.f, vec_a1.y / -abs(vec_a1.z))).normalized();// x=0�Ƃ���
		float cos_t = GetCosFormula((Vector3DX(pObj->GetFramePosition(Wrist)) - pObj->GetFramePosition(Arm2)).magnitude(), (Vector3DX(pObj->GetFramePosition(Arm2)) - pObj->GetFramePosition(Arm)).magnitude(), (Vector3DX(pObj->GetFramePosition(Arm)) - DirPos).magnitude());
		auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
		// ��r
		pObj->SetFrameLocalMatrix(Arm, Matrix4x4DX::identity() * FrameBaseLocalMatArm);
		Matrix4x4DX a1_inv = Matrix4x4DX::RotVec2(Matrix4x4DX::Vtrans(Vector3DX(pObj->GetFramePosition(Arm2)) - pObj->GetFramePosition(Arm), matBase), vec_t);
		pObj->SetFrameLocalMatrix(Arm, a1_inv * FrameBaseLocalMatArm);
		// ���r
		matBase = pObj->GetParentFrameWorldMatrix(Arm2).rotation().inverse();
		pObj->SetFrameLocalMatrix(Arm2, Matrix4x4DX::identity() * FrameBaseLocalMatArm2);
		Matrix4x4DX a2_inv = Matrix4x4DX::RotVec2(
			Matrix4x4DX::Vtrans(Vector3DX(pObj->GetFramePosition(Wrist)) - pObj->GetFramePosition(Arm2), matBase),
			Matrix4x4DX::Vtrans(DirPos - pObj->GetFramePosition(Arm2), matBase));
		pObj->SetFrameLocalMatrix(Arm2, a2_inv * FrameBaseLocalMatArm2);
		// ��
		matBase = pObj->GetParentFrameWorldMatrix(Wrist).rotation().inverse();
		Matrix4x4DX mat1;
		{
			auto zvec = Matrix4x4DX::Vtrans(Localzvec, pObj->GetFrameLocalWorldMatrix(Wrist).rotation());
			mat1 = Matrix4x4DX::RotVec2(Matrix4x4DX::Vtrans(zvec, matBase), Matrix4x4DX::Vtrans(Dirzvec, matBase)) * mat1;
			pObj->SetFrameLocalMatrix(Wrist, mat1 * FrameBaseLocalMatWrist);
			auto xvec = Matrix4x4DX::Vtrans(Localyvec, pObj->GetFrameLocalWorldMatrix(Wrist).rotation());
			mat1 = Matrix4x4DX::RotAxis(Localzvec, Vector3DX::Angle(xvec, Vector3DX::Cross(Dirzvec, Diryvec) * -1.f) * ((Vector3DX::Dot(Diryvec, xvec) > 0.f) ? -1.f : 1.f)) * mat1;
		}
		pObj->SetFrameLocalMatrix(Wrist, mat1 * FrameBaseLocalMatWrist);
	}

	static void			IK_RightArm(MV1* pObj,
		int Arm, const Matrix4x4DX& FrameBaseLocalMatArm,
		int Arm2, const Matrix4x4DX& FrameBaseLocalMatArm2,
		int Wrist, const Matrix4x4DX& FrameBaseLocalMatWrist,
		const Vector3DX& DirPos, const Vector3DX& Diryvec, const Vector3DX& Dirzvec) noexcept {
		IK_move_Arm(
			Vector3DX::vget(0.f, 0.f, -1.f).normalized(),
			Vector3DX::vget(-1.f, -1.f, 0.f).normalized(),
			pObj, -0.5f,
			Arm,
			FrameBaseLocalMatArm,
			Arm2,
			FrameBaseLocalMatArm2,
			Wrist,
			FrameBaseLocalMatWrist,
			DirPos, Diryvec * -1.f, Dirzvec);
	}
	static void			IK_LeftArm(MV1* pObj,
		int Arm, const Matrix4x4DX& FrameBaseLocalMatArm,
		int Arm2, const Matrix4x4DX& FrameBaseLocalMatArm2,
		int Wrist, const Matrix4x4DX& FrameBaseLocalMatWrist,
		const Vector3DX& DirPos, const Vector3DX& Diryvec, const Vector3DX& Dirzvec) noexcept {

		IK_move_Arm(
			Vector3DX::vget(0.f, 0.f, -1.f).normalized(),
			Vector3DX::vget(1.f, -1.f, 0.f).normalized(),
			pObj, 1.5f,
			Arm,
			FrameBaseLocalMatArm,
			Arm2,
			FrameBaseLocalMatArm2,
			Wrist,
			FrameBaseLocalMatWrist,
			DirPos, Diryvec, Dirzvec);
	}
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// �J���������ʏ�̍��W���擾
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	static Vector3DX GetScreenPos(const Vector3DX& campos, const Vector3DX& camvec, const Vector3DX& camup, float fov, float near_t, float far_t, const Vector3DX& worldpos) noexcept;
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*�N���X																																	*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// �����񂩂琔�l�����o��
	class getparams {
		static const int charLength = 512;
	public:
		// �����񂩂�=���E�̒l�擾
		static std::string getleft(std::string tempname) noexcept { return tempname.substr(0, tempname.find('=')); }
		// �����񂩂�=���E�̒l�擾
		static std::string getright(std::string tempname) noexcept { return tempname.substr(tempname.find('=') + 1); }
	public:
		// ���ӂ̏����Ƃ�
		static std::string		getcmd(int p1, int* p2) noexcept {
			char mstr[charLength]; /*tank*/
			FileRead_gets(mstr, charLength, p1);
			*p2 = std::stoi(getright(mstr));
			return getleft(mstr);
		}
		// 1�s���̂܂܎擾
		static auto						Getstr(int p1) noexcept {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return std::string(mstr);
		}
		// =���E�̒l�擾
		static std::string		_str(int p1) noexcept {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return getright(mstr);
		}
		static int			_int(int p1) noexcept {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return std::stoi(getright(mstr));
		}
		static long int			_long(int p1) noexcept {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return std::stol(getright(mstr));
		}
		static unsigned long int	_ulong(int p2) noexcept {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p2);
			return std::stoul(getright(mstr));
		}
		static float				_float(int p1) noexcept {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return std::stof(getright(mstr));
		}
		static bool				_bool(int p1) noexcept {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return (getright(mstr).find("true") != std::string::npos);
		}
	};
	// ���f���̃t���[�����ێ�
	class frames {
		int			m_FrameID{ InvalidID };
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
			if (obj.GetFrameParent(i) >= 0) {
				m_LocalPos = obj.GetFrameLocalMatrix(i);
			}
			else {
				m_LocalPos = obj.GetFrameLocalWorldMatrix(i);// 
			}
		}
		const auto& GetFrameID(void) const noexcept { return m_FrameID; }
		const auto& GetFrameWorldPosition(void) const noexcept { return m_WorldPos; }
		const auto& GetFrameLocalPosition(void) const noexcept { return m_LocalPos; }
	};

	// �ʒu���
	class moves {
		Vector3DX pos;		// ���f�p���W
		Vector3DX posbuf;	// ���Z�p���W
		Vector3DX repos;	// �O�t���[���̍��W
		Vector3DX vec;		// ����
		Matrix3x3DX mat;	// ��]
		Matrix3x3DX matbuf;	// ��]
	public:
		const auto& GetPos(void) const noexcept { return pos; }
		const auto& GetRePos(void) const noexcept { return repos; }
		const auto& GetVec(void) const noexcept { return vec; }
		const auto& GetMat(void) const noexcept { return mat; }

		const auto& GetPosBuf(void) const noexcept { return posbuf; }// ���Z�ȊO�ł͎g����
		const auto& GetMatBuf(void) const noexcept { return matbuf; }// ���Z�ȊO�ł͎g����
	public:
		void			SetPos(const Vector3DX& tgt) noexcept { this->posbuf = tgt; }
		void			SetVec(const Vector3DX& tgt) noexcept { this->vec = tgt; }
		void			SetMat(const Matrix3x3DX& tgt) noexcept { this->matbuf = tgt; }
	public:
		moves(void) noexcept {}
		moves(const moves& tgt) noexcept { *this = tgt; }
		moves(moves&& tgt) noexcept { *this = tgt; }
		// moves& operator=(const moves&) = delete;
		// moves& operator=(moves&& o) = delete;
		~moves(void) noexcept {}

		void			operator=(const moves& tgt) noexcept {
			this->pos = tgt.pos;
			this->posbuf = tgt.posbuf;
			this->repos = tgt.repos;
			this->vec = tgt.vec;
			this->mat = tgt.mat;
			this->matbuf = tgt.matbuf;
		}
	public:
		auto LerpMove(const moves& o, float Per) const noexcept {
			moves tmp;
			tmp.pos = Lerp(this->pos, o.pos, Per);
			tmp.posbuf = Lerp(this->posbuf, o.posbuf, Per);
			tmp.repos = Lerp(this->repos, o.repos, Per);
			tmp.vec = Lerp(this->vec, o.vec, Per);
			tmp.mat = Lerp(this->mat, o.mat, Per);
			tmp.matbuf = Lerp(this->matbuf, o.matbuf, Per);
			return tmp;
		}
	public:
		void			Init(const Vector3DX& Pos_t, const Vector3DX& Vec_t, const Matrix3x3DX& Mat_t) noexcept {
			this->pos = Pos_t;
			this->posbuf = Pos_t;
			this->repos = Pos_t;
			this->vec = Vec_t;
			this->mat = Mat_t;
			this->matbuf = Mat_t;
		}
		void			Update(float posper, float matper) noexcept {
			Easing(&this->pos, this->posbuf, posper, EasingType::OutExpo);
			this->repos = this->posbuf;
			Easing(&this->mat, this->matbuf, matper, EasingType::OutExpo);
		}
		void			SetAll(const Vector3DX& Pos_t, const Vector3DX& PosBuf_t, const Vector3DX& RePos_t, const Vector3DX& Vec_t, const Matrix3x3DX& Mat_t, const Matrix3x3DX& MatBuf_t) noexcept {
			this->pos = Pos_t;
			this->posbuf = PosBuf_t;
			this->repos = RePos_t;
			this->vec = Vec_t;
			this->mat = Mat_t;
			this->matbuf = MatBuf_t;
		}
	};

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*�V�F�[�_�[																																*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// �V�F�[�_�[���g�p����ۂ̕⏕�N���X
	class ShaderUseClass {
	public:
		// 2D�ɃV�F�[�_�[��K�p����ۂɎg�p�����ʃT�C�Y�̒��_���
		class ScreenVertex {
			VERTEX3DSHADER Screen_vertex[6] = {};
		public:
			// ���_�f�[�^�̎擾
			const auto* GetScreenVertex(void) noexcept { return Screen_vertex; }
			// ���_�f�[�^�̏���
			void			SetScreenVertex(int dispx, int dispy) noexcept {
				int xp1 = 0;
				int yp1 = dispy;
				int xp2 = dispx;
				int yp2 = 0;
				Screen_vertex[0].pos = VGet(static_cast<float>(xp1), static_cast<float>(yp1), 0.0f);
				Screen_vertex[1].pos = VGet(static_cast<float>(xp2), static_cast<float>(yp1), 0.0f);
				Screen_vertex[2].pos = VGet(static_cast<float>(xp1), static_cast<float>(yp2), 0.0f);
				Screen_vertex[3].pos = VGet(static_cast<float>(xp2), static_cast<float>(yp2), 0.0f);
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
		// DXLIB������������Ă����V�F�[�_�[�p�̒�`
		typedef float DX_D3D11_SHADER_FLOAT2[2];
		typedef float DX_D3D11_SHADER_FLOAT4[4];

		struct DX_D3D11_GS_CONST_BUFFER_BASE {
			DX_D3D11_SHADER_FLOAT4		ProjectionMatrix[4]{};											// �r���[�@���@�v���W�F�N�V�����s��
			DX_D3D11_SHADER_FLOAT4		ViewMatrix[3]{};												// ���[���h�@���@�r���[�s��
		};
		struct Wave {
			DX_D3D11_SHADER_FLOAT2 dir{};
			float amplitude{};
			float waveLength{};
		};
		struct ImmutableCB {
			Wave waves[20]{};
		};
	private:
		// �V�F�[�_�[�n���h��
		int m_VertexShaderhandle{ InvalidID };
		int m_GeometryShaderhandle{ InvalidID };
		int m_PixelShaderhandle{ InvalidID };
		// �V�F�[�_�[�ɓn���ǉ��p�����[�^�[��z����n���h��
		std::array<int, 4> LightCameraMatrixConstantBufferHandle{};	// �e�p�̐[�x�L�^�摜���쐬�����ۂ̃J�����̃r���[�s��Ǝˉe�s���ݒ肷�邽�߂̒萔�o�b�t�@
		std::array<int, 4> m_VertexShadercbhandle{};
		int m_GeometryShaderMatcbhandle{ InvalidID };
		int m_PixelShaderSendDispSizeHandle{ InvalidID };
		std::array<int, 4> m_PixelShadercbhandle{};
		ImmutableCB WaveData{};
		int m_VertexShadercbWaveDataHandle{ InvalidID };
	public:
		ShaderUseClass(void) noexcept {
			// �V�F�[�_�[�n���h��
			m_VertexShaderhandle = InvalidID;
			m_GeometryShaderhandle = InvalidID;
			m_PixelShaderhandle = InvalidID;
			// �V�F�[�_�[�ɓn���ǉ��p�����[�^�[��z����n���h��
			for (auto& h : m_VertexShadercbhandle) {
				h = InvalidID;
			}
			m_GeometryShaderMatcbhandle = InvalidID;
			m_PixelShaderSendDispSizeHandle = InvalidID;
			for (auto& h : m_PixelShadercbhandle) {
				h = InvalidID;
			}
		}
		~ShaderUseClass(void) noexcept {
			Dispose();
		}
	public:
		// ������
		void			Init(const char* VertexShader, const char* PixelShader) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			// ���_�V�F�[�_�[����
			for (auto& h : m_VertexShadercbhandle) {
				h = CreateShaderConstantBuffer(sizeof(float) * 4);
			}
			m_VertexShadercbWaveDataHandle = CreateShaderConstantBuffer(sizeof(ImmutableCB));
			// �e�p�̐[�x�L�^�摜���쐬�����ۂ̃J�����̃r���[�s��Ǝˉe�s���ݒ肷�邽�߂̒萔�o�b�t�@�̍쐬
			for (auto& h : LightCameraMatrixConstantBufferHandle) {
				h = CreateShaderConstantBuffer(sizeof(LIGHTCAMERA_MATRIX));
			}
			this->m_VertexShaderhandle = LoadVertexShader(VertexShader);		// ���_�V�F�[�_�[�o�C�i���R�[�h�̓ǂݍ���
			// �s�N�Z���V�F�[�_�\����
			this->m_PixelShaderSendDispSizeHandle = CreateShaderConstantBuffer(sizeof(float) * 4);
			for (auto& h : m_PixelShadercbhandle) {
				h = CreateShaderConstantBuffer(sizeof(float) * 4);
			}
			this->m_PixelShaderhandle = LoadPixelShader(PixelShader);			// �s�N�Z���V�F�[�_�[�o�C�i���R�[�h�̓ǂݍ���
		}
		void			AddGeometryShader(const char* GeometryShader) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			this->m_GeometryShaderMatcbhandle = CreateShaderConstantBuffer(sizeof(DX_D3D11_GS_CONST_BUFFER_BASE));
			this->m_GeometryShaderhandle = LoadGeometryShader(GeometryShader);
		}
		// ��n��
		void			Dispose(void) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			// ���_�V�F�[�_�[����
			for (auto& h : LightCameraMatrixConstantBufferHandle) {
				DeleteShaderConstantBuffer(h);
			}
			for (auto& h : m_VertexShadercbhandle) {
				DeleteShaderConstantBuffer(h);
			}
			DeleteShaderConstantBuffer(m_VertexShadercbWaveDataHandle);
			DeleteShader(this->m_VertexShaderhandle);
			// 
			DeleteShaderConstantBuffer(this->m_GeometryShaderMatcbhandle);
			DeleteShader(this->m_GeometryShaderhandle);
			// �s�N�Z���V�F�[�_�\����
			DeleteShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle);
			for (auto& h : m_PixelShadercbhandle) {
				DeleteShaderConstantBuffer(h);
			}
			DeleteShader(this->m_PixelShaderhandle);
		}
	public:
		// ���_�V�F�[�_�\��Slot�Ԗڂ̃��W�X�^�ɏ����Z�b�g(Slot>=4)
		void			SetVertexCameraMatrix(int Slot, const Matrix4x4DX& View, const Matrix4x4DX& Projection) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			// �ݒ肵���J�����̃r���[�s��Ǝˉe�s����擾���Ă���
			LIGHTCAMERA_MATRIX* LightCameraMatrixConst = (LIGHTCAMERA_MATRIX*)GetBufferShaderConstantBuffer(LightCameraMatrixConstantBufferHandle.at(static_cast<size_t>(Slot - 4)));
			LightCameraMatrixConst->ViewMatrix = View.get();
			LightCameraMatrixConst->ProjectionMatrix = Projection.get();

			UpdateShaderConstantBuffer(LightCameraMatrixConstantBufferHandle.at(static_cast<size_t>(Slot - 4)));
			SetShaderConstantBuffer(LightCameraMatrixConstantBufferHandle.at(static_cast<size_t>(Slot - 4)), DX_SHADERTYPE_VERTEX, Slot);		// �e�p�[�x�L�^�摜��`�悵���Ƃ��̃J�����̃r���[�s��Ǝˉe�s���萔�ɐݒ肷��
		}
		// ���_�V�F�[�_�\��Slot�Ԗڂ̃��W�X�^�ɏ����Z�b�g(Slot>=4)
		void			SetVertexParam(int Slot, float param1, float param2, float param3, float param4) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			FLOAT4* f4 = (FLOAT4*)GetBufferShaderConstantBuffer(this->m_VertexShadercbhandle[0]);		// ���_�V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			UpdateShaderConstantBuffer(this->m_VertexShadercbhandle[0]);								// ���_�V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
			SetShaderConstantBuffer(this->m_VertexShadercbhandle[0], DX_SHADERTYPE_VERTEX, Slot);		// ���_�V�F�[�_�[�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^�S�ɃZ�b�g
		}
		// 
		void CalcGWave() {
			for (int i : std::views::iota(0, 20)) {
				Wave& w = WaveData.waves[i];
				float randomRad = static_cast<float>(GetRand(30) * DX_PI_F * 2 * 0.3f);
				w.dir[0] = sinf(randomRad);
				w.dir[1] = cosf(randomRad);
				w.amplitude = (0.03f + powf(2.0f, static_cast<float>(GetRand(3)) * 2.0f) * 0.05f) * 0.05f * Scale3DRate;
				w.waveLength = 1.0f + powf(2.f, 1.f + static_cast<float>(GetRand(3))) * 10.f;
			}
		}
		// ���_�V�F�[�_�\��Slot�Ԗڂ̃��W�X�^�ɏ����Z�b�g(Slot>=4)
		void			SetVertexWave(void) const noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			ImmutableCB* f4 = (ImmutableCB*)GetBufferShaderConstantBuffer(this->m_VertexShadercbWaveDataHandle);		// ���_�V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
			*f4 = WaveData;
			UpdateShaderConstantBuffer(this->m_VertexShadercbWaveDataHandle);								// ���_�V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
			SetShaderConstantBuffer(this->m_VertexShadercbWaveDataHandle, DX_SHADERTYPE_VERTEX, 5);		// ���_�V�F�[�_�[�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^�S�ɃZ�b�g
		}
		// �V�F�[�_�\��Slot�Ԗڂ̃��W�X�^�ɏ����Z�b�g(Slot>=4)
		void			SetGeometryCONSTBUFFER(int Slot, const MATRIX* ViewMatrix, const MATRIX* ProjectionMatrix) const noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			if (this->m_GeometryShaderhandle == InvalidID) { return; }
			DX_D3D11_GS_CONST_BUFFER_BASE* LightCameraMatrixConst = (DX_D3D11_GS_CONST_BUFFER_BASE*)GetBufferShaderConstantBuffer(this->m_GeometryShaderMatcbhandle);

			// �r���[�ϊ��p�s����Z�b�g����
			LightCameraMatrixConst->ViewMatrix[0][0] = ViewMatrix->m[0][0];
			LightCameraMatrixConst->ViewMatrix[0][1] = ViewMatrix->m[1][0];
			LightCameraMatrixConst->ViewMatrix[0][2] = ViewMatrix->m[2][0];
			LightCameraMatrixConst->ViewMatrix[0][3] = ViewMatrix->m[3][0];
			LightCameraMatrixConst->ViewMatrix[1][0] = ViewMatrix->m[0][1];
			LightCameraMatrixConst->ViewMatrix[1][1] = ViewMatrix->m[1][1];
			LightCameraMatrixConst->ViewMatrix[1][2] = ViewMatrix->m[2][1];
			LightCameraMatrixConst->ViewMatrix[1][3] = ViewMatrix->m[3][1];
			LightCameraMatrixConst->ViewMatrix[2][0] = ViewMatrix->m[0][2];
			LightCameraMatrixConst->ViewMatrix[2][1] = ViewMatrix->m[1][2];
			LightCameraMatrixConst->ViewMatrix[2][2] = ViewMatrix->m[2][2];
			LightCameraMatrixConst->ViewMatrix[2][3] = ViewMatrix->m[3][2];
			// ���e�ϊ��p�s����Z�b�g����
			LightCameraMatrixConst->ProjectionMatrix[0][0] = ProjectionMatrix->m[0][0];
			LightCameraMatrixConst->ProjectionMatrix[0][1] = ProjectionMatrix->m[1][0];
			LightCameraMatrixConst->ProjectionMatrix[0][2] = ProjectionMatrix->m[2][0];
			LightCameraMatrixConst->ProjectionMatrix[0][3] = ProjectionMatrix->m[3][0];
			LightCameraMatrixConst->ProjectionMatrix[1][0] = ProjectionMatrix->m[0][1];
			LightCameraMatrixConst->ProjectionMatrix[1][1] = ProjectionMatrix->m[1][1];
			LightCameraMatrixConst->ProjectionMatrix[1][2] = ProjectionMatrix->m[2][1];
			LightCameraMatrixConst->ProjectionMatrix[1][3] = ProjectionMatrix->m[3][1];
			LightCameraMatrixConst->ProjectionMatrix[2][0] = ProjectionMatrix->m[0][2];
			LightCameraMatrixConst->ProjectionMatrix[2][1] = ProjectionMatrix->m[1][2];
			LightCameraMatrixConst->ProjectionMatrix[2][2] = ProjectionMatrix->m[2][2];
			LightCameraMatrixConst->ProjectionMatrix[2][3] = ProjectionMatrix->m[3][2];
			LightCameraMatrixConst->ProjectionMatrix[3][0] = ProjectionMatrix->m[0][3];
			LightCameraMatrixConst->ProjectionMatrix[3][1] = ProjectionMatrix->m[1][3];
			LightCameraMatrixConst->ProjectionMatrix[3][2] = ProjectionMatrix->m[2][3];
			LightCameraMatrixConst->ProjectionMatrix[3][3] = ProjectionMatrix->m[3][3];

			UpdateShaderConstantBuffer(this->m_GeometryShaderMatcbhandle);
			SetShaderConstantBuffer(this->m_GeometryShaderMatcbhandle, DX_SHADERTYPE_GEOMETRY, Slot);		// �e�p�[�x�L�^�摜��`�悵���Ƃ��̃J�����̃r���[�s��Ǝˉe�s���萔�ɐݒ肷��
		}
		// �s�N�Z���V�F�[�_�\��Slot�Ԗڂ̃��W�X�^�ɏ����Z�b�g(Slot>=4)
		void			SetPixelCameraMatrix(int Slot, const Matrix4x4DX& View, const Matrix4x4DX& Projection) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			// �ݒ肵���J�����̃r���[�s��Ǝˉe�s����擾���Ă���
			LIGHTCAMERA_MATRIX* LightCameraMatrixConst = (LIGHTCAMERA_MATRIX*)GetBufferShaderConstantBuffer(LightCameraMatrixConstantBufferHandle.at(static_cast<size_t>(Slot - 4)));
			LightCameraMatrixConst->ViewMatrix = View.get();
			LightCameraMatrixConst->ProjectionMatrix = Projection.get();

			UpdateShaderConstantBuffer(LightCameraMatrixConstantBufferHandle.at(static_cast<size_t>(Slot - 4)));
			SetShaderConstantBuffer(LightCameraMatrixConstantBufferHandle.at(static_cast<size_t>(Slot - 4)), DX_SHADERTYPE_PIXEL, Slot);		// �e�p�[�x�L�^�摜��`�悵���Ƃ��̃J�����̃r���[�s��Ǝˉe�s���萔�ɐݒ肷��
		}
		// �s�N�Z���V�F�[�_�\��2�Ԗڂ̃��W�X�^�ɉ�ʃT�C�Y�̏����Z�b�g
		void			SetPixelDispSize(int dispx, int dispy) const noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			FLOAT2* dispsize = (FLOAT2*)GetBufferShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle);	// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
			dispsize->u = static_cast<float>(dispx);
			dispsize->v = static_cast<float>(dispy);
			UpdateShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle);									// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
			SetShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle, DX_SHADERTYPE_PIXEL, 2);				// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^2�ɃZ�b�g
		}
		// �s�N�Z���V�F�[�_�\��Slot�Ԗڂ̃��W�X�^�ɏ����Z�b�g(Slot>=3)
		void			SetPixelParam(int Slot, float param1, float param2, float param3, float param4) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			FLOAT4* f4 = (FLOAT4*)GetBufferShaderConstantBuffer(this->m_PixelShadercbhandle.at(static_cast<size_t>(Slot - 3)));				// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			UpdateShaderConstantBuffer(this->m_PixelShadercbhandle.at(static_cast<size_t>(Slot - 3)));											// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
			SetShaderConstantBuffer(this->m_PixelShadercbhandle.at(static_cast<size_t>(Slot - 3)), DX_SHADERTYPE_PIXEL, Slot);					// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^3�ɃZ�b�g
		}
		// 3D��ԂɓK�p����ꍇ�̊֐�(������3D�`��̃����_������)
		void			Draw_lamda(std::function<void()> doing) const noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) {
				doing();
				return;
			}
			SetUseVertexShader(this->m_VertexShaderhandle);											// �g�p���钸�_�V�F�[�_�[���Z�b�g
			SetUsePixelShader(this->m_PixelShaderhandle);											// �g�p����s�N�Z���V�F�[�_�[���Z�b�g
			SetUseGeometryShader(this->m_GeometryShaderhandle);										// �g�p����W�I���g���V�F�[�_�[���Z�b�g
			MV1SetUseOrigShader(TRUE);
			doing();
			MV1SetUseOrigShader(FALSE);
			SetUseVertexShader(InvalidID);
			SetUsePixelShader(InvalidID);
			SetUseGeometryShader(InvalidID);
		}
		// 2D�摜�ɓK�p����ꍇ�̊֐�
		void			Draw(ScreenVertex& Screenvertex) const noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			Draw_lamda([&] {DrawPolygon3DToShader(Screenvertex.GetScreenVertex(), 2); });
		}
	};
	// �L���[�u�}�b�v����
	class RealTimeCubeMap {
	private:
		GraphHandle dynamicCubeTex;		// ���͂���鏬�������f���������f�肱�܂��邽�߂̕`��Ώۂɂł���L���[�u�}�b�v�e�N�X�`��
		Vector3DX lookAt[6]{};	// �f�肱�ގ��͂̊���`�悷��ۂ̃J�����̒����_
		Vector3DX up[6]{};		// �ڂ肱�ގ��͂̊���`�悷��ۂ̃J�����̏����
		int MIPLEVEL = 2;
	public:
		RealTimeCubeMap(void) noexcept {}
		RealTimeCubeMap(const RealTimeCubeMap&) = delete;
		RealTimeCubeMap(RealTimeCubeMap&& o) = delete;
		RealTimeCubeMap& operator=(const RealTimeCubeMap&) = delete;
		RealTimeCubeMap& operator=(RealTimeCubeMap&& o) = delete;

		~RealTimeCubeMap(void) noexcept {}
	public:
		void Init(void) noexcept {
			// �`��Ώۂɂł���L���[�u�}�b�v�e�N�X�`�����쐬
			SetCreateDrawValidGraphMipLevels(MIPLEVEL);
			SetCubeMapTextureCreateFlag(TRUE);
			dynamicCubeTex.Make(512, 512, true);
			SetCubeMapTextureCreateFlag(FALSE);
			SetCreateDrawValidGraphMipLevels(0);
			// �f�肱�ފ���`�悷��ۂɎg�p����J�����̒����_�ƃJ�����̏������ݒ�
			lookAt[0] = Vector3DX::right();
			lookAt[1] = Vector3DX::left();
			lookAt[2] = Vector3DX::up();
			lookAt[3] = Vector3DX::down();
			lookAt[4] = Vector3DX::forward();
			lookAt[5] = Vector3DX::back();
			up[0] = Vector3DX::up();
			up[1] = Vector3DX::up();
			up[2] = Vector3DX::back();
			up[3] = Vector3DX::forward();
			up[4] = Vector3DX::up();
			up[5] = Vector3DX::up();
		}

		void ReadyDraw(const Vector3DX& Pos, const std::function<void()>& Doing) noexcept {
			for (int i : std::views::iota(0, 6)) {		// �f�肱�ފ���`�悷��ʂ̐������J��Ԃ�
				for (int j : std::views::iota(0, MIPLEVEL)) {			// �~�b�v�}�b�v�̐������J��Ԃ�
					dynamicCubeTex.SetRenderTargetToShader(0, i, j);		// �`���ԍ��O�Ԃ̕`��Ώۂ�`��Ώۂɂł���L���[�u�}�b�v��i�Ԗڂ̖ʂɐݒ�
					ClearDrawScreen();										// �N���A
					{
						SetupCamera_Perspective(90.0f / 180.0f * DX_PI_F);								// �J�����̉�p��90�x�ɐݒ�
						SetCameraNearFar(0.5f * Scale3DRate, 1000.0f * Scale3DRate);									// Near�N���b�v�ʂ�Far�N���b�v�ʂ̋�����ݒ�
						SetCameraPositionAndTargetAndUpVec(Pos.get(), (Pos + lookAt[static_cast<size_t>(i)]).get(), up[static_cast<size_t>(i)].get());	// �J�����̈ʒu�ƒ����_�A�J�����̏������ݒ�
						Doing();
					}
				}
			}
		}

		void Dispose(void) noexcept {
			dynamicCubeTex.Dispose();
		}

		const auto& GetCubeMapTex(void) const noexcept { return dynamicCubeTex; }
	};

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// �ėp�Z�[�u�f�[�^
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	typedef std::pair<std::string, int64_t> SaveParam;
	class SaveDataClass : public SingletonBase<SaveDataClass> {
	private:
		friend class SingletonBase<SaveDataClass>;
	private:
		std::vector<SaveParam> m_data;
	private:
		SaveDataClass(void) noexcept {
			Load();
		}
		SaveDataClass(const SaveDataClass&) = delete;
		SaveDataClass(SaveDataClass&& o) = delete;
		SaveDataClass& operator=(const SaveDataClass&) = delete;
		SaveDataClass& operator=(SaveDataClass&& o) = delete;

		~SaveDataClass(void) noexcept {}
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
			return (int64_t)InvalidID;
		}
	public:
		void Save(void) noexcept {
			std::ofstream outputfile("Save/new.svf");
			for (auto& d : m_data) {
				outputfile << d.first + "=" + std::to_string(d.second) + "\n";
			}
			outputfile.close();
		}
		bool Load(void) noexcept {

			m_data.clear();

			std::ifstream inputputfile("Save/new.svf");
			if (!inputputfile) {
				return false;
			}
			std::string line;
			while (std::getline(inputputfile, line)) {
				auto Start = line.find("=");
				if (Start != std::string::npos) {
					m_data.emplace_back(std::make_pair(line.substr(0, Start), std::stoi(line.substr(Start + 1))));
				}
			}
			inputputfile.close();
			return true;
		}
		void Reset(void) noexcept {
			m_data.clear();
		}
	};

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// PC�̏��𑖍�
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class CheckPCSpec {
		struct MatchScore {
			std::string		m_Name;
			int				m_Score{ 0 };
			int				m_HitCount{ 0 };
		};
	private:
		bool IsEnd = false;
		std::vector<MatchScore> CPUResult;
		std::vector<MatchScore> GPUResult;

		TCHAR CPUString[256]{ "" };
		double FreeMemorySize{ 0 };
		double TotalMemorySize{ 0 };
		TCHAR GPUString[256]{ "" };

		std::array<std::string, 12> CPUStr;
		std::array<std::string, 12> GPUStr;
		std::thread m_thread;
	public:
		static void GetOnlyNumber(const char* Target, std::array<std::string, 12>* String) noexcept {
			int NumCount = 0;
			for (auto& s : *String) {
				s = "";
			}
			for (int i : std::views::iota(0, static_cast<int>(strlenDx(Target)))) {
				if (!
					(
						Target[static_cast<size_t>(i)] == ' ' ||
						Target[static_cast<size_t>(i)] == '-'
						)
					) {
					String->at(static_cast<size_t>(NumCount)) += Target[static_cast<size_t>(i)];
				}
				else {
					if (String->at(static_cast<size_t>(NumCount)) != "") {
						++NumCount;
						String->at(static_cast<size_t>(NumCount)) = "";
					}
				}
			}
		}
	public:
		void FindCPU(void) noexcept {
			IsEnd = false;
			std::vector<MatchScore> Tmp; Tmp.reserve(64);
			{
				CPUResult.clear();
				std::ifstream inputputfile("CommonData/PassMarkCPU.txt");
				if (inputputfile) {
					Tmp.clear();
					std::string ALL;
					std::array<std::string, 12> TmpString;
					int HitCount = 0;
					int border = 0;
					while (std::getline(inputputfile, ALL)) {
						if (ALL == "") {
							continue;
						}
						auto LEFT = getparams::getleft(ALL);
						auto RIGHT = getparams::getright(ALL);
						GetOnlyNumber(LEFT.c_str(), &TmpString);
						HitCount = 0;
						for (auto& s : CPUStr) {
							if (s == "") {
								continue;
							}
							for (auto& t : TmpString) {
								if (t == "") {
									continue;
								}
								if ((s.find(t) != std::string::npos) || (t.find(s) != std::string::npos)) {
									++HitCount;
								}
							}
						}
						if (HitCount > 0) {
							border = std::max(HitCount, border);
							Tmp.resize(Tmp.size() + 1);
							Tmp.back().m_Name = LEFT;
							Tmp.back().m_Score = std::stoi(RIGHT);
							Tmp.back().m_HitCount = HitCount;
						}
					}
					inputputfile.close();
					for (auto& n : Tmp) {
						if (n.m_HitCount >= border) {
							CPUResult.emplace_back(n);
						}
					}
				}
			}
			{
				GPUResult.clear();
				std::ifstream inputputfile("CommonData/PassMarkGPU.txt");
				if (inputputfile) {
					Tmp.clear();
					std::string ALL;
					std::array<std::string, 12> TmpString;
					int HitCount = 0;
					int border = 0;
					while (std::getline(inputputfile, ALL)) {
						if (ALL == "") {
							continue;
						}
						auto LEFT = getparams::getleft(ALL);
						auto RIGHT = getparams::getright(ALL);
						GetOnlyNumber(LEFT.c_str(), &TmpString);
						HitCount = 0;
						for (auto& s : GPUStr) {
							if (s == "") {
								continue;
							}
							for (auto& t : TmpString) {
								if (t == "") {
									continue;
								}
								if ((s.find(t) != std::string::npos) || (t.find(s) != std::string::npos)) {
									++HitCount;
								}
							}
						}
						if (HitCount > 0) {
							border = std::max(HitCount, border);
							Tmp.resize(Tmp.size() + 1);
							Tmp.back().m_Name = LEFT;
							Tmp.back().m_Score = std::stoi(RIGHT);
							Tmp.back().m_HitCount = HitCount;
						}
					}
					inputputfile.close();
					for (auto& n : Tmp) {
						if (n.m_HitCount >= border) {
							GPUResult.emplace_back(n);
						}
					}
				}
			}
			IsEnd = true;
		}
	public:
	public:
		CheckPCSpec(void) noexcept {}
		CheckPCSpec(const CheckPCSpec&) = delete;
		CheckPCSpec(CheckPCSpec&& o) = delete;
		CheckPCSpec& operator=(const CheckPCSpec&) = delete;
		CheckPCSpec& operator=(CheckPCSpec&& o) = delete;
		~CheckPCSpec(void) noexcept {
			if (m_thread.joinable()) {
				m_thread.detach();
			}
		}
	public:
		const auto* GetCPUDatas(void) const noexcept { return IsEnd ? &CPUResult : nullptr; }
		const auto* GetGPUDatas(void) const noexcept { return IsEnd ? &GPUResult : nullptr; }
		const auto& GetFreeMemorySize(void) const noexcept { return FreeMemorySize; }
		const auto& GetTotalMemorySize(void) const noexcept { return TotalMemorySize; }
	public:
		void Set(void) noexcept {
			GetPcInfo(NULL, NULL, CPUString, NULL, &FreeMemorySize, &TotalMemorySize, NULL, GPUString, NULL, NULL);
			GetOnlyNumber(CPUString, &CPUStr);
			GetOnlyNumber(GPUString, &GPUStr);
		}
		void StartSearch(void) noexcept {
			if (m_thread.joinable()) {
				m_thread.detach();
			}
			std::thread newThead(&CheckPCSpec::FindCPU, this);
			m_thread.swap(newThead);
		}
	};

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// �J�����V�F�C�N
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class CameraShake : public SingletonBase<CameraShake> {
	private:
		friend class SingletonBase<CameraShake>;
	private:
		bool						m_SendCamShake{ false };
		float						m_SendCamShakeTime{ 1.f };
		float						m_SendCamShakePower{ 1.f };
		float						m_CamShake{ 0.f };
		Vector3DX					m_CamShake1;
		Vector3DX					m_CamShake2;
	public:
		const auto& GetCamShake(void) const noexcept { return m_CamShake2; }
	public:
		void			SetCamShake(float time, float power) noexcept {
			this->m_SendCamShake = true;
			this->m_SendCamShakeTime = time;
			this->m_SendCamShakePower = power;
		}
	private:
		CameraShake(void) noexcept {}
		CameraShake(const CameraShake&) = delete;
		CameraShake(CameraShake&& o) = delete;
		CameraShake& operator=(const CameraShake&) = delete;
		CameraShake& operator=(CameraShake&& o) = delete;

		~CameraShake(void) noexcept {}
	public:
		void Update(void) noexcept;
	};

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*UDP�ʐM																																	*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class UDPNetWorkDX {
		int			m_Handle{ InvalidID };				// �l�b�g���[�N�n���h��
		IPDATA		m_SendIP{ 127,0,0,1 };		// ���M�p�h�o�A�h���X�f�[�^
		int			m_SendPort{ InvalidID };				// �ʐM�p�|�[�g
		IPDATA		m_RecvIp{ 127,0,0,1 };			// ��M�p�h�o�A�h���X�f�[�^
		int			m_RecvPort{ 0 };				// ��M�p�|�[�g
	public:
		auto			IsActive(void) const noexcept { return (m_Handle != InvalidID); }
	public:
		void			SetServerIP(const IPDATA& pIP) noexcept { m_SendIP = pIP; }// �N���C�A���g�͕K���s��
		bool			Init(bool IsServer, int PORT = InvalidID) noexcept {
			if (!IsActive()) {
				m_SendPort = PORT;
				m_Handle = MakeUDPSocket(IsServer ? m_SendPort : InvalidID);
			}
			return m_Handle != InvalidID;
		}
		void			Dispose(void) noexcept {
			if (IsActive()) {
				DeleteUDPSocket(m_Handle);	// �t�c�o�\�P�b�g�n���h���̍폜
				m_Handle = InvalidID;
				m_SendPort = InvalidID;
			}
		}
	private:
		template<class T>
		int				SendData(IPDATA& Ip, int SendPort, const T& Data) noexcept {
			if (IsActive()) {
				return NetWorkSendUDP(m_Handle, Ip, SendPort, &Data, sizeof(T));
			}
			return InvalidID;
		}
	public:
		// ���M
		template<class T>
		int				SendData(const T& Data) noexcept { return SendData(m_SendIP, m_SendPort, Data); }
		// ��M
		template<class T>
		bool			RecvData(T* Data, int* RecvReturn, bool IsPeek) noexcept {
			*RecvReturn = InvalidID;
			if (IsActive()) {
				switch (CheckNetWorkRecvUDP(m_Handle)) {
				case TRUE:
					*RecvReturn = NetWorkRecvUDP(m_Handle, &m_RecvIp, &m_RecvPort, Data, sizeof(T), IsPeek ? TRUE : FALSE);		// ��M
					return true;
				case FALSE:// �ҋ@
					break;
				default:// error
					break;
				}
			}
			return false;
		}
		// �ԑ�
		template<class T>
		int				ReturnData(const T& Data) noexcept { return SendData(m_RecvIp, m_RecvPort, Data); }
	};

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*DXLIB�̃t�@�C���ǂݍ��ݕ⏕�N���X																											*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class FileStreamDX {
		int mdata = InvalidID;
	public:
		//�R���X�g���N�^
		FileStreamDX() noexcept {}
		//�錾���Ƀt�@�C���I�[�v����
		FileStreamDX(const char* FilePath) noexcept { Open(FilePath); }
		//�f�X�g���N�^
		~FileStreamDX() noexcept { Close(); }
	public:
		//�t�@�C�����J���A�T���|�C���g���n�_�Ɉړ�
		void Open(const char* FilePath) noexcept { mdata = DxLib::FileRead_open(FilePath, FALSE); }
		// 1�s���̂܂܎擾���A���̍s�ɒT���|�C���g���ڂ�
		std::string SeekLineAndGetStr() const noexcept {
			const int charLength = 512;
			char mstr[charLength] = "";
			DxLib::FileRead_gets(mstr, charLength, mdata);
			return std::string(mstr);
		}
		// �T���|�C���g���I�[(EOF)�ŏI���
		bool ComeEof() const noexcept { return DxLib::FileRead_eof(mdata) != 0; }
		//�@����
		void Close() noexcept {
			if (mdata != InvalidID) {
				DxLib::FileRead_close(mdata);
				mdata = InvalidID;
			}
		}
	public:
		// �����񂩂�=���E�̒l�擾
		static std::string getleft(std::string tempname) noexcept { return tempname.substr(0, tempname.find('=')); }
		// �����񂩂�=���E�̒l�擾
		static std::string getright(std::string tempname) noexcept { return tempname.substr(tempname.find('=') + 1); }
	};
}
