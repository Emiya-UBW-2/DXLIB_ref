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
			ShaderUseClass		m_ShaderRigid;
			Vector3DX			m_ShadowVec{ Vector3DX::up() };

			std::array<Matrix4x4DX, 2> m_CamViewMatrix{};
			std::array<Matrix4x4DX, 2> m_CamProjectionMatrix{};
		public:
			const auto& GetDepthScreen(void) const noexcept { return DepthScreenHandle; }
			const auto& GetDepthFarScreen(void) const noexcept { return DepthFarScreenHandle; }

			const auto& GetCamViewMatrix(bool isFar) const noexcept { return m_CamViewMatrix[static_cast<std::size_t>(isFar ? 1 : 0)]; }
			const auto& GetCamProjectionMatrix(bool isFar) const noexcept { return m_CamProjectionMatrix[static_cast<std::size_t>(isFar ? 1 : 0)]; }

			void			SetVec(const Vector3DX& Vec) noexcept { m_ShadowVec = Vec; }

			void Init(int ShadowMapSize, int dispsizex, int dispsizey) noexcept;
			void Update(std::function<void()> Shadowdoing, Vector3DX Center, float Scale) noexcept;
			void UpdateFar(std::function<void()> Shadowdoing, Vector3DX Center, float Scale) noexcept;
			void SetDraw(std::function<void()> doing_rigid, std::function<void()> doing, Camera3DInfo tmp_cam) noexcept;
			void Draw(void) noexcept;
			void Dispose(void) noexcept;
		private:
			void SetupCam(Vector3DX Center, float scale) const noexcept;
		private:
			bool						m_PrevShadow{ false };
		public:
			ShadowDraw(void) noexcept {}
			ShadowDraw(const ShadowDraw&) = delete;
			ShadowDraw(ShadowDraw&& o) = delete;
			ShadowDraw& operator=(const ShadowDraw&) = delete;
			ShadowDraw& operator=(ShadowDraw&& o) = delete;

			~ShadowDraw(void) noexcept {}
		public:
			void SetActive(void) noexcept;
			bool UpdateActive(void) noexcept;
		};
	private:
		int							m_DispXSize{ deskx };
		int							m_DispYSize{ desky };

		int							m_DispXSize_Win{ deskx };
		int							m_DispYSize_Win{ desky };

		int							m_DispXSize_Border{ deskx };
		int							m_DispYSize_Border{ desky };

		int							m_DispXSize_Max{ deskx };
		int							m_DispYSize_Max{ desky };

		float						m_FPS{ 60.f };

		switchs						m_PauseActive;
		LONGLONG					m_StartTime{ 0 };
		bool						m_IsEnd{ false };

		std::unique_ptr<ShadowDraw>	m_ShadowDraw;

		Vector3DX					m_LightVec;
		COLOR_F						m_LightColorF{ GetColorF(0, 0, 0, 0) };
		GraphHandle					UI_Screen;								//UI
		Camera3DInfo				m_MainCamera;							//カメラ
		//カメラシェイク
		//
		float						m_AberrationPower{ 1.f };
		//
		bool						m_IsExitSelect{ false };
		bool						m_IsRestartSelect{ false };
		float						m_PauseFlashCount{ 0.f };

		ShaderUseClass::ScreenVertex	m_ScreenVertex;						// 頂点データ
		std::array<ShaderUseClass, 2>	m_Shader2D;
		std::array<shaderparam, 2>		m_ShaderParam;						//シェーダーパラメーター

		LONGLONG					Update_effect_was = 0;					//エフェクトのアップデートタイミングタイマー
		bool						m_IsFirstBoot{ false };

		bool						m_IsCubeMap{ true };
		RealTimeCubeMap				m_RealTimeCubeMap;

		CheckPCSpec					m_CheckPCSpec;
		ShaderUseClass				m_PBR_Shader;

		Matrix4x4DX					m_CamViewMatrix{};
		Matrix4x4DX					m_CamProjectionMatrix{};
	public://ゲッター
		const auto& is_lens(void) const noexcept { return m_ShaderParam[0].use; }
		const auto& zoom_lens(void) const noexcept { return m_ShaderParam[0].param[3]; }
		void			Set_is_lens(bool value) noexcept { m_ShaderParam[0].use = value; }
		void			Set_xp_lens(float value) noexcept { m_ShaderParam[0].param[0] = value; }
		void			Set_yp_lens(float value) noexcept { m_ShaderParam[0].param[1] = value; }
		void			Set_size_lens(float value) noexcept { m_ShaderParam[0].param[2] = value; }
		void			Set_zoom_lens(float value) noexcept { m_ShaderParam[0].param[3] = value; }
		void			Set_is_Blackout(bool value) noexcept { m_ShaderParam[1].use = value; }
		void			Set_Per_Blackout(float value) noexcept { m_ShaderParam[1].param[0] = value; }

		//UI以外のスクリーン空間
		auto			GetScreenX(int value) const noexcept { return (value * this->m_DispXSize_Max / this->m_DispXSize_Win); }
		auto			GetScreenY(int value) const noexcept { return (value * this->m_DispYSize_Max / this->m_DispYSize_Win); }
		//UI用
		int				GetUIX(int value) const noexcept { return (value * m_DispXSize / basex); }
		int				GetUIY(int value) const noexcept { return (value * m_DispYSize / basey); }
		//
		void			GetMousePosition(int* MouseX, int* MouseY) const noexcept;
		const auto& GetFps(void) const noexcept { return m_FPS; }
		const auto& GetShadowDraw(void) const noexcept { return m_ShadowDraw; }
		const auto& GetCamViewMatrix(void) const noexcept { return m_CamViewMatrix; }
		const auto& GetCamProjectionMatrix(void) const noexcept { return m_CamProjectionMatrix; }
		const auto& IsExit(void) const noexcept { return m_IsExitSelect; }
		const auto& IsRestart(void) const noexcept { return m_IsRestartSelect; }
		auto			IsPause(void) const noexcept { return m_PauseActive.on(); }
		const auto& GetMainCamera(void) const noexcept { return m_MainCamera; }
		const auto& GetAberrationPower(void) const noexcept { return m_AberrationPower; }
		const auto& GetCubeMapTex(void) const noexcept { return m_RealTimeCubeMap.GetCubeMapTex(); }
	public:
		void			SetPause(bool value) noexcept;
		auto& SetMainCamera(void) noexcept { return m_MainCamera; }
		void			SetAberrationPower(float value) noexcept { m_AberrationPower = value; }
		void			SetAmbientLight(const Vector3DX& AmbientLightVec, const COLOR_F& LightColor) noexcept;
		void			Update_Shadow(std::function<void()> doing, const Vector3DX& CenterPos, float Scale, bool IsFar) noexcept;
		void			Update_CubeMap(std::function<void()> doing, const Vector3DX& CenterPos) noexcept;
		void			SetWindowOrBorderless(void) noexcept;
	private:
		void			FirstBootSetting(void) noexcept;
		void			PauseDraw(void) const noexcept;
	private://コンストラクタ
		DXDraw(void) noexcept;
		DXDraw(const DXDraw&) = delete;
		DXDraw(DXDraw&& o) = delete;
		DXDraw& operator=(const DXDraw&) = delete;
		DXDraw& operator=(DXDraw&& o) = delete;

		~DXDraw(void) noexcept;
	public:
		void			Init(void) noexcept;
		auto			IsFirstBoot(void) noexcept {
			if (m_IsFirstBoot) {
				FirstBootSetting();
			}
			return m_IsFirstBoot;
		}
		bool			FirstExecute(void) noexcept;
		bool			UpdateShadowActive(void) noexcept;
		void			Execute(void) noexcept;
		void			Draw(
			std::function<void()> sky_doing,
			std::function<void()> setshadowdoing_rigid,
			std::function<void()> setshadowdoing,
			std::function<void()> doing,
			std::function<void()> doingFront,
			std::function<void()> doingUI,
			std::function<void()> doingUI2
		) noexcept;
		void			Draw2D(
			std::function<void()> doing,
			std::function<void()> doingUI,
			std::function<void()> doingUI2
		) noexcept;
		bool			Screen_Flip(void) noexcept;

		//VR
	private:
		class VRControl;
		VRControl* m_VRControl{ nullptr };
		VRControl* GetVRControl(void) noexcept { return m_VRControl; }
		const VRControl* GetVRControl(void) const noexcept { return m_VRControl; }
	public:
		void				Get_VR_HMDPositionVR(Vector3DX* pos_, Matrix4x4DX* mat) noexcept;
		void				Reset_VR_HMD(void) noexcept;
		bool				Get_VR_Hand1PadPress(VR_PAD) const noexcept;
		bool				Get_VR_Hand1TouchPress(VR_PAD) const noexcept;
		Vector3DX			Get_VR_Hand1TouchPadPoint(void) const noexcept;
		bool				Get_VR_Hand2PadPress(VR_PAD) const noexcept;
		bool				Get_VR_Hand2TouchPress(VR_PAD) const noexcept;
		Vector3DX			Get_VR_Hand2TouchPadPoint(void) const noexcept;
		void				VR_Haptic(char id_, unsigned short times) noexcept;	//VRコントローラー振動
	};
};
