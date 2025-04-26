#pragma once
// #include "DXLib_ref.h"

namespace DXLibRef {

	// --------------------------------------------------------------------------------------------------
	// 角度変換
	// --------------------------------------------------------------------------------------------------
	// 角度からラジアンに
	extern void* enabler;// ダミー変数
	template <class T, typename std::enable_if<std::is_arithmetic<T>::value>::type*& = enabler>
	constexpr float deg2rad(T p1) noexcept { return static_cast<float>(p1) * DX_PI_F / 180.f; }
	// ラジアンから角度に
	template <class T, typename std::enable_if<std::is_arithmetic<T>::value>::type*& = enabler>
	constexpr float rad2deg(T p1) noexcept { return static_cast<float>(p1) * 180.f / DX_PI_F; }

	// 余弦定理
	constexpr float GetCosFormula(float a, float b, float c) noexcept {
		if (b + c > a && c + a > b && a + b > c) {
			return std::clamp((b * b + c * c - a * a) / (2.f * b * c), -1.f, 1.f);
		}
		return 1.f;
	}
	// ---------------------------------------------------------------------------------------------
	// Vector3
	// ---------------------------------------------------------------------------------------------
	class Vector3DX {
	public:
		float x{ 0 };
		float y{ 0 };
		float z{ 0 };
	public:
		Vector3DX(void) noexcept : x(0), y(0), z(0) {}
		Vector3DX(VECTOR value) noexcept { this->Set(value.x, value.y, value.z); }
		inline VECTOR get(void) const noexcept { return DxLib::VGet(this->x, this->y, this->z); }					// 変換
		inline static const Vector3DX vget(float x, float y, float z) noexcept { return DxLib::VGet(x, y, z); }	// 入力
		// 
		inline static const Vector3DX back(void) noexcept { return vget(0.f, 0.f, -1.f); }
		inline static const Vector3DX down(void) noexcept { return vget(0.f, -1.f, 0.f); }
		inline static const Vector3DX forward(void) noexcept { return vget(0.f, 0.f, 1.f); }
		inline static const Vector3DX left(void) noexcept { return vget(-1.f, 0.f, 0.f); }

		inline static const Vector3DX one(void) noexcept { return vget(1.f, 1.f, 1.f); }

		inline static const Vector3DX right(void) noexcept { return vget(1.f, 0.f, 0.f); }
		inline static const Vector3DX up(void) noexcept { return vget(0.f, 1.f, 0.f); }
		inline static const Vector3DX zero(void) noexcept { return vget(0.f, 0.f, 0.f); }
		// 
		inline float magnitude(void) const noexcept {
			float Square = sqrMagnitude();
			return Square < 0.0000001f ? 0.0f : std::sqrtf(Square);
		}// サイズ
		inline Vector3DX normalized(void) const noexcept {
			float Size = magnitude();
			if (Size <= 0.0f) {
				return vget(-1.0f, -1.0f, -1.0f);
			}
			return *this / Size;
		}		// 正規化
		inline float sqrMagnitude(void) const noexcept { return this->x * this->x + this->y * this->y + this->z * this->z; }		// サイズ
		// 
		inline bool Equals(const Vector3DX& obj) const noexcept { return *this == obj; }
		inline void Set(float x1, float y1, float z1) noexcept {
			this->x = x1;
			this->y = y1;
			this->z = z1;
		}
		// 
		inline static float			Angle(const Vector3DX& A, const Vector3DX& B) noexcept { return std::acos(Dot(A, B) / (A.magnitude() * B.magnitude())); }
		inline static Vector3DX		ClampMagnitude(const Vector3DX& A, float Limit) noexcept { return A.normalized() * std::clamp(A.magnitude(), 0.f, Limit); }
		inline static Vector3DX		Cross(const Vector3DX& A, const Vector3DX& B) noexcept { return vget(A.y * B.z - A.z * B.y, A.z * B.x - A.x * B.z, A.x * B.y - A.y * B.x); }
		inline static float			Distance(const Vector3DX& A, const Vector3DX& B) noexcept { return (A - B).magnitude(); }
		inline static float			Dot(const Vector3DX& A, const Vector3DX& B) noexcept { return A.x * B.x + A.y * B.y + A.z * B.z; }
		//static Vector3DX			LerpVector3DX(const Vector3DX& A, const Vector3DX& B, float Per) noexcept { return Lerp<Vector3DX>(A, B, Per); }
		//static Vector3DX			LerpUnclampedVector3DX(const Vector3DX& A, const Vector3DX& B, float Per) noexcept { return Lerp<Vector3DX>(A, B, Per); }
		inline static Vector3DX		Max(const Vector3DX& A, const Vector3DX& B) noexcept { return vget(std::max(A.x, B.x), std::max(A.y, B.y), std::max(A.z, B.z)); }
		inline static Vector3DX		Min(const Vector3DX& A, const Vector3DX& B) noexcept { return vget(std::min(A.x, B.x), std::min(A.y, B.y), std::min(A.z, B.z)); }
		// MoveTowards
		inline static Vector3DX		Normalize(const Vector3DX& A) noexcept { return A.normalized(); }
		// OrthoNormalize
		// Project
		// ProjectOnPlane
		inline static Vector3DX		Reflect(const Vector3DX& inDirection, const Vector3DX& inNormal) noexcept { return inDirection + inNormal * (Dot(inNormal, inDirection * -1.f)) * 2.f; }
		// RotateTowards
		inline static Vector3DX		Scale(const Vector3DX& A, const Vector3DX& B) noexcept { return vget((A.x * B.x), (A.y * B.y), (A.z * B.z)); }
		inline static float			SignedAngle(const Vector3DX& A, const Vector3DX& B, const Vector3DX& Axis) noexcept { return Angle(A, B) * (Dot(Cross(Axis, A), B) > 0.f ? 1.f : -1.f); }
		// Slerp
		// SlerpUnclamped
		// SmoothDamp

