﻿#pragma once
#include "DXLib_ref.h"

#define _USE_EFFEKSEER_2D_ (FALSE)

namespace DXLibRef {
#if defined(_USE_EFFEKSEER_)
	class EffekseerEffectResourceHandle;

	// エフェクト再生時のハンドル
#if _USE_EFFEKSEER_2D_
	class Effekseer2DPlayingHandle : public DXHandle {
		friend EffekseerEffectResourceHandle;
	protected:
		void	Dispose_Sub(void) noexcept override {
			this->Stop();
		}
	public:
		// 2D表示のエフェクトが再生中か取得する。 true:再生中、false:再生されていない、もしくは再生終了
		bool IsPlaying(void) const noexcept { return 0 == IsEffekseer2DEffectPlaying(DXHandle::get()); }
		// 2D表示のエフェクトを停止する。
		void Stop(void) const noexcept { StopEffekseer2DEffect(DXHandle::get()); }
		// 再生中の2D表示のエフェクトの位置を設定する。
		void SetPos(const Vector3DX& pos) const noexcept { SetPosPlayingEffekseer2DEffect(DXHandle::get(), pos.x, pos.y, pos.z); }
		// 再生中の2D表示のエフェクトの角度を設定する。
		// 回転の方向は時計回りである。回転の順番は Z軸回転 → X軸回転 → Y軸回転である。
		void SetRotation(float x, float y, float z) const noexcept { SetRotationPlayingEffekseer2DEffect(DXHandle::get(), x, y, z); }
		// 再生中の2D表示のエフェクトの拡大率を設定する。
		void SetScale(float x, float y, float z) const noexcept { SetScalePlayingEffekseer2DEffect(DXHandle::get(), x, y, z); }
		// 再生中の2D表示のエフェクトの再生速度を取得する。
		float GetSpeed(void) const noexcept { return GetSpeedPlayingEffekseer2DEffect(DXHandle::get()); }
		// 再生中の2D表示のエフェクトの再生速度を設定する。
		void SetSpeed(float speed) const noexcept { SetSpeedPlayingEffekseer2DEffect(DXHandle::get(), speed); }
		// 再生中の2D表示のエフェクトの色を設定する。
		void SetColor(int r, int g, int b, int a) const noexcept { SetColorPlayingEffekseer2DEffect(DXHandle::get(), r, g, b, a); }
		// 指定されたEffekseerにより再生中の2Dエフェクトを描画する。
		// 特定のエフェクトのみを描画したい時に使用する。 DrawEffekseer32とは併用できない。
		void Draw(void) noexcept { DrawEffekseer2D_Draw(DXHandle::get()); }
	};
#endif
	class Effekseer3DPlayingHandle : public DXHandle {
		friend EffekseerEffectResourceHandle;
	protected:
		void	Dispose_Sub(void) noexcept override {
			this->Stop();
		}
	public:
		// 3D表示のエフェクトが再生中か取得する。 true:再生中、false:再生されていない、もしくは再生終了
		bool IsPlaying(void) const noexcept { return IsEffekseer3DEffectPlaying(DXHandle::get()) == 0; }
		// 3D表示のエフェクトを停止する。
		void Stop(void) const noexcept { StopEffekseer3DEffect(DXHandle::get()); }
		// 再生中の3D表示のエフェクトの位置を設定する。
		void SetPos(const Vector3DX& pos) const noexcept { SetPosPlayingEffekseer3DEffect(DXHandle::get(), pos.x, pos.y, pos.z); }
		// 再生中の3D表示のエフェクトの角度を設定する。
		// 回転の方向は時計回りである。回転の順番は Z軸回転 → X軸回転 → Y軸回転である。
		void SetRotation(float x, float y, float z) const noexcept { SetRotationPlayingEffekseer3DEffect(DXHandle::get(), x, y, z); }
		// 再生中の3D表示のエフェクトの拡大率を設定する。
		void SetScale(float size) const noexcept { SetScalePlayingEffekseer3DEffect(DXHandle::get(), size, size, size); }
		// 再生中の3D表示のエフェクトの再生速度を取得する。
		float GetSpeed(void) const noexcept { return GetSpeedPlayingEffekseer3DEffect(DXHandle::get()); }
		// 再生中の3D表示のエフェクトの再生速度を設定する。
		void SetSpeed(float speed) const noexcept { SetSpeedPlayingEffekseer3DEffect(DXHandle::get(), speed); }
		// 再生中の3D表示のエフェクトの色を設定する。
		void SetColor(int r, int g, int b, int a) const noexcept { SetColorPlayingEffekseer3DEffect(DXHandle::get(), r, g, b, a); }
		// 指定されたEffekseerにより再生中の3Dエフェクトを描画する。
		// 特定のエフェクトのみを描画したい時に使用する。 DrawEffekseer3Dとは併用できない。
		void Draw(void) const noexcept { DrawEffekseer3D_Draw(DXHandle::get()); }
	};
	// エフェクトリソース
	class EffekseerEffectResourceHandle : public DXHandle {
	protected:
		void	Dispose_Sub(void) noexcept override {
			DeleteEffekseerEffect(DXHandle::get());
		}
	public:
#if _USE_EFFEKSEER_2D_
		// メモリ上のEffekseerのエフェクトリソースを2D表示で再生する。
		void Play2D(Effekseer2DPlayingHandle* pHandle) const noexcept { pHandle->SetHandleDirect(PlayEffekseer2DEffect(DXHandle::get())); }
#endif
		// メモリ上のEffekseerのエフェクトリソースを3D表示で再生する。
		void Play3D(Effekseer3DPlayingHandle* pHandle) const noexcept { pHandle->SetHandleDirect(PlayEffekseer3DEffect(DXHandle::get())); }
	public:
		// LoadEffekseerEffectはfileNameをstd::wstringに必ず格納する、オーバーロード必要
		void load(const char* fileName, float magnification = 1.0f) noexcept { DXHandle::SetHandleDirect(LoadEffekseerEffect(fileName, magnification)); }
		void load(const wchar_t* fileName, float magnification = 1.0f) noexcept { DXHandle::SetHandleDirect(LoadEffekseerEffect(fileName, magnification)); }
		void load(const std::string& fileName, float magnification = 1.0f) noexcept { load(fileName.c_str(), magnification); }
		void load(const std::wstring& fileName, float magnification = 1.0f) noexcept { load(fileName.c_str(), magnification); }
	};
	// エフェクトリソース
	class EffectResource : public SingletonBase<EffectResource> {
	private:
		friend class SingletonBase<EffectResource>;
	public:
		std::vector<EffekseerEffectResourceHandle>	m_Sorce;
		size_t										m_Size;
	private:
		EffectResource(void) noexcept;
		EffectResource(const EffectResource&) = delete;
		EffectResource(EffectResource&&) = delete;
		EffectResource& operator=(const EffectResource&) = delete;
		EffectResource& operator=(EffectResource&&) = delete;

