/*=============================================================================
  Copyright (C) 2020 yumetodo <yume-wikijp@live.jp>
  Distributed under the Boost Software License, Version 1.0.
  (See https://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#pragma once
#include "DXLib_ref.h"

namespace DXLib_ref {
	//共通のサウンドEnum
	enum class SoundEnumCommon {
		UI_Select,
		UI_OK,
		UI_CANCEL,
		UI_NG,
		Num,
	};
	//
	class SoundHandle {
	private:
		int handle_;
		constexpr SoundHandle(int h) : handle_(h) {}
		static constexpr int invalid_handle = -1;
	public:
		constexpr SoundHandle() : handle_(invalid_handle) {}
		SoundHandle(const SoundHandle&) = delete;
		SoundHandle(SoundHandle&& o) noexcept : handle_(o.handle_) {
			o.handle_ = invalid_handle;
		}
		SoundHandle& operator=(const SoundHandle&) = delete;
		SoundHandle& operator=(SoundHandle&& o) noexcept {
			this->handle_ = o.handle_;
			o.handle_ = invalid_handle;
			return *this;
		}
		~SoundHandle(void) noexcept {
			Dispose();
		}
		void Dispose(void) noexcept {
			if (invalid_handle != this->handle_) {
				DeleteSoundMem(this->handle_);
				this->handle_ = invalid_handle;
			}
		}
		int get(void) const noexcept { return handle_; }
		SoundHandle Duplicate(void) const noexcept { return DxLib::DuplicateSoundMem(this->handle_); }
		bool check()  const noexcept { return (DxLib::CheckSoundMem(handle_) == TRUE); }
		bool play(int type, int flag = 1) const noexcept { return (PlaySoundMem(handle_, type, flag) == 0); }
		bool stop(void) const noexcept {
			if (invalid_handle != this->handle_) {
				return (StopSoundMem(handle_) == 0);
			}
			else {
				return false;
			}
		}
		bool vol(int vol) const noexcept { return (ChangeVolumeSoundMem(std::clamp<int>(vol, 0, 255), handle_) == 0); }
		const auto vol(void) const noexcept { return GetVolumeSoundMem2(handle_); }
		bool SetPosition(const VECTOR_ref& pos) const noexcept { return (Set3DPositionSoundMem(pos.get(), handle_) == 0); }
		bool Radius(float radius) const noexcept { return (Set3DRadiusSoundMem(radius, handle_) == 0); }
		void play_3D(const VECTOR_ref& pos, float radius, int type_t = DX_PLAYTYPE_BACK)const {
			SetPosition(pos);
			Radius(radius);
			play(type_t, TRUE);
		}
		static SoundHandle Load(std::basic_string_view<TCHAR> FileName, int BufferNum = 3) {
			return { DxLib::LoadSoundMemWithStrLen(FileName.data(), FileName.length(), BufferNum) };
		}
	};

	//サウンドプール
	class SoundPool : public SingletonBase<SoundPool> {
	private:
		friend class SingletonBase<SoundPool>;
	private:
		class Soundhave {
			class handles {
			public:
				std::string path;
				std::vector<SoundHandle> handle;
			};

			int ID{ 0 };
			std::vector<handles> shandle;
			size_t now = 0;
			int Set_vol = 255;
			float vol_rate = 1.f;
		public:
			const auto&		GetHandles(void)const noexcept { return shandle; }
			const auto&		Get_ID(void)const noexcept { return ID; }
			void			Set(int ID_t, size_t buffersize, std::string path_t, bool is3Dsound = true) {
				if (path_t == "") { return; }
				for (auto& h : this->shandle) {
					if (h.path == path_t) { return; }
				}
				this->ID = ID_t;
				this->shandle.resize(this->shandle.size() + 1);
				this->shandle.back().path = path_t;
				this->shandle.back().handle.resize(buffersize);
				if (is3Dsound) {
					SetCreate3DSoundFlag(TRUE);
				}
				this->shandle.back().handle[0] = SoundHandle::Load(this->shandle.back().path);
				for (size_t i = 1; i < this->shandle.back().handle.size(); i++) {
					if (is3Dsound) {
						SetCreate3DSoundFlag(TRUE);
					}
					this->shandle.back().handle[i] = this->shandle.back().handle[0].Duplicate();
				}
				SetCreate3DSoundFlag(FALSE);

				for (size_t i = 0; i < this->shandle.back().handle.size(); i++) {
					Set3DPresetReverbParamSoundMem(DX_REVERB_PRESET_MOUNTAINS, this->shandle.back().handle[i].get());
				}

			}
			void			Delete() {
				for (auto& h : shandle) {
					h.handle.clear();
				}
				shandle.clear();
			}
			void			StopAll(int Sel_t) {
				for (auto& h : shandle[Sel_t].handle) {
					h.stop();
				}
			}
			auto			Play(int Sel_t, int type_t = DX_PLAYTYPE_BACK, int Flag_t = 1, int vol_t = -1, int panpal = -256) {
				auto ans = now;
				shandle[Sel_t].handle[now].play(type_t, Flag_t);
				if (vol_t != -1) {
					Set_vol = vol_t;
					shandle[Sel_t].handle[now].vol((int)(vol_rate * Set_vol));
				}
				if (panpal != -256) {
					ChangePanSoundMem(panpal, shandle[Sel_t].handle[now].get());
				}
				++now %= shandle[Sel_t].handle.size();
				return (int)ans;
			}
			int 			Play_3D(int Sel_t, const VECTOR_ref& pos_t, float radius, int vol_t = -1, int type_t = DX_PLAYTYPE_BACK) noexcept {
				bool isplay = true;
				{
					//距離内にいない場合鳴らさない
					//float dist = (pos_t - GetCameraPosition()).size();
					//isplay = (dist < radius);
				}
				if (isplay) {
					auto ans = now;
					shandle[Sel_t].handle[now].play_3D(pos_t, radius, type_t);
					if (vol_t != -1) {
						Set_vol = vol_t;
						shandle[Sel_t].handle[now].vol((int)(vol_rate * Set_vol));
					}
					++now %= shandle[Sel_t].handle.size();
					return (int)ans;
				}
				return -1;
			}
			void			SetVol_Local(int Sel_t, int Sel2_t, int vol) {
				shandle[Sel_t].handle[Sel2_t].vol((int)(vol_rate * std::clamp(vol, 0, 255)));
			}
			void			SetPos(int Sel_t, int Sel2_t, const VECTOR_ref& pos_t) {
				shandle[Sel_t].handle[Sel2_t].SetPosition(pos_t);
			}
			void			SetVol_Local(int vol) {
				Set_vol = std::clamp(vol, 0, 255);
				for (auto& sh : this->shandle) {
					for (auto& h : sh.handle) {
						h.vol((int)(vol_rate * Set_vol));
					}
				}
			}
			void			SetVol(float vol) {
				vol_rate = std::clamp(vol, 0.f, 1.f);
				for (auto& sh : this->shandle) {
					for (auto& h : sh.handle) {
						h.vol((int)(vol_rate * Set_vol));
					}
				}
			}
		};
	private:
		std::vector<Soundhave> havehandle;
	public:
		void			SetVol(float vol) {
			for (auto& h : this->havehandle) {
				h.SetVol(vol);
			}
		}
	public:
		size_t			Add(int ID_t, size_t buffersize = 1, std::string path_t = "", bool is3Dsound = true) {
			for (auto& h : this->havehandle) {
				if (h.Get_ID() == ID_t) {
					h.Set(ID_t, buffersize, path_t, is3Dsound);
					return &h - &this->havehandle.front();
				}
			}
			this->havehandle.resize(this->havehandle.size() + 1);
			this->havehandle.back().Set(ID_t, buffersize, path_t, is3Dsound);
			return this->havehandle.size() - 1;
		}
		Soundhave&		Get(int ID_t) { return this->havehandle[Add(ID_t)]; }
		void			Delete(int ID_t) {
			for (int i = 0; i < (int)this->havehandle.size(); i++) {
				auto& h = this->havehandle[i];
				if (h.Get_ID() == ID_t) {
					h.StopAll(0);
					h.Delete();
					std::swap(h, this->havehandle.back());
					this->havehandle.pop_back();
					i--;
				}
			}
		}
	};


	//サウンドプール
	class BGMPool : public SingletonBase<BGMPool> {
	private:
		friend class SingletonBase<BGMPool>;
	private:
		class BGMhave {
			int ID{0};
			std::string path;
			SoundHandle handle;
			int Set_vol = 255;
			float vol_rate = 1.f;
		public:
			//const auto&		GetHandles(void)const noexcept { return shandle; }
			const auto&		Get_ID(void)const noexcept { return ID; }
			void			Set(int ID_t, std::string path_t) {
				if (path_t == "") { return; }
				if (path_t == path) { return; }
				this->ID = ID_t;
				this->path = path_t;
				SetCreate3DSoundFlag(FALSE);
				this->handle = SoundHandle::Load(this->path);
			}
			bool			Check() {
				return this->handle.check();
			}
			void			Delete() {
				this->handle.Dispose();
			}
			void			Stop() {
				this->handle.stop();
			}
			void			Play(int type_t = DX_PLAYTYPE_BACK, int Flag_t = 1) {
				this->handle.play(type_t, Flag_t);
			}
			void			SetVol_Local(int vol) {
				Set_vol = std::clamp(vol, 0, 255);
				this->handle.vol((int)(vol_rate * Set_vol));
			}
			void			SetVol(float vol) {
				vol_rate = std::clamp(vol, 0.f, 1.f);
				this->handle.vol((int)(vol_rate * Set_vol));
			}
		};
	private:
		std::vector<BGMhave> havehandle;
	public:
		void			SetVol(float vol) {
			for (auto& h : this->havehandle) {
				h.SetVol(vol);
			}
		}
	public:
		size_t			Add(int ID_t, std::string path_t = "") {
			for (auto& h : this->havehandle) {
				if (h.Get_ID() == ID_t) {
					h.Set(ID_t, path_t);
					return &h - &this->havehandle.front();
				}
			}
			this->havehandle.resize(this->havehandle.size() + 1);
			this->havehandle.back().Set(ID_t, path_t);
			return this->havehandle.size() - 1;
		}
		BGMhave&		Get(int ID_t) { return this->havehandle[Add(ID_t)]; }
		void			Delete(int ID_t) {
			for (int i = 0; i < (int)this->havehandle.size(); i++) {
				auto& h = this->havehandle[i];
				if (h.Get_ID() == ID_t) {
					h.Stop();
					h.Delete();
					std::swap(h, this->havehandle.back());
					this->havehandle.pop_back();
					i--;
				}
			}
		}
		void			StopAll() {
			for (auto& h : this->havehandle) {
				h.Stop();
			}
		}
	};

};