		// 比較
		inline bool operator==(const Vector3DX& obj) const noexcept { return (*this - obj).magnitude() <= 0.001f; }
		inline bool operator!=(const Vector3DX& obj) const noexcept { return !(*this == obj); }
		// 加算
		inline Vector3DX operator+(const Vector3DX& obj) const noexcept { return vget(this->x + obj.x, this->y + obj.y, this->z + obj.z); }
		inline void operator+=(const Vector3DX& obj) noexcept {
			this->x += obj.x;
			this->y += obj.y;
			this->z += obj.z;
		}
		// 減算
		inline Vector3DX operator-(const Vector3DX& obj) const noexcept { return vget(this->x - obj.x, this->y - obj.y, this->z - obj.z); }
		inline void operator-=(const Vector3DX& obj) noexcept {
			this->x -= obj.x;
			this->y -= obj.y;
			this->z -= obj.z;
		}
		// 乗算
		inline Vector3DX operator*(float p1) const noexcept { return vget(this->x * p1, this->y * p1, this->z * p1); }
		inline void operator*=(float p1) noexcept {
			this->x *= p1;
			this->y *= p1;
			this->z *= p1;
		}
		// 除算
		inline Vector3DX operator/(float p1) const noexcept { return *this * (1.f / p1); }
		inline void operator/=(float p1) noexcept { *this *= (1.f / p1); }
	};
	// ---------------------------------------------------------------------------------------------
	// Vector3
	// ---------------------------------------------------------------------------------------------
	class Vector2DX {
	public:
		float x{ 0 };
		float y{ 0 };
	public:
		Vector2DX(void) noexcept : x(0), y(0) {}
		Vector2DX(VECTOR value) noexcept { this->Set(value.x, value.y); }
		inline VECTOR get(void) const noexcept { return DxLib::VGet(this->x, this->y, 0.f); }				// 変換
		inline static const Vector2DX vget(float x, float y) noexcept { return DxLib::VGet(x, y, 0.f); }	// 入力
		// 
		static const Vector2DX down(void) noexcept { return vget(0.f, -1.f); }
		static const Vector2DX left(void) noexcept { return vget(-1.f, 0.f); }
		static const Vector2DX one(void) noexcept { return vget(1.f, 1.f); }
		static const Vector2DX right(void) noexcept { return vget(1.f, 0.f); }
		static const Vector2DX up(void) noexcept { return vget(0.f, 1.f); }
		static const Vector2DX zero(void) noexcept { return vget(0.f, 0.f); }
		// 
		inline float magnitude(void) const noexcept {
			float Square = sqrMagnitude();
			return Square < 0.0000001f ? 0.0f : std::sqrtf(Square);
		}// サイズ
		inline Vector2DX normalized(void) const noexcept {
			float Size = magnitude();
			if (Size <= 0.0f) {
				return vget(-1.0f, -1.0f);
			}
			return *this / Size;
		}		// 正規化
		inline float sqrMagnitude(void) const noexcept { return this->x * this->x + this->y * this->y; }		// サイズ
		// 
		bool Equals(const Vector2DX& obj) const noexcept { return *this == obj; }
		void Set(float x1, float y1) noexcept {
			this->x = x1;
			this->y = y1;
		}

		inline static float			Angle(const Vector2DX& A, const Vector2DX& B) noexcept { return std::acos(Dot(A, B) / (A.magnitude() * B.magnitude())); }
		inline static Vector2DX		ClampMagnitude(const Vector2DX& A, float Limit) noexcept { return A.normalized() * std::clamp(A.magnitude(), 0.f, Limit); }
		inline static float			Cross(const Vector2DX& A, const Vector2DX& B) noexcept { return A.x * B.y - A.y * B.x; }
		inline static float			Distance(const Vector2DX& A, const Vector2DX& B) noexcept { return (A - B).magnitude(); }
		inline static float			Dot(const Vector2DX& A, const Vector2DX& B) noexcept { return A.x * B.x + A.y * B.y; }
		//static Vector2DX			LerpVector2DX(const Vector2DX& A, const Vector2DX& B, float Per) noexcept { return Lerp<Vector2DX>(A, B, Per); }
		//static Vector2DX			LerpUnclampedVector2DX(const Vector2DX& A, const Vector2DX& B, float Per) noexcept { return Lerp<Vector2DX>(A, B, Per); }
		inline static Vector2DX		Max(const Vector2DX& A, const Vector2DX& B) noexcept { return vget(std::max(A.x, B.x), std::max(A.y, B.y)); }
		inline static Vector2DX		Min(const Vector2DX& A, const Vector2DX& B) noexcept { return vget(std::min(A.x, B.x), std::min(A.y, B.y)); }
		// MoveTowards
		inline static Vector2DX		Normalize(const Vector2DX& A) noexcept { return A.normalized(); }
		// OrthoNormalize
		// Project
		// ProjectOnPlane
		inline static Vector2DX		Reflect(const Vector2DX& inDirection, const Vector2DX& inNormal) noexcept { return inDirection + inNormal * (Dot(inNormal, inDirection * -1.f)) * 2.f; }
		// RotateTowards
		inline static Vector2DX		Scale(const Vector2DX& A, const Vector2DX& B) noexcept { return vget((A.x * B.x), (A.y * B.y)); }
		inline static float			SignedAngle(const Vector2DX& A, const Vector2DX& B) { return std::atan2f(Cross(A, B), Dot(A, B)); }
		// Slerp
		// SlerpUnclamped
		// SmoothDamp

