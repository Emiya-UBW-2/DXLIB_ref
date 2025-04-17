#include "DXLib_ref.h"
// #include "DXLib_ref_08_Etc.hpp"

namespace DXLibRef {
	const SaveData* SingletonBase<SaveData>::m_Singleton = nullptr;
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	float GetEasingRatio(EasingType EasingType, float ratio) noexcept {
		auto* DXLib_refParts = DXLib_ref::Instance();
		switch (EasingType) {
		case EasingType::OutExpo:
			return (1.f - std::powf(ratio, FrameRate * DXLib_refParts->GetDeltaTime()));
		default:
			return 1.f;
		}
	}
	static Vector3DX GetScreenPos(const Vector3DX& campos, const Vector3DX& camvec, const Vector3DX& camup, float fov, float near_t, float far_t, const Vector3DX& worldpos) noexcept {
		auto* WindowSizeParts = WindowSizeControl::Instance();
		// ビュー行列と射影行列の取得
		Camera3DInfo tmpcam;
		tmpcam.SetCamPos(campos, camvec, camup);
		tmpcam.SetCamInfo(fov, near_t, far_t);
		// ビューポート行列（スクリーン行列）の作成
		float w = static_cast<float>(WindowSizeParts->GetScreenXMax()) / 2.0f;
		float h = static_cast<float>(WindowSizeParts->GetScreenYMax()) / 2.0f;
		MATRIX viewport = {
			w , 0 , 0 , 0 ,
			0 ,-h , 0 , 0 ,
			0 , 0 , 1 , 0 ,
			w , h , 0 , 1
		};
		// ビュー変換とプロジェクション変換
		Vector3DX tmp = Matrix4x4DX::Vtrans(Matrix4x4DX::Vtrans(worldpos, tmpcam.GetViewMatrix()), tmpcam.GetProjectionMatrix());
		// zで割って-1~1の範囲に収める
		tmp /= tmp.z;
		// スクリーン変換
		Vector3DX screenPos = Matrix4x4DX::Vtrans(tmp, viewport);
		// 深度チェックを簡素に追加
		if (Vector3DX::Dot(camvec - campos, worldpos - campos) > 0.f) {
			screenPos.z = 0.5f;
		}
		else {
			screenPos.z = -1.f;
		}
		return screenPos;
	}
	// 
	void CheckPCSpec::FindCPU(void) noexcept {
		IsEnd = false;
		std::vector<MatchScore> Tmp; Tmp.reserve(64);
		{
			CPUResult.clear();
			if (IsFileExist("CommonData/PassMarkCPU.txt")) {
				Tmp.clear();
				std::array<std::string, 12> TmpString;
				int HitCount = 0;
				int border = 0;
				FileStreamDX FileStream("CommonData/PassMarkCPU.txt");
				while (true) {
					if (FileStream.ComeEof()) { break; }
					auto ALL = FileStream.SeekLineAndGetStr();
					if (ALL == "") {
						continue;
					}
					auto LEFT = FileStreamDX::getleft(ALL);
					auto RIGHT = FileStreamDX::getright(ALL);
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
						Tmp.emplace_back();
						Tmp.back().m_Name = LEFT;
						Tmp.back().m_Score = std::stoi(RIGHT);
						Tmp.back().m_HitCount = HitCount;
					}
				}
				for (auto& n : Tmp) {
					if (n.m_HitCount >= border) {
						CPUResult.emplace_back(n);
					}
				}
			}
		}
		{
			GPUResult.clear();
			if (IsFileExist("CommonData/PassMarkGPU.txt")) {
				Tmp.clear();
				std::array<std::string, 12> TmpString;
				int HitCount = 0;
				int border = 0;
				FileStreamDX FileStream("CommonData/PassMarkGPU.txt");
				while (true) {
					if (FileStream.ComeEof()) { break; }
					auto ALL = FileStream.SeekLineAndGetStr();
					if (ALL == "") {
						continue;
					}
					auto LEFT = FileStreamDX::getleft(ALL);
					auto RIGHT = FileStreamDX::getright(ALL);
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
						Tmp.emplace_back();
						Tmp.back().m_Name = LEFT;
						Tmp.back().m_Score = std::stoi(RIGHT);
						Tmp.back().m_HitCount = HitCount;
					}
				}
				for (auto& n : Tmp) {
					if (n.m_HitCount >= border) {
						GPUResult.emplace_back(n);
					}
				}
			}
		}
		IsEnd = true;
	}
	bool SaveData::Load(void) noexcept {
		this->m_data.clear();
		if (!IsFileExist("Save/new.svf")) {
			return false;
		}
		FileStreamDX FileStream("Save/new.svf");
		while (true) {
			if (FileStream.ComeEof()) { break; }
			auto ALL = FileStream.SeekLineAndGetStr();
			if (ALL == "") {
				continue;
			}
			auto LEFT = FileStreamDX::getleft(ALL);
			auto RIGHT = FileStreamDX::getright(ALL);
			this->m_data.emplace_back(std::make_pair(LEFT, std::stoi(RIGHT)));
		}
		return true;
	}
	// 
};
