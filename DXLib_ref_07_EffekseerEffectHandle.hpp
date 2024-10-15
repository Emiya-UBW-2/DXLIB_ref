/*=============================================================================
 Copyright (C) 2020 yumetodo <yume-wikijp@live.jp>
 Distributed under the Boost Software License, Version 1.0.
 (See https://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
#ifdef _USE_EFFEKSEER_
	class EffekseerEffectHandle;
	/*
	class Effekseer2DPlayingHandle {
	private:
		int handle_;
		constexpr Effekseer2DPlayingHandle(int h) noexcept : handle_(h) {}
		friend EffekseerEffectHandle;
		static constexpr int invalid_handle = InvalidID;

	public:
		constexpr Effekseer2DPlayingHandle(void) noexcept : handle_(invalid_handle) {}
		Effekseer2DPlayingHandle(const Effekseer2DPlayingHandle&) = delete;
		Effekseer2DPlayingHandle(Effekseer2DPlayingHandle&& o) noexcept : handle_(o.handle_) {
			o.handle_ = invalid_handle;
		}
		Effekseer2DPlayingHandle& operator=(const Effekseer2DPlayingHandle&) = delete;
		Effekseer2DPlayingHandle& operator=(Effekseer2DPlayingHandle&& o) noexcept {
			this->handle_ = o.handle_;
			o.handle_ = invalid_handle;
			return *this;
		}
		~Effekseer2DPlayingHandle(void) noexcept {
			if (InvalidID != this->handle_)
		{
				this->Stop();
			}
		}
		//  @brief	2D表示のエフェクトが再生中か取得する。
		//  @return	再生中かどうか?
		//  @return	true:再生中、false:再生されていない、もしくは再生終了
		bool IsPlaying(void) const noexcept {
			return 0 == IsEffekseer2DEffectPlaying(this->handle_);
		}
		// @brief 2D表示のエフェクトを停止する。
		void Stop(void) const noexcept {
			StopEffekseer2DEffect(this->handle_);
		}
		//  @brief	再生中の2D表示のエフェクトの位置を設定する。
		//  @param	x	X座標
		//  @param	y	Y座標
		void SetPos(Vector3DX pos) const noexcept {
			SetPosPlayingEffekseer2DEffect(this->handle_, pos.x, pos.y, pos.z);
		}
		//  @brief	再生中の2D表示のエフェクトの角度を設定する。
		//  @param	x	X軸角度(ラジアン)
		//  @param	y	Y軸角度(ラジアン)
		//  @param	z	Z軸角度(ラジアン)
		//  @note
		//  回転の方向は時計回りである。
		//  回転の順番は Z軸回転 → X軸回転 → Y軸回転である。
		void SetRotation(float x, float y, float z) const noexcept {
			SetRotationPlayingEffekseer2DEffect(this->handle_, x, y, z);
		}
		//  @brief	再生中の2D表示のエフェクトの拡大率を設定する。
		//  @param	x	X方向拡大率
		//  @param	y	Y方向拡大率
		//  @param	z	Z方向拡大率
		void SetScale(float x, float y, float z) const noexcept {
			SetScalePlayingEffekseer2DEffect(this->handle_, x, y, z);
		}
		//  @brief	再生中の2D表示のエフェクトの再生速度を取得する。
		//  @return	再生速度
		float GetSpeed(void) const noexcept {
			return GetSpeedPlayingEffekseer2DEffect(this->handle_);
		}
		//  @brief	再生中の2D表示のエフェクトの再生速度を設定する。
		//  @param	speed	再生速度
		void SetSpeed(float speed) const noexcept {
			SetSpeedPlayingEffekseer2DEffect(this->handle_, speed);
		}
		// * @brief 再生中の2D表示のエフェクトの色を設定する。
		void SetColor(int r, int g, int b, int a) const noexcept {
			SetColorPlayingEffekseer2DEffect(this->handle_, r, g, b, a);
		}
		// * @brief ハンドルを破棄する
		void Dispose(void) noexcept {
			if (InvalidID != this->handle_)
		{
				this->Stop();
				this->handle_ = InvalidID;
			}
		}
		//  @brief	指定されたEffekseerにより再生中の2Dエフェクトを描画する。
		//  @note
		//  特定のエフェクトのみを描画したい時に使用する。
		//  特定のエフェクトを描画する。
		//  DrawEffekseer2Dとは併用できない。
		void Draw(void) noexcept {
			DrawEffekseer2D_Draw(this->handle_);
		}
		[[deprecated]] int get(void) const noexcept { return this->handle_; }
	};
	*/
	class Effekseer3DPlayingHandle {
	private:
		int handle_;
		constexpr Effekseer3DPlayingHandle(int h) noexcept : handle_(h) {}
		friend EffekseerEffectHandle;
		static constexpr int invalid_handle = InvalidID;

	public:
		constexpr Effekseer3DPlayingHandle(void) noexcept : handle_(invalid_handle) {}
		Effekseer3DPlayingHandle(const Effekseer3DPlayingHandle&) = delete;
		Effekseer3DPlayingHandle(Effekseer3DPlayingHandle&& o) noexcept : handle_(o.handle_) {
			o.handle_ = invalid_handle;
		}
		Effekseer3DPlayingHandle& operator=(const Effekseer3DPlayingHandle&) = delete;
		Effekseer3DPlayingHandle& operator=(Effekseer3DPlayingHandle&& o) noexcept {
			this->handle_ = o.handle_;
			o.handle_ = invalid_handle;
			return *this;
		}
		~Effekseer3DPlayingHandle(void) noexcept {
			if (InvalidID != this->handle_) {
				this->Stop();
			}
		}
		/**
			@brief	3D表示のエフェクトが再生中か取得する。
			@return	再生中かどうか?
			@return	true:再生中、false:再生されていない、もしくは再生終了
		*/
		bool IsPlaying(void) const noexcept {
			return 0 == IsEffekseer3DEffectPlaying(this->handle_);
		}
		/**
		 * @brief 3D表示のエフェクトを停止する。
		 */
		void Stop(void) const noexcept {
			StopEffekseer3DEffect(this->handle_);
		}
		/**
			@brief	再生中の3D表示のエフェクトの位置を設定する。
			@param	x	X座標
			@param	y	Y座標
		*/
		void SetPos(const Vector3DX& pos) const noexcept {
			SetPosPlayingEffekseer3DEffect(this->handle_, pos.x, pos.y, pos.z);
		}
		/**
			@brief	再生中の3D表示のエフェクトの角度を設定する。
			@param	x	X軸角度(ラジアン)
			@param	y	Y軸角度(ラジアン)
			@param	z	Z軸角度(ラジアン)
			@note
			回転の方向は時計回りである。
			回転の順番は Z軸回転 → X軸回転 → Y軸回転である。
		*/
		void SetRotation(float x, float y, float z) const noexcept {
			SetRotationPlayingEffekseer3DEffect(this->handle_, x, y, z);
		}
		/**
			@brief	再生中の3D表示のエフェクトの拡大率を設定する。
			@param	x	X方向拡大率
			@param	y	Y方向拡大率
			@param	z	Z方向拡大率
		*/
		void SetScale(float size) const noexcept {
			SetScalePlayingEffekseer3DEffect(this->handle_, size, size, size);
		}
		/**
			@brief	再生中の3D表示のエフェクトの再生速度を取得する。
			@return	再生速度
		*/
		float GetSpeed(void) const noexcept {
			return GetSpeedPlayingEffekseer3DEffect(this->handle_);
		}
		/**
			@brief	再生中の3D表示のエフェクトの再生速度を設定する。
			@param	speed	再生速度
		*/
		void SetSpeed(float speed) const noexcept {
			SetSpeedPlayingEffekseer3DEffect(this->handle_, speed);
		}
		/**
		 * @brief 再生中の3D表示のエフェクトの色を設定する。
		 */
		void SetColor(int r, int g, int b, int a) const noexcept {
			SetColorPlayingEffekseer3DEffect(this->handle_, r, g, b, a);
		}
		/**
		 * @brief ハンドルを破棄する
		 */
		void Dispose(void) noexcept {
			if (InvalidID != this->handle_) {
				this->Stop();
				this->handle_ = InvalidID;
			}
		}
		/**
			@brief	指定されたEffekseerにより再生中の3Dエフェクトを描画する。
			@note
			特定のエフェクトのみを描画したい時に使用する。
			特定のエフェクトを描画する。
			DrawEffekseer3Dとは併用できない。
		*/
		void Draw(void) const noexcept {
			DrawEffekseer3D_Draw(this->handle_);
		}
		[[deprecated]] int get(void) const noexcept { return this->handle_; }
	};

	class EffekseerEffectHandle {
	private:
		int handle_;
		constexpr EffekseerEffectHandle(int h) noexcept : handle_(h) {}
		static constexpr int invalid_handle = InvalidID;

	public:
		constexpr EffekseerEffectHandle(void) noexcept : handle_(invalid_handle) {}
		EffekseerEffectHandle(const EffekseerEffectHandle&) = delete;
		EffekseerEffectHandle(EffekseerEffectHandle&& o) noexcept : handle_(o.handle_) {
			o.handle_ = invalid_handle;
		}
		EffekseerEffectHandle& operator=(const EffekseerEffectHandle&) = delete;
		EffekseerEffectHandle& operator=(EffekseerEffectHandle&& o) noexcept {
			this->handle_ = o.handle_;
			o.handle_ = invalid_handle;
			return *this;
		}
		~EffekseerEffectHandle(void) noexcept {
			if (InvalidID != this->handle_) {
				DeleteEffekseerEffect(this->handle_);
			}
		}
		void Dispose(void) noexcept {
			if (InvalidID != this->handle_) {
				DeleteEffekseerEffect(this->handle_);
				this->handle_ = InvalidID;
			}
		}
		/*
		//  @brief	メモリ上のEffekseerのエフェクトリソースを2D表示で再生する。
		//  @return	エフェクトのハンドル
		Effekseer2DPlayingHandle Play2D(void) const noexcept {
			return { PlayEffekseer2DEffect(this->handle_) };
		}
		*/
		/**
			@brief	メモリ上のEffekseerのエフェクトリソースを3D表示で再生する。
			@return	エフェクトのハンドル
		*/
		Effekseer3DPlayingHandle Play3D(void) const noexcept {
			return { PlayEffekseer3DEffect(this->handle_) };
		}
		[[deprecated]] int get(void) const noexcept { return this->handle_; }
		constexpr explicit operator bool(void) noexcept { return InvalidID != this->handle_; }
		// LoadEffekseerEffectはfileNameをstd::wstringに必ず格納する、オーバーロード必要
		static EffekseerEffectHandle load(const char* fileName, float magnification = 1.0f) noexcept { return { LoadEffekseerEffect(fileName, magnification) }; }
		static EffekseerEffectHandle load(const wchar_t* fileName, float magnification = 1.0f) noexcept { return { LoadEffekseerEffect(fileName, magnification) }; }
		static EffekseerEffectHandle load(const std::string& fileName, float magnification = 1.0f) noexcept { return load(fileName.c_str(), magnification); }
		static EffekseerEffectHandle load(const std::wstring& fileName, float magnification = 1.0f) noexcept { return load(fileName.c_str(), magnification); }
	};


	//エフェクトリソース
	class EffectResource : public SingletonBase<EffectResource> {
	private:
		friend class SingletonBase<EffectResource>;
	public:
		enum class Effect {
			ef_fire,			//発砲炎
			ef_reco,			//小口径跳弾
			ef_smoke,			//銃の軌跡
			ef_gndsmoke,		//地面の軌跡
			ef_fire2,			//発砲炎
			ef_hitblood,		//血しぶき
			ef_greexp,			//グレ爆発
			ef_greexp2,			//グレ爆発2
			ef_FireBallStart,	//火はじめ
			ef_FireBallLoop,	//火ループ
			ef_FireBallHit,		//火ヒット

			ef_ThunderStart,	//雷はじめ
			ef_ThunderLoop,		//雷ループ

			ef_Sonic,			//衝撃波

			ef_ThunderHit,		//雷ヒット
			effects,			//読み込む総量
		};
	public:
		std::vector<EffekseerEffectHandle> effsorce;	/*エフェクトリソース*/
	private:
		EffectResource(void) noexcept {
			std::vector<WIN32_FIND_DATA> data_t;
			GetFileNamesInDirectory("data/effect/*", &data_t);
			for (const auto& d : data_t) {
				std::string p = d.cFileName;
				if (p.find(".efk") != std::string::npos && p.find(".efkproj") == std::string::npos) {
					effsorce.resize(effsorce.size() + 1);
					effsorce.back() = EffekseerEffectHandle::load("data/effect/" + p);
				}
			}
			effsorce.resize(effsorce.size() + 1);
			effsorce.back() = EffekseerEffectHandle::load("data/effect/gndsmk.efk");								//戦車用エフェクト
		}
		EffectResource(const EffectResource&) = delete;
		EffectResource(EffectResource&& o) = delete;
		EffectResource& operator=(const EffectResource&) = delete;
		EffectResource& operator=(EffectResource&& o) = delete;

		~EffectResource(void) noexcept {
			for (auto& e : effsorce) {
				e.Dispose();
			}
		}
	};
	//エフェクトコントロール
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
			Effekseer3DPlayingHandle	m_handle;						//
		public:
			EffectS(void) noexcept {}
			EffectS(const EffectS&) = delete;
			EffectS(EffectS&& o) = delete;
			EffectS& operator=(const EffectS&) = delete;
			EffectS& operator=(EffectS&& o) = delete;

			~EffectS(void) noexcept {}
		public:
			auto			GetIsPlaying(void) const noexcept { return this->m_handle.IsPlaying(); }
			const auto&			GetIsFirst(void) const noexcept { return this->m_IsFirst; }
		private:
			void			Ef_Set(const EffekseerEffectHandle& handle_, const Vector3DX& pPos, const Vector3DX& pNormal, float pScale = 1.f) noexcept {
				this->m_IsFirst = true;
				if (GetIsPlaying()) {
					this->m_handle.Stop();
				}
				SetParam(pPos, pNormal, pScale);
				this->m_handle = handle_.Play3D();
			}
			void			Ef_Update(void) noexcept {
				if (GetIsPlaying()) {
					this->m_handle.SetPos(this->m_pos);
					this->m_handle.SetRotation(atan2(this->m_Normal.y, std::hypot(this->m_Normal.x, this->m_Normal.z)), atan2(-this->m_Normal.x, -this->m_Normal.z), 0);
					this->m_handle.SetScale(this->m_scale);
				}
			}
		public:
			void			SetParam(const Vector3DX& pPos, const Vector3DX& pNormal, float pScale = 1.f) noexcept {
				this->m_pos = pPos;
				this->m_Normal = pNormal;
				this->m_scale = pScale;
			}

			void			SetLoop(const EffekseerEffectHandle& handle_, const Vector3DX& pPos = Vector3DX::zero(), const Vector3DX& pNormal = Vector3DX::forward(), float pScale = 1.f) noexcept {
				Ef_Set(handle_, pPos, pNormal, pScale);
				Ef_Update();
				this->m_IsLoop = true;
			}
			void			SetOnce(const EffekseerEffectHandle& handle_, const Vector3DX& pPos, const Vector3DX& pNormal, float pScale = 1.f) noexcept {
				Ef_Set(handle_, pPos, pNormal, pScale);
				Ef_Update();
				this->m_IsLoop = false;
			}
			void			StopEffect(void) noexcept {
				if (GetIsPlaying()) {
					this->m_handle.Stop();
				}
			}
			void			SetEffectSpeed(float pSpeed) noexcept {
				if (GetIsPlaying()) {
					this->m_handle.SetSpeed(pSpeed);
				}
			}
			void			SetEffectPos(const Vector3DX& pPos) noexcept {
				if (GetIsPlaying()) {
					this->m_pos = pPos;
					this->m_handle.SetPos(pPos);
				}
			}
			void			SetEffectScale(float pScale = 1.f) noexcept {
				if (GetIsPlaying()) {
					this->m_scale = pScale;
					this->m_handle.SetScale(pScale);
				}
			}
			void			SetEffectColor(int r, int g, int b, int a) noexcept {
				if (GetIsPlaying()) {
					this->m_handle.SetColor(r, g, b, a);
				}
			}
		public:
			void			Execute(void) noexcept {
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
				this->m_handle.Dispose();
			}
		};
	private:
		static const int EffectNum = 16;
		std::array<std::pair<int, std::array<std::unique_ptr<EffectS>, EffectNum + 1>>, static_cast<int>(EffectResource::Effect::effects)> m_effect;//エフェクト
	public:
		//複数エフェクトの再生
		void		SetOnce_Any(EffectResource::Effect ID, const Vector3DX& pos_t, const Vector3DX& nomal_t, float scale = 1.f, float speed = 1.f) noexcept {
			this->m_effect.at(static_cast<size_t>(ID)).second[static_cast<size_t>(this->m_effect.at(static_cast<size_t>(ID)).first)]->SetOnce(EffectResource::Instance()->effsorce.at(static_cast<size_t>(ID)), pos_t, nomal_t, scale);
			this->m_effect.at(static_cast<size_t>(ID)).second[static_cast<size_t>(this->m_effect.at(static_cast<size_t>(ID)).first)]->SetEffectSpeed(speed);
			++this->m_effect.at(static_cast<size_t>(ID)).first %= EffectNum;
		}
		auto		CheckEffectCount(void) const noexcept {
			int cnt = 0;
			for (int i : std::views::iota(0, EffectNum)) {
				for (auto& t : this->m_effect.at(static_cast<size_t>(i)).second) {
					if (t->GetIsPlaying()) {
						++cnt;
					}
				}
			}
			return cnt;
		}
		//単体で制御したいエフェクトの制御
		auto		CheckPlayEffect(EffectResource::Effect ID) const noexcept {
			return this->m_effect.at(static_cast<size_t>(ID)).second[EffectNum]->GetIsPlaying();
		}
		void		SetLoop(EffectResource::Effect ID, const Vector3DX& pos_t) noexcept {
			this->m_effect.at(static_cast<size_t>(ID)).second[EffectNum]->SetLoop(EffectResource::Instance()->effsorce.at(static_cast<size_t>(ID)), pos_t);
		}
		void		Update_LoopEffect(EffectResource::Effect ID, const Vector3DX& pos_t, const Vector3DX& nomal_t, float scale = 1.f) noexcept {
			this->m_effect.at(static_cast<size_t>(ID)).second[EffectNum]->SetParam(pos_t, nomal_t, scale);
		}
		void		SetOnce(EffectResource::Effect ID, const Vector3DX& pos_t, const Vector3DX& nomal_t, float scale = 1.f) noexcept {
			this->m_effect.at(static_cast<size_t>(ID)).second[EffectNum]->SetOnce(EffectResource::Instance()->effsorce.at(static_cast<size_t>(ID)), pos_t, nomal_t, scale);
		}
		void		StopEffect(EffectResource::Effect ID) noexcept { this->m_effect.at(static_cast<size_t>(ID)).second[EffectNum]->StopEffect(); }
		void		SetEffectSpeed(EffectResource::Effect ID, float value) noexcept { this->m_effect.at(static_cast<size_t>(ID)).second[EffectNum]->SetEffectSpeed(value); }
		void		SetEffectScale(EffectResource::Effect ID, float value) noexcept { this->m_effect.at(static_cast<size_t>(ID)).second[EffectNum]->SetEffectScale(value); }
		void		SetEffectColor(EffectResource::Effect ID, int r, int g, int b, int a) noexcept { this->m_effect.at(static_cast<size_t>(ID)).second[EffectNum]->SetEffectColor(r, g, b, a); }
	public:
		EffectControl(void) noexcept {}
		EffectControl(const EffectControl&) = delete;
		EffectControl(EffectControl&& o) = delete;
		EffectControl& operator=(const EffectControl&) = delete;
		EffectControl& operator=(EffectControl&& o) = delete;

		~EffectControl(void) noexcept {}
	public:
		//全体の更新
		void		Init(void) noexcept {
			for (auto& ef : this->m_effect) {
				ef.first = 0;
				for (auto& t : ef.second) {
					t = std::make_unique<EffectS>();
				}
			}
		}
		void		Execute(void) noexcept {
			for (auto& ef : this->m_effect) {
				for (auto& t : ef.second) {
					t->Execute();
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
		}
	};
#endif
}