		// 比較
		inline bool operator==(const Vector2DX& obj) const noexcept { return (*this - obj).magnitude() <= 0.001f; }
		inline bool operator!=(const Vector2DX& obj) const noexcept { return !(*this == obj); }
		// 加算
		inline Vector2DX operator+(const Vector2DX& obj) const noexcept { return vget(this->x + obj.x, this->y + obj.y); }
		inline void operator+=(const Vector2DX& obj) noexcept {
			this->x += obj.x;
			this->y += obj.y;
		}
		// 減算
		inline Vector2DX operator-(const Vector2DX& obj) const noexcept { return vget(this->x - obj.x, this->y - obj.y); }
		inline void operator-=(const Vector2DX& obj) noexcept {
			this->x -= obj.x;
			this->y -= obj.y;
		}
		// 乗算
		inline Vector2DX operator*(float p1) const noexcept { return vget(this->x * p1, this->y * p1); }
		inline void operator*=(float p1) noexcept {
			this->x *= p1;
			this->y *= p1;
		}
		// 除算
		inline Vector2DX operator/(float p1) const noexcept { return *this * (1.f / p1); }
		inline void operator/=(float p1) noexcept { *this *= (1.f / p1); }
	};
	// ---------------------------------------------------------------------------------------------
	// Matrix 4x4バージョン
	// ---------------------------------------------------------------------------------------------
	class Matrix4x4DX {
		MATRIX value;
	public:
		Matrix4x4DX(void) noexcept : value(DxLib::MGetIdent()) {}
		Matrix4x4DX(MATRIX value) noexcept { this->value = value; }
		MATRIX get(void) const noexcept { return this->value; }		// 変換
		// 
		static Matrix4x4DX identity(void) noexcept { return DxLib::MGetIdent(); }

		static Matrix4x4DX Axis1(const Vector3DX& yvec, const Vector3DX& zvec, const Vector3DX& pos = Vector3DX::zero()) noexcept { return { DxLib::MGetAxis1(Vector3DX::Cross(yvec, zvec).get(),yvec.get(),zvec.get(),pos.get()) }; }
		static Matrix4x4DX Axis2(const Vector3DX& yvec, const Vector3DX& zvec, const Vector3DX& pos = Vector3DX::zero()) noexcept { return { DxLib::MGetAxis2(Vector3DX::Cross(yvec, zvec).get(),yvec.get(),zvec.get(),pos.get()) }; }
		static Matrix4x4DX RotAxis(const Vector3DX& p1, float p2) noexcept { return DxLib::MGetRotAxis(p1.get(), p2); }
		static Matrix4x4DX RotVec2(const Vector3DX& p1, const Vector3DX& p2) noexcept { return { DxLib::MGetRotVec2(p1.get(), p2.get()) }; }
		static Matrix4x4DX GetScale(float scale) noexcept { return { DxLib::MGetScale(DxLib::VGet(scale,scale,scale)) }; }
		static Matrix4x4DX GetScale(const Vector3DX& scale) noexcept { return { DxLib::MGetScale(scale.get()) }; }
		static Matrix4x4DX Mtrans(const Vector3DX& p1) noexcept { return DxLib::MGetTranslate(p1.get()); }
		static Vector3DX Vtrans(const Vector3DX& p1, const Matrix4x4DX& p2) noexcept { return DxLib::VTransform(p1.get(), p2.get()); }
		// 
		Matrix4x4DX inverse(void) const noexcept { return DxLib::MInverse(this->get()); }		// 逆
		bool isIdentity(void) const noexcept { return *this == DxLib::MGetIdent(); }		// 逆
		// lossyScale
		Vector3DX lossyScale(void) const noexcept { return DxLib::MGetSize(this->get()); }
		Matrix4x4DX rotation(void) const noexcept { return DxLib::MGetRotElem(this->get()); }		// 逆
		Matrix4x4DX transpose(void) const noexcept { return DxLib::MTranspose(this->get()); }		// 逆

