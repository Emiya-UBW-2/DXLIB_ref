#pragma once
#include "DXLib_ref.h"

//#define _USE_WAVECALC_
namespace DXLibRef {

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*�V�F�[�_�[																																*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// �V�F�[�_�[���g�p����ۂ̕⏕�N���X
	class ShaderController {
	public:
		// 2D�ɃV�F�[�_�[��K�p����ۂɎg�p�����ʃT�C�Y�̒��_���
		class ScreenVertex {
			VERTEX3DSHADER Screen_vertex[6] = {};
		public:
			// ���_�f�[�^�̎擾
			const auto* GetScreenVertex(void) const noexcept { return Screen_vertex; }
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
		// DXLIB������������Ă����V�F�[�_�[�p�̒�`
		typedef float DX_D3D11_SHADER_FLOAT2[2];
		typedef float DX_D3D11_SHADER_FLOAT4[4];
		//
		struct DX_D3D11_GS_CONST_BUFFER_BASE {
			DX_D3D11_SHADER_FLOAT4		ProjectionMatrix[4]{};											// �r���[�@���@�v���W�F�N�V�����s��
			DX_D3D11_SHADER_FLOAT4		ViewMatrix[3]{};												// ���[���h�@���@�r���[�s��
		};
#if defined(_USE_WAVECALC_)
		struct Wave {
			DX_D3D11_SHADER_FLOAT2 dir{};
			float amplitude{};
			float waveLength{};
		};
		struct ImmutableCB {
			Wave waves[20]{};
		};
#endif
	private:
		// �V�F�[�_�[�n���h��
		int					m_VertexShaderhandle{ InvalidID };
		int					m_GeometryShaderhandle{ InvalidID };
		int					m_PixelShaderhandle{ InvalidID };
		// �V�F�[�_�[�ɓn���ǉ��p�����[�^�[��z����n���h��
		std::array<int, 2>	LightCameraMatrixConstantBufferHandle{};	// �e�p�̐[�x�L�^�摜���쐬�����ۂ̃J�����̃r���[�s��Ǝˉe�s���ݒ肷�邽�߂̒萔�o�b�t�@
		std::array<int, 4>	m_VertexShadercbhandle{};
		int					m_GeometryShaderMatcbhandle{ InvalidID };
		int					m_PixelShaderSendDispSizeHandle{ InvalidID };
		std::array<int, 3>	m_PixelShadercbhandle{};
#if defined(_USE_WAVECALC_)
		ImmutableCB WaveData{};
		int	m_VertexShadercbWaveDataHandle{ InvalidID };
#endif
		ScreenVertex	m_ScreenVertex;					// ���_�f�[�^
	public:
		ShaderController(void) noexcept {
			// �V�F�[�_�[�n���h��
			this->m_VertexShaderhandle = InvalidID;
			this->m_GeometryShaderhandle = InvalidID;
			this->m_PixelShaderhandle = InvalidID;
			// �V�F�[�_�[�ɓn���ǉ��p�����[�^�[��z����n���h��
			for (auto& h : this->m_VertexShadercbhandle) {
				h = InvalidID;
			}
			this->m_GeometryShaderMatcbhandle = InvalidID;
			this->m_PixelShaderSendDispSizeHandle = InvalidID;
			for (auto& h : this->m_PixelShadercbhandle) {
				h = InvalidID;
			}
		}
		~ShaderController(void) noexcept {
			Dispose();
		}
	public:
		// ������
		void			Init(const char* VertexShader, const char* PixelShader) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			// ���_�V�F�[�_�[����
#if defined(_USE_WAVECALC_)
			this->m_VertexShadercbWaveDataHandle = CreateShaderConstantBuffer(sizeof(ImmutableCB));
#endif
			for (auto& h : this->m_VertexShadercbhandle) {
				h = CreateShaderConstantBuffer(sizeof(float) * 4);
			}
			this->m_VertexShaderhandle = LoadVertexShader(VertexShader);
			// �s�N�Z���V�F�[�_�\����
			this->m_PixelShaderSendDispSizeHandle = CreateShaderConstantBuffer(sizeof(float) * 4);
			for (auto& h : this->m_PixelShadercbhandle) {
				h = CreateShaderConstantBuffer(sizeof(float) * 4);
			}
			this->m_PixelShaderhandle = LoadPixelShader(PixelShader);
			// �e�p�̐[�x�L�^�摜���쐬�����ۂ̃J�����̃r���[�s��Ǝˉe�s���ݒ肷�邽�߂̒萔�o�b�t�@�̍쐬
			for (auto& h : LightCameraMatrixConstantBufferHandle) {
				h = CreateShaderConstantBuffer(sizeof(LIGHTCAMERA_MATRIX));
			}
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
			for (auto& h : this->m_VertexShadercbhandle) {
				DeleteShaderConstantBuffer(h);
			}
#if defined(_USE_WAVECALC_)
			DeleteShaderConstantBuffer(this->m_VertexShadercbWaveDataHandle);
#endif
			DeleteShader(this->m_VertexShaderhandle);
			// 
			DeleteShaderConstantBuffer(this->m_GeometryShaderMatcbhandle);
			DeleteShader(this->m_GeometryShaderhandle);
			// �s�N�Z���V�F�[�_�\����
			DeleteShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle);
			for (auto& h : this->m_PixelShadercbhandle) {
				DeleteShaderConstantBuffer(h);
			}
			DeleteShader(this->m_PixelShaderhandle);
		}
	public:
		// ���_�V�F�[�_�\��Slot�Ԗڂ̃��W�X�^�ɏ����Z�b�g(Slot>=4)
		void			SetVertexCameraMatrix(int Slot, const Matrix4x4DX& View, const Matrix4x4DX& Projection) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = LightCameraMatrixConstantBufferHandle[static_cast<size_t>(Slot - 4)];
			// �ݒ肵���J�����̃r���[�s��Ǝˉe�s����擾���Ă���
			LIGHTCAMERA_MATRIX* LightCameraMatrixConst = (LIGHTCAMERA_MATRIX*)GetBufferShaderConstantBuffer(BufferHandle);
			LightCameraMatrixConst->ViewMatrix = View.get();
			LightCameraMatrixConst->ProjectionMatrix = Projection.get();

			UpdateShaderConstantBuffer(BufferHandle);
			SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_VERTEX, Slot);		// �e�p�[�x�L�^�摜��`�悵���Ƃ��̃J�����̃r���[�s��Ǝˉe�s���萔�ɐݒ肷��
		}
		// ���_�V�F�[�_�\��Slot�Ԗڂ̃��W�X�^�ɏ����Z�b�g(Slot>=4)
		void			SetVertexParam(int Slot, float param1, float param2, float param3, float param4) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = this->m_VertexShadercbhandle[static_cast<size_t>(Slot - 4)];
			FLOAT4* f4 = (FLOAT4*)GetBufferShaderConstantBuffer(BufferHandle);		// ���_�V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			UpdateShaderConstantBuffer(BufferHandle);								// ���_�V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
			SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_VERTEX, Slot);		// ���_�V�F�[�_�[�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^�S�ɃZ�b�g
		}
		// 