		~EffectResource(void) noexcept {
			for (auto& e : this->m_Sorce) {
				e.Dispose();
			}
			this->m_Sorce.clear();
			this->m_Size = 0;
		}
	};
	// エフェクトコントロール
	class EffectControl {
	public:
		class EffectS {
		private:
			bool						m_IsFirst{ false };				// 
			bool						m_IsLoop{ false };				// 
			size_t						m_ID{ 0 };						// 
			Vector3DX					m_pos;							// 
			Vector3DX					m_Normal;						// 
			float						m_scale{ 1.f };					// 
			Effekseer3DPlayingHandle	m_PlayHandle;					// 
		public:
			EffectS(void) noexcept {}
			EffectS(const EffectS&) = delete;
			EffectS(EffectS&&) = delete;
			EffectS& operator=(const EffectS&) = delete;
			EffectS& operator=(EffectS&&) = delete;

			~EffectS(void) noexcept {}
		public:
			auto			GetIsPlaying(void) const noexcept { return this->m_PlayHandle.IsPlaying(); }
			const auto& GetIsFirst(void) const noexcept { return this->m_IsFirst; }
		private:
			void			Ef_Set(const EffekseerEffectResourceHandle& handle, const Vector3DX& pPos, const Vector3DX& pNormal, float pScale = 1.f) noexcept {
				this->m_IsFirst = true;
				if (GetIsPlaying()) {
					this->m_PlayHandle.Stop();
				}
				SetParam(pPos, pNormal, pScale);
				handle.Play3D(&this->m_PlayHandle);
			}
			void			Ef_Update(void) noexcept {
				if (GetIsPlaying()) {
					this->m_PlayHandle.SetPos(this->m_pos);
					this->m_PlayHandle.SetRotation(atan2(this->m_Normal.y, std::hypot(this->m_Normal.x, this->m_Normal.z)), atan2(-this->m_Normal.x, -this->m_Normal.z), 0);
					this->m_PlayHandle.SetScale(this->m_scale);
				}
			}
		public:
			void			SetParam(const Vector3DX& pPos, const Vector3DX& pNormal, float pScale = 1.f) noexcept {
				this->m_pos = pPos;
				this->m_Normal = pNormal;
				this->m_scale = pScale;
			}