		Vector3DX pos(void) const noexcept { return Vtrans(Vector3DX::zero(), this->get()); }
		Vector3DX xvec(void) const noexcept { return Vtrans(Vector3DX::right(), rotation()); }
		Vector3DX yvec(void) const noexcept { return Vtrans(Vector3DX::up(), rotation()); }
		Vector3DX zvec(void) const noexcept { return Vtrans(Vector3DX::forward(), rotation()); }
		Vector3DX zvec2(void) const noexcept { return Vtrans(Vector3DX::back(), rotation()); }//左手座標系で右手座標系のキャラを描画した際の正面
		// 
		void GetRadian(float* angle_x, float* angle_y, float* angle_z) const noexcept {
			constexpr float threshold = 0.001f;
			if (std::abs(value.m[1][2] - 1.0f) < threshold) { // R(2,1) = sin(x) = 1の時
				if (angle_x) {
					*angle_x = DX_PI_F / 2.f;
				}
				if (angle_y) {
					*angle_y = 0.f;
				}
				if (angle_z) {
					*angle_z = std::atan2f(value.m[0][1], value.m[0][0]);
				}
			}
			else if (std::abs(value.m[1][2] + 1.0f) < threshold) { // R(2,1) = sin(x) = -1の時
				if (angle_x) {
					*angle_x = -DX_PI_F / 2.f;
				}
				if (angle_y) {
					*angle_y = 0.f;
				}
				if (angle_z) {
					*angle_z = std::atan2f(value.m[0][1], value.m[0][0]);
				}
			}
			else {
				if (angle_x) {
					*angle_x = std::asinf(value.m[1][2]);
				}
				if (angle_y) {
					*angle_y = std::atan2f(-value.m[0][2], value.m[2][2]);
				}
				if (angle_z) {
					*angle_z = std::atan2f(-value.m[1][0], value.m[1][1]);
				}
			}
		}
		void SetRadian(float x, float y, float z) noexcept {
			value.m[0][0] = std::cos(y) * std::cos(z) - std::sin(x) * std::sin(y) * std::sin(z);
			value.m[1][0] = -std::cos(x) * std::sin(z);
			value.m[2][0] = std::sin(y) * std::cos(z) + std::sin(x) * std::cos(y) * std::sin(z);
			value.m[0][1] = std::cos(y) * std::sin(z) + std::sin(x) * std::sin(y) * std::cos(z);
			value.m[1][1] = std::cos(x) * std::cos(z);
			value.m[2][1] = std::sin(y) * std::sin(z) - std::sin(x) * std::cos(y) * std::cos(z);
			value.m[0][2] = -std::cos(x) * std::sin(y);
			value.m[1][2] = std::sin(x);
			value.m[2][2] = std::cos(x) * std::cos(y);
		}
		// 比較
		bool operator==(const Matrix4x4DX& obj) const noexcept {
			for (int x : std::views::iota(0, 4)) {
				for (int y : std::views::iota(0, 4)) {
					if (this->get().m[x][y] != obj.get().m[x][y]) {
						return false;
					}
				}
			}
			return true;


		}
		bool operator!=(const Matrix4x4DX& obj) const noexcept { return !(*this == obj); }
		// 乗算
		Matrix4x4DX operator*(const Matrix4x4DX& obj)  const noexcept { return DxLib::MMult(this->get(), obj.get()); }
		void operator*=(const Matrix4x4DX& obj) noexcept { *this = *this * obj; }
	};
	// ---------------------------------------------------------------------------------------------
	// Matrix 3x3バージョン
	// ---------------------------------------------------------------------------------------------
	// 行列構造体
	struct MATRIX33 {
		float					m[3][3];
	};
	// 相互変換
	static void M33toMATRIX(MATRIX* pTarget, const MATRIX33& pAtr) noexcept {
		*pTarget = DxLib::MGetIdent();
		for (int x : std::views::iota(0, 3)) {
			for (int y : std::views::iota(0, 3)) {
				pTarget->m[x][y] = pAtr.m[x][y];
			}
		}
	}
	static void MATRIXtoM33(MATRIX33* pTarget, const MATRIX& pAtr) noexcept {
		for (int x : std::views::iota(0, 3)) {
			for (int y : std::views::iota(0, 3)) {
				pTarget->m[x][y] = pAtr.m[x][y];
			}
		}
	}

