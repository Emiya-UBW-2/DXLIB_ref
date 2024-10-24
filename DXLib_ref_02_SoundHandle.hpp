/*=============================================================================
 Copyright (C) 2020 yumetodo <yume-wikijp@live.jp>
 Distributed under the Boost Software License, Version 1.0.
 (See https:// www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	// 共通のサウンドEnum
	enum class SoundEnumCommon {
		UI_Select,
		UI_CANCEL,
		UI_OK,
		UI_NG,
		Num,
	};
	// 
	class SoundHandle : public DXHandle {
	protected:
		void	Dispose_Sub(void) noexcept override {
			DeleteSoundMem(DXHandle::get());
		}
	public:
		bool	CheckPlay(void) const noexcept { return (DxLib::CheckSoundMem(DXHandle::get()) == TRUE); }
		bool	Play(int type, int flag = 1) const noexcept { return (PlaySoundMem(DXHandle::get(), type, flag) == 0); }
		bool	Stop(void) const noexcept {
			if (InvalidID != DXHandle::get()) {
				return (StopSoundMem(DXHandle::get()) == 0);
			}
			else {
				return false;
			}
		}
		LONGLONG GetTotalTIme(void) const noexcept { return DxLib::GetSoundTotalTime(DXHandle::get()); }
		bool	SetVol(int vol) const noexcept { return (ChangeVolumeSoundMem(std::clamp<int>(vol, 0, 255), DXHandle::get()) == 0); }
		int		GetVol(void) const noexcept { return GetVolumeSoundMem2(DXHandle::get()); }
		bool	SetPosition(const Vector3DX& pos) const noexcept { return (Set3DPositionSoundMem(pos.get(), DXHandle::get()) == 0); }
		bool	Radius(float radius) const noexcept { return (Set3DRadiusSoundMem(radius, DXHandle::get()) == 0); }

		bool	Pan(int panpal) const noexcept { return (ChangePanSoundMem(panpal, DXHandle::get()) == 0); }
		bool	PresetReverbParam(int PresetNo) const noexcept { return (Set3DPresetReverbParamSoundMem(PresetNo, DXHandle::get()) == 0); }

		void	Play3D(const Vector3DX& pos, float radius, int type_t = DX_PLAYTYPE_BACK)const {
			SetPosition(pos);
			Radius(radius);
			Play(type_t, TRUE);
		}
	public:
		void Load(std::basic_string_view<TCHAR> FileName, int BufferNum = 3) noexcept {
			DXHandle::SetHandleDirect(DxLib::LoadSoundMemWithStrLen(FileName.data(), FileName.length(), BufferNum));
		}
		void Duplicate(const SoundHandle& o) noexcept {
			DXHandle::SetHandleDirect(DxLib::DuplicateSoundMem(o.get()));
		}
	};

	// サウンドプール
	class SoundPool : public SingletonBase<SoundPool> {
	private:
		friend class SingletonBase<SoundPool>;
	private:
		class Soundhave {
			class handles {
			public:
				handles(void) noexcept {}
				handles(const handles&) = delete;
				handles(handles&& o) = delete;
				handles& operator=(const handles&) = delete;
				handles& operator=(handles&& o) = delete;
			public:
				std::string path;
				std::vector<SoundHandle> handle;
			};

			int ID{ 0 };
			std::vector<std::shared_ptr<handles>> shandle;
			size_t now = 0;
			int Set_vol = 255;
			float vol_rate = 1.f;
		public:
			const auto& GetHandles(void)const noexcept { return shandle; }
			const auto& Get_ID(void)const noexcept { return ID; }
			void			Set(int ID_t, size_t buffersize, std::string path_t, bool is3Dsound = true) noexcept {
				if (path_t == "") {
					return;
				}
				for (auto& h : this->shandle) {
					if (h->path == path_t) {
						return;
					}
				}
				this->ID = ID_t;
				this->shandle.emplace_back(std::make_shared<handles>());
				this->shandle.back()->path = path_t;
				this->shandle.back()->handle.resize(buffersize);
				if (is3Dsound) {
					SetCreate3DSoundFlag(TRUE);
				}
				this->shandle.back()->handle.at(0).Load(this->shandle.back()->path);
				for (size_t i : std::views::iota(1, static_cast<int>(this->shandle.back()->handle.size()))) {
					if (is3Dsound) {
						SetCreate3DSoundFlag(TRUE);
					}
					this->shandle.back()->handle.at(static_cast<size_t>(i)).Duplicate(this->shandle.back()->handle.at(0));
				}
				SetCreate3DSoundFlag(FALSE);

				for (size_t i : std::views::iota(0, static_cast<int>(this->shandle.back()->handle.size()))) {
					this->shandle.back()->handle.at(i).PresetReverbParam(DX_REVERB_PRESET_MOUNTAINS);
				}

			}
			void			Delete(void) noexcept {
				for (auto& h : shandle) {
					h->handle.clear();
					h.reset();
				}
				shandle.clear();
			}
			void			StopAll(int Sel_t) noexcept {
				for (auto& h : shandle.at(static_cast<size_t>(Sel_t))->handle) {
					h.Stop();
				}
			}
			auto			Play(int Sel_t, int type_t = DX_PLAYTYPE_BACK, int Flag_t = 1, int vol_t = InvalidID, int panpal = -256) noexcept {
				size_t ans = now;
				shandle.at(static_cast<size_t>(Sel_t))->handle[now].Play(type_t, Flag_t);
				if (vol_t != InvalidID) {
					Set_vol = vol_t;
					shandle.at(static_cast<size_t>(Sel_t))->handle[now].SetVol(static_cast<int>(vol_rate * static_cast<float>(Set_vol)));
				}
				if (panpal != -256) {
					shandle.at(static_cast<size_t>(Sel_t))->handle[now].Pan(panpal);
				}
				++now %= shandle.at(static_cast<size_t>(Sel_t))->handle.size();
				return static_cast<int>(ans);
			}
			int 			Play_3D(int Sel_t, const Vector3DX& pos_t, float radius, int vol_t = InvalidID, int type_t = DX_PLAYTYPE_BACK) noexcept {
				bool isplay = true;
				{
					// 距離内にいない場合鳴らさない
					// float dist = (pos_t - GetCameraPosition()).size();
					// isplay = (dist < radius);
				}
				if (isplay) {
					size_t ans = now;
					shandle.at(static_cast<size_t>(Sel_t))->handle[now].Play3D(pos_t, radius, type_t);
					if (vol_t != InvalidID) {
						Set_vol = vol_t;
						shandle.at(static_cast<size_t>(Sel_t))->handle[now].SetVol(static_cast<int>(vol_rate * static_cast<float>(Set_vol)));
					}
					++now %= shandle.at(static_cast<size_t>(Sel_t))->handle.size();
					return static_cast<int>(ans);
				}
				return InvalidID;
			}
			void			SetVol_Local(int Sel_t, int Sel2_t, int vol) noexcept {
				shandle.at(static_cast<size_t>(Sel_t))->handle[static_cast<size_t>(Sel2_t)].SetVol(static_cast<int>(vol_rate * static_cast<float>(std::clamp(vol, 0, 255))));
			}
			void			SetPos(int Sel_t, int Sel2_t, const Vector3DX& pos_t) noexcept {
				shandle.at(static_cast<size_t>(Sel_t))->handle[static_cast<size_t>(Sel2_t)].SetPosition(pos_t);
			}
			LONGLONG			GetTotalTIme(int Sel_t, int Sel2_t) noexcept {
				return shandle.at(static_cast<size_t>(Sel_t))->handle[static_cast<size_t>(Sel2_t)].GetTotalTIme();
			}
			void			SetVol_Local(int vol) noexcept {
				Set_vol = std::clamp(vol, 0, 255);
				for (auto& sh : this->shandle) {
					for (auto& h : sh->handle) {
						h.SetVol(static_cast<int>(vol_rate * static_cast<float>(Set_vol)));
					}
				}
			}
			void			SetVol(float vol) noexcept {
				vol_rate = std::clamp(vol, 0.f, 1.f);
				for (auto& sh : this->shandle) {
					for (auto& h : sh->handle) {
						h.SetVol(static_cast<int>(vol_rate * static_cast<float>(Set_vol)));
					}
				}
			}
		};
	private:
		std::vector<Soundhave> havehandle;
	private:
		SoundPool(void) noexcept {}
		SoundPool(const SoundPool&) = delete;
		SoundPool(SoundPool&& o) = delete;
		SoundPool& operator=(const SoundPool&) = delete;
		SoundPool& operator=(SoundPool&& o) = delete;

		~SoundPool(void) noexcept {}
	public:
		void			SetVol(float vol) noexcept {
			for (auto& h : this->havehandle) {
				h.SetVol(vol);
			}
		}
	public:
		size_t			Add(int ID_t, size_t buffersize = 1, std::string path_t = "", bool is3Dsound = true) noexcept {
			for (size_t index = 0; auto & h : this->havehandle) {
				if (h.Get_ID() == ID_t) {
					h.Set(ID_t, buffersize, path_t, is3Dsound);
					return index;
				}
				index++;
			}
			this->havehandle.resize(this->havehandle.size() + 1);
			this->havehandle.back().Set(ID_t, buffersize, path_t, is3Dsound);
			return this->havehandle.size() - 1;
		}
		Soundhave& Get(int ID_t) noexcept { return this->havehandle.at(Add(ID_t)); }
		void			Delete(int ID_t) noexcept {
			for (int i = 0, Max = static_cast<int>(this->havehandle.size()); i < Max; i++) {
				auto& h = this->havehandle.at(static_cast<size_t>(i));
				if (h.Get_ID() == ID_t) {
					h.StopAll(0);
					h.Delete();
					std::swap(h, this->havehandle.back());
					this->havehandle.pop_back();
					i--;
					Max--;
				}
			}
		}
	};

	// サウンドプール
	class BGMPool : public SingletonBase<BGMPool> {
	private:
		friend class SingletonBase<BGMPool>;
	private:
		class BGMhave {
			int ID{ 0 };
			std::string path;
			SoundHandle handle;
			int Set_vol = 255;
			float vol_rate = 1.f;
		public:
			BGMhave(void) noexcept {}
			BGMhave(const BGMhave&) = delete;
			BGMhave(BGMhave&& o) = delete;
			BGMhave& operator=(const BGMhave&) = delete;
			BGMhave& operator=(BGMhave&& o) = delete;
		public:
			// const auto&		GetHandles(void)const noexcept { return shandle; }
			const auto& Get_ID(void)const noexcept { return ID; }
			void			Set(int ID_t, std::string path_t, bool is3Dsound = false) noexcept {
				if (path_t == "") {
					return;
				}
				if (path_t == path) {
					return;
				}
				this->ID = ID_t;
				this->path = path_t;
				SetCreate3DSoundFlag(is3Dsound ? TRUE : FALSE);
				this->handle.Load(this->path);
				SetCreate3DSoundFlag(FALSE);
			}
			bool			Check(void) noexcept {
				return this->handle.CheckPlay();
			}
			void			Delete(void) noexcept {
				this->handle.Dispose();
			}
			void			Stop(void) noexcept {
				this->handle.Stop();
			}
			void			Play(int type_t = DX_PLAYTYPE_BACK, int Flag_t = 1) noexcept {
				this->handle.Play(type_t, Flag_t);
			}
			void 			Play_3D(const Vector3DX& pos_t, float radius, int type_t = DX_PLAYTYPE_BACK) noexcept {
				bool isplay = true;
				{
					// 距離内にいない場合鳴らさない
					// float dist = (pos_t - GetCameraPosition()).size();
					// isplay = (dist < radius);
				}
				if (isplay) {
					this->handle.Play3D(pos_t, radius, type_t);
				}
			}
			void			SetVol_Local(int vol) noexcept {
				Set_vol = std::clamp(vol, 0, 255);
				this->handle.SetVol(static_cast<int>(vol_rate * static_cast<float>(Set_vol)));
			}
			void			SetVol(float vol) noexcept {
				vol_rate = std::clamp(vol, 0.f, 1.f);
				this->handle.SetVol(static_cast<int>(vol_rate * static_cast<float>(Set_vol)));
			}
		};
	private:
		BGMPool(void) noexcept {}
		BGMPool(const BGMPool&) = delete;
		BGMPool(BGMPool&& o) = delete;
		BGMPool& operator=(const BGMPool&) = delete;
		BGMPool& operator=(BGMPool&& o) = delete;

		~BGMPool(void) noexcept {}
	private:
		std::vector<std::shared_ptr<BGMhave>> havehandle;
	public:
		void			SetVol(float vol) noexcept {
			for (auto& h : this->havehandle) {
				h->SetVol(vol);
			}
		}
	public:
		size_t						Add(int ID_t, std::string path_t = "", bool is3Dsound = false) noexcept {
			for (size_t index = 0; auto & h : this->havehandle) {
				if (h->Get_ID() == ID_t) {
					h->Set(ID_t, path_t, is3Dsound);
					return index;
				}
				index++;
			}
			this->havehandle.emplace_back(std::make_shared<BGMhave>());
			this->havehandle.back()->Set(ID_t, path_t, is3Dsound);
			return this->havehandle.size() - 1;
		}
		auto& Get(int ID_t) noexcept { return this->havehandle.at(Add(ID_t)); }
		void						Delete(int ID_t) noexcept {
			for (int i = 0, Max = static_cast<int>(this->havehandle.size()); i < Max; i++) {
				auto& h = this->havehandle.at(static_cast<size_t>(i));
				if (h->Get_ID() == ID_t) {
					h->Stop();
					h->Delete();
					std::swap(h, this->havehandle.back());
					this->havehandle.back().reset();
					this->havehandle.pop_back();
					i--;
					Max--;
				}
			}
		}
		void						StopAll(void) noexcept {
			for (auto& h : this->havehandle) {
				h->Stop();
			}
		}
	};

};
