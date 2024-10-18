#pragma once
// #include "DXLib_ref.h"

namespace DXLibRef {
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
		inline VECTOR get(void) const noexcept { return DxLib::VGet(this->x, this->y, this->z); }					// ïœä∑
		inline static const Vector3DX vget(float x, float y, float z) noexcept { return DxLib::VGet(x, y, z); }	// ì¸óÕ
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
		}// ÉTÉCÉY
		inline Vector3DX normalized(void) const noexcept {
			float Size = magnitude();
			if (Size <= 0.0f) {
				return vget(-1.0f, -1.0f, -1.0f);
			}
			return *this / Size;
		}		// ê≥ãKâª
		inline float sqrMagnitude(void) const noexcept { return this->x * this->x + this->y * this->y + this->z * this->z; }		// ÉTÉCÉY
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
		// static Vector3DX		Lerp(const Vector3DX& A, const Vector3DX& B, float Per) noexcept { return Lerp<Vector3DX>(A, B, Per); }
		// LerpUnclamped
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
		// SignedAngle
		// Slerp
		// SlerpUnclamped
		// SmoothDamp

		// î‰är
		inline bool operator==(const Vector3DX& obj) const noexcept { return (*this - obj).magnitude() <= 0.001f; }
		inline bool operator!=(const Vector3DX& obj) const noexcept { return !(*this == obj); }
		// â¡éZ
		inline Vector3DX operator+(const Vector3DX& obj) const noexcept { return vget(this->x + obj.x, this->y + obj.y, this->z + obj.z); }
		inline void operator+=(const Vector3DX& obj) noexcept {
			this->x += obj.x;
			this->y += obj.y;
			this->z += obj.z;
		}
		// å∏éZ
		inline Vector3DX operator-(const Vector3DX& obj) const noexcept { return vget(this->x - obj.x, this->y - obj.y, this->z - obj.z); }
		inline void operator-=(const Vector3DX& obj) noexcept {
			this->x -= obj.x;
			this->y -= obj.y;
			this->z -= obj.z;
		}
		// èÊéZ
		inline Vector3DX operator*(float p1) const noexcept { return vget(this->x * p1, this->y * p1, this->z * p1); }
		inline void operator*=(float p1) noexcept {
			this->x *= p1;
			this->y *= p1;
			this->z *= p1;
		}
		// èúéZ
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
		inline VECTOR get(void) const noexcept { return DxLib::VGet(this->x, this->y, 0.f); }				// ïœä∑
		inline static const Vector2DX vget(float x, float y) noexcept { return DxLib::VGet(x, y, 0.f); }	// ì¸óÕ
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
		}// ÉTÉCÉY
		inline Vector2DX normalized(void) const noexcept {
			float Size = magnitude();
			if (Size <= 0.0f) {
				return vget(-1.0f, -1.0f);
			}
			return *this / Size;
		}		// ê≥ãKâª
		inline float sqrMagnitude(void) const noexcept { return this->x * this->x + this->y * this->y; }		// ÉTÉCÉY
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
		// static Vector2DX		Lerp(const Vector2DX& A, const Vector2DX& B, float Per) noexcept { return Lerp<Vector2DX>(A, B, Per); }
		// LerpUnclamped
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
		// SignedAngle
		// Slerp
		// SlerpUnclamped
		// SmoothDamp

		// î‰är
		inline bool operator==(const Vector2DX& obj) const noexcept { return (*this - obj).magnitude() <= 0.001f; }
		inline bool operator!=(const Vector2DX& obj) const noexcept { return !(*this == obj); }
		// â¡éZ
		inline Vector2DX operator+(const Vector2DX& obj) const noexcept { return vget(this->x + obj.x, this->y + obj.y); }
		inline void operator+=(const Vector2DX& obj) noexcept {
			this->x += obj.x;
			this->y += obj.y;
		}
		// å∏éZ
		inline Vector2DX operator-(const Vector2DX& obj) const noexcept { return vget(this->x - obj.x, this->y - obj.y); }
		inline void operator-=(const Vector2DX& obj) noexcept {
			this->x -= obj.x;
			this->y -= obj.y;
		}
		// èÊéZ
		inline Vector2DX operator*(float p1) const noexcept { return vget(this->x * p1, this->y * p1); }
		inline void operator*=(float p1) noexcept {
			this->x *= p1;
			this->y *= p1;
		}
		// èúéZ
		inline Vector2DX operator/(float p1) const noexcept { return *this * (1.f / p1); }
		inline void operator/=(float p1) noexcept { *this *= (1.f / p1); }
	};
	// ---------------------------------------------------------------------------------------------
	// Matrix 4x4ÉoÅ[ÉWÉáÉì
	// ---------------------------------------------------------------------------------------------
	class Matrix4x4DX {
		MATRIX value;
	public:
		Matrix4x4DX(void) noexcept : value(DxLib::MGetIdent()) {}
		Matrix4x4DX(MATRIX value) noexcept { this->value = value; }
		MATRIX get(void) const noexcept { return this->value; }		// ïœä∑
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
		Matrix4x4DX inverse(void) const noexcept { return DxLib::MInverse(this->get()); }		// ãt
		bool isIdentity(void) const noexcept { return *this == DxLib::MGetIdent(); }		// ãt
		// lossyScale
		Vector3DX lossyScale(void) const noexcept { return DxLib::MGetSize(this->get()); }
		Matrix4x4DX rotation(void) const noexcept { return DxLib::MGetRotElem(this->get()); }		// ãt
		Matrix4x4DX transpose(void) const noexcept { return DxLib::MTranspose(this->get()); }		// ãt

		Vector3DX pos(void) const noexcept { return Vtrans(Vector3DX::zero(), this->get()); }
		Vector3DX xvec(void) const noexcept { return Vtrans(Vector3DX::right(), rotation()); }
		Vector3DX yvec(void) const noexcept { return Vtrans(Vector3DX::up(), rotation()); }
		Vector3DX zvec(void) const noexcept { return Vtrans(Vector3DX::forward(), rotation()); }
		// 
		void GetRadian(float* angle_x, float* angle_y, float* angle_z) const noexcept {
			constexpr float threshold = 0.001f;
			if (std::abs(value.m[1][2] - 1.0f) < threshold) { // R(2,1) = sin(x) = 1ÇÃéû
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
			else if (std::abs(value.m[1][2] + 1.0f) < threshold) { // R(2,1) = sin(x) = -1ÇÃéû
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
		// î‰är
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
		// èÊéZ
		Matrix4x4DX operator*(const Matrix4x4DX& obj)  const noexcept { return DxLib::MMult(this->get(), obj.get()); }
		void operator*=(const Matrix4x4DX& obj) noexcept { *this = *this * obj; }
	};
	// ---------------------------------------------------------------------------------------------
	// Matrix 3x3ÉoÅ[ÉWÉáÉì
	// ---------------------------------------------------------------------------------------------
	// çsóÒç\ë¢ëÃ
	struct MATRIX33 {
		float					m[3][3];
	};
	// ëäå›ïœä∑
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

	// íPà çsóÒÇìæÇÈ
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
	// éwíËÇÃÇRé≤ÉçÅ[ÉJÉãÇÃÉxÉNÉgÉãÇäÓñ{é≤è„ÇÃÉxÉNÉgÉãÇ…ïœä∑Ç∑ÇÈçsóÒÇìæÇÈ
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
	// äÓñ{é≤è„ÇÃÉxÉNÉgÉãÇéwíËÇÃÇRé≤è„Ç…ìäâeÇµÇΩÉxÉNÉgÉãÇ…ïœä∑Ç∑ÇÈçsóÒÇìæÇÈ
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
	// éwíËé≤Ç≈éwíËäpìxâÒì]Ç∑ÇÈçsóÒÇìæÇÈ
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
		// _SINCOS_PLATFORM(Rotate, &Sin, &Cos);
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
	// In1 ÇÃå¸Ç´Ç©ÇÁ In2 ÇÃå¸Ç´Ç÷ïœä∑Ç∑ÇÈâÒì]çsóÒÇìæÇÈ
	static MATRIX33 M33GetRotVec2(const Vector3DX& In1, const Vector3DX& In2) noexcept {
		Vector3DX av = Vector3DX::Cross(In1, In2);
		if (av.sqrMagnitude() < 0.0000001f) {
			av = Vector3DX::right();
		}
		float rad = VRad(In1.get(), In2.get());
		return M33GetRotAxis(av, rad);
	}
	// ãtçsóÒÇìæÇÈ
	static MATRIX33 M33Inverse(MATRIX33 InM) noexcept {
		MATRIX In;
		MATRIX33 Result;
		M33toMATRIX(&In, InM);
		In = DxLib::MInverse(In);
		MATRIXtoM33(&Result, In);
		return Result;
	}
	// ì]íuçsóÒÇìæÇÈ
	static MATRIX33 M33Transpose(MATRIX33 InM) noexcept {
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
	// çsóÒÇÃèÊéZÇçsÇ§
	static MATRIX33 M33Mult(MATRIX33 In1, MATRIX33 In2) noexcept {
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
	// çsóÒÇégÇ¡ÇΩç¿ïWïœä∑
	static const Vector3DX V33Transform(const Vector3DX& InV, const MATRIX33& InM) noexcept {
		Vector3DX Result{};
		Result.x = InV.x * InM.m[0][0] + InV.y * InM.m[1][0] + InV.z * InM.m[2][0];
		Result.y = InV.x * InM.m[0][1] + InV.y * InM.m[1][1] + InV.z * InM.m[2][1];
		Result.z = InV.x * InM.m[0][2] + InV.y * InM.m[1][2] + InV.z * InM.m[2][2];
		return Result;
	}

	class Matrix3x3DX {
		MATRIX33 value;
	public:
		Matrix3x3DX(void) noexcept : value(M33GetIdent()) {}
		Matrix3x3DX(MATRIX33 value) noexcept { this->value = value; }
		MATRIX33 get(void) const noexcept { return this->value; }		// ïœä∑
		MATRIX Get44(void) const noexcept {
			MATRIX Result;
			M33toMATRIX(&Result, this->value);
			return Result;
		}// ïœä∑
		Matrix4x4DX Get44DX(void) const noexcept {
			MATRIX Result;
			M33toMATRIX(&Result, this->value);
			return Result;
		}// ïœä∑
		static Matrix3x3DX Get33DX(const Matrix4x4DX& value) noexcept {
			MATRIX33 Result;
			MATRIXtoM33(&Result, value.get());
			return Result;
		}// ïœä∑
		// 
		static Matrix3x3DX identity(void) noexcept { return M33GetIdent(); }

		static Matrix3x3DX Axis1(const Vector3DX& yvec, const Vector3DX& zvec) noexcept { return { M33GetAxis1(Vector3DX::Cross(yvec, zvec),yvec,zvec) }; }
		static Matrix3x3DX Axis2(const Vector3DX& yvec, const Vector3DX& zvec) noexcept { return { M33GetAxis2(Vector3DX::Cross(yvec, zvec),yvec,zvec) }; }
		static Matrix3x3DX RotAxis(const Vector3DX& p1, float p2) noexcept { return M33GetRotAxis(p1, p2); }
		static Matrix3x3DX RotVec2(const Vector3DX& p1, const Vector3DX& p2) noexcept { return { M33GetRotVec2(p1, p2) }; }
		static Vector3DX Vtrans(const Vector3DX& p1, const Matrix3x3DX& p2) noexcept { return V33Transform(p1, p2.get()); }
		// 
		Matrix3x3DX inverse(void) const noexcept { return M33Inverse(this->get()); }		// ãt
		bool isIdentity(void) const noexcept { return *this == M33GetIdent(); }		// ãt
		// lossyScale
		Matrix3x3DX transpose(void) const noexcept { return M33Transpose(this->get()); }		// ãt

		Vector3DX xvec(void) const noexcept { return Vtrans(Vector3DX::right(), *this); }
		Vector3DX yvec(void) const noexcept { return Vtrans(Vector3DX::up(), *this); }
		Vector3DX zvec(void) const noexcept { return Vtrans(Vector3DX::forward(), *this); }
		// 
		void GetRadian(float* angle_x, float* angle_y, float* angle_z) const noexcept {
			constexpr float threshold = 0.001f;
			if (std::abs(value.m[1][2] - 1.0f) < threshold) { // R(2,1) = sin(x) = 1ÇÃéû
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
			else if (std::abs(value.m[1][2] + 1.0f) < threshold) { // R(2,1) = sin(x) = -1ÇÃéû
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
		// î‰är
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
		// èÊéZ
		Matrix3x3DX operator*(const Matrix3x3DX& obj)  const noexcept { return M33Mult(this->get(), obj.get()); }
		void operator*=(const Matrix3x3DX& obj) noexcept { *this = *this * obj; }
	};
}