	// 単位行列を得る
	static MATRIX33 M33GetIdent(void) noexcept {
		static MATRIX33 Result =
		{
			{
				{ 1.0f, 0.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f }
			}
		};
		return Result;
	}
	// 指定の３軸ローカルのベクトルを基本軸上のベクトルに変換する行列を得る
	// x' = XAxis.x * x + YAixs.x * y + ZAxis.z * z + Pos.x
	// y' = XAxis.y * x + YAixs.y * y + ZAxis.y * z + Pos.y
	// z' = XAxis.z * x + YAixs.z * y + ZAxis.z * z + Pos.z
	static MATRIX33 M33GetAxis1(const Vector3DX& XAxis, const Vector3DX& YAxis, const Vector3DX& ZAxis) noexcept {
		MATRIX33 Result =
		{
			{
				{ XAxis.x, XAxis.y, XAxis.z },
				{ YAxis.x, YAxis.y, YAxis.z },
				{ ZAxis.x, ZAxis.y, ZAxis.z }
			}
		};
		return Result;
	}
	// 基本軸上のベクトルを指定の３軸上に投影したベクトルに変換する行列を得る
	// x' = XAxis.x * ( x - Pos.x ) + XAxis.y * ( x - Pos.x ) + XAxis.z * ( x - Pos.x )
	// y' = YAxis.x * ( x - Pos.x ) + YAxis.y * ( x - Pos.x ) + YAxis.z * ( x - Pos.x )
	// z' = ZAxis.x * ( x - Pos.x ) + ZAxis.y * ( x - Pos.x ) + ZAxis.z * ( x - Pos.x )
	static MATRIX33 M33GetAxis2(const Vector3DX& XAxis, const Vector3DX& YAxis, const Vector3DX& ZAxis) noexcept {
		MATRIX33 Result =
		{
			{
				{ XAxis.x, YAxis.x, ZAxis.x },
				{ XAxis.y, YAxis.y, ZAxis.y },
				{ XAxis.z, YAxis.z, ZAxis.z }
			}
		};
		return Result;
	}
	// 指定軸で指定角度回転する行列を得る
	static MATRIX33 M33GetRotAxis(const Vector3DX& RotateAxis, float Rotate) noexcept {
		MATRIX33 Result{};
		Vector3DX xv = Vector3DX::right(), yv = Vector3DX::up(), zv = Vector3DX::forward();
		Vector3DX xv2{}, yv2{}, zv2{}, xv3{}, yv3{}, zv3{};
		float f{}, Sin{}, Cos{};

		zv2 = RotateAxis.normalized();
		yv2 = VGet(0.0f, 1.0f, 0.0f);
		if (Vector3DX::Cross(yv2, zv2).sqrMagnitude() < 0.00001f) {
			yv2 = VGet(0.0f, 0.0f, 1.0f);
		}
		xv2 = Vector3DX::Cross(zv2, yv2);
		yv2 = Vector3DX::Cross(xv2, zv2).normalized();
		xv2 = xv2.normalized();

		xv3.x = xv2.x * xv.x + xv2.y * xv.y + xv2.z * xv.z;
		xv3.y = yv2.x * xv.x + yv2.y * xv.y + yv2.z * xv.z;
		xv3.z = zv2.x * xv.x + zv2.y * xv.y + zv2.z * xv.z;

		yv3.x = xv2.x * yv.x + xv2.y * yv.y + xv2.z * yv.z;
		yv3.y = yv2.x * yv.x + yv2.y * yv.y + yv2.z * yv.z;
		yv3.z = zv2.x * yv.x + zv2.y * yv.y + zv2.z * yv.z;

		zv3.x = xv2.x * zv.x + xv2.y * zv.y + xv2.z * zv.z;
		zv3.y = yv2.x * zv.x + yv2.y * zv.y + yv2.z * zv.z;
		zv3.z = zv2.x * zv.x + zv2.y * zv.y + zv2.z * zv.z;

		Sin = std::sinf(Rotate);
		Cos = std::cosf(Rotate);

		f = xv3.x * Cos - xv3.y * Sin;
		xv3.y = xv3.x * Sin + xv3.y * Cos;
		xv3.x = f;

		f = yv3.x * Cos - yv3.y * Sin;
		yv3.y = yv3.x * Sin + yv3.y * Cos;
		yv3.x = f;

		f = zv3.x * Cos - zv3.y * Sin;
		zv3.y = zv3.x * Sin + zv3.y * Cos;
		zv3.x = f;

		Result.m[0][0] = xv2.x * xv3.x + yv2.x * xv3.y + zv2.x * xv3.z;
		Result.m[1][0] = xv2.y * xv3.x + yv2.y * xv3.y + zv2.y * xv3.z;
		Result.m[2][0] = xv2.z * xv3.x + yv2.z * xv3.y + zv2.z * xv3.z;

		Result.m[0][1] = xv2.x * yv3.x + yv2.x * yv3.y + zv2.x * yv3.z;
		Result.m[1][1] = xv2.y * yv3.x + yv2.y * yv3.y + zv2.y * yv3.z;
		Result.m[2][1] = xv2.z * yv3.x + yv2.z * yv3.y + zv2.z * yv3.z;

		Result.m[0][2] = xv2.x * zv3.x + yv2.x * zv3.y + zv2.x * zv3.z;
		Result.m[1][2] = xv2.y * zv3.x + yv2.y * zv3.y + zv2.y * zv3.z;
		Result.m[2][2] = xv2.z * zv3.x + yv2.z * zv3.y + zv2.z * zv3.z;

		return Result;
	}
	// In1 の向きから In2 の向きへ変換する回転行列を得る
	static MATRIX33 M33GetRotVec2(const Vector3DX& In1, const Vector3DX& In2) noexcept {
		Vector3DX av = Vector3DX::Cross(In1, In2);
		if (av.sqrMagnitude() < 0.0000001f) {
			av = Vector3DX::right();
		}
		float rad = VRad(In1.get(), In2.get());
		return M33GetRotAxis(av, rad);
	}

	// 逆行列を作成する
	static int CreateInverseMatrix33(MATRIX33* Out, const MATRIX33& In) {
		float detA =
			In.m[0][0] * In.m[1][1] * In.m[2][2] +
			In.m[0][1] * In.m[1][2] * In.m[2][0] +
			In.m[0][2] * In.m[1][0] * In.m[2][1] -
			In.m[0][0] * In.m[1][2] * In.m[2][1] -
			In.m[0][1] * In.m[1][0] * In.m[2][2] -
			In.m[0][2] * In.m[1][1] * In.m[2][0];

		if (detA < 0.0000001f && detA > -0.0000001f) {
			return FALSE;
		}

		Out->m[0][0] =
			(
				In.m[1][1] * In.m[2][2] -
				In.m[1][2] * In.m[2][1] 
				) / detA;

		Out->m[0][1] =
			(
				
				In.m[0][2] * In.m[2][1] -
				In.m[0][1] * In.m[2][2] 
				) / detA;

		Out->m[0][2] =
			(
				In.m[0][1] * In.m[1][2] -
				In.m[0][2] * In.m[1][1] 
				) / detA;

		Out->m[1][0] =
			(
				
				In.m[1][2] * In.m[2][0] -
				In.m[1][0] * In.m[2][2] 
				) / detA;

		Out->m[1][1] =
			(
				In.m[0][0] * In.m[2][2] -
				In.m[0][2] * In.m[2][0] 
				) / detA;

		Out->m[1][2] =
			(
				
				In.m[0][2] * In.m[1][0] -
				In.m[0][0] * In.m[1][2] 
				) / detA;

		Out->m[2][0] =
			(
				In.m[1][0] * In.m[2][1] -
				In.m[1][1] * In.m[2][0]
				) / detA;

		Out->m[2][1] =
			(
				
				In.m[0][1] * In.m[2][0] -
				In.m[0][0] * In.m[2][1]
				) / detA;

		Out->m[2][2] =
			(
				In.m[0][0] * In.m[1][1] -
				In.m[0][1] * In.m[1][0]
				) / detA;

		return TRUE;
	}
	// 逆行列を得る
	static MATRIX33 M33Inverse(const MATRIX33& InM) {
		MATRIX33 Result;

		if (!CreateInverseMatrix33(&Result, InM)) {
			return M33GetIdent();
		}

		return Result;
	}

