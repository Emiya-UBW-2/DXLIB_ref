﻿#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	// 共通のSEのEnum
	enum class SoundSelectCommon {
		UI_Select,
		UI_CANCEL,
		UI_OK,
		UI_NG,
		Num,
	};
	//サウンドの分類
	enum class SoundType {
		SE,
		BGM,
	};
	// 1音声を管理するハンドル
	class SoundHandle : public DXHandle {
	protected:
		// 破棄する際に呼ばれる関数
		void	Dispose_Sub(void) noexcept override {
			DeleteSoundMem(DXHandle::get());
		}
	public:// 変更系
		// サウンドを再生
		bool	Play(int type, int flag = 1) const noexcept { return (PlaySoundMem(DXHandle::get(), type, flag) == 0); }
		void	Play3D(const Vector3DX& pos, float radius, int type_t = DX_PLAYTYPE_BACK)const {
			SetPosition(pos);
			Radius(radius);
			Play(type_t, TRUE);
		}
		// サウンドを停止
		bool	Stop(void) const noexcept { return (InvalidID != DXHandle::get()) ? (StopSoundMem(DXHandle::get()) == 0) : false; }
		// サウンドの音量を設定(0~255)
		bool	SetVol(int vol) const noexcept { return (ChangeVolumeSoundMem(std::clamp(vol, 0, 255), DXHandle::get()) == 0); }
		// サウンドの左右バランスを決定(-255~255)
		bool	Pan(int panpal) const noexcept { return (ChangePanSoundMem(panpal, DXHandle::get()) == 0); }

		bool	SetPosition(const Vector3DX& pos) const noexcept { return (Set3DPositionSoundMem(pos.get(), DXHandle::get()) == 0); }
		bool	Radius(float radius) const noexcept { return (Set3DRadiusSoundMem(radius, DXHandle::get()) == 0); }
		bool	PresetReverbParam(int PresetNo) const noexcept { return (Set3DPresetReverbParamSoundMem(PresetNo, DXHandle::get()) == 0); }
	public:// 取得系
		// サウンドが再生中かどうかを取得
		bool	CheckPlay(void) const noexcept { return (DxLib::CheckSoundMem(DXHandle::get()) == TRUE); }
		// ミリ秒単位で総裁性時間を取得
		LONGLONG GetTotalTIme(void) const noexcept { return DxLib::GetSoundTotalTime(DXHandle::get()); }
		// サウンドの音量を取得
		int		GetVol(void) const noexcept { return GetVolumeSoundMem2(DXHandle::get()); }
	public:// 読み込み系
		// ファイル名からサウンドを読み込み
		void Load(const std::basic_string<TCHAR>& FileName, int BufferNum = 3) noexcept {
			DXHandle::SetHandleDirect(DxLib::LoadSoundMemWithStrLen(FileName.data(), FileName.length(), BufferNum));
		}
		// 渡されたサウンドハンドルと同じデータを自分にも複製して別ハンドルとして持つ
		void Duplicate(const SoundHandle& o) noexcept {
			DXHandle::SetHandleDirect(DxLib::DuplicateSoundMem(o.get()));
		}
	};
	// 同じ音声を多重に持つクラス(1ハンドルで鳴らせる音は一つであるため)
	class SoundHandles {
	private:
		std::vector<SoundHandle> m_HandleList;
		size_t Size = 0;
		size_t nowSelect = 0;
	public:
		// コンストラクタ
		SoundHandles(size_t buffersize, std::string path_t, bool is3Dsound = true) noexcept {
			if (is3Dsound) {
				SetCreate3DSoundFlag(TRUE);
			}
			this->Size = buffersize;
			this->m_HandleList.resize(this->Size);
			this->m_HandleList[0].Load(path_t);
			for (size_t loop = 1; loop < this->Size; ++loop) {
				this->m_HandleList[loop].Duplicate(this->m_HandleList[0]);
			}
			SetCreate3DSoundFlag(FALSE);
		}
		SoundHandles(void) noexcept = delete;
		SoundHandles(const SoundHandles&) = delete;// コピーしてはいけないのですべてdelete
		SoundHandles(SoundHandles&&) = delete;
		SoundHandles& operator=(const SoundHandles&) = delete;
		SoundHandles& operator=(SoundHandles&&) = delete;
		// デストラクタ
		~SoundHandles(void) noexcept {
			StopAll();
			this->m_HandleList.clear();
		}
	public:
		// サウンドが再生中かどうかを取得
		bool	CheckPlay(void) const noexcept { return this->m_HandleList[this->nowSelect].CheckPlay(); }
		// サウンドを一つ再生
		const auto		Play(int type_t = DX_PLAYTYPE_BACK, int Flag_t = 1, int panpal = -256) noexcept {
			auto Answer = this->nowSelect;
			auto& NowHandle = this->m_HandleList[this->nowSelect];
			NowHandle.Play(type_t, Flag_t);
			if (panpal != -256) { NowHandle.Pan(panpal); }
			++this->nowSelect %= this->Size;
			return Answer;
		}
		const auto		Play3D(const Vector3DX& pos_t, float radius, int type_t = DX_PLAYTYPE_BACK) noexcept {
			auto Answer = this->nowSelect;
			this->m_HandleList[this->nowSelect].Play3D(pos_t, radius, type_t);
			++this->nowSelect %= this->Size;
			return Answer;
		}
		void			SetPosition(const Vector3DX& pos_t) noexcept {
			for (auto& h : this->m_HandleList) {
				h.SetPosition(pos_t);
			}
		}
		// サウンドをすべて停止
		void			StopAll(void) noexcept {
			for (auto& h : this->m_HandleList) {
				h.Stop();
			}
		}
		// サウンドの音量をすべて変更
		void			SetVolAll(int vol) noexcept {
			for (auto& h : this->m_HandleList) {
				h.SetVol(vol);
			}
		}
	public:
		// ミリ秒単位で総再生時間を取得
		LONGLONG		GetTotalTIme(void) noexcept { return this->m_HandleList[0].GetTotalTIme(); }
	};
	// SoundTypeごとに分かれた音声管理クラス
	class Soundhave {
	private:
		int								m_SoundID{ 0 };		//サウンドの識別用ID
		std::unique_ptr<SoundHandles>	m_Handles;				//実際に音声を持っているハンドル
		int								m_LocalVolume = 255;	//音声単位で指定できる音量(フェードアウトなどで使用)
		SoundType						m_SoundType{ SoundType::SE };//自分の音声タイプ
	public:
		//コンストラクタ
		Soundhave(int SoundIDSelect, size_t buffersize, std::string path_t, SoundType soundType, bool is3Dsound = true) noexcept {
			// パスに何も描かれていない場合は特に何もしない
			if (path_t == "") { return; }
			this->m_SoundID = SoundIDSelect;
			this->m_Handles = std::make_unique<SoundHandles>(buffersize, path_t, is3Dsound);
			this->m_SoundType = soundType;
			FlipVolume();
		}
		// デストラクタ
		~Soundhave(void) noexcept {
			this->m_Handles.reset();
		}
	public:
		//サウンドの識別用ID
		const auto& GetSoundID(void)const noexcept { return this->m_SoundID; }
	public:
		// サウンドが再生中かどうかを取得
		bool			CheckPlay(void) const noexcept { return this->m_Handles->CheckPlay(); }
		//再生中の音声をすべて停止
		void			StopAll(void) noexcept { this->m_Handles->StopAll(); }
		//再生
		const auto		Play(int type_t = DX_PLAYTYPE_BACK, int Flag_t = TRUE, int panpal = -256) noexcept { return this->m_Handles->Play(type_t, Flag_t, panpal); }
		const auto		Play3D(const Vector3DX& pos_t, float radius, int type_t = DX_PLAYTYPE_BACK) noexcept { return this->m_Handles->Play3D(pos_t, radius, type_t); }
		void			SetPosition(const Vector3DX& pos_t) noexcept { this->m_Handles->SetPosition(pos_t); }
		// 音声の総裁性時間をミリ秒で取得
		LONGLONG		GetTotalTIme(void) noexcept { return this->m_Handles->GetTotalTIme(); }
		// その音量でのみの音量設定
		void			SetLocalVolume(int vol) noexcept {
			this->m_LocalVolume = std::clamp(vol, 0, 255);
			FlipVolume();
		}
		// 保持している音声すべての音量を反映
		void			FlipVolume(void) noexcept;
	};
	// SE,BGMのプール
	class SoundPool : public SingletonBase<SoundPool> {
	private:
		friend class SingletonBase<SoundPool>;
	private:
		//各種サウンドを保持しておくリスト
		std::array<std::vector<std::unique_ptr<Soundhave>>, 2>	m_SoundHas;
	private:
		// コンストラクタ
		SoundPool(void) noexcept {}// コピーしてはいけないので通常のコンストラクタ以外をすべてdelete
		SoundPool(const SoundPool&) = delete;
		SoundPool(SoundPool&&) = delete;
		SoundPool& operator=(const SoundPool&) = delete;
		SoundPool& operator=(SoundPool&&) = delete;
		// デストラクタはシングルトンなので呼ばれません
	public:
		// 特定のIDにサウンドを追加
		void			Add(SoundType Type, int Select, size_t buffersize, std::string path_t, bool is3Dsound = true) noexcept {
			// 既に同じIDが登録されている場合何もしない
			for (auto& h : this->m_SoundHas[static_cast<size_t>(Type)]) {
				if (h->GetSoundID() == Select) {
					return;
				}
			}
			//末尾に追加
			this->m_SoundHas[static_cast<size_t>(Type)].emplace_back(std::make_unique<Soundhave>(Select, buffersize, path_t, Type, is3Dsound));
		}
		// 特定のIDのサウンドを取得
		std::unique_ptr<Soundhave>& Get(SoundType Type, int Select) noexcept {
			for (auto& h : this->m_SoundHas[static_cast<size_t>(Type)]) {
				if (h->GetSoundID() == Select) {
					return h;
				}
			}
			return this->m_SoundHas[static_cast<size_t>(Type)][0];// 探したいサウンドがなかった　エラー処理は特にしていませんので注意
		}
		// 保持している音声すべての音量を反映
		void			FlipVolume(void) noexcept {
			for (auto& s : this->m_SoundHas) {
				for (auto& h : s) {
					h->FlipVolume();
				}
			}
		}
		// 
		void			StopAll(SoundType Type) noexcept {
			for (auto& h : this->m_SoundHas[static_cast<size_t>(Type)]) {
				h->StopAll();
			}
		}
		// 特定のIDのサウンドを探し出して削除
		void			Delete(SoundType Type, int Select) noexcept {
			for (auto& h : this->m_SoundHas[static_cast<size_t>(Type)]) {
				if (h->GetSoundID() == Select) {
					h.reset();
					std::swap(h, this->m_SoundHas[static_cast<size_t>(Type)].back());
					this->m_SoundHas[static_cast<size_t>(Type)].pop_back();
					break;
				}
			}
		}
	};
};
