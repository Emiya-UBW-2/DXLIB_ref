#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	class DXDraw : public SingletonBase<DXDraw> {
	private:
		friend class SingletonBase<DXDraw>;
	private:
		class ShadowDraw {
			GraphHandle			BaseShadowHandle;
			GraphHandle			DepthBaseScreenHandle;
			GraphHandle			DepthScreenHandle;
			GraphHandle			DepthFarScreenHandle;

			ShaderUseClass		m_Shader;
			Vector3DX			m_ShadowVec{Vector3DX::up()};

			std::array<Matrix4x4DX, 2> m_CamViewMatrix{};
			std::array<Matrix4x4DX, 2> m_CamProjectionMatrix{};
		public:
			const auto&		GetDepthScreen(void) const noexcept { return DepthScreenHandle; }
			const auto&		GetDepthFarScreen(void) const noexcept { return DepthFarScreenHandle; }

			const auto&		GetCamViewMatrix(bool isFar) const noexcept { return m_CamViewMatrix[isFar ? 1 : 0]; }
			const auto&		GetCamProjectionMatrix(bool isFar) const noexcept { return m_CamProjectionMatrix[isFar ? 1 : 0]; }

			void			SetVec(const Vector3DX& Vec) noexcept { m_ShadowVec = Vec; }

			void Init(int ShadowMapSize, int dispsizex, int dispsizey);
			void Update(std::function<void()> Shadowdoing, Vector3DX Center);
			void UpdateFar(std::function<void()> Shadowdoing, Vector3DX Center);
			void SetDraw(std::function<void()> doing, Camera3DInfo tmp_cam);
			void Draw();
			void Dispose();
		private:
			void SetupCam(Vector3DX Center, float scale) const noexcept;
		};
	private:
		int							m_DispXSize{deskx};
		int							m_DispYSize{desky};

		int							m_DispXSize_Win{deskx};
		int							m_DispYSize_Win{desky};

		int							m_DispXSize_Border{deskx};
		int							m_DispYSize_Border{desky};

		int							m_DispXSize_Max{deskx};
		int							m_DispYSize_Max{desky};

		float						m_FPS{60.f};

		switchs						m_PauseActive;
		LONGLONG					m_StartTime{0};
		bool						m_IsEnd{false};

		bool						m_PrevShadow{false};

		ShadowDraw					m_ShadowDraw;

		Vector3DX					m_LightVec;
		COLOR_F						m_LightColorF{GetColorF(0, 0, 0, 0)};
		GraphHandle					UI_Screen;								//UI
		Camera3DInfo				m_MainCamera;							//カメラ
		//カメラシェイク
		bool						m_SendCamShake{false};
		float						m_SendCamShakeTime{1.f};
		float						m_SendCamShakePower{1.f};
		float						m_CamShake{0.f};
		Vector3DX					m_CamShake1;
		Vector3DX					m_CamShake2;
		//
		float						m_AberrationPower{1.f};
		//
		bool						m_IsExitSelect{false};
		bool						m_IsRestartSelect{false};
		float						m_PauseFlashCount{0.f};

		ShaderUseClass::ScreenVertex	m_ScreenVertex;						// 頂点データ
		std::array<ShaderUseClass, 2>	m_Shader2D;
		std::array<shaderparam, 2>		m_ShaderParam;						//シェーダーパラメーター

		LONGLONG					Update_effect_was = 0;					//エフェクトのアップデートタイミングタイマー
		bool						m_IsFirstBoot{false};

		bool						m_IsCubeMap{true};
		RealTimeCubeMap				m_RealTimeCubeMap;

		CheckPCSpec					m_CheckPCSpec;
		ShaderUseClass				m_PBR_Shader;

		Matrix4x4DX					m_CamViewMatrix{};
		Matrix4x4DX					m_CamProjectionMatrix{};
	public://ゲッター
		const auto&		GetDispXSize(void) const noexcept { return m_DispXSize; }
		const auto&		GetDispYSize(void) const noexcept { return m_DispYSize; }

		const auto&		GetDispXSizeMin(void) const noexcept { return m_DispXSize_Win; }
		const auto&		GetDispYSizeMin(void) const noexcept { return m_DispYSize_Win; }
		const auto&		GetDispXSizeMax(void) const noexcept { return m_DispXSize_Max; }
		const auto&		GetDispYSizeMax(void) const noexcept { return m_DispYSize_Max; }

		const auto&		GetFps(void) const noexcept { return m_FPS; }

		const auto&		is_lens(void) const noexcept { return m_ShaderParam[0].use; }
		const auto&		zoom_lens(void) const noexcept { return m_ShaderParam[0].param[3]; }

		const auto&		GetShadowDraw(void) const noexcept { return m_ShadowDraw; }

		void			Set_is_lens(bool value) noexcept { m_ShaderParam[0].use = value; }
		void			Set_xp_lens(float value) noexcept { m_ShaderParam[0].param[0] = value; }
		void			Set_yp_lens(float value) noexcept { m_ShaderParam[0].param[1] = value; }
		void			Set_size_lens(float value) noexcept { m_ShaderParam[0].param[2] = value; }
		void			Set_zoom_lens(float value) noexcept { m_ShaderParam[0].param[3] = value; }

		void			Set_is_Blackout(bool value) noexcept { m_ShaderParam[1].use = value; }
		void			Set_Per_Blackout(float value) noexcept { m_ShaderParam[1].param[0] = value; }

		const auto&		GetCamViewMatrix() const noexcept { return m_CamViewMatrix; }
		const auto&		GetCamProjectionMatrix() const noexcept { return m_CamProjectionMatrix; }
	private://コンストラクタ
		DXDraw(void) noexcept;
		~DXDraw(void) noexcept;
	public:
		const auto		IsExit() const noexcept { return m_IsExitSelect; }
		const auto		IsRestart() const noexcept { return m_IsRestartSelect; }

		const auto		IsPause() const noexcept { return m_PauseActive.on(); }
		void			SetPause(bool value) noexcept;
	public:
		auto&			SetMainCamera(void) noexcept { return m_MainCamera; }
		const auto&		GetMainCamera(void) const noexcept { return m_MainCamera; }
	public:
		void			SetAberrationPower(float value) noexcept { m_AberrationPower = value; }
		const auto&		GetAberrationPower(void) const noexcept { return m_AberrationPower; }
	public:
		void			SetAmbientLight(const Vector3DX& AmbientLightVec, const COLOR_F& LightColor) noexcept;
		void			Update_Shadow(std::function<void()> doing, const Vector3DX& CenterPos, bool IsFar) noexcept;
		void			Update_CubeMap(std::function<void()> doing, const Vector3DX& CenterPos) noexcept;
	public:
		void			SetCamShake(float time, float power) noexcept {
			this->m_SendCamShake = true;
			this->m_SendCamShakeTime = time;
			this->m_SendCamShakePower = power;
		}
		const auto&		GetCamShake(void) const noexcept { return m_CamShake2; }
		const auto&		IsFirstBoot(void) const noexcept { return m_IsFirstBoot; }
		const auto&		GetCubeMapTex(void) const noexcept { return m_RealTimeCubeMap.GetCubeMapTex(); }

		void			SetWindowOrBorderless() noexcept;
	public:
		void			Init(void) noexcept;
		bool			FirstExecute(void) noexcept;
		bool			UpdateShadowActive() noexcept;
		void			Execute(void) noexcept;
		void			Draw(
			std::function<void()> sky_doing,
			std::function<void()> doing,
			std::function<void()> doingFront,
			std::function<void()> doingUI,
			std::function<void()> doingUI2
		) noexcept;
		bool			Screen_Flip(void) noexcept;

		//VR
	private:
		class VRControl;
		VRControl*			m_VRControl{nullptr};
		VRControl*			GetVRControl() noexcept { return m_VRControl; }
		const VRControl*	GetVRControl() const noexcept { return m_VRControl; }
	public:
		void				Get_VR_HMDPositionVR(Vector3DX* pos_, Matrix4x4DX*mat) noexcept;
		void				Reset_VR_HMD(void) noexcept;
		bool				Get_VR_Hand1PadPress(VR_PAD) const noexcept;
		bool				Get_VR_Hand1TouchPress(VR_PAD) const noexcept;
		Vector3DX			Get_VR_Hand1TouchPadPoint() const noexcept;
		bool				Get_VR_Hand2PadPress(VR_PAD) const noexcept;
		bool				Get_VR_Hand2TouchPress(VR_PAD) const noexcept;
		Vector3DX			Get_VR_Hand2TouchPadPoint() const noexcept;
		void				VR_Haptic(char id_, unsigned short times) noexcept;	//VRコントローラー振動
	private:
	};
};