	// 転置行列を得る
	static MATRIX33 M33Transpose(const MATRIX33& InM) noexcept {
		MATRIX33 Result =
		{
			{
				{ InM.m[0][0], InM.m[1][0], InM.m[2][0] },
				{ InM.m[0][1], InM.m[1][1], InM.m[2][1] },
				{ InM.m[0][2], InM.m[1][2], InM.m[2][2] }
			}
		};
		return Result;
	}
	// 行列の乗算を行う
	static MATRIX33 M33Mult(const MATRIX33& In1, const MATRIX33& In2) noexcept {
		MATRIX33 Result =
		{
			{
				{
					In1.m[0][0] * In2.m[0][0] + In1.m[0][1] * In2.m[1][0] + In1.m[0][2] * In2.m[2][0],
					In1.m[0][0] * In2.m[0][1] + In1.m[0][1] * In2.m[1][1] + In1.m[0][2] * In2.m[2][1],
					In1.m[0][0] * In2.m[0][2] + In1.m[0][1] * In2.m[1][2] + In1.m[0][2] * In2.m[2][2]
				},
				{
					In1.m[1][0] * In2.m[0][0] + In1.m[1][1] * In2.m[1][0] + In1.m[1][2] * In2.m[2][0],
					In1.m[1][0] * In2.m[0][1] + In1.m[1][1] * In2.m[1][1] + In1.m[1][2] * In2.m[2][1],
					In1.m[1][0] * In2.m[0][2] + In1.m[1][1] * In2.m[1][2] + In1.m[1][2] * In2.m[2][2],
				},
				{
					In1.m[2][0] * In2.m[0][0] + In1.m[2][1] * In2.m[1][0] + In1.m[2][2] * In2.m[2][0],
					In1.m[2][0] * In2.m[0][1] + In1.m[2][1] * In2.m[1][1] + In1.m[2][2] * In2.m[2][1],
					In1.m[2][0] * In2.m[0][2] + In1.m[2][1] * In2.m[1][2] + In1.m[2][2] * In2.m[2][2],
				}
			}
		};
		return Result;
	}
	// 行列を使った座標変換
	static const Vector3DX V33Transform(const Vector3DX& InV, const MATRIX33& InM) noexcept {
		Vector3DX Result{};
		Result.x = InV.x * InM.m[0][0] + InV.y * InM.m[1][0] + InV.z * InM.m[2][0];
		Result.y = InV.x * InM.m[0][1] + InV.y * InM.m[1][1] + InV.z * InM.m[2][1];
		Result.z = InV.x * InM.m[0][2] + InV.y * InM.m[1][2] + InV.z * InM.m[2][2];
		return Result;
	}

	class Matrix3x3DX {
		MATRIX33	m_value;
	public:
		Matrix3x3DX(void) noexcept : m_value(M33GetIdent()) {}
		Matrix3x3DX(MATRIX33 value) noexcept { this->m_value = value; }
		MATRIX33 get(void) const noexcept { return this->m_value; }		// 変換
		MATRIX Get44(void) const noexcept {
			MATRIX Result;
			M33toMATRIX(&Result, this->m_value);
			return Result;
		}// 変換
		Matrix4x4DX Get44DX(void) const noexcept {
			MATRIX Result;
			M33toMATRIX(&Result, this->m_value);
			return Result;
		}// 変換
		static Matrix3x3DX Get33DX(const Matrix4x4DX& value) noexcept {
			MATRIX33 Result;
			MATRIXtoM33(&Result, value.get());
			return Result;
		}// 変換
		// 
		static Matrix3x3DX identity(void) noexcept { return M33GetIdent(); }

		static Matrix3x3DX Axis1(const Vector3DX& yvec, const Vector3DX& zvec) noexcept { return { M33GetAxis1(Vector3DX::Cross(yvec, zvec),yvec,zvec) }; }
		static Matrix3x3DX Axis2(const Vector3DX& yvec, const Vector3DX& zvec) noexcept { return { M33GetAxis2(Vector3DX::Cross(yvec, zvec),yvec,zvec) }; }
		static Matrix3x3DX RotAxis(const Vector3DX& p1, float p2) noexcept { return M33GetRotAxis(p1, p2); }
		static Matrix3x3DX RotVec2(const Vector3DX& p1, const Vector3DX& p2) noexcept { return { M33GetRotVec2(p1, p2) }; }
		static Vector3DX Vtrans(const Vector3DX& p1, const Matrix3x3DX& p2) noexcept { return V33Transform(p1, p2.get()); }
		// 
		Matrix3x3DX inverse(void) const noexcept { return M33Inverse(this->get()); }		// 逆
		bool isIdentity(void) const noexcept { return *this == M33GetIdent(); }		// 逆
		// lossyScale
		Matrix3x3DX transpose(void) const noexcept { return M33Transpose(this->get()); }		// 逆