			void			SetLoop(const EffekseerEffectResourceHandle& handle, const Vector3DX& pPos = Vector3DX::zero(), const Vector3DX& pNormal = Vector3DX::forward(), float pScale = 1.f) noexcept {
				Ef_Set(handle, pPos, pNormal, pScale);
				Ef_Update();
				this->m_IsLoop = true;
			}
			void			SetOnce(const EffekseerEffectResourceHandle& handle, const Vector3DX& pPos, const Vector3DX& pNormal, float pScale = 1.f) noexcept {
				Ef_Set(handle, pPos, pNormal, pScale);
				Ef_Update();
				this->m_IsLoop = false;
			}
			void			StopEffect(void) noexcept {
				if (GetIsPlaying()) {
					this->m_PlayHandle.Stop();
				}
			}
			void			SetEffectSpeed(float pSpeed) noexcept {
				if (GetIsPlaying()) {
					this->m_PlayHandle.SetSpeed(pSpeed);
				}
			}
			void			SetEffectPos(const Vector3DX& pPos) noexcept {
				if (GetIsPlaying()) {
					this->m_pos = pPos;
					this->m_PlayHandle.SetPos(pPos);
				}
			}
			void			SetEffectScale(float pScale = 1.f) noexcept {
				if (GetIsPlaying()) {
					this->m_scale = pScale;
					this->m_PlayHandle.SetScale(pScale);
				}
			}
			void			SetEffectColor(int r, int g, int b, int a) noexcept {
				if (GetIsPlaying()) {
					this->m_PlayHandle.SetColor(r, g, b, a);
				}
			}
		public:
			void			Update(void) noexcept {
				if (this->m_IsLoop) {
					this->m_IsFirst = false;
					Ef_Update();
				}
				else {
					if (this->m_IsFirst) {
						this->m_IsFirst = false;
						Ef_Update();
					}
				}
			}
			void			Dispose(void) noexcept {
				this->m_PlayHandle.Dispose();
			}
		};

