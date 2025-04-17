#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	constexpr int BaseDPI = 96;

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// 補完
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// イージング
	enum class EasingType {
		OutExpo,
	};
	extern float GetEasingRatio(EasingType EasingType, float ratio) noexcept;
	// 線形補完
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
	// Matrix版の線形補完
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
	/*関数																																		*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// マウスと矩形の判定
	extern bool IntoMouse(int x1, int y1, int x2, int y2) noexcept;
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*DXLIBラッパー																																*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// ランダム
	static float GetRandf(float arg) noexcept { return -arg + static_cast<float>(GetRand(static_cast<int>(arg * 2.f * 10000.f))) / 10000.f; }
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
		auto matBase = Matrix3x3DX::Get33DX(pObj->GetParentFrameWorldMatrix(Arm)).inverse();
		// 基準
		auto vec_a1 = Matrix3x3DX::Vtrans((DirPos - pObj->GetFramePosition(Arm)).normalized(), matBase);// 基準
		auto vec_a1L1 = Vector3DX(Vector3DX::vget(XPer, -1.f, vec_a1.y / -abs(vec_a1.z))).normalized();// x=0とする
		float cos_t = GetCosFormula((Vector3DX(pObj->GetFramePosition(Wrist)) - pObj->GetFramePosition(Arm2)).magnitude(), (Vector3DX(pObj->GetFramePosition(Arm2)) - pObj->GetFramePosition(Arm)).magnitude(), (Vector3DX(pObj->GetFramePosition(Arm)) - DirPos).magnitude());
		auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
		// 上腕
		pObj->SetFrameLocalMatrix(Arm, Matrix3x3DX::RotVec2(
			Matrix3x3DX::Vtrans(Vector3DX(pObj->GetFramePosition(Arm2)) - pObj->GetFramePosition(Arm), matBase),
			vec_t
		).Get44DX() * FrameBaseLocalMatArm);
		// 下腕
		matBase = Matrix3x3DX::Get33DX(pObj->GetParentFrameWorldMatrix(Arm2)).inverse();
		pObj->SetFrameLocalMatrix(Arm2, Matrix3x3DX::RotVec2(
			Matrix3x3DX::Vtrans(Vector3DX(pObj->GetFramePosition(Wrist)) - pObj->GetFramePosition(Arm2), matBase),
			Matrix3x3DX::Vtrans(DirPos - pObj->GetFramePosition(Arm2), matBase)
		).Get44DX() * FrameBaseLocalMatArm2);
		// 手
		matBase = Matrix3x3DX::Get33DX(pObj->GetParentFrameWorldMatrix(Wrist)).inverse();
		{
			Matrix3x3DX mat1;
			mat1 = Matrix3x3DX::RotVec2(Matrix3x3DX::Vtrans(Matrix3x3DX::Vtrans(Localzvec, Matrix3x3DX::Get33DX(pObj->GetFrameLocalWorldMatrix(Wrist))), matBase), Matrix3x3DX::Vtrans(Dirzvec, matBase));
			pObj->SetFrameLocalMatrix(Wrist, mat1.Get44DX() * FrameBaseLocalMatWrist);
			auto xvec = Matrix3x3DX::Vtrans(Localyvec, Matrix3x3DX::Get33DX(pObj->GetFrameLocalWorldMatrix(Wrist)));
			mat1 = Matrix3x3DX::RotAxis(Localzvec, Vector3DX::Angle(xvec, Vector3DX::Cross(Diryvec, Dirzvec)) * ((Vector3DX::Dot(Diryvec, xvec) > 0.f) ? -1.f : 1.f)) * mat1;
			pObj->SetFrameLocalMatrix(Wrist, mat1.Get44DX() * FrameBaseLocalMatWrist);
		}
	}

	static void			IK_RightArm(MV1* pObj,
		int Arm, const Matrix4x4DX& FrameBaseLocalMatArm,
		int Arm2, const Matrix4x4DX& FrameBaseLocalMatArm2,
		int Wrist, const Matrix4x4DX& FrameBaseLocalMatWrist,
		const Matrix4x4DX& DirMat) noexcept {
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
			DirMat.pos(), DirMat.yvec() * -1.f, DirMat.zvec());
	}
	static void			IK_LeftArm(MV1* pObj,
		int Arm, const Matrix4x4DX& FrameBaseLocalMatArm,
		int Arm2, const Matrix4x4DX& FrameBaseLocalMatArm2,
		int Wrist, const Matrix4x4DX& FrameBaseLocalMatWrist,
		const Matrix4x4DX& DirMat) noexcept {

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
			DirMat.pos(), DirMat.yvec(), DirMat.zvec());
	}
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// カメラから画面上の座標を取得
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	static Vector3DX GetScreenPos(const Vector3DX& campos, const Vector3DX& camvec, const Vector3DX& camup, float fov, float near_t, float far_t, const Vector3DX& worldpos) noexcept;
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*クラス																																	*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// モデルのフレーム情報保持
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
		void			Set(int frameID, const MV1& obj) noexcept {
			this->m_FrameID = frameID;
			this->m_WorldPos = obj.GetFrameLocalWorldMatrix(frameID);
			if (obj.GetFrameParent(frameID) >= 0) {
				this->m_LocalPos = obj.GetFrameLocalMatrix(frameID);
			}
			else {
				this->m_LocalPos = obj.GetFrameLocalWorldMatrix(frameID);// 
			}
		}
		const auto&		GetFrameID(void) const noexcept { return this->m_FrameID; }
		const auto&		GetFrameWorldPosition(void) const noexcept { return this->m_WorldPos; }
		const auto&		GetFrameLocalPosition(void) const noexcept { return this->m_LocalPos; }
	};

	// 位置情報
	class moves {
		Vector3DX pos;		// 反映用座標
		Vector3DX posbuf;	// 演算用座標
		Vector3DX repos;	// 前フレームの座標
		Vector3DX vec;		// 加速
		Matrix3x3DX mat;	// 回転
		Matrix3x3DX matbuf;	// 回転
	public:
		const auto&		GetPos(void) const noexcept { return pos; }
		const auto&		GetRePos(void) const noexcept { return repos; }
		const auto&		GetVec(void) const noexcept { return vec; }
		const auto&		GetMat(void) const noexcept { return mat; }

		const auto&		GetPosBuf(void) const noexcept { return posbuf; }// 演算以外では使うな
		const auto&		GetMatBuf(void) const noexcept { return matbuf; }// 演算以外では使うな
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
	// 汎用セーブデータ
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	typedef std::pair<std::string, int64_t> SaveParam;
	class SaveData : public SingletonBase<SaveData> {
	private:
		friend class SingletonBase<SaveData>;
	private:
		std::vector<SaveParam>	m_data;
	private:
		SaveData(void) noexcept {
			Load();
		}
		SaveData(const SaveData&) = delete;
		SaveData(SaveData&& o) = delete;
		SaveData& operator=(const SaveData&) = delete;
		SaveData& operator=(SaveData&& o) = delete;
	public:
		SaveParam* GetData(std::string_view Name) noexcept {
			for (auto& d : this->m_data) {
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
				this->m_data.emplace_back(std::make_pair((std::string)Name, value));
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
			for (auto& d : this->m_data) {
				outputfile << d.first + "=" + std::to_string(d.second) + "\n";
			}
			outputfile.close();
		}
		bool Load(void) noexcept;
		void Reset(void) noexcept {
			this->m_data.clear();
		}
	};
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*UDP通信																																	*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class UDPNetWorkDX {
		int			m_Handle{ InvalidID };				// ネットワークハンドル
		IPDATA		m_SendIP{ 127,0,0,1 };		// 送信用ＩＰアドレスデータ
		int			m_SendPort{ InvalidID };				// 通信用ポート
		IPDATA		m_RecvIp{ 127,0,0,1 };			// 受信用ＩＰアドレスデータ
		int			m_RecvPort{ 0 };				// 受信用ポート
	public:
		auto			IsActive(void) const noexcept { return (this->m_Handle != InvalidID); }
	public:
		void			SetServerIP(const IPDATA& pIP) noexcept { this->m_SendIP = pIP; }// クライアントは必ず行う
		bool			Init(bool IsServer, int PORT = InvalidID) noexcept {
			if (!IsActive()) {
				this->m_SendPort = PORT;
				this->m_Handle = MakeUDPSocket(IsServer ? this->m_SendPort : InvalidID);
			}
			return this->m_Handle != InvalidID;
		}
		void			Dispose(void) noexcept {
			if (IsActive()) {
				DeleteUDPSocket(this->m_Handle);	// ＵＤＰソケットハンドルの削除
				this->m_Handle = InvalidID;
				this->m_SendPort = InvalidID;
			}
		}
	private:
		template<class T>
		int				SendData(IPDATA& Ip, int SendPort, const T& Data) noexcept {
			if (IsActive()) {
				return NetWorkSendUDP(this->m_Handle, Ip, SendPort, &Data, sizeof(T));
			}
			return InvalidID;
		}
	public:
		// 送信
		template<class T>
		int				SendData(const T& Data) noexcept { return SendData(this->m_SendIP, this->m_SendPort, Data); }
		// 受信
		template<class T>
		bool			RecvData(T* Data, int* RecvReturn, bool IsPeek) noexcept {
			*RecvReturn = InvalidID;
			if (IsActive()) {
				switch (CheckNetWorkRecvUDP(this->m_Handle)) {
				case TRUE:
					*RecvReturn = NetWorkRecvUDP(this->m_Handle, &this->m_RecvIp, &this->m_RecvPort, Data, sizeof(T), IsPeek ? TRUE : FALSE);		// 受信
					return true;
				case FALSE:// 待機
					break;
				default:// error
					break;
				}
			}
			return false;
		}
		// 返送
		template<class T>
		int				ReturnData(const T& Data) noexcept { return SendData(this->m_RecvIp, this->m_RecvPort, Data); }
	};

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// PCの情報を走査
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
		std::thread					m_thread;
	public:
		CheckPCSpec(void) noexcept {}
		CheckPCSpec(const CheckPCSpec&) = delete;
		CheckPCSpec(CheckPCSpec&& o) = delete;
		CheckPCSpec& operator=(const CheckPCSpec&) = delete;
		CheckPCSpec& operator=(CheckPCSpec&& o) = delete;
		~CheckPCSpec(void) noexcept {
			if (this->m_thread.joinable()) {
				this->m_thread.detach();
			}
		}
	public:
		static void GetOnlyNumber(const char* Target, std::array<std::string, 12>* String) noexcept {
			size_t NumCount = 0;
			for (auto& s : *String) {
				s = "";
			}
			for (size_t loop : std::views::iota(static_cast<size_t>(0), strlenDx(Target))) {
				if (!
					(
						Target[loop] == ' ' ||
						Target[loop] == '-'
						)
					) {
					String->at(NumCount) += Target[loop];
				}
				else {
					if (String->at(NumCount) != "") {
						++NumCount;
						String->at(NumCount) = "";
					}
				}
			}
		}
	public:
		void FindCPU(void) noexcept;
	public:
		const auto*		GetCPUDatas(void) const noexcept { return IsEnd ? &CPUResult : nullptr; }
		const auto*		GetGPUDatas(void) const noexcept { return IsEnd ? &GPUResult : nullptr; }
		const auto&		GetFreeMemorySize(void) const noexcept { return FreeMemorySize; }
		const auto&		GetTotalMemorySize(void) const noexcept { return TotalMemorySize; }
	public:
		void StartSearch(void) noexcept {
			GetPcInfo(NULL, NULL, CPUString, NULL, &FreeMemorySize, &TotalMemorySize, NULL, GPUString, NULL, NULL);
			GetOnlyNumber(CPUString, &CPUStr);
			GetOnlyNumber(GPUString, &GPUStr);
			if (this->m_thread.joinable()) {
				this->m_thread.detach();
			}
			std::thread newThead(&CheckPCSpec::FindCPU, this);
			this->m_thread.swap(newThead);
		}
	};

	static int GetDPI() noexcept {
		int DPI = BaseDPI;
		GetMonitorDpi(NULL, &DPI);
		if (DPI == 0) {
			DPI = BaseDPI;
		}
		return DPI;
	}
}