		Vector3DX xvec(void) const noexcept { return Vtrans(Vector3DX::right(), *this); }
		Vector3DX yvec(void) const noexcept { return Vtrans(Vector3DX::up(), *this); }
		Vector3DX zvec(void) const noexcept { return Vtrans(Vector3DX::forward(), *this); }
		Vector3DX zvec2(void) const noexcept { return Vtrans(Vector3DX::back(), *this); }//左手座標系で右手座標系のキャラを描画した際の正面
		// 
		void GetRadian(float* angle_x, float* angle_y, float* angle_z) const noexcept {
			constexpr float threshold = 0.001f;
			if (std::abs(this->m_value.m[1][2] - 1.0f) < threshold) { // R(2,1) = sin(x) = 1の時
				if (angle_x) {
					*angle_x = DX_PI_F / 2.f;
				}
				if (angle_y) {
					*angle_y = 0.f;
				}
				if (angle_z) {
					*angle_z = std::atan2f(this->m_value.m[0][1], this->m_value.m[0][0]);
				}
			}
			else if (std::abs(this->m_value.m[1][2] + 1.0f) < threshold) { // R(2,1) = sin(x) = -1の時
				if (angle_x) {
					*angle_x = -DX_PI_F / 2.f;
				}
				if (angle_y) {
					*angle_y = 0.f;
				}
				if (angle_z) {
					*angle_z = std::atan2f(this->m_value.m[0][1], this->m_value.m[0][0]);
				}
			}
			else {
				if (angle_x) {
					*angle_x = std::asinf(this->m_value.m[1][2]);
				}
				if (angle_y) {
					*angle_y = std::atan2f(-m_value.m[0][2], this->m_value.m[2][2]);
				}
				if (angle_z) {
					*angle_z = std::atan2f(-m_value.m[1][0], this->m_value.m[1][1]);
				}
			}
		}
		void SetRadian(float x, float y, float z) noexcept {
			this->m_value.m[0][0] = std::cos(y) * std::cos(z) - std::sin(x) * std::sin(y) * std::sin(z);
			this->m_value.m[1][0] = -std::cos(x) * std::sin(z);
			this->m_value.m[2][0] = std::sin(y) * std::cos(z) + std::sin(x) * std::cos(y) * std::sin(z);
			this->m_value.m[0][1] = std::cos(y) * std::sin(z) + std::sin(x) * std::sin(y) * std::cos(z);
			this->m_value.m[1][1] = std::cos(x) * std::cos(z);
			this->m_value.m[2][1] = std::sin(y) * std::sin(z) - std::sin(x) * std::cos(y) * std::cos(z);
			this->m_value.m[0][2] = -std::cos(x) * std::sin(y);
			this->m_value.m[1][2] = std::sin(x);
			this->m_value.m[2][2] = std::cos(x) * std::cos(y);
		}
		// 比較
		bool operator==(const Matrix3x3DX& obj) const noexcept {
			for (int x : std::views::iota(0, 3)) {
				for (int y : std::views::iota(0, 3)) {
					if (this->get().m[x][y] != obj.get().m[x][y]) {
						return false;
					}
				}
			}
			return true;
		}
		bool operator!=(const Matrix3x3DX& obj) const noexcept { return !(*this == obj); }
		// 乗算
		Matrix3x3DX operator*(const Matrix3x3DX& obj)  const noexcept { return M33Mult(this->get(), obj.get()); }
		void operator*=(const Matrix3x3DX& obj) noexcept { *this = *this * obj; }
	};