		typedef size_t EffectID;
	private:
		static const size_t EffectNum = 16;
		std::vector<std::pair<size_t, std::array<std::unique_ptr<EffectS>, EffectNum + 1>>>		m_effect;// エフェクト
	public:
		const auto& GetEffect(EffectID ID) const noexcept { return this->m_effect[ID]; }
		auto& GetEffect(EffectID ID) noexcept { return this->m_effect[ID]; }
		// 複数エフェクトの再生
		void		SetOnce_Any(EffectID ID, const Vector3DX& pos_t, const Vector3DX& nomal_t, float scale = 1.f, float speed = 1.f) noexcept {
			GetEffect(ID).second[GetEffect(ID).first]->SetOnce(EffectResource::Instance()->m_Sorce[ID], pos_t, nomal_t, scale);
			GetEffect(ID).second[GetEffect(ID).first]->SetEffectSpeed(speed);
			++GetEffect(ID).first %= EffectNum;
		}
		auto		CheckEffectCount(void) const noexcept {
			int cnt = 0;
			for (auto& e : this->m_effect) {
				for (auto& t : e.second) {
					if (t->GetIsPlaying()) {
						++cnt;
					}
				}
			}
			return cnt;
		}
		// 単体で制御したいエフェクトの制御
		auto		CheckPlayEffect(EffectID ID) const noexcept {
			return GetEffect(ID).second[EffectNum]->GetIsPlaying();
		}
		bool		IsPlayLoopEffect(EffectID ID) noexcept {
			return GetEffect(ID).second[EffectNum]->GetIsPlaying();
		}
		void		SetLoop(EffectID ID, const Vector3DX& pos_t) noexcept {
			GetEffect(ID).second[EffectNum]->SetLoop(EffectResource::Instance()->m_Sorce[ID], pos_t);
		}
		void		Update_LoopEffect(EffectID ID, const Vector3DX& pos_t, const Vector3DX& nomal_t, float scale = 1.f) noexcept {
			GetEffect(ID).second[EffectNum]->SetParam(pos_t, nomal_t, scale);
		}

		int 		SetLoopAny(EffectID ID, const Vector3DX& pos_t) noexcept {
			GetEffect(ID).second[GetEffect(ID).first]->StopEffect();
			GetEffect(ID).second[GetEffect(ID).first]->SetLoop(EffectResource::Instance()->m_Sorce[ID], pos_t);
			int UniqueID = (int)GetEffect(ID).first;
			++GetEffect(ID).first %= EffectNum;
			return UniqueID;
		}
		void		Update_LoopAnyEffect(EffectID ID, int UniqueID, const Vector3DX& pos_t, const Vector3DX& nomal_t, float scale = 1.f) noexcept {
			GetEffect(ID).second[UniqueID]->SetParam(pos_t, nomal_t, scale);
		}
		void		StopEffectAny(EffectID ID, int UniqueID) noexcept {
			GetEffect(ID).second[UniqueID]->StopEffect();
		}

		void		SetOnce(EffectID ID, const Vector3DX& pos_t, const Vector3DX& nomal_t, float scale = 1.f) noexcept {
			GetEffect(ID).second[EffectNum]->SetOnce(EffectResource::Instance()->m_Sorce[ID], pos_t, nomal_t, scale);
		}
		void		StopEffect(EffectID ID) noexcept { GetEffect(ID).second[EffectNum]->StopEffect(); }
		void		SetEffectSpeed(EffectID ID, float value) noexcept { GetEffect(ID).second[EffectNum]->SetEffectSpeed(value); }
		void		SetEffectScale(EffectID ID, float value) noexcept { GetEffect(ID).second[EffectNum]->SetEffectScale(value); }
		void		SetEffectColor(EffectID ID, int r, int g, int b, int a) noexcept { GetEffect(ID).second[EffectNum]->SetEffectColor(r, g, b, a); }
	public:
		EffectControl(void) noexcept {}
		EffectControl(const EffectControl&) = delete;
		EffectControl(EffectControl&&) = delete;
		EffectControl& operator=(const EffectControl&) = delete;
		EffectControl& operator=(EffectControl&&) = delete;

		~EffectControl(void) noexcept {}
	public:
		// 全体の更新
		void		Init(void) noexcept {
			this->m_effect.resize(EffectResource::Instance()->m_Size);
			for (auto& ef : this->m_effect) {
				ef.first = 0;
				for (auto& t : ef.second) {
					t = std::make_unique<EffectS>();
				}
			}
		}
		void		Update(void) noexcept {
			for (auto& ef : this->m_effect) {
				for (auto& t : ef.second) {
					t->Update();
				}
			}
		}
		void		Dispose(void) noexcept {
			for (auto& ef : this->m_effect) {
				for (auto& t : ef.second) {
					t->Dispose();
					t.reset();
				}
			}
			this->m_effect.clear();
		}
	};
#endif
}