#if defined(_USE_WAVECALC_)
		void CalcGWave() {
			for (int loop : std::views::iota(0, 20)) {
				Wave& w = WaveData.waves[loop];
				float randomRad = static_cast<float>(GetRand(30) * DX_PI_F * 2 * 0.3f);
				w.dir[0] = sinf(randomRad);
				w.dir[1] = cosf(randomRad);
				w.amplitude = (0.03f + powf(2.0f, static_cast<float>(GetRand(3)) * 2.0f) * 0.05f) * 0.05f * Scale3DRate;
				w.waveLength = 1.0f + powf(2.f, 1.f + static_cast<float>(GetRand(3))) * 10.f;
			}
		}
		void			SetVertexWave(void) const noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = this->m_VertexShadercbWaveDataHandle;
			ImmutableCB* f4 = (ImmutableCB*)GetBufferShaderConstantBuffer(BufferHandle);
			*f4 = WaveData;
			UpdateShaderConstantBuffer(BufferHandle);
			SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_VERTEX, 5);
		}
#endif
		// �V�F�[�_�\��Slot�Ԗڂ̃��W�X�^�ɏ����Z�b�g(Slot>=4)
		void			SetGeometryCONSTBUFFER(int Slot, const MATRIX* ViewMatrix, const MATRIX* ProjectionMatrix) const noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			if (this->m_GeometryShaderhandle == InvalidID) { return; }
			auto& BufferHandle = this->m_GeometryShaderMatcbhandle;
			DX_D3D11_GS_CONST_BUFFER_BASE* LightCameraMatrixConst = (DX_D3D11_GS_CONST_BUFFER_BASE*)GetBufferShaderConstantBuffer(BufferHandle);

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

			UpdateShaderConstantBuffer(BufferHandle);
			SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_GEOMETRY, Slot);		// �e�p�[�x�L�^�摜��`�悵���Ƃ��̃J�����̃r���[�s��Ǝˉe�s���萔�ɐݒ肷��
		}
		// �s�N�Z���V�F�[�_�\��Slot�Ԗڂ̃��W�X�^�ɏ����Z�b�g(Slot>=4)
		void			SetPixelCameraMatrix(int Slot, const Matrix4x4DX& View, const Matrix4x4DX& Projection) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = LightCameraMatrixConstantBufferHandle[static_cast<size_t>(Slot - 4)];
			// �ݒ肵���J�����̃r���[�s��Ǝˉe�s����擾���Ă���
			LIGHTCAMERA_MATRIX* LightCameraMatrixConst = (LIGHTCAMERA_MATRIX*)GetBufferShaderConstantBuffer(BufferHandle);
			LightCameraMatrixConst->ViewMatrix = View.get();
			LightCameraMatrixConst->ProjectionMatrix = Projection.get();

			UpdateShaderConstantBuffer(BufferHandle);
			SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_PIXEL, Slot);		// �e�p�[�x�L�^�摜��`�悵���Ƃ��̃J�����̃r���[�s��Ǝˉe�s���萔�ɐݒ肷��
		}
		// �s�N�Z���V�F�[�_�\��2�Ԗڂ̃��W�X�^�ɉ�ʃT�C�Y�̏����Z�b�g
		void			SetPixelDispSize(int dispx, int dispy) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = this->m_PixelShaderSendDispSizeHandle;
			FLOAT2* dispsize = (FLOAT2*)GetBufferShaderConstantBuffer(BufferHandle);	// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
			dispsize->u = static_cast<float>(dispx);
			dispsize->v = static_cast<float>(dispy);
			UpdateShaderConstantBuffer(BufferHandle);									// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
			SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_PIXEL, 2);				// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^2�ɃZ�b�g
			this->m_ScreenVertex.SetScreenVertex(dispx, dispy);
		}
		// �s�N�Z���V�F�[�_�\��Slot�Ԗڂ̃��W�X�^�ɏ����Z�b�g(Slot>=3)
		void			SetPixelParam(int Slot, float param1, float param2, float param3, float param4) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = this->m_PixelShadercbhandle[static_cast<size_t>(Slot - 3)];
			FLOAT4* f4 = (FLOAT4*)GetBufferShaderConstantBuffer(BufferHandle);				// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			UpdateShaderConstantBuffer(BufferHandle);											// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
			SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_PIXEL, Slot);					// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^3�ɃZ�b�g
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
		void			Draw(void) const noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			Draw_lamda([this] {DrawPolygon3DToShader(this->m_ScreenVertex.GetScreenVertex(), 2); });
		}
		// 2D�摜�ɓK�p����ꍇ�̊֐�
		void			Draw(const ScreenVertex& Screenvertex) const noexcept {
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
		RealTimeCubeMap(RealTimeCubeMap&&) = delete;
		RealTimeCubeMap& operator=(const RealTimeCubeMap&) = delete;
		RealTimeCubeMap& operator=(RealTimeCubeMap&&) = delete;

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
			for (size_t loop : std::views::iota(0, 6)) {		// �f�肱�ފ���`�悷��ʂ̐������J��Ԃ�
				for (int loop2 : std::views::iota(0, MIPLEVEL)) {			// �~�b�v�}�b�v�̐������J��Ԃ�
					dynamicCubeTex.SetRenderTargetToShader(0, static_cast<int>(loop), loop2);		// �`���ԍ��O�Ԃ̕`��Ώۂ�`��Ώۂɂł���L���[�u�}�b�v��loop�Ԗڂ̖ʂɐݒ�
					ClearDrawScreen();										// �N���A
					{
						SetupCamera_Perspective(90.0f / 180.0f * DX_PI_F);								// �J�����̉�p��90�x�ɐݒ�
						SetCameraNearFar(0.5f * Scale3DRate, 1000.0f * Scale3DRate);									// Near�N���b�v�ʂ�Far�N���b�v�ʂ̋�����ݒ�
						SetCameraPositionAndTargetAndUpVec(Pos.get(), (Pos + lookAt[loop]).get(), up[loop].get());	// �J�����̈ʒu�ƒ����_�A�J�����̏������ݒ�
						Doing();
					}
				}
			}
		}

		void Dispose(void) noexcept {
			dynamicCubeTex.Dispose();
		}

		const auto&		GetCubeMapTex(void) const noexcept { return dynamicCubeTex; }
	};
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*�X�N���[���o�b�t�@�̂����܂킵																											*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class PostPassScreenBuffer {
		static const size_t			m_Size = 3;
	public:
		int xSize{};
		int ySize{};
		bool isTrans{};
		bool m_isDepth{};
		int m_ZBufferBitDepth{};
		std::array<GraphHandle, m_Size>	m_Screen{};
		int m_UsedLocal = 0;
		int m_Used = 0;
		int m_UnUse = 0;
	public:
		PostPassScreenBuffer(int xsize, int ysize, bool trans, bool isDepth = false, int ZBufferBitDepth = InvalidID) noexcept {
			this->xSize = xsize;
			this->ySize = ysize;
			this->isTrans = trans;
			this->m_isDepth = isDepth;
			this->m_ZBufferBitDepth = ZBufferBitDepth;
			int PrevZBufferBitDepth{};
			if (this->m_ZBufferBitDepth != InvalidID) {
				PrevZBufferBitDepth = GetCreateDrawValidGraphZBufferBitDepth();
				SetCreateDrawValidGraphZBufferBitDepth(this->m_ZBufferBitDepth);
			}
			for (auto& s : this->m_Screen) {
				if (this->m_isDepth) {
					s.MakeDepth(this->xSize, this->ySize);
				}
				else {
					s.Make(this->xSize, this->ySize, this->isTrans);
				}
			}
			if (this->m_ZBufferBitDepth != InvalidID) {
				SetCreateDrawValidGraphZBufferBitDepth(PrevZBufferBitDepth);
			}
			this->m_Used = 0;
			this->m_UsedLocal = 0;
			this->m_UnUse = 0;
		}
		~PostPassScreenBuffer(void) noexcept {
			for (auto& s : this->m_Screen) {
				s.Dispose();
			}
		}
	public:
		const GraphHandle* PopBlankScreen(void) noexcept {
			if (this->m_UsedLocal >= this->m_Size) {
				MessageBox(NULL, "None Blank PostPassScreenBuffer", "", MB_OK);
				exit(InvalidID);
			}
			auto* Ret = &this->m_Screen[this->m_UsedLocal];
			++this->m_Used;
			++this->m_UsedLocal;
			return Ret;
		}
	};
	class PostPassScreenBufferPool : public SingletonBase<PostPassScreenBufferPool> {
	private:
		friend class SingletonBase<PostPassScreenBufferPool>;
	private:
		std::vector<std::unique_ptr<PostPassScreenBuffer>>	m_ScreenBuffer;		// �`��X�N���[��
	private:
		PostPassScreenBufferPool(void) noexcept {}
		PostPassScreenBufferPool(const PostPassScreenBufferPool&) = delete;
		PostPassScreenBufferPool(PostPassScreenBufferPool&&) = delete;
		PostPassScreenBufferPool& operator=(const PostPassScreenBufferPool&) = delete;
		PostPassScreenBufferPool& operator=(PostPassScreenBufferPool&&) = delete;
		virtual ~PostPassScreenBufferPool(void) noexcept {}
	public:
		const GraphHandle* PopBlankScreen(int xsize, int ysize, bool trans, bool isDepth = false, int ZBufferBitDepth = InvalidID) noexcept {
			auto Find = std::find_if(this->m_ScreenBuffer.begin(), this->m_ScreenBuffer.end(), [&](const std::unique_ptr<PostPassScreenBuffer>& tgt) {
				return  ((tgt->xSize == xsize) && (tgt->ySize == ysize) && (tgt->isTrans == trans) && (tgt->m_isDepth == isDepth) && tgt->m_ZBufferBitDepth == ZBufferBitDepth);
				});
			if (Find != this->m_ScreenBuffer.end()) {
				return (*Find)->PopBlankScreen();
			}
			this->m_ScreenBuffer.emplace_back(std::make_unique<PostPassScreenBuffer>(xsize, ysize, trans, isDepth, ZBufferBitDepth));
			return this->m_ScreenBuffer.back()->PopBlankScreen();
		}
		void ResetUseCount(int xsize, int ysize, bool trans, bool isDepth = false, int ZBufferBitDepth = InvalidID) noexcept {
			for (auto& s : this->m_ScreenBuffer) {
				if ((s->xSize == xsize) && (s->ySize == ysize) && (s->isTrans == trans) && (s->m_isDepth == isDepth) && s->m_ZBufferBitDepth == ZBufferBitDepth) {
					s->m_UsedLocal = 0;
				}
			}
		}
	public:
		void FirstUpdate(void) noexcept {
			for (auto& s : this->m_ScreenBuffer) {
				if (s->m_Used != 0) {
					s->m_UnUse = 0;
				}
				else {
					++s->m_UnUse;
				}
				s->m_UsedLocal = 0;
				s->m_Used = 0;
			}
			//5�t���[���Ԏg���Ă��Ȃ��X�N���[���o�b�t�@�͏���
			for (size_t loop = 0, max = this->m_ScreenBuffer.size(); loop < max; ++loop) {
				auto& s = this->m_ScreenBuffer[loop];
				if (s->m_UnUse > 5) {
					s.reset();
					std::swap(s, this->m_ScreenBuffer.back());
					this->m_ScreenBuffer.pop_back();
					--loop;
					--max;
				}
			}
		}
	};
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*�|�X�g�v���Z�X																															*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// �x�[�X
	class PostPassBase {
	protected:
		bool	m_PrevActive{ false };
	public:
		PostPassBase(void) noexcept {}
		virtual ~PostPassBase(void) noexcept {}
	protected:
		virtual void Load_Sub(void) noexcept {}
		virtual void Dispose_Sub(void) noexcept {}
		virtual bool IsActive_Sub(void) noexcept { return true; }
		virtual void SetEffect_Sub(GraphHandle*, GraphHandle*, GraphHandle*, GraphHandle*) noexcept {}
	public:
		bool IsActive(void) noexcept { return IsActive_Sub(); }
		void UpdateActive(bool active) noexcept {
			if (this->m_PrevActive != active) {
				this->m_PrevActive = active;
				if (active) {
					Load_Sub();
				}
				else {
					Dispose_Sub();
				}
			}
		}
		void SetEffect(GraphHandle* TargetGraph, GraphHandle* ColorGraph, GraphHandle* NormalPtr, GraphHandle* DepthPtr) noexcept {
			if (IsActive()) {
				SetEffect_Sub(TargetGraph, ColorGraph, NormalPtr, DepthPtr);
			}
		}
	};
	//
	class ShadowDraw {
		static const int EXTEND = 1;
	private:
		GraphHandle			BaseShadowHandle;
		GraphHandle			DepthBaseScreenHandle;
		GraphHandle			DepthScreenHandle;
		GraphHandle			DepthFarScreenHandle;
		
		ShaderController			m_Shader;
		ShaderController			m_ShaderRigid;
		Vector3DX					m_ShadowVec{ Vector3DX::up() };
		float						m_Scale{ 1.f };
		float						m_ScaleFar{ 1.f };

		bool						m_PrevShadow{ false };

		std::array<Matrix4x4DX, 2>	m_CamViewMatrix{};
		std::array<Matrix4x4DX, 2>	m_CamProjectionMatrix{};
	private:
		void SetupCam(Vector3DX Center, float scale) const noexcept;
	public:
		ShadowDraw(void) noexcept {}
		ShadowDraw(const ShadowDraw&) = delete;
		ShadowDraw(ShadowDraw&&) = delete;
		ShadowDraw& operator=(const ShadowDraw&) = delete;
		ShadowDraw& operator=(ShadowDraw&&) = delete;
		~ShadowDraw(void) noexcept { Dispose(); }
	public:
		const auto&		GetCamViewMatrix(bool isFar) const noexcept { return this->m_CamViewMatrix[static_cast<size_t>(isFar ? 1 : 0)]; }
		const auto&		GetCamProjectionMatrix(bool isFar) const noexcept { return this->m_CamProjectionMatrix[static_cast<size_t>(isFar ? 1 : 0)]; }
		const auto&		GetDepthScreen(void) const noexcept { return DepthScreenHandle; }
		const auto&		GetDepthFarScreen(void) const noexcept { return DepthFarScreenHandle; }//���g�p
		const auto&		GetShadowDir(void) const noexcept { return this->m_ShadowVec; }
	public:
		void			SetVec(const Vector3DX& Vec) noexcept { this->m_ShadowVec = Vec; }
		void			SetDraw(std::function<void()> doing_rigid, std::function<void()> doing, Camera3DInfo tmp_cam) noexcept;

		void			Update(std::function<void()> Shadowdoing, Vector3DX Center, float Scale) noexcept;
		void			UpdateFar(std::function<void()> Shadowdoing, Vector3DX Center, float Scale) noexcept;
		void			Draw(void) noexcept;
		void			Dispose(void) noexcept;
	public:
		void			SetActive(void) noexcept;
		bool			UpdateActive(void) noexcept;
	};
	//
	class PostPassEffect : public SingletonBase<PostPassEffect> {
	private:
		friend class SingletonBase<PostPassEffect>;
	private:
		std::array<std::unique_ptr<PostPassBase>, 16>	m_PostPass;
		//
		bool						m_IsActiveGBuffer{ false };
		GraphHandle					m_BufferScreen;	// �`��X�N���[��
		GraphHandle					m_ColorScreen;	// ���̂܂ܓ��߂Ȃ��ɂ����X�N���[��
		GraphHandle					m_NormalScreen;	// �@����G�o�b�t�@
		GraphHandle					m_DepthScreen;	// �[�x��G�o�b�t�@
		// 
		float						m_near_DoF = 0.f;
		float						m_far_DoF = 0.f;
		float						m_near_DoFMax = 0.f;
		float						m_far_DoFMin = 0.f;
		int							m_InColorPerMin = 20;
		int							m_InColorPerMax = 255;
		float						m_InColorGamma = 1.1f;
		Matrix4x4DX					m_CamViewMat{};
		Matrix4x4DX					m_CamProjectionMat{};
		Camera3DInfo				m_CamInfo{};

		bool						m_useScope{ false };
		float						m_ScopeXpos{ 0.f };
		float						m_ScopeYpos{ 0.f };
		float						m_ScopeSize{ 0.f };
		float						m_ScopeZoom{ 0.f };
		bool						m_useBlackOut{ false };
		float						m_BlackOutPer{ 1.f };

		float						m_AberrationPower{ 1.f };
		float						m_DistortionPer{ 120.f };
		float						m_GodRayPer{ 0.5f };
		float						m_GodRayPerByPostPass{ 1.f };
		std::unique_ptr<ShadowDraw>	m_ShadowDraw;
		bool						m_IsCubeMap{ false };
		RealTimeCubeMap				m_RealTimeCubeMap;
		ShaderController			m_PBR_Shader;
		float						m_ShadowScale{ 1.f };
	public:
		auto& GetBufferScreen(void) noexcept { return this->m_BufferScreen; }
	public:
		const auto&		GetCamViewMat(void) const noexcept { return this->m_CamViewMat; }
		const auto&		GetCamProjectionMat(void) const noexcept { return this->m_CamProjectionMat; }
		const auto&		GetShadowDraw(void) const noexcept { return this->m_ShadowDraw; }
		const auto&		GetShadowDir(void) const noexcept { return this->m_ShadowDraw->GetShadowDir(); }
		const auto&		GetCubeMapTex(void) const noexcept { return this->m_RealTimeCubeMap.GetCubeMapTex(); }
		const auto&		Get_near_DoF(void) const noexcept { return this->m_near_DoF; }
		const auto&		Get_far_DoF(void) const noexcept { return this->m_far_DoF; }
		const auto&		Get_near_DoFMax(void) const noexcept { return this->m_near_DoFMax; }
		const auto&		Get_far_DoFMin(void) const noexcept { return this->m_far_DoFMin; }
		const auto&		is_lens(void) const noexcept { return this->m_useScope; }
		const auto&		zoom_xpos(void) const noexcept { return this->m_ScopeXpos; }
		const auto&		zoom_ypos(void) const noexcept { return this->m_ScopeYpos; }
		const auto&		zoom_size(void) const noexcept { return this->m_ScopeSize; }
		const auto&		zoom_lens(void) const noexcept { return this->m_ScopeZoom; }
		const auto&		is_Blackout(void) const noexcept { return this->m_useBlackOut; }
		const auto&		GetBlackoutPer(void) const noexcept { return this->m_BlackOutPer; }
		const auto&		GetAberrationPower(void) const noexcept { return this->m_AberrationPower; }
		const auto		GetGodRayPerRet(void) const noexcept { return this->m_GodRayPer * this->m_GodRayPerByPostPass; }
		const auto&		GetDistortionPer(void) const noexcept { return this->m_DistortionPer; }
		const auto&		GetShadowScale(void) const noexcept { return this->m_ShadowScale; }
		const auto&		GetGodRayPer(void) const noexcept { return this->m_GodRayPer; }
	public:
		void			Set_is_lens(bool value) noexcept { this->m_useScope = value; }
		void			Set_xp_lens(float value) noexcept { this->m_ScopeXpos = value; }
		void			Set_yp_lens(float value) noexcept { this->m_ScopeYpos = value; }
		void			Set_size_lens(float value) noexcept { this->m_ScopeSize = value; }
		void			Set_zoom_lens(float value) noexcept { this->m_ScopeZoom = value; }
		void			Set_is_Blackout(bool value) noexcept { this->m_useBlackOut = value; }
		void			Set_Per_Blackout(float value) noexcept { this->m_BlackOutPer = value; }
		void			SetAberrationPower(float value) noexcept { this->m_AberrationPower = value; }
		void			SetGodRayPer(float value) noexcept { this->m_GodRayPer = value; }
		void			SetGodRayPerByPostPass(float value) noexcept { Easing(&this->m_GodRayPerByPostPass, value, 0.975f, EasingType::OutExpo); }
		void			SetDistortionPer(float value) noexcept { this->m_DistortionPer = value; }
		void			SetShadowScale(float value) noexcept { this->m_ShadowScale = value; }
		// �{�P�n�߂�ꏊ���w��(���S�Ƀ{�P��̂̓j�A�t�@�[�̌��x)
		void			Set_DoFNearFar(float near_d, float far_d, float near_m, float far_m) noexcept {
			this->m_near_DoF = near_d;
			this->m_far_DoF = far_d;
			this->m_near_DoFMax = near_m;
			this->m_far_DoFMin = far_m;
		}
		void			SetLevelFilter(int inMin, int inMax, float gamma) noexcept {
			this->m_InColorPerMin = std::clamp(inMin, 0, 255);
			this->m_InColorPerMax = std::clamp(inMax, 0, 255);
			this->m_InColorGamma = std::max(1.f, gamma);
		}
		void			ResetAllParams(void) noexcept {
			SetLevelFilter(0, 255, 1.f);
			SetAberrationPower(1.f);
			Set_is_Blackout(false);
			Set_Per_Blackout(0.f);
			Set_is_lens(false);
			Set_zoom_lens(1.f);
			// �����Ɖe�̏�����
			SetAmbientLight(Vector3DX::vget(0.25f, -1.f, 0.25f));
		}
	private:
		PostPassEffect(void) noexcept;
		PostPassEffect(const PostPassEffect&) = delete;
		PostPassEffect(PostPassEffect&&) = delete;
		PostPassEffect& operator=(const PostPassEffect&) = delete;
		PostPassEffect& operator=(PostPassEffect&&) = delete;
		virtual ~PostPassEffect(void) noexcept {
			PostPassScreenBufferPool::Release();
			Dispose();

		}
	private:
		void			Dispose(void) noexcept;
	public:
		void			Init(void) noexcept;
		void			UpdateActive(void) noexcept;
		void			SetCamMat(const Camera3DInfo& camInfo) noexcept;
		void			ResetBuffer(void) noexcept;
		void			DrawGBuffer(float near_len, float far_len, std::function<void()> done) noexcept;
		void			SetDrawShadow(const Camera3DInfo& camInfo,
			std::function<void()> setshadowdoing_rigid,
			std::function<void()> setshadowdoing) noexcept;
		void			DrawPostProcess(void) noexcept;
		void			ResetAllBuffer(void) noexcept;

		bool			UpdateShadowActive(void) noexcept { return this->m_ShadowDraw->UpdateActive(); }
		void			SetAmbientLight(const Vector3DX& AmbientLightVec) noexcept { this->m_ShadowDraw->SetVec(AmbientLightVec); }
		void			Update_Shadow(std::function<void()> doing, const Vector3DX& CenterPos, bool IsFar) noexcept;
		void			Update_CubeMap(std::function<void()> doing, const Vector3DX& CenterPos) noexcept;

		void			DrawByPBR(std::function<void()> doing) noexcept;
	private:
		void LoadGBuffer(void) noexcept;
		void DisposeGBuffer(void) noexcept;

		void UpdateActiveGBuffer(bool ActiveGBuffer) noexcept {
			if (this->m_IsActiveGBuffer != ActiveGBuffer) {
				this->m_IsActiveGBuffer = ActiveGBuffer;
				if (this->m_IsActiveGBuffer) {
					LoadGBuffer();
				}
				else {
					DisposeGBuffer();
				}
			}
		}

		void UpdateActiveCubeMap(bool ActiveCubeMap) noexcept {
			if (ActiveCubeMap != this->m_IsCubeMap) {
				this->m_IsCubeMap = ActiveCubeMap;
				if (this->m_IsCubeMap) {
					this->m_RealTimeCubeMap.Init();
				}
				else {
					this->m_RealTimeCubeMap.Dispose();
				}
			}
		}
	};
};