	// 点と矩形との2D判定
	static bool HitPointToRectangle(int xp, int yp, int x1, int y1, int x2, int y2) noexcept { return (xp >= x1 && xp <= x2 && yp >= y1 && yp <= y2); }
	// 矩形と矩形との2D判定
	static bool HitRectangleToRectangle(int xp1, int yp1, int xp2, int yp2, int x1, int y1, int x2, int y2) noexcept { return (xp1 < x2 && x1 < xp2) && (yp1 < y2 && y1 < yp2); }
	// 点と四角形との2D判定
	static bool HitPointToSquare(int xp, int yp, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) noexcept {
		if (0 > Vector2DX::Cross(Vector2DX::vget(static_cast<float>(x2 - x1), static_cast<float>(y2 - y1)), Vector2DX::vget(static_cast<float>(xp - x1), static_cast<float>(yp - y1)))) { return false; }
		if (0 > Vector2DX::Cross(Vector2DX::vget(static_cast<float>(x3 - x2), static_cast<float>(y3 - y2)), Vector2DX::vget(static_cast<float>(xp - x2), static_cast<float>(yp - y2)))) { return false; }
		if (0 > Vector2DX::Cross(Vector2DX::vget(static_cast<float>(x4 - x3), static_cast<float>(y4 - y3)), Vector2DX::vget(static_cast<float>(xp - x3), static_cast<float>(yp - y3)))) { return false; }
		if (0 > Vector2DX::Cross(Vector2DX::vget(static_cast<float>(x1 - x4), static_cast<float>(y1 - y4)), Vector2DX::vget(static_cast<float>(xp - x4), static_cast<float>(yp - y4)))) { return false; }
		return true;
	}
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// 演算補助
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// 球と三角との判定
	static bool GetHitSphereToTriangle(const Vector3DX& pos, float size, const Vector3DX& tri_p1, const Vector3DX& tri_p2, const Vector3DX& tri_p3) noexcept {
		return HitCheck_Sphere_Triangle(pos.get(), size, tri_p1.get(), tri_p2.get(), tri_p3.get()) == TRUE;
	}
	// 三角とカプセルとの判定
	static bool GetHitCapsuleToTriangle(const Vector3DX& startpos, const Vector3DX& endpos, float size, const Vector3DX& tri_p1, const Vector3DX& tri_p2, const Vector3DX& tri_p3) noexcept {
		return HitCheck_Capsule_Triangle(startpos.get(), endpos.get(), size, tri_p1.get(), tri_p2.get(), tri_p3.get()) == TRUE;
	}
	// 球とカプセルとの判定
	static bool GetHitSphereToCapsule(const Vector3DX& pos, float size, const Vector3DX& tri_p1, const Vector3DX& tri_p2, float size2) noexcept {
		return HitCheck_Sphere_Capsule(pos.get(), size, tri_p1.get(), tri_p2.get(), size2) == TRUE;
	}
	// カプセルとカプセルとの判定
	static bool GetHitCheckToCapsule(const Vector3DX& pos1, const Vector3DX& pos2, float size, const Vector3DX& tri_p1, const Vector3DX& tri_p2, float size2) noexcept {
		return HitCheck_Capsule_Capsule(pos1.get(), pos2.get(), size, tri_p1.get(), tri_p2.get(), size2) == TRUE;
	}
	// 直線と直線の一番近い距離
	static float GetMinLenSegmentToSegment(const Vector3DX& startpos, const Vector3DX& endpos, const Vector3DX& tgtstartpos, const Vector3DX& tgtendpos) noexcept {
		return Segment_Segment_MinLength(startpos.get(), endpos.get(), tgtstartpos.get(), tgtendpos.get());
	}
	// 直線と点の一番近い点
	static float GetMinLenSegmentToPoint(const Vector3DX& startpos, const Vector3DX& endpos, const Vector3DX& tgt) noexcept {
		return Segment_Point_MinLength(startpos.get(), endpos.get(), tgt.get());
	}
	// 平面と点の一番近い点
	static Vector3DX GetMinPosSegmentToPoint(const Vector3DX& startpos, const Vector3DX& endpos, const Vector3DX& tgt) noexcept {
		return Plane_Point_MinLength_Position(startpos.get(), endpos.get(), tgt.get());
	}
	// 線分同士の交差判定
	static bool GetSegmenttoSegment(const Vector3DX& SegmentAPos1, const Vector3DX& SegmentAPos2, const Vector3DX& SegmentBPos1, const Vector3DX& SegmentBPos2, SEGMENT_SEGMENT_RESULT* Result) noexcept {
		VECTOR Pos1t = SegmentAPos1.get();
		VECTOR Pos2t = SegmentAPos2.get();
		VECTOR PosAt = SegmentBPos1.get();
		VECTOR PosBt = SegmentBPos2.get();

		Segment_Segment_Analyse(&Pos1t, &Pos2t, &PosAt, &PosBt, Result);
		float len = 0.001f;
		return (Result->SegA_SegB_MinDist_Square <= (len * len));
	}
	// 線分と三角との交差判定
	static bool GetSegmenttoTriangle(const Vector3DX& SegmentAPos1, const Vector3DX& SegmentAPos2, const Vector3DX& SegmentBPos1, const Vector3DX& SegmentBPos2, const Vector3DX& SegmentBPos3, SEGMENT_TRIANGLE_RESULT* Result) noexcept {
		VECTOR Pos1t = SegmentAPos1.get();
		VECTOR Pos2t = SegmentAPos2.get();
		VECTOR PosAt = SegmentBPos1.get();
		VECTOR PosBt = SegmentBPos2.get();
		VECTOR PosCt = SegmentBPos3.get();

		Segment_Triangle_Analyse(&Pos1t, &Pos2t, &PosAt, &PosBt, &PosCt, Result);
		float len = 0.001f;
		return (Result->Seg_Tri_MinDist_Square <= (len * len));
	}
	// 線分と点との交差判定
	static bool GetSegmenttoPoint(const Vector3DX& SegmentAPos1, const Vector3DX& SegmentAPos2, const Vector3DX& PointPos, SEGMENT_POINT_RESULT* Result) noexcept {
		VECTOR Pos1t = SegmentAPos1.get();
		VECTOR Pos2t = SegmentAPos2.get();
		VECTOR PosAt = PointPos.get();

		Segment_Point_Analyse(&Pos1t, &Pos2t, &PosAt, Result);
		float len = 0.001f;
		return (Result->Seg_Point_MinDist_Square <= (len * len));
	}

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*クラス																																	*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// 2次元振り子演算
	class Pendulum2D {
		float	m_PendulumLength = 10.f;
		float	m_PendulumMass = 2.f;
		float	m_drag_coeff = 2.02f;

		float	m_rad = deg2rad(12.f);
		float	m_vel = 0.f;
	public:
		void Init(float Length, float N, float rad) noexcept {
			this->m_PendulumLength = Length;
			this->m_PendulumMass = N;
			this->m_rad = rad;
			this->m_vel = 0.f;
		}
		void Update(float deltaTime) noexcept {
			this->m_vel += (-9.8f / this->m_PendulumLength * std::sin(this->m_rad) - this->m_drag_coeff / this->m_PendulumMass * this->m_vel) * deltaTime;
			this->m_rad += this->m_vel * deltaTime;
		}
	public:
		auto GetRad(void) const noexcept { return this->m_rad; }
		void AddRad(float value) noexcept { this->m_rad += value; }
	